/**************
FILE          : mouse.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to obtain mouse
              : information using [libhd] 
              :
              :
STATUS        : development
**************/
#include "hwdata.h"
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <X11/extensions/xf86misc.h>

//===================================
// Prototypes...
//-----------------------------------
int  catchErrors  (Display*, XErrorEvent*);
void enableMouse  (Display*);
void disableMouse (Display*);

//===================================
// GetMouseData...
//-----------------------------------
MouseData* MouseGetData(void) {
	hd_data_t *hd_data = NULL;
	driver_info_t *di  = NULL;
	MouseData* data    = NULL;
	MouseData* first   = NULL;
	MouseData* last    = NULL;
	hd_t *hd           = NULL;
	hd_t *first_dev    = NULL;
	Display* dpy       = NULL;
	int haveDisplay    = 0;
	char buf[256]      = "";

	//===================================
	// Open X11 display
	//-----------------------------------
	int old_stderr = dup (2);
	freopen("/dev/null","w",stderr);
	if ((dpy = XOpenDisplay (getenv("DISPLAY")))) {
		fflush (stderr);
		dup2 (old_stderr,2);
		setlinebuf (stderr);
		haveDisplay = 1;
	}
	//===================================
	// Disable mouse
	//-----------------------------------
	if (haveDisplay) {
		disableMouse (dpy);
	}
	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	//hd_data->flags.fast = 1;
	hd = hd_list(hd_data, hw_mouse, 1, NULL);
	first_dev = hd;

	for(; hd; hd = hd->next) {
		if (! hd->unix_dev_name) {
			continue;
		}
		//===================================
		// create new element and defaults...
		// -----------------------------------
		data = (MouseData*)malloc(sizeof(MouseData));
		sprintf (buf,"0x%04x",ID_VALUE(hd->device.id));
		strcpy(data->did,buf);
		sprintf (buf,"0x%04x",ID_VALUE(hd->vendor.id));
		strcpy(data->vid,buf);
		strcpy(data->name,hd->model);
		strcpy(data->device,"/dev/mouse");
		strcpy(data->realdev,"<undefined>");
		strcpy(data->protocol,"Auto");
		data->buttons = -1;
		data->wheel   = 1;
		data->emulate = 1;
		data->next    = NULL;

		if (last != NULL) {
			last->next = data;
		}
		if (hd == first_dev) {
			first = data;
		}
		//===================================
		// save hddata to struct...
		// ----------------------------------
		di = hd->driver_info;
		if(di && di->any.type == di_mouse && di->mouse.xf86) {
			strcpy(data->protocol,di->mouse.xf86);
			if (di->mouse.buttons) {
				data->buttons = di->mouse.buttons;
			}
			if (di->mouse.wheels) {
				data->wheel   = di->mouse.wheels;
			}
			if (data->buttons) {
			if (
				(data->buttons >= 3) || 
				(strcmp(data->protocol,"imps/2") == 0) ||
				(strcmp(data->protocol,"explorerps/2") == 0)
			) {
				data->emulate = 0;
			}
			}
			if (data->wheel) {
			if (data->wheel >= 1) {
				data->buttons = (data->wheel * 2) + data->buttons;
			}
			}
		}
		if (hd->unix_dev_name) {
			strcpy(data->device,hd->unix_dev_name);
		}
		if (hd->unix_dev_names && hd->unix_dev_names->next) {
			str_list_t* str = hd->unix_dev_names;
			for (; str; str = str->next) {
			if (strstr (str->str,"event")) {
				strcpy(data->realdev,str->str);
			}
			}
		}
		last = data;
	}
	//===================================
	// Enable mouse
	//-----------------------------------
	if (haveDisplay) {
		enableMouse (dpy);
	}
	if (haveDisplay) {
		XCloseDisplay (dpy);
	}
	return(first);
}

//===================================
// enableMouse...
//-----------------------------------
void enableMouse (Display* dpy) {
	XSetErrorHandler (catchErrors);
	XF86MiscMouseSettings mseinfo;
	if (!XF86MiscGetMouseSettings(dpy, &mseinfo)) {
		return;
	}
	mseinfo.flags |= MF_REOPEN;
	XF86MiscSetMouseSettings(dpy, &mseinfo);
	XSync(dpy, False);
}

//===================================
// disableMouse...
//-----------------------------------
void disableMouse (Display* dpy) {
	XSetErrorHandler (catchErrors);
	XF86MiscMouseSettings mseinfo;
	if (!XF86MiscGetMouseSettings(dpy, &mseinfo)) {
		return;
	}
	mseinfo.flags |= MF_REOPEN;
	strcpy(mseinfo.device,"/dev/unused");
	XF86MiscSetMouseSettings(dpy, &mseinfo);
	XSync(dpy, False);
}

//===================================
// catchErrors (X11)...
//-----------------------------------
int catchErrors (Display *dpy, XErrorEvent *ev) {
	return 0;
}
