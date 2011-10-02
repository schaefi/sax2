/**************
FILE          : xquery.c
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
              :
BESCHREIBUNG  : xquery delivers some information about
              : all connected displays
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <getopt.h>
#include <stdarg.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdSel.h>
#include <X11/Xmd.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xinerama.h>

#include "xquery.h"
#include "nstring.h"
#include "render.h"

//===============================================
// main...
//-----------------------------------------------
int main (int argc, char*argv[]) {
	int DEBUG = FALSE;
	Display* dpy;
	int  c;

	int  screen     = FALSE;
	int  adjustable = FALSE;
	int  resolution = FALSE;
	int  xinerama   = FALSE;
	int  applymode  = FALSE;
	int  modeline   = FALSE;
	int  sync       = FALSE;
	int  nextmode   = FALSE;
	int  prevmode   = FALSE;
	int  color      = FALSE;
	int  freq       = FALSE;
	int  active     = FALSE;
	int  middle     = FALSE;
	int  render     = FALSE;

	int  userscreen    = 0;
	int  count         = 0;
	char mydisp  [256] = "";
	char timing  [256] = "";
	char rstring [256] = "";

	//=============================
	// get commandline options...
	//-----------------------------
	if (argc == 1) {
		usage();
	}
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"screen"     , 0 , 0 , 's'},
		{"applymode"  , 1 , 0 , 'a'},
		{"modeline"   , 0 , 0 , 'm'},
		{"resolution" , 0 , 0 , 'r'},
		{"xinerama"   , 0 , 0 , 'x'},
		{"adjustable" , 0 , 0 , 'j'},
		{"sync"       , 0 , 0 , 'y'},
		{"nextmode"   , 1 , 0 , 'n'},
		{"prevmode"   , 1 , 0 , 'p'},
		{"display"    , 1 , 0 , 'd'},
		{"color"      , 0 , 0 , 'c'},
		{"currentscr" , 0 , 0 , 'S'},
		{"render"     , 1 , 0 , 'R'},
		{"freq"       , 0 , 0 , 'f'},
		{"middle"     , 0 , 0 , 'M'},
		{"help"       , 0 , 0 , 'h'},
		{0            , 0 , 0 , 0  }
	};

	c = getopt_long (
		argc,argv,"hsmryn:p:cfa:d:xSMR:j",long_options, &option_index
	);
	if (c == -1)
	break;

	switch (c) {
	case 0:
	if (DEBUG) {
		fprintf (stderr,"option %s", long_options[option_index].name);
		if (optarg)
			fprintf (stderr," with arg %s", optarg);
		fprintf (stderr,"\n");
	}
	break;

	case 'h':
	usage();

	case 's':
	screen     = TRUE;
	break;

	case 'j':
	adjustable = TRUE;
	break;

	case 'S':
	active     = TRUE;
	break;

	case 'M':
	middle     = TRUE;
	break;

	case 'x':
	xinerama   = TRUE;
	break;

	case 'm':
	modeline   = TRUE;
	break;

	case 'r':
	resolution = TRUE;
	break;

	case 'R':
	render     = TRUE;
	strcpy (rstring,optarg);
	break;

	case 'a':
	applymode  = TRUE;
	strcpy (timing,optarg);
	break;

	case 'd':
	strcpy(mydisp,optarg);
	break;

	case 'n':
	nextmode   = TRUE;
	userscreen = atoi(optarg);
	break;

	case 'p':
	prevmode   = TRUE;
	userscreen = atoi(optarg);
	break;

	case 'y':
	sync = TRUE;
	break;

	case 'c':
	color = TRUE;
	break;

	case 'f':
	freq = TRUE;
	break;

	default:
	usage();
	}
	}

	//==================================
	// if render call fork()...
	//----------------------------------
	if (render) {
		daemon (0,0);
	}

	//==================================
	// open display check and close...
	//----------------------------------
	if (strcmp(mydisp,"") == 0) {
		dpy = XOpen ("null",0);
	} else {
		dpy = XOpen (mydisp,0);
	}
	if (!dpy) {
		printf ("xquery: could not init display\n");
		exit (1);
	}
	CountScreens(dpy);

	for (count = 0;count < scr_count; count++) {
	int current_screen = scr[count];
	//=================================
	// Number of Screens...
	//---------------------------------
	if (screen == TRUE) {
		printf("%d\n",scr_count);
		break;
	}

	//=================================
    // Current active screen...
    //---------------------------------
	if (active == TRUE) {
		Window dummy;
		unsigned int mask;
		int x,y,rx,ry,i;
		int x1,y1,x2,y2;

		if (IsXinerama(dpy)) {
			XQueryPointer(
				dpy,DefaultRootWindow(dpy),&dummy,&dummy,
				&rx,&ry,&x,&y,&mask
			);
			for (i=0;i<scr_count;i++) {
			x1 = screens[i].x_org;
			y1 = screens[i].y_org;
			x2 = x1 + screens[i].width;
			y2 = y1 + screens[i].height;
			if ((x >= x1 ) && (x <= x2) && (y >= y1) && (y <= y2)) {
				printf("%d\n",i);
				break;
			}
			}
		} else {
			for (i=0;i<scr_count;i++) {
			XQueryPointer(
				dpy,RootWindow(dpy,i),&dummy,&dummy,
           	 &rx,&ry,&x,&y,&mask
        	);
			if ((x != 0) && (y != 0)) {
				printf("%d\n",i);
				break;
			}
			}
		}
		break;
	}

	//=================================
    // Middle points per screen...
    //---------------------------------
	if (middle == TRUE) {
	int x,y;
	if (IsXinerama(dpy)) {
		x = (int)(screens[current_screen].width  / 2);
		y = (int)(screens[current_screen].height / 2);
		x = x + screens[current_screen].x_org;
		y = y + screens[current_screen].y_org;
		printf ("%d %d:%d\n",
			current_screen,x,y
		);
	} else {
		x = DisplayWidth(dpy,current_screen);
		y = DisplayHeight(dpy,current_screen);
		printf ("%d %d:%d\n",
			current_screen,(int)x/2,(int)y/2
		);
	}
	}

	//=================================
	// Active Modeline per screen...
	//---------------------------------
	if (modeline == TRUE) {
	if (GetModeLineData (dpy,current_screen)) {
		printf("%d ",current_screen); ShowModeline();
	}
	}

	//=================================
    // Adjustable Modelines/screen...
    //---------------------------------
	if (adjustable == TRUE) {
	if (GetModeLineData (dpy,current_screen)) {
		float hsync = AppRes.field[HSyncStart].val;
		float vsync = AppRes.field[VSyncStart].val;
		float clock = AppRes.field[PixelClock].val;
		if ((hsync == 0) || ( vsync == 0) || (clock < 0) || (clock > 500e3)) {
			printf("%d no\n",current_screen);
		} else {
			printf("%d yes\n",current_screen);
		}
	}
	}

	//=================================
	// current frequencies used...
	// --------------------------------
	if (freq == TRUE) {
	if (GetModeLineData (dpy,current_screen)) {
		float zf,rr;
		zf = (float)dot_clock / (float)AppRes.field[HTotal].val;
		rr = zf*1e3 / (float)AppRes.field[VTotal].val;
		printf("%d %.0f Khz : %.0f Hz\n",current_screen,zf,rr);
	}
	}

	//=================================
	// show rendered text...
	// --------------------------------
	if (render == TRUE) {
		str screen;
		int scrnr;
		strsplit (rstring,' ',screen,rstring);
		scrnr = atoi (screen);	
		showRenderText (rstring,dpy,scrnr);
		XCloseDisplay(dpy);
		exit (0);
	}

	//=================================
	// Active Resolution per screen...
	// --------------------------------
	if (resolution == TRUE) {
		str reso;
		if (IsXinerama(dpy)) {
		sprintf (
			reso,"%dx%d",
			screens[current_screen].width,
			screens[current_screen].height
		);
		} else {
		sprintf (
			reso,"%dx%d",
			DisplayWidth(dpy,current_screen),
			DisplayHeight(dpy,current_screen)
   		);
		}
		printf("%d %s\n",current_screen,reso);
	}

	//=================================
	// apply mode on screen x...
	// --------------------------------
	if (applymode == TRUE) {
		int hdisp,hsyncstart,hsyncend,htotal;
		int vdisp,vsyncstart,vsyncend,vtotal;
		int badValue;
		str left;
		strsplit(timing,' ',left,timing);
		userscreen = atoi(left);
		strsplit(timing,' ',left,timing);
		hdisp = atoi(left);
		strsplit(timing,' ',left,timing);
		hsyncstart = atoi(left);
		strsplit(timing,' ',left,timing);
		hsyncend = atoi(left);
		strsplit(timing,' ',left,timing);
		htotal = atoi(left);
		strsplit(timing,' ',left,timing);
		vdisp = atoi(left);
		strsplit(timing,' ',left,timing);
		vsyncstart = atoi(left);
		strsplit(timing,' ',left,timing);
		vsyncend = atoi(left);
		strsplit(timing,' ',left,timing);
		vtotal = atoi(left);

		badValue = PrepareModeline (
			dpy,current_screen,
			hdisp,hsyncstart,hsyncend,htotal,
			vdisp,vsyncstart,vsyncend,vtotal
		);
		if (! badValue) {
			ApplyModeline (dpy,userscreen);
		}
		break;
	}

	//=================================
	// sync frequencies per screen...
	// --------------------------------
	if (sync == TRUE) {
	XF86VidModeMonitor monitor;
	if (XF86VidModeGetMonitor (dpy, current_screen, &monitor)) {
		printf("%d %6.2f-%-6.2f  %6.2f-%-6.2f\n", 
			current_screen,monitor.hsync[0].lo, monitor.hsync[0].hi,
			monitor.vsync[0].lo, monitor.vsync[0].hi
		);
	}
	}

	//=================================
	// colors per screen...
	// --------------------------------
	if (color == TRUE) {
	int screen = current_screen;
	if (IsXinerama(dpy)) {
		screen = 0;
	}
	printf ("%d %d\n",
		current_screen,DisplayPlanes(dpy,screen)
	);
	}

	//=================================
	// switch to next mode on screen X
	// --------------------------------
	if (nextmode == TRUE) {
		SwitchMode (dpy,Next,userscreen);
		break;
	}

	//=================================
	// switch to prev mode on screen X
	// -------------------------------
	if (prevmode == TRUE) {
		SwitchMode (dpy,Back,userscreen);
		break;
	}

	//=================================
    // check if Xinerama is active
    // -------------------------------
	if (xinerama == TRUE) {
		if (IsXinerama(dpy)) {
			printf("Xinerama is enabled\n");
		} else {
			printf("Xinerama is disabled\n");
		}
		break;
	}
	CleanUp(dpy,current_screen);
	}
	XCloseDisplay(dpy);
	exit(0);
} 

//=====================
// errorHandler
//---------------------
int errorHandler (Display* dpy,XErrorEvent *err) {
	exit (1);
}

//=====================
// CleanUp()
//---------------------
void CleanUp(Display *dpy, int scr) {
	XF86VidModeLockModeSwitch(dpy, scr, FALSE);
	XFlush(dpy);
}

//=====================
// GetModeLineData()
//---------------------
Bool GetModeLineData (Display* dpy, int scr) {
	XF86VidModeModeLine mode_line;
	fields i;
	if (!XF86VidModeGetModeLine (
		dpy, scr, &dot_clock, &mode_line)
	) {
		// .../
		// in case of failure we assume the timings
		// from the first screen, hopefully this will apply
		// to a special xinerama implementation like NVidia
		// is using
		// ----
		XF86VidModeGetModeLine ( dpy, 0, &dot_clock, &mode_line );
	}
	AppRes.field[HDisplay].val   = mode_line.hdisplay;
	AppRes.field[HSyncStart].val = mode_line.hsyncstart;
	AppRes.field[HSyncEnd].val   = mode_line.hsyncend;
	AppRes.field[HTotal].val     = mode_line.htotal;
	AppRes.field[VDisplay].val   = mode_line.vdisplay;
	AppRes.field[VSyncStart].val = mode_line.vsyncstart;
	AppRes.field[VSyncEnd].val   = mode_line.vsyncend;
	AppRes.field[VTotal].val     = mode_line.vtotal;

	AppRes.old[HDisplay]         = mode_line.hdisplay;
	AppRes.old[HSyncStart]       = mode_line.hsyncstart;
	AppRes.old[HSyncEnd]         = mode_line.hsyncend;
	AppRes.old[HTotal]           = mode_line.htotal;
	AppRes.old[VDisplay]         = mode_line.vdisplay;
	AppRes.old[VSyncStart]       = mode_line.vsyncstart;
	AppRes.old[VSyncEnd]         = mode_line.vsyncend;
	AppRes.old[VTotal]           = mode_line.vtotal;

	mode_flags = mode_line.flags;
	AppRes.field[Flags].val = mode_flags & V_FLAG_MASK;
	AppRes.field[PixelClock].val = dot_clock;

	if (mode_line.privsize && mode_line.private) {
		S3Specials = True;
		AppRes.field[InvertVclk].val  = mode_line.private[1];
		AppRes.field[BlankDelay1].val = mode_line.private[2] & 7;
		AppRes.field[BlankDelay2].val = (mode_line.private[2] >> 4) & 7;
		AppRes.field[EarlySc].val     = mode_line.private[3];
	}
	for (i = HDisplay; i < fields_num; i++) {
		AppRes.orig[i] = AppRes.field[i].val;
	}
	return TRUE;
}

//=====================
// ShowModeline()
//---------------------
void ShowModeline (void) {
	char tmpbuf[16] = {0};

	strcpy(tmpbuf,"");
	sprintf(tmpbuf,"\"%dx%d\"",
	AppRes.field[HDisplay].val, AppRes.field[VDisplay].val);
	snprintf (
		modebuf, sizeof(modebuf),"%s %.2f %d %d %d %d %d %d %d %d",
		tmpbuf, (float)dot_clock/1000.0,
		AppRes.field[HDisplay].val,
		AppRes.field[HSyncStart].val,
		AppRes.field[HSyncEnd].val,
		AppRes.field[HTotal].val,
		AppRes.field[VDisplay].val,
		AppRes.field[VSyncStart].val,
		AppRes.field[VSyncEnd].val,
		AppRes.field[VTotal].val
	);

 	if (mode_flags & V_PHSYNC)    strncat (
		modebuf, " +hsync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_NHSYNC)    strncat (
		modebuf, " -hsync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_PVSYNC)    strncat (
		modebuf, " +vsync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_NVSYNC)    strncat (
		modebuf, " -vsync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_INTERLACE) strncat (
		modebuf, " interlace", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_CSYNC)     strncat (
		modebuf, " composite", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_PCSYNC)    strncat (
		modebuf, " +csync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_PCSYNC)    strncat (
		modebuf, " -csync", sizeof(modebuf)-strlen(modebuf)-1
	);
	if (mode_flags & V_DBLSCAN)   strncat (
		modebuf, " doublescan", sizeof(modebuf)-strlen(modebuf)-1
	);
	modebuf[sizeof(modebuf)-strlen(modebuf)-1] = 0;
	printf("%s\n",modebuf);
	return;
}

//=====================
// SwitchMode()
//---------------------
void SwitchMode (Display *dpy, int call, int scr) {
	XF86VidModeLockModeSwitch (dpy,scr,FALSE);
	XF86VidModeSwitchMode (dpy,scr,call);
	XF86VidModeLockModeSwitch (dpy,scr,TRUE);
}

//=====================
// ApplyModeline()
//---------------------
void ApplyModeline (Display *dpy, int scr) {
	XF86VidModeModeLine mode_line;

	mode_line.hdisplay   = AppRes.field[HDisplay].val;
	mode_line.hsyncstart = AppRes.field[HSyncStart].val;
	mode_line.hsyncend   = AppRes.field[HSyncEnd].val;
	mode_line.htotal     = AppRes.field[HTotal].val;
	mode_line.vdisplay   = AppRes.field[VDisplay].val;
	mode_line.vsyncstart = AppRes.field[VSyncStart].val;
	mode_line.vsyncend   = AppRes.field[VSyncEnd].val;
	mode_line.vtotal     = AppRes.field[VTotal].val;
	mode_line.flags      = mode_flags;
	mode_line.privsize   = 0;

	XF86VidModeModModeLine (
		dpy, scr, &mode_line
	);
	XSync(dpy, False);
}

//=====================
// PrepareModeline()
//---------------------
int PrepareModeline (
	Display* dpy,int current_screen,
	int h1,int h2,int h3,int h4,int v1,int v2,int v3,int v4
) {
	int hsmin,hsmax;
	int vsmin,vsmax;
	if (
		h2 < h1 || h3 < h2 || h4 < h3 ||
		v2 < v1 || v3 < v2 || v4 < v3
	) {
		return 1;
	}
	XF86VidModeMonitor monitor;
	if (XF86VidModeGetMonitor (dpy, current_screen, &monitor)) {
		hsmax = monitor.hsync[0].hi;
		hsmin = monitor.hsync[0].lo;
		vsmax = monitor.vsync[0].hi;
		vsmin = monitor.vsync[0].lo;
		XF86VidModeModeLine mode_line;
		if (!XF86VidModeGetModeLine (
			dpy, current_screen, &dot_clock, &mode_line)
		) {
			return 1;
		}
		double dc = (double)dot_clock * 1000;
		double hs = dc / (double)h4;
		double vs = dc / (double)(h4 * v4);
		hs = hs / 1000;
		if ((hs > hsmax) || (hs < hsmin)) {
			return 1;
		}
		if ((vs > vsmax) || (vs < vsmin)) {
			return 1;
		}
	}
	AppRes.field[HDisplay].val   =h1;
	AppRes.field[HSyncStart].val =h2;
	AppRes.field[HSyncEnd].val   =h3;
	AppRes.field[HTotal].val     =h4;

	AppRes.field[VDisplay].val   =v1;
	AppRes.field[VSyncStart].val =v2;
	AppRes.field[VSyncEnd].val   =v3;
	AppRes.field[VTotal].val     =v4;
	return 0;
}

//=====================
// XOpen()
//---------------------
Display* XOpen(char name[256],int screen) {
	Display *dpy;
	str display = "";
	
	if (strcmp(name,"null") == 0) {
	if (getenv("DISPLAY") != NULL) {
		strcpy(display,getenv("DISPLAY"));
	}
	} else {
		strcpy(display,name);
	}
	if(!(dpy = XOpenDisplay(display))) {
	return(NULL);
	}
	if (!XF86VidModeQueryVersion(
		dpy, &MajorVersion, &MinorVersion)
	) {
	return(NULL);
	}
	if (!XF86VidModeQueryExtension(
		dpy, &EventBase, &ErrorBase)
	) {
	return(NULL);
	}
	XSetErrorHandler (errorHandler);
	return(dpy);
}

//=====================
// CountScreens()
//---------------------
void CountScreens (Display *dpy) {
	int s_num,count;
	if ( XineramaIsActive(dpy)) {
		screens = XineramaQueryScreens(dpy, &s_num);
		scr_count = s_num;
		for (count = 0;count < s_num; count++) {
			if (count > 0) {
			if ((screens[count].x_org == 0) && (screens[count].y_org == 0)) {
				// is cloned; do not handle that screen
				scr_count--;
				continue;
			}
			}
			scr[count] = screens[count].screen_number;
		}
	} else {
		scr_count = ScreenCount(dpy);
		for (count = 0;count < scr_count; count++) {
		scr[count] = count;
		}
	}
}

//=====================
// IsXinerama()
//---------------------
Bool IsXinerama (Display *dpy) {
	if (XineramaIsActive(dpy)) {
	return(TRUE);
	}
	return(FALSE);
}

//=====================
// usage()
//---------------------
void usage(void) {
	printf("usage: xquery [ options ]\n");
	printf("options:\n");
	printf("  [ -s | --screen ]\n");
	printf("    query number of available screens\n\n");
	printf("  [ -a | --applymode ]\n");
	printf("    set new modeline timings for a specific screen\n");
	printf("    format: \"screen timing\"\n\n");
	printf("  [ -m | --modeline ]\n");
	printf("    query current used modelines for all screens\n\n");
	printf("  [ -r | --resolution ]\n");
	printf("    query current used resolutions for all screens\n\n");
	printf("  [ -y | --sync ]\n");
	printf("    query sync ranges from the used configuration\n");
	printf("    for all screens\n\n");
	printf("  [ -n | --nextmode <ScreenNumber> ]\n");
	printf("    switch to the next mode if defined in MODES\n");
	printf("    list of the used configuration\n\n");
	printf("  [ -p | --prevmode <ScreenNumber> ]\n");
	printf("    switch to the previous mode if defined in MODES\n");
	printf("    list of the used configuration\n\n");
	printf("  [ -d | --display ]\n");
	printf("    set basic display to use\n\n");
	printf("  [ -c | --color ]\n");
	printf("    get current color depths for all screens\n\n");
	printf("  [ -f | --freq ]\n");
	printf("    get current used frequencies for all screens\n\n");
	printf("  [ -M | --middle ]\n");
	printf("    get middle point coordinates of all screens\n\n");
	printf("  [ -S | --currentscr ]\n");
	printf("    get screen number the mouse is currently over\n\n");
	printf("  [ -R | --render <screen string>]\n");
	printf("    show XFT rendered string on a specific screen\n");
	printf("    format: \"screen text\"\n");
	printf("--\n");
	exit(0);
}
