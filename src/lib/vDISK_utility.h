//! Utility
/*!
 * This file provides utility functions.
 */

#ifndef VDISK_VDISK_HELPERS_H
#define VDISK_VDISK_HELPERS_H
//////////////////////////

#include <stdio.h>

#include "vDISK_drive.h"

//! Checks whether or not a number is a power of two.
bool isPowerOfTwo(const uint n);

//! Prints Hexdump of drive (n bytes from offset).
void printHexdump(const vDrive* drive, uint offset, uint n, bool skipEmptyRows);

//! Prints Error on terminal.
void printError(const string causedBy, const string message);

//! Calculates size of file
uint getFileSize(FILE* file);

//! Prints byte array to console in ascii format.
void printByteArrayAscii(byte* data, uint n);

//! Returns the current date as a word.
word getDateNow();

//! Returns the current time as a word.
word getTimeNow();

//! Decodes date and returns is as a word.
string decodeDate(word date);

//! Decodes time and returns it as a word.
string decodeTime(word time);

//! Converts a string to upper case.
void toUpperCase(string s);

//! Returns the end of a path, which is the filename.
string getNameFromPath(string path);

//! Returns only the path component of a path+file string.
string getPathWithoutName(string path);

//////////////////////////
#endif //VDISK_VDISK_HELPERS_H
