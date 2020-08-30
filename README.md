# vDISK
vDISK is a student project that simulates a virtual filesystem.
The program is designed for linux systems and is controlled via the terminal.

## Structure of the Program
vDisk distributes the tasks between the files as follows:

#### vdisk_cli.c
This is the main application that controls the console interaction.

#### vDISK_drive.h 
This file allows you to create and modify the virtual “hard disks” in memory. Which mean in  essence, it reserves memory for the partition.

#### vDISK_fat16.h 
This file allows you need to create a FAT16 partition on the virtual hard disk and interact with the filesystem.

#### vDISK_utility.h 
This file contains helper metheds.

## Cloning and building
### Linux 
#### vDISK-cli (for use in command line):
Perform these commands to clone and build vDISK-cli:
```
git clone https://github.com/saucechef/vDISK.git
cd vDISK/
chmod +x build_linux_cli.sh
./build_linux_cli.sh
cd build/
```
You can now use the binary `./vdisk_cli` to start vDISK-cli in the terminal.

## Usage
To get started, you must first create a new virtual drive. You can use the "newdrive" command to create a new virtual drive of any size. With the "format" command you can create a new FAT16 partition with any properties you want. Inside this partition you can now create directories with the "mkdir" command and files with the "wrfile" command. <br>
The navigation within the directories works like in the Linux terminal. To change the directory the "cd" command is used and to list the contents of the directory the "ls" command is used. <br>
The created filesystem can be saved as .img file with the "savedisk" command. It is also possible to import an existing filesystem into vDISK using the "loaddisk" command. <br>
With the "getinfo" command it is possible to display all available information about a file or the partition. To defragment the partition the "defrag" command is used. To check if a defragmentation is necessary you can use the "getfrag" command to display the fragmentation of the partition in percent. <br>
To list all commands you can simply use the "help" command.


## Commands:
#### help
Displays available commands and their usage.
#### hexdump [filepath/address] [number of rows]
Example: hexdump 0x800 65536 <br> This would dump the first copy of the FAT provided the formatting parameters are normal.
#### cd <path>
Example: cd /dir1 <br> Change from current working directory into subdirectory "dir1". <br>
Example: cd .. <br> Go up one level.
#### ls
Lists the content of current working directory.
#### mkdir <virtualpath>
Example: mkdir /dir2 <br> Creates new directory "dir2" in current working directory.
#### getinfo
Example: getinfo_fat16 <br> Prints all available info about the drive and its FAT16 partition. 
#### wrfile <physicalpath> <virtualpath>
Example: wrfile testfile.txt /test/file1.txt <br> Copies physical file "testfile.txt" into "file1.txt" ind directory "test".
#### exfile <virtualpath> <physicalpath>
Example: exfile /test/file1.txt extract.txt <br> Extracts file and writes it to "extract.txt".
#### rm <virtualpath>
Example: rm     /test <br> Removes directory "test" and all its content recursively.
#### getfrag
Gets fragmentation status of virtual FAT16 partition in percent.
#### defrag
Defrags virtual FAT16 partition.
#### savedisk <physicalpath>
Example: savedisk disk2 <br> Saves disk-image of vDrive to "disk2.img" (extension is added automatically).
#### loaddisk <physicalpath>
Example: loaddisk disk1.img <br> Loads disk-image into vDrive (extension is NOT added automatically).
#### format <fat16> <label> [sectorsPerCluster/4] [sectorsPerFat/128]
Example: format fat16 "TESTDRIVE" <br> Will format vDrive with a FAT16 partition with 4 sectors per cluster and 128 sectors per fat.
#### newdrive <size>

