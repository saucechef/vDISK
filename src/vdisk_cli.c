/*!
 * This is an executable command-line interface to test and use vDISK.
 */

#include <stdio.h>

#include "core/vDISK.h"

int main(int argc, char* argv[]) {
    vDrive* testDrive = createDrive(1024, 1);
    writeArray(testDrive, 0, 12, " X mkfs fat ");
    writeArray(testDrive, 128, 12, " X mkfs fat ");
    printHexdump(testDrive, 0, 1024, false);
}