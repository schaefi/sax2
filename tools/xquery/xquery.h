/**************
FILE          : xquery.c
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
              :
BESCHREIBUNG  : xquery delivers display information and take
              : care about multihead setups. This is the header
              : definition for xquery.c
              : 
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XQUERY_H
#define XQUERY_H 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdSel.h>
#include <X11/Xmd.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xinerama.h>

#include "string.h"

//========================
// Globals...
//------------------------
XineramaScreenInfo *screens;
int EventBase, ErrorBase;
int MajorVersion, MinorVersion;
int dot_clock, mode_flags;
int scr[100];
int scr_count = 1;

//========================
// Defines...
//------------------------
#define V_FLAG_MASK 0x1FF;
#define V_PHSYNC    0x001 
#define V_NHSYNC    0x002
#define V_PVSYNC    0x004
#define V_NVSYNC    0x008
#define V_INTERLACE 0x010 
#define V_DBLSCAN   0x020
#define V_CSYNC     0x040
#define V_PCSYNC    0x080
#define V_NCSYNC    0x100

#define TRUE        1
#define FALSE       0

typedef enum {
 Back = -1,Next = 1
} direction;

typedef enum {
 HDisplay, HSyncStart, HSyncEnd, HTotal,
 VDisplay, VSyncStart, VSyncEnd, VTotal, Flags, 
 InvertVclk, BlankDelay1, BlankDelay2, EarlySc,
 PixelClock, HSyncRate, VSyncRate, fields_num 
} fields;

typedef struct {
  fields  me;
  fields  use;
  int     val;
  int     lastpercent;
  int     range;
} ScrollData;

static struct _AppResources {
  ScrollData    field[fields_num];
  Bool          ad_installed;
  int       orig[fields_num];
  int       old[fields_num];
} AppRes = {
    {
    { HDisplay, },
    { HSyncStart, HDisplay, },
    { HSyncEnd, HDisplay, },
    { HTotal, HDisplay, },
    { VDisplay, },
    { VSyncStart, VDisplay, },
    { VSyncEnd, VDisplay, },
    { VTotal, VDisplay, },
    { Flags, },
    { InvertVclk, },
    { BlankDelay1, },
    { BlankDelay2, },
    { EarlySc, },
    { PixelClock, },
    { HSyncRate, },
    { VSyncRate, },
    },
};

static Bool S3Specials = False;
static char modebuf[160];
Window win;

//=====================
// Prototypes...
//---------------------
void usage (void);
Display* XOpen (char name[256],int screen);
void CountScreens (Display *dpy);
void CleanUp (Display *dpy,int scr);
void SwitchMode (Display *dpy, int call, int scr);
Bool GetModeLineData (Display* dpy, int scr);
void ShowModeline (void);
void ApplyModeline (Display *dpy, int scr);
int PrepareModeline (
	Display* dpy,int current_screen,
    int h1,int h2,int h3,int h4,int v1,int v2,int v3,int v4
);
Bool IsXinerama (Display *dpy);
#endif
