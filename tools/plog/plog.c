/**************
FILE          : plog.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : PLog.pm perl module function 
              : implementation 
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "../../sysp/lib/syslib.h"

//=================================
// Functions...
//---------------------------------
MsgDetect* PLogParse (char* logfile);
char* PLogGetResolution (MsgDetect* pm);
char* PLogGetVideoRam   (MsgDetect* pm);
char* PLogGetMonitorManufacturer (MsgDetect* pm);
char* PLogCheckDisplay (MsgDetect* pm);
char* PLogGetVMwareColorDepth (MsgDetect* pm);
char* PLogGetDisplaySize (MsgDetect* pm);

//=================================
// Test in main...
//---------------------------------
int main (int argc,char* argv[]) {
	MsgDetect *parse = NULL;
	char *bla;

	if ( ! argv[1] ) {
		printf ("No file given... abort\n");
		exit (1);
	}

	parse = PLogParse(argv[1]);
	if (parse) {
		bla = PLogGetVideoRam(parse);
		printf("%s\n",bla);

		bla = PLogGetResolution(parse);
		printf("%s\n",bla);

		bla = PLogGetDisplaySize(parse);
		printf("%s\n",bla);
	}
	exit(0);
}


//=================================
// Implementation...
//---------------------------------
MsgDetect* PLogParse (str logfile) {
	MsgDetect *parse = NULL;
	parse = PLogGetData(logfile);
	return(parse);
}

char* PLogGetVideoRam (MsgDetect* pm) {
	char *vram = NULL;
	vram = (char*)malloc(MAX_PROGRAM_SIZE);
	sprintf(vram,"%ld",pm[0].memory);
	return(vram);
}

char* PLogGetResolution (MsgDetect* pm) {
	char *result = NULL;
	int n = 0;
	str res;

	if (pm[0].vesacount <= 0) {
		result = (char*)malloc(MAX_PROGRAM_SIZE);
		strcpy(result,"none");
		return(result);
	}

	result = (char*)malloc(MAX_PROGRAM_SIZE*pm[0].vesacount);
	strcpy(result,"");
	for (n=0;n<pm[0].vesacount;n++) {
		sprintf(res,"%dx%d:",pm[0].vmodes[n].x,pm[0].vmodes[n].y);
		strcat(result,res);
	}
	result[strlen(result)-1] = '\0';
	return(result);
}

char* PLogGetMonitorManufacturer (MsgDetect* pm) {
	char *ddc = NULL;
	ddc = (char*)malloc(MAX_PROGRAM_SIZE);
	sprintf(ddc,"%s",pm[0].ddc);
	return(ddc);
}

char* PLogCheckDisplay (MsgDetect* pm) {
	char *type = NULL;
	type = (char*)malloc(MAX_PROGRAM_SIZE);
	sprintf(type,"%s",pm[0].displaytype);
	return(type);
}

char* PLogGetVMwareColorDepth (MsgDetect* pm) {
	char *vmdepth = NULL;
	vmdepth = (char*)malloc(MAX_PROGRAM_SIZE);
	sprintf(vmdepth,"%d",pm[0].vmdepth);
	return(vmdepth);
}

char* PLogGetDisplaySize (MsgDetect* pm) {
	char *dpi = NULL;
	dpi = (char*)malloc(MAX_PROGRAM_SIZE);
	sprintf(dpi,"%dx%d",pm[0].dpix,pm[0].dpiy);
	return(dpi);
}
