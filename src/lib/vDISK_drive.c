#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vDISK_drive.h"
#include "vDISK_utility.h"

vDrive* createDrive(const uint size, const uint sectorsPerCluster) {
    if (!isPowerOfTwo(sectorsPerCluster)) {
        printError("CREATEDRIVE", "sectorsPerCluster not power of 2.");
        return NULL;
    }
    vDrive* drive = (vDrive*) malloc(sizeof(vDrive));
    drive->clustersize = sectorsPerCluster * SECTOR_SIZE;
    drive->size_bytes = (size / drive->clustersize) * drive->clustersize; // Get rid of unusable overhead.
    drive->bytes = (byte*) calloc(drive->size_bytes, sizeof(byte));

    return drive;
}

void saveDrive(const vDrive* drive, const string path) {
    char filepath[strlen(path)];
    strcpy(filepath, path);
    strcat(filepath, ".img");
    printf("path: %s\n", filepath);
    FILE* file;
    if ((file = fopen(filepath, "w+")) != NULL) {
        fwrite(drive->bytes, sizeof(byte), drive->size_bytes, file);
        fclose(file);
    } else {
        fclose(file);
        printError("SAVEDRIVE", "Could not open file. Drive image was not saved.");
    }
}

vDrive* loadDrive(const string path) {
    char filepath[strlen(path)];
    strcpy(filepath, path);
    //strcat(filepath, ".img"); // Append file extension
    FILE* file;
    if ((file = fopen(filepath, "r")) != NULL) {
        uint size = getFileSize(file);
        vDrive* drive = createDrive(size, 1);
        fread(drive->bytes, sizeof(byte), drive->size_bytes, file);
        fclose(file);
        return drive;
    }
    fclose(file);
    printError("LOADDRIVE", "Could not open file. No drive image was loaded.");
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

word readWord(const vDrive* drive, const uint addr) {
    return ((word) readByte(drive, addr)) + ((word) readByte(drive, addr+1) << 8);
}

void writeWord(vDrive* drive, const uint addr, const word val) {
    writeByte(drive, addr, val & 0xFF);
    writeByte(drive, addr+1, val >> 8);
}

uint readDWord(const vDrive* drive, const uint addr) {
    uint sum = 0;
    sum += readByte(drive, addr);
    sum += readByte(drive, addr+1) << 8;
    sum += readByte(drive, addr+2) << 16;
    sum += readByte(drive, addr+3) << 24;
    return sum;
}

void writeDWord(vDrive* drive, const uint addr, const uint val) {
    writeByte(drive, addr, val & 0xFF);
    writeByte(drive, addr+1, (val >> 8) & 0xFF);
    writeByte(drive, addr+2, (val >> 16) & 0xFF);
    writeByte(drive, addr+3, (val >> 24) & 0xFF);
}

void readArray(const vDrive* drive, const uint offset, const uint n, byte* dest) {
    if (n == 0) {
        printError("READARRAY", "n was 0.");
        return;
    }
    if (offset + (n-1) >= drive->size_bytes) {
        printError("READARRAY", "Out of range.");
        return;
    }
    for (uint i = 0; i < n; i++)
        dest[i] = drive->bytes[offset + i];
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

void readSector(const vDrive* drive, const uint sectorID, byte* dest) {
    readArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE, dest);
}

void writeSector(vDrive* drive, const uint sectorID, const byte* data) {
    writeArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE, data);
}

void readCluster(const vDrive* drive, const uint clusterID, byte* dest) {
    readArray(drive, clusterID * drive->clustersize, drive->clustersize, dest);
}

void writeCluster(vDrive* drive, const uint clusterID, const byte* data) {
    writeArray(drive, clusterID * drive->clustersize, drive->clustersize, data);
}
