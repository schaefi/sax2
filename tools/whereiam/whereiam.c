/**************
FILE          : whereiam.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHORS       : Thomas Fehr <fehr@suse.de>
              : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : whereiam will check if the currently used
              : X11 Display link point to a remote or a local
              : display: Exit codes:
              : ---
              : 0 -> local display
              : 1 -> remote display
              : ---
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <ctype.h>
#include <time.h>
#include <utmp.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "proc/readproc.h"
#include "proc/devname.h"
#include "remote.h"

//=====================================
// Defines
//-------------------------------------
#define USERSZ (sizeof u->ut_user)

//=====================================
// Structures
//-------------------------------------
typedef struct utmp utmp_t;

//=====================================
// Globals
//-------------------------------------
static proc_t **procs;

//=====================================
// Functions
//-------------------------------------
proc_t *getproc (utmp_t *u, char *tty, int *jcpu, int *found_utpid);

//=====================================
// main
//-------------------------------------
int main (void) {
	char *user   = NULL;
	char *sshtty = NULL;
	char *dpy    = NULL;
	utmp_t *u;
	char tty[128] = "";
	
	// 1) ...
	// display not remote if first character of the
	// DISPLAY variable is a colon or the DISPLAY variable
	// contains the name of the local hostname in combination
	// with the display number smaller than 10 (SSH uses number > 10)
	// ---
	dpy = getenv ("DISPLAY");
	if (dpy) {
	if (dpy[0] == ':') {
		exit (0);
	} else {
		char* xhost = strtok (dpy,":");
		char* xvt   = strtok (NULL,".");
		if ((! remoteDisplay (xhost)) && (atoi(xvt) < 10)) {
			exit (0);
		}
	}
	}
	// 2) ...
	// display is a remote display if the SSH_TTY point
	// to a remote host
	// ---
	procs  = readproctab (
		PROC_FILLCMD | PROC_FILLUSR
	);
	sshtty = getenv("SSH_TTY");
	if (sshtty) {
	utmpname(UTMP_FILE);
	setutent();
	while ((u=getutent())) {
	if (
		u->ut_type == USER_PROCESS && 
		(user ? !strncmp(u->ut_user, user, USERSZ) : *u->ut_user)
	) {
		if (strcmp(u->ut_host,"") != 0) {
		sprintf(tty,"/dev/%s",u->ut_line);
		if (strcmp(tty,sshtty) == 0) {
			exit (
				remoteDisplay (u->ut_host)
			);
		}
		}
	}
	}
	endutent();
	}
	// 3) ...
	// display is a remote display if the hostname within
	// the DISPLAY variable point to a remote host
	// ---
    exit (
		remoteDisplay (NULL)
	);
}

//=====================================
// getproc
//-------------------------------------
proc_t *getproc (utmp_t *u, char *tty, int *jcpu, int *found_utpid) {
	// ...
	// This function scans the process table accumulating total cpu times for
	// any processes "associated" with this login session.  It also searches
	// for the "best" process to report as "(w)hat" the user for that login
	// session is doing currently.  This the essential core of 'w'.
	// ---
	int line;
	proc_t **p, *best = NULL, *secondbest = NULL;

	line = tty_to_dev(tty);
	*jcpu = *found_utpid = 0;
	for (p = procs; *p; p++) {
	if ((**p).pid == u->ut_pid)
		*found_utpid = 1;
	if ((**p).tty == line) {
		(*jcpu) += (**p).utime + (**p).stime;
		secondbest=*p;
		if ((!strncmp((**p).euser, u->ut_user, USERSZ)) &&
		((**p).pid == (**p).tpgid && (!best || (**p).pid > best->pid)))
		best = *p;
	}
	}
	if (!best) {
		return secondbest;
	}
	return best;
}
