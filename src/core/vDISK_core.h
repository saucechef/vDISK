/*!
 * This file provides the core funtionality of vDISK.
 */

#ifndef VDISK_VDISK_CORE_H
#define VDISK_VDISK_CORE_H
//////////////////////////

#include "vDISK_macros.h"
#include "vDISK_structs.h"

drive* createDrive(uint size);

drive* loadDrive(string path);

void saveDrive();

//////////////////////////
#endif //VDISK_VDISK_CORE_H
