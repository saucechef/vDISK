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
        return fat16_getAddress(bs, FAT16_ROOT_DIRECTORY) + bs->number_root_entries * FAT16_DIRECTORY_ENTRY_SIZE;
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
    for (uint j = 0; j < fat->length_in_bytes/sizeof(word); j++) {
        fat->entries[j] = readWord(drive, fat16_getAddress(bs, FAT16_FAT*number)+(j*sizeof(word)));
    }
    free(bs);
    return fat;
}

void fat16_writeFat(vDrive* drive, const fat16* fat) {
    fatBS* bs = fat16_readBootSector(drive);
    for (uint i = 1; i <= bs->number_fats; i++) {
        for (uint j = 0; j < fat->length_in_bytes/sizeof(word); j++) {
            writeWord(drive, fat16_getAddress(bs, FAT16_FAT*i)+(j*sizeof(word)), fat->entries[j]);
        }
    }
    free(bs);
}

uint fat16_getClustersWithState(const fat16* fat, uint state) {
    uint sum = 0;
    for (uint i = 0; i < fat->length_in_bytes/2; i++)
        if (fat->entries[i] == state)
            sum++;
    return sum;
}

word fat16_getNextFreeCluster(const fat16* fat) {
    for (uint i = 0; i < fat->length_in_bytes/sizeof(word); i++) {
        if (fat->entries[i] == FAT16_CLUSTER_FREE)
            return i;
    }
    return 0;
}

folderEntry* fat16_generateFolderEntry(string fileName, byte attributes, word firstCluster, uint sizeOfData) {
    folderEntry* entry = (folderEntry*) calloc(1, sizeof(folderEntry));
    uint posOfSeparator = strlen(fileName);
    toUpperCase(fileName);
    for (uint i = 0; i < strlen(fileName); i++) {
        if (fileName[i] == '.')
            posOfSeparator = i;
    }
    strncpy(entry->name, fileName, posOfSeparator < 8 ? posOfSeparator : 8);
    memcpy(entry->extension, &fileName[posOfSeparator+1], 3);
    if (strlen(entry->name) < 8)
        for (uint i = strlen(entry->name); i < 8; i++)
            entry->name[i] = ' ';
    if (strlen(entry->extension) < 3)
        for (uint i = strlen(entry->extension); i < 3; i++)
            entry->extension[i] = ' ';
    entry->attributes = attributes;
    entry->create_time = getTimeNow();
    entry->create_date = getDateNow();
    entry->last_access_date = getDateNow();
    entry->last_modified_time = getTimeNow();
    entry->last_modified_date = getDateNow();
    entry->first_cluster = firstCluster;
    entry->size_of_data = sizeOfData;
    return entry;
}

folderEntry* fat16_readFolderEntry(const vDrive* drive, uint folderStartAddress, uint offset) {
    uint entryAddress = folderStartAddress + offset * FAT16_DIRECTORY_ENTRY_SIZE;
    byte marker = readByte(drive, entryAddress);
    if (marker == FAT16_ENTRY_LAST || marker == FAT16_ENTRY_DOT || marker == FAT16_ENTRY_FREE)
        return NULL;
    folderEntry* entry = (folderEntry*) calloc(1, sizeof(folderEntry));
    readArray(drive, entryAddress, 8, entry->name);
    readArray(drive, entryAddress + 0x8, 3, entry->extension);
    entry->attributes = readByte(drive, entryAddress + 0xB);
    entry->create_time = readWord(drive, entryAddress + 0xE);
    entry->create_date = readWord(drive, entryAddress + 0x10);
    entry->last_access_date = readWord(drive, entryAddress + 0x12);
    entry->last_modified_time = readWord(drive, entryAddress + 0x16);
    entry->last_modified_date = readWord(drive, entryAddress + 0x18);
    entry->first_cluster = readWord(drive, entryAddress + 0x1A);
    entry->size_of_data = readDWord(drive, entryAddress + 0x1C);
    return entry;
}

void fat16_writeFolderEntry(vDrive* drive, uint folderStartAddress, const folderEntry* entry) {
    uint entryAddress = 0;
    for (uint i = 0; entryAddress == 0; i++) {
        byte marker = readByte(drive, folderStartAddress + i * FAT16_DIRECTORY_ENTRY_SIZE);
        if (marker == FAT16_ENTRY_FREE || marker == FAT16_ENTRY_LAST)
            entryAddress = folderStartAddress + i * FAT16_DIRECTORY_ENTRY_SIZE;
    }
    writeArray(drive, entryAddress, 8, entry->name);
    writeArray(drive, entryAddress + 0x8, 3, entry->extension);
    writeByte(drive, entryAddress + 0xB, entry->attributes);
    writeWord(drive, entryAddress + 0xE, entry->create_time);
    writeWord(drive, entryAddress + 0x10, entry->create_date);
    writeWord(drive, entryAddress + 0x12, entry->last_access_date);
    writeWord(drive, entryAddress + 0x16, entry->last_modified_time);
    writeWord(drive, entryAddress + 0x18, entry->last_modified_date);
    writeWord(drive, entryAddress + 0x1A, entry->first_cluster);
    writeDWord(drive, entryAddress + 0x1C, entry->size_of_data);
}

void fat16_clearFolderEntry(vDrive* drive, uint folderStartAddress, uint offset, byte flag) {
    writeByte(drive, folderStartAddress + offset * FAT16_DIRECTORY_ENTRY_SIZE, flag);
}

uint fat16_findFolderAddress(const vDrive* drive, const string virtualpath) {
    fatBS* bs = fat16_readBootSector(drive);
    if (!strncmp(virtualpath, "/", 1) && strlen(virtualpath) == 1)
        return fat16_getAddress(bs, FAT16_ROOT_DIRECTORY);
    string localPath = (string) calloc(strlen(virtualpath), sizeof(char));
    strcpy(localPath, virtualpath);

    uint address = fat16_getAddress(bs, FAT16_ROOT_DIRECTORY);
    char delimiter[] = "/";
    for (uint i = 1; i < strlen(virtualpath) + 1; i++)
        localPath[i-1] = localPath[i];

    string pathPart = strtok(localPath, delimiter);
    folderEntry* entry;
    while (pathPart != NULL) {
        toUpperCase(pathPart);
        for (uint i = 0;; i++) {
            entry = fat16_readFolderEntry(drive, address, i);
            if (!strncmp(entry->name, pathPart, strlen(pathPart)) && entry->attributes & FAT16_ATTR_SUBDIR) {
                address = fat16_getAddress(bs, FAT16_USER_AREA) + (entry->first_cluster-2) * drive->cluster_size;
                break;
            }
            if (i >= 0xFFFFFFFF)
                return 0;
        }

        pathPart = (string) strtok(NULL, delimiter);
    }
    free(entry);
    free(localPath);
    free(bs);
    return address;
}

fat16* fat16_initialiseDrive(vDrive* drive) {
    fatBS* bs = fat16_readBootSector(drive);
    if (!fat16_checkBootSector(bs)) {
        free(bs);
        return NULL;
    }
    drive->cluster_size = bs->sectors_per_cluster * SECTOR_SIZE;
    fat16* fat = fat16_readFat(drive, 1);
    free(bs);
    return fat;
}

void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, uint sectorsPerFat, string label) {
    fatBS* bs = fat16_generateBootSector(drive, sectorsPerCluster, sectorsPerFat, label);
    fat16_writeBootSector(drive, bs);
    fat16* fat = fat16_generateEmptyFat(bs);
    fat16_writeFat(drive, fat);
    for (uint i = 0; i < bs->number_root_entries; i++)
        fat16_clearFolderEntry(drive, fat16_getAddress(bs, FAT16_ROOT_DIRECTORY), i, FAT16_ENTRY_LAST);
    free(bs);
    free(fat);
}

folderEntry* fat16_findFile(const vDrive* drive, string virtualPath) {
    // Prepare:
    uint folderAddress = fat16_findFolderAddress(drive, getPathWithoutName(virtualPath));
    string fileName = (string) calloc(64, sizeof(char));
    strcpy(fileName, getNameFromPath(virtualPath));
    toUpperCase(fileName);
    uint posOfSeparator = strlen(fileName);
    for (uint i = 0; i < strlen(fileName); i++) {
        if (fileName[i] == '.')
            posOfSeparator = i;
    }
    string n = (string) calloc(8, sizeof(char));
    string e = (string) calloc(3, sizeof(char));
    strncpy(n, fileName, strlen(fileName)-4 < 8 ? posOfSeparator : 8);
    strncpy(e, fileName+posOfSeparator+1, 3);
    if (strlen(n) < 8)
        for (uint i = strlen(n); i < 8; i++)
            n[i] = ' ';
    if (strlen(e) < 3)
        for (uint i = strlen(e); i < 3; i++)
            e[i] = ' ';
    free(fileName);

    // Search:
    folderEntry* entry;
    for (uint i = 0;; i++) {
        entry = fat16_readFolderEntry(drive, folderAddress, i);
        if (entry == NULL) {
            printError("FINDFILE", "Could not find file.");
            return NULL;
        }
        if (!strncmp(entry->name, n, 8) && !strncmp(entry->extension, e, 3)) // TODO: FIX
            break;
    }
    free(n);
    free(e);
    return entry;
}

uint fat16_writeFile(vDrive* drive, string physicalPath, string virtualPath) {
    FILE* file;
    if ((file = fopen(physicalPath, "r")) != NULL) {
        // READ FILE
        uint realSize = getFileSize(file);
        fat16_file* physicalFile = (fat16_file*) calloc(1, sizeof(fat16_file));
        physicalFile->canonical_name = (string) calloc(strlen(getNameFromPath(virtualPath)), sizeof(char));
        strcpy(physicalFile->canonical_name, getNameFromPath(virtualPath));
        physicalFile->attributes = 0;
        physicalFile->real_size = realSize;
        physicalFile->necessary_clusters = (realSize / drive->cluster_size) + 1;
        physicalFile->data = (byte*) calloc(physicalFile->necessary_clusters * drive->cluster_size, sizeof(byte));
        fread(physicalFile->data, sizeof(byte), realSize, file);
        fclose(file);

        // WRITE FILE
        // Prepare:
        fat16* fat = fat16_readFat(drive, 1);
        word firstCluster = fat16_getNextFreeCluster(fat);

        // Things regarding the folder entry:
        folderEntry* entry = fat16_generateFolderEntry(physicalFile->canonical_name, physicalFile->attributes, firstCluster, physicalFile->real_size);
        uint folderAddress = fat16_findFolderAddress(drive, getPathWithoutName(virtualPath));
        if (folderAddress == 0)
            return 2;

        // Things regarding the fat:
        if (physicalFile->necessary_clusters > fat16_getClustersWithState(fat, FAT16_CLUSTER_FREE))
            return 3;
        for (uint i = 1; i <= physicalFile->necessary_clusters; i++) {
            uint fatEntryToWriteTo = fat16_getNextFreeCluster(fat);
            fat->entries[fatEntryToWriteTo] = FAT16_CLUSTER_EOC;
            if (i < physicalFile->necessary_clusters)
                fat->entries[fatEntryToWriteTo] = fat16_getNextFreeCluster(fat);
        }

        // Execute write:
        fat16_writeFolderEntry(drive, folderAddress, entry);
        free(entry);
        fat16_writeFat(drive, fat);
        // Data:
        fatBS* bs = fat16_readBootSector(drive);
        word actualCluster = firstCluster;
        for (uint i = 0;; i++) {
            writeArray(drive, fat16_getAddress(bs, FAT16_USER_AREA) + (actualCluster-2) * drive->cluster_size,
                    drive->cluster_size, physicalFile->data + i * drive->cluster_size);
            if (fat->entries[actualCluster] == FAT16_CLUSTER_EOC)
                break;
            actualCluster = fat->entries[actualCluster];
        }
        free(bs);
        free(fat);

        return 0;
    }
    fclose(file);
    return 1;
}

uint fat16_extractFile(const vDrive* drive, string virtualPath, string physicalPath) {

    // Read from vDrive:
    fatBS* bs = fat16_readBootSector(drive);
    fat16* fat = fat16_readFat(drive, 1);
    folderEntry* entry = fat16_findFile(drive, virtualPath);
    if (entry == NULL)
        return 1;
    byte* data = (byte*) calloc(entry->size_of_data, sizeof(byte));
    word actualCluster = entry->first_cluster;
    uint bytesToRead = entry->size_of_data;
    while (true) {
        readArray(drive, fat16_getAddress(bs, FAT16_USER_AREA) + (actualCluster-2) * drive->cluster_size,
                bytesToRead > drive->cluster_size ? drive->cluster_size : bytesToRead, data+entry->size_of_data-bytesToRead);
        bytesToRead -= bytesToRead > drive->cluster_size ? drive->cluster_size : bytesToRead;
        if (fat->entries[actualCluster] == FAT16_CLUSTER_EOC)
            break;
        actualCluster = fat->entries[actualCluster];
    }

    // Write to physical file:
    FILE* file;
    if ((file = fopen(physicalPath, "w+")) != NULL) {
        fwrite(data, sizeof(byte), entry->size_of_data, file);
        fclose(file);
    } else {
        fclose(file);
        return 2;
    }

    free(entry);
    free(fat);
    free(bs);
    free(data);
    return 0;
}

uint fat16_remove(vDrive* drive, string virtualPath) {
    fat16* fat = fat16_readFat(drive, 1);
    folderEntry* entry = fat16_findFile(drive, virtualPath);
    if (entry == NULL) {
        free(entry);
        free(fat);
        return 1;
    }

    // Locate folder entry
    uint folderAddress = fat16_findFolderAddress(drive, getPathWithoutName(virtualPath));
    uint entryOffset = 0;
    while(true) {
        folderEntry* e = fat16_readFolderEntry(drive, folderAddress, entryOffset);
        if (e->first_cluster == entry->first_cluster)
            break;
        entryOffset++;
    }

    // Locate and remove FAT entries
    word actualCluster = entry->first_cluster;
    while(true) {
        if (fat->entries[actualCluster] == FAT16_CLUSTER_EOC) {
            fat->entries[actualCluster] = FAT16_CLUSTER_FREE;
            break;
        }
        word previousCluster = actualCluster;
        actualCluster = fat->entries[actualCluster];
        fat->entries[previousCluster] = FAT16_CLUSTER_FREE;
    }
    fat16_writeFat(drive, fat);

    // Clear folder entry
    fat16_clearFolderEntry(drive, folderAddress, entryOffset, FAT16_ENTRY_FREE);

    free(entry);
    free(fat);
    return 0;
}

void fat16_makeDir(vDrive* drive, string virtualPath) {
    string name = (string) calloc(8, sizeof(char));
    strcpy(name, getNameFromPath(virtualPath));
    toUpperCase(name);
    for (uint i = strlen(name)-1; i < 8; i++)
        name[i] = ' ';
    fat16* fat = fat16_readFat(drive, 1);
    word firstCluster = fat16_getNextFreeCluster(fat);
    fat->entries[firstCluster] = 0xFFFF;
    folderEntry* entry = fat16_generateFolderEntry(name, FAT16_ATTR_SUBDIR, firstCluster, 0);
    uint address = fat16_findFolderAddress(drive, getPathWithoutName(virtualPath));
    if (address) {
        fat16_writeFat(drive, fat);
        fat16_writeFolderEntry(drive, address, entry);
    }
    free(entry);
    free(fat);
    free(name);
}

float fat16_getFragmentation(const fat16* fat) {
    uint defragCounter = 0;
    for (word i = 2; i < fat->length_in_bytes/2; i++) {
        if (fat->entries[i] == 0x0 || fat->entries[i] == 0xFFFF)
            continue;
        defragCounter += fat->entries[i]-i == 1 ? 0 : 1;
    }
    return (float) defragCounter / (fat->length_in_bytes/2);
}

void fat16_defrag(vDrive* drive) {
    // TODO: IMPLEMENT
}
