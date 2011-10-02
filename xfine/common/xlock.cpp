/**************
FILE          : xlock.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : check for process blocking a file
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "xlock.h"
#include "log.h"

int getproc (char* name) {
	struct stat st;
	unsigned int inode;
	struct dirent *de;
	DIR *dir;
	int failed = 0;

	// stat file and save inode...
	// ----------------------------
	while (1) {
	if (stat(name,&st) < 0) {
		log (L_DEBUG,
			"stat failed on: %s -> %s\n",name,strerror(errno)
		);
		failed++;
		if (failed > 100) {
			break;
		} 
		usleep (500);
		continue;
    }
	break;
	}
	inode = st.st_ino;

	// walk proc system and search for inode...
	// ------------------------------------------
	if (! (dir = opendir("/proc"))) {
		log (L_ERROR,
			"open failed on: /proc -> %s\n",strerror(errno)
		);
		exit(1);
	}
    while ((de = readdir(dir))) {
		if (atoi(de->d_name) != 0) {
		char* path = (char*)malloc (sizeof(char) * 256);
		struct dirent *sub_de;
		DIR *subdir;
		sprintf(path,"/proc/%s/fd",de->d_name);
		if ((subdir = opendir(path))) {

		while ((sub_de = readdir(subdir))) {
		struct stat st;
		if (strcmp(sub_de->d_name,".") && strcmp(sub_de->d_name,"..")) {
			sprintf(path,"/proc/%s/fd/%s",de->d_name,sub_de->d_name);
			if (stat(path,&st) >= 0) {
			if (inode == st.st_ino) {
				closedir(subdir); closedir(dir);
				free(path);
				return(atoi(de->d_name));
			}
			}
		}
		}	
		closedir(subdir);
		free(path);
		}
		}
	} 
	closedir(dir);
	return(0);
}
