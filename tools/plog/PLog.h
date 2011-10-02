/**************
FILE          : PLog.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : PLog.pm perl module for parsing X11
              : log files
              :
              :
STATUS        : development
**************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "../../sysp/lib/syslib.h"


//========================================
// Parse and save data...
//----------------------------------------
extern MsgDetect* PLogParse (char* logfile);

//========================================
// Extract data from MsgDetect struct...
//----------------------------------------
extern char* PLogGetResolution (MsgDetect* pm);
extern char* PLogGetVideoRam   (MsgDetect* pm);
extern char* PLogGetMonitorManufacturer (MsgDetect* pm);
extern char* PLogCheckDisplay (MsgDetect* pm);
extern char* PLogGetVMwareColorDepth (MsgDetect* pm);
extern char* PLogGetDisplaySize (MsgDetect* pm);
