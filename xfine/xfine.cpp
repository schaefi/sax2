/**************
FILE          : xfine.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning too)
              : main program initializing all dialog objects,
              : log handler and option handler
              :
              :
STATUS        : Status: Up-to-date
**************/
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <qapplication.h>
#include <q3mainwindow.h>
#include <QDesktopWidget>
#include <qregexp.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <X11/Xlib.h>
extern "C" {
#include <X11/extensions/Xinerama.h>
#include <GL/glx.h>
}

#include "frame.h"
#include "xfine.h"
#include "xquery.h"
#include "config.h"

//=====================================
// Globals...
//-------------------------------------
XFineWindow* xfine = NULL;
int* child         = NULL;
int  childCount    = 1;

//=====================================
// main program...
//-------------------------------------
int main(int argc,char*argv[]) {
	//=====================================
	// init options...
	//-------------------------------------
	bool initWindow = false;
	bool setStyle   = false;
	int  screen,xmiddle,ymiddle;
	char style[128];
	for (int n=0;n<argc;n++) {
	if (strcmp(argv[n],"-style") == 0) {
		sprintf(style,"%s",argv[n+1]);
		setStyle = true;
		break;
	} 
	}

	//=====================================
	// check for root priviliges...
	//-------------------------------------
	if (! accessAllowed()) {
		fprintf (stderr,"xfine: only root can do this\n");
		usage();
	}

	//=====================================
	// start logging...
	//-------------------------------------
	logInit();

	//=====================================
	// allocate main qt object...
	//-------------------------------------
	for (int i=0;i<argc;i++) {
	QString item (argv[i]);
	if ((item == "-h") || (item == "--help")) {
		logExit(); usage(); exit(0);
	}
	}
	QApplication app (argc,argv);
	xmiddle = QApplication::desktop()->width()  / 2;
	ymiddle = QApplication::desktop()->height() / 2;
	screen  = 0;

	//=====================================
	// get additional options...
	//-------------------------------------
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"help"       , 0 , 0 , 'h'},
		{"window"     , 0 , 0 , 'w'},
		{"xmiddle"    , 1 , 0 , 'x'},
		{"ymiddle"    , 1 , 0 , 'y'},
		{"screen"     , 1 , 0 , 's'},
		{0            , 0 , 0 , 0  }
	};
	int c = getopt_long (argc, argv, "hwUs:x:y:",long_options, &option_index);
	if (c == -1)
	break;

	switch (c) {
	case 0:
		fprintf (stderr,"xfine: option %s", long_options[option_index].name);
		if (optarg)
		fprintf (stderr," with arg %s", optarg);
		fprintf (stderr,"\n");
	break;

	case 'w':
		initWindow = true;
	break;

	case 's':
		screen  = QString (optarg).toInt();
	break;

	case 'x':
		xmiddle = QString (optarg).toInt();
	break;

	case 'y':
		ymiddle = QString (optarg).toInt();
	break;

	case 'h':
		logExit(); usage();
	break;
	default:
		logExit(); exit(1);
	}
	}

	//=====================================
	// init application setup
	//-------------------------------------
	// if initWindow is set we will create the application window 
	// otherwhise the application will check for all available 
	// screen and multihead environments to call itself with
	// initWindow = true
	// ---
	if (initWindow) {
	// ... /
	// build the application window
	// ---
	QString optc ("-cursor_name");
	QString optd ("-display");
	QString cursorName ("watch");
	QString display (
		DisplayString (QApplication::desktop()->x11Display())
	);
	qx (XSETROOT,STDNONE,4,"%s %s %s %s",
		optc.ascii(),cursorName.ascii(),optd.ascii(),display.ascii()
	);
	xfine = new XFineWindow (
		xmiddle,ymiddle,screen
	);
	signal (SIGUSR1,signalSave);
	signal (SIGTERM,signalReset);
	app.setMainWidget (xfine);
	xfine->show();
	cursorName.sprintf("top_left_arrow");
	qx (XSETROOT,STDNONE,4,"%s %s %s %s",
		optc.ascii(),cursorName.ascii(),optd.ascii(),display.ascii()
	);

	} else {
	// ... /
	// check for available displays in single and
	// multihead environments
	// ...
	// - install signal handler
	// - check how many screens we have
	// - fork xfine processes for all screens
	// ---
	signal (SIGTERM,signalQuit);
	signal (SIGHUP ,signalActivateSave);
	XQuery screen;
	screen.setOption ("-M");
	QString* data = screen.run();
	XStringList screenData (*data);
	screenData.setSeperator ("\n");
	QList<const char*> screenList = screenData.getList();
	childCount = screenList.count();
	child = (int*)malloc (sizeof(int) * childCount);
	for (int n=0;n < (int)screenList.count();n++) {
		QString resolution (screenList.at(n));
		resolution = resolution.right (
			resolution.length() - 2
		);
		XStringList resData (resolution);
		resData.setSeperator (":");
		QList<const char*> resList = resData.getList();

		// ...
		// add aditional options needed for the
		// XFine2 window startup
		// ---
		char* pArgv [15];
		int count = 0;
		char sty[] = "-style";
		char dpy[] = "-display";
		char scr[] = "--screen";
		char xm[]  = "--xmiddle";
		char ym[]  = "--ymiddle";
		char win[] = "-w";
		char* nr  = NULL;
		QString display (
			DisplayString (QApplication::desktop()->x11Display())
		);
		QString screen (".0");
		if (! XineramaIsActive (QApplication::desktop()->x11Display())) {
			screen.sprintf(".%d",n);
		}
		display.replace(QRegExp("\\.[0-9]"),screen);
		nr = (char*) malloc (sizeof(char)*2);
		sprintf (nr,"%d",n);
		pArgv[count] = qApp->argv()[0];
		count++;
		if (setStyle) {
			pArgv[count] = sty;   count++;
			pArgv[count] = style; count++;
		}
		pArgv[count] = dpy;
		count++;
		pArgv[count] = (char*)display.ascii();
		count++;
		pArgv[count] = scr; 
		count++;
		pArgv[count] = nr; 
		count++;
		pArgv[count] = xm;
		count++;
		pArgv[count] = (char*)resList.at(0); 
		count++;
		pArgv[count] = ym;
		count++;
		pArgv[count] = (char*)resList.at(1);
		count++;
		pArgv[count] = win;
		count++;
		pArgv[count] = NULL;

		switch (child[n] = fork()) {
		case -1:
		fprintf(stderr,"fork() failed: %s\n",
			strerror(errno)
		);
		break;
		case 0:
			execvp(pArgv[0],pArgv);
			fprintf(stderr,"execvp() failed: %s\n",
				strerror(errno)
			);
			exit (1);
		}
	}
	}
	return app.exec();
}

//=====================================
// check root privileges...
//-------------------------------------
bool accessAllowed (void) {
	if (getuid() == 0) { return (true); }
	return (false);
}

//=====================================
// send activate for saving signal(s)
//-------------------------------------
void signalActivateSave (int) {
	for (int n=0;n<childCount;n++) {
		kill (child[n],SIGUSR1);
	}
	printf("2\n"); exit (2);
}

//=====================================
// exit all procs...
//-------------------------------------
void signalQuit (int) {
	for (int n=0;n<childCount;n++) {
		kill (child[n],SIGTERM);
	}
	printf("1\n"); exit (1);
}

//=====================================
// save data to /var/cache/sax
//-------------------------------------
void signalSave (int) {
	xfine -> saveMode ();
	exit (0);
}

//=====================================
// reset screen and abort
//-------------------------------------
void signalReset (int) {
	xfine -> resize (XAbort);
	exit (0);
}

//=====================================
// check for render extension
//-------------------------------------
bool hasDirectRendering (int screen) {
	Display *dpy = QApplication::desktop()->x11Display();
	int attribSingle[] = {
		GLX_RGBA,
		GLX_RED_SIZE,   1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE,  1,
		None
	};
	XVisualInfo* visinfo = glXChooseVisual (
		dpy, screen, attribSingle
	);
	if (visinfo) {
		GLXContext ctx = glXCreateContext ( dpy, visinfo, NULL, True );
		if (glXIsDirect(dpy, ctx)) {
			glXDestroyContext (dpy,ctx);
			return true;
		}
		glXDestroyContext (dpy,ctx);
		return false;
	} else {
		return false;
	}
}

//=====================================
// usage...
//-------------------------------------
void usage (void) {
	printf ("Linux XFine2 Version 8.1 - (C) Copyright 2002 SuSE GmbH\n");
	printf ("usage: xfine [ options ]\n");
	printf ("options:\n");
	printf ("[ -h | --help ]\n");
	printf ("   you already got it :-)\n");
	printf ("--\n");
	exit (1);
}
