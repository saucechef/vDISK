//! Drive
/*!
 * This file provides the drive funtionality of vDISK.
 */

#ifndef VDISK_VDISK_DRIVE_H
#define VDISK_VDISK_DRIVE_H
//////////////////////////

#include "vDISK_macros.h"

typedef struct {
    uint size_bytes;
    byte* bytes;
    uint blocksize;
} vDrive;

//! Creates a new vDrive.
/*!
 * @param size Overall size in bytes.
 * @param sectorsPerBlock Amount of sectors (each sector is 512 byte) in one block. Has to be power of 2.
 * @return Pointer to new vDrive.
 */
vDrive* createDrive(uint size, uint sectorsPerBlock);

//! Saves a vDrive to a (real) file.
/*!
 * @param path Path and name for file WITHOUT FILE EXTENSION.
 */
void saveDrive(const vDrive* drive, const string path);

//! Loads drive from a (real) file.
/*!
 * @param drive
 * @return Pointer to new loaded drive.
 */
vDrive* loadDrive(const string path);

//! Clears a vDrive from RAM.
/*!
 * @param To be deleted.
 */
void deleteDrive(vDrive* drive);

//! Reads byte from Drive.
/*!
 * @param drive To be read from.
 * @param addr Byte to be read.
 * @return Value
 */
byte readByte(const vDrive* drive, uint addr);

//! Writes byte to Drive.
/*!
 * @param drive To be written to.
 * @param addr Address
 * @param val Value
 */
void writeByte(vDrive* drive, uint addr, byte val);

//! Reads bytes from Drive.
/*!
 * @param drive To be read from.
 * @param offset Address that reading starts at.
 * @param n Bytes to be read from offset.
 * @return Pointer to new byte array (copy of drive data).
 */
byte* readArray(const vDrive* drive, uint offset, uint n);

//! Writes bytesconst to Drive.
/*!
 * @param drive To be written to.
 * @param data Which will overwrite n bytes from offset.
 * @param offset Address where writing starts.
 * @param n Amount of bytes to write.
 */
void writeArray(vDrive* drive, uint offset, uint n, const byte* data);

//! Reads a sector from Drive.
/*!
 * @param drive To be read from.
 * @param sectorID Sector to be read.
 * @return Pointer to new byte array (copy of sector).
 */
byte* readSector(const vDrive* drive, uint sectorID);

//! Writes a sector to the drive.
/*!
 * @param drive To be written to.
 * @param sectorID Sector that will be overwritten.
 * @param data To be written.
 */
void writeSector(vDrive* drive, uint sectorID, const byte* data);

//! Reads a block from Drive.
/*!
 * @param drive To be read from.
 * @param blockID Sector to be read.
 * @return Pointer to new byte array (copy of block).
 */
byte* readBlock(const vDrive* drive, uint blockID);

//! Writes a block to the drive.
/*!
 * @param drive To be written to.
 * @param blockID block that will be overwritten.
 * @param data To be written.
 */
void writeBlock(vDrive* drive, uint blockID, const byte* data);

//////////////////////////
#endif //VDISK_VDISK_DRIVE_H