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

//! Entry marker values
#define FAT16_ENTRY_LAST 0x0
#define FAT16_ENTRY_DOT 0x2E
#define FAT16_ENTRY_FREE 0xE5

//! File attribute values
#define FAT16_ATTR_READONLY 0x1
#define FAT16_ATTR_HIDDEN 0x2
#define FAT16_ATTR_SYSTEM 0x4
#define FAT16_ATTR_VOLUMELABEL 0x8
#define FAT16_ATTR_SUBDIR 0x10
#define FAT16_ATTR_ARCHIVE 0x20
#define FAT16_ATTR_DEVICE 0x40

//! Other Macros
#define FAT16_OEM_IDENTIFIER "vDISKfat"
#define FAT16_DIRECTORY_ENTRY_SIZE 32
#define FAT16_MAX_DIRECTORY_DEPTH 64

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

//! This struct models an entry to a folder.
typedef struct {
    byte name[8]; // First character can have special value -> lookup in macros.
    byte extension[3];
    byte attributes;
    word create_time;
    word create_date;
    word last_access_date;
    word last_modified_time;
    word last_modified_date;
    word first_cluster;
    uint size_of_data;
} folderEntry;

//! This struct models a file, which can also be a folder.
typedef struct {
    string canonical_name;
    byte attributes;
    byte* data;
    uint real_size;
    uint necessary_clusters;
} fat16_file;

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

//! Returns the number of clusters that fullfil a given state, eg FAT16_CLUSTER_FREE or FAT16_CLUSTER_BAD.
/*!
 * @param fat To be analysed.
 * @param state To be checked for.
 * @return Number of clusters fullfilling given state.
 */
uint fat16_getClustersWithState(const fat16* fat, uint state);

//! Returns id of next free cluster from fat.
/*!
 * @param fat To be checked.
 * @return id of next free cluster.
 */
word fat16_getNextFreeCluster(const fat16* fat);

//! Generates new folder entry for given parameters.
/*!
 * @param fileName Name of file with extension.
 * @param attributes Attributes of file.
 * @param firstCluster ID of first cluster of file.
 * @param sizeOfData Size of file data in bytes.
 * @return Pointer to newly generated entry struct.
 */
folderEntry* fat16_generateFolderEntry(string fileName, byte attributes, word firstCluster, uint sizeOfData);

//! Reads folder entry from vDrive.
/*!
 * @param drive To be read from.
 * @param folderStartAddress Starting address of folder.
 * @param offset Number of entry to be read.
 * @return Pointer to new copy of folder entry.
 */
folderEntry* fat16_readFolderEntry(const vDrive* drive, uint folderStartAddress, uint offset);

//! Writes new folder Entry to folder.
/*!
 * @param drive To be written to.
 * @param folderStartAddress Starting address of folder region.
 * @param entry Entry to write.
 */
void fat16_writeFolderEntry(vDrive* drive, uint folderStartAddress, const folderEntry* entry);

//! Removes an entry from the folder.
/*!
 * @param drive To be changed.
 * @param folderStartAddress Starting address of folder.
 * @param offset Number of entry to be cleared.
 */
void fat16_clearFolderEntry(vDrive* drive, uint folderStartAddress, uint offset, byte flag);

//! Finds and returns starting address of folder.
/*!
 * @param drive To be looked in.
 * @param virtualpath Directory to be looked for.
 * @return Starting Address.
 */
uint fat16_findFolderAddress(const vDrive* drive, const string virtualpath);

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
 * @param sectorsPerFat Number of sectors per FAT.
 * @param label Volume name. 10 characters maximum.
 */
void fat16_formatDrive(vDrive* drive, uint sectorsPerCluster, uint sectorsPerFat, string label);

//! Finds and returns the folder entry for a file or NULL if cant be found.
/*!
 * @param drive To be looked in.
 * @param virtualPath Path and name of file.
 * @return Pointer to copy of folder entry or NULL if cant be found.
 */
folderEntry* fat16_findFile(const vDrive* drive, string virtualPath);

//! Copies file from physical disk to vDrive.
/*!
 * @param drive To which file will be copied.
 * @param physicalPath Path of file on physical drive, eg. "/dir/testfile.txt".
 * @param virtualPath Path on vDrive where file is to be created, eg. "/virtualdir/testfile.txt".
 * @return 0 if successful, 1 if not able to open file, 2 if not able to create file on vDrive, 3 if file is too big.
 */
uint fat16_writeFile(vDrive* drive, string physicalPath, string virtualPath);

//! Extracts file from vDrive and writes it to physical disk.
/*!
 * @param drive To be read from.
 * @param virtualPath Location of file on vDrive, eg "/virtualdir/testfile.txt".
 * @param physicalPath Path on physical disk where file shall be written to, eg. "/dir/testfile.txt".
 * @return 0 if successful, 1 if not able to open file ON vDrive, 2 if not able to write to physical drive.
 */
uint fat16_extractFile(const vDrive* drive, string virtualPath, string physicalPath);

//! Removes file from vDrive (only overwrites FAT).
/*!
 * @param drive From which file is to be removed.
 * @param virtualPath Location of file on vDrive, eg "/virtualdir/testfile.txt".
 * @return 0 if successful, 1 if file could not be found.
 */
uint fat16_remove(vDrive* drive, string virtualPath);

//! Creates new directory at given path.
/*!
 * @param drive Drive on which diretory is to be created.
 * @param virtualPath Path of new directory, eg. "/dir1/newdir"
 */
void fat16_makeDir(vDrive* drive, string virtualPath);

//! Calculates the fragmentation of the partition as a value between 0 and 1
/*!
 * @param fat To be checked.
 * @return Amount of fragmentation between 0 and 1.
 */
float fat16_getFragmentation(const fat16* fat);

//! Defragments the FAT16 partition.
/*!
 * @param drive To defrag.
 */
void fat16_defrag(vDrive* drive);

//////////////////////////
#endif //VDISK_CLI_VDISK_FAT16_H
