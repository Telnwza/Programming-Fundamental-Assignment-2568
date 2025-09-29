/*
Assignment 2 : Done
ให้นักศึกษาใช้ text file ตัวอย่าง [boyer-moore.txt] เป็นชุดข้อมูลสำหรับการค้นหา 
แล้วให้นักศึกษาสร้างฟังก์ชั่น Boyer-Moore(text,data) ซึ่งเป็นฟังก์ชั่นที่ใช้ในการค้นหา text ใน data 
และให้นักศึกษาเขียนโปรแกรมรับ keyword ที่ใช้ในการค้นหา แล้วเรียกฟังก์ชั่น Boyer-Moore ที่สร้างขึ้น แล้วแสดงผลตำแหน่งที่พบ 
keyword ทัั้งหมดใน data 
*/

#include <stdio.h>
#include <string.h>

// ฟังก์ชัน BoyerMoore ใช้ค้นหาคำ (text) ในไฟล์ (data)
void BoyerMoore(char *text, char *data)
{
  FILE *in;                // ตัวชี้ไฟล์ที่จะเปิดอ่าน
  char ReadLine[255];      // buffer เก็บข้อความทีละบรรทัด (สูงสุด 254 ตัวอักษร + null terminator)
  int lineN = 0;           // นับเลขบรรทัด (เริ่มจาก 0)
  int count = 0;

  in = fopen(data, "r");   // เปิดไฟล์ในโหมดอ่าน

  // อ่านไฟล์ทีละบรรทัดจนกว่าจะหมด
  while (fgets(ReadLine, sizeof(ReadLine), in) != NULL)
  {
    lineN++;               // เพิ่มตัวนับบรรทัด
    char *pos = ReadLine;  // ตัวชี้ตำแหน่งที่ใช้ค้นหาในบรรทัดปัจจุบัน
    int index;             // เก็บตำแหน่งที่เจอข้อความ (เริ่มนับจาก 1)

    // ค้นหาข้อความ text ในบรรทัดเดียวกัน
    while ((pos = strstr(pos, text)) != NULL)
    {
      index = (pos - ReadLine) + 1; // คำนวณตำแหน่งที่เจอ (pos - ต้นบรรทัด + 1)
      printf("line : %d, pos : %d\n", lineN, index);
      count++;
      pos += strlen(text); // ขยับ pointer ไปหลังข้อความที่เจอ เพื่อหาคำต่อไป
    }
  }

  if(!count) printf("Not Found %s in %s file\n", text, data);

  fclose(in);              // ปิดไฟล์หลังจากอ่านเสร็จ
}

int main()
{
  char text[100], data[100];  // text = คำที่จะค้นหา, data = ชื่อไฟล์ที่จะค้นหา
  
  printf("KeyWord : ");
  scanf("%s", text);          // รับคำค้นหาจากผู้ใช้
  printf("File(path) : ");
  scanf("%s", data);          // รับชื่อไฟล์จากผู้ใช้

  BoyerMoore(text, data);     // เรียกใช้ฟังก์ชันค้นหา
  printf("Done\n");           // แสดงข้อความว่าเสร็จสิ้น
}