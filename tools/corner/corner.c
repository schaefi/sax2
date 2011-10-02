/**************
FILE          : corner.c
***************
PROJECT       : SaX2 ( SuSE advanced X11 configuration )
              :
DESCRIPTION   : little tool to generate a corner 
              : Window as marker and/or test X11 connection
              : mainly used for testing if we can create
              : a client on the specific display
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

//===================================
// Defines...
//-----------------------------------
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

//===================================
// Functions...
//-----------------------------------
void usage(void);
void setsize (
	XSizeHints *hintp, int min_width, int min_height, 
	int defwidth, int defheight, int defx, int defy, char *geom
);

//===================================
// Globals...
//-----------------------------------
int screen;
Display *dpy;

int main(int argc, char *argv[]) {
	char *displayname = NULL;           /* name of the display    */
	XSetWindowAttributes attr;          /* window attributes      */
	Window w;                           /* the window             */
	Window subw;                        /* the subwindow          */
	unsigned long mask = 0L;            /* pixel mask             */
	XSizeHints hints;                   /* geometry of the window */
	int borderwidth = 1;                /* width of the border    */
	unsigned long back = 128;           /* background dark blue   */
	unsigned long fore = 0;             /* foreground black       */   
	char *name = "Corner";              /* window title string    */
	char *geom = NULL;                  /* geometry string        */
	int i;                              /* loop counter (args)    */
	int test = 0;                       /* test only (X11 conn.)  */

	for (i = 1; i < argc; i++) {
	char *arg = argv[i];
	if (arg[0] == '-') {
		switch (arg[1]) {
		case 'd':  {
			if (++i >= argc) usage();
			displayname = argv[i];
			continue;
		}      
		case 'g': {
			if (++i >= argc) usage();
			geom = argv[i];
			continue;
		}
		case 't': {
			if (++i >= argc) usage();
			test = 1;
			continue;
		}
		case 'h': {
			usage();
		}
		}
	}
	}

	if (geom == NULL) {
		geom = (char*)malloc(20*sizeof(char));
		strcpy(geom,"15x15+0+0");
	}

	// ...
	// handle test case
	// ---
	if (test == 1) {
	if ((dpy = XOpenDisplay (displayname))) {
		XCloseDisplay(dpy);
		printf(":-)\n");
		exit(0);
	} else {
		printf(":-(\n");
		exit(1);
	}
	}

	// ...
	// create window as corner mark and enter the
	// event loop
	// ---
	dpy = XOpenDisplay (displayname);
	if (!dpy) {
	fprintf (
		stderr, "unable to open display %s\n", XDisplayName(displayname)
	);
	exit (1);
	}
	fore = BlackPixel(dpy, DefaultScreen(dpy));
	back = WhitePixel(dpy, DefaultScreen(dpy));
	screen = DefaultScreen (dpy);
	attr.event_mask = ButtonPressMask | ButtonReleaseMask | FocusChangeMask;
	attr.background_pixel = back;
	attr.border_pixel     = fore;
	attr.override_redirect= 1;
	mask |= (
		CWBackPixel | 
		CWBorderPixel | 
		CWEventMask | 
		CWOverrideRedirect
	);
	setsize (
		&hints, OUTER_WINDOW_MIN_WIDTH, OUTER_WINDOW_MIN_HEIGHT,
		OUTER_WINDOW_DEF_WIDTH, OUTER_WINDOW_DEF_HEIGHT,
		OUTER_WINDOW_DEF_X, OUTER_WINDOW_DEF_Y, geom
	);
	w = XCreateWindow (
		dpy, RootWindow (dpy, screen), hints.x, hints.y,
		hints.width, hints.height, borderwidth, 0,
		InputOutput, (Visual *)CopyFromParent,
		mask, &attr
	);
	XSetStandardProperties (
		dpy, w, name, NULL, (Pixmap) 0,argv,argc,&hints
	);
	subw = XCreateSimpleWindow (
		dpy, w, 2, 2,
		hints.width-6, hints.height-6,
		INNER_WINDOW_BORDER,
		attr.border_pixel, attr.background_pixel
	);
	XMapWindow (dpy, subw);
	XMapWindow (dpy, w);

	while(1) {
		XEvent event;
		XNextEvent (dpy, &event);
	}
}

//===================================
// setSize...
//-----------------------------------
void setsize (
	XSizeHints *hintp, int min_width, int min_height, 
	int defwidth, int defheight, int defx, int defy, char *geom
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
		(unsigned int *)&hintp->width,
		(unsigned int *)&hintp->height
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
		hintp->x = DisplayWidth (dpy, DefaultScreen (dpy)) + 
		hintp->x - hintp->width;
	}
	if (geom_result & YNegative) {
		hintp->y = DisplayHeight (dpy, DefaultScreen (dpy)) + 
		hintp->y - hintp->height;
	}
	return;
}

//===================================
// usage...
//-----------------------------------
void usage(void) {
	static char *msg[] = {
		" [ -d < displayname > ]  X server to contact",
		" [ -g < geometry >    ]  size and location of window",
		" [ -t x ]  test X11 connection",
		"",
		NULL
	};
	char **cpp;
	printf ("usage: corner [-options ...]\n");
	printf ("where options include:\n");
	for (cpp = msg; *cpp; cpp++) {
		printf ("%s\n", *cpp);
	}
	exit (1);
}


