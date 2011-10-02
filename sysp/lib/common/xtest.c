/**************
FILE          : xtest.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : common used functions: 
              : xtest -> get next free display number
              :
STATUS        : development
**************/
#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>
#include <unistd.h>

#include "../syslib.h"

//===================================
// GetDisplay: get next unused dpy
//-----------------------------------
int GetDisplay (void) {
	Display *dpy;
	char displayname[80] = "";
	char data[20] = "/tmp/disp";
	FILE *fd;
	int size;
	int i;

	sprintf (data,"%s-%d",data,getpid());

	for (i=0;i<=10;i++) {
		fd = freopen(data,"w",stderr);
		sprintf(displayname,":%d.0",i);
		dpy = XOpenDisplay (displayname);

		fseek (fd,0L,SEEK_END);
		size = ftell(fd) - 1;

		//printf("size: %d -> %d\n",i,size);

		if (!dpy) {
			if (size <= 0) {
				unlink(data); return(i);
			}
		} else {
			XCloseDisplay(dpy);
			continue;
		}
	}
	unlink (data);
	return -1;
}
