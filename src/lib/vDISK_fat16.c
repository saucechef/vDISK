#include <stdlib.h>
#include <string.h>

#include "vDISK_fat16.h"
#include "vDISK_utility.h"

bool fat16_checkDrive(const vDrive *drive) {
    return 0;
}

fatBS *fat16_generateBootSector(const vDrive *drive, uint sectorsPerCluster, string label) {
    if (!isPowerOfTwo(sectorsPerCluster))
        return NULL;
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
    bs->sectors_per_fat = 64; // TODO: CALCULATE
    bs->sectors_per_track = 0x3F;
    bs->number_of_heads = 1;
    bs->hidden_sectors = 0x3F; // TODO: "Same as the relative sector in the partition table...?"
    bs->large_sectors = drive->size_bytes / SECTOR_SIZE > 0xFFFF ? drive->size_bytes / SECTOR_SIZE : 0;
    bs->physical_disk_number = 0x80;
    bs->current_head = 0;
    bs->signature = 0x29;
    bs->volume_serial_number = rand();
    strncpy(bs->volume_label, label, strlen(label) < 10 ? strlen(label) : 10);
    return bs;
}



/*
    byte jump_instruction[3];
    byte oem_identifier[8];
    word bytes_per_sector;
    byte sectors_per_cluster;
    word reserved_sectors;
    byte number_fats;
    word number_root_entries;
    word small_sectors;
    byte media_type;
    word sectors_per_fat;
    word sectors_per_track;
    word number_of_heads;
    uint hidden_sectors;
    uint large_sectors;
    byte physical_disk_number;
    byte current_head;
    byte signature;
    uint volume_serial_number;
    byte volume_label[11];
    byte system_id[8];
 */