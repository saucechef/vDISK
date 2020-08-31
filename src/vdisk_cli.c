//! vDISK command-line-input
/*!
 * This is an executable command-line interface to test and use vDISK.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/vDISK.h"

int main(int argc, char* argv[]) {

    // Initialisation //
    if (argc != 3) {
        printf("\nFor command-line use, it is necessary to initially create or load a vDrive.\n\n");
        printf("Usage:\n\tvdisk_cli create <size of disk in bytes>/<sectors (512 byte) per cluster>\n");
        printf("or\n\tvdisk_cli load <path and name of existing disk image>\n");
        printf("Example:\n\tvdisk_cli create 65536/4\nor\n\tvdisk_cli load drive1.img\n\n");
        return -1;
    }

    vDrive* drive = NULL; // Central vDrive

    if (!strncmp(argv[1], "create", 6)) {
        uint size, spc;
        if (2 == sscanf(argv[2], "%d/%d", &size, &spc)) {
            drive = createDrive(size, spc);
            printf("\nCreated new vDrive with size %d bytes, sector size %d bytes and %d sectors per cluster.\n",
                    drive->size_bytes, SECTOR_SIZE, drive->cluster_size / SECTOR_SIZE);
            fat16_formatDrive(drive, spc, 128, "PLAYGROUND");
            printf("Formatted drive FAT16 with %d sectors per FAT, labelled \"PLAYGROUND\".\n", 128);
        }
    } else if (!strncmp(argv[1], "load", 4)) {
        drive = loadDrive(argv[2]);
    }

    if (drive == NULL) { // Check if vDrive initialised successfully.
        printError("TERMINAL", "Was not able to initialise vDrive.\n");
        return -2;
    }

    if (fat16_initialiseDrive(drive) == NULL) // Check if can initialise FAT16 partition.
        printf("Was not able to initialise FAT16 partition on vDrive.\n");
    else
        printf("Successfully initialised FAT16 partition on vDrive.\n");

    string workDir = (string) calloc(2, sizeof(char));
    strcpy(workDir, "/");
    fatBS* bs = fat16_readBootSector(drive);
    uint workDirAddr = fat16_getAddress(bs, FAT16_ROOT_DIRECTORY);
    free(bs);

    // Control loop //
    printf("\nType help to get info about available commands.\n\n");
    while(true) {
        break;
    }

    // DEBUG //
    string test = (string) calloc(24, sizeof(char));
    strcpy(test, "/dir1");
    fat16_makeDir(drive, test);
    string test2 = (string) calloc(24, sizeof(char));
    strcpy(test2, "/dir1/dir2");
    fat16_makeDir(drive, test2);
    string test3 = (string) calloc(24, sizeof(char));
    strcpy(test3, "/dir1/dir2/dir3");
    fat16_makeDir(drive, test3);
    fat16_writeFile(drive, "test.txt", "/dir1/test.txt");
    printHexdump(drive, 0, 0, true);
    fat16_remove(drive, "/dir1/test.txt");
    printHexdump(drive, 0, 0, true);
    fat16_writeFile(drive, "test.txt", "/dir1/test.txt");
    printHexdump(drive, 0, 0, true);
    fat16_remove(drive, "/dir1/test.txt");
    printHexdump(drive, 0, 0, true);

    return 0;
}