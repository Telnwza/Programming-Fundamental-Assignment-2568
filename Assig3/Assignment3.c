/*
Assignment 3 : Working
ให้นักศึกษาใช้ text file ตัวอย่าง [MAP01.TXT] ซึ่งข้อมูลในไฟล์มีลักษณะเป็นแผนที่เขาวงกต (MAZE)
ให้นักศึกษาเขียนโปรแกรมสร้างเส้นทางการเดินของผู้เล่น จากจุด ‘S’ (Start) ไปยังจุด ‘E’ (End)
บนทางเดินที่เป็นช่องว่าง ‘ ‘ มี ‘#’ เป็นผนังที่ผู้เล่นไม่สามารถเดินผ่านได้ หลังจากนั้นสร้าง text file
ใหม่โดยใช้ชื่อไฟล์เป็นรหัสนักศึกษา ที่มีลักษณะคล้ายไฟล์ตัวอย่างแต่มีการกำหนดเส้นทางการเดินโดยใช้เครื่องหมาย ‘+’

หมายเหตุ: ไฟล์นี้เพิ่ม "คอมเมนต์อธิบายการทำงาน" อย่างละเอียด โดยไม่แก้ตรรกะเดิม
*/

#include <stdio.h>
#include <string.h>

/* โครงสร้างจุดบนกริด — เก็บตำแหน่งเป็น (บรรทัด, คอลัมน์) */
typedef struct {
  int line, index;
} Point;

int main() {
  /* ===== 1) เปิดไฟล์อินพุต/เอาต์พุต ===== */
  FILE *MAP, *Print;
  MAP = fopen("MAP01.txt", "r");            // ไฟล์แผนที่ต้นฉบับ (ต้องมี S,E,# และช่องว่าง)
  Print = fopen("68010403.txt", "w");        // ไฟล์ผลลัพธ์ ใส่เครื่องหมาย '+' บนเส้นทาง

  /*
     หมายเหตุ: โค้ดนี้คงพฤติกรรมเดิม (ไม่มีเช็ค NULL ของไฟล์) ตามที่ผู้เขียนกำหนดไว้
     ถ้าต้องการความปลอดภัยเพิ่ม สามารถเช็ค MAP/Print เป็น NULL ก่อนทำงานได้ในภายหลัง
  */

  /* ===== 2) ประกาศตัวแปรหลักของอัลกอริทึม ===== */
  int LINE = 0, index = 0;         // ตัวเดินแถว/คอลัมน์ระหว่างอ่านไฟล์
  int qStart = 0, qEnd = 0;        // ชี้หัว/ท้ายคิว (BFS queue แบบอาร์เรย์)
  int visited[50][50];             // ตำแหน่งที่เคยเยี่ยมแล้ว (1 = เคย, 0 = ยัง)
  int Found = 0;                   // ธงว่าพบเส้นทางถึง E แล้วหรือไม่
  int len = 0;                     // หาความกว้างของแผนที่
  int line = 0;                    // หาความสูงแผนที่
  Point Start, End;                // ตำแหน่งเริ่ม(S) และสิ้นสุด(E)
  Point Pbefore[50][50];           // parent ของแต่ละเซลล์: ใช้ย้อนเส้นทางจาก E -> S
  Point queue[50*50];                // คิวเก็บจุดที่จะขยาย (ความจุ 20*20)
  char data[64];                   // บัฟเฟอร์อ่าน 1 บรรทัดจากไฟล์
  char map[50][50];                // กริดตัวอักษรเก็บแผนที่ (เผื่อขนาดมากกว่าใช้งานจริงเล็กน้อย)

  /* เวกเตอร์ทิศทางสำหรับเพื่อนบ้าน 4 ทิศ: ขึ้น, ลง, ซ้าย, ขวา */
  int dline[4]  = {-1, 1, 0, 0};
  int dindex[4] = { 0, 0,-1, 1};

  /* ===== 3) อ่านแผนที่จากไฟล์ บรรทัดต่อบรรทัด ===== */
  while (fgets(data, sizeof(data), MAP) != NULL) {
    len = strlen(data);
    /* คัดลอกตัวอักษรของบรรทัดลงในแถว LINE ของ map และค้นหา S/E ไปด้วย */
    for (index = 0; index < len; index++) {
      map[LINE][index] = data[index];

      if (data[index] == 's' || data[index] == 'S') {   // รองรับ s และ S
        Start.index = index;
        Start.line  = LINE;
      }
      if (data[index] == 'E') {                         // รองรับ E (และ e ตอนเดิน)
        End.index = index;
        End.line  = LINE;
      }
    }
    LINE++;  // ไปแถวถัดไป
  }
  line = LINE;
  /* แสดงตำแหน่ง S/E เพื่อดีบักบนหน้าจอ */
  printf("Si = %d, Sl = %d\nEi = %d, El = %d\nLINE = %d, len = %d\n", Start.index, Start.line, End.index, End.line, line, len);

  /* ===== 4) เตรียมโครงสร้างข้อมูล BFS =====  */
  LINE = 0; index = 0;  // รีเซ็ตตัวเดิน

  for (LINE = 0; LINE < line; LINE++) {
    for (index = 0; index < len; index++) {
      visited[LINE][index] = 0;          // ยังไม่เคยเยี่ยม
      Pbefore[LINE][index].line  = -1;   // parent ยังไม่มี
      Pbefore[LINE][index].index = -1;
    }
  }

  /* ===== 5) ใส่จุดเริ่มต้นลงคิว และ mark ว่าเยี่ยมแล้ว ===== */
  queue[qEnd++] = (Point){ Start.line, Start.index };  // enqueue S
  visited[Start.line][Start.index] = 1;                // mark visited(S)

  /* ===== 6) วนลูป BFS จนกว่าคิวจะว่างหรือพบ E ===== */
  while (qStart < qEnd) {
    /* 6.1) ดึงหัวคิวออกมาเป็นจุดปัจจุบัน */
    Point Bnow = queue[qStart++];
    Point now;  // ตัวแปรใช้คำนวณเพื่อนบ้าน

    /* 6.2) ถ้าเจอ E แล้วก็จบการค้นหา */
    if (Bnow.line == End.line && Bnow.index == End.index) {
      Found = 1;
      break;
    }

    /* 6.3) ลองขยายไปเพื่อนบ้าน 4 ทิศ */
    for (int d = 0; d < 4; d++) {
      now.line  = Bnow.line  + dline[d];
      now.index = Bnow.index + dindex[d];

      /* 6.3.1) เช็คไม่ให้ออกนอกขอบตาราง 19x20 */
      if (now.line < 0 || now.line >= line || now.index < 0 || now.index >= len)
        continue;

      /* 6.3.2) เงื่อนไขการเดินผ่าน: ไม่ใช่กำแพง และยังไม่เคยเยี่ยม */
      if (map[now.line][now.index] != '#' && !visited[now.line][now.index]) {
        /* เดินได้เฉพาะช่องว่าง ' ' และจุดหมาย 'E' หรือ 'e' */
        if (map[now.line][now.index] == ' ' || map[now.line][now.index] == 'E') {
          visited[now.line][now.index] = 1;                   // mark เยี่ยมแล้ว
          Pbefore[now.line][now.index] = (Point){ Bnow.line, Bnow.index }; // บันทึก parent
          queue[qEnd++] = (Point){ now.line, now.index };      // enqueue เพื่อนบ้านนี้
        }
      }
    }
  }
  
  /* ===== 7) ถ้าไม่พบเส้นทาง ให้ปิดไฟล์และจบ ===== */
  if (!Found) {
    printf("No path Found\n");
    fprintf(Print,"No path Found\n");
    fclose(MAP);
    fclose(Print);
    return 0;
  } else {
    /* ===== 8) พบเส้นทางแล้ว: ย้อนรอยจาก E -> S แล้ววาง '+' ===== */
    int printline = End.line, printindex = End.index;
    while (!(printline == Start.line && printindex == Start.index)) {
      Point p = Pbefore[printline][printindex];  // parent ก่อนหน้า

      /* วาง '+' เฉพาะช่องว่างระหว่างทาง (ไม่ทับ S/E) */
      if (!(printline == Start.line && printindex == Start.index) &&
          !(printline == End.line   && printindex == End.index)) {
        if (map[printline][printindex] == ' ') {
          map[printline][printindex] = '+';
        }
      }
      printline = p.line;
      printindex = p.index;
    }
  }

  /* ===== 9) เขียนผลลัพธ์ลงไฟล์เอาต์พุต และพิมพ์หน้าจอ ===== */
  for (int i = 0; i < line; i++) {
    for (int j = 0; j < len; j++) {
      fprintf(Print, "%c", map[i][j]); // เขียนตัวอักษรลงไฟล์ผลลัพธ์
      printf("%c", map[i][j]);         // แสดงบนหน้าจอสำหรับตรวจสอบ
    }
    fprintf(Print, "\n");
    printf("\n");
  }

  /* ===== 10) ปิดไฟล์และจบโปรแกรม ===== */
  fclose(Print);
  fclose(MAP);
  return 0;
}