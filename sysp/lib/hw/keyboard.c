/**************
FILE          : keyboard.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to obtain keyboard
              : information using [libhd] 
              :
              :
STATUS        : development
**************/
#include "hwdata.h"

//===================================
// GetKeyboardData...
//-----------------------------------
KbdData* KeyboardGetData(void) {
	hd_data_t *hd_data;
	driver_info_t *di;
	KbdData* data   = NULL;
	KbdData* first  = NULL;
	KbdData* last   = NULL;
	hd_t *hd        = NULL;
	hd_t *first_dev = NULL;
	char buf[256]   = "";

	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_keyboard, 1, NULL);
	first_dev = hd;

	for(; hd; hd = hd->next) {
		// create new element and defaults...
		// -----------------------------------
		data = (KbdData*)malloc(sizeof(KbdData));
		sprintf (buf,"0x%04x",ID_VALUE(hd->device.id));
		strcpy(data->did,buf);
		sprintf (buf,"0x%04x",ID_VALUE(hd->vendor.id));
		strcpy(data->vid,buf);
		strcpy(data->rules,"undef");
		strcpy(data->model,"undef");
		strcpy(data->layout,"undef");
		strcpy(data->name,"undef");
		strcpy(data->device,"undef");
		data->next = NULL;

		if (last != NULL) {
			last->next = data;
		}
		if (hd == first_dev) {
			first = data;
		}

		// save hddata to new struct...
		// -----------------------------
		di = hd->driver_info;
		if(di && di->any.type == di_kbd) {
		if (di->kbd.XkbRules) {
			strcpy(data->rules,di->kbd.XkbRules);
		}
		if (di->kbd.XkbModel) {
			strcpy(data->model,di->kbd.XkbModel);
		}
		if (di->kbd.XkbLayout) {
			strcpy(data->layout,di->kbd.XkbLayout);
		}
		if (hd->unix_dev_name) {
			strcpy(data->device,hd->unix_dev_name);
		}
		if (hd->model) {
			strcpy(data->name,hd->model);
		}
		}
		last = data;
	}
	free(hd_data);
	return(first);
}
