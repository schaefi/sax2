/**************
FILE          : syslib.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the libsysp  
              : --> syslib.h
              :
STATUS        : development
**************/

#ifndef SYSLIB_H

//==============================
// Definitions...
//------------------------------
#define MAX_PROGRAM_SIZE  256
#define MAXVESA           100
#define IS_TRUE           1
#define IS_FALSE          0

#define SYSLIB_H          1

//==============================
// Types...
//------------------------------
typedef char str[512];

//==============================
// Structures...
//------------------------------
typedef struct { str key,value,id; } LOGSTRUCT; 
typedef struct { str clk,id; } CLKSTRUCT;

typedef struct {
 int x;
 int y;
 int hsync;
 int vsync;
} mode;

typedef struct MsgDetect_t {
 str id;
 int cards;
 long memory;
 int dacspeed;
 int clkcount;
 float *clocks;
 int vesacount;
 mode vmodes[MAXVESA];
 unsigned bandwidth;
 str modeline;
 int hsync_max;
 int vsync_max;
 str chipset;
 str primary;
 str ddc;
 str dds;
 str clkstr;
 str displaytype;
 int vmdepth;
 int dpix;
 int dpiy;
 str model;
 str vendor;
 int port;
 struct MsgDetect_t* next;
} MsgDetect;

typedef struct PciData_t {
 int domain;
 int cls;
 int bus;
 int slot;
 int func;
 int vid;
 int did;
 int svid;
 int sdid;
 struct PciData_t* next;
} PciData;

typedef struct {
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

typedef struct FbBootEntry_t {
 int mode;
 int depth;
 int x;
 int y;
 struct FbBootEntry_t* next;
} FbBootEntry;

typedef struct FbBootData_t {
 int fbcount;
 FbBootEntry* entry;
 struct FbBootData_t* next;
} FbBootData;

typedef struct MouseData_t {
 str name;
 str protocol;
 str device;
 str did;
 str vid;
 str profile;
 str driver;
 str realdev;
 int emulate;
 int wheel;
 int buttons;
 struct MouseData_t* next;
} MouseData;

typedef struct KbdData_t {
 str rules;
 str model;
 str layout;
 str name;
 str device;
 str did;
 str vid;
 str profile;
 struct KbdData_t* next;
} KbdData;

//==============================
// Functions...
//------------------------------
extern MsgDetect* PLogGetData (str logfile);
extern PciData* PciGetData(void);
extern FbData* FbGetData(void);
extern MouseData* MouseGetData(void);
extern KbdData* KeyboardGetData(void); 
extern MsgDetect* MonitorGetData(void);
extern FbBootData* FrameBufferGetData(void);
extern unsigned long MemorySize (char*);
extern int TvSupport (void);
extern char* vesaBIOS (void);
extern char* mediaDevices (void);
extern void trim(char *message);
extern char* qx(const char*command,int channel,int anz=0,const char* format=NULL,...);
extern int GetDisplay (void);
extern void GetBusFormat(char *idstr,int *bus,int* slot,int *func);
extern int GetDValue(char *hex);
extern int chvt (int);
extern int getvt (void);

#endif

