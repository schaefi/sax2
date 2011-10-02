/**************
FILE          : xaccess.h
***************
PROJECT       : SaX2 ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
DESCRIPTION   : header file for XAccess...
              : 
              : 
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#ifndef XACCESS_H
#define XACCESS_H 1

//===========================
// Prototypes for xaccess.h
//---------------------------
Display* XOpen (char name[256]);
void XAccess (Display *dpy,char *displayname);

#endif
