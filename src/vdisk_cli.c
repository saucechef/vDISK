//! vDISK command-line-input
/*!
 * This is an executable command-line interface to test and use vDISK.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/vDISK.h"

//! Global variables
vDrive* drive;
string workDir;
uint workDirAddr;

void cli_help() {
    printf("\nCLI COMMANDS:\n\n");
    printf("help\n");
    printf("hexdump [address] [number of rows]\n");
    printf("cd <path>\n");
    printf("ls\n");
    printf("mkdir <virtualpath>\n");
    printf("cp <virtualpath> <virtualpath>\n");
    printf("mv <virtualpath> <virtualpath>\n");
    printf("file-getinfo <virtualpath>\n");
    printf("fat16-getinfo\n");
    printf("wrfile <physicalpath> <virtualpath>\n");
    printf("exfile <virtualpath> <physicalpath>\n");
    printf("rm <virtualpath>\n");
    printf("getfrag\n");
    printf("defrag\n");
    printf("savedisk <physicalpath>\n");
    printf("loaddisk <physicalpath>\n");
    printf("format <fat16> <label> [sectorsPerCluster (default: 4)] [sectorsPerFats (default: 128)]\n");
    printf("newdrive <size>\n");
    printf("quit\n");
    printf("\n");
    printf("ANNOTATION: Physical paths are relative to the executable, virtual path are relative to the virtual working directory.\n");
    printf("\n");
    printf("EXAMPLES AND EXPLANATIONS:\n");
    printf("\n");
    printf("help                          - Displays available commands and their usage.\n");
    printf("hexdump 0x800 65536           - This would dump the first copy of the FAT provided normal formatting parameters.\n");
    printf("cd dir1                       - Change from current working directory into subdirectory \"dir1\".\n");
    printf("cd /                          - Go to root directory.\n");
    printf("ls                            - Lists the content of current working directory.\n");
    printf("mkdir dir2                    - Creates new directory \"dir2\" in current working directory.\n");
    printf("cp file1.txt file2.txt        - Copies a file to another location.\n");
    printf("mv file1.txt file2.txt        - Moves a file from one location to another. Can also be used to rename file.\n");
    printf("file-getinfo file1.txt        - Prints info of \"file1.txt\".\n");
    printf("fat16-getinfo                 - Prints all available info about the drive and its FAT16 partition.\n");
    printf("wrfile file.txt test.txt      - Copies physical file \"file.txt\" into current working directory under name \"test.txt\".\n");
    printf("exfile test.txt file.txt      - Extracts file and writes it to physical file.\n");
    printf("rm test                       - Removes directory \"test\". CAUTION: DOES NOT REMOVE RECURSIVELY.\n");
    printf("getfrag                       - Gets fragmentation status of virtual FAT16 partition in percent.\n");
    printf("defrag                        - Defrags virtual FAT16 partition.\n");
    printf("savedisk disk2                - Saves disk-image of vDrive to \"disk2.img\" (extension is added automatically).\n");
    printf("loaddisk disk1.img            - Loads disk-image into vDrive (extension is NOT added automatically).\n");
    printf("format fat16 TESTDRIVE        - Will format vDrive FAT16 with 4 sectors per cluster and 128 sectors per fat.\n");
    printf("newdrive 16777216             - Creates new drive with 16MiB of Storage.\n");
    printf("quit                          - Quits the program.\n\n");
}

void cli_hexdump(string addressS, string numberS) {
    uint address = 0;
    uint number = 0;
    if (addressS != NULL)
        sscanf(addressS, "0x%X", &address);
    if (numberS != NULL)
        sscanf(numberS, "%d", &number);
    printf("\n");
    printHexdump(drive, address, number, true);
    printf("\n");
}

void cli_format(string format, string label, string spc, string spf) {
    if (strcmp(format, "fat16")) {
        printf("Do not recognise this format.\n");
        return;
    }
    uint sectorsPerCluster = 4;
    uint sectorsPerFat = 128;
    if (spc != NULL)
        sscanf(spc, "%d", &sectorsPerCluster);
    if (spf != NULL)
        sscanf(spf, "%d", &sectorsPerFat);
    fat16_formatDrive(drive, sectorsPerCluster, sectorsPerFat, label);
}

void cli_getinfo_fat16() {
    fatBS* bs = fat16_readBootSector(drive);
    printf("\nvDrive size: %d bytes\n", drive->size_bytes);
    printf("Sectors per cluster: %d\n", drive->cluster_size);
    printf("\nValid FAT16 partition found: ");
    if (!fat16_checkBootSector(bs)) {
        printf("No.\n");
    }
    printf("Yes.\n");
    printf("Volume label: %.*s\n", 11, bs->volume_label);
    printf("OEM ID: %.*s\n", 8, bs->oem_identifier);
    printf("System ID: %.*s\n", 8, bs->system_id);
    printf("Volume serial number: 0x%08X\n", bs->volume_serial_number);
    printf("Bytes per sector: %d\n", bs->bytes_per_sector);
    printf("Number of small sectors: %d\n", bs->small_sectors);
    printf("Number of large sectors: %d\n", bs->large_sectors);
    printf("Sectors per FAT: %d\n", bs->sectors_per_fat);
    printf("Number of FATs: %d\n", bs->number_fats);
    printf("Number of root entries: %d\n", bs->number_root_entries);
    printf("Reserved clusters: %d\n", bs->reserved_clusters);
    printf("Hidden sectors: %d\n", bs->hidden_sectors);
    printf("Media type: 0x%02X\n", bs->media_type);
    printf("Physical disk number: 0x%02X\n\n", bs->physical_disk_number);
}

void cli_ls() {
    folderEntry* entry;
    for (uint i = 0; i < 64; i++) {
        entry = fat16_readFolderEntry(drive, workDirAddr, i);
        if (entry == NULL)
            continue;
        if (entry->attributes & FAT16_ATTR_SUBDIR) {
            printf("\ndir: %.*s\n", 8, entry->name);
            printf("\tCreation date: %s, %s\n", decodeDate(entry->create_date), decodeTime(entry->create_time));
        } else {
            printf("\nfile: %.*s(.%.*s)\n", 8, entry->name, 3, entry->extension);
            printf("\tCreation date: %s, %s\n", decodeDate(entry->create_date), decodeTime(entry->create_time));
            printf("\tSize: %d bytes\n", entry->size_of_data);
        }
    }
    printf("\n");
}

void cli_cd(string path) {
    toUpperCase(path);
    string combined = combinePath(workDir, path);
    if (path[strlen(path)-1] == '/') {
        workDirAddr = fat16_findFolderAddress(drive, "/");
        workDir = (string) realloc(workDir, 2);
        workDir[0] = '/';
        workDir[1] = '\0';
    } else if (fat16_findFolderAddress(drive, combined)) {
        workDirAddr = fat16_findFolderAddress(drive, combined);
        workDir = (string) realloc(workDir, strlen(combined)+2);
        strcpy(workDir, combined);
        workDir[strlen(workDir)] = '/';
    }
    free(combined);
}

void cli_file_getinfo(string path) {
    folderEntry* entry = fat16_findFile(drive, path);
    printf("\n\n%.*s(.%.*s)\n", 8, entry->name, 3, entry->extension);
    printf("\tCreation date: %s, %s\n", decodeDate(entry->create_date), decodeTime(entry->create_time));
    printf("\tLast modified: %s, %s\n", decodeDate(entry->last_modified_date), decodeTime(entry->last_modified_time));
    printf("\tIs directory: %s\n", entry->attributes & FAT16_ATTR_SUBDIR ? "YES" : "NO");
    printf("\tIs read only: %s\n", entry->attributes & FAT16_ATTR_READONLY ? "YES" : "NO");
    printf("\tIs system file: %s\n", entry->attributes & FAT16_ATTR_SYSTEM ? "YES" : "NO");
    printf("\tIs hidden: %s\n", entry->attributes & FAT16_ATTR_HIDDEN ? "YES" : "NO");
    printf("\tSize: %d bytes\n\n", entry->size_of_data);
    free(entry);
}

int main(int argc, char* argv[]) {

    // Initialisation //
    bool autoCreate = false;
    if (argc != 3) {
        printf("\nYou can also use following commands to start vdisk_cli:\n\n");
        printf("Usage:\n\tvdisk_cli create <size of disk in bytes>/<sectors per cluster>\n");
        printf("or\n\tvdisk_cli load <path and name of existing disk image>\n");
        printf("Example:\n\tvdisk_cli create 16777216/4\nor\n\tvdisk_cli load drive1.img\n\n");
        autoCreate = true;
    }

    drive = NULL; // Central vDrive

    uint size = 16777216, spc = 4;
    if (autoCreate) {
        drive = createDrive(size, spc);
        printf("\nCreated new vDrive with size %d bytes, sector size %d bytes and %d sectors per cluster.\n",
               drive->size_bytes, SECTOR_SIZE, drive->cluster_size / SECTOR_SIZE);
        fat16_formatDrive(drive, spc, 128, "PLAYGROUND");
        printf("Formatted drive FAT16 with %d sectors per FAT, labelled \"PLAYGROUND\".\n", 128);
    } else {
        if (!strncmp(argv[1], "create", 6) || autoCreate) {
            sscanf(argv[2], "%d/%d", &size, &spc);
            drive = createDrive(size, spc);
            printf("\nCreated new vDrive with size %d bytes, sector size %d bytes and %d sectors per cluster.\n",
                   drive->size_bytes, SECTOR_SIZE, drive->cluster_size / SECTOR_SIZE);
            fat16_formatDrive(drive, spc, 128, "PLAYGROUND");
            printf("Formatted drive FAT16 with %d sectors per FAT, labelled \"PLAYGROUND\".\n", 128);
        } else if (!strncmp(argv[1], "load", 4)) {
            drive = loadDrive(argv[2]);
        }
    }

    if (drive == NULL) { // Check if vDrive initialised successfully.
        printError("TERMINAL", "Was not able to initialise vDrive.\n");
        return -2;
    }

    if (fat16_initialiseDrive(drive) == NULL) // Check if can initialise FAT16 partition.
        printf("Was not able to initialise FAT16 partition on vDrive.\n");
    else
        printf("Successfully initialised FAT16 partition on vDrive.\n");

    workDir = (string) calloc(2, sizeof(char));
    strcpy(workDir, "/");
    fatBS* bs = fat16_readBootSector(drive);
    workDirAddr = fat16_getAddress(bs, FAT16_ROOT_DIRECTORY);
    free(bs);

    string inputBuf = (string) calloc(128, sizeof(char));
    string delimiter = " ";
    string args[5];

    // Control loop //
    printf("\nType help to get info about available commands.\n\n");
    while(true) {
        // Print
        printf("%s>", workDir);

        // Scan
        fgets(inputBuf, 128, stdin);
        args[0] = strtok(inputBuf, delimiter);
        uint argCount = 0;
        for (uint i = 1; i < 5; i++) {
            args[i] = strtok(NULL, delimiter);
            if (args[i] != NULL)
                argCount++;
        }
        // Execute
        if (!strncmp(args[0], "quit", 4)) {
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else
                break;
        } else if (!strncmp(args[0], "help", 4)) {
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else
                cli_help();
        } else if (!strncmp(args[0], "hexdump", 7)) {
            if (argCount > 2)
                printf("Syntax error. Type help for available commands.\n");
            else
                cli_hexdump(args[1], args[2]);
        } else if (!strncmp(args[0], "cd", 2)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string copy = (string) calloc(strlen(args[1]), sizeof(char));
                memcpy(copy, args[1], strlen(args[1])-1);
                cli_cd(copy);
                free(copy);
            }
        } else if (!strncmp(args[0], "ls", 2)) {
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else
                cli_ls();
        } else if (!strncmp(args[0], "mkdir", 5)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combined = combinePath(workDir, args[1]);
                fat16_makeDir(drive, combined);
                free(combined);
            }
        } else if (!strncmp(args[0], "cp", 2)) {
            if (argCount != 2)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combinedSrc = combinePath(workDir, args[1]);
                string combinedDest = combinePath(workDir, args[2]);
                fat16_extractFile(drive, combinedSrc, "temp.dat");
                fat16_writeFile(drive, "temp.dat", combinedDest);
                free(combinedSrc);
                free(combinedDest);
            }
        } else if (!strncmp(args[0], "mv", 2)) {
            if (argCount != 2)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combinedSrc = combinePath(workDir, args[1]);
                string combinedDest = combinePath(workDir, args[2]);
                fat16_extractFile(drive, combinedSrc, "temp.dat");
                fat16_writeFile(drive, "temp.dat", combinedDest);
                fat16_remove(drive, combinedSrc);
                free(combinedSrc);
                free(combinedDest);
            }
        } else if (!strncmp(args[0], "file-getinfo", 12)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combined = combinePath(workDir, args[1]);
                cli_file_getinfo(combined);
                free(combined);
            }
        } else if (!strncmp(args[0], "fat16-getinfo", 13)) {
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else
                cli_getinfo_fat16();
        } else if (!strncmp(args[0], "wrfile", 6)) {
            if (argCount != 2)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combined = combinePath(workDir, args[2]);
                fat16_writeFile(drive, args[1], combined);
                free(combined);
            }
        } else if (!strncmp(args[0], "exfile", 6)) {
            if (argCount != 2)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combined = combinePath(workDir, args[1]);
                string copy = (string) calloc(strlen(args[2]), sizeof(char));
                memcpy(copy, args[2], strlen(args[2])-1);
                fat16_extractFile(drive, combined, copy);
                free(copy);
                free(combined);
            }
        } else if (!strncmp(args[0], "rm", 2)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string combined = combinePath(workDir, args[1]);
                fat16_remove(drive, combined);
                free(combined);
            }
        } else if (!strncmp(args[0], "getfrag", 7)) {
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else {
                fat16* fat = fat16_readFat(drive, 1);
                printf("Current fragmentation status: %.02f%%\n", fat16_getFragmentation(fat));
                free(fat);
            }
        } else if (!strncmp(args[0], "defrag", 6)) { // TODO
            if (argCount > 0)
                printf("Syntax error. Type help for available commands.\n");
            else
                continue;
        } else if (!strncmp(args[0], "savedisk", 8)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string copy = (string) calloc(strlen(args[1]), sizeof(char));
                memcpy(copy, args[1], strlen(args[1])-1);
                saveDrive(drive, copy);
                free(copy);
            }
        } else if (!strncmp(args[0], "loaddisk", 8)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                string copy = (string) calloc(strlen(args[1]), sizeof(char));
                memcpy(copy, args[1], strlen(args[1])-1);
                drive = loadDrive(copy);
                if (drive == NULL)
                    printf("Failed to load drive. You need to execute newdrive next!\n");
                free(copy);
            }
        } else if (!strncmp(args[0], "format", 6)) {
            if (argCount < 2)
                printf("Syntax error. Type help for available commands.\n");
            else {
                cli_format(args[1], args[2], args[3], args[4]);
            }
                continue;
        } else if (!strncmp(args[0], "newdrive", 8)) {
            if (argCount != 1)
                printf("Syntax error. Type help for available commands.\n");
            else {
                uint size = 0;
                sscanf(args[1], "%d", &size);
                if (size)
                    drive = createDrive(size, 4);
            }
        } else {
            printf("Command not found. Type help for available commands.\n");
        }
    }

    fat16* fat = fat16_readFat(drive, 1);
    printf("FRAG: %f\n", fat16_getFragmentation(fat));
    free(fat);

    free(inputBuf);
    free(workDir);
    free(drive);
    return 0;
}