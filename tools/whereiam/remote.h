/**************
FILE          : remote.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : check for remote/local login
              : header for remote.c
              :
STATUS        : Status: Up-to-date
**************/
#ifndef REMOTE_H
#define REMOTE_H 1

//=================================
// Functions...
//---------------------------------
int checkRemote( const char* );
int remoteDisplay (char*);

#endif
