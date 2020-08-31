#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "vDISK_utility.h"

bool isPowerOfTwo(const uint n) {
    if(n==0)
        return false;
    return (ceil(log2(n)) == floor(log2(n)));
}

void printHexdump(const vDrive* drive, uint offset, uint n, bool skipEmptyRows) {

    if (n == 0)
        n = drive->size_bytes - offset;
    if (offset + (n-1) >= drive->size_bytes) {
        printError("HEXDUMP", "Out of range");
        return;
    }

    printf("HEXDUMP from 0x%08X to 0x%08X:\n\n", offset, offset+n-1);
    bool skipping = false;

    for (uint i = offset; i < offset + n; i+=16) {

        if (skipEmptyRows) {
            uint sum = 0;
            for (uint j = 0; j < 16; j++)
                sum += drive->bytes[i + j];
            if (sum == 0 && !skipping)
                printf("...\n");
            if (sum == 0) {
                skipping = true;
                continue;
            }
            else
                skipping = false;
        }

        printf("0x%08X\t", i);
        for (uint j = 0; j < 8; j++)
            printf("%02X  ", drive->bytes[i+j]);
        printf("  ");
        for (uint j = 8; j < 16; j++)
            printf("%02X  ", drive->bytes[i+j]);
        printf("\t|");
        for (uint j = 0; j < 16; j++) {
            if (drive->bytes[i + j] >= 32 && drive->bytes[i + j] <= 126)
                printf("%c", drive->bytes[i + j]);
            else
                printf(".");
        }
        printf("|");
        if (i % drive->cluster_size == 0)
            printf(" C%d,", i / drive->cluster_size);
        if (i % SECTOR_SIZE == 0)
            printf(" S%d", i / SECTOR_SIZE);
        printf("\n");
    }
}

void printError(const string causedBy, const string message) {
    printf("ERROR at %s: %s\n", causedBy, message);
}

uint getFileSize(FILE *file) {
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_END);
    uint length = ftell(file);
    fseek(file, 0, 0);
    return length;
}

void printByteArrayAscii(unsigned char *data, unsigned int n) {
    for (uint i = 0; i < n; i++)
        putchar(data[i]);
}

word getDateNow() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    uint year = tm.tm_year - 80;
    uint month = tm.tm_mon;
    uint day = tm.tm_mday - 1;
    uint date = (year << 9) | (month << 5) | day;
    return date;
}

word getTimeNow() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    uint hour = tm.tm_hour;
    uint minute = tm.tm_min;
    uint second = tm.tm_sec;
    uint time = (hour << 11) | (minute << 5) | second;
    return time;
}

string decodeDate(word date) {
    string str = (string) calloc(11, sizeof(char));
    uint year = ((date >> 9) & 0b1111111) + 1980;
    uint month = ((date >> 5) & 0b1111) + 1;
    uint day = (date & 0b11111) + 1;
    sprintf(str, "%02d.%02d.%d", day, month, year);
    return str;
}

string decodeTime(word time) {
    string str = calloc(6, sizeof(char));
    uint hour = (time >> 11) & 0b11111;
    uint minute = (time >> 5) & 0b11111;
    sprintf(str, "%02d:%02d", hour, minute);
    return str;
}


void toUpperCase(string s) {
    const char OFFSET = 'a' - 'A';
    while (*s)
    {
        *s = (*s >= 'a' && *s <= 'z') ? *s -= OFFSET : *s;
        s++;
    }
}

string getNameFromPath(string path) {
    for (string s = path+strlen(path); s >= path; s--)
        if (*s == '/')
            return s+1;
    return path;
}

string getPathWithoutName(string pathWithFile) {
    uint lengthOfPath = strlen(pathWithFile)-strlen(getNameFromPath(pathWithFile));
    string pathOnly = (string) calloc(lengthOfPath, sizeof(char));
    strncpy(pathOnly, pathWithFile, lengthOfPath);
    return pathOnly;
}

string combinePath(const string part1, const string part2) {
    string new = (string) calloc(strlen(part1)+strlen(part2)+1, sizeof(char));
    strcpy(new, part1);
    strcat(new, part2);
}
