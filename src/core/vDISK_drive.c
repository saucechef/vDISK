#include <stdlib.h>

#include "vDISK_drive.h"
#include "vDISK_helpers.h"

vDrive* createDrive(uint size, uint sectorsPerBlock) {
    if (!isPowerOfTwo(sectorsPerBlock))
        return NULL;

    vDrive* drive = (vDrive*) malloc(sizeof(vDrive));
    drive->blocksize = sectorsPerBlock * SECTOR_SIZE;
    drive->size_bytes = (size / drive->blocksize) * drive->blocksize; // Get rid of unnecessary overhead.
    drive->bytes = (byte*) calloc(drive->size_bytes, sizeof(byte));

    return drive;
}

byte readByte(vDrive* drive, uint addr) {
    return drive->bytes[addr];
}

void writeByte(vDrive* drive, uint addr, byte val) {
    if (addr < drive->size_bytes)
        drive->bytes[addr] = val;
}

byte* readArray(vDrive* drive, uint offset, uint n) {
    if (n == 0)
        return NULL;
    if (offset + (n-1) >= drive->size_bytes)
        return NULL;
    byte* array = (byte*) calloc(n, sizeof(byte));
    for (uint i = 0; i < n; i++)
        array[i] = drive->bytes[offset + i];
    return array;
}

void writeArray(vDrive* drive, uint offset, uint n, byte* data) {
    if (n == 0)
        return;
    if (offset + (n-1) >= drive->size_bytes)
        return;
    for (uint i = 0; i < n; i++)
        drive->bytes[offset + i] = data[i];
}

byte* readSector(vDrive* drive, uint sectorID) {
    return readArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE);
}

void writeSector(vDrive* drive, uint sectorID, byte* data) {
    writeArray(drive, sectorID * SECTOR_SIZE, SECTOR_SIZE, data);
}

byte* readBlock(vDrive* drive, uint blockID) {
    return readArray(drive, blockID * drive->blocksize, drive->blocksize);
}

void writeBlock(vDrive* drive, uint blockID, byte* data) {
    writeArray(drive, blockID * drive->blocksize, drive->blocksize, data);
}
