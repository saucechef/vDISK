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
        printf("\nFor command-line use, it is necessary to initially create or load a vDrive.\n\n");
        printf("Usage:\n\tvdisk_cli create <size of disk in bytes>/<sectors (512 byte) per block>\n");
        printf("or\n\tvdisk_cli load <path of existing disk image (.img)>\n");
        printf("Example:\n\tvdisk_cli create 65536/4\nor\n\tvdisk_cli load drive1\n\n");
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

    // DEBUG //
    writeByte(drive, 0x0, 0b11111111);
    writeByte(drive, 0x2, 0b11111111);
    printHexdump(drive, 0, 0, true);

    return 0;
}