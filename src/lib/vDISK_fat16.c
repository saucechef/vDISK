#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vDISK_fat16.h"
#include "vDISK_utility.h"

fatBS *fat16_generateBootSector(const vDrive *drive, uint sectorsPerCluster, uint sectorsPerFat, string label) {
    if (!isPowerOfTwo(sectorsPerCluster))
        return NULL;
    srand(time(0));
    fatBS* bs = (fatBS*) calloc(1, sizeof(fatBS));
    bs->jump_instruction[0] = 0xEB;
    bs->jump_instruction[1] = 0x3C;
    bs->jump_instruction[2] = 0x90;
    strncpy(bs->oem_identifier, FAT16_OEM_IDENTIFIER, 8);
    bs->bytes_per_sector = SECTOR_SIZE;
    if (isPowerOfTwo(sectorsPerCluster) && sectorsPerCluster <= 128)
        bs->sectors_per_cluster = sectorsPerCluster & 0xFF;
    else
        bs->sectors_per_cluster = 4;
    bs->reserved_clusters = 1;
    bs->number_fats = 2;
    bs->number_root_entries = 512;
    bs->small_sectors = drive->size_bytes / SECTOR_SIZE <= 0xFFFF ? drive->size_bytes / SECTOR_SIZE : 0;
    bs->media_type = 0xF8;
    if (!isPowerOfTwo(sectorsPerFat) || sectorsPerFat < 16 || sectorsPerFat > 256) // Set size constraints.
        bs->sectors_per_fat = 128;
    else
        bs->sectors_per_fat = sectorsPerFat;
    bs->sectors_per_track = 0;
    bs->number_of_heads = 0;
    bs->hidden_sectors = 0;
    bs->large_sectors = drive->size_bytes / SECTOR_SIZE > 0xFFFF ? drive->size_bytes / SECTOR_SIZE : 0;
    bs->physical_disk_number = 0x80;
    bs->current_head = 0;
    bs->signature = 0x29;
    bs->volume_serial_number = rand() << 16 | rand();
    strncpy(bs->volume_label, "           ", 11);
    strncpy(bs->volume_label, label, strlen(label) < 10 ? strlen(label) : 10);
    strncpy(bs->system_id, "FAT16   ", 8);
    byte bootstrap[130] = {0x0E, 0x1F, 0xBE, 0x5B, 0x7C, 0xAC, 0x22, 0xC0, 0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD,
                           0x10, 0x5E, 0xEB, 0xF0, 0x32, 0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE, 0x54, 0x68, 0x69, 0x73, 0x20,
                           0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C,
                           0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x2E, 0x20, 0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20,
                           0x69, 0x6E, 0x73, 0x65, 0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C,
                           0x65, 0x20, 0x66, 0x6C, 0x6F, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6E, 0x64, 0x0D, 0x0A, 0x70, 0x72,
                           0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74,
                           0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6E, 0x20, 0x2E, 0x2E, 0x2E, 0x20, 0x0D, 0x0A, 0x00};
    memcpy(bs->bootstrap_code, bootstrap, 130);
    return bs;
}

fatBS* fat16_readBootSector(const vDrive* drive) {
    fatBS* bs = (fatBS*) calloc(1, sizeof(fatBS));
    readArray(drive, 0x0, 3, bs->jump_instruction);
    readArray(drive, 0x3, 8, bs->oem_identifier);
    bs->bytes_per_sector = readWord(drive, 0xB);
    bs->sectors_per_cluster = readByte(drive, 0xD);
    bs->reserved_clusters = readWord(drive, 0xE);
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
    readArray(drive, 0x3E, 130, bs->bootstrap_code);
    return bs;
}

void fat16_writeBootSector(vDrive* drive, const fatBS* bs) {
    writeArray(drive, 0x0, 3, bs->jump_instruction);
    writeArray(drive, 0x3, 8, bs->oem_identifier);
    writeWord(drive, 0xB, bs->bytes_per_sector);
    writeByte(drive, 0xD, bs->sectors_per_cluster);
    writeWord(drive, 0xE, bs->reserved_clusters);
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
    writeArray(drive, 0x3E, 130, bs->bootstrap_code);
    writeWord(drive, 0x1FE, 0xAA55); // End bootsector
}

bool fat16_checkBootSector(const fatBS* bs) {
    if (!isPowerOfTwo(bs->sectors_per_cluster))
        return false;
    if (strncmp(bs->system_id, "FAT16", 5))
        return false;
    return true;
}

uint fat16_getAddress(const fatBS* bs, uint area) {
    if (area == FAT16_MBR)
        return 0;
    if (area == FAT16_ROOT_DIRECTORY)
        return (bs->reserved_clusters * bs->sectors_per_cluster * SECTOR_SIZE) + (bs->number_fats * bs->sectors_per_fat * SECTOR_SIZE);
    if (area == FAT16_USER_AREA)
        return fat16_getAddress(bs, FAT16_ROOT_DIRECTORY) + bs->number_root_entries / FAT16_DIRECTORY_ENTRY_SIZE;
    if (area % FAT16_FAT == 0)
        return (bs->reserved_clusters * bs->sectors_per_cluster * SECTOR_SIZE) + ((area / FAT16_FAT - 1) * bs->sectors_per_fat * SECTOR_SIZE);
    return -1;
}

fat16* fat16_generateEmptyFat(const fatBS* bs) {
    if (!fat16_checkBootSector(bs))
        return NULL;
    fat16* fat = (fat16*) malloc(sizeof(fat16));
    fat->length_in_bytes = bs->sectors_per_fat * SECTOR_SIZE;
    fat->entries = (word*) calloc(fat->length_in_bytes/2, sizeof(word));
    fat->entries[0x0] = 0xFF00 + bs->media_type;
    fat->entries[0x1] = 0xFFFF;
    return fat;
}

fat16* fat16_readFat(const vDrive* drive, uint number) {
    fat16* fat = (fat16*) malloc(sizeof(fat16));
    fatBS* bs = fat16_readBootSector(drive);
    fat->length_in_bytes = bs->sectors_per_fat * SECTOR_SIZE;
    fat->entries = (word*) calloc(fat->length_in_bytes/2, sizeof(word));
    for (uint j = 0; j < fat->length_in_bytes/2; j++) {
        fat->entries[j] = readWord(drive, fat16_getAddress(bs, FAT16_FAT*number)+(j*sizeof(word)));
    }
    free(bs);
    return fat;
}

void fat16_writeFat(vDrive* drive, const fat16* fat) {
    fatBS* bs = fat16_readBootSector(drive);
    for (uint i = 1; i <= bs->number_fats; i++) {
        for (uint j = 0; j < fat->length_in_bytes/2; j++) {
            writeWord(drive, fat16_getAddress(bs, FAT16_FAT*i)+(j*sizeof(word)), fat->entries[j]);
        }
    }
    free(bs);
}

fat16* fat16_initialiseDrive(vDrive* drive) {
    fatBS* bs = fat16_readBootSector(drive);
    if (!fat16_checkBootSector(bs)) {
        free(bs);
        return NULL;
    }
    drive->clustersize = bs->sectors_per_cluster * SECTOR_SIZE;
    fat16* fat = fat16_readFat(drive, 1);
    free(bs);
    return fat;
}

void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, uint sectorsPerFat, string label) {
    fatBS* bs = fat16_generateBootSector(drive, sectorsPerCluster, sectorsPerFat, label);
    fat16_writeBootSector(drive, bs);
    fat16* fat = fat16_generateEmptyFat(bs);
    fat16_writeFat(drive, fat);
    free(bs);
    free(fat);

    // TODO: IS THERE MORE TO DO?
}

void fat16_makeDir(vDrive* drive, string virtualPath) {
    // TODO: IMPLEMENT
}

uint fat16_writeFile(vDrive* drive, string physicalPath, string virtualPath) {
    // TODO: IMPLEMENT
    return 0;
}

uint fat16_extractFile(const vDrive* drive, string virtualPath, string physicalPath) {
    // TODO: IMPLEMENT
    return 0;
}

uint fat16_remove(vDrive* drive, string virtualPath) {
    // TODO: IMPLEMENT
    return 0;
}

void fat16_ls(const vDrive* drive, string virtualPath) {
    // TODO: IMPLEMENT
}
