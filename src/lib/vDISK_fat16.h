//! FAT16 filesystem
/*!
 * This file allows to format and use a vDrive with a FAT16-like filesystem.
 */

#ifndef VDISK_CLI_VDISK_FAT16_H
#define VDISK_CLI_VDISK_FAT16_H
//////////////////////////

#include "vDISK_drive.h"

//! This struct models the boot sector of a FAT partition.
typedef struct {
    byte jump_instruction[3];
    byte oem_identifier[8];
    word bytes_per_sector;
    byte sectors_per_cluster;
    word reserved_sectors;
    byte number_fats;
    word number_root_entries;
    word small_sectors;
    byte media_type;
    word sectors_per_fat;
    word sectors_per_track;
    word number_of_heads;
    uint hidden_sectors;
    uint large_sectors;
    byte physical_disk_number;
    byte current_head;
    byte signature;
    uint volume_serial_number;
    byte volume_label[11];
    byte system_id[8];
} fatBS;

typedef struct {
    // TODO: CONSTRUCT
} fat16;

//! Generates a boot sector for given drive with given parameters.
/*!
 * @param drive To generate boot sector for.
 * @param sectorsPerCluster Number of sectors per cluster.
 * @param label Volume label of partition.
 * @return Pointer to struct containing necessary info.
 */
fatBS* fat16_generateBootSector(const vDrive* drive, uint sectorsPerCluster, string label);

//! Reads boot sector from drive. Part of checking process.
/*!
 * @param drive To be checked.
 * @return Pointer to new fatBS (copy of drive).
 */
fatBS* fat16_readBootSector(const vDrive* drive);

//! Writes boot sector to drive. Part of formatting process.
/*!
 * @param drive To be written to.
 * @param bs Data to be written.
 */
void fat16_writeBootSector(vDrive* drive, const fatBS* bs);

// TODO: COMMENT AND IMPLEMENT
fat16* fat16_readFat(const vDrive* drive);

// TODO: COMMENT AND IMPLEMENT
void fat16_writeFat(vDrive* drive, const fat16* fat);

//! Checks the boot sector of given drive to see if it has a valid FAT16 partition.
/*!
 * @param drive To be checked.
 * @return true if drive has valid FAT16 partition.
 */
bool fat16_checkDrive(const vDrive* drive);

//! Initialises drive with FAT16 filesystem on it (Loads FAT into RAM)
/*!
 * This is necessary to be able to work with the drive. WORKS ONLY IF checkDrive RETURNS true ON GIVEN DRIVE!
 * @param drive To be initialised.
 * @return Pointer to copy of FAT that was found in drive.
 */
fat16* fat16_initialiseDrive(const vDrive* drive);

//! Formats given drive to FAT16
/*!
 * @param drive To be formatted.
 * @param sectorsPerCluster Number of sectors per cluster. Has to be power of two.
 * @param label Volume name. 10 characters maximum.
 */
void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, string label);

//////////////////////////
#endif //VDISK_CLI_VDISK_FAT16_H
