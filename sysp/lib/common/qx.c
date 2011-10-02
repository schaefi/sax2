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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#define new __new
#include <linux/vt.h> 
#include <linux/kd.h>
#include <string.h>

#include "../syslib.h"

//=================================
// Functions...
//---------------------------------
char* qx(const char*command,int channel,int anz,const char* format,...) {
	const char prefix[30] = "/tmp/data";
	char data[30] = "";
	char *result  = NULL;
	char *arg[anz+2];
	va_list ap;
	FILE *fd;
	int n=1;
	long size=0;
	int items=0;

	//=================================
	// prepare arguments for execvp...
	//---------------------------------
	arg[0] = (char*)malloc(sizeof(char)*MAX_PROGRAM_SIZE);
	strcpy(arg[0],command);
	arg[anz+1] = NULL;

	//=================================
	// prepare data file...
	//---------------------------------
	sprintf(data,"%s-%d",prefix,getpid());

	//=================================
	// get arguments...
	//---------------------------------
	if (format != NULL) {
		va_start(ap, format);
		while (*format) {
			switch(*format++) {
				case 's':
					arg[n] = va_arg(ap, char*);
					n++;
				break;
				default:
					// nothing to do for other formats
				break;
			}
		}
		va_end(ap);
	}

	//=================================
	// call the program...
	//---------------------------------
	switch(fork()) {
		case -1:
			perror ("fork");
			exit (1);
		break;

		case 0:
			//=================================
			// child process...
			//---------------------------------
			if (channel == 0) {
				freopen(data,"w",stdout);
			}
			if (channel == 1) {
				freopen(data,"w",stderr);
			}
			execvp (command,arg);
		break;

		default:
			//=================================
			// parent process...
			//---------------------------------
			wait (NULL);

			if ((channel == 0) || (channel == 1)) {
				fd = fopen(data,"r");
				if (fd == NULL) {
					return NULL;
				}
				fseek(fd,0L,SEEK_END);
				size = ftell(fd) - 1;
				rewind(fd);
				if (size <= 0) {
					fclose(fd); return NULL;
				}
				result = (char*)malloc(sizeof(char)*size + 1);
				items = fread(result,size,1,fd);
				result[size] = '\0';
				fclose(fd);
				unlink(data);
			}
		break;
	}
	return result;
}

//=====================================
// Get console file descriptor
//-------------------------------------
int is_a_console (int fd) {
	char arg = 0;
	return (
		ioctl(fd, KDGKBTYPE, &arg) == 0 && ((arg == KB_101) || (arg == KB_84))
	);
}
int open_a_console (const char *fnam) {
	int fd = open(fnam, O_RDWR);
	if (fd < 0 && errno == EACCES) {
		fd = open(fnam, O_WRONLY);
	}
	if (fd < 0 && errno == EACCES) {
		fd = open(fnam, O_RDONLY);
	}
	if (fd < 0) {
		return -1;
	}
	if (!is_a_console(fd)) {
		close(fd);
		return -1;
	}
	return fd;
}
int getfd (void) {
	int fd;
	fd = open_a_console("/dev/tty");
	if (fd >= 0) {
		return fd;
	}
	fd = open_a_console("/dev/tty0");
	if (fd >= 0) {
		return fd;
	}
	fd = open_a_console("/dev/vc/0");
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
	return -1;
}

//=====================================
// Change virtual terminal
//-------------------------------------
// bnc 504728: on some machines the intel agpgart takes a long time
// for memory cleanup, thus a race condition triggers and the VT_ACTIVATE
// never gets through. Thus do it every second.
static int chvt_fd, chvt_num, chvt_ret;
static void chvt_alrm_handler (int signum)
{
	if (ioctl(chvt_fd, VT_ACTIVATE, chvt_num)) {
		chvt_ret = 0;
	}
	alarm (1);
}
int chvt (int num) {
	int fd = getfd();
	int ret;
	struct sigaction act;
	if (fd < 0) {
		return 0;
	}
	if (ioctl(fd,VT_ACTIVATE,num)) {
		return 0;
	}
	memset (&act, 0, sizeof(struct sigaction));
	act.sa_handler = chvt_alrm_handler;
	act.sa_flags   = SA_RESTART;
	sigaction (SIGALRM, &act, NULL);
	// Yes, this is not multithread-proof. And ugly.
	chvt_fd  = fd;
	chvt_num = num;
	chvt_ret = 1;
	alarm (1);
	while ( (ret = ioctl(fd,VT_WAITACTIVE,num)) == -1 && errno == EINTR && chvt_ret)
		;
	alarm (0);
	act.sa_handler = SIG_DFL;
	sigaction (SIGALRM, &act, NULL);
	return (ret == 0);
}   

//=====================================
// Get current virtual terminal
//-------------------------------------
int getvt (void) {
	int fd  = getfd();
	struct vt_stat status;
	if (fd < 0) {
		return 0;
	}
	if (ioctl(fd,VT_GETSTATE,&status)) {
		return -1;
	}
	return status.v_active;
}
