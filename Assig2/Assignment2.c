#include <stdio.h>
#include <string.h>
#define maxchar 2048

// สร้างตาราง bad-character heuristic
void badCharHeuristic(const char* str, int size, int badchar[maxchar]) {
    for (int i = 0; i < maxchar; i++)
        badchar[i] = -1;
    for (int i = 0; i < size; i++)
        badchar[(unsigned char)str[i]] = i;
}

int bm_scan_line(const char* line, const char* pattern, const int badCharTable[maxchar], int lineN) {
    int n = strlen(line);
    int m = strlen(pattern);
    if (m == 0 || n == 0 || n < m) return 0;

    int count = 0;
    int shift = 0;

    while (shift <= n - m) {
        int j = m - 1;

        while (j >= 0 && pattern[j] == line[shift + j]) {
            j--;
        }

        if (j < 0) {
            int col = shift + 1; // index เริ่มจาก 1
            printf("line : %d, at col : %d\n", lineN, col);
            count++;

            if (shift + m < n) {
                shift += m - badCharTable[(unsigned char)line[shift + m]];
            } else {
                shift += 1;
            }
        } else {
            int badIndex = badCharTable[(unsigned char)line[shift + j]];
            int move = j - badIndex;
            shift += (move > 1) ? move : 1;
        }
    }
    return count;
}

// ฟังก์ชัน BoyerMoore ใช้ค้นหา text ในไฟล์ data
void BoyerMoore(char *text, char *data) {
    int badCharTable[maxchar];
    FILE *in;
    char ReadLine[maxchar];
    int lineN = 0;
    int count = 0;

    badCharHeuristic(text, strlen(text), badCharTable);
    in = fopen(data, "r");
    if (!in) {
        fprintf(stderr, "Cannot open file: %s\n", data);
        return;
    }

    while (fgets(ReadLine, sizeof(ReadLine), in) != NULL) {
        lineN++;
        count += bm_scan_line(ReadLine, text, badCharTable, lineN);
    }

    if (!count) printf("Not Found %s in %s file\n", text, data);

    fclose(in);
}

int main() {
    char text[100], data[100];
    scanf("%s", text);
    scanf("%s", data);
    BoyerMoore(text, data);
    printf("Done\n");
    return 0;
}