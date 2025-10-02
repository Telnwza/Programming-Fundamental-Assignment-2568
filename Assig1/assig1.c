#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

/* ====== โครงสร้างเก็บข้อมูลที่ curl โหลดมา ====== */
struct Memory
{
  char *data;   // buffer สำหรับเก็บข้อมูล JSON ที่โหลดจาก URL
  size_t size;  // ขนาดข้อมูลที่โหลดมา
};

/* callback สำหรับให้ curl เขียนข้อมูลลง buffer */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t total = size * nmemb;                      // คำนวณขนาดข้อมูลจริง
  struct Memory *mem = (struct Memory *)userp;      // cast เป็น struct Memory

  // realloc buffer ให้ใหญ่พอเก็บข้อมูลใหม่
  char *ptr = realloc(mem->data, mem->size + total + 1);
  if (!ptr) return 0;                               // ถ้า realloc fail → return 0

  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), contents, total); // คัดลอกข้อมูลใหม่ต่อท้าย
  mem->size += total;
  mem->data[mem->size] = '\0';                      // ปิดท้ายด้วย '\0' ให้เป็น string
  return total;
}

typedef struct
{
  int no;
  float lat;
  float lon;
  char *place;
} location;

int main()
{
  CURL *curl = curl_easy_init();
  if (!curl) return 1;

  struct Memory chunk = {0};

  char url[256] = "https://nominatim.openstreetmap.org/reverse?format=json&lat=";
  char slat[32] = "";
  char sc[6] = "&lon=";
  char slon[32] = "";

  FILE *in, *out;
  in = fopen("latlonfile.csv", "r");
  out = fopen("68010403.csv", "w");
  if (!in) {
    printf("Cannot open file\n");
    return 1;
  }

  location lo[100];
  int index = 0;

  //chear header
  char header[100];
  fgets(header, sizeof(header), in);

  while (fscanf(in, "%d,%f,%f%*[^\n]%*c", &lo[index].no, &lo[index].lat, &lo[index].lon) == 3)
  {
    lo[index].place = "NaN";

    if (chunk.data) {
      free(chunk.data);
      chunk.data = NULL;
      chunk.size = 0;
    }

    strcpy(url, "https://nominatim.openstreetmap.org/reverse?format=json&lat=");
    sprintf(slat, "%.6f", lo[index].lat);
    sprintf(slon, "%.6f", lo[index].lon);
    strcat(url, slat);
    strcat(url, sc);
    strcat(url, slon);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "simple-c-fetch/1.0");

    if (curl_easy_perform(curl) == CURLE_OK && chunk.data)
    {
      // หา "display_name"
      char *p = strstr(chunk.data, "\"display_name\"");
      if (p)
      {
        p = strchr(p + 14, '\"'); 
        if (p)
        {
          p++;
          char *q = strchr(p, '\"');
          if (q) {
              *q = '\0'; 
              char *clean = malloc(strlen(p) + 1);
              char *dst = clean;
              for (char *c = p; *c; c++) {
                  if (*c != ',') {
                      *dst++ = *c;
                  }
              }
              *dst = '\0';
              printf("display_name: %s\n", clean);
              lo[index].place = clean;
          }
        }
      }
      else {
        printf("📍 display_name: NaN\n");
        lo[index].place = "NaN";
      }
    }

    index++;
  }

  fprintf(out, "no,lat,lon,place\n");
  for (int i = 0; i < index; i++)
  {
    //printf("%d: %.6f, %.6f, %s\n", lo[i].no, lo[i].lat, lo[i].lon, lo[i].place);
    fprintf(out, "%d,%.6f,%.6f,%s,\n", lo[i].no, lo[i].lat, lo[i].lon, lo[i].place);
  }
  
  fclose(in);
  fclose(out);
  free(chunk.data);
  curl_easy_cleanup(curl);

  return 0;
}