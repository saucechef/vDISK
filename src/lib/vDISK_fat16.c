#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vDISK_fat16.h"
#include "vDISK_utility.h"

fatBS *fat16_generateBootSector(const vDrive *drive, uint sectorsPerCluster, string label) {
    if (!isPowerOfTwo(sectorsPerCluster))
        return NULL;
    srand(time(0));
    fatBS* bs = (fatBS*) calloc(1, sizeof(fatBS));
    bs->jump_instruction[0] = 0xEB;
    bs->jump_instruction[1] = 0x3C;
    bs->jump_instruction[2] = 0x90;
    strncpy(bs->oem_identifier, "vDISKfat", 8);
    bs->bytes_per_sector = SECTOR_SIZE;
    bs->sectors_per_cluster = sectorsPerCluster & 0xFF;
    bs->reserved_sectors = 1;
    bs->number_fats = 2;
    bs->number_root_entries = 512;
    bs->small_sectors = drive->size_bytes / SECTOR_SIZE <= 0xFFFF ? drive->size_bytes / SECTOR_SIZE : 0;
    bs->media_type = 0xF8;
    bs->sectors_per_fat = 128; // TODO: CALCULATE
    bs->sectors_per_track = 0x3F;
    bs->number_of_heads = 255;
    bs->hidden_sectors = 0x3F; // TODO: "Same as the relative sector in the partition table...?"
    bs->large_sectors = drive->size_bytes / SECTOR_SIZE > 0xFFFF ? drive->size_bytes / SECTOR_SIZE : 0;
    bs->physical_disk_number = 0x80;
    bs->current_head = 0;
    bs->signature = 0x29;
    bs->volume_serial_number = rand() << 16 | rand();
    strncpy(bs->volume_label, "           ", 11);
    strncpy(bs->volume_label, label, strlen(label) < 10 ? strlen(label) : 10);
    strncpy(bs->system_id, "FAT16   ", 8);
    return bs;
}

fatBS* fat16_readBootSector(const vDrive* drive) {
    fatBS* bs = (fatBS*) calloc(1, sizeof(fatBS));
    readArray(drive, 0x0, 3, bs->jump_instruction);
    readArray(drive, 0x3, 8, bs->oem_identifier);
    bs->bytes_per_sector = readWord(drive, 0xB);
    bs->sectors_per_cluster = readByte(drive, 0xD);
    bs->reserved_sectors = readWord(drive, 0xE);
    bs->number_fats = readByte(drive, 0x10);
    bs->number_root_entries = readWord(drive, 0x11);
    bs->small_sectors = readWord(drive, 0x13);
    bs->media_type = readByte(drive, 0x15);
    bs->sectors_per_fat = readWord(drive, 0x16);
    bs->sectors_per_track = readWord(drive, 0x18);
    bs->number_of_heads = readWord(drive, 0x1A);
    bs->hidden_sectors = readDWord(drive, 0x1C);
    bs->large_sectors = readDWord(drive, 0x20);
    bs->physical_disk_number = readByte(drive, 0x24);
    bs->current_head = readByte(drive, 0x25);
    bs->signature = readByte(drive, 0x26);
    bs->volume_serial_number = readDWord(drive, 0x27);
    readArray(drive, 0x2B, 11, bs->volume_label);
    readArray(drive, 0x36, 8, bs->system_id);
    return bs;
}

void fat16_writeBootSector(vDrive* drive, const fatBS* bs) {
    writeArray(drive, 0x0, 3, bs->jump_instruction);
    writeArray(drive, 0x3, 8, bs->oem_identifier);
    writeWord(drive, 0xB, bs->bytes_per_sector);
    writeByte(drive, 0xD, bs->sectors_per_cluster);
    writeWord(drive, 0xE, bs->reserved_sectors);
    writeByte(drive, 0x10, bs->number_fats);
    writeWord(drive, 0x11, bs->number_root_entries);
    writeWord(drive, 0x13, bs->small_sectors);
    writeByte(drive, 0x15, bs->media_type);
    writeWord(drive, 0x16, bs->sectors_per_fat);
    writeWord(drive, 0x18, bs->sectors_per_track);
    writeWord(drive, 0x1A, bs->number_of_heads);
    writeDWord(drive, 0x1C, bs->hidden_sectors);
    writeDWord(drive, 0x20, bs->large_sectors);
    writeByte(drive, 0x24, bs->physical_disk_number);
    writeByte(drive, 0x25, bs->current_head);
    writeByte(drive, 0x26, bs->signature);
    writeDWord(drive, 0x27, bs->volume_serial_number);
    writeArray(drive, 0x2B, 11, bs->volume_label);
    writeArray(drive, 0x36, 8, bs->system_id);
    // Insert x86 Bootstrap code for non-bootable disk (Stolen from Linux/mkfs).
    byte bootstrap[130] = {0x0E, 0x1F, 0xBE, 0x5B, 0x7C, 0xAC, 0x22, 0xC0, 0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD,
                           0x10, 0x5E, 0xEB, 0xF0, 0x32, 0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE, 0x54, 0x68, 0x69, 0x73, 0x20,
                           0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C,
                           0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x2E, 0x20, 0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20,
                           0x69, 0x6E, 0x73, 0x65, 0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C,
                           0x65, 0x20, 0x66, 0x6C, 0x6F, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6E, 0x64, 0x0D, 0x0A, 0x70, 0x72,
                           0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74,
                           0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6E, 0x20, 0x2E, 0x2E, 0x2E, 0x20, 0x0D, 0x0A, 0x00};
    writeArray(drive, 0x3E, 130, bootstrap);
    writeWord(drive, 0x1FE, 0xAA55);
}

bool fat16_checkDrive(const vDrive* drive) {
    fatBS* bs = fat16_readBootSector(drive);
    if (!isPowerOfTwo(bs->sectors_per_cluster))
        return false;
    if (strncmp(bs->system_id, "FAT16", 5))
        return false;
    // TODO: EXTEND
    free(bs);
    return true;
}

fat16* fat16_initialiseDrive(const vDrive* drive) {
    // TODO: IMPLEMENT AFTER readFat AND writeFat ARE FINISHED

    return NULL;
}

void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, string label) {
    fat16_writeBootSector(drive, fat16_generateBootSector(drive, sectorsPerCluster, label));
    // TODO: EXTEND
}
