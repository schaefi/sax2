/**************
FILE          : xidle.c
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.0
              :  
DESCRIPTION   : idle looks if there is any action on the
              : display after a few seconds
              : 
STATUS        : Status: Up-to-date
**************/
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdSel.h>
#include <X11/Xmd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xcms.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/keysym.h>
#include <sys/wait.h>

// ===================================
// Defines...
// -----------------------------------
#define PATH_DEV_NULL "/dev/null"
#define SOFT  15
#define HARD  9
#define TTL   30
#define ESC   9

// ===================================
// Globals...
// -----------------------------------
int timeout = TTL;
int timeval = TTL;
int pid     = 0;
int tpid    = 0;
int output  = 0;
int ss_event;
int ss_error;

// ===================================
// Prototypes...
// -----------------------------------
Display* XOpen(char name[256]);
void usage (void);
void settimer(int sec);
int  gettimer(void);
void timerend(int s);
void timerstatus(int s);
void timersignal(void);
void triggerreset(void);
void resettimer(int s);
int  daemon(int nochdir, int noclose);
void setsaver(Display *dpy, int value); 
void handlesighup   (int s);
void handlesigchld  (int s);
void handlesigreset (int s);
void Exit(void);

// ===================================
// main...
// -----------------------------------
int main (int argc,char *argv[]) {
	XSetWindowAttributes attr;
	Display* dpy;
	Window root;
	char mydisp  [256] = "";
	int  fg            = 0;
	int  screen;
	int  c;
	// ============================
	// get commandline options...
	// ----------------------------
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"display"    , 1 , 0 , 'd'},
		{"pid"        , 1 , 0 , 'p'},
		{"help"       , 0 , 0 , 'h'},
		{"timeout"    , 1 , 0 , 't'},
		{"foreground" , 0 , 0 , 'f'},
		{"output"     , 0 , 0 , 'o'},
		{0            , 0 , 0 , 0  }
	};
	c = getopt_long (
		argc, argv, "hd:p:t:fo",long_options, &option_index
	);
	if (c == -1)
	break;

	switch (c) {
	case 'h':
		usage();
		exit(0);

	case 't':
		timeout = atoi(optarg);
	break;

	case 'd':
		strcpy(mydisp,optarg);
	break;

	case 'p':
		pid = atoi(optarg);
	break;

	case 'o':
		output = 1;
	break;

	case 'f':
		fg = 1;
	break;

	default:
		usage();
		exit(1);
	}
	}

	// prove pid...
	// --------------
	if (pid == 0) {
		printf("xidle: no valid pid defined\n");
		exit(1);
	}

	// open display...
	// ------------------
	if (strcmp(mydisp,"") == 0) {
		dpy = XOpen("null");
	} else {
		dpy = XOpen(mydisp);
	}
	if (dpy == NULL) {
		printf("xidle: unable to open display: %s\n",mydisp); 
		exit(1);
	}

	// go for daemon...
	// -----------------
	if (fg == 0) {
		daemon(1,0);
	}

	// install signal handler...
	// --------------------------
	signal( SIGHUP  , handlesighup   );
	signal( SIGCHLD , handlesigchld  );
	signal( SIGTRAP , timerstatus    );
	signal( SIGCONT , handlesigreset );

	// default window is the root window...
	// -------------------------------------
	root   = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);

	// look if the MIT extension is available...
	// -------------------------------------------
	if (!XScreenSaverQueryExtension (dpy, &ss_event, &ss_error)) {
		exit(1);
	}
	XScreenSaverSelectInput (dpy, root, ScreenSaverNotifyMask);
	XSync (dpy, False);

	// select keyboard for press and release...
	// ------------------------------------------
	attr.event_mask = KeyPressMask | PointerMotionMask;
	XSelectInput(dpy, DefaultRootWindow(dpy), attr.event_mask);

	// set internal screen saver timeout...
	// --------------------------------------- 
	timersignal();
	setsaver(dpy,timeout);

	while(1) {
		XEvent event; 
		XScreenSaverNotifyEvent *sevent;
		XNextEvent (dpy, &event);
		// ==========================================
		// reset timer for pointer motion events...
		// ------------------------------------------
		if (event.type == MotionNotify) {
			triggerreset();
		}
		// =====================================================
		// reset timer for keyevents differently from Escape...
		// -----------------------------------------------------
		else if (event.type == KeyPress) {
			XKeyEvent *e = (XKeyEvent*)&event;
			if (e->keycode == ESC) {
				Exit();
			} else {
				triggerreset();
			}
		}
		// ==========================================
		// catch screen saver event...
		// ------------------------------------------
		else if (event.type == ss_event) {
			sevent = (XScreenSaverNotifyEvent *) &event;
			if (sevent->state == ScreenSaverOn) {
			// get screen saver event...
			// --------------------------
			Exit();
			}
		}
		// ==============================================
		// put back event to the queue if not handled...
		// ----------------------------------------------
		else {
			XPutBackEvent(dpy,&event);
		}
	}
	XCloseDisplay(dpy);
	return(0);
}


// ===================================
// handlesighup...
// -----------------------------------
void handlesighup ( int s) {
	kill(pid,SOFT); sleep(5); kill(pid,HARD);
}

// ===================================
// handlesigchld...
// -----------------------------------
void handlesigchld  (int s) {
	union wait wstatus;
	while (wait3(&wstatus,WNOHANG,NULL) > 0);
}

// ===================================
// settimer...
// -----------------------------------
void settimer(int sec) {
	struct itimerval value;
	memset (&value,0,sizeof(value));
	signal (SIGALRM,timerend);
	value.it_value.tv_sec  = sec;
	setitimer(ITIMER_REAL, &value, 0);
}

// ===================================
// gettimer...
// -----------------------------------
int gettimer(void) {
	struct itimerval value;
	getitimer(ITIMER_REAL,&value);
	return(value.it_value.tv_sec);
}

// ===================================
// timersignal...
// -----------------------------------
void timersignal(void) {
	int pid;
	int val;
	switch (tpid = fork()) {
	case 0:
	signal(SIGUSR2,resettimer);
	settimer(timeout);
	pid = getppid();
	while(1) {
		val = gettimer(); sleep(1);
		kill (pid,SIGTRAP);
	}
	}
}

// ===================================
// timerend...
// -----------------------------------
void timerend(int s) {
	//...
}

// ===================================
// timerstatus...
// -----------------------------------
void timerstatus(int s) {
	if (output == 1) {
		printf("%d\n",timeval--);
		fflush(stdout);
	}
}

// ===================================
// resettimer...
// -----------------------------------
void resettimer(int s) {
	settimer(timeout);
}

// ===================================
// triggerreset...
// -----------------------------------
void triggerreset(void) {
	kill (tpid,SIGUSR2);
	timeval = timeout;
}

// ===================================
// handlesigreset...
// -----------------------------------
void handlesigreset (int s) {
	triggerreset();
}

// ===================================
// Exit...
// -----------------------------------
void Exit(void) {
	kill (tpid,SOFT);
	kill (pid ,SOFT);
	exit(0);
}

// ===================================
// XOpen...
// -----------------------------------
Display* XOpen(char name[256]) {
	Display *dpy;
	char display[256] = "";

	if (strcmp(name,"null") == 0) {
	if (getenv("DISPLAY") != NULL) {
		strcpy(display,getenv("DISPLAY"));
	}
	} else {
		strcpy(display,name);
	}
	if (!(dpy = XOpenDisplay(display))) {
		return(NULL);
	}
	return(dpy);
}

// ===================================
// setsaver...
// -----------------------------------
void setsaver(Display *dpy, int value) {
	int timeout; 
	int interval; 
	int prefer_blank; 
	int allow_exp;

	XGetScreenSaver(dpy,&timeout,&interval,&prefer_blank,&allow_exp);
	timeout = value;
	XSetScreenSaver(dpy,timeout,interval,prefer_blank,allow_exp);
	return;
}

// ===================================
// daemon...
// -----------------------------------
int daemon(int nochdir, int noclose) {
	int fd;
	switch (fork()) {
	case -1:
		return (-1);
	case 0:
		break;
	default:
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

// ===================================
// usage...
// -----------------------------------
void usage(void) {
	printf("usage: xidle [ options ]\n");
	printf("options:\n");
	printf("  [ -p | --pid ]\n");
	printf("    set the process ID to kill after ttl\n");
	printf("    TTL is set to 30 sec at the time\n");
	printf("  [ -d | --display ]\n");
	printf("    set display to use\n");
	printf("  [ -t | --timeout ]\n");
	printf("    set timeout for idle period\n");
	printf("  [ -f | --foreground ]\n");
	printf("    do not fork into the background\n");
	printf("--\n");
}
