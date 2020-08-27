#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vDISK_drive.h"
#include "vDISK_utility.h"

vDrive* createDrive(const uint size, const uint sectorsPerBlock) {
    if (!isPowerOfTwo(sectorsPerBlock)) {
        printError("CREATEDRIVE", "sectorsPerBlock not power of 2.");
        return NULL;
    }
    vDrive* drive = (vDrive*) malloc(sizeof(vDrive));
    drive->blocksize = sectorsPerBlock * SECTOR_SIZE;
    drive->size_bytes = (size / drive->blocksize) * drive->blocksize; // Get rid of unusable overhead.
    drive->bytes = (byte*) calloc(drive->size_bytes, sizeof(byte));

    return drive;
}

void saveDrive(const vDrive* drive, const string path) {
    char filepath[strlen(path)];
    strcpy(filepath, path);
    strcat(filepath, ".vdisk");
    printf("path: %s\n", filepath);
    FILE* file;
    if ((file = fopen(filepath, "w+")) != NULL) {
        fwrite(drive->bytes, sizeof(byte), drive->size_bytes, file);
        fclose(file);
    } else {
        fclose(file);
        printError("SAVEDRIVE", "Could not open file. Drive was not saved.");
    }
}

vDrive* loadDrive(const string path) {
    char filepath[strlen(path)];
    strcpy(filepath, path);
    strcat(filepath, ".vdisk");
    FILE* file;
    if ((file = fopen(filepath, "r")) != NULL) {
        uint size = getFileSize(file);
        vDrive* drive = createDrive(size, 1); // SET REAL BLOCK SIZE AFTER READING IT FROM PARTITION
        fread(drive->bytes, sizeof(byte), drive->size_bytes, file);
        fclose(file);
        return drive;
    }
    fclose(file);
    printError("LOADDRIVE", "Could not open file. No drive was loaded.");
    return NULL;
}

void deleteDrive(vDrive *drive) {
    free(drive->bytes);
    free(drive);
}

byte readByte(const vDrive* drive, const  uint addr) {
    return drive->bytes[addr];
}

void writeByte(vDrive* drive, const uint addr, const byte val) {
    if (addr < drive->size_bytes)
        drive->bytes[addr] = val;
}

byte* readArray(const vDrive* drive, const uint offset, const uint n) {
    if (n == 0) {
        printError("READARRAY", "n was 0.");
        return NULL;
    }
    if (offset + (n-1) >= drive->size_bytes) {
        printError("READARRAY", "Out of range.");
        return NULL;
    }
    byte* array = (byte*) calloc(n, sizeof(byte));
    for (uint i = 0; i < n; i++)
        array[i] = drive->bytes[offset + i];
    return array;
}

void writeArray(vDrive* drive, const uint offset, const uint n, const byte* data) {
    if (n == 0) {
        printError("READARRAY", "n was 0.");
        return;
    }
    if (offset + (n-1) >= drive->size_bytes) {
        printError("READARRAY", "Out of range.");
        return;
    }
    for (uint i = 0; i < n; i++)
        drive->bytes[offset + i] = data[i];
}

byte* readSector(const vDrive* drive, const uint sectorID) {
    return readArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE);
}

void writeSector(vDrive* drive, const uint sectorID, const byte* data) {
    writeArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE, data);
}

byte* readBlock(const vDrive* drive, const uint blockID) {
    return readArray(drive, blockID * drive->blocksize, drive->blocksize);
}

void writeBlock(vDrive* drive, const uint blockID, const byte* data) {
    writeArray(drive, blockID * drive->blocksize, drive->blocksize, data);
}
