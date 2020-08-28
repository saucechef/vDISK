#include <math.h>
#include <stdio.h>

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
                printf("*\n");
            if (sum == 0) {
                skipping = true;
                continue;
            }
            else
                skipping = false;
        }

        printf("0x%08X\t", i);
        for (uint j = 0; j < 8; j++)
            printf("%02X  ", drive->bytes[i+j] & 0xFF);
        printf("  ");
        for (uint j = 8; j < 16; j++)
            printf("%02X  ", drive->bytes[i+j] & 0xFF);
        printf("\t|");
        for (uint j = 0; j < 16; j++) {
            if (drive->bytes[i + j] >= 32 && drive->bytes[i + j] <= 126)
                printf("%c", drive->bytes[i + j]);
            else
                printf(".");
        }
        printf("|");
        if (i % drive->blocksize == 0)
            printf(" B%d,", i / drive->blocksize);
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
