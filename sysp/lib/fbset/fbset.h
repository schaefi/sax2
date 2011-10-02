/**************
FILE          : fb.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : framebuffer header file and definitions  
              :
              :
STATUS        : development
**************/
#ifndef _LINUX_FB_H
#define _LINUX_FB_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <sys/types.h>

//========================================
// Defines
//----------------------------------------
#define LOW         (0)
#define HIGH        (1)
#define FALSE       (0)
#define TRUE        (1)

#define DEFAULT_FRAMEBUFFER "/dev/fb0"
#define FBIOGET_VSCREENINFO	0x4600
#define FBIOPUT_VSCREENINFO	0x4601
#define FBIOGET_FSCREENINFO	0x4602

#define FB_NONSTD_HAM				1	// Hold-And-Modify (HAM)        

#define FB_ACTIVATE_NOW				0	// set values immediately (or vbl)
#define FB_ACTIVATE_NXTOPEN			1	// activate on next open	
#define FB_ACTIVATE_TEST			2	// don't set, round up impossible 
#define FB_ACTIVATE_MASK       		15
										// values			
#define FB_ACTIVATE_VBL	       		16	// activate values on next vbl  
#define FB_CHANGE_CMAP_VBL     		32	// change colormap on vbl	
#define FB_ACTIVATE_ALL	       		64	// change all VCs on this fb

#define FB_ACCELF_TEXT				1	// text mode acceleration 

#define FB_SYNC_HOR_HIGH_ACT		1	// horizontal sync high active	
#define FB_SYNC_VERT_HIGH_ACT		2	// vertical sync high active	
#define FB_SYNC_EXT					4	// external sync		
#define FB_SYNC_COMP_HIGH_ACT		8	// composite sync high active   
#define FB_SYNC_BROADCAST			16	// broadcast video timings      
										// vtotal = 144d/288n/576i => PAL 
										// vtotal = 121d/242n/484i => NTSC
#define FB_SYNC_ON_GREEN			32	// sync on green

#define FB_VMODE_NONINTERLACED  	0	// non interlaced 
#define FB_VMODE_INTERLACED			1	// interlaced	
#define FB_VMODE_DOUBLE				2	// double scan 
#define FB_VMODE_MASK				255

#define FB_VMODE_YWRAP				256	// ywrap instead of panning     
#define FB_VMODE_SMOOTH_XPAN		512	// smooth xpan possible 
#define FB_VMODE_CONUPDATE			512	// don't update x/yoffset	


//==================================
// Structures (fbset)...
//----------------------------------
// .../
// Interpretation of offset for color fields: All offsets are from
// the right,inside a "pixel" value, which is exactly 'bits_per_pixel'
// wide (means: you can use the offset as right argument to <<). A pixel
// afterwards is a bit stream and is written to video memory as that
// unmodified. This implies big-endian byte order if bits_per_pixel
// is greater than 8.
// ----
typedef struct _fb_bitfield {
	__u32 offset;			// beginning of bitfield	
	__u32 length;			// length of bitfield		
	__u32 msb_right;		// != 0 : Most significant bit is right 
} fb_bitfield;

typedef struct _fb_var_screeninfo {
	__u32 xres;				// visible resolution
	__u32 yres;
	__u32 xres_virtual;		// virtual resolution
	__u32 yres_virtual;
	__u32 xoffset;			// offset from virtual to visible */
	__u32 yoffset;			// resolution	

	__u32 bits_per_pixel;	// guess what		
	__u32 grayscale;		// != 0 Graylevels instead of colors 

	struct _fb_bitfield red;		// bitfield in fb mem if true color,
	struct _fb_bitfield green;	// else only length is significant 
	struct _fb_bitfield blue;
	struct _fb_bitfield transp;	// transparency				

	__u32 nonstd;			// != 0 Non standard pixel format 

	__u32 activate;			// see FB_ACTIVATE_*		

	__u32 height;			// height of picture in mm   
	__u32 width;			// width of picture in mm     

	__u32 accel_flags;		// acceleration flags (hints)	

	__u32 pixclock;			// pixel clock in ps (pico seconds) 
	__u32 left_margin;		// time from sync to picture
	__u32 right_margin;		// time from picture to sync
	__u32 upper_margin;		// time from sync to picture
	__u32 lower_margin;
	__u32 hsync_len;		// length of horizontal sync
	__u32 vsync_len;		// length of vertical sync	
	__u32 sync;				// see FB_SYNC_*		
	__u32 vmode;			// see FB_VMODE_*		
	__u32 reserved[6];		// Reserved for future compatibility 
} fb_var_screeninfo;

//=================================
// Structures (FbGetData)...
//---------------------------------
typedef char str[512];

typedef struct _FbData {
	int depth;
	int x;
	int y;
	float hsync;
	float vsync;
	float clock;
	str ht;
	str vt;
	str flags;
} FbData;

//=================================
// Structures...
//---------------------------------
typedef struct _color {
	unsigned int length;
	unsigned int offset;
} color;

typedef struct _VideoMode {
	struct _VideoMode *next;
	char *name;
	__u32 xres;
	__u32 yres;
	__u32 vxres;
	__u32 vyres;
	__u32 depth;
	__u32 nonstd;
	__u32 accel_flags;
	__u32 pixclock;
	__u32 left;
	__u32 right;
	__u32 upper;
	__u32 lower;
	__u32 hslen;
	__u32 vslen;
	unsigned hsync : 1;
	unsigned vsync : 1;
	unsigned csync : 1;
	unsigned gsync : 1;
	unsigned extsync : 1;
	unsigned bcast : 1;
	unsigned laced : 1;
	unsigned dblscan : 1;
	unsigned grayscale : 1;
	double drate;
	double hrate;
	double vrate;
	struct _color red, green, blue, transp;
} VideoMode;

//=================================
// Functions...
//---------------------------------
int FillScanRates (VideoMode *vmode);
FbData* FbGetData (void);

#endif
