/**************
FILE          : log.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : message logging implementation
              :
STATUS        : Status: Up-to-date
**************/
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <libintl.h>
                     
#include "log.h"

//====================================
// Global variables...
//------------------------------------
static FILE *logsio     = NULL;
static char *logname    = NULL;
static long	logdbg      = L_DEFAULT;
static char baseName[5] = "SaX2";

//====================================
// Structs...
//------------------------------------
static struct logsequence logsequence[] =
{
	{ ETX ,  "<ETX>" }, { NL  ,   "<NL>" }, { CR  ,   "<CR>" },
	{ DLE ,  "<DLE>" }, { XON ,  "<XON>" }, { XOFF, "<XOFF>" },
	{ DC4 ,  "<DC4>" }, { CAN ,  "<CAN>" }, { 0   ,     NULL }
};

//====================================
// logInit...
//------------------------------------
int logInit(void) {
	size_t size;
	size = (strlen(LOGFILEDIR) + strlen("SaX.log") + 2);
	if ((logname = (char *)malloc(size))) {
	sprintf(logname, "%s/SaX.log", LOGFILEDIR);
	if ((logsio = fopen(logname, "a"))) {
		logDebuglevel(L_DEFAULT);
		return(1);
	} else {
	log (L_STDERR, 
		"%s: Can't open log '%s'.\n", baseName, logname
	);
	}
	} else {
	log (L_STDERR, 
		"%s: Not enough memory to allocate logname.\n", baseName
	);
	}
	return(0);	
}

//====================================
// logExit...
//------------------------------------
void logExit(void) {
	if (logsio) {
		fclose(logsio); logsio = NULL;
	}
	if (logname) {
		free(logname); logname = NULL;
	}
}

//====================================
// logDebuglevel...
//------------------------------------
void logDebuglevel(long level) {
	logdbg = level;
}

//====================================
// logLine...
//------------------------------------
void logLine (long level, const char *fmt, ...) {
	struct tm   *timel;
	time_t       timec;
	va_list      arg;
	char		 logsign;
	char         timeline[20];
	
	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR)) {
	if (!logsio) level = L_STDERR;

	if (level == L_STDERR) {
		va_start(arg, fmt);
		vfprintf(stderr, fmt, arg);
		va_end(arg);
	}
	else {
		timec = time(NULL);
                       
		if ((timel = localtime(&timec))) {
			setlocale (LC_ALL,"POSIX");
			if (strftime(timeline, 20, "%d-%b %H:%M:%S", timel) != 15) {
				strcpy(timeline, "\?\?-\?\?\? \?\?:\?\?:\?\?");
			}
			setlocale (LC_ALL,"");
		}

		switch (level) {
		case L_FATAL:
		logsign = 'F';
		break;

		case L_ERROR:
		logsign = 'E';
		break;

		case L_WARN:
		logsign = 'W';
		break;

		case L_INFO:
		logsign = 'I';
		break;

		case L_DEBUG:
		logsign = 'D';
		break;

 		case L_JUNK:
		logsign = 'J';
		break;

		default:
		logsign = '?';
		break;
		}

		fprintf(logsio, "%s <%c> ", timeline, logsign);

		va_start(arg, fmt);
		vfprintf(logsio, fmt, arg);
		va_end(arg);

		fflush(logsio);
	}
	}
}

//====================================
// logChar...
//------------------------------------
void logChar(long level, const char c) {
	int i;
	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR)) {
	if (!isprint(c)) {
		i = 0;
		while (logsequence[i].text) {
			if (logsequence[i].code == c) {
				logText(level, "%s", logsequence[i].text);
				return;
			}
			i++;
		}
		logText(level, "[0x%02X]", (unsigned char)c);
	} else {
		logText(level, "%c", c);
	}
	}
}

//====================================
// logText...
//------------------------------------
void logText(long level, const char *fmt, ...) {
	FILE *useio;
	va_list arg;

	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR)) {
	if ((!logsio) || (level == L_STDERR)) {
		useio = stderr;
		level = L_STDERR;
	} else {
		useio = logsio;
	}

	va_start(arg, fmt);
	vfprintf(useio, fmt, arg);
	va_end(arg);

	fflush(useio);
	}
}

//====================================
// logCode...
//------------------------------------
void logCode(long level, const char *sequence) {
	int i;

	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR)) {
	for (i = 0; i < (int)strlen(sequence); i++) {
		logChar(level, sequence[i]);
	}
	}
}
