#include <stdio.h>
#include <string.h>

int MAX_CHARS = 256;

void MakeBadCharTable(const char *pattern, int patternLength, int badCharTable[MAX_CHARS])
{
    for (int i = 0; i < MAX_CHARS; i++)
    {
        badCharTable[i] = -1;
    }
    for (int i = 0; i < patternLength; i++)
    {
        badCharTable[pattern[i]] = i;
    }
}

int searchLine(const char *textLine, const char *pattern, const int badCharTable[MAX_CHARS], int lineNumber)
{
    int textLength = strlen(textLine);
    int patternLength = strlen(pattern);

    if (patternLength == 0 || textLength == 0 || textLength < patternLength)
    {
        return 0;
    }

    int Line = 0;
    int shift = 0;

    while (shift <= (textLength - patternLength))
    {
        int patternIndex = patternLength - 1;

        while (patternIndex >= 0 && pattern[patternIndex] == textLine[shift + patternIndex])
        {
            patternIndex--;
        }

        if (patternIndex < 0)
        {
            int col = shift + 1;
            printf("Found at Line: %d, Column(Byte): %d\n", lineNumber, col);
            Line++;

            if (shift + patternLength < textLength)
            {
                shift += patternLength - badCharTable[textLine[shift + patternLength]];
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
            int badCharIndex = badCharTable[mismatchedChar];
            int shiftAmount = patternIndex - badCharIndex;

            // เลื่อน pattern ไปข้างหน้าอย่างน้อย 1 ตำแหน่งเสมอ
            shift += (shiftAmount > 1) ? shiftAmount : 1;
        }
    }
    return Line;
}
void BoyerMoore(char *pattern, char *filePath)
{
    int badCharTable[MAX_CHARS];
    char Buffer[2048];
    int lineNum = 0;
    int times = 0;

    MakeBadCharTable(pattern, strlen(pattern), badCharTable);

    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        printf("Cant open file\n");
        return;
    }

    while (fgets(Buffer, sizeof(Buffer), file) != NULL)
    {
        lineNum++;
        times += searchLine(Buffer, pattern, badCharTable, lineNum);
    }

    if (times == 0)
    {
        printf("Pattern not found\n");
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
    printf("Done\n");

    return 0;
}
