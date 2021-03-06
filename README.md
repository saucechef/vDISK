# vDISK

vDISK is a student project that simulates a virtual FAT16 filesystem.  
Core features are:  
* Initialising and managing a virtual harddrive in RAM.
* Formatting and using the virtual harddrive with a FAT16 filesystem.
* Interfacing with the physical drive by file IO.
* Saving and loading the virtual harddrive from image files on physical medium.

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

[**Doxygen Doc**](https://saucechef.github.io/vDISK/) - [**alternative link**](https://bytekitchen.de/doc/vDISK/)

## Cloning and building
### Linux 
#### vDISK-cli (command line interface):
Perform these commands to clone and build vDISK-cli:  
Notice: You need to have packages `git-all`, `cmake` and `build-essential` installed.
```
git clone https://github.com/saucechef/vDISK.git
cd vDISK/
chmod +x build_linux_cli.sh
./build_linux_cli.sh
cd build/
```
You can now use the binary `./vdisk_cli` to start vDISK-cli in the terminal.

## Usage
If you launch the program it will create a virtual drive named PLAYGROUND as example. To get your own project started, you should first create a new virtual drive. You can use the "newdrive" command to create a new virtual drive of any size. With the "format" command you can create a new FAT16 partition with any properties you want.  If you don't specify any properties, the default values will be used. Inside this partition you can now create directories with the "mkdir" command and files with the "wrfile" command. <br>
The navigation within the directories works like in the Linux terminal. To change the directory the "cd" command is used and to list the contents of the directory the "ls" command is used. <br>
The created filesystem can be saved as .img file with the "savedisk" command. It is also possible to import an existing filesystem into vDISK using the "loaddisk" command. <br>
With the "fat16-getinfo" command it is possible to display all available information about the partition. To defragment the partition the "defrag" command is used. To check if a defragmentation is necessary you can use the "getfrag" command to display the fragmentation of the partition in percent. <br>
If you want to display contents in hexadecimal, for example to read the status of a sector, the "hexdump" command can be used. <br>
To list all commands you can simply use the "help" command.


## Commands (for more detail, see manual.md):
#### help
Displays available commands and their usage.
#### hexdump [address] [number of rows]
Example: hexdump 0x800 65536 <br> This would dump the first copy of the FAT provided the formatting parameters are normal.
#### cd <path>
Example: cd /dir1 <br> Change from current working directory into subdirectory "dir1".
#### ls
Lists the content of current working directory.
#### mmkdir <dirname>  
Example: mkdir /dir2 <br>
Creates new directory "dir2" in current working directory.
#### getinfo_fat16
EPrints all available info about the drive and its FAT16 partition. 
#### wrfile <physicalpath> <virtualpath>
Example: wrfile testfile.txt /test/file1.txt <br> Copies physical file "testfile.txt" into "file1.txt" ind directory "test".
#### exfile <virtualpath> <physicalpath>
Example: exfile /test/file1.txt extract.txt  <br> Extracts file and writes it to "extract.txt".
#### rm <virtualpath>
Example: rm /test <br> Removes directory "test" and all its content recursively.
#### getfrag
Gets fragmentation status of virtual FAT16 partition in percent.
#### defrag
Defrags virtual FAT16 partition.
#### savedisk <physicalpath>
Example: savedisk disk2 <br> Saves disk-image of vDrive to "disk2.img" (extension is added automatically).
#### loaddisk <physicalpath>
Example: loaddisk disk1.img <br> Loads disk-image into vDrive (extension is NOT added automatically).
#### format <format> <label> [sectorspercluster] [sectorsperfat]
Example: format fat16 "TESTDRIVE" <br> Will format vDrive with a FAT16 partition with 4 sectors per cluster and 128 sectors per fat.
#### newdrive <size>
Creats a new virtual drive.

