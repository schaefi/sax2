/**************
FILE          : hwdata.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide header to include the syslib.h 
              : and all other related headers
              :
              :
STATUS        : development
**************/
#ifndef HWDATA_H
#define HWDATA_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hd.h"
#include "../syslib.h"
#include "../../config.h"

//===================================
// Defines...
//-----------------------------------
#define ID_VALUE(id) ((id) & 0xffff)

//===================================
// Prototypes...
//-----------------------------------
extern "C" char *vend_id2str(unsigned vend);

KbdData* KeyboardGetData (void);
MouseData* MouseGetData (void);
int TvSupport (void);
char* vesaBIOS (void);
char* mediaDevices (void);
MsgDetect* MonitorGetData (void);
FbBootData* FrameBufferGetData (void);
unsigned long MemorySize (void);

#endif
