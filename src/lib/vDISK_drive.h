//! Drive
/*!
 * This file provides the drive funtionality of vDISK.
 */

#ifndef VDISK_VDISK_DRIVE_H
#define VDISK_VDISK_DRIVE_H
//////////////////////////

#include "vDISK_macros.h"

//! This struct models a hard drive with only the simplest of characteristics.
typedef struct {
    uint size_bytes;
    byte* bytes;
    uint cluster_size;
} vDrive;

//! Creates a new vDrive.
/*!
 * @param size Overall size in bytes.
 * @param sectorsPerCluster Amount of sectors (each sector is 512 byte) in one cluster. Has to be power of 2.
 * @return Pointer to new vDrive.
 */
vDrive* createDrive(uint size, uint sectorsPerCluster);

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

//! Reads word from Drive IN LITTLE-ENDIAN FORMAT!
/*!
 * @param drive To be read from.
 * @param addr Word to be read.
 * @return Value
 */
word readWord(const vDrive* drive, uint addr);

//! Writes word to Drive IN LITTLE-ENDIAN FORMAT!
/*!
 * @param drive To be written to.
 * @param addr Address
 * @param val Value
 */
void writeWord(vDrive* drive, uint addr, word val);

//! Reads dword from Drive IN LITTLE-ENDIAN FORMAT!
/*!
 * @param drive To be read from.
 * @param addr Dword to be read.
 * @return Value
 */
uint readDWord(const vDrive* drive, uint addr);

//! Writes dword to Drive IN LITTLE-ENDIAN FORMAT!
/*!
 * @param drive To be written to.
 * @param addr Address
 * @param val Value
 */
void writeDWord(vDrive* drive, uint addr, uint val);

//! Reads bytes from Drive.
/*!
 * @param drive To be read from.
 * @param offset Address that reading starts at.
 * @param n Bytes to be read from offset.
 * @param dest Destination byte array.
 */
void readArray(const vDrive* drive, uint offset, uint n, byte* dest);

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
 * @param dest Destination byte array.
 */
void readSector(const vDrive* drive, uint sectorID, byte* dest);

//! Writes a sector to the drive.
/*!
 * @param drive To be written to.
 * @param sectorID Sector that will be overwritten.
 * @param data To be written.
 */
void writeSector(vDrive* drive, uint sectorID, const byte* data);

//! Reads a cluster from Drive.
/*!
 * @param drive To be read from.
 * @param clusterID Sector to be read.
 * @param dest Destination byte array.
 */
void readCluster(const vDrive* drive, uint clusterID, byte* dest);

//! Writes a cluster to the drive.
/*!
 * @param drive To be written to.
 * @param clusterID cluster that will be overwritten.
 * @param data To be written.
 */
void writeCluster(vDrive* drive, uint clusterID, const byte* data);

//////////////////////////
#endif //VDISK_VDISK_DRIVE_H
