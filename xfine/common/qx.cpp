/**************
FILE          : qx.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : common used functions: 
              : qx -> call program and return stdout data
              :
STATUS        : development
**************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <libgen.h>

#include "qx.h"
#include "log.h"
#include "xlock.h"

//=================================
// Functions...
//---------------------------------
char* qx (const char* command,int channel,int anz,const char* format,...) {
	char data[30] = "/tmp/data";
	char *result  = NULL;
	FILE* new_channel = NULL;
	char *arg[anz+2];
	char *cmdstr = NULL;
	va_list ap;
	FILE *fd;
	int n=1;
	long size=0;
	int child;
	int items;

	signal( SIGCHLD , handlesigchld );
	sprintf(data,"%s-%d",data,getpid());

	//=================================
	// call the program (prefork)...
	//---------------------------------
	switch ( child = fork() ) {

	//=================================
	// Check system call...
	//---------------------------------
	case -1:
		log(L_ERROR,
			"qx: could not fork: %s\n",strerror(errno)
		);
		exit(1);
	break;

	//=================================
	// Handle child process...
	//---------------------------------
	case 0:
		// ... /
		// prepare arguments for execvp...
		// ---
		arg[0] = (char*)malloc(sizeof(char)*MAX_PROGRAM_SIZE);
		cmdstr = (char*)malloc(sizeof(char)*strlen(command));
		strcpy (cmdstr,command);
		strcpy(arg[0],basename (cmdstr));
		arg[anz+1] = NULL;
		if (format != NULL) {
			va_start(ap, format);
			while (*format) {
				switch(*format++) {
				case 's':
				arg[n] = strdup(va_arg(ap, char*));
				n++;
				break;
				}
			}
			va_end(ap);
		}
		arg[n] = NULL;
		if (channel == 0) {
			new_channel = freopen(data,"w",stdout);
		}
		if (channel == 1) {
			new_channel = freopen(data,"w",stderr);
		}
		execvp (command,arg);
		// ... /
		// the following code shouldn't be reached
		// ---
		if (new_channel) {
			fclose (new_channel);
			unlink (data);
		}
		log(L_ERROR,
			"qx: could not execute command: %s\n",command
		);
		exit (0);
	break;

	//=================================
	// Handle parent process...
	//---------------------------------
	default:
		waitpid (child,NULL,0);
		if ((channel == 0) || (channel == 1)) {
		while (getproc(data)) {
			usleep(500);
		}
		if (! (fd = fopen(data,"r"))) {
			return(NULL);
		}
		fseek(fd,0L,SEEK_END);
		size = ftell(fd) - 1;
		rewind(fd);
		if (size <= 0) {
			fclose(fd); unlink(data); 
			log(L_INFO,
    	    	"qx: command returned (null) string\n"
			);
			return(NULL);
		}
		result = (char*)malloc(sizeof(char)*size + 1);
		items = fread(result,size,1,fd);
		result[size] = '\0';
		fclose(fd);
		unlink(data);
		}
		return(result);
	}
}

void handlesigchld  (int) {
	union wait wstatus;
	while (wait3(&wstatus,WNOHANG,NULL) > 0);
}
