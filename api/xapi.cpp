/**************
FILE          : xapi.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 GUI system using libsax to provide
              : configurations for a wide range of X11 capabilities
              : //.../
              : - xapi.cpp: main program start sequence
              : - xapi.h: buildins and templates
              : ----
              :
STATUS        : Status: Development
**************/

#include <QWidget>
#include <QDesktopWidget>

#include "xapi.h"

//=====================================
// Globals
//-------------------------------------
SaXGUI::SCCFrame* sccGUI = 0;
SaXGUI::SCCWidgetProfile* SaXWidgetProfile = new SaXGUI::SCCWidgetProfile();

//=====================================
// Globals
//-------------------------------------
QString SaXDisplay (":0");

//=====================================
// main program...
//-------------------------------------
int main (int argc,char*argv[]) {
	// .../
	// This is the main program building the graphical user
	// interface providing X11 configuration dialogs. The base
	// for all functionality is the libsax library.
	// ----
	//=====================================
	// init variables...
	//-------------------------------------
	bool mSetInfo    = false;
	bool mCheckPacs  = false;
	bool mFullScreen = false;
	bool mSetXIdle   = false;
	bool mYaSTMode   = false;
	bool mMiddle     = false;
	bool mSizeCheck  = false;
	if (getenv("DISPLAY") != NULL) {
		SaXDisplay = getenv("DISPLAY");
	}

	//=====================================
	// init variables...
	//-------------------------------------
	int  mGUIMode    = ISAX_CURRENT;

	//=====================================
	// init variables...
	//-------------------------------------
	QString* mRequestedDialog = NULL;
	QString* mIdlePID = NULL;

	//=====================================
	// check for root priviliges...
	//-------------------------------------
	#if 0
	if (getuid() != 0) {
		fprintf (stderr,"xapi: only root can do this\n");
		usage (1);
	}
	#endif

	//=====================================
	// allocate main qt object...
	//-------------------------------------
	for (int i=0;i<argc;i++) {
		//=====================================
		// handle --help option...
		//-------------------------------------
		if (QString(argv[i]) == "--help") {
			usage (0);
		}
		//=====================================
		// handle -display option...
		//-------------------------------------
		if (QString(argv[i]) == "-display") {
		if (argv[i+1]) {
			SaXDisplay = argv[i+1];
		}
		}
		//=====================================
		// handle commandline tool...
		//-------------------------------------
		if (QString(argv[i]) == "--") {
			int index = 0;
			char* args[argc];
			for (int n=0;n<argc;n++) {
				if (n < i) {
					continue;
				}
				args[index] = (char*)malloc(strlen(argv[n])+1);
				strcpy(args[index],argv[n]);
				index++;
			}
			args[index] = 0;
			execv (XCMD,args);
			fprintf (
				stderr,"xapi: can't run xcmd: %s\n",
				strerror(errno)
			);
			exit (1);
		}
	}
	QApplication SCCApp (argc,argv);

	//=====================================
	// start logging...
	//-------------------------------------
	logInit();

	//=====================================
	// get additional options...
	//-------------------------------------
	while (1) {
		int option_index = 0;
		static struct option long_options[] =
		{
			{"info"       , 0 , 0 , 'i'},
			{"yast"       , 0 , 0 , 'y'},
			{"checkpacs"  , 0 , 0 , 'c'},
			{"dialog"     , 1 , 0 , 'O'},
			{"fullscreen" , 0 , 0 , 'l'},
			{"help"       , 0 , 0 , 'h'},
			{"mode"       , 1 , 0 , 'm'},
			{"xidle"      , 0 , 0 , 'x'},
			{"pid"        , 1 , 0 , 'p'},
			{"sizecheck"  , 0 , 0 , 'S'},
			{"middle"     , 0 , 0 , 'M'}
		};
		int c = getopt_long (
			argc, argv, "icO:m:lhxp:yMS",long_options, &option_index
		);
		if (c == -1) {
			break;
		}
		switch (c) {
			case 'i':
				mSetInfo   = true;
			break;
			case 'y':
				mYaSTMode  = true;
			break;
			case 'M':
				mMiddle    = true;
			break;
			case 'S':
				mSizeCheck = true;
			break;
			case 'c':
				mCheckPacs = true;
			break;
			case 'x':
				mSetXIdle  = true;
			break;
			case 'p':
				mIdlePID = new QString (optarg);
			break;
			case 'O':
				mRequestedDialog = new QString (optarg);
			break;	
			case 'l':
				mFullScreen = true;
			break;
			case 'm':
				if (QString (optarg) == "auto") {
					mGUIMode = ISAX_AUTO;
				}
			break;
			case 'h':
				logExit(); usage (0);
			break;
			default:
				logExit(); usage (1);
			break;
		}
	}

	//=====================================
	// init program...
	//-------------------------------------
	Qt::WindowFlags wflags = Qt::WType_TopLevel;
	//QWidget::WindowFlags wflags = Qt::WType_TopLevel;
	int width  = qApp->desktop()->width();
	int height = qApp->desktop()->height();
	if ((mSizeCheck) && ((width <= 800) || (height <= 600))) {
		wflags |= Qt::WStyle_Customize | Qt::WStyle_NoBorder;
	}
	sccGUI = new SaXGUI::SCCFrame (
		mFullScreen, mGUIMode, mSetInfo, mCheckPacs,
		mYaSTMode, mSetXIdle, mIdlePID, mMiddle, mRequestedDialog, wflags
	);

	//=====================================
	// set signal handler...
	//-------------------------------------
	signal (SIGUSR2,SIG_IGN);
	signal (SIGINT ,gotInterrupted);

	//=====================================
	// enter main event loop...
	//-------------------------------------
	SCCApp.setMainWidget (sccGUI);
	if (! mRequestedDialog) {
		sccGUI->evaluateAutoDetection();
	} else {
		if (! sccGUI->startDialog ( *mRequestedDialog )) {
			log (L_ERROR, "No such configuration dialog: %s\n",
				mRequestedDialog->ascii()
			);
			logExit(); exit (1);
		}
	}
	sccGUI -> show();
	sccGUI -> setCommonButtonWidth();
	return SCCApp.exec();
}

//=====================================
// gotInterupted...
//-------------------------------------
void gotInterrupted (int) {
	if (sccGUI) {
		sccGUI -> slotCancel();
	}
}

//=====================================
// usage...
//-------------------------------------
void usage (int code) {
	// .../
	// usage information for options to call the SCC GUI
	// ----
	printf ("Linux SaX2 Version 8.1 startup level (xapi)\n");
	printf ("(C) Copyright 2005 SuSE GmbH\n");
	printf ("\n");
	printf ("usage: xapi [ options ]\n");
	printf ("options:\n");
	printf ("[ -m | --mode [ auto | current ]\n");
	printf ("   set startup mode to either initialize the GUI with the\n");
	printf ("   SaX2 auto probed data (auto) or the currently active\n");
	printf ("   configuration information (current)\n"); 
	printf ("[ -O | --dialog <name> ]\n");
	printf ("   start SaX2 with the given dialog name. The following\n");
	printf ("   dialogs are available for direct access:\n\n");
	printf ("   . Monitor\n");
	printf ("   . Mouse\n");
	printf ("   . Keyboard\n");
	printf ("   . Tablet\n");
	printf ("   . Touchscreen\n");
	printf ("   . VNC\n\n");
	printf ("[ -l | --fullscreen ]\n");
	printf ("   start in fullscreen mode, use one screen if\n");
	printf ("   Xinerama is in use\n");
	printf ("[ -i | --info ]\n");
	printf ("   show introduction box, used while starting on\n");
	printf ("   a separate X-Server\n");
	printf ("[ -M | --middle ]\n");
	printf ("   placement option to center the main window\n");
	printf ("[ -c | --checkpacs ]\n");
	printf ("   check if all packages needed to configure X11\n");
	printf ("   are installed. If not YaST2 is called to install\n");
	printf ("   the missing packages\n\n");
	printf ("[ -y | --yast ]\n");
	printf ("   activate YaST mode, which will switch off a view\n");
	printf ("   popup windows while running as child of YaST\n");
	printf ("[ -x | --xidle [ --pid <PID> ]]\n");
	printf ("   initalize an idle timer based on the MIT screensaver\n");
	printf ("   extension. The timer will wait until the signal\n");
	printf ("   SIGUSR2 has been sent to the timer. After the signal\n");
	printf ("   has been received a simple timer window appears\n\n");
	printf ("[ -- <commandline options> ]\n");
	printf ("   activate the commandline mode. For help about the\n");
	printf ("   current commandline options type \"-- --help\"\n");
	printf ("--\n");
	exit (code);
}

