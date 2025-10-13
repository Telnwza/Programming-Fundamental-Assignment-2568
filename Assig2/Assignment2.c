#include <stdio.h>
#include <string.h>

// กำหนดจำนวนตัวอักษรสูงสุดใน ASCII table
#define MAX_CHARS 256

void buildBadCharTable(const char *pattern, int patternLength, int badCharTable[MAX_CHARS])
{
    // 1. เริ่มต้นให้ทุกช่องในตารางมีค่าเป็น -1 (หมายถึงไม่พบตัวอักษรนั้นใน pattern)
    for (int i = 0; i < MAX_CHARS; i++)
    {
        badCharTable[i] = -1;
    }

    // 2. วนลูปใน pattern เพื่อเก็บตำแหน่งล่าสุดของแต่ละตัวอักษร
    //    ถ้าเจอตัวอักษรซ้ำ จะใช้ตำแหน่งที่มาทีหลังสุด
    for (int i = 0; i < patternLength; i++)
    {
        badCharTable[(unsigned char)pattern[i]] = i;
    }
}

int searchLineWithBoyerMoore(const char *textLine, const char *pattern, const int badCharTable[MAX_CHARS], int lineNumber)
{
    int textLength = strlen(textLine);
    int patternLength = strlen(pattern);

    // ถ้า pattern หรือ text ว่างเปล่า หรือ text สั้นกว่า pattern ก็ไม่ต้องค้นหา
    if (patternLength == 0 || textLength == 0 || textLength < patternLength)
    {
        return 0;
    }

    int occurrencesInLine = 0;
    int shift = 0; // ตำแหน่งเริ่มต้นของการเปรียบเทียบใน textLine

    // วนลูปเพื่อเลื่อน pattern ไปเรื่อยๆ จนกว่าจะสุดข้อความใน textLine
    while (shift <= (textLength - patternLength))
    {
        int patternIndex = patternLength - 1; // เริ่มเปรียบเทียบจากตัวอักษรสุดท้ายของ pattern

        // เปรียบเทียบ pattern กับส่วนของ textLine แบบย้อนหลังจากขวาไปซ้าย
        while (patternIndex >= 0 && pattern[patternIndex] == textLine[shift + patternIndex])
        {
            patternIndex--;
        }

        // ถ้า patternIndex กลายเป็น -1 หมายความว่าเปรียบเทียบครบทุกตัวและตรงกันทั้งหมด (เจอ pattern)
        if (patternIndex < 0)
        {
            int columnNumber = shift + 1; // ตำแหน่งที่เจอ (เริ่มนับจาก 1)
            printf("Found at Line: %d, Column(Byte): %d\n", lineNumber, columnNumber);
            occurrencesInLine++;

            // คำนวณการเลื่อน (shift) ครั้งต่อไปหลังจากเจอ pattern
            // โดยใช้ "Good Suffix Heuristic" แบบง่าย
            if (shift + patternLength < textLength)
            {
                // เลื่อน pattern ไปตามค่าใน badCharTable ของตัวอักษรใน textLine ที่อยู่ถัดจากตำแหน่งที่เจอ
                shift += patternLength - badCharTable[(unsigned char)textLine[shift + patternLength]];
            }
            else
            {
                // ถ้าเจอตอนท้ายสุดของ textLine แล้ว ก็เลื่อนไป 1 ช่องเพื่อจบการทำงาน
                shift += 1;
            }
        }
        else
        {
            // ถ้าตัวอักษรไม่ตรงกัน (mismatch)
            // คำนวณการเลื่อนโดยใช้ "Bad Character Heuristic"
            char mismatchedChar = textLine[shift + patternIndex];
            int badCharIndex = badCharTable[(unsigned char)mismatchedChar];
            int shiftAmount = patternIndex - badCharIndex;

            // เลื่อน pattern ไปข้างหน้าอย่างน้อย 1 ตำแหน่งเสมอ
            shift += (shiftAmount > 1) ? shiftAmount : 1;
        }
    }
    return occurrencesInLine;
}
void searchFileWithBoyerMoore(char *pattern, char *filePath)
{
    int badCharTable[MAX_CHARS];
    char lineBuffer[2048]; // Buffer สำหรับเก็บข้อมูลทีละบรรทัด
    int lineNumber = 0;
    int totalOccurrences = 0;

    // 1. สร้าง Bad Character Table จาก pattern
    buildBadCharTable(pattern, strlen(pattern), badCharTable);

    // 2. เปิดไฟล์
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filePath);
        return;
    }

    // 3. อ่านไฟล์ทีละบรรทัดแล้วทำการค้นหา
    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL)
    {
        lineNumber++;
        totalOccurrences += searchLineWithBoyerMoore(lineBuffer, pattern, badCharTable, lineNumber);
    }

    // 4. แสดงผลหากไม่พบ pattern เลย
    if (totalOccurrences == 0)
    {
        printf("Pattern '%s' not found in file '%s'.\n", pattern, filePath);
    }

    fclose(file);
}

int main()
{
    char patternToFind[100];
    char fileName[100];

    printf("Enter the pattern to search for: ");
    scanf("%s", patternToFind);

    printf("Enter the file name to search in: ");
    scanf("%s", fileName);

    printf("\n--- Searching... ---\n");
    searchFileWithBoyerMoore(patternToFind, fileName);
    printf("--- Search complete. ---\n");

    return 0;
}
