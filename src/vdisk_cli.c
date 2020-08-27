//! vDISK command-line-input
/*!
 * This is an executable command-line interface to test and use vDISK.
 */

#include <stdio.h>
#include <string.h>

#include "lib/vDISK.h"

int main(int argc, char* argv[]) {

    // Initialisation //
    if (argc != 3) {
        printf("Usage:\nvdisk_cli create <size of disk in bytes>/<sectors (512 byte) per block>\n");
        printf("or\nvdisk_cli load <path of existing disk file (.vdisk)>\n");
        printf("Example: vdisk_cli create 65536/4 or vdisk_cli load drive1\n");
        return -1;
    }

    vDrive* drive = NULL; // Central vDrive

    if (!strncmp(argv[1], "create", 6)) {
        uint size, spb;
        if (2 == sscanf(argv[2], "%d/%d", &size, &spb))
            drive = createDrive(size, spb);
    } else if (!strncmp(argv[1], "load", 4)) {
        drive = loadDrive(argv[2]);
    }

    if (drive == NULL) {
        printError("TERMINAL", "Was not able to initialise disk");
        return -2;
    }

    // Control loop //
    while(true) {
        break;
    }

    return 0;
}