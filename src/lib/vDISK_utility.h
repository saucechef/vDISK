//! Utility
/*!
 * This file provides utility functions.
 */

#ifndef VDISK_VDISK_HELPERS_H
#define VDISK_VDISK_HELPERS_H
//////////////////////////

#include "vDISK_drive.h"

//! Checks whether or not a number is a power of two.
bool isPowerOfTwo(const uint n);

//! Prints Hexdump of drive (n bytes from offset).
void printHexdump(const vDrive* drive, uint offset, uint n, bool skipEmptyRows);

//! Prints Error on terminal.
void printError(const string causedBy, const string message);

//! Calculates size of file
uint getFileSize(FILE* file);

//////////////////////////
#endif //VDISK_VDISK_HELPERS_H
