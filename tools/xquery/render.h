/**************
FILE          : render.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : header for render.cpp
              :
STATUS        : Status: Up-to-date
**************/
#ifndef RENDER_H
#define RENDER_H 1

#include <X11/Xft/Xft.h>
// ========================
// Defines...
// ------------------------
#define XFTFONT       "serif-30"
#define MASK           0xffff
#define PATH_DEV_NULL  "/dev/null"

//=================================
// Structs...
//---------------------------------
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

//=================================
// Prototypes...
//---------------------------------
void showRenderText (char* text,Display* dpy,int screen);
int daemon (int,int);
XInit XInitDisplay (Display *dpy,int screen);
XTColor XColorSetup(XInit xi,XTFont xt,Window win,Color col);
Window XWindowSetup (XInit xi,XTFont xt);
XTFont XFontSetup (XInit xi,char *text);

#endif
