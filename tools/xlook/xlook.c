/**************
FILE          : xlook.c
***************
PROJECT       : SaX2 ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
DESCRIPTION   : xlook use the XRender extension to display
              : a string which will fade out after some
              : seconds
              : 
STATUS        : Status: Up-to-date
**************/
#include "xlook.h"

//=====================================
// Globals...
//-------------------------------------
int NoFade     = 0;
int HasRender  = 1;
XImage* img = NULL;

//=====================================
// Main...
//-------------------------------------
int main(int argc,char *argv[]) {
	char *displayname = NULL;
	char *mtext = NULL;
	char *param = NULL;
	char *text  = NULL;
	char *token = NULL;
	Window win;
	XInit  xi;
	XTFont xt;
	XTColor xc;
	int c;
	long i;

	//=====================================
	// get options
	//------------------------------------- 
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"display"    , 1 , 0 , 'd'},
		{"string"     , 1 , 0 , 's'},
		{"nofade"     , 0 , 0 , 'n'},
		{"help"       , 0 , 0 , 'h'},
		{0            , 0 , 0 , 0  }
	};
	c = getopt_long (
		argc, argv, "hd:s:n",long_options, &option_index
	);
	if (c == -1)
	break;

	switch (c) {
	case 0:
	break;

	case 'h':
		usage();

	case 'n':
		NoFade = 1;
	break;

	case 's':
		param = (char*)malloc(strlen(optarg)+1);
		strcpy (param,optarg);
	break;

	case 'd':
		displayname = (char*)malloc(80*sizeof(char));
		strcpy (displayname,optarg);
	break;

	default:
		usage();
		exit(1);
	}
	}

	//=====================================
	// check text contents
	//-------------------------------------
	if (! param) {
		param = (char*)malloc(6);
		strcpy(param,"XLook");
	}
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

	//=====================================
	// open and init display...
	// ------------------------------------
	xi  = XInitDisplay(displayname);
	HasRender = XTestRender (xi);
	if (! HasRender) {
		XCloseDisplay(xi.dpy);
		exit (1);
	}
	daemon(1,1);

	//======================================
	// setup font, create and map window...
	// -------------------------------------
	Color color;
	xt  = XFontSetup   (xi,text);
	win = XWindowSetup (xi,xt);
	color.red   = 255;
	color.blue  = 255;
	color.green = 255;
	xc  = XColorSetup (xi,xt,win,color);
	xc.FTcolor->color.alpha = 0;

	if (NoFade) {
	//=======================================
	// Just display the string
	//---------------------------------------
	XSetErrorHandler(xerror);
	i = MASK;
	xc.FTcolor->color.alpha = i;
	xc.FTcolor->color.red   = (xc.color.red   * i) / MASK;
	xc.FTcolor->color.green = (xc.color.green * i) / MASK;
	xc.FTcolor->color.blue  = (xc.color.blue  * i) / MASK;
	XftDrawStringUtf8 (
		xc.FTdraw, xc.FTcolor,
		xt.FTfont, 0, (2 / 3.0) * xt.XTheight,
		(unsigned char*)text, strlen(text)
	);
	XCopyArea(
		xi.dpy,xc.PAint,win,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
	);
	XFlush (xi.dpy);
	while (1) {
		sleep (100);
	}
	} else {
	//=======================================
	// Display the string and fade away
	//---------------------------------------
	i = MASK;
	while(i > 0) {
		XCopyArea(
			xi.dpy,xc.PBack,xc.PAint,
			xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
		);
		xc.FTcolor->color.alpha = i;
		xc.FTcolor->color.red   = (xc.color.red   * i) / MASK;
		xc.FTcolor->color.green = (xc.color.green * i) / MASK;
		xc.FTcolor->color.blue  = (xc.color.blue  * i) / MASK;
		XftDrawStringUtf8 (
			xc.FTdraw, xc.FTcolor,
			xt.FTfont, 0, (2 / 3.0) * xt.XTheight,
			(unsigned char*)text,strlen (text)
        );
		XCopyArea(
			xi.dpy,xc.PAint,win,
			xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
		);
		XFlush (xi.dpy);
		if (i == MASK) {
			usleep(0.5e6);
		} else {
			usleep (0);
		}
		i-=500;
	}
	}
	XCloseDisplay(xi.dpy);
	exit (0);
}


//=========================================
// XColorSetup
//-----------------------------------------
XTColor XColorSetup (XInit xi,XTFont xt,Window win,Color col) {
	// ...
	// Create two pixmaps initially containing the background
	// of the window win and use them for drawing with double
	// buffering to avoid flickering while the string fade out
	// Create a XFT drawable needed to print text using xft
	// ---
	XGCValues values;
	XTColor xc;

	xc.FTcolor = (XftColor*)malloc(sizeof(XftColor));
	xc.PBack = XCreatePixmap (xi.dpy,win,xt.XTwidth,xt.XTheight,xi.DDepth);
	xc.PAint = XCreatePixmap (xi.dpy,win,xt.XTwidth,xt.XTheight,xi.DDepth);
	XCopyArea(
		xi.dpy,win,xc.PAint,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
	);
	XCopyArea(
		xi.dpy,win,xc.PBack,xi.DGc,0,0,xt.XTwidth,xt.XTheight,0,0
	);
	xc.color.red   = ((col.red   << 8) | col.red );
	xc.color.green = ((col.green << 8) | col.green);
	xc.color.blue  = ((col.blue  << 8) | col.blue );
	XAllocColor(xi.dpy,xi.DMap,&xc.color);
	XSetForeground(xi.dpy,xi.DGc,xc.color.pixel);

	XGetGCValues (xi.dpy, xi.DGc, GCForeground|GCBackground, &values);
	xc.FTdraw = XftDrawCreate (
		xi.dpy, xc.PAint, xi.DVisual, xi.DMap
	);
	xc.FTcolor->color.red   = xc.color.red;
	xc.FTcolor->color.green = xc.color.green;
	xc.FTcolor->color.blue  = xc.color.blue;
	return(xc);
}

//=========================================
// XWindowSetup
//-----------------------------------------
Window XWindowSetup (XInit xi,XTFont xt) {
	// ...
	// Create a window with the correct size according
	// to the text displayed later in this window. Do
	// not allow any decorations on this window and
	// put the back view area as pixel into the window
	// background
	// ---
	XSetWindowAttributes xswa;
	unsigned long mask = 0;
	Visual DVisual;
	Window win;
	//XImage* img;
	int x1,y1;

	x1 = (int)((xi.DWidth  / 2) - (xt.XTwidth  / 2));
	y1 = (int)((xi.DHeight / 2) - (xt.XTheight / 2));
	img = XGetImage(
		xi.dpy,RootWindow(xi.dpy,xi.DScreen),
		x1, y1, xt.XTwidth,xt.XTheight,
		AllPlanes,XYPixmap
	);

	xswa.background_pixmap = None;
	xswa.override_redirect = True;
	DVisual.visualid       = CopyFromParent;
	mask  |= (
		CWBackPixmap   | 
		CWOverrideRedirect
	);
	win = XCreateWindow(
		xi.dpy, xi.DWin,x1,y1,
		xt.XTwidth, xt.XTheight, 0, 
		DefaultDepth(xi.dpy, xi.DScreen), 
		InputOutput, &DVisual, 
		mask,&xswa
	);
	XMapWindow (xi.dpy, win);
	XPutImage (
		xi.dpy,win,xi.DGc,img,0,0,0,0,xt.XTwidth,xt.XTheight
	);
	return(win);
}

//=========================================
// XFontSetup
//-----------------------------------------
XTFont XFontSetup (XInit xi,char *text) {
	// ...
	// Open the defined XFTFONT and calculate the
	// pixel width and height needed for text with the
	// given font
	// ---
	XftPattern *pattern;
	XGlyphInfo FTinfo;
	XTFont xt;

	pattern = XftNameParse (XFTFONT);
	pattern = XftFontMatch (xi.dpy,xi.DScreen,pattern,NULL);
	xt.FTfont = XftFontOpenPattern (xi.dpy, pattern);
	XftTextExtentsUtf8 (
		xi.dpy,xt.FTfont,(unsigned char*)text,strlen(text),&FTinfo
	);
	xt.XTheight = xt.FTfont->height;
	xt.XTwidth  = FTinfo.xOff;
	return(xt);
}

//=========================================
// XInitDisplay
//-----------------------------------------
XInit XInitDisplay (char *displayname) {
	// ...
	// Open the display and save geometry data and
	// display information about colors, screen, etc.
	// ---
	XInit xi;
	xi.dpy = XOpenDisplay (displayname);
	if (!xi.dpy) {
		fprintf (stderr,
			"xlook: unable to open display %s\n",
			XDisplayName(displayname)
		);
		exit (1);
	}
	xi.DScreen = XDefaultScreen(xi.dpy);
	xi.DMap    = DefaultColormap(xi.dpy,xi.DScreen);
	xi.DWin    = RootWindow (xi.dpy,XDefaultScreen(xi.dpy));
	xi.DGc     = DefaultGC  (xi.dpy,XDefaultScreen(xi.dpy));
	xi.DWidth  = DisplayWidth (xi.dpy,XDefaultScreen(xi.dpy));
	xi.DHeight = DisplayHeight (xi.dpy,XDefaultScreen(xi.dpy));
	xi.DDepth  = DefaultDepth(xi.dpy,xi.DScreen);
	xi.DVisual = DefaultVisual(xi.dpy,xi.DScreen);
	return(xi);
}

//=========================================
// daemon
//-----------------------------------------
int daemon(int nochdir, int noclose) {
	// ...
	// put the current process into the background
	// duplicate std descriptors and close them on
	// parameter.
	int fd;

	switch (fork()) {
	case -1:
		return (-1);
	case 0:
		// we are the child... leave case
	break;

	default:
		//we are the parent process.. exit
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

//=========================================
// usage
//-----------------------------------------
int XTestRender (XInit xi) {
	// ...
	// test if the render extension is available on the
	// currently used display
	// ---
	int n = 0; 
	int i = 0;
	char **extlist = XListExtensions (xi.dpy, &n);
	for (i = 0; i < n; i++) {
	if (strcmp(extlist[i],"RENDER") == 0) {
		return(1);
	}
	}
	return(0);
}

//=========================================
// usage
//-----------------------------------------
void usage(void) {
	printf ("usage: xlook [ options ]\n");
	printf ("options:\n");
	printf ("  [ -d | --display ]\n");
	printf ("    set display to use\n");
	printf ("  [ -s | --string ]\n");
	printf ("    set the string to display\n");
	printf ("  [ -n | --nofade ]\n");
	printf ("    don't fade out the displayed string");
	printf ("  [ -h | --help ]\n");
	printf ("    you already got it\n");
	exit(1 );
}

//=========================================
// xerror
//-----------------------------------------
int xerror (Display* dpy, XErrorEvent* e) {
	XCloseDisplay (dpy);
	exit (1);
}

