/**************
FILE          : log.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xapi (X11-Application-Interface)
              : header for log.cpp
              :
STATUS        : Status: Up-to-date
**************/
#ifndef LOG_H
#define LOG_H 1

//=================================
// Defines...
//---------------------------------
#define LOGFILEDIR      "/var/log"
#define LOG_MAX_LOGNAME	(256)

#define L_FATAL		(0)
#define L_ERROR		(1)
#define L_WARN		(2)
#define L_INFO		(4)
#define L_DEBUG		(8)
#define L_JUNK		(16)
#define L_STDERR	(32)
#define L_DEFAULT	(L_FATAL|L_ERROR|L_WARN|L_INFO|L_STDERR)

#define ETX  		(0x03)
#define NL   		(0x0A)
#define CR   		(0x0D)
#define DLE  		(0x10)
#define XON  		(0x11)
#define XOFF 		(0x13)
#define DC4  		(0x14)
#define CAN  		(0x18)
#define log			logLine


//=================================
// Structs...
//---------------------------------
struct logsequence {
	char code;
	const char *text;
};

//=================================
// Prototypes...
//---------------------------------
extern int	 logInit(void);
extern void	 logExit(void);
extern void	 logDebuglevel(long);
extern void	 logChar(long, const char);
extern void	 logCode(long, const char *);
extern void	 logText(long, const char *, ...);
extern void	 logLine(long, const char *, ...);

#endif
