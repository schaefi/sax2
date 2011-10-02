/**************
FILE          : vbios.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to check for the VESA
              : BIOS version using [ libhd ]
              :
              :
STATUS        : development
**************/
#include "hwdata.h"

//===================================
// vesaBIOS...
//-----------------------------------
char* vesaBIOS (void) {
	bios_info_t *bt;
	hd_data_t *hd_data;
	hd_t *hd;
 
	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_vbe, 1, NULL);

	if (! hd) {
		return 0;
	}
	if (!hd->detail || hd->detail->type != hd_detail_bios) {
		return 0;
	}
	if (!(bt = hd->detail->bios.data)) {
		return 0;
	}
	if(bt->vbe_ver) {
		char* result = (char*)malloc (sizeof(char)*128);
		sprintf (result,"%u.%u",bt->vbe_ver >> 8, bt->vbe_ver & 0xff);
		return result;
	}
	return 0;
} 
