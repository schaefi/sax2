/**************
FILE          : xlook.h
***************
PROJECT       : SaX2 ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
DESCRIPTION   : xlook use the XRender extension to display
              : a string which will fade out after some
              : seconds
              :
              : [ header file ]
              : 
STATUS        : Status: Up-to-date
**************/
#ifndef XLOOK_H
#define XLOOK_H 1

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <X11/Xft/Xft.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>

//===========================
// Defines...
//---------------------------
#define PATH_DEV_NULL "/dev/null"
#define XFTFONT       "serif-30"
#define MASK          0xffff

//==============================
// Types...
//------------------------------
typedef unsigned long Pixel;

typedef struct {
	Display *dpy;
	Visual* DVisual;
	Window DWin;
	Colormap DMap;
	GC DGc;
	int DWidth;
	int DHeight;
	int DScreen;
	int DDepth;
} XInit; 
 
typedef struct {
	XftFont* FTfont;
	int XTwidth;
	int XTheight;
} XTFont;
 
typedef struct {
	XftDraw* FTdraw;
	XftColor* FTcolor;
	XColor color;
	Pixmap PBack;
	Pixmap PAint; 
	int red;
	int green;
	int blue;
} XTColor;

typedef struct {
	int red;
	int green;
	int blue;
} Color;

//===========================
// Prototypes...
//---------------------------
int XTestRender (XInit);
XInit XInitDisplay (char*);
XTFont XFontSetup (XInit,char*);
Window XWindowSetup (XInit,XTFont);
XTColor XColorSetup (XInit,XTFont,Window,Color);
int xerror (Display*, XErrorEvent*);
void usage (void);
int  daemon (int,int);

#endif
