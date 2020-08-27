//! vDISK command-line-input
/*!
 * This is an executable command-line interface to test and use vDISK.
 */

#include <stdio.h>
#include <string.h>

#include "lib/vDISK.h"

int main(int argc, char* argv[]) {

    // Initialisation
    if (argc != 3) {
        printf("Usage:\nvdisk_cli create <size of disk in bytes>/<sectors (512 byte) per block>\n");
        printf("or\nvdisk_cli load <path of existing disk file (.vdisk)>\n");
        printf("Example: vdisk_cli create 65536/4\n");
        return 1;
    }

    vDrive* drive = NULL; // Central vDrive

    if (strcmp(argv[1], "create ")) {
        uint size, spb;
        if (2 == sscanf(argv[2], "%d/%d", &size, &spb))
            drive = createDrive(size, spb);
    } else if (strcmp(argv[1], "load ")) {
        //IMPLEMENT
    }

    if (drive == NULL) {
        printError("TERMINAL", "Was not able to initialise disk");
        return 1;
    }

    // Loop
    writeArray(drive, 0, 12, " X mkfs fat ");
    printHexdump(drive, 0, 0, false);
}