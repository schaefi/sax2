/**************
FILE          : qx.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : header for qx.h
              :
STATUS        : Status: Up-to-date
**************/
#ifndef QX_H
#define QX_H 1

//=================================
// Defines...
//---------------------------------
#define MAX_PROGRAM_SIZE 256

//=================================
// Functions...
//---------------------------------
char* qx (const char*command,int channel,int anz=0,const char* format=NULL,...);
void handlesigchld (int);

#endif
