/***************
FILE          : xmode.h
***************
PROJECT       : X11 Modeline algorithm
              :
BELONGS TO    : X11 Inc.
              :  
              :
DESCRIPTION   : header file for xmode.c
              : install this header to /usr/include/
              : 
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XMODE_H
#define XMODE_H 1

//===========================================
// Defines...
//-------------------------------------------
#define TRUE  1
#define FALSE 0
#define FLAG_DOUBLE_SCAN  7

#define MARGIN_PERCENT    1.8   // % of active vertical image
#define CELL_GRAN         8.0   // assumed character cell granularity
#define MIN_PORCH         1     // minimum front porch
#define V_SYNC_RQD        3     // width of vsync in lines
#define H_SYNC_PERCENT    8.0   // width of hsync as % of total line
#define MIN_VSYNC_PLUS_BP 550.0 // min time of vsync + back porch (microsec)
#define M                 600.0 // blanking formula gradient
#define C                 40.0  // blanking formula offset
#define K                 128.0 // blanking formula scaling factor
#define J                 20.0  // blanking formula scaling factor

#define C_PRIME           (((C - J) * K/256.0) + J)
#define M_PRIME           (K/256.0 * M)

//===========================================
// Structs...
//-------------------------------------------
struct xmode {
 double dac;
 float  hsync;
 float  vsync;
 int    x;
 int    y;
 int    fbdev;
 int    flag;
 int    interlaced;
 int    margins;
};

struct xtiming {
 int hdisp;
 int hsyncstart;
 int hsyncend;
 int htotal;
 int vdisp;
 int vsyncstart;
 int vsyncend;
 int vtotal;
 int flag;
 double dac;
 float hsync;
 float vsync;
};

//===========================================
// Prototypes...
//-------------------------------------------
int  XmodeInit     (int argc,char *argv[],struct xmode *data);
int  XmodeTiming   (struct xmode *data,struct xtiming *vt);
int  XmodeFbTiming (struct xtiming *t);
void XmodeUsage    (void);

#endif
