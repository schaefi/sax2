/**************
FILE          : color.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : commandline configuration tool for the X
              : window system released under the GPL license
              :
DESCRIPTION   : SaX2 commandline system using libsax to provide
              : functions for setting the default color depth
              : //.../
              : - color.cpp: set default color depth
              : - color.h: header file for color.cpp
              : ----
              :
STATUS        : Status: Development 
**************/
#ifndef CAPICOLOR
#define CAPICOLOR 1

#include <q3dict.h>

#include "xcmd.h"

//=====================================
// Functions...
//-------------------------------------
bool setDefaultColorDepth ( Q3Dict<SaXImport>,int,int );

#endif
