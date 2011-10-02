/**************
FILE          : framebuffer.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to get framebuffer mode
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
// FrameBufferGetData...
//-----------------------------------
FbBootData* FrameBufferGetData (void) {
	FbBootData* display = NULL;
	FbBootEntry* entry  = NULL;
	FbBootData* first   = NULL;
	FbBootEntry* efirst = NULL;
	FbBootData* last    = NULL;
	FbBootEntry* elast  = NULL;
	hd_t *first_dev     = NULL;
	hd_data_t* hd_data  = NULL;
	hd_res_t*  res      = NULL;
	hd_t *hd            = NULL;
	int fbcount         = 0;

	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_framebuffer, 1, NULL);
	first_dev = hd;

	for(; hd; hd = hd->next) {
		//===================================
		// create new FbBootData element ...
		//-----------------------------------
		display = (FbBootData*)malloc (sizeof(FbBootData));
		display->next = NULL;
		if (last != NULL) {
			last->next = display;
		}
		if (hd == first_dev) {
			first = display;
		}
		//===================================
		// Save Framebuffer resolution/color
		//-----------------------------------
		fbcount = 0;
		elast   = 0;
		efirst  = 0;
		for(res = hd->res; res; res = res->next) {
			if (res->any.type != res_framebuffer) {
				continue;
			}
			entry = (FbBootEntry*)malloc (sizeof(FbBootEntry));
			entry->next = NULL;
			if (elast) {
				elast->next = entry;
			}
			if (fbcount == 0) {
				efirst = entry;
			}
			entry->mode  = res->framebuffer.mode;
			entry->x     = res->framebuffer.width;
			entry->y     = res->framebuffer.height;
			entry->depth = res->framebuffer.colorbits;
			elast = entry;
			fbcount++;
		}
		//===================================
		// Save entry count
		//-----------------------------------
		display -> entry = efirst;

		//===================================
		// Save entry count
		//-----------------------------------
		display -> fbcount = fbcount;

		//===================================
		// Add next hop ...
		//-----------------------------------
		last = display;
	}
	return (first);
}
