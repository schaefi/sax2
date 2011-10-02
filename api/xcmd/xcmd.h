/**************
FILE          : xcmd.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : commandline configuration tool for the X
              : window system released under the GPL license
              :
DESCRIPTION   : SaX2 commandline system using libsax to provide
              : configurations for the most important X11 capabilities
              : //.../
              : - xcmd.cpp: main program start sequence
              : - xcmd.h: buildins and defines
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef XCMDMAIN_H
#define XCMDMAIN_H 1

//====================================
// Includes...
//------------------------------------
#include <qapplication.h>
#include <qfileinfo.h>
#include <getopt.h>

//====================================
// Includes...
//------------------------------------
#include "../../libsax/sax.h"
#include "../common/log.h"

//====================================
// Includes...
//------------------------------------
#include "color.h"
#include "glinfo.h"

//====================================
// Defines...
//------------------------------------
#define XORG_CONF "/etc/X11/xorg.conf"

//===================================
// Prototypes...
//-----------------------------------
void usage ( int );

#endif
