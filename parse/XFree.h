#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef HAVE_XORG_SERVER_HEADER
#undef HAS_SHM
#include "xorg-server.h"
#include "xorgVersion.h"
#define PARSER_VERSION XORG_VERSION_MINOR
#else
#define PARSER_VERSION 3
#endif

#include "xf86Parser.h"

extern XF86ConfigPtr ReadConfigFile (char *filename);

/* Parser Version... */
extern int GetParserVersion (void) {
	return PARSER_VERSION;
}

/* Files Section... */
extern char* GetModulePath (XF86ConfigPtr conf);
extern char* GetFontPath (XF86ConfigPtr conf);
#if PARSER_VERSION <= 4
extern char* GetRgbPath (XF86ConfigPtr conf);
#endif
extern char* GetLogFile (XF86ConfigPtr conf);

/* Module Section... */
extern char* GetModuleSpecs (XF86ConfigPtr conf);
extern char* GetModuleDisableSpecs (XF86ConfigPtr conf);
extern char* GetModuleSubSpecs (XF86ConfigPtr conf);

/* InputDevice Section... */
extern char* GetInputSpecs (XF86ConfigPtr conf);

/* ServerFlags Section... */
extern char* GetFlags (XF86ConfigPtr conf);

/* Extensions Section... */
extern char* GetExtensions (XF86ConfigPtr conf);

/* ServerLayout Section... */
extern char* GetLayoutSpecs (XF86ConfigPtr conf);

/* Device Section... */
extern char* GetDeviceSpecs (XF86ConfigPtr conf);

/* Monitor Section... */
extern char* GetMonitorSpecs (XF86ConfigPtr conf);
extern char* GetModesSpecs   (XF86ConfigPtr conf);

/* Screen Section... */
extern char* GetDisplaySpecs (XF86ConfigPtr conf);


