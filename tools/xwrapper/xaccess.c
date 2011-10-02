/**************
FILE          : xaccess.c
***************
PROJECT       : SaX2 ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool X11 version 4.x
              :  
DESCRIPTION   : XAccess use the XKB extension to use the
              : numeric keyblock as virtual mouse. This will 
              : replace the old wrap and fake code
              : 
              : 
STATUS        : Status: Up-to-date
**************/
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBgeom.h>
#include <X11/extensions/XKM.h>
#include <X11/extensions/XKBfile.h>
#include <X11/extensions/XKBrules.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>
#include <X11/XKBlib.h>
#include <unistd.h>

#include "xaccess.h"

//========================================
// XAccess...
//----------------------------------------
void XAccess(Display *dpy,char *displayname) {
 #ifndef __powerpc__
 XkbDescPtr xkb;
 int major, minor,op, event, error;
 int timeout = 5;

 // Query XKB Extension...
 // -----------------------
 major = XkbMajorVersion;
 minor = XkbMinorVersion;
 if (XkbQueryExtension(dpy, &op, &event, &error, &major, &minor) == 0) {
  printf("xaccess: Unable to initialize XKEYBOARD extension");
  exit(1);
 }
 while (timeout > 0) {
  xkb = XkbGetKeyboard(dpy, XkbGBN_AllComponentsMask, XkbUseCoreKbd);
  if (xkb == NULL || xkb->geom == NULL) {
   timeout -= 1; sleep(1);
  } else {
   break;
  }
 }
 if (timeout <= 0) {
  printf("xaccess: Couldn't get keyboard\n");
  exit(1);
 }
 
 // Everything is fine, now set the access block...
 // ------------------------------------------------ 
 XkbGetControls(dpy, XkbAllControlsMask, xkb);
 if (xkb->ctrls == NULL) {
  xkb->ctrls = (XkbControlsPtr)malloc(sizeof(XkbControlsRec));
 }

 xkb->ctrls->enabled_ctrls |= XkbMouseKeysMask | XkbMouseKeysAccelMask;
 xkb->ctrls->mk_delay       = 40;
 xkb->ctrls->mk_interval    = 10;
 xkb->ctrls->mk_time_to_max = 1000;
 xkb->ctrls->mk_max_speed   = 500;
 xkb->ctrls->mk_curve       = 0;

 // Set keyboard control...
 // ------------------------
 XkbSetControls(dpy, XkbAllControlsMask, xkb);
 XSync(dpy, False);
 #endif
}


//==============================
// XOpen...
//------------------------------
Display* XOpen(char *name) {
 char *display = NULL;
 Display *dpy;

 display = (char*)malloc(sizeof(char)*256);
 if (name != NULL) {
  strcpy(display,name);
 } else {
  display = getenv("DISPLAY");
  if (display == NULL) {
   return(NULL);
  }
 }
 if(!(dpy = XOpenDisplay(display))) {
  return(NULL);
 }
 return(dpy);
}
