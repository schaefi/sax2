/**************
FILE          : monitor.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to get monitor data
              : and resolution information using [libhd] 
              :
              :
STATUS        : development
**************/
#include "hwdata.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#define new __new
#include <linux/vt.h>
#include <linux/kd.h>
#include <stdarg.h>
#include <dirent.h>
#include <math.h>

//===================================
// Defines...
//-----------------------------------
#define ACPI_BATTERY  "/proc/acpi/battery"

//===================================
// Internal functions...
//-----------------------------------
void toUpper  (char*);
bool isNoteBook (void);
char *dumpHID (hd_id_t*, char*, int);

//===================================
// MonitorGetData...
//-----------------------------------
MsgDetect* MonitorGetData (void) {
	MsgDetect* display = NULL;
	MsgDetect* first   = NULL;
	MsgDetect* last    = NULL;
	hd_t *first_dev    = NULL;
	hd_data_t* hd_data = NULL;
	hd_res_t*  res     = NULL;
	hd_t *hd           = NULL;
	driver_info_t *di  = NULL;
	driver_info_t *di0 = NULL;
	unsigned bestX     = 0;
	unsigned bestY     = 0;
	unsigned portID    = 0;
	unsigned i         = 0;
	char buf[256]      = "";
	//int  vt_orig       = getvt();

	//chvt  (1);
	//sleep (1);
	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_monitor, 1, NULL);
	first_dev = hd;

	for(; hd; hd = hd->next) {
		//===================================
		// create new MsgDetect element ...
		//-----------------------------------
		display = (MsgDetect*)malloc (sizeof(MsgDetect));
		display->next = NULL;
		display->port = portID;
		if (last != NULL) {
			last->next = display;
		}
		if (hd == first_dev) {
			first = display;
		}
		//===================================
		// Save Monitor Model ...
		//-----------------------------------
		sprintf(display->model,hd->model);
		sprintf(display->vendor,vend_id2str(hd->vendor.id));
		if (strcmp (display->vendor,"0000") == 0) {
			sprintf(display->model ,hd->device.name);
			sprintf(display->vendor,hd->vendor.name);
		}
		//===================================
		// Remove bad letters from model
		//-----------------------------------
		if (strstr (display->model,"\"")) {
			char* nname = (char*)malloc(sizeof(char)*strlen(display->model)+1);
			char* token = strtok (display->model,"\"");
			memset (nname,'\0',strlen(display->model)+1);
			while (token) {
				if (strlen(nname)) {
					sprintf (nname,"%s %s",nname,token);
				} else {
					sprintf (nname,token);
				}
				token = strtok (NULL,"\"");
			}
			sprintf (display->model,nname);
		}
		//===================================
		// Save Monitor DisplayType ...
		//-----------------------------------
		sprintf(display->displaytype,"CRT");
		if (strstr (dumpHID (&hd->device, buf, sizeof buf),"LCD")) {
			sprintf(display->displaytype,"LCD/TFT");
		} else if (isNoteBook()) {
			sprintf(display->displaytype,"LCD/TFT");
		}
		//===================================
		// Save Monitor Hsync/Vsync best Res
		//-----------------------------------
		di0 = hd->driver_info;
		display->hsync_max = 0;
		display->vsync_max = 0;
		display->bandwidth = 0;
		strcpy (display->modeline,"<undefined>");
		for(di = di0, i = 0; di; di = di->next, i++) {
		if (di->any.type == di_display) {
			if (di->display.bandwidth) {
			if (di->display.bandwidth >= 70) {
				display->bandwidth = di->display.bandwidth;
			}
			}
			if (di->display.max_hsync) {
				display->hsync_max = (int)di->display.max_hsync;
			}
			if (di->display.max_vsync) {
				display->vsync_max = (int)di->display.max_vsync;
			}
			if (di->display.width) {
				bestX = di->display.width;
				bestY = di->display.height;
			}
			if ((di->display.hdisp) && (display->bandwidth)) {
				sprintf (display->modeline,
				"%4u %4u %4u %4u %4u %4u %4u %4u %c/%c",
					di->display.hdisp,
					di->display.hsyncstart,
					di->display.hsyncend,
					di->display.htotal,
					di->display.vdisp,
					di->display.vsyncstart,
					di->display.vsyncend,
					di->display.vtotal,
					di->display.hflag,
					di->display.vflag
				);
				// ...
				// try to check for displaytype again. If the modeline
				// timings produces a 60 - 65 Hz timing we assume this
				// is a LCD/TFT panel
				// ----
				double clock = display->bandwidth * 1e6;
				double vsync = clock / di->display.htotal / di->display.vtotal;
				if ((vsync >= 50) && (vsync <= 65)) {
					sprintf (display->displaytype,"LCD/TFT");
				}
			}
			break;
		}
		}
		//===================================
		// Save Monitor DDC ID ...
		//-----------------------------------
		snprintf(display->ddc, sizeof(display->ddc), "%s%04x",
			vend_id2str(hd->vendor.id),ID_VALUE(hd->device.id)
		);
		toUpper (display->ddc);
		//===================================
		// Save Monitor DisplaySize + Res 
		//-----------------------------------
		int count = 0;
		for(res = hd->res; res; res = res->next) {
		switch (res->any.type) {
			case res_size:
			display->dpix = 0;
			display->dpiy = 0;
			if (res->size.unit == size_unit_mm) {
				if ((res->size.val1) && (res->size.val2)) {
					display->dpix = (int)res->size.val1;
					display->dpiy = (int)res->size.val2;
				}
			}
			if (res->size.unit == size_unit_cm) {
				if ((res->size.val1) && (res->size.val2)) {
					display->dpix = (int)(res->size.val1 * 10);
					display->dpiy = (int)(res->size.val2 * 10);
				}
			}
			break;

			case res_monitor:
			if ( 
				((res->monitor.width  > bestX) && (bestX > 0)) &&
				((res->monitor.height > bestY) && (bestY > 0))
			) { 
				// ...
				// resolution(s) higher than best resolution
				// won`t be handled here
				// ---
				break;
			}
			display->vmodes[count].x = res->monitor.width;
			display->vmodes[count].y = res->monitor.height;
			display->vmodes[count].vsync = res->monitor.vfreq;
			display->vmodes[count].hsync = (int)(
				(res->monitor.height*res->monitor.vfreq)/1000
			);
			if (display->vmodes[count].hsync > display->hsync_max) {
				display->vmodes[count].hsync = display->hsync_max;
			}
			count++;
			display->vesacount = count;
			break;
		
			default:
			break;
		}
		}
		if ((bestX > 0) && (bestY > 0)) {
			display->vmodes[count].x = bestX;
			display->vmodes[count].y = bestY;
			display->vmodes[count].hsync = display->hsync_max;
			display->vmodes[count].vsync = display->vsync_max;
			count++;
			display->vesacount = count;
		}
		//===================================
		// Add next hop ...
		//-----------------------------------
		last = display;
		portID++;
	}
	//chvt (vt_orig);
	return (first);
}

//=====================================
// check for battery -> laptop
//-------------------------------------
bool isNoteBook ( void ) {
	struct dirent* entry = NULL;
	DIR* batteryDir = NULL;
	batteryDir = opendir (ACPI_BATTERY);
	if (! batteryDir) {
		return false;
	}
	int BATs = 0;
	while (1) {
		entry = readdir (batteryDir);
		if (! entry) {
			break;
		}
		if (
			(strcmp(entry->d_name,".") == 0) ||
			(strcmp(entry->d_name,"..") == 0)
		) {
			continue;
		}
		BATs++;
	}
	if (BATs == 0) {
		return false;
	}
	return true;
}

//=====================================
// Transform string to upper case
//-------------------------------------
void toUpper(char* string) {
	int l=strlen(string);
	int x=0;
	for (x=0;x<l;x++) {
	if ((char)string[x] >= 97 && (char)string[x] <= 122) {
		string[x]=string[x]-32;
	}
	}
}

//=====================================
// Dump HID device name
//-------------------------------------
char* dumpHID (hd_id_t *hid, char *buf, int buf_size) {
	char *s;
	int i;

	*buf = 0;
	i = strlen(buf);
	s = buf + i;
	if (hid->name) {
		snprintf(s, buf_size - 1, "%s", hid->name);
	}
	return buf;
}
