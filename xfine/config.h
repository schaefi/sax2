/**************
FILE          : config.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning tool)
              : global configuation settings for the XFine2
              : 
              :
STATUS        : Status: Up-to-date
**************/
#ifndef CONFIG_H
#define CONFIG_H 1

//==============================
// Pixmaps and translations...
//------------------------------
#define PIXDIR          "/usr/share/xfine/figures/"
#define XFINEGTX        "/usr/share/xfine/xfine.gtx"

#define LOGO            PIXDIR "suse.gif"
#define DRI_ENABLED     PIXDIR "dri-enabled.gif"
#define DRI_DISABLED    PIXDIR "dri-disabled.gif" 
#define PIXRIGHT        PIXDIR "xfine-right.xpm"
#define PIXLEFT         PIXDIR "xfine-left.xpm"
#define PIXUP           PIXDIR "xfine-up.xpm"
#define PIXDOWN         PIXDIR "xfine-down.xpm"
#define PIXSHORTER      PIXDIR "xfine-shorter.xpm"
#define PIXTALLER       PIXDIR "xfine-taller.xpm"
#define PIXWIDER        PIXDIR "xfine-wider.xpm"
#define PIXNARROWER     PIXDIR "xfine-narrower.xpm"

//==============================
// Startup options...
//------------------------------
#define TDOMAIN         "/usr/share/locale"

//==============================
// Cache directory...
//------------------------------
#define CACHE           "/var/cache/xfine"

//==============================
// Binary files...
//------------------------------
#define KILLALL         "/usr/bin/killall"
#define XQUERY          "/usr/sbin/xquery"
#define XSETROOT        "xsetroot"

//==============================
// QX paramters...
//------------------------------
#define STDOUT          0
#define STDERR          1
#define STDNONE         2

//==============================
// LOGGING definitions...
//------------------------------
#define LOGFILEDIR      "/var/log"

#endif
