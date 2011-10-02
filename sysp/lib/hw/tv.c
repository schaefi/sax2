/**************
FILE          : tv.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : provide functions to check if there is
              : a TV card in your system, using [libhd] 
              :
              :
STATUS        : development
**************/
#include "hwdata.h"

//===================================
// TvSupport...
//-----------------------------------
int TvSupport (void) {
	hd_data_t *hd_data;
	hd_t *hd;
 
	hd_data = (hd_data_t*)calloc(1, sizeof *hd_data);
	hd = hd_list(hd_data, hw_tv, 1, NULL);
  
	for(; hd; hd = hd->next) {
		return(1);    
	}
	return (0);
} 

