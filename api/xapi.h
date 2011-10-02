/**************
FILE          : xapi.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 GUI system using libsax to provide
              : configurations for a wide range of X11 capabilities
              : //.../
              : - xapi.h: buildins and templates
              : - xapi.cpp: main program start sequence
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMAIN_H
#define SCCMAIN_H 1

//====================================
// Includes...
//------------------------------------
#include <qapplication.h>
#include <getopt.h>
#include <signal.h>

//====================================
// Includes...
//------------------------------------
#include "config.h"
#include "frame.h"
#include "profile.h"
#include "common/log.h"

//====================================
// Defines...
//------------------------------------
#define XCMD "/usr/sbin/xcmd"

//===================================
// Prototypes...
//-----------------------------------
void gotInterrupted (int);
void usage ( int );

#endif
