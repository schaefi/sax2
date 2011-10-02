/**************
FILE          : parse.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : parse the server message from the 
              : startup leve using lex and yacc 
              :
			  :
STATUS        : development
**************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../syslib.h"

//==============================
// External variables
//------------------------------
extern int    zeilennr,tokenwert;
extern char*  yytext;
extern char   pdev[30];
extern int    detectindex;
extern int    clocksindex;
extern LOGSTRUCT detect[];
extern CLKSTRUCT clocks[];

//==============================
// Protottypes 
//------------------------------
void init(MsgDetect *msg,int anz);
int  strsplit(str string,char deli,str left,str right);
void strtolower(str string);
void strtoupper(str string);
void initp(void);
void parsemsg(MsgDetect *msg);
void CheckRange(MsgDetect *msg,int n,int k);
int  getcard(void);

extern int yyparse (void);
extern int yylex   (void);

//==============================
// Function initp...
//------------------------------
void initp(void) {
	yyparse();
}

//==============================
// Function getcard...
//------------------------------
int getcard(void) {
	int count,i;
	str id,oldid;

	count = 0;
	for(i=0;i<detectindex;i++) {
		strcpy(id,detect[i].id);
		if (strcmp(id,oldid) != 0) {
			count++;
		}
		strcpy(oldid,id);
	}
	if (count != 0) {
		return (count);
	} else {
		return (1);
	}
}

//==============================
// Function parsemsg...
//------------------------------
void parsemsg(MsgDetect *msg) {
	str key,value,id,oldid;
	int i,count,n,l,k,vsync;
	str left,right;
	str x,y;
	int dac;

	// store the ID strings...
	// ------------------------
	count = -1;
	for(i=0;i<detectindex;i++) {
		strcpy(id,detect[i].id);
		if (strcmp(id,oldid) != 0) {
			count++;
			strcpy(msg[count].id,id);
		}
		strcpy(oldid,id);
	}

	// printf("### %d\n",count);
	for (n=0;n<=count;n++) {

		// first run...
		// --------------
		/* get clocks... */
		for(i=0;i<clocksindex;i++) {
			strcpy(id,clocks[i].id);
			if (strcmp(id,msg[n].id) != 0) {
				continue;
			}
			msg[n].clkcount++;
		}
		msg[n].clocks = (float*)malloc(sizeof(float)*msg[n].clkcount);

		/* get other stuff... */
		for(i=0;i<detectindex;i++) {
			strcpy(key,detect[i].key);
			strtoupper(key); 
			strcpy(id,detect[i].id);
			if (strcmp(id,msg[n].id) != 0) {
				continue;
			}
			strcpy(value,detect[i].value);
			trim(value);

			if (strcmp(pdev,"") != 0) {
				strcpy(msg[n].primary,pdev);
			}
			if (strcmp(key,"DDC") == 0) {
			if (strcmp(value,"") != 0) {
				strcpy(msg[n].ddc,value);
			}
			}
			if (strcmp(key,"DDS") == 0) {
			if (strcmp(value,"") != 0) {
				strsplit(value,':',left,right);
				trim(right);
				value[0] = right[0];
				value[1] = right[1];
				value[2] = right[2];
				value[3] = '\0'; 
				strcpy(msg[n].dds,value);
			}
			}
			if (strcmp(key,"DPX") == 0) {
				msg[n].dpix = atoi(value) * 10;
				if ((msg[n].dpix < 0) || (msg[n].dpix > 1000)) {
					msg[n].dpix = 0;
					msg[n].dpiy = 0;
				}
			}
			if (strcmp(key,"DPY") == 0) {
				msg[n].dpiy = atoi(value) * 10;
				if ((msg[n].dpiy < 0) || (msg[n].dpiy > 1000)) {
					msg[n].dpix = 0;
					msg[n].dpiy = 0;
				}
			}
			if (strcmp(key,"CHIP") == 0) {
			if (strcmp(value,"") != 0) {
				strcpy(msg[n].chipset,value);
			}
			}
			if (strcmp(key,"VMW")  == 0) {
				msg[n].vmdepth = atoi(value);
			}
			if (strcmp(key,"MEM")  == 0) {
				msg[n].memory = atol(value);
			}
			if (strcmp(key,"DAC")  == 0) {
				dac = atoi(value);
				if (dac > 1000) {
					dac = dac / 1000;
					msg[n].dacspeed = dac;
				} else {
					if (dac != 0)
					msg[n].dacspeed = dac;
				}
			}
			if (strcmp(key,"VESA")  == 0) {
				msg[n].vesacount++;
			}
			if (strcmp(key,"REFRESH")  == 0) {
				msg[n].clkcount++;
			}
			if (strcmp(key,"PAN") == 0) {
				msg[n].vesacount++;
				strcpy(msg[n].displaytype,"LCD/TFT");
			} else {
				if (strcmp(msg[n].displaytype,"LCD/TFT") != 0) {
					strcpy(msg[n].displaytype,"CRT");
				}
			}
			/*printf("%s %s %s\n",id,key,value);*/
		}

		// second run...
		// ---------------
		/* get clocks... */
		k=0;
		for(i=0;i<clocksindex;i++) {
			strcpy(id,clocks[i].id);
			if (strcmp(id,msg[n].id) != 0) {
				k=0;
				continue;
			}
			strcpy(value,clocks[i].clk);
			msg[n].clocks[k] = atof(value);
			/*printf("%d %d %f\n",k, n,atof(value));*/
			k++;
		}

		/* get rest stuff... */
		k=0;
		for(i=0;i<detectindex;i++) {
			strcpy(key,detect[i].key);
			strtoupper(key); 
			strcpy(id,detect[i].id);
			if (strcmp(id,msg[n].id) != 0) {
				continue;
			}
			strcpy(value,detect[i].value);
			trim(value); 

			if (strcmp(key,"PAN") == 0) {
				strsplit(value,'x',left,right);
				msg[n].vmodes[k].x = atoi(left);
				msg[n].vmodes[k].y = atoi(right);
				msg[n].vmodes[k].vsync = 70;

				vsync = msg[n].vmodes[k].vsync;
				switch(msg[n].vmodes[k].x) {
				/*640x480...*/
				case 640 :
					msg[n].vmodes[k].hsync = 35;
					CheckRange(msg,n,k);
				break;

				/*800x600...*/
				case 800 :
					msg[n].vmodes[k].hsync = 45;
					CheckRange(msg,n,k);
				break;

				/*1024x768...*/
				case 1024:
					msg[n].vmodes[k].hsync = 62;
					CheckRange(msg,n,k);
				break;
    
				/*1152x864...*/
				case 1152:
					msg[n].vmodes[k].hsync = 72;
					CheckRange(msg,n,k);
				break;
 
				/*1280x1024...*/
				case 1280:
					msg[n].vmodes[k].hsync = 80;
					CheckRange(msg,n,k);
				break; 

				/*1600x1200...*/
				case 1600:
					msg[n].vmodes[k].hsync = 90;
					CheckRange(msg,n,k);
				break;

				/* any other mode...*/
				default:
					msg[n].vmodes[k].hsync = msg[n].vmodes[k].vsync;
				break;
				}
				k++;
			}

			if (strcmp(key,"VESA")  == 0) {
				strsplit(value,'@',left,right);
				l=strlen(right);
				right[l-2]='\0';
				strsplit(left,'x',x,y);  

				msg[n].vmodes[k].x     = atoi(x);
				msg[n].vmodes[k].y     = atoi(y);
				msg[n].vmodes[k].vsync = atoi(right);
   
				vsync = msg[n].vmodes[k].vsync;
				switch(msg[n].vmodes[k].x) {
				/*640x480...*/
				case 640 :
					if ((vsync > 50) && (vsync <= 60)) {
						msg[n].vmodes[k].hsync = 32;
					}
					if ((vsync > 60) && (vsync <= 90)) {
						msg[n].vmodes[k].hsync = 35;
					}
					CheckRange(msg,n,k);
				break;

				/*800x600...*/
				case 800 :
					if ((vsync > 50) && (vsync <= 60)) {
						msg[n].vmodes[k].hsync = 35;
					}
					if ((vsync > 60) && (vsync <= 90)) {
						msg[n].vmodes[k].hsync = 45;
					}
					CheckRange(msg,n,k);
				break;

				/*1024x768...*/
				case 1024:
					if ((vsync > 50) && (vsync <= 60)) {
						msg[n].vmodes[k].hsync = 45;
					}
					if ((vsync > 60) && (vsync <= 90)) {
						msg[n].vmodes[k].hsync = 62;
					}
					CheckRange(msg,n,k);
				break; 

				/*1152x864...*/
				case 1152:
					if ((vsync > 50) && (vsync <= 70)) {
						msg[n].vmodes[k].hsync = 62;
					}
					if ((vsync > 70) && (vsync <= 90)) {
						msg[n].vmodes[k].hsync = 72;
					}
					CheckRange(msg,n,k);
				break;

				/*1280x1024...*/
				case 1280:
					if ((vsync > 50) && (vsync <= 80)) {
						msg[n].vmodes[k].hsync = 70;
					}
					if ((vsync > 80) && (vsync <= 100)) {
						msg[n].vmodes[k].hsync = 80;
					}
					CheckRange(msg,n,k);
				break;

				/*1600x1200...*/
				case 1600:
					if ((vsync > 50) && (vsync <= 80)) {
						msg[n].vmodes[k].hsync = 85;
					}
					if ((vsync > 80) && (vsync <= 100)) {
						msg[n].vmodes[k].hsync = 95;
					}
					CheckRange(msg,n,k);
				break;

				/* any other mode...*/
				default:
					//msg[n].vmodes[k].hsync = msg[n].vmodes[k].vsync; 

					// delete this ugly mode...
					// -------------------------
					msg[n].vmodes[k].x = -1;
					msg[n].vmodes[k].y = -1;
				break;
				}
				k++;
				/*printf("%s %s %s\n",x,y,right);*/
			}

			if (strcmp(key,"REFRESH")  == 0) {
				/*----[ dirty hack should be improved ]---*/
				msg[n].vsync_max = atoi(value);
				msg[n].hsync_max = msg[n].vsync_max + (msg[n].vsync_max / 3);
			}
		}
	}
}

//==============================
// Function CheckRange...
//------------------------------
void CheckRange(MsgDetect *msg,int n,int k) {
	if (msg[n].hsync_max < msg[n].vmodes[k].hsync) {
		msg[n].hsync_max = msg[n].vmodes[k].hsync;
	}
	if (msg[n].vsync_max < msg[n].vmodes[k].vsync) {
		msg[n].vsync_max = msg[n].vmodes[k].vsync;
	}
}

//==============================
// Function init...
//------------------------------
void init(MsgDetect *msg,int anz) { 
	int i;
	for (i=0;i<anz;i++) {
		msg[i].cards     = 0;
		msg[i].memory    = 1024;
		msg[i].dacspeed  = 120;
		msg[i].clkcount  = 0;
		msg[i].clocks    = NULL;
		msg[i].dpix      = 0;
		msg[i].dpiy      = 0;
		msg[i].vesacount = 1;
		msg[i].vsync_max = 72;
		msg[i].hsync_max = 33; 
		msg[i].vmodes->x = 640;
		msg[i].vmodes->y = 480;
		msg[i].vmdepth   = 0;
		msg[i].vmodes->hsync = 25;
		msg[i].vmodes->vsync = 60;
		strcpy(msg[i].chipset,"generic");
		strcpy(msg[i].id,"VGA(0)");
		strcpy(msg[i].primary,"");
		strcpy(msg[i].displaytype,"CRT");
		strcpy(msg[i].clkstr,"");
		strcpy(msg[i].ddc,"");
		strcpy(msg[i].dds,"");
	}
}

//==============================
// Function trim...
//------------------------------
void trim(char *message) {
	int n=strlen(message);
	char *erg = NULL;
	int i,a,count;

	for(i=n;i>=0;i--) {
	if (
		(message[i] != ' ')    &&
		(message[i] != '\t')   &&
		(message[i] != '\0')   &&
		(message[i] != '"')    &&
		(message[i] != '\n')
	) {
		message[i+1]='\0';
		break;
	}
	}
	n=strlen(message);
	count=0;
	for(i=0;i<n;i++) {
	if (
		(message[i] == ' ')    ||
		(message[i] == '\t')   ||
		(message[i] == '\0')   ||
		(message[i] == '"')    ||
		(message[i] == '\n')
	) {
		count++;
	} else {
		break;
	}
	}
	erg = (char*)malloc(sizeof(char)*(n+1));
	for (a=0;a<n;a++) {
		erg[a] = '\0';
	}
	for(i=count;i<n;i++) {
		erg[i-count] = message[i];
	}
	erg[i] = '\0';
	strcpy(message,erg);
}

//==============================
// Function strtolower...
//------------------------------
void strtolower(str string) {
	int l=strlen(string);
	int x=0;
	for (x=0;x<l;x++) {
	if ((char)string[x] >= 65 && (char)string[x] <= 90) {
		string[x]=string[x]+32;
	}
	}
}

//==============================
// Function strtoupper...
//------------------------------
void strtoupper(str string) {
	int l=strlen(string);
	int x=0;
	for (x=0;x<l;x++) {
	if ((char)string[x] >= 97 && (char)string[x] <= 122) {
		string[x]=string[x]-32;
	}
	}
}

//==============================
// Function strsplit...
//------------------------------
int strsplit(str string,char deli,str left,str right) {
	int l=strlen(string);
	int start=0;
	str part1="";
	str part2="";
	int found=0;
	int n;

	for (n=start;n<=l;n++) {
		if (string[n] != deli) {
			part1[n]=string[n];
		} else {
			start=n+1; part1[n]='\0'; found=1;
			break;
		}
	}
	for (n=start;n<=l;n++) {
		part2[n-start] = string[n];
	}

	part2[l+1] = '\0';
	strcpy(left,part1);
	strcpy(right,part2);
	if (found == 0) {
		return(-1);
	} else {
		return(0);
	}
}


