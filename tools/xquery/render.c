/**************
FILE          : render.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : provide function to print a string to the
              : root window which will fade out using the
              : render extension
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xinerama.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>

#include "render.h"

//====================================
// showRenderText...
//------------------------------------
void showRenderText (char* param,Display* dpy,int screen) {
	char *mtext = NULL;
	char *text  = NULL;
	char *token = NULL;
	XInit xi;
	XTFont xt;
	Window win;
	Color color;
	XTColor xc;
	long i;
	setlocale (LC_ALL, "");
	bindtextdomain ("sax", "/usr/share/locale");
	textdomain ("sax");
	bind_textdomain_codeset ("sax","UTF-8");
	text = (char*)malloc(1024);
	token = strtok(param,",");
	mtext = gettext (token);
	sprintf (text,"%s",mtext);
	while (1) {
		token = strtok (NULL,",");
		if (! token) {
			break;
		}
		mtext = gettext (token);
		sprintf (text,"%s %s",text,mtext);
	}

	xi  = XInitDisplay (dpy,screen);
	xt  = XFontSetup   (xi,text);
	win = XWindowSetup (xi,xt);

	color.red   = 255;
	color.blue  = 255;
	color.green = 255;
	xc = XColorSetup (
		xi,xt,win,color
	);
	i = MASK;
	while(1) {
		if (i <= 0) { break; }
		XCopyArea (
			xi.dpy,xc.PAint,xc.PBack,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
		);
		xc.FTcolor->color.alpha = i;
		xc.FTcolor->color.red   = (xc.color.red   * i) / MASK;
		xc.FTcolor->color.green = (xc.color.green * i) / MASK;
		xc.FTcolor->color.blue  = (xc.color.blue  * i) / MASK;
    
		XftDrawStringUtf8 (
			xc.FTdraw, xc.FTcolor,
			xt.FTfont, 0, (int)((2 / 3.0) * xt.XTheight),
			(unsigned char*)text, strlen(text)
		);
		XCopyArea (
			xi.dpy,xc.PBack,win,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
		);
		XFlush (xi.dpy);
		if (i == MASK) {
			usleep(500000);
		} else {
			usleep (0);
		}
		i-=500;
	}
}

//====================================
// XInitDisplay...
//------------------------------------
XInit XInitDisplay (Display *dpy,int screen) {
	XInit xi;
	xi.dpy     = dpy;
	xi.DScreen = screen;
	if (XineramaIsActive (dpy)) {
		int s_num;
		XineramaScreenInfo *screens;
		screens = XineramaQueryScreens (dpy, &s_num);
		xi.DWidth  = screens[screen].width / 2;
		xi.DWidth  = xi.DWidth + screens[screen].x_org;
		xi.DHeight = screens[screen].height / 2;
		xi.DHeight = xi.DHeight + screens[screen].y_org;
		xi.DScreen = 0;
	} else {
		xi.DWidth  = DisplayWidth  (xi.dpy,xi.DScreen) / 2;
		xi.DHeight = DisplayHeight (xi.dpy,xi.DScreen) / 2;
	}
	xi.DMap    = DefaultColormap(xi.dpy,xi.DScreen);
	xi.DWin    = RootWindow (xi.dpy,xi.DScreen);
	xi.DGc     = DefaultGC  (xi.dpy,xi.DScreen);
	xi.DDepth  = DefaultDepth(xi.dpy,xi.DScreen);
	xi.DVisual = DefaultVisual(xi.dpy,xi.DScreen);
	return(xi);
}

//====================================
// XColorSetup...
//------------------------------------
XTColor XColorSetup(XInit xi,XTFont xt,Window win,Color col) {
	XGCValues values;
	XTColor xc;
	xc.FTcolor = (XftColor*)malloc(sizeof(XftColor));
	xc.PBack = XCreatePixmap (
		xi.dpy,win,xt.XTwidth,xt.XTheight,xi.DDepth
	);
	xc.PAint = XCreatePixmap (
		xi.dpy,win,xt.XTwidth,xt.XTheight,xi.DDepth
	);
	XCopyArea (
		xi.dpy,win,xc.PAint,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
	);
	xc.color.red   = ((col.red   << 8) | col.red );
	xc.color.green = ((col.green << 8) | col.green);
	xc.color.blue  = ((col.blue  << 8) | col.blue );
	XAllocColor(xi.dpy,xi.DMap,&xc.color);
	XSetForeground (
		xi.dpy,xi.DGc,xc.color.pixel
	);
	XGetGCValues (xi.dpy, xi.DGc, GCForeground|GCBackground, &values);
	xc.FTdraw = XftDrawCreate (
		xi.dpy, xc.PBack, xi.DVisual, xi.DMap
	);
	xc.FTcolor -> color.red   = xc.color.red;
	xc.FTcolor -> color.green = xc.color.green;
	xc.FTcolor -> color.blue  = xc.color.blue;
	return(xc);
}

//====================================
// XWindowSetup...
//------------------------------------
Window XWindowSetup (XInit xi,XTFont xt) {
	XSetWindowAttributes xswa;
	unsigned long mask = 0;
	Visual DVisual;
	Window win;
	XImage* img;

	img = XGetImage(
		xi.dpy,RootWindow(xi.dpy,xi.DScreen),
		xi.DWidth - xt.XTwidth / 2, xi.DHeight - xt.XTheight / 2,
		xt.XTwidth,xt.XTheight,AllPlanes,XYPixmap
	);
	xswa.background_pixmap = None;
	xswa.override_redirect = True;
	DVisual.visualid       = CopyFromParent;
	mask  |= (
		CWBackPixmap |
		CWOverrideRedirect
	);
	win = XCreateWindow(
		xi.dpy, xi.DWin,
		(int)(xi.DWidth  - (xt.XTwidth  / 2)),
		(int)(xi.DHeight - (xt.XTheight / 2)),
		xt.XTwidth, xt.XTheight, 0,
		DefaultDepth (xi.dpy, xi.DScreen),
		InputOutput,
		&DVisual,
		mask,
		&xswa
	);
	XMapWindow (xi.dpy, win);
	XPutImage (
		xi.dpy,win,xi.DGc,img,0,0,0,0,xt.XTwidth,xt.XTheight
	);
	return(win);
}

//====================================
// XFontSetup...
//------------------------------------
XTFont XFontSetup (XInit xi,char *text) {
	XftPattern *pattern;
	XGlyphInfo FTinfo;
	XTFont xt;
	pattern = XftNameParse (XFTFONT);
	pattern = XftFontMatch (xi.dpy,xi.DScreen,pattern,NULL);
	xt.FTfont = XftFontOpenPattern (xi.dpy, pattern);
	XftTextExtentsUtf8 (
		xi.dpy,xt.FTfont,(XftChar8*)text,strlen(text),&FTinfo
	);
	xt.XTheight = xt.FTfont->height;
	xt.XTwidth  = FTinfo.xOff;
	return(xt);
}

//====================================
// go to background...
//------------------------------------
int daemon (int nochdir, int noclose) {
	int fd;

	switch (fork()) {
	case -1:
		return (-1);
	case 0:
	//==================================
	// we are the child... leave case
	//----------------------------------
	break;

	default:
	//==================================
	// we are the parent process.. exit
	//----------------------------------
	_exit(0);
	}
	if (setsid() == -1) {
		return (-1);
	}
	if (!nochdir) {
		(void)chdir("/");
	}
	if (!noclose && (fd = open(PATH_DEV_NULL, O_RDWR, 0)) != -1) {
		(void)dup2(fd, STDIN_FILENO);
		(void)dup2(fd, STDOUT_FILENO);
		(void)dup2(fd, STDERR_FILENO);
		if (fd > 2) {
			(void)close (fd);
		}
	}
	return (0);
}

