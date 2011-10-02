/**************
FILE          : fashion.c
***************
PROJECT       : SaX2 ( SuSE advanced X11 configuration )
              :
DESCRIPTION   : some functions to generate the background
              : look and feel used for xw.c
              :
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

#include "fashion.h"

//===========================
// Globals...
//---------------------------
char *fore_color = NULL;
char *back_color = NULL;

//===========================
// setCorner...
//---------------------------
void setCorner (char* geom,Display* dpy,int screen) {
	XSetWindowAttributes attr;
	Window w,subw;
	unsigned long mask = 0L;
	XSizeHints hints;
	int borderwidth = 1;
	unsigned long back = 128;
	unsigned long fore = 0;

	fore = BlackPixel(dpy, screen);
	back = WhitePixel(dpy, screen);
	attr.event_mask = ButtonPressMask | ButtonReleaseMask | FocusChangeMask;
 
	attr.background_pixel = back;
	attr.border_pixel     = fore;
	attr.override_redirect= 1;
	mask |= (CWBackPixel | CWBorderPixel | CWEventMask | CWOverrideRedirect);

	setSize (
		&hints, OUTER_WINDOW_MIN_WIDTH, OUTER_WINDOW_MIN_HEIGHT,
		OUTER_WINDOW_DEF_WIDTH, OUTER_WINDOW_DEF_HEIGHT,
		OUTER_WINDOW_DEF_X, OUTER_WINDOW_DEF_Y, geom,dpy,screen
	);
 
	w = XCreateWindow (
		dpy, RootWindow (dpy, screen), hints.x, hints.y,
		hints.width, hints.height, borderwidth, 0,
		InputOutput, (Visual *)CopyFromParent,
		mask, &attr
	);
	subw = XCreateSimpleWindow (
		dpy, w, 2, 2,
		hints.width-6, hints.height-6,
		INNER_WINDOW_BORDER,
		attr.border_pixel, attr.background_pixel
	);
	XMapWindow (dpy, subw);
	XMapWindow (dpy, w);
}

//===========================
// setSize...
//---------------------------
void setSize (
	XSizeHints *hintp, int min_width, int min_height, 
	int defwidth, int defheight, int defx, int defy, char *geom,
	Display* dpy,int screen
) {
	int geom_result;

	/* set the size hints, algorithm from xlib xbiff */
	hintp->width = hintp->min_width = min_width;
	hintp->height = hintp->min_height = min_height;
	hintp->flags = PMinSize;
	hintp->x = hintp->y = 0;
	geom_result = NoValue;
	if (geom != NULL) {
	geom_result = XParseGeometry (
		geom, &hintp->x, &hintp->y, 
		(unsigned int *)&hintp->width, (unsigned int *)&hintp->height
	);
	if ((geom_result & WidthValue) && (geom_result & HeightValue)) {
		#define max(a,b) ((a) > (b) ? (a) : (b))
		hintp->width = max (hintp->width, hintp->min_width);
		hintp->height = max (hintp->height, hintp->min_height);
		hintp->flags |= USSize;
	}
	if ((geom_result & XValue) && (geom_result & YValue)) {
		hintp->flags += USPosition;
	}
	}
	if (!(hintp->flags & USSize)) {
		hintp->width = defwidth;
		hintp->height = defheight;
		hintp->flags |= PSize;
	}
	if (geom_result & XNegative) {
		hintp->x = DisplayWidth (dpy, screen) + 
		hintp->x - hintp->width;
	}
	if (geom_result & YNegative) {
		hintp->y = DisplayHeight (dpy, screen) + 
		hintp->y - hintp->height;
	}
	return;
}

//===========================
// setBounding...
//---------------------------
void setBounding (Display* dpy,int screen) {
	Window root;
	XSetWindowAttributes xswa;
	Window win1,win2,win3,win4;
	unsigned long mask = 0;
	GC mgc;
	Visual vis;
	int x2,y2,depth;
	
	x2    = DisplayWidth  (dpy,screen);
	y2    = DisplayHeight (dpy,screen);
	root  = RootWindow    (dpy,screen);
	mgc   = DefaultGC     (dpy,screen);
	depth = DefaultDepth  (dpy,screen);

	xswa.event_mask        = EnterWindowMask |
		LeaveWindowMask         |
		ExposureMask            |
		VisibilityChangeMask    |
		StructureNotifyMask     |
		SubstructureNotifyMask  |
		SubstructureRedirectMask
	;
	xswa.background_pixmap = None;
	xswa.border_pixel      = WhitePixel(dpy,XDefaultScreen(dpy));
	xswa.background_pixel  = WhitePixel(dpy,XDefaultScreen(dpy));
	xswa.override_redirect = True;
	xswa.backing_store     = NotUseful;
	xswa.save_under        = False;
	vis.visualid           = CopyFromParent;
	mask  |= (
		CWBackPixmap   |
		CWOverrideRedirect |
		CWBackingStore |
		CWSaveUnder |
		CWBackPixel |
		CWBorderPixel
	);

	// create windows (top,bottom,left,right)...
	// ---------------------------------------------
	win1 = XCreateWindow(
		dpy,root,0,0,x2,1,0,depth,InputOutput,&vis,mask,&xswa
	);
	win2 = XCreateWindow(
		dpy,root,0,y2-1,x2,y2,0,depth,InputOutput,&vis,mask,&xswa
	);
	win3 = XCreateWindow(
		dpy,root,0,0,1,y2,0,depth,InputOutput,&vis,mask,&xswa
	);
	win4 = XCreateWindow(
		dpy,root,x2-1,0,x2,y2,0,depth,InputOutput,&vis,mask,&xswa
	);
	XMapWindow(dpy,win1);
	XMapWindow(dpy,win2);
	XMapWindow(dpy,win3);
	XMapWindow(dpy,win4);
	XFlush(dpy);
}

//===========================
// CreateCursorFromName...
//---------------------------
Cursor CreateCursorFromName (Display* dpy,int screen,char *name) {
	XColor fg, bg;
	int     i;
	Font    fid;
	fg = NameToXColor (dpy,screen,fore_color, BlackPixel(dpy, screen));
	bg = NameToXColor (dpy,screen,back_color, WhitePixel(dpy, screen));
	i = XmuCursorNameToIndex (name);
	if (i == -1) {
		return (Cursor) 0;
	}
	fid = XLoadFont (dpy, "cursor");
	if (!fid) {
	    return (Cursor) 0;
	}
	return XCreateGlyphCursor (
		dpy, fid, fid, i, i+1, &fg, &bg
	);
}

//===========================
// NameToXColor...
//---------------------------
XColor NameToXColor (
	Display* dpy,int screen,char *name, unsigned long pixel
) {
	XColor c;
	if (!name || !*name) {
	c.pixel = pixel;
	XQueryColor(
		dpy, DefaultColormap(dpy, screen), &c
	);
	} else if (!XParseColor(dpy, DefaultColormap(dpy, screen), name, &c)) {
	fprintf (stderr,
		"unknown color or bad color format: %s\n",name
	);
	exit(1);
	}
	return(c);
}   

//===========================
// NameToPixel...
//---------------------------
unsigned long NameToPixel (
	char *name, unsigned long pixel,Display* dpy,int screen
) {
	XColor ecolor;
	if (!name || !*name) {
	return pixel;
	}
	if (!XParseColor(dpy,DefaultColormap(dpy,screen),name,&ecolor)) {
		fprintf(stderr,"unknown color: %s\n",name); exit(1);
	}
	if (!XAllocColor(dpy, DefaultColormap(dpy, screen),&ecolor)) {
		fprintf(stderr, "unable to allocate color for: %s\n",name); exit(1);
	}
	return(ecolor.pixel);
}

//===========================
// daemon...
//---------------------------
int daemon(int nochdir, int noclose) {
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
	sleep(1);
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

