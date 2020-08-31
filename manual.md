# Manual for command line interface
This file provides a more detailed explaination for every command.  
Legend: `command <necessary parameter> [optional parameter]`

# Help and info

## file-getinfo
`file-getinfo <file path>`  
Use this command to retrieve information about a file.
#### Examples
`file-getinfo dir1/dir2/file1.dat` Prints info of file1.dat.

## fat16-getinfo
`fat16-getinfo`  
Use this command to retrieve info about the drive and its FAT16 partition, if it has one.

## getfrag
`getfrag`  
Will give information about the fragmentation status of the vDrive. See `defrag`.

## help
`help`  
Use this command to display information about all commands.

## hexdump
`hexdump [address] [number of rows]`  
A hexdump is a simple but powerful tool to gain insight into a data structure.
#### Examples
`hexdump` Will print a hexdump of the whole vDrive.  
`hexdump 0x800 65536` Will print the first copy of the FAT16, 
provided the drive is formatted with usual parameters.

## quit
`quit`  
Use this to quit the program.

# Navigating the filesystem

## cd
`cd <directory>`  
Use this command to change the working directory.  
The current working directory path is shown on the left side of the input line.
#### Examples
`cd dir1` Go into dir1.  
`cd dir1/dir2` Go past dir1, straight into dir2.  
`cd /` Go back to root folder.  
`cd ..` Does not work yet!

## ls
`ls`  
Use this command to list the content of the current working directory.

# Making modifications to files and directories

## cp
`cp <source file path> <destination file path>`  
Use this command to copy a file to another location.
#### Examples
`cp file1.txt file2.txt` Will create a copy of file1.txt under name file2.txt in the same directory.  
`cp file1.txt dir/file1.txt` Will create a copy of file1.txt under the same name in subdirectory dir.

## exfile
`exfile <file on vDrive> <physical destination>`  
Use this command to extract a file from the vDrive and write it to the physical disk.  
This is the counterpart to `wrfile`.  
#### Examples
`exfile dir/virtualfile.txt extractedfile.txt` Will extract the virtual file and write it to extractedfile.txt

## defrag
`defrag`  
Will attempt to defragment the drive. See `getfrag`.

## mkdir
`mkdir <directory>`  
Use this command to create a new subdirectory within the current working directory.
#### Examples
`mkdir dir1` Creates the new subdirectory dir1.

## mv
`mv <source file path> <destination file path>`  
Use this command to move or rename a file. It works like `cp`, but also deletes the source file.

## rm
`rm <file to remove>`  
Removes a file from the vDrive.  
It will not overwrite its data, just remove it from the FAT and clear folder entries.  
Warning: it does not remove files recursively!
#### Examples
`rm dir/file1.txt` Removes file1.txt from subdirectory dir.
`rm dir` Removes directory dir.

## wrfile
`wrfile <physical file> <destination path on vDrive>`  
Use this command to write a file from your physical disk to the vDrive.  
This is the counterpart to `exfile`.
#### Examples
`wrfile testfile.txt testfile.txt` Will write the file testfile.txt, which is in the same folder
as the executable on your physical disk, to the current working directory of the vDrive.  
`wrfile testfiles/file1.dat dir/subdir/test.txt` Also works with subdirectories.

# Manage vDrive and partition

## format
`format <format> <label> [sectors per cluster] [sectors per FAT]`  
Will format the vDrive with given parameters. Only supported format at the moment is "fat16".  
Default value sectors per cluster is 4, for sectors per FAT 128.  
Do this after using `newdrive`.
#### Examples
`format fat16 PLAYGROUND` Will format the vDrive FAT16 with label "PLAYGROUND".  
`format fat16 PLAYGROUND 2 64` This time with additional parameters.

## loaddisk
`loaddist <physical image file>`  
Will make an attempt to load the vDrive from an image file and also initialise it as FAT16.  
File extension is NOT appended automatically!
#### Examples
`loaddisk disk1.img` Will try to load image from file disk1.img.

## newdrive
`newdrive <size in bytes>`  
Will dump the vDrive which is held currently and initilise a new one with given size.  
Do this before formatting.
#### Examples
`newdrive 16777216` Will initialise a new drive with 16MiB of space.

## savedisk
`savedisk <physical name>`  
Will dump an image file of the vDrive at its current state to the given path/name.  
File extension will be appended automatically!
#### Examples
`savedisk disk1` Will save image to file disk1.img in the directory of the executable.  
`savedisk disks/disk2` Will also work. 