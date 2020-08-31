# vDISK
vDISK is a student project that simulates a virtual FAT16 filesystem.  
Core features are:  
* Initialising and managing a virtual harddrive in RAM.
* Formatting and using the virtual harddrive with a FAT16 filesystem.
* Interfacing with the physical drive by file IO.
* Saving and loading the virtual harddrive from image files on physical medium.

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
