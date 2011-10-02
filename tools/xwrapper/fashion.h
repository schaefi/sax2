/**************
FILE          : fashion.h
***************
PROJECT       : SaX2 ( SuSE advanced X configuration )
              :
DESCRIPTION   : header file for fashion.c
              : set up defines and function prototypes
              : 
STATUS        : Status: Up-to-date
**************/
#ifndef FASHION_H
#define FASHION_H 1

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/Xinerama.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CurUtil.h>

//===========================
// Defines...
//--------------------------- 
#define Dynamic 1
#define INNER_WINDOW_WIDTH  1
#define INNER_WINDOW_HEIGHT 1
#define INNER_WINDOW_BORDER 1
#define INNER_WINDOW_X      0
#define INNER_WINDOW_Y      0
 
#define OUTER_WINDOW_MIN_WIDTH (INNER_WINDOW_WIDTH + \
                2 * (INNER_WINDOW_BORDER + INNER_WINDOW_X))
#define OUTER_WINDOW_MIN_HEIGHT (INNER_WINDOW_HEIGHT + \
                2 * (INNER_WINDOW_BORDER + INNER_WINDOW_Y))
#define OUTER_WINDOW_DEF_WIDTH (OUTER_WINDOW_MIN_WIDTH + 100)
#define OUTER_WINDOW_DEF_HEIGHT (OUTER_WINDOW_MIN_HEIGHT + 100)
#define OUTER_WINDOW_DEF_X  0
#define OUTER_WINDOW_DEF_Y  0
#define PATH_DEV_NULL "/dev/null"

//===========================
// Prototypes...
//---------------------------  
void setSize (
	XSizeHints *hintp, int min_width, int min_height, 
	int defwidth, int defheight, int defx, int defy, char *geom,
	Display* dpy,int screen
);
void setCorner (char* geom,Display* dpy,int screen);
unsigned long NameToPixel (
	char *name, unsigned long pixel,Display* dpy,int screen
);
int daemon(int nochdir, int noclose);
void setBounding (Display* dpy,int screen);
Cursor CreateCursorFromName (Display* dpy,int screen,char *name);
XColor NameToXColor (Display* dpy,int screen,char *name, unsigned long pixel);

#endif
