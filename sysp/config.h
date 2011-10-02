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
DESCRIPTION   : Global definitions and configuration   
              : options for sysp
              :
STATUS        : Status: Up-to-date
**************/

#ifndef CONFIG_H

//==============================
// machine types...
//------------------------------ 
#define CONFIG_H          1
#define ARCH_PPC          "ppc"
#define ARCH_SPARC        "sparc"

//==============================
// paths...
//------------------------------
#define RC_SYSCONFIG      "/etc/sysconfig/"
#define INPUT_MAP         "/usr/share/sax/sysp/maps/Input.map"
#define DRIVER_MAP        "/usr/share/sax/sysp/maps/Driver.map"
#define KBD_MAP           "/usr/share/sax/sysp/maps/Keyboard.map"
#define VENDOR_MAP        "/usr/share/sax/sysp/maps/Vendor.map"
#define IDENTITY          "/usr/share/sax/sysp/maps/Identity.map"
#define IDENTITY_UPDATED  "/usr/share/sax/sysp/maps/update/"
#define PROFILEDIR        "/usr/share/sax/profile/"

//==============================
// Expressions...
//------------------------------
#define FLAG_DEFAULT      "DEFAULT"
#define FLAG_3D           "3D"
#define TEXT_3D           "Do you want to enable 3D for this card ?"
#define TMP_CONFIG        "/tmp/sysdata"
#define XW_LOG            "/tmp/xwlog"
#define XWRAPPER          "/usr/sbin/xw"
#define PROBE             "-probeonly"
#define CONFIG            "-xf86config"
#define BLANK             "-blank"
#define VERBOSE           "-logverbose"
#define NO_FLAG_QUESTION  99
#define FB_DEV            "/dev/fb0"
#define KILL              "/usr/bin/killall"
#define QTASK             "/usr/sbin/xapi"
#define STDOUT            0
#define STDERR            1
#define STDNONE           2
#define UNCVID            0x1234
#define UNCDID            0x5678

//==============================
// RDBMS data files... 
//------------------------------
#define MOUSE_DATA        "/var/cache/sax/sysp/rdbms/scanmouse"
#define KEYBOARD_DATA     "/var/cache/sax/sysp/rdbms/scankeyboard"
#define SERVER_DATA       "/var/cache/sax/sysp/rdbms/scanserver"
#define SERVER_STUFF_DATA "/var/cache/sax/sysp/rdbms/scanserverstuff"
#define STUFF_DATA        "/var/cache/sax/sysp/rdbms/scanstuff"
#define D3_DATA           "/var/cache/sax/sysp/rdbms/scan3d"

//==============================
// Scripts...
//------------------------------
#define PDIR              "/usr/share/sax/profile/"
#define MACHINE           "/usr/share/sax/sysp/script/machine.pl"
#define INSTALLED         "/usr/share/sax/sysp/script/installed.pl"
#define KILLDOT           "/usr/share/sax/sysp/script/killdot.pl"
#define GETMEMORY         "/usr/share/sax/sysp/script/memory.pl"
#define PREPARELOG        "/usr/share/sax/sysp/script/preparelog.pl"
#define CHECKMAP          "/usr/share/sax/sysp/script/checkmap.pl"
#define PROFILECOUNT      "/usr/share/sax/sysp/script/profilecount.pl"
#define PSECTION          "/usr/share/sax/sysp/script/psection.pl"
#define PDRIVER           "/usr/share/sax/sysp/script/profiledriver.pl"
#define XCMD              "/usr/sbin/xcmd"
#define GETVENDOR         "/usr/share/sax/sysp/script/vendor.pl"

#ifdef XORG_LOADER
#define  LOADER_NAME      "Xorg"
#define  MODULE_VENDOR    "X.Org Foundation"
#else
#define  LOADER_NAME      "XFree86"
#define  MODULE_VENDOR    "The XFree86 Project"
#endif

#endif

