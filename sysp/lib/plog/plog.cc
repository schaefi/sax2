/**************
FILE          : plog.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : parse the startup messages from a  
              : probeonly started server
              :
              :
STATUS        : development
**************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "../syslib.h"

//==============================
// functions...
//------------------------------
char  *snip(char *string);
void  save(void);
int   geths(int y,int vs);
void  sort(int card,MsgDetect result[],str erg);
void  initp(void);
int   getcard(void);
void  init(MsgDetect* result,int cards);
void  parsemsg(MsgDetect* result);
char* FormatDDCString (char *id,char *is);


//==============================
// Function PLogGetData...
//------------------------------
MsgDetect* PLogGetData (str logfile) {
	int allow[6] = {640,800,1024,1152,1280,1600};
	FILE *data;
	MsgDetect *new_vesa;
	MsgDetect *result;
	int new_vesanr[MAXVESA];
	int cards,i,n,l;
	int size;
	int aspect;
	int ok;
	str clkstr  = "";
	str compare = "";
	int newid   = 0;
	int found   = IS_FALSE;
	int max     = 0;
	int nr      = 0;
	int mode_nr = 0;
	int curx;
	int cury;
	int curv;
	int nextx;
	int nexty;

	// parsing...
	// -------------
	data = freopen(logfile,"r",stdin);
	if (data == NULL) {
		return(NULL);
	}
	initp();
	cards  = getcard();
	size   = sizeof(MsgDetect) * cards;
	result = (MsgDetect*)malloc(size);
	init(result,cards);

	// get results...
	// -----------------
	parsemsg(result);

	// check for double ID`s...
	// -------------------------
	for (i=0;i<cards;i++) {
		strcpy(compare,result[i].id);
		found = IS_FALSE;
		for (n=i+1;n<cards;n++) {
			if (strcmp(result[n].id,compare) == 0) {
				found = IS_TRUE;
				break;
			}
		}
		if (found == IS_FALSE) {
			newid++;
		}
	}
	cards = newid;
	size  = sizeof(MsgDetect) * cards;
	new_vesa = (MsgDetect*)malloc(size); 

	// save primary device to all chains
	// ----------------------------------
	for (i=0;i<cards;i++) {
	if (strcmp(result[i].primary,"") != 0) {
		for (l=0;l<cards;l++) {
			strcpy(result[l].primary,result[i].primary);
		}
		break;
	}
	}

	for (i=0;i<cards;i++) {
		mode_nr = 0;
		nr = 0;
		result[i].cards = cards;
		for (n=0;n<result[i].vesacount-1;n++) {
			curx  = result[i].vmodes[n].x;
			cury  = result[i].vmodes[n].y;
			curv  = result[i].vmodes[n].vsync;
			nextx = result[i].vmodes[n+1].x;
			nexty = result[i].vmodes[n+1].y;
   
			if ((curx == nextx) && (cury == nexty)) {
			if (result[i].vmodes[n+1].vsync > max) {
				max = result[i].vmodes[n+1].vsync;
				nr = n + 1;
				continue;
			}
			} else {
				if (result[i].vmodes[nr].x != -1) {
				// check the resolution...
				// ------------------------
				ok = 0;
				for(aspect=0;aspect<6;aspect++) { 
				if (allow[aspect] == result[i].vmodes[nr].x) {
					ok = 1; break;
				}
				}   
				if (ok == 1) {
					new_vesa[i].vmodes[mode_nr].x     = result[i].vmodes[nr].x;
					new_vesa[i].vmodes[mode_nr].y     = result[i].vmodes[nr].y;
					new_vesa[i].vmodes[mode_nr].hsync =
						result[i].vmodes[nr].hsync;
					new_vesa[i].vmodes[mode_nr].vsync =
						result[i].vmodes[nr].vsync;
					new_vesanr[i] = mode_nr;
					mode_nr++;
				}
				}
				max = result[i].vmodes[n+1].vsync;
				nr  = n + 1;
			}
		}

		sort(i,result,clkstr);
		if (strlen(clkstr) != 0) {
			strcpy(result[i].clkstr,clkstr);
		}
		if (strcmp(result[i].ddc,"") != 0) {
			strcpy(result[i].ddc,FormatDDCString(result[i].ddc,result[i].dds));
		} else {
			strcpy(result[i].ddc,"<undefined>");
		}
	}

	for (i=0;i<cards;i++) {
		if (result[i].vesacount -1 == 0) {
			result[i].vesacount = -1;
			continue;
		}
		result[i].vesacount = new_vesanr[i] + 1;
		for (n=0;n<MAXVESA;n++) {
			if (n <= new_vesanr[i]) {
				result[i].vmodes[n].x      = new_vesa[i].vmodes[n].x;
				result[i].vmodes[n].y      = new_vesa[i].vmodes[n].y;
				result[i].vmodes[n].hsync  = new_vesa[i].vmodes[n].hsync;
				result[i].vmodes[n].vsync  = new_vesa[i].vmodes[n].vsync;
			}
		}
	}
	return(result);
}

//==============================
// Function FormatDDCString...
//------------------------------
char* FormatDDCString (char *id,char *is) {
	char *result = NULL;
	char *save   = NULL;
	int size;
	int i,n;

	// get size and prepare mem...
	// ----------------------------
	size   = strlen(id);
	result = (char*)malloc(sizeof(char)*7);
	save   = (char*)malloc(sizeof(char)*7);

	// init with NULL...
	// -------------------
	strcpy(result,"0000");

	i = 3;
	n = 0;
	while(1) {
		result[i] = id[size-n-1];
		i--; n++;
		if (n == size) {
			break;
		}
	}
	strcpy(save,is);
	strcat(save,result);
	return(save);
}

//==============================
// Function sort...
//------------------------------
void sort(int card,MsgDetect result[],str erg) {
	float clk[result[card].clkcount];
	float uniq[result[card].clkcount];
	float save;
	int n     = 0;
	int i     = 0;
	int anz   = 0;
	float old = -1;
	int ready = 0;
	str clock = "";
	str clkstr= "";
 
	// fill clk array... 
	for (n=0;n<result[card].clkcount;n++) {
	if ((int)result[card].clocks[n] > 0) {
		clk[anz] = result[card].clocks[n];
		anz++;
	}
	}
	if (anz == 0) {
		strcpy(erg,"");
		return;
	}
	if (anz > 1) {
		// sort...
		for (n=0;n<anz-1;n++) {
		for (i=n+1;i<anz;i++) {
			if (clk[n] > clk[i]) {
				save = clk[n]; clk[n] = clk[i]; clk[i] = save;
			}
		}
		}
		// uniq... 
		for (n=0;n<anz;n++) {
			if (old != clk[n]) {
				uniq[ready] = clk[n];
				ready++;
			}
			old = clk[n];
		}
		// save as string...
		for (n=0;n<ready;n++) {
			sprintf(clock,"%2.2f ",uniq[n]);
			strcat(clkstr,clock);
		}
	} else {
		sprintf(clkstr,"%2.2f",clk[0]);
	}
	strcpy(erg,clkstr);
}

//==============================
// Function snip...
//------------------------------
char *snip(char *string) {
	int i;
	while(
		((i = strlen(string)) > 0) &&
		(isspace(string[i - 1]) ||
		(string[i - 1] == '\n') ||
		(string[i - 1] == '\r'))) {
			string[i - 1] = '\0';
	}
	return string;
}

//==============================
// Function geths...
//------------------------------
int geths (int y,int vs) {
	int hs;

	hs = (int)((vs*y)/1000);
	return(hs);
}


