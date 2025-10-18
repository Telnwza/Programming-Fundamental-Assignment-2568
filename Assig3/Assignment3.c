/*
Assignment 3
ให้นักศึกษาใช้ text file ตัวอย่าง [MAP01.TXT] ซึ่งข้อมูลในไฟล์มีลักษณะเป็นแผนที่เขาวงกต (MAZE)
ให้นักศึกษาเขียนโปรแกรมสร้างเส้นทางการเดินของผู้เล่น จากจุด ‘S’ (Start) ไปยังจุด ‘E’ (End)
บนทางเดินที่เป็นช่องว่าง ‘ ‘ มี ‘#’ เป็นผนังที่ผู้เล่นไม่สามารถเดินผ่านได้ หลังจากนั้นสร้าง text file
ใหม่โดยใช้ชื่อไฟล์เป็นรหัสนักศึกษา ที่มีลักษณะคล้ายไฟล์ตัวอย่างแต่มีการกำหนดเส้นทางการเดินโดยใช้เครื่องหมาย ‘+’

หมายเหตุ: ไฟล์นี้เพิ่ม "คอมเมนต์อธิบายการทำงาน" อย่างละเอียด โดยไม่แก้ตรรกะเดิม
*/

#include <stdio.h>
#include <string.h>

typedef struct
{
  int line, index;
} Point;

int main()
{
  FILE *MAP, *Print;
  MAP = fopen("MAP01.txt", "r");
  Print = fopen("68010403.txt", "w");
  int LINE = 0, index = 0;
  int qStart = 0, qEnd = 0;
  int visited[50][50];
  int Found = 0;
  int len = 0;
  int line = 0;
  Point Start, End;
  Point Pbefore[50][50];
  Point queue[50 * 50];
  char data[64];
  char map[50][50];
  int dline[4] = {-1, 1, 0, 0};
  int dindex[4] = {0, 0, -1, 1};

  while (fgets(data, sizeof(data), MAP) != NULL) //อ่าน map
  {
    len = strlen(data);
    for (index = 0; index < len; index++)
    {
      map[LINE][index] = data[index]; //เก็บ map
      if (data[index] == 's' || data[index] == 'S') //mark ตำแหน่ง s / e
      {
        Start.index = index;
        Start.line = LINE;
      }
      if (data[index] == 'E')
      {
        End.index = index;
        End.line = LINE;
      }
    }
    LINE++;
  }
  line = LINE;

  printf("Si = %d, Sl = %d\nEi = %d, El = %d\nLINE = %d, len = %d\n", Start.index, Start.line, End.index, End.line, line, len);

  LINE = 0;
  index = 0;
  for (LINE = 0; LINE < line; LINE++) // clear virable
  {
    for (index = 0; index < len; index++)
    {
      visited[LINE][index] = 0;
      Pbefore[LINE][index].line = -1;
      Pbefore[LINE][index].index = -1;
    }
  }

  queue[qEnd] = (Point){Start.line, Start.index};
  qEnd++;
  visited[Start.line][Start.index] = 1;
  while (qStart < qEnd)
  {
    Point Bnow = queue[qStart++];
    Point now;
    if (Bnow.line == End.line && Bnow.index == End.index)
    {
      Found = 1;
      break;
    }
    for (int d = 0; d < 4; d++)
    {
      now.line = Bnow.line + dline[d];
      now.index = Bnow.index + dindex[d];

      if (now.line < 0 || now.line >= line || now.index < 0 || now.index >= len)
        continue;
      if (map[now.line][now.index] != '#' && !visited[now.line][now.index])
      {
        if (map[now.line][now.index] == ' ' || map[now.line][now.index] == 'E')
        {
          visited[now.line][now.index] = 1;
          Pbefore[now.line][now.index] = (Point){Bnow.line, Bnow.index};
          queue[qEnd] = (Point){now.line, now.index};
          qEnd++;
        }
      }
    }
  }

  if (!Found)
  {
    printf("No path Found\n");
    fprintf(Print, "No path Found\n");
    fclose(MAP);
    fclose(Print);
    return 0;
  }
  else
  {
    int printline = End.line, printindex = End.index;
    while (!(printline == Start.line && printindex == Start.index))
    {
      Point p = Pbefore[printline][printindex];

      if (!(printline == Start.line && printindex == Start.index) && !(printline == End.line && printindex == End.index))
      {
        if (map[printline][printindex] == ' ')
        {
          map[printline][printindex] = '+';
        }
      }
      printline = p.line;
      printindex = p.index;
    }
  }

  for (int i = 0; i < line; i++)
  {
    for (int j = 0; j < len; j++)
    {
      fprintf(Print, "%c", map[i][j]);
      printf("%c", map[i][j]);
    }
    fprintf(Print, "\n");
    printf("\n");
  }

  fclose(Print);
  fclose(MAP);
  return 0;
}
