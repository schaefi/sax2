/**************
FILE          : xw.c
***************
PROJECT       : SaX2 ( SuSE advanced X11 configuration )
              :
DESCRIPTION   : xw.c is used to work as X-Server wrapper.
              : It will start the X11 loader where the first
              : is used as file for the X-log.
              :
              : if the server is ready the fvwm windowmanager is started 
              : and the background,corner marks are set as well as 
              : xaccess is enabled. If the process receives the 
              : ready [SIGUSR1] we will fork into the background 
              : and exit
              :
              : 
STATUS        : Status: Up-to-date
**************/
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/kd.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBgeom.h>
#include <X11/extensions/XKM.h>
#include <X11/extensions/XKBfile.h>
#include <X11/extensions/XKBrules.h>
#include <X11/extensions/Xinerama.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>
#include <X11/XKBlib.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/Xft/Xft.h>
#include <unistd.h>

#include "xaccess.h"
#include "fashion.h"
#include "xw.h"

//=========================================
// Globals
//-----------------------------------------
XineramaScreenInfo *screens = NULL;
struct mirror save[100];    // save bottom right area
Display* dpy      = NULL;
XImage* img       = NULL;
char* displayname = NULL;
char* blank       = NULL;
int   scr_count   = 1;
int   looppid     = 0;
int   wmpid       = 0;
int   timerpid    = 0;
int   imgx        = 0;
int   imgy        = 0;
int   tunerpid    = 0;
char* logtimer    = NULL;
int   scr[100];
int   terminal;
int   cpid;

//=========================================
// Main
//-----------------------------------------
int main(int argc,char *argv[]) {
	char* args[argc];
	int fd         = 0;
	int timeout    = 8;
	int status     = 0;
	char* logfile  = NULL;
	char* logPart1 = NULL;
	char* logPart2 = NULL;
	FILE* data;
	int   i,n,count;

	//=========================================
	// Prepare for i18n
	//-----------------------------------------
	setlocale (LC_ALL, "");
	bindtextdomain ("sax", "/usr/share/locale");
	textdomain ("sax");
	bind_textdomain_codeset ("sax","UTF-8");

	//=========================================
	// save first parameter as logfile name
	//-----------------------------------------
	if (argc > 1) {
		logfile = (char*)malloc(sizeof(char)*128);
		strcpy(logfile,argv[1]);
		logPart1 = strtok (logfile,":");
		if (logPart1) {
			logPart2 = strtok (NULL,":");
			logfile  = logPart1;
			logtimer = logPart2;
		}
	}

	//=========================================
	// save the rest of the parameters for the
	// execvp call to the X-Server
	//-----------------------------------------
	displayname = (char*)malloc(DISPSIZE);
	strcpy(displayname,":0");

	args[0]     = (char*)malloc(strlen(argv[0])+1);
	strcpy(args[0],argv[0]);
	count = argc;
	n = 1;
	for (i=2;i<argc;i++) {
	if (strncmp(argv[i],"-blank",6) == 0) {
		blank = argv[i];
		count--;
	} else {
		args[n] = (char*)malloc(strlen(argv[i])+1);
		strcpy(args[n],argv[i]);
		n++;
	}
	if (strstr(argv[i],":") != NULL) {
		strcpy(displayname,argv[i]); 
	}
	}
	if (count > 1) {
		args[count-1] = (char*)malloc(sizeof(char)*4);
		strcpy(args[count-1],"-ac"); count++;
		args[count-1] = NULL;
	} else {
		args[count] = (char*)malloc(sizeof(char*)*4);
		strcpy(args[count],"-ac"); count++;
		args[count] = NULL;
	}

	//=========================================
	// setup signal handler
	//-----------------------------------------
	signal (SIGHUP  , trapper);
	signal (SIGINT  , trapper);
	signal (SIGTERM , trapper);
	signal (SIGPIPE , trapper);
	signal (SIGCHLD , child  );

	if ((fd = open(PATH_DEV_NULL, O_RDWR, 0)) != -1) {
		dup2  ( fd, STDERR_FILENO );
		close ( fd );
	}

	cpid = fork();
	switch (cpid) {
	case -1:
		// ...
		// can not fork
		// ---
		perror("fork"); exit(1);
	break;
	case 0:
		// ...
		// child process...
		// ---
		if (logfile != NULL) {
			data = freopen (logfile,"w",stderr );
			setvbuf ( data,(char*)NULL,_IONBF,0 );
		}
		execvp (XLOADER,args);
	break;
	default:
		// ...
		// parent process
		// ---
		break;
	}
	while ( timeout > 0  ) {
	sleep ( timeout -= 2 );
	if ( waitpid ( cpid, &status, WNOHANG | WUNTRACED) == cpid ) {
		break;
	} else {
		dpy = XOpenDisplay (displayname);
		if (dpy != NULL) {
			break;
		}
	}
	}
	if (dpy == NULL) {
		exit(1);
	}
	prepare ();
	//=====================================
	// start idle timer if logtimer is set
	//-------------------------------------
	if (logtimer) {
		char pidToKill [20] = "";
		sprintf (pidToKill,"%d",looppid);
		if (blank) {
			sprintf (pidToKill,"%d",cpid);
		}
		timerpid = fork();
		switch (timerpid) {
		case -1:
			perror("fork"); exit(1);
		break;
		case 0:
			execl (XIDLE,
				"xapi","-x","-p",pidToKill,"-display",displayname,NULL
			);
		break;
		default:
		waitpid (
			timerpid, NULL, WNOHANG | WUNTRACED
		);
		}
	}
	if (! blank) {
	if (! kill (looppid,0)) {
		printf ("%d\n",cpid);
	} else {
		printf ("%d\n",looppid);
	}
	} else {
		printf ("%d\n",cpid);
	}
	exit (0);
}

//=========================================
// prepare when ready
//-----------------------------------------
void prepare (void) {
	int count;
	char disp[80] = "";
	if (displayname != NULL) {
		sprintf(disp,"DISPLAY=%s",displayname); 
		putenv (disp);
	}
	//=========================================
	// start windowmanager...
	//-----------------------------------------
	wmpid = fork();
	switch(wmpid) {
	case -1: 
		perror("fork"); exit(1);
	break;
	case 0:
		execl(FVWM,"saxwm",NULL);
	break;
	default:
	waitpid (
		wmpid, NULL, WNOHANG | WUNTRACED
	);
	}
	//=========================================
	// enable accessX...
	//-----------------------------------------
	// .../
	// disable access X because vmware doesn't support it
	// XAccess(dpy,displayname);

	//=========================================
	// set the fashion style for all displays
	// including Xinerama handling here...
	//------------------------------------------
	CountScreens (dpy);
	for (count = 0;count < scr_count; count++) {
		int current_screen = scr[count];
		Window root = RootWindow(dpy, current_screen);
		int depth   = DefaultDepth   (dpy,current_screen);
		int xpixels = XDisplayWidth  (dpy,current_screen);
		int ypixels = XDisplayHeight (dpy,current_screen);
		char* display = (char*)malloc(sizeof(char)*128);
		char* dspstr  = (char*)malloc(sizeof(char)*128);
		char* color   = (char*)malloc(sizeof(char)*128);
		char screenStr [255] = "";
		char x [128] = "";
		char y [128] = "";
		int x1,y1,width,height;
		GC mgc = DefaultGC(dpy,current_screen);
		Cursor cursor;

		//=========================================
		// set the background... 
		//-----------------------------------------
		if (blank) {
			//=========================================
			// set the background to black
			//-----------------------------------------
			sprintf(color,"black");
			XSetWindowBackground(
				dpy, root, NameToPixel(
				color,BlackPixel(dpy,current_screen),dpy,current_screen
				)
			);
			XClearWindow(dpy,root);
			XSetCloseDownMode(dpy, RetainPermanent);
			usleep (1000);
			XFlush(dpy);
		} else {
			//=========================================
			// set the background to a picture
			//-----------------------------------------
			char xsize[20] = "";
			char ysize[20] = "";
			char sdpy[20]  = "";
			sprintf (xsize,"%d",xpixels);
			sprintf (ysize,"%d",ypixels);
			sprintf (sdpy,"%s.%d",displayname,current_screen);
			int bgpid = fork();
			switch(bgpid) {
			case -1:
				perror("fork"); exit(1);
			break;
			case 0:
				execl (DISPLAY,
					"ximage","-xsize",xsize,"-ysize",ysize,
					"-display",sdpy,"-image",BACKGROUND,NULL
	            );
			break;
			default:
			waitpid (
				bgpid, NULL, 0
			);
			}
		}
		//=========================================
		// set the corner marks
		//-----------------------------------------
		if (! blank) {
		setCorner ("10x15-0-0",dpy,current_screen); 
		setCorner ("10x15-0+0",dpy,current_screen);
		setCorner ("10x15+0-0",dpy,current_screen); 
		setCorner ("10x15+0+0",dpy,current_screen);
		//=========================================
		// set the bounding rectangle
		//-----------------------------------------
		setBounding (dpy,current_screen);
		//=========================================
		// print a hint in each screen
		//-----------------------------------------
		if (DisplayPlanes (dpy, current_screen) >= 8) {
			sprintf(dspstr,"%s",displayname);
			dspstr = strtok (dspstr,".");
			sprintf(display,"%s.%d",dspstr,current_screen);
			sprintf(x,"%d",DisplayWidth  (dpy,current_screen) - 150);
			sprintf(y,"%d",DisplayHeight (dpy,current_screen) - 80);
			//===================================================
			// create a xlook banner text on all non primary screen
			// pointing out, that this screen is not the one the
			// configuration program is running on
			//---------------------------------------------------
			if (current_screen > 0) {
				int HasRender;
				sprintf(screenStr,"%s %d. %s 1.",
					gettext("Display"),count+1,
					gettext("Configuration on display")
				);
				HasRender = XTestRender (dpy);
				if (! HasRender) {
					break;
				}
				img = setText (
					dpy,current_screen,screenStr,&imgx,&imgy
				);
			}

			//=========================================
			// set save area of the root window to be
			// restored on demand
			//-----------------------------------------
			x1 = DisplayWidth  (dpy,current_screen) - 380;
			y1 = DisplayHeight (dpy,current_screen) - 120;
			width  = 380; height = 120;
			save[current_screen].width  = width;
			save[current_screen].height = height;
			save[current_screen].x1 = x1;
			save[current_screen].y1 = y1;
			save[current_screen].bg = XCreatePixmap (
				dpy,root,width,height,depth
			);
			XCopyArea (
				dpy,root,save[current_screen].bg,mgc,x1,y1,width,height,0,0
			);
		}
		//=========================================
		// set the cursor look and feel
		//-----------------------------------------
		cursor = CreateCursorFromName(dpy,current_screen,"top_left_arrow");
		if (cursor) {
			XDefineCursor (dpy,root,cursor);
			XFreeCursor (dpy,cursor);
		}
		}
	}
	//=========================================
	// Enter main loop, restore and redraw
	//-----------------------------------------
	if (! blank) {
		looppid = fork();
		switch (looppid) {
		case -1:
			perror("fork"); exit(1);
		break;
		case 0:
			{
			FILE* fd = NULL;
			char s_idle[80];
			char s_load[80];
			char s_nice[80];
			char s_null[80];
			long idle,load,nice,pval;
			long old_idle = 0;
			long old_nice = 0;
			long old_load = 0;
			int mouse_is_arrow = 1;
			Window root = RootWindow(dpy,0);
			Cursor cursor;
			while(1) {
				RedrawWindow();
				if ((fd=fopen ("/proc/stat","r"))) {
					fscanf (fd,"cpu %s %s %s %s\n",s_load,s_nice,s_null,s_idle);
					fclose (fd);
				}
				idle = atol (s_idle);
				nice = atol (s_nice);
				load = atol (s_load);

				pval = (load-old_load)+(nice-old_nice)+(idle-old_idle);
				if (pval) {
					pval = ((load-old_load)*100)/pval;
				} else {
					pval = 100;
				}
				if (pval >= 20) {
					if (mouse_is_arrow) {
						cursor = CreateCursorFromName(dpy,0,"watch");
						if (cursor) {
							XDefineCursor (dpy,root,cursor);
							XFreeCursor (dpy,cursor);
						}
						mouse_is_arrow = 0;
					}
				} else {
					if (! mouse_is_arrow) {
						cursor = CreateCursorFromName(dpy,0,"top_left_arrow");
						if (cursor) {
							XDefineCursor (dpy,root,cursor);
							XFreeCursor (dpy,cursor);
						}
						mouse_is_arrow = 1;
					}
				}
				old_idle = idle;
				old_load = load;
				old_nice = nice;

				usleep (50000);
			}
			}
		break;
		}
	}
}

//=========================================
// update root window contents
//-----------------------------------------
void RedrawWindow (void) {
	int i;
	int planes = 0;
	for (i=0;i<scr_count;i++) {
		Window root = RootWindow(dpy,i);
		GC mgc = DefaultGC(dpy,i);
		planes = DisplayPlanes (dpy,i);
		if (planes < 8) {
			continue;
		}
		XCopyArea (
			dpy,save[i].bg,root,mgc,0,0,
			save[i].width,save[i].height,save[i].x1,save[i].y1
		);
		if (i > 0) {
		XPutImage (
			dpy,root,mgc,img,0,0,
			imgx,imgy,img->width,img->height
		);
		}
	}
	XFlush(dpy);
}

//=========================================
// CountScreens
//-----------------------------------------
void CountScreens (Display *dpy) {
	int s_num,count;
	if ( XineramaIsActive(dpy)) {
		screens = XineramaQueryScreens(dpy, &s_num);
		scr_count = s_num;
		for (count = 0;count < scr_count; count++) {
		scr[count] = screens[count].screen_number;
		}
		scr_count = 1;
	} else {
		scr_count = ScreenCount(dpy);
		for (count = 0;count < scr_count; count++) {
		scr[count] = count;
		}
	}
}

//=========================================
// IsXinerama
//-----------------------------------------
Bool IsXinerama (Display *dpy) {
	if (XineramaIsActive(dpy)) {
	return(TRUE);
	}
	return(FALSE);
}

//=========================================
// trapper...
//-----------------------------------------
void trapper (int s) {
	if (looppid != 0) {
		kill (looppid,SIGTERM);
	}
	kill (wmpid,SIGTERM);
	kill (cpid ,SIGTERM);
	exit(0);
}

//=========================================
// child...
//-----------------------------------------
void child (int s) {
	union wait wstatus;
	if (dpy == NULL) {
		printf("%d\n",cpid); exit(0);
	}
	while(wait3(&wstatus,WNOHANG,NULL) > 0);
}

//=========================================
// tty...
//-----------------------------------------
int tty (void) {
	char *name = NULL;
	int  l;

	name = (char*)malloc(sizeof(char)*128);
	name = ttyname(0);
	l = strlen(name); 
	l--;

	sprintf(name,"%c",name[l]);
	l = atoi(name);
	return(l);
}

//=========================================
// chvt...
//-----------------------------------------
int chvt (int num) {
	int fd;
	fd = getfd();
	if (ioctl(fd,VT_ACTIVATE,num)) {
		perror("chvt: VT_ACTIVATE");
		return(1);
	}
	if (ioctl(fd,VT_WAITACTIVE,num)) {
		perror("VT_WAITACTIVE");
		return(1);
	}
	return(0);
}

//=========================================
// is_a_console...
//-----------------------------------------
int is_a_console(int fd) {
	char arg = 0;
	return (
	ioctl(fd, KDGKBTYPE, &arg) == 0 && ((arg == KB_101) || (arg == KB_84))
	);
}

//=========================================
// open_a_console...
//-----------------------------------------
int open_a_console(char *fnam) {
	int fd;

	fd = open(fnam, O_RDONLY);
	if (fd < 0 && errno == EACCES) {
		fd = open(fnam, O_WRONLY);
	}
	if (fd < 0 || ! is_a_console(fd)) {
		return -1;
	}
	return fd;
}

//=========================================
// getfd...
//-----------------------------------------
int getfd(void) {
	int fd;

	fd = open_a_console("/dev/tty");
	if (fd >= 0) {
		return fd;
	}
	fd = open_a_console("/dev/tty0");
	if (fd >= 0) {
		return fd;
	}
	fd = open_a_console("/dev/console");
	if (fd >= 0) {
		return fd;
	}

	for (fd = 0; fd < 3; fd++) {
	if (is_a_console(fd)) {
		return fd;
	}
	}
	fprintf(
		stderr,"could not get a console file descriptor\n"
	);
	exit(1);
}

//=========================================
// kill all clients...
//-----------------------------------------
void closeClients (Display *dpy) {
	Window root = DefaultRootWindow (dpy);
	Window dummywindow;
	Window *children;
	XWindowAttributes attr;
	unsigned int nchildren;
	unsigned int i;

	XSync (dpy, 0);
	XSetErrorHandler (catchWindowErrors);

	nchildren = 0;
	XQueryTree (
		dpy, root, &dummywindow, &dummywindow, 
		&children, &nchildren
	);
	for (i = 0; i < nchildren; i++) {
	if ( XGetWindowAttributes (dpy, children[i], &attr) && 
		(attr.map_state == IsViewable)
	) {
		children[i] = XmuClientWindow(dpy, children[i]);
	} else {
		children[i] = 0;
	}
	}
	for (i = 0; i < nchildren; i++) {
	if (children[i])
		XKillClient (dpy, children[i]);
	}
	XFree ((char *)children);
	XSync (dpy, 0);
	XSetErrorHandler (NULL);        
}

//=========================================
// catch errors...
//-----------------------------------------
int catchWindowErrors (Display *dpy, XErrorEvent *ev) {
	return (0);
}

//=========================================
// XTestRender
//-----------------------------------------
int XTestRender (Display* dpy) {
	// ...
	// test if the render extension is available on the
	// currently used display
	// ---
	int n = 0;
	int i = 0;
	char **extlist = XListExtensions (dpy, &n);
	for (i = 0; i < n; i++) {
	if (strcmp(extlist[i],"RENDER") == 0) {
		return(1);
	}
	}
	return(0);
}

//=========================================
// setText
//-----------------------------------------
XImage* setText (Display* dpy,int screen,char* text, int* x, int* y) {
	Window root = RootWindow    ( dpy,screen );
	int DWidth  = DisplayWidth  ( dpy,screen );
	int DHeight = DisplayHeight ( dpy,screen );

	XftPattern *pattern = NULL;
	XftFont*  FTfont    = NULL;
	XftDraw*  FTdraw    = NULL;
	XftColor* FTcolor   = NULL;
	XImage*   Image     = NULL;

	XGlyphInfo FTinfo;
	XColor color;
	XGCValues values;
	int XTwidth;
	int XTheight;
	int x1,y1;

	pattern = XftNameParse (XFTFONT);
	pattern = XftFontMatch (dpy,screen,pattern,NULL);
	FTfont  = XftFontOpenPattern (dpy, pattern);
	XftTextExtentsUtf8 (
		dpy,FTfont,(unsigned char*)text,strlen(text),&FTinfo
	);
	XTwidth = FTinfo.xOff;
	XTheight= FTfont->height + 20;
	FTdraw  = XftDrawCreate (
		dpy, root, DefaultVisual(dpy,screen),DefaultColormap(dpy,screen)
	);
	FTcolor = (XftColor*)malloc(sizeof(XftColor));
	color.red   = 255 << 8;
	color.green = 255 << 8;
	color.blue  = 255 << 8;
	XAllocColor (dpy,DefaultColormap(dpy,screen),&color);
	XSetForeground(dpy,DefaultGC (dpy,screen),color.pixel);
	XGetGCValues (
		dpy, DefaultGC (dpy,screen),
		GCForeground | GCBackground, &values
	);
	FTcolor->color.red   = color.red;
	FTcolor->color.green = color.green;
	FTcolor->color.blue  = color.blue;
	FTcolor->color.alpha = 0xffff;

	x1 = (int)((DWidth  / 2) - (XTwidth  / 2));
	y1 = (int)((DHeight / 2) - (XTheight / 2));

	XftDrawStringUtf8 (
		FTdraw, FTcolor,FTfont,x1,y1,(unsigned char*)text,strlen(text)
	);
	XFlush (dpy);
	y1 = (int)((DHeight / 2) - XTheight);
	Image = XGetImage(
		dpy,root,x1,y1,XTwidth,XTheight,AllPlanes,XYPixmap
	);
	*x = x1;
	*y = y1;
	return (Image);
}

