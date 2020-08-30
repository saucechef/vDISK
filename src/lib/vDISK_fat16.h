//! FAT16 filesystem
/*!
 * This file allows to format and use a vDrive with a FAT16-like filesystem.
 */

#ifndef VDISK_CLI_VDISK_FAT16_H
#define VDISK_CLI_VDISK_FAT16_H
//////////////////////////

#include "vDISK_drive.h"

//! Macros to be used by the getAddress function.
#define FAT16_MBR 0
#define FAT16_ROOT_DIRECTORY 1
#define FAT16_USER_AREA 2
#define FAT16_FAT 10

//! Macros that declare clusters a certain state.
#define FAT16_CLUSTER_FREE 0x0000
#define FAT16_CLUSTER_BAD 0xFFF7
#define FAT16_CLUSTER_EOC 0xFFFF // CAUTION: Can also be 0xFFF8 - 0xFFFE

//! Other Macros
#define FAT16_OEM_IDENTIFIER "vDISKfat"
#define FAT16_DIRECTORY_ENTRY_SIZE 32

//! This struct models the boot sector of a FAT partition.
typedef struct {
    byte jump_instruction[3];
    byte oem_identifier[8];
    word bytes_per_sector;
    byte sectors_per_cluster;
    word reserved_clusters;
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
    byte bootstrap_code[130];
} fatBS;

//! This struct models a FAT.
typedef struct {
    word* entries;
    uint length_in_bytes;
} fat16;

//! Generates a boot sector for given drive with given parameters.
/*!
 * @param drive To generate boot sector for.
 * @param sectorsPerCluster Number of sectors per cluster.
 * @param label Volume label of partition.
 * @return Pointer to struct containing necessary info.
 */
fatBS* fat16_generateBootSector(const vDrive* drive, uint sectorsPerCluster, uint sectorsPerFat, string label);

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

//! Checks the boot sector to see if it appears to have a valid FAT16 format.
/*!
 * @param bs Boot sector to be checked.
 * @return true if boot sector has valid FAT16 format.
 */
bool fat16_checkBootSector(const fatBS* bs);

//! Seeks and returns the starting address of a given area.
/*!
 * USE MACROS! If looking for second copy of FAT, use FAT16_FAT like "FAT16_FAT*2".
 * @param bs Boot sector that provides metadata.
 * @param area To be looked for.
 * @return Starting address of area.
 */
uint fat16_getAddress(const fatBS* bs, uint area);

//! Generates new empty FAT16.
/*!
 * @param drive That FAT should be generated for. Drive has to have valid boot sector at this point.
 * @return
 */
fat16* fat16_generateEmptyFat(const fatBS* bs);

//! Reads and compares FATs from drive.
/*!
 * @param drive To be read from.
 * @param number Which copy of the FAT is to be read (Usually 1).
 * @return Pointer to new copy of FAT.
 */
fat16* fat16_readFat(const vDrive* drive, uint number);

//! Writes FAT to drive.
/*!
 * @param drive To be written to.
 * @param fat To be written.
 */
void fat16_writeFat(vDrive* drive, const fat16* fat);

//! Initialises drive with FAT16 filesystem on it (Loads FAT into RAM).
/*!
 * This is necessary to be able to work with the drive. WORKS ONLY IF checkBootSector RETURNS true ON GIVEN DRIVE!
 * @param drive To be initialised.
 * @return Pointer to copy of FAT that was found in drive.
 */
fat16* fat16_initialiseDrive(vDrive* drive);

//! Formats given drive to FAT16.
/*!
 * @param drive To be formatted.
 * @param sectorsPerCluster Number of sectors per cluster. Has to be power of two.
 * @param label Volume name. 10 characters maximum.
 */
void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, uint sectorsPerFat, string label);

// TODO: COMMENT AND IMPLEMENT
void fat16_makeDir(vDrive* drive, string virtualPath);

// TODO: COMMENT AND IMPLEMENT
uint fat16_writeFile(vDrive* drive, string physicalPath, string virtualPath);

// TODO: COMMENT AND IMPLEMENT
uint fat16_extractFile(const vDrive* drive, string virtualPath, string physicalPath);

// TODO: COMMENT AND IMPLEMENT
uint fat16_remove(vDrive* drive, string virtualPath);

// TODO: COMMENT AND IMPLEMENT
void fat16_ls(const vDrive* drive, string virtualPath);

//////////////////////////
#endif //VDISK_CLI_VDISK_FAT16_H
