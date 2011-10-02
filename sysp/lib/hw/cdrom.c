/**************
FILE          : cdrom.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide function to check for the read-only
              : devices CD / DVD using [ libhd ]
              :
              :
STATUS        : development
**************/
#include "hwdata.h"

//===================================
// mediaDevices...
//-----------------------------------
char* mediaDevices (void) {
	char* result = 0;
	int count = 0;
	hd_data_t *hd_data;
	hd_t *hd,*hd_orig;
 
	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_cdrom, 1, NULL);
	hd_orig = hd;

	if (! hd) {
		return 0;
	}
	for(; hd; hd = hd->next) {
		if (hd->unix_dev_name) {
			if (! count) {
				count++;
			}
			count += strlen(hd->unix_dev_name);
		}
	}
	hd = hd_orig;
	result = (char*)malloc (sizeof(char)*count + 1);
	memset ((char*)result,'\0',count);
	for (; hd; hd = hd->next) {
		if (hd->unix_dev_name) {
			if (strcmp(result,"") != 0) {
				strcat (result,",");
			}
			strcat (result, hd->unix_dev_name);
		}
	}
	return result;
} 
