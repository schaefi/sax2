/***************
FILE          : parse.c
***************
PROJECT       : SaX - SuSE advanced X-configuration
              :
BELONGS TO    : Parser for X11 R6 v4.x configuration file 
              :  
              :
BESCHREIBUNG  : parse.c provide functions to use the 
              : libxf86config within a perl modul called XFree 
              :
              :
              :
STATUS        : Status: Up-to-date
**************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#ifdef HAVE_XORG_SERVER_HEADER
#include "xorg-server.h"
#include "xorgVersion.h"
#else
#define XORG_VERSION_MAJOR 1
#define XORG_VERSION_MINOR 5
#endif

#include "xf86Parser.h"

#define XF86CONF_PHSYNC    0x0001
#define XF86CONF_NHSYNC    0x0002
#define XF86CONF_PVSYNC    0x0004
#define XF86CONF_NVSYNC    0x0008
#define XF86CONF_INTERLACE 0x0010
#define XF86CONF_DBLSCAN   0x0020
#define XF86CONF_CSYNC     0x0040
#define XF86CONF_PCSYNC    0x0080
#define XF86CONF_NCSYNC    0x0100
#define XF86CONF_HSKEW     0x0200 

#define SIZE               20000
#define BUF_SIZE           22

/******************
 Prototypes
******************/
XF86ConfigPtr ReadConfigFile (char *filename); 

/* Files Section... */
char* GetModulePath (XF86ConfigPtr conf);
char* GetFontPath (XF86ConfigPtr conf);
#if XORG_VERSION_MINOR <= 4
char* GetRgbPath (XF86ConfigPtr conf);
#endif
char* GetLogFile (XF86ConfigPtr conf);

/* Module Section... */
char* GetModuleSpecsBase (XF86ConfigPtr conf,int t);
char* GetModuleSpecs (XF86ConfigPtr conf);
char* GetModuleDisableSpecs (XF86ConfigPtr conf);
char* GetModuleSubSpecs (XF86ConfigPtr conf);

/* InputDevice Section... */
char* GetInputSpecs (XF86ConfigPtr conf);

/* ServerFlags Section... */
char* GetFlags (XF86ConfigPtr conf);

/* ServerLayout Section... */
char* GetLayoutSpecs (XF86ConfigPtr conf);

/* Device Section... */
char* GetDeviceSpecs (XF86ConfigPtr conf);

/* Monitor Section... */
char* GetMonitorSpecs (XF86ConfigPtr conf);

/* Modes Section... */
char* GetModesSpecs (XF86ConfigPtr conf);

/* Screen Section... */
char* GetDisplaySpecs (XF86ConfigPtr conf);

/* Extensions Section... */
char* GetExtensions (XF86ConfigPtr conf);

/*---------------------------------------------------------------------------
 Next part is the main function which build the logical layer of the module
---------------------------------------------------------------------------*/
int main (int argc, char *argv[]) {
	char *config = NULL;                    /* config file name */
	XF86ConfigPtr conf;

	conf = ReadConfigFile("/etc/X11/xorg.conf");
 
	printf("%p\n",conf->conf_flags);
	printf("%p\n",conf->conf_input_lst);
	printf("%p\n",conf->conf_layout_lst);
	printf("%p\n",conf->conf_device_lst);
	printf("%p\n",conf->conf_monitor_lst);
	printf("%p\n",conf->conf_screen_lst);  

	exit(0);
}



/*---------------------------------------------------------------------------
 Next part is the implementaion of all functions 
---------------------------------------------------------------------------*/
XF86ConfigPtr ReadConfigFile (char *filename) {
	const char *file = NULL; 
	XF86ConfigPtr conf;

	char CONFPATH[SIZE]=
		"%A,%R,/etc/%R,%P/etc/X11/%R,%E,%F,/etc/X11/%F," \
		"%P/etc/X11/%F,/etc/X11/%X,/etc/%X,%P/etc/X11/%X.%H," \
		"%P/etc/X11/%X,%P/lib/X11/%X.%H,%P/lib/X11/%X";

	//===================================
	// X11 version 4.1 or higher...
	//-----------------------------------
	if (!(file = (char*)xf86openConfigFile (CONFPATH, filename, NULL))) {
		fprintf (stderr, "Unable to open config file\n");
		return(NULL);
	}

	if ((conf = (XF86ConfigPtr)xf86readConfigFile ()) == NULL) {
		fprintf (stderr, "Problem when parsing config file\n");
		xf86closeConfigFile ();
		return(NULL);
	}
	xf86closeConfigFile ();
	return(conf);
}

/********************
 Files Section... 
********************/
char* GetModulePath (XF86ConfigPtr conf) {
	if (conf == NULL) return("null");
	return(conf->conf_files->file_modulepath);
}

char* GetFontPath (XF86ConfigPtr conf) {
	if (conf == NULL) return("null");
	return(conf->conf_files->file_fontpath);
}

#if XORG_VERSION_MINOR <= 4
char* GetRgbPath (XF86ConfigPtr conf) {
	if (conf == NULL) return("null");
	return(conf->conf_files->file_rgbpath);
}
#endif

char* GetLogFile (XF86ConfigPtr conf) {
	if (conf == NULL) return("null");
	return(conf->conf_files->file_logfile);
}


/**********************
 Module Section... 
**********************/
char* GetModuleSpecs (XF86ConfigPtr conf) {
	return GetModuleSpecsBase (conf,XF86_LOAD_MODULE);
}

char* GetModuleDisableSpecs (XF86ConfigPtr conf) {
	return GetModuleSpecsBase (conf,XF86_DISABLE_MODULE);
}

char* GetModuleSpecsBase (XF86ConfigPtr conf,int t) {
	int  count       = 0;
	char *result     = NULL; 
	char name[SIZE]  = "";
	int  type        = 0;
	char line[SIZE]  = "";

	XF86LoadPtr       lp;

	if (conf == NULL) return("null");
	if (t == XF86_DISABLE_MODULE) {
		lp = (XF86LoadPtr) conf->conf_modules->mod_disable_lst;
	} else {
		lp = (XF86LoadPtr) conf->conf_modules->mod_load_lst;
	}
	if (lp == NULL) {
		return (NULL);
	}

	result = (char*) malloc(SIZE);
	strcpy(result,"");

	for (lp;lp;lp=lp->list.next) {
		type = lp->load_type;
		if (type != t) {
			continue;
		}
		strcpy(name,lp->load_name);
		if (count == 0) {
			sprintf(line,"%s",name);
			strcat(result,line);
		} else {
			sprintf(line,",%s",name);
			strcat(result,line);
		}
		count++;
	}
	return(result); 
}

char* GetModuleSubSpecs (XF86ConfigPtr conf) {
	int  count       = 0;
	char name[SIZE]  = "";
	char line[SIZE]  = "";
	char optn[SIZE]  = ""; 
	char optv[SIZE]  = "";
	char *result     = NULL;
	XF86LoadPtr      lp;
	XF86OptionPtr    op;

	if (conf == NULL) return("null");
	lp = (XF86LoadPtr) conf->conf_modules->mod_load_lst;
	if (lp == NULL) {
		return(NULL);
	}
	result = (char*) malloc(SIZE);
	strcpy(result,"");

	for (lp;lp;lp=lp->list.next) {
		strcpy(name,lp->load_name);
		/* handle extmod subsection... */
		if (strcmp(name,"extmod") == 0) {
			strcat(result,"extmod:");
			count = 0; 

			op = (XF86OptionPtr) lp->load_opt;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);

				sprintf(line,"%s:%s",optn,optv);
				if (count == 0) {
					strcat(result,line);
				} else {
					strcat(result,",");
					strcat(result,line);
				}
				count++;
			}
		}
	}
	return(result);
}


/**********************
 InputDevice Sections
**********************/
char* GetInputSpecs (XF86ConfigPtr conf) {
	char line[SIZE]    = "";
	char id[SIZE]      = "";
	char driver[SIZE]  = "";
	char optn[SIZE]    = "";
	char optv[SIZE]    = "";
	char tmp[SIZE]     = "";
	char optline[SIZE] = "";
	int count          = 0;
	int next;
	char *result       = NULL;
	XF86ConfInputPtr ip;
	XF86OptionPtr    op;

	if (conf == NULL) return("null");
	ip = (XF86ConfInputPtr) conf->conf_input_lst;
	if (ip == NULL) {
		return(NULL);
	}

	result = (char*) malloc(SIZE);
	strcpy(result,"");

	next = 0;
	for (ip;ip;ip=ip->list.next) {
		if (next > 0) {
			strcat(result,"| ");
		}
		next++; 

		/* set identifier... */
		if (ip->inp_identifier != NULL) {
			strcpy(id,ip->inp_identifier);
			sprintf(line,"id=%s ",id);
			strcat(result,line);
		}

		/* set driver... */
		if (ip->inp_driver != NULL) {
			strcpy(driver,ip->inp_driver);
			sprintf(line,"driver=%s ",driver);
			strcat(result,line);
		}

		/* build option line... */
		op = (XF86OptionPtr) ip->inp_option_lst;
		if (op != NULL) {
			strcpy(optline,"option=");
			count = 0;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
				if (strcmp(optn,"XkbOptions") == 0) {
					sprintf(tmp,"§%s§",optv);
					strcpy(optv,tmp);
				}
				if (count == 0) {
					sprintf(line,"%s:%s",optn,optv); 
				} else {
					sprintf(line,",%s:%s",optn,optv);
				}
				count++;
				strcat(optline,line);
			}
  
			sprintf(line,"%s ",optline);
			strcat(result,line);
		}
	}
	return(result);
}

/***********************
 ServerFlags Section
***********************/
char* GetFlags (XF86ConfigPtr conf) {
	char line[SIZE]   = "";
	int count        = 0;
	char *result     = NULL;
	char optv[SIZE]  = "";
	char optn[SIZE]  = "";  
	XF86ConfFlagsPtr fp;
	XF86OptionPtr    op;
 
	if (conf == NULL) return("null");
	fp = (XF86ConfFlagsPtr) conf->conf_flags;
	if (fp == NULL) {
		return(NULL);
	}

	result = (char*) malloc(SIZE);
	strcpy(result,"");

	for (op=fp->flg_option_lst;op;op=op->list.next) {
		if (op->opt_name == NULL) {
			continue;
		}
		strcpy(optn,op->opt_name);
		if (op->opt_val != NULL) {
			strcpy(optv,op->opt_val);
		} else {
			strcpy(optv,"none");
		}
		if (count == 0) {
			sprintf(line,"%s:%s",optn,optv);
		} else {
			sprintf(line,",%s:%s",optn,optv);
		}
		strcat(result,line);
		count++;
	}
	return(result);
}

/********************
 Extensions Section... 
********************/
#ifdef HAVE_EXTENSIONS_SECTION
char* GetExtensions (XF86ConfigPtr conf) {
	// TODO...
	char line[SIZE]   = "";
	int count        = 0;
	char *result     = NULL;
	char optv[SIZE]  = "";
	char optn[SIZE]  = "";
	XF86ConfExtensionsPtr ep;
	XF86OptionPtr         op;

	if (conf == NULL) return("null");
	ep = (XF86ConfExtensionsPtr) conf->conf_extensions;
	if (ep == NULL) {
		return(NULL);
	}

	result = (char*) malloc(SIZE);
	strcpy(result,"");

	for (op=ep->ext_option_lst;op;op=op->list.next) {
		if (op->opt_name == NULL) {
			continue;
		}
		strcpy(optn,op->opt_name);
		if (op->opt_val != NULL) {
			strcpy(optv,op->opt_val);
		} else {
			strcpy(optv,"none");
		}
		if (count == 0) {
			sprintf(line,"%s:%s",optn,optv);
		} else {
			sprintf(line,",%s:%s",optn,optv);
		}
		strcat(result,line);
		count++;
	}
	return(result);
}
#else
// for compatibility only...
char* GetExtensions (XF86ConfigPtr conf) {
	char* result = (char*) malloc(SIZE);
	strcpy(result,"");
	return(result);
}
#endif

/************************
 ServerLayout Section... 
*************************/
char* GetLayoutSpecs (XF86ConfigPtr conf) {
	char line[8192]    = "";
	char buf[8192]     = "";
	char id[8192]      = "";
	char top[8192]     = "";
	char left[8192]    = "";
	char bottom[8192]  = "";
	char right[8192]   = "";
	char relative[8192]= "";
	char screen[8192]  = "";
	char input[8192]   = "";
	int  num           = 0;
	char optn[255]     = "";
	char sc[255][255];
	char optv[255]     = "";
	char optline[8192] = "";
	char scid[255]     = "";
	int count          = 0;
	int scount         = 0;
	int i;
	int loop;
	int next           = 0;
	char *result       = NULL; 
	XF86ConfAdjacencyPtr ap,hp;
	XF86ConfInputrefPtr  ip;
	XF86ConfLayoutPtr    lp;
	XF86OptionPtr        op;

	if (conf == NULL) return("null");
	lp = (XF86ConfLayoutPtr) conf->conf_layout_lst;
	if (lp == NULL) {
		return(NULL);
	}

	result = (char*) malloc(SIZE);
	strcpy(result,"");

	next = 0;
	for (lp;lp;lp=lp->list.next) {
		if (next > 0) {
			strcat(result,"| ");
		}
		next++;

		/* set identifier... */
		if (lp->lay_identifier != NULL) {
			sprintf(line,"id=%s ",lp->lay_identifier);
			strcat(result,line);
		}

		/* set screen... */
		ap = (XF86ConfAdjacencyPtr) lp->lay_adjacency_lst;
		hp = (XF86ConfAdjacencyPtr) lp->lay_adjacency_lst;
		if ((ap != NULL) && (ap->adj_where != -1)) {
			strcpy(screen,"screen=");
			num    = -1;
			scount = 0;
			for (ap;ap;ap=ap->list.next) {
				loop = 0;
				for (i=0;i<scount;i++) {
				if (strcmp(sc[i],ap->adj_screen_str) == 0) {
					loop = 1; break; 
				}
				}
				if (loop == 1) {
					continue;
				}
				strcpy(scid,ap->adj_screen_str);
				strcpy(sc[scount],scid);
				scount++;
				strcpy(top,     "<none>");
				strcpy(bottom,  "<none>");
				strcpy(left,    "<none>");
				strcpy(right,   "<none>");
				strcpy(relative,"<none>");
				for (hp;hp;hp=hp->list.next) {
					if (strcmp(scid,hp->adj_screen_str) == 0) {
						switch(hp->adj_where) {
						case 1: {
							strcpy(right,hp->adj_refscreen);
							break;
						} 
						case 2: {
							strcpy(left,hp->adj_refscreen);
							break;
						}
						case 3: {
							strcpy(top,hp->adj_refscreen);
							break;
						}
						case 4: {
							strcpy(bottom,hp->adj_refscreen);
							break;
						}
						case 5: {
							sprintf(relative,"%d-%d-%s",
							hp->adj_x,hp->adj_y,hp->adj_refscreen);
						}
						}
					}
				}
				hp = (XF86ConfAdjacencyPtr) lp->lay_adjacency_lst;
				if (scount == 1) {
					sprintf ( line,
						"%s,%d,%s,%s,%s,%s,%s",
						scid,num,top,bottom,left,right,relative
					);
				} else {
					sprintf ( line,
						":%s,%d,%s,%s,%s,%s,%s",
						scid,num,top,bottom,left,right,relative
					); 
				}
				strcat(screen,line);
			}
			sprintf(line,"%s ",screen);
			strcat(result,line);
		}

		/* set input device...  */
		ip = (XF86ConfInputrefPtr) lp->lay_input_lst;
		if (ip != NULL) {
			strcpy(input,"inputdev=");
			count = 0;
			for (ip;ip;ip=ip->list.next) {
				if (ip->iref_inputdev_str != NULL) {
					if (count == 0) {
						sprintf(buf,"%s",ip->iref_inputdev_str);
					} else {
						sprintf(buf,":%s",ip->iref_inputdev_str); 
					}
					strcat(input,buf);
				} 
				if (ip->iref_option_lst   != NULL) {
					op = (XF86OptionPtr) ip->iref_option_lst;
					for (op;op;op=op->list.next) {
						strcpy(optn,"<none>");
						strcpy(optv,"<none>");
						if (op->opt_name != NULL) strcpy(optn,op->opt_name);
						if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
						sprintf(buf,",%s",optn);
						strcat(input,buf);
					}
				}
				count++;
			}
			sprintf(line,"%s ",input);
			strcat(result,line);
		}


		/* build option line... */
		op = (XF86OptionPtr) lp->lay_option_lst;
		if (op != NULL) {
			strcpy(optline,"option=");
			count = 0;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
				if (count == 0) {
					sprintf(line,"%s:%s",optn,optv);
				} else {
					sprintf(line,",%s:%s",optn,optv);
				}
				count++;
				strcat(optline,line);
			}
			sprintf(line,"%s ",optline);
			strcat(result,line);
		}
	}
	return(result);
}


/*******************
 Device Section... 
*******************/
char* GetDeviceSpecs (XF86ConfigPtr conf) {
	char line[SIZE]    = "";
	int  dacspeed      = 0;
	int  i             = 0;
	char optn[SIZE]    = "";
	char optv[SIZE]    = "";
	char optline[SIZE] = "";
	char clk[SIZE]     = "";
	char clkline[SIZE] = "";
	int  count         = 0;
	int  next;
	char *result       = NULL;
	XF86ConfDevicePtr dp;
	XF86OptionPtr     op;

	if (conf == NULL) return("null");
	dp = (XF86ConfDevicePtr) conf->conf_device_lst;
	if (dp == NULL) {
		return(NULL);
	}
	result = (char*) malloc(SIZE);
	strcpy(result,"");

	next = 0;
	for (dp;dp;dp=dp->list.next) {
		if (next > 0) {
			strcat(result,"| ");
		}
		next++;

		/* set identifier... */
		if (dp->dev_identifier != NULL) {
			sprintf(line,"id=%s ",dp->dev_identifier);
			strcat(result,line);
		}

		/* set vendor... */
		if (dp->dev_vendor != NULL) {
			sprintf(line,"vendor=\"%s\" ",dp->dev_vendor);
			strcat(result,line);
		}

		/* set board... */
		if (dp->dev_board != NULL) {
			sprintf(line,"board=\"%s\" ",dp->dev_board);
			strcat(result,line);
		}

		/* set chipset... */
		if (dp->dev_chipset != NULL) {
			sprintf(line,"chipset=%s ",dp->dev_chipset);
			strcat(result,line);
		}

		/* set screen.... */
		if (dp->dev_screen >= 0) {
			sprintf(line,"screen=%d ",dp->dev_screen);
			strcat(result,line);
		}

		/* set card... */
		if (dp->dev_card != NULL) {
			sprintf(line,"card=%s ",dp->dev_card);
			strcat(result,line);
		}

		/* set driver... */
		if (dp->dev_driver != NULL) {
			sprintf(line,"driver=%s ",dp->dev_driver);
			strcat(result,line);
		}

		/* set ramdac chip... */
		if (dp->dev_ramdac != NULL) {
			sprintf(line,"ramdac=%s ",dp->dev_ramdac);
			strcat(result,line);
		}

		/* set dacspeeds 8,16,24,32 bit... */
		for (i=0;i<4;i++) {
			dacspeed = dp->dev_dacSpeeds[i];
			if (dacspeed > 0) {
				sprintf(line,"dac%d=%d ",(i+1)*8,dacspeed/1000);
				strcat(result,line);
			}
		}

		/* set videoram... */
		if (dp->dev_videoram > 0) {
			sprintf(line,"videoram=%d ",dp->dev_videoram);
			strcat(result,line);
		}

		/* set biosbase... */
		if (dp->dev_bios_base != 0) {
			sprintf(line,"biosbase=%d ",dp->dev_bios_base);
			strcat(result,line);
		}
  
		/* set membase... */
		if (dp->dev_mem_base != 0) {
			sprintf(line,"membase=%d ",dp->dev_mem_base);
			strcat(result,line);
		}

		/* set iobase... */
		if (dp->dev_io_base != 0) {
			sprintf(line,"iobase=%d ",dp->dev_io_base);
			strcat(result,line);
		}

		/* set clock chip... */
		if (dp->dev_clockchip != NULL) {
			sprintf(line,"clockchip=%s ",dp->dev_clockchip);
			strcat(result,line);
		}
  
		/* set chipid... */
		if (dp->dev_chipid > 0) {
			sprintf(line,"chipid=%d ",dp->dev_chipid);
			strcat(result,line);
		}

		/* set chip revision... */
		if (dp->dev_chiprev > 0) {
			sprintf(line,"chiprev=%d ",dp->dev_chiprev);
			strcat(result,line);
		}

		/* set clocks... */
		if (dp->dev_clocks != 0) {
			strcpy(clkline,"");
			for (i = 0; i < dp->dev_clocks; i++) {
				if (i==0) {
					sprintf(clk, "%d", dp->dev_clock[i]/1000);
				} else {
					sprintf(clk, ",%d", dp->dev_clock[i]/1000);
				}
				strcat(clkline,clk);
			}
			sprintf(line,"clocks=%s ",clkline);
			strcat(result,line);
		}

		/* set text clock freq... */
		if (dp->dev_textclockfreq != 0) {
			sprintf(line,"textclkfreq=%d ",dp->dev_textclockfreq);
			strcat(result,line);
		}

		/* set busid... */
		if (dp->dev_busid != NULL) {
			sprintf(line,"busid=%s ",dp->dev_busid);
			strcat(result,line);
		}

		/* set irq... */
		if (dp->dev_irq > 0) {
			sprintf(line,"irq=%d ",dp->dev_irq);
			strcat(result,line);
		}

		/* build option line... */
		op = (XF86OptionPtr) dp->dev_option_lst;
		if (op != NULL) {
			strcpy(optline,"option=");
			count = 0;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
				if (count == 0) {
					sprintf(line,"%s:%s",optn,optv);
				} else {
					sprintf(line,",%s:%s",optn,optv);
				}
				count++;
				strcat(optline,line);
			}
			sprintf(line,"%s ",optline);
			strcat(result,line);
		}
	}
	return(result);
}


/*********************
 Monitor Section... 
*********************/
char* GetMonitorSpecs (XF86ConfigPtr conf) {
	char line[8192]     = "";
	char id[SIZE]       = "";
	char vendor[SIZE]   = "";
	char optn[SIZE]     = "";
	char optv[SIZE]     = "";
	char optline[SIZE]  = "";
	char model[SIZE]    = "";
	char syncline[SIZE] = "";
	char modeline[8192] = "";
	char usemode[SIZE]  = "";
	char buf[SIZE]      = ""; 
	float width,height;
	float red,green,blue;
	int i;
	int count          = 0;
	int next           = 0;
	char *result       = NULL;

	XF86ConfMonitorPtr    mp;
	XF86ConfModeLinePtr   lp;
	XF86OptionPtr         op;
	XF86ConfModesLinkPtr  cp;

	if (conf == NULL) return("null");
	mp = (XF86ConfMonitorPtr) conf->conf_monitor_lst;
	if (mp == NULL) {
		return(NULL);
	}

	result = (char*) malloc(8192);
	strcpy(result,"");

	next = 0;
	for (mp;mp;mp=mp->list.next) {
		if (next > 0) {
			strcat(result,"| ");
		}
		next++;

		/* set identifier... */
		if (mp->mon_identifier != NULL) {
			strcpy(id,mp->mon_identifier);
			sprintf(line,"id=%s ",id);
			strcat(result,line);
		}

		/* set vendor... */
		if (mp->mon_vendor != NULL) {
			strcpy(vendor,mp->mon_vendor);
			sprintf(line,"vendor=%s ",vendor);
			strcat(result,line);
		}

		/* set model... */
		if (mp->mon_modelname != NULL) {
			strcpy(model,mp->mon_modelname);
			sprintf(line,"model=%s ",model);
			strcat(result,line);
		}

		/* set display size... */
		if (mp->mon_width != 0) {
			width=mp->mon_width;
			sprintf(line,"width=%4.2f ",width);
			strcat(result,line);
		}
		if (mp->mon_height != 0) {
			height=mp->mon_height;
			sprintf(line,"height=%4.2f ",height);
			strcat(result,line);
		}

		/* set gamma value... */
		if (mp->mon_gamma_red != 0) {
			red = mp->mon_gamma_red;
			sprintf(line,"red=%4.2f ",red);
			strcat(result,line);
		}
		if (mp->mon_gamma_green != 0) {
			green = mp->mon_gamma_green;
			sprintf(line,"green=%4.2f ",green);
			strcat(result,line);
		}
		if (mp->mon_gamma_blue != 0) {
			blue = mp->mon_gamma_blue;
			sprintf(line,"blue=%4.2f ",blue);
			strcat(result,line);
		}
 
		/* set the sync range... */
		syncline[0] = '\0';
		for (i = 0; i < mp->mon_n_hsync; i++) {
			sprintf ( syncline,
				"%.5g-%.5g",mp->mon_hsync[i].lo,mp->mon_hsync[i].hi
			);
		}
		sprintf(line,"Hs=%s ",syncline);
		strcat(result,line);
		syncline[0] = '\0';
		for (i = 0; i < mp->mon_n_vrefresh; i++) {
			sprintf ( syncline,
				"%.5g-%.5g",mp->mon_vrefresh[i].lo,mp->mon_vrefresh[i].hi
			);
		}
		sprintf(line,"Vs=%s ",syncline);
		strcat(result,line);

		/* set usemodes... */
		cp = (XF86ConfModesLinkPtr) mp->mon_modes_sect_lst;
		if (cp != NULL) {
			count = 0;
			for (cp;cp;cp=cp->list.next) {
				if (count == 0) {
					sprintf(line,"usemodes=%s ",cp->ml_modes_str); 
				} else {
					sprintf(line,"%s,%s ",line,cp->ml_modes_str);
				}
				count++;
			}
		}
		strcat(result,line);

		/* set modelines (only new format)... */
		lp = (XF86ConfModeLinePtr) mp->mon_modeline_lst;
		if (lp != NULL) {
			strcpy(modeline,"modeline=");
			count = 0;
			for (lp;lp;lp=lp->list.next) {
				if (count == 0) {
					sprintf ( line,"%s %4.2f %d %d %d %d %d %d %d %d",
						lp->ml_identifier, lp->ml_clock/1000.0,
						lp->ml_hdisplay,lp->ml_hsyncstart,
						lp->ml_hsyncend,lp->ml_htotal,
						lp->ml_vdisplay,lp->ml_vsyncstart,
						lp->ml_vsyncend,lp->ml_vtotal
					);
				} else {
					sprintf (line,",%s %4.2f %d %d %d %d %d %d %d %d",
						lp->ml_identifier, lp->ml_clock/1000.0,
						lp->ml_hdisplay,lp->ml_hsyncstart,
						lp->ml_hsyncend,lp->ml_htotal,
						lp->ml_vdisplay,lp->ml_vsyncstart,
						lp->ml_vsyncend,lp->ml_vtotal
					);
				}
				count++;

				if (lp->ml_flags & 0x0010) strcat(line," Interlace");
				if (lp->ml_flags & 0x0001) strcat(line," +HSync");
				if (lp->ml_flags & 0x0002) strcat(line," -HSync");
				if (lp->ml_flags & 0x0004) strcat(line," +VSync");
				if (lp->ml_flags & 0x0008) strcat(line," -VSync");
				if (lp->ml_flags & 0x0040) strcat(line," Composite");
				if (lp->ml_flags & 0x0080) strcat(line," +CSync");
				if (lp->ml_flags & 0x0100) strcat(line," -CSync");
				if (lp->ml_flags & 0x0020) strcat(line," DoubleScan");
				if (lp->ml_flags & 0x0200) {
					sprintf(buf," HSkew %d",lp->ml_hskew);
					strcat(line,buf);
				}
				if (lp->ml_vscan) {
					sprintf(buf," VScan %d",lp->ml_vscan);
					strcat(line,buf);
				}
				strcat(modeline,line);
			}
			sprintf(line,"%s ",modeline);
			strcat(result,line);
		}

		/* build option line... */
		op = (XF86OptionPtr) mp->mon_option_lst;
		if (op != NULL) {
			strcpy(optline,"option=");
			count = 0;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
				if (count == 0) {
					sprintf(line,"%s:%s",optn,optv);
				} else {
					sprintf(line,",%s:%s",optn,optv);
				}
				count++;
				strcat(optline,line);
			}
			sprintf(line,"%s ",optline);
			strcat(result,line);
		}
	}
	return(result);
}


/*******************
 Modes Section... 
*******************/
char* GetModesSpecs (XF86ConfigPtr conf) {
	char line[8192]     = ""; 
	char *result        = NULL;
	int  next           = 0;
	char modeline[8192] = "";
	char buf[8192]      = "";
	int  count          = 0;

	XF86ConfModesPtr     mp;
	XF86ConfModeLinePtr  lp;

	if (conf == NULL) return("null");
	mp = (XF86ConfModesPtr) conf->conf_modes_lst;
	if (mp == NULL) {
		return(NULL);
	}

	result = (char*) malloc(8192);
	strcpy(result,"");

	next = 0;
	for (mp;mp;mp=mp->list.next) {
		if (next > 0) {
			strcat(result," | ");
		}
		next++;

		/* set identifier... */
		if (mp->modes_identifier != NULL) {
			sprintf(line,"id=%s ",mp->modes_identifier);
			strcat(result,line);
		}

		/* set modelines (only new format)... */
		lp = (XF86ConfModeLinePtr) mp->mon_modeline_lst;
		if (lp != NULL) {
			strcpy(modeline,"modeline=");
			count = 0;
			for (lp;lp;lp=lp->list.next) {
				if (count == 0) {
					sprintf ( line,"%s %4.2f %d %d %d %d %d %d %d %d",
						lp->ml_identifier, lp->ml_clock/1000.0,
						lp->ml_hdisplay,lp->ml_hsyncstart,
						lp->ml_hsyncend,lp->ml_htotal,
						lp->ml_vdisplay,lp->ml_vsyncstart,
						lp->ml_vsyncend,lp->ml_vtotal
					);
				} else {
					sprintf ( line,",%s %4.2f %d %d %d %d %d %d %d %d",
						lp->ml_identifier, lp->ml_clock/1000.0,
						lp->ml_hdisplay,lp->ml_hsyncstart,
						lp->ml_hsyncend,lp->ml_htotal,
						lp->ml_vdisplay,lp->ml_vsyncstart,
						lp->ml_vsyncend,lp->ml_vtotal
					);
				}
				count++;

				if (lp->ml_flags & 0x0010) strcat(line," Interlace");
				if (lp->ml_flags & 0x0001) strcat(line," +HSync");
				if (lp->ml_flags & 0x0002) strcat(line," -HSync");
				if (lp->ml_flags & 0x0004) strcat(line," +VSync");
				if (lp->ml_flags & 0x0008) strcat(line," -VSync");
				if (lp->ml_flags & 0x0040) strcat(line," Composite");
				if (lp->ml_flags & 0x0080) strcat(line," +CSync");
				if (lp->ml_flags & 0x0100) strcat(line," -CSync");
				if (lp->ml_flags & 0x0020) strcat(line," DoubleScan");
				if (lp->ml_flags & 0x0200) {
					sprintf(buf," HSkew %d",lp->ml_hskew);
					strcat(line,buf);
				}
				if (lp->ml_vscan) {
					sprintf(buf," VScan %d",lp->ml_vscan);
					strcat(line,buf);
				}
				strcat(modeline,line);
			}
			sprintf(line,"%s ",modeline);
			strcat(result,line);
		}
	}
	return(result);
}


/*******************
 Screen Section... 
*******************/
char* GetDisplaySpecs (XF86ConfigPtr conf) {
	char line[SIZE]     = "";
	char driver[SIZE]   = "";
	char optn[SIZE]     = "";
	char optv[SIZE]     = "";
	char optline[SIZE]  = "";
	char dpopt[SIZE]    = "";
	int  count          = 0;
	int  next           = 0; 
	int  anz            = 0;
	char *result        = NULL;

	XF86ConfScreenPtr   sp;
	XF86OptionPtr       op;
	XF86ConfDisplayPtr  dp;
	XF86ModePtr         mp;

	if (conf == NULL) return("null");
	sp = (XF86ConfScreenPtr) conf->conf_screen_lst;
	if (sp == NULL) {
		return(NULL);
	}
	result = (char*) malloc(SIZE);
	strcpy(result,"");

	next = 0;
	for (sp;sp;sp=sp->list.next) {
		if (next > 0) {
			strcat(result," | ");
		}
		next++;

		/* set identifier... */
		if (sp->scrn_identifier != NULL) {
			sprintf(line,"id=%s ",sp->scrn_identifier);
			strcat(result,line);
		}

		/* set obsdriver... */
		if (sp->scrn_obso_driver != NULL) {
			sprintf(line,"obsd=%s ",sp->scrn_obso_driver);
			strcat(result,line);
		}

		/* set default color depth... */
		if ((int)sp->scrn_defaultdepth > 0) {
			sprintf(line,"defaultdepth=%d ",(int)sp->scrn_defaultdepth);
			strcat(result,line);
		}

		/* set default color depth bpp... */
		if ((int)sp->scrn_defaultbpp > 0) {
			sprintf(line,"defaultbpp=%d ",(int)sp->scrn_defaultbpp);
			strcat(result,line);
		}

		/* set default fb color depth bpp... */
		if ((int)sp->scrn_defaultfbbpp > 0) {
			sprintf(line,"defaultfbbpp=%d ",(int)sp->scrn_defaultfbbpp);
			strcat(result,line);
		}

		/* set device... */
		if (sp->scrn_device_str != NULL) {
			sprintf(line,"device=%s ",sp->scrn_device_str);
			strcat(result,line);
		}

		/* set monitor... */
		if (sp->scrn_monitor_str != NULL) {
			sprintf(line,"monitor=%s ",sp->scrn_monitor_str);
			strcat(result,line);
		}

		/* build option line... */
		op = (XF86OptionPtr) sp->scrn_option_lst;
		if (op != NULL) {
			strcpy(optline,"option=");
			count = 0;
			for (op;op;op=op->list.next) {
				strcpy(optn,"<none>");
				strcpy(optv,"<none>");
				if (op->opt_name != NULL) strcpy(optn,op->opt_name);
				if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
				if (count == 0) {
					sprintf(line,"%s:%s",optn,optv);
				} else {
					sprintf(line,",%s:%s",optn,optv);
				}
				count++;
				strcat(optline,line);
			}
			sprintf(line,"%s ",optline);
			strcat(result,line);
		}


		/* get display subsection... */
		dp = (XF86ConfDisplayPtr) sp->scrn_display_lst;
		if (dp != NULL) {
			for (dp;dp;dp=dp->list.next) {   
				count = 0;
				strcpy(line," display=");

				/* get viewport... */
				if (dp->disp_frameX0 >= 0) {
					if (count == 0) {
						sprintf(dpopt,"vpx:%d",dp->disp_frameX0);
					} else {
						sprintf(dpopt,",vpx:%d",dp->disp_frameX0);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_frameY0 >= 0) {
					if (count == 0) {
						sprintf(dpopt,"vpy:%d",dp->disp_frameY0);
					} else {
						sprintf(dpopt,",vpy:%d",dp->disp_frameY0);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get virtual... */
				if (dp->disp_virtualX > 0) {
					if (count == 0) {
						sprintf(dpopt,"vx:%d",dp->disp_virtualX);
					} else {
						sprintf(dpopt,",vx:%d",dp->disp_virtualX);
					}
					count++; 
					strcat(line,dpopt);
				}
				if (dp->disp_virtualY > 0) {
					if (count == 0) {
						sprintf(dpopt,"vy:%d",dp->disp_virtualY);
					} else {
						sprintf(dpopt,",vy:%d",dp->disp_virtualY);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get depth value... */
				if (dp->disp_depth > 0) {
					if (count == 0) {
						sprintf(dpopt,"depth:%d",dp->disp_depth);
					} else {
						sprintf(dpopt,",depth:%d",dp->disp_depth);
					}
					count++;
					strcat(line,dpopt);
				}  
 
				/* get bpp... */
				if (dp->disp_bpp > 0) {
					if (count == 0) {
						sprintf(dpopt,"bpp:%d",dp->disp_bpp);
					} else {
						sprintf(dpopt,",bpp:%d",dp->disp_bpp);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get visual... */
				if (dp->disp_visual != NULL) {  
					if (count == 0) {
						sprintf(dpopt,"visual:%s",dp->disp_visual);
					} else {
						sprintf(dpopt,",visual:%s",dp->disp_visual);
					}
					count++;
					strcat(line,dpopt);
				}
 
				/* get weight... */
				if (dp->disp_weight.red > 0) {
					if (count == 0) {
						sprintf(dpopt,"w.red:%d",dp->disp_weight.red);
					} else {
						sprintf(dpopt,",w.red:%d",dp->disp_weight.red);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_weight.green > 0) {
					if (count == 0) {
						sprintf(dpopt,"w.green:%d",dp->disp_weight.green);
					} else {
						sprintf(dpopt,",w.green:%d",dp->disp_weight.green);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_weight.blue > 0) {
					if (count == 0) {
						sprintf(dpopt,"w.blue:%d",dp->disp_weight.blue);
					} else {
						sprintf(dpopt,",w.blue:%d",dp->disp_weight.blue);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get black tok... */
				if (dp->disp_black.red > 0) {
					if (count == 0) {
						sprintf(dpopt,"bt.red:%d",dp->disp_black.red);
					} else {
						sprintf(dpopt,",bt.red:%d",dp->disp_black.red);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_black.green > 0) {
					if (count == 0) {
						sprintf(dpopt,"bt.green:%d",dp->disp_black.green);
					} else {
						sprintf(dpopt,",bt.green:%d",dp->disp_black.green);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_black.blue > 0) {
					if (count == 0) {
						sprintf(dpopt,"bt.blue:%d",dp->disp_black.blue);
					} else {
						sprintf(dpopt,",bt.blue:%d",dp->disp_black.blue);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get white tok... */
				if (dp->disp_white.red > 0) {
					if (count == 0) {
						sprintf(dpopt,"wt.red:%d",dp->disp_white.red);
					} else {
						sprintf(dpopt,",wt.red:%d",dp->disp_white.red);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_white.green > 0) {
					if (count == 0) {
						sprintf(dpopt,"wt.green:%d",dp->disp_white.green);
					} else {
						sprintf(dpopt,",wt.green:%d",dp->disp_white.green);
					}
					count++;
					strcat(line,dpopt);
				}
				if (dp->disp_white.blue > 0) {
					if (count == 0) {
						sprintf(dpopt,"wt.blue:%d",dp->disp_white.blue);
					} else {
						sprintf(dpopt,",wt.blue:%d",dp->disp_white.blue);
					}
					count++;
					strcat(line,dpopt);
				}

				/* get modes... */
				mp = (XF86ModePtr) dp->disp_mode_lst;
				if (mp != NULL) {
				strcpy(optline,"modes:");
				anz = 0; 
				for (mp;mp;mp=mp->list.next) {
					if (anz == 0) {
						sprintf(dpopt,"%s",mp->mode_name);
					} else {
						sprintf(dpopt,"@%s",mp->mode_name);
					}
					anz++;
					strcat(optline,dpopt);
				}
				if (count == 0) {
					sprintf(dpopt,"%s",optline);
				} else {
					sprintf(dpopt,",%s",optline);
				}
				count++;
				strcat(line,dpopt);
				}

				/* get options... */
				op = (XF86OptionPtr) dp->disp_option_lst;
				if (op != NULL) {
					strcpy(optline,"option=");
					anz = 0;
					for (op;op;op=op->list.next) {
						strcpy(optn,"<none>");
						strcpy(optv,"<none>");
						if (op->opt_name != NULL) strcpy(optn,op->opt_name);
						if (op->opt_val  != NULL) strcpy(optv,op->opt_val);
						if (anz == 0) {
							sprintf(dpopt,"optname:%s",optn);
						} else {
							sprintf(dpopt,",optname:%s",optn);
						}
						anz++;
						strcat(optline,dpopt);
						if (anz == 0) {
							sprintf(dpopt,"optval:%s",optv);
						} else {
							sprintf(dpopt,",optval:%s",optv);
						}
						anz++;
						strcat(optline,dpopt);
					}
					if (count == 0) {
						sprintf(dpopt,"%s",optline);
					} else {
						sprintf(dpopt,",%s",optline);
					}
					count++;
					strcat(line,dpopt);
				}
     
				/* add display line to result string... */
				strcat(result,line);
			}
		}
	}
	return(result);
}

/*****************************************
 error functions needed by libxf86config
*****************************************/
void VErrorF(const char *f, va_list args) {
	vfprintf(stderr, f, args);
}

void ErrorF(const char *f, ...) {
	va_list args;
	va_start(args, f);
	vfprintf(stderr, f, args);
	va_end(args);
}

