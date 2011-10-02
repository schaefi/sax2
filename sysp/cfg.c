/**************
FILE          : cfg.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XF86ConfigFile class methods to create
              : a probeonly structure <MsgDetect>
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/Xproto.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "cfg.h"
#include "config.h"
#include "lib/syslib.h"

//=========================================
// XF86ConfigFile: constructor...
//-----------------------------------------
XF86ConfigFile::XF86ConfigFile (void) {
	mouseprotocol = "Auto";
	mousedevice   = "/dev/mouse";
	driver        = "vga";
	memory        = 0;
	#if __powerpc__
	colordepth    = 16;
	#else
	colordepth    = 16;
	#endif
	id       = 0;
	elements = 0;
	current  = 0;
	idc      = 0;
}

//=========================================
// XF86ConfigFile: create Files section
//-----------------------------------------
string XF86ConfigFile::DoFilesSection (void) {
	string section;
	section = "Section \"Files\"\n";
	section = section + " FontPath \"/usr/lib/X11/fonts/misc\"\n";
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create ServerFlags section
//-----------------------------------------
string XF86ConfigFile::DoServerFlagsSection (void) {
	string section;
	section = "Section \"ServerFlags\"\n";
	section = section + " Option \"SyncLog\" \"on\"\n";
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create Module section
//-----------------------------------------
string XF86ConfigFile::DoModuleSection (void) {
	string section;
	section = "Section \"Module\"\n";
	section = section + " Load \"extmod\"\n";
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create I. Device section
//-----------------------------------------
string XF86ConfigFile::DoInputDeviceSection (void) {
	string section;
	section = "Section \"InputDevice\"\n";
	section = section + " Driver     \"kbd\"\n";
	section = section + " Identifier \"Keyboard[0]\"\n";
	section = section + " Option     \"XkbLayout\" \"us\"\n";
	section = section + " Option     \"XkbModel\"  \"pc104\"\n";
	section = section + "EndSection\n\n"; 
	section = section + "Section \"InputDevice\"\n";
	section = section + " Driver     \"mouse\"\n";
	section = section + " Identifier \"Mouse[1]\"\n";
	section = section + " Option     \"Device\" "; 
	section = section + "\"" + mousedevice   + "\"\n";
	section = section + " Option     \"Protocol\" ";
	section = section + "\"" + mouseprotocol + "\"\n";
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create Monitor section
//-----------------------------------------
string XF86ConfigFile::DoMonitorSection (void) {
	string section;
	str ident ; sprintf(ident," Identifier \"Monitor[%d]\"\n",id);
	section = "Section \"Monitor\"\n";
	section = section + ident;
	section = section + " HorizSync     30-33\n";
	section = section + " VertRefresh   50-72\n";
	section = section + "EndSection\n"; 
	return(section);
}

//=========================================
// XF86ConfigFile: create Screen section
//-----------------------------------------
string XF86ConfigFile::DoScreenSection (void) {
	string section;
	string depth[6];

	depth[0] = "4";   // 4  bit
	depth[1] = "8";   // 8  bit
	depth[2] = "15";  // 15 bit
	depth[3] = "16";  // 16 bit
	depth[4] = "24";  // 24 bit
	depth[5] = "32";  // 32 bit
	str bpp ; sprintf(bpp,"%d",colordepth);
	str id1 ; sprintf(id1," Identifier \"Screen[%d]\"\n",id);
	str id2 ; sprintf(id2," Device     \"Device[%d]\"\n",id);
	str id3 ; sprintf(id3," Monitor    \"Monitor[%d]\"\n",id);
	section = "Section \"Screen\"\n";
	// ...
	// include DefaultDepth for r128 based cards
	// otherwhise the memory manager got problems...
	// -----------------------------------------------
	if (driver == "r128") {
		section = section + " DefaultDepth  " + bpp + "\n";
	}
	for (int i=0;i<6;i++) {
		section = section + " SubSection \"Display\"\n";
		section = section + "  Depth         " + depth[i] + "\n";       
		section = section + "  Modes        \"640x480\"\n"; 
		section = section + " EndSubSection\n";
	}
	section = section + id1;
	section = section + id2;
	section = section + id3;
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create Device section
//-----------------------------------------
string XF86ConfigFile::DoDeviceSection (void) {
	string section;
	str ident ; sprintf(ident," Identifier \"Device[%d]\"\n",id);
	#if __powerpc__
	str fbdevice; sprintf(fbdevice, " Option \"fbdev\" \"/dev/fb%d\"\n",id);
	#endif
	list<string> opt;
	char* token;
	string data;
	string option;
	string videoram;
	str memstr;
	str work;

	// get the options...
	// --------------------
	strcpy(work,deviceopt.c_str());
	token = strtok(work,",");
	if (token != NULL) {
		trim(token); data = token;
		opt.push_back(data);
	}
	while (token) {
	token = strtok(NULL,",");
	if (token != NULL) {
		trim(token); data = token;
		opt.push_back(data);
	}
	}

	// create the section...
	// -----------------------
	section = "Section \"Device\"\n";
	if ((driver != "vmware") && (driver != "fbdev")) {
		section = section + " BusID \""  + busid  + "\"\n";
	}
	section = section + " Driver \"" + driver + "\"\n";
	#if __powerpc__
	if (driver == "fbdev") {
		section = section + fbdevice;
	}
	#endif
	if (memory > 0) {
		sprintf(memstr,"%d",memory); videoram = memstr;
		section = section + " VideoRam " + videoram + "\n";
	}
	while(! opt.empty()) {
		option = opt.front(); opt.pop_front();
		section = section + " Option \"" + option + "\"\n";
	}
	section = section + ident;
	section = section + "EndSection\n";
	return(section);
}

//=========================================
// XF86ConfigFile: create Layout section
//-----------------------------------------
string XF86ConfigFile::DoServerLayoutSection (void) {
	string section;
	str screen;
	section = "Section \"ServerLayout\"\n";
	section = section + " Identifier   \"Layout[all]\"\n";
	section = section + " InputDevice  \"Keyboard[0]\" \"CoreKeyboard\"\n";
	section = section + " InputDevice  \"Mouse[1]\"    \"CorePointer\"\n";
	for(int i=0;i<id;i++) {
		sprintf(screen," Screen       \"Screen[%d]\"\n",i);
		section = section + screen;
	}
	section = section + "EndSection\n";
	return (section);
}

//=========================================
// XF86ConfigFile: set mouse data...
//-----------------------------------------
void XF86ConfigFile::SetMouseProperties (str prot,str dev) {
	mouseprotocol = prot;
	mousedevice   = dev;
}

//=========================================
// XF86ConfigFile: set color depth...
//-----------------------------------------
void XF86ConfigFile::SetColorDepth (int depth) {
	colordepth = depth;
}

//=========================================
// XF86ConfigFile: set device options...
//-----------------------------------------
void XF86ConfigFile::SetDeviceOption (string opt) {
	deviceopt = opt;
}

//=========================================
// XF86ConfigFile: set BusID string...
//-----------------------------------------
void XF86ConfigFile::SetBus (int d,int b,int s,int f) {
	str busid_str;
	#if __powerpc__
		sprintf(busid_str,"%d:%d:%d",b,s,f);
	#else
		sprintf(busid_str,"PCI:%d@%d:%d:%d",b,d,s,f);
	#endif
	busid = busid_str;
	bus[idc]  = b;
	slot[idc] = s; 
	func[idc] = f;
	idc++;
}

//=========================================
// XF86ConfigFile: set driver line...
//-----------------------------------------
int XF86ConfigFile::SetDriver (string module) {
	driver = module;
	if (driver == "fbdev") {
		int fb = open("/dev/fb0", O_RDONLY);
		if (fb < 0) { 
			return(1); 
		}
		close(fb); 
	}
	return(0);
}

//=========================================
// XF86ConfigFile: set memory line...
//-----------------------------------------
void XF86ConfigFile::SetVideoRam (int mem) {
	memory = mem; 
}

//=========================================
// XF86ConfigFile: set section id...
//-----------------------------------------
void XF86ConfigFile::SetSectionID (int nr) {
	id = nr;
}

//=========================================
// XF86ConfigFile: kill running server
//-----------------------------------------
void XF86ConfigFile::ShutdownServer (int spid, int disp) {
	str lock ; sprintf(lock,"/tmp/.X%d-lock",disp);
	int count = 0;
	kill(spid,15);
	while(1) {
		ifstream handle(lock);
		if (! handle) {
			break;
		}
		handle.close();
		sleep(1);
		count++;
		if (count >= 3) {
			unlink(lock); kill(spid,9);
			break;
		}
	}
	sleep(2);
}

//=========================================
// XF86ConfigFile: check randr data...
//-----------------------------------------
string XF86ConfigFile::CallRandR (str file) {
	//=======================================
	// Call a server and open the display
	//---------------------------------------
	//int disp = GetDisplay();
	int disp = 99;
	str scr  ; sprintf(scr,":%d",disp);
	str log  ; sprintf(log,"/var/log/%s.%d.log",LOADER_NAME,disp);
	str lgv  ; sprintf(lgv,"255");
	str bground; sprintf(bground,"-br");
	Display   *dpy  = NULL;
	int spid    = 0;
	int hasRR12 = False;
	int major, minor;
	string result = "";
	string proc = qx(
		XWRAPPER,STDOUT,8,"%s %s %s %s %s %s %s %s",
		XW_LOG,BLANK,VERBOSE,lgv,CONFIG,file,bground,scr
	);
	spid = atoi(proc.c_str());
	dpy = XOpenDisplay (scr);
	if (! dpy) {
		ShutdownServer (spid,disp);
		return result;
	}
	//=======================================
	// check randr extension
	//---------------------------------------
	if (! XRRQueryVersion (dpy, &major, &minor)) {
		//printf ("RandR extension missing\n");
		XCloseDisplay (dpy);
		ShutdownServer (spid,disp);
		return result;
	}
	if (major > 1 || (major == 1 && minor >= 2)) {
		hasRR12 = True;
	}
	if (! hasRR12) {
		//printf ("RandR version 1.2 required, got: %d.%d\n",major,minor);
		XCloseDisplay (dpy);
		ShutdownServer (spid,disp);
		return result;
	}
	//=======================================
	// get output information
	//---------------------------------------
	#if RANDR_MINOR >= 2
	int screen = DefaultScreen (dpy);
	int root   = RootWindow    (dpy, screen);
	int m = 0;
	int o = 0;
	XRRScreenResources* res = XRRGetScreenResources (dpy, root);
	if (! res) {
		//printf ("Couldn't get screen resources");
		XCloseDisplay (dpy);
		ShutdownServer (spid,disp);
		return result;
	}
	for (o = 0; o < res->noutput; o++) {
		XRROutputInfo* output = XRRGetOutputInfo (dpy, res, res->outputs[o]);
		if (! output) {
			//printf ("Couldn't get output 0x%x data",
			//	(unsigned int)res->outputs[o]
			//);
			XCloseDisplay (dpy);
			ShutdownServer (spid,disp);
			return result;
		}
		static const char *connect_state[3] = {
			"connected", "disconnected", "unknown"
		};
		//char pref[20];
		//sprintf(pref,"%d",output->npreferred);
		result = result + string(output->name)
			+ " " + string(connect_state[output->connection]);
			//+ " " + string(pref)
			//+ " ";
		for (m = 0; m < output->nmode; m++) {
			XRRModeInfo *mode = NULL;
			int n; for (n = 0; n < res->nmode; n++)
			if (res->modes[n].id == output->modes[m]) {
				mode = &res->modes[n];
			}
			float rate;
			if (mode->hTotal && mode->vTotal) {
			rate = ((float) mode->dotClock / 
				((float) mode->hTotal * (float) mode->vTotal));
			} else {
				rate = 0;
			}
			if (mode) {
				char width[20];
				char height[20];
				char srate[20];
				sprintf (width,"%d",mode->width);
				sprintf (height,"%d",mode->height);
				sprintf (srate,"%.0f",rate);
				//result = result
				//	+ string(mode->name) + "="
				//	+ string(width) + "x" + string(height)
				//	+ "@" + string(srate);
			}
			if (m < output->nmode -1) {
				//result = result + ",";
			}
		}
		if (o < res->noutput - 1) {
			result = result + "%";
		}
	}
	#endif
	//=======================================
	// clean up
	//---------------------------------------
	XCloseDisplay (dpy);
	ShutdownServer (spid,disp);
	return result;
}

//=========================================
// XF86ConfigFile: call XF86 loader...
//-----------------------------------------
void XF86ConfigFile::CallXF86Loader (str file) {
	//int dpy  = GetDisplay();
	int dpy = 99;
	str scr  ; sprintf(scr,":%d",dpy);
	str log  ; sprintf(log,"/var/log/%s.%d.log",LOADER_NAME,dpy);
	str lgv  ; sprintf(lgv,"255");
	str bground; sprintf(bground,"-br");
	MsgDetect *parse = NULL;
	Display   *disp  = NULL;
	int spid  = 0;
	int vmd   = 0;

	if (access(log,R_OK) != 0) {
		string proc = qx(
			XWRAPPER,STDOUT,8,"%s %s %s %s %s %s %s %s",
			XW_LOG,BLANK,VERBOSE,lgv,CONFIG,file,bground,scr
		);
		disp = XOpenDisplay (scr);
		if (disp) {
			vmd = DisplayPlanes (disp, DefaultScreen(disp));
			XCloseDisplay (disp);
		}
		spid = atoi(proc.c_str());
		ShutdownServer (spid,dpy);
	}
	parse = PLogGetData(log);
	//unlink(log);
 
	if (parse != NULL) { 
	for (int i=0;i<parse[0].cards;i++) {
		ParseData plog;
		plog.id        = parse[i].id;
		plog.clock     = parse[i].clkstr;
		plog.ddc       = parse[i].ddc;
		plog.dtype     = parse[i].displaytype;
		plog.primary   = parse[i].primary;
		plog.chipset   = parse[i].chipset;
		plog.hsmax     = parse[i].hsync_max;
		plog.vsmax     = parse[i].vsync_max;
		plog.dacspeed  = parse[i].dacspeed;
		plog.videoram  = parse[i].memory;
		plog.modecount = parse[i].vesacount;
		plog.dpix      = parse[i].dpix;
		plog.dpiy      = parse[i].dpiy;

		plog.bus       = bus[i];
		plog.slot      = slot[i];
		plog.func      = func[i];
		plog.vmdepth   = vmd;

		for (int n=0;n<parse[i].vesacount;n++) {
			XMode mode;
			mode.x        = parse[i].vmodes[n].x;
			mode.y        = parse[i].vmodes[n].y;
			mode.hsync    = parse[i].vmodes[n].hsync;
			mode.vsync    = parse[i].vmodes[n].vsync;
			plog.modes[n] = mode;
		}

		GetBusFormat(
			parse[i].primary,
			&plog.pbus,&plog.pslot,&plog.pfunc
		);
		if ((plog.pbus == 0) && (plog.pslot == 0) && (plog.pfunc == 0)) {
			plog.pbus  = bus[0];
			plog.pslot = slot[0];
			plog.pfunc = func[0];
		}

		Push(plog);
	}
	} else {
		// no parse information...
	}
}

//=========================================
// XF86ConfigFile: refresh map...
//-----------------------------------------
void XF86ConfigFile::Reset (void) {
	current = 0;  
}

//=========================================
// XF86ConfigFile: retrieve ParseData...
//-----------------------------------------
ParseData XF86ConfigFile::Pop (void) {
	if (current >= elements) {
		throw ReadLastElement();
	}
	ParseData element(qP[current]);
	current++;
	return element;
}

//=========================================
// XF86ConfigFile: push data...
//-----------------------------------------
void XF86ConfigFile::Push (ParseData e) {
	qP[elements] = e;
	elements++;
}

//======================================
// XF86ConfigFile: set file for saving
//--------------------------------------
void XF86ConfigFile::SetFile(const str name) {
	file = name;
}

//======================================
// XF86ConfigFile: read data from file
//--------------------------------------
int XF86ConfigFile::Read(void) {
	ifstream handle(file.c_str(),ios::binary); 

	if (! handle) {
		//cout << "XF86ConfigFile: could not open file: ";
		//cout << file << endl;
		return(-1);
	}
	elements = 0;
	current  = 0;
	qP.clear();

	while(1) {
		ParseData data;
		ParseData_S part1; 
 
		// read base data...
		// ------------------- 
		handle.read((char*)&part1,sizeof(part1));
		data.id         = part1.id;
		data.clock      = part1.clock;
		data.ddc        = part1.ddc;
		data.dtype      = part1.dtype;
		data.primary    = part1.primary;
		data.pbus       = part1.pbus;
		data.pslot      = part1.pslot;
		data.pfunc      = part1.pfunc;
		data.chipset    = part1.chipset;
		data.hsmax      = part1.hsmax;
		data.vsmax      = part1.vsmax;
		data.modecount  = part1.modecount;
		data.dacspeed   = part1.dacspeed; 
		data.videoram   = part1.videoram;
		data.bus        = part1.bus;
		data.slot       = part1.slot;
		data.func       = part1.func;
		data.dpix       = part1.dpix;
		data.dpiy       = part1.dpiy;
		data.vmdepth    = part1.vmdepth;

		// read mode list...
		// -------------------
		for (int i=0;i<part1.modecount;i++) {
			XMode part2;
			handle.read((char*)&part2,sizeof(part2));
			data.modes[i] = part2;
		}
		if (handle.eof()) {
			break;
		}
		Push(data);
	}
	handle.close(); 
	return(0);
}

//======================================
// XF86ConfigFile: save data to file
//--------------------------------------
int XF86ConfigFile::Save(void) {
	ofstream handle(file.c_str(),ios::binary);

	if (! handle) {
		cout << "XF86ConfigFile: could not create file: ";
		cout << file << endl;
		return(-1);
	}

	Reset();
	for (int i = Count(); i > 0; i--) {
		ParseData data = Pop();
		ParseData_S part1;

		// save the single struct data 
		// -----------------------------
		strcpy(part1.id      , data.id.c_str());
		strcpy(part1.clock   , data.clock.c_str());
		strcpy(part1.ddc     , data.ddc.c_str());
		strcpy(part1.primary , data.primary.c_str());
		strcpy(part1.chipset , data.chipset.c_str());
		strcpy(part1.dtype   , data.dtype.c_str());
		part1.vmdepth        = data.vmdepth;
		part1.pbus           = data.pbus;
		part1.pslot          = data.pslot; 
		part1.pfunc          = data.pfunc; 
		part1.hsmax          = data.hsmax;
		part1.vsmax          = data.vsmax;
		part1.dacspeed       = data.dacspeed;
		part1.videoram       = data.videoram;
		part1.bus            = data.bus;
		part1.slot           = data.slot;
		part1.func           = data.func;
		part1.modecount      = data.modecount;
		part1.dpix           = data.dpix;
		part1.dpiy           = data.dpiy;
		handle.write((char*)&part1,sizeof(part1));

		// save the mode structs...
		// -------------------------
		for (int i=0;i<(int)data.modes.size();i++) {
			XMode part2;
			part2.x     = data.modes[i].x;
			part2.y     = data.modes[i].y;
			part2.hsync = data.modes[i].hsync;
			part2.vsync = data.modes[i].vsync;
			handle.write((char*)&part2,sizeof(part2));
		}
	}
	handle.close();
	return(0);
}
