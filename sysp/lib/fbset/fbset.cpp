/**************
FILE          : fbset.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : framebuffer implementation
              : © Copyright 1995-1998 by Geert Uytterhoeven
              :
              :
STATUS        : development
**************/
#include "fbset.h"

int OpenFrameBuffer(const char *name) {
	int fh;
	fh = open (name, O_RDONLY);
	return fh;
}


void CloseFrameBuffer(int fh) {
	close(fh);
}

void GetVarScreenInfo(int fh, fb_var_screeninfo *var) {
	if (ioctl(fh, FBIOGET_VSCREENINFO, var)) {
		fprintf(stderr,"ioctl FBIOGET_VSCREENINFO: %s\n", strerror(errno));
	}
}

static void ConvertToVideoMode(
	fb_var_screeninfo *var, VideoMode *vmode)
{
	vmode->name = NULL;
	vmode->xres = var->xres;
	vmode->yres = var->yres;
	vmode->vxres = var->xres_virtual;
	vmode->vyres = var->yres_virtual;
	vmode->depth = var->bits_per_pixel;
	vmode->nonstd = var->nonstd;
	vmode->accel_flags = var->accel_flags;
	vmode->pixclock = var->pixclock;
	vmode->left = var->left_margin;
	vmode->right = var->right_margin;
	vmode->upper = var->upper_margin;
	vmode->lower = var->lower_margin;
	vmode->hslen = var->hsync_len;
	vmode->vslen = var->vsync_len;
	vmode->hsync = var->sync & FB_SYNC_HOR_HIGH_ACT ? HIGH : LOW;
	vmode->vsync = var->sync & FB_SYNC_VERT_HIGH_ACT ? HIGH : LOW;
	vmode->csync = var->sync & FB_SYNC_COMP_HIGH_ACT ? HIGH : LOW;
	vmode->gsync = var->sync & FB_SYNC_ON_GREEN ? TRUE : FALSE;
	vmode->extsync = var->sync & FB_SYNC_EXT ? TRUE : FALSE;
	vmode->bcast = var->sync & FB_SYNC_BROADCAST ? TRUE : FALSE;
	vmode->grayscale = var->grayscale;
	vmode->laced = FALSE;
	vmode->dblscan = FALSE;
	switch (var->vmode & FB_VMODE_MASK) {
	case FB_VMODE_INTERLACED:
		vmode->laced = TRUE;
		break;
	case FB_VMODE_DOUBLE:
		vmode->dblscan = TRUE;
		break;
	}
	vmode->red.length = var->red.length;
	vmode->red.offset = var->red.offset;
	vmode->green.length = var->green.length;
	vmode->green.offset = var->green.offset;
	vmode->blue.length = var->blue.length;
	vmode->blue.offset = var->blue.offset;
	vmode->transp.length = var->transp.length;
	vmode->transp.offset = var->transp.offset;
	FillScanRates(vmode);
}

FbData* FbGetData(void) {
	FbData *fb;
	VideoMode vmode;
	fb_var_screeninfo var;
	int res,sstart,send,total;
	str ht,vt;
	str flags;
	int fh = -1; 

	fb = (FbData*)malloc(sizeof(FbData));
	fb->x     = 640;
	fb->y     = 480;
	fb->depth = 8;
	fb->hsync = 33; 
	fb->vsync = 72;
	fb->clock = 35;
	strcpy(fb->ht,"");
	strcpy(fb->vt,"");
	strcpy(fb->flags,"");

	fh = OpenFrameBuffer(DEFAULT_FRAMEBUFFER);
	if (fh < 0) {
		return(fb);
	}
	GetVarScreenInfo(fh, &var);
	ConvertToVideoMode(&var, &vmode);

	#if !defined(__PPC__)
	// .../
	// limit the framebuffer to a 60Hz mode
	// ...
	vmode.hrate = (60.0 / vmode.vrate) * vmode.hrate;
	vmode.drate = (vmode.xres+vmode.right+vmode.hslen+vmode.left) * vmode.hrate;
	vmode.vrate = 60;
	#endif

	fb->x     = vmode.xres;
	fb->y     = vmode.yres;
	fb->depth = vmode.depth;
	fb->hsync = vmode.hrate/1e3;
	fb->vsync = vmode.vrate;
	fb->clock = vmode.drate/1e6;

	res = vmode.xres;
	sstart = res+vmode.right;
	send = sstart+vmode.hslen;
	total = send+vmode.left;
	sprintf(ht,"%d %d %d %d",res, sstart, send, total);

	res = vmode.yres;
	sstart = res+vmode.lower;
	send = sstart+vmode.vslen;
	total = send+vmode.upper;
	sprintf(vt,"%d %d %d %d", res, sstart, send, total);

	strcpy(flags,"");
	if (vmode.laced) {
		strcat(flags,"Interlace ");
	}
	if (vmode.dblscan) {
		strcat(flags,"DoubleScan ");
	}
	if (vmode.hsync) {
		strcat(flags,"+HSync ");
	} else {
		strcat(flags,"-HSync ");
	}
	if (vmode.vsync) {
		strcat(flags,"+VSync ");
	} else {
		strcat(flags,"-VSync ");
	}
	if (vmode.csync) {
		strcat(flags,"Composite ");
	}
	if (vmode.bcast) {
		strcat(flags,"bcast ");
	}

	strcpy(fb->ht,ht);
	strcpy(fb->vt,vt);
	strcpy(fb->flags,flags);
	CloseFrameBuffer(fh);
	return(fb);
}

int FillScanRates (VideoMode *vmode) {
	u_int htotal = vmode->left+vmode->xres+vmode->right+vmode->hslen;
	u_int vtotal = vmode->upper+vmode->yres+vmode->lower+vmode->vslen;

	if (vmode->dblscan) {
		vtotal <<= 2;
	} else if (!vmode->laced) {
		vtotal <<= 1;
	}
	if (!htotal || !vtotal) {
		return 0;
	}
	if (vmode->pixclock) {
		vmode->drate = 1E12/vmode->pixclock;
		vmode->hrate = vmode->drate/htotal;
		vmode->vrate = vmode->hrate/vtotal*2;
	} else {
		vmode->drate = 0;
		vmode->hrate = 0;
		vmode->vrate = 0;
	}
	return 1;
}
