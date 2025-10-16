#include <stdio.h>
#include <string.h>
#include <ctype.h>

int MAX_CHARS = 2048;
int total = 0;

void MakeBadCharTable(const char *pattern, int patternLength, int badCharTable[MAX_CHARS])
{
    for (int i = 0; i < MAX_CHARS; i++) // clearค่า
    {
        badCharTable[i] = -1;
    }
    for (int i = 0; i < patternLength; i++)
    {
        badCharTable[pattern[i]] = i; // ใส่เรียงเลขในarrayของตัวอักษรของpattern เป็น i ไล่จากหน้าไปลัง
    }
}

int searchLine(char *textLine, const char *pattern, const int badCharTable[MAX_CHARS], int lineNum) // search ใน line ที่ read มาจากไฟล์
{
    int textLength = strlen(textLine);
    int patternLength = strlen(pattern);
    for(int i = 0; i<textLength-1; i++){ //ดักตัวไหนไม่อยู่ใน ascii ให้เป็น spacebar 
        if (!(textLine[i] >= 1) || !(textLine[i] <= 127))
            {
                textLine[i] = 32;
            }
    }

    if (patternLength == 0 || textLength == 0 || textLength < patternLength)
    {
        return 0;
    }

    int times = 0;
    int shift = 0;

    while (shift <= (textLength - patternLength))
    {
        int patternIndex = patternLength - 1;

        while (patternIndex >= 0 && pattern[patternIndex] == textLine[shift + patternIndex])
        {
            patternIndex--; // ไล่เช็คจากขวาไปซ้าย
        }

        if (patternIndex < 0) // match ทุกตัว เจอครบ
        {
            int col = shift + 1;
            printf("Found at Line: %d, Column(Byte): %d\n", lineNum, col);
            times++;
            total++;

            if (shift + patternLength < textLength) // ยังไม่หมด
            {
                shift += patternLength - badCharTable[textLine[shift + patternLength]];
            }
            else // หมดแล้ว
            {
                shift += 1;
            }
        }
        else // ไม่เจอ
        {
            // คำนวนshift ตามตาราง เช็คว่าตัวที่ไม่ตรงมีใน pattern ไหม อยู่ที่ index เท่าไหร่(เรียงกลับกัน) แล้วเลื่อนไปตามนั้น ขั้นต่ำ = 1
            char mismatchedChar = textLine[shift + patternIndex];
            int badCharIndex = badCharTable[mismatchedChar];
            int shiftAmount = patternIndex - badCharIndex;

            shift += (shiftAmount > 1) ? shiftAmount : 1;
        }
    }
    return times;
}
void BoyerMoore(char *pattern, char *filePath) // อ่านไฟล์เเล้วเรียกใช้ฟังก์ชั่นหาคำใน line นั้นๆ
{
    int badCharTable[MAX_CHARS];
    char Buffer[MAX_CHARS];
    int lineNum = 0;
    int times = 0;

    MakeBadCharTable(pattern, strlen(pattern), badCharTable);

    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        printf("Can't open file\n");
        return;
    }

    while (fgets(Buffer, sizeof(Buffer), file) != NULL)
    {
        lineNum++;
        times += searchLine(Buffer, pattern, badCharTable, lineNum);
    }

    if (times == 0)
    {
        printf("No Pattern found\n");
    }

    fclose(file);
}

int main()
{
    char patternToFind[100];
    char fileName[100];

    printf("Pattern: ");
    scanf("%s", patternToFind);

    printf("File: ");
    scanf("%s", fileName);

    BoyerMoore(patternToFind, fileName);
    printf("Total = %d \nDone\n",total);

    return 0;
}
