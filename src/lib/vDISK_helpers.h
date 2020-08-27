#ifndef VDISK_VDISK_HELPERS_H
#define VDISK_VDISK_HELPERS_H
//////////////////////////

#include "vDISK_drive.h"

//! Checks whether or not a number is a power of two.
bool isPowerOfTwo(uint n);

//! Prints Hexdump of drive (n bytes from offset).
void printHexdump(vDrive* drive, uint offset, uint n, bool skipEmptyRows);

//! Prints Error on terminal.
void printError(string causedBy, string message);

//////////////////////////
#endif //VDISK_VDISK_HELPERS_H
