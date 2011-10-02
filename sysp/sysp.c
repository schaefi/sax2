/**************
FILE          : sysp.cc
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : sysp <System Profiler> is used to obtain any 
              : information needed for X11 configuration tasks
              : Modules:
              :  - ScanMouse  
              :  - ScanKeyboard
              :  - ScanServer
              :  - ScanXStuff
              :  - Scan3DLib
              :
STATUS        : Status: Up-to-date
**************/
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <queue>
#include <map>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <getopt.h>

using namespace std;

#include "lib/syslib.h"
#include "sysp.h"
#include "mouse.h"
#include "keyboard.h"
#include "server.h"
#include "cfg.h"
#include "xstuff.h"
#include "d3.h"

#include "config.h"

//============================================
// Defines...
//--------------------------------------------
#define SCANNER      0
#define PROFILE      1
#define QUERIES      2
#define MEDIADEVICES 3

//============================================
// Globals...
//--------------------------------------------
int UseXForQuestions  = 0;
int InfoOnly          = 0;
int AskForFlag        = 1;
int CheckQuestionOnly = 0;
int CheckPCIVendor    = 0;
int task              = 10;
int checkFlag         = 1;
str themodule         = "";
string ModuleToUse    = "";
string CardToUse      = "";


//============================================
// Functions...
//--------------------------------------------
void usage(void);
void ScanModule(str name);
void RemoveRegistry(void);
void QueryModule(str name);
void ShowCards(void);
void ShowProfile(void);
void ShowMediaDevices(void);
void CheckRoot(void);
void PrintMouseData(ScanMouse m);
void PrintKeyboardData(ScanKeyboard k);
void PrintServerData(ScanServer s);
void PrintStuffData(ScanXStuff s);
void Print3DData (Scan3D d);
void SyncOnDisk (void);

//============================================
// Functions...
//--------------------------------------------
char* GetProfileDriver ( string );

//============================================
// Main
//--------------------------------------------
int main(int argc,char *argv[]) {
	int c;
	//===========================================
	// get commandline options...
	//-------------------------------------------
	while (1) {
		int option_index = 0;
		static struct option long_options[] =
		{
			{"scan"        , 1 , 0 , 's'},
			{"query"       , 1 , 0 , 'q'},
			{"cards"       , 0 , 0 , 'c'},
			{"chip"        , 1 , 0 , 'C'},
			{"module"      , 1 , 0 , 'M'},
			{"pcivendor"   , 0 , 0 , 'P'},
			{"ask"         , 1 , 0 , 'A'},
			{"xbox"        , 0 , 0 , 'x'},
			{"profile"     , 0 , 0 , 'p'},
			{"remove"      , 0 , 0 , 'r'},
			{"nocheckflag" , 0 , 0 , 'f'},
			{"info"        , 0 , 0 , 'i'},
			{"needquestion", 0 , 0 , 'n'},
			{"help"        , 0 , 0 , 'h'},
			{0             , 0 , 0 , 0  }
		};

		c = getopt_long (
			argc, argv, "s:q:cC:M:A:prihxnPfD",long_options, &option_index
		);
		if (c == -1)
		break;

		switch (c) {
		case 0:                             // should not happen...
		break;

		case 'h':                           // Show the usage message
			usage();                        // and exit sysp

		case 's':                           // Scanning stuff needs a root
			task = SCANNER;                 // check in front of the 
			strcpy(themodule,optarg);       // ScanModule call
		break;

		case 'f':                           // Do not call vendor.pl in 3D
			checkFlag = 0;                  // scanning
		break;

		case 'P':                           // use Vendor ID only to identify
			CheckPCIVendor = 1;             // driver for unknown cards
		break;

		case 'n':                           // during server scan
			CheckQuestionOnly = 1;          // check for 3D question only
		break;

		case 'r':                           // Removing registry files
			CheckRoot();                    // needs root permissions
			RemoveRegistry();
		break;

		case 'i':                           // Set info status flag
			InfoOnly = 1;                   // used only in ScanModule
		break;
 
		case 'M':                           // Set module to use for scan
			ModuleToUse = optarg;           // used only in ScanModule 
		break;
   
		case 'A':                           // Set Ask Flag for scan
			if (strcmp(optarg,"no") == 0) { // used only in ScanModule
				AskForFlag = 2;             // answer question with no
			} else {
				AskForFlag = 3;             // answer question with yes
			}
		break;

		case 'x':                           // If there is a question use
			UseXForQuestions = 1;           // a Qt message box to display it
		break;

		case 'C':                           // Set chip numbers to use 
			CardToUse = optarg;             // for the scan. used only 
		break;                              // ScanModule

		case 'q':                           // Query existing module
			task = QUERIES;                 // database
			strcpy(themodule,optarg);
		break;
  
		case 'c':                           // Show current detected cards
			ShowCards();                    // do not probe for primary device
		break;

		case 'p':                           // show profiles needed for chip X 
			task = PROFILE;                 // may cause a server probing
		break;

		case 'D':                           // show media devices CD / DVD
			task = MEDIADEVICES;            // using libhd to obtain that
		break;

		default:
			usage();
		}
	}
	// ...
	// run the task
	// ---
	switch (task) {
	case SCANNER :
		CheckRoot();
		ScanModule(themodule);
		SyncOnDisk();
	break;

	case QUERIES :
		QueryModule(themodule);
	break;

	case PROFILE :
		CheckRoot();
		ShowProfile();
	break;

	case MEDIADEVICES :
		ShowMediaDevices();
	break;
	} 
	return(0);
}

//============================================
// sync on disk...
//--------------------------------------------
void SyncOnDisk(void) {
	sync();sync();sync();
}

//============================================
// scan module...
//--------------------------------------------
void ScanModule(str name) {
	string module = name;
	// ...
	// Mouse scan
	// ---
	if (module == "mouse") {
		ScanMouse mouse;
		mouse.SetFile(MOUSE_DATA);
		mouse.Scan(); PrintMouseData(mouse);
		if (! InfoOnly) {
			mouse.Save();
		}
		exit(0);
	}
 	// ...
	// Keyboard scan
	// ---
	if (module == "keyboard") {
		ScanKeyboard key;
		key.SetFile(KEYBOARD_DATA);
		key.Scan(); PrintKeyboardData(key);
		if (! InfoOnly) {
			key.Save();
		}
		exit(0);
	}
	// ...
	// Server scan
	// ---
	if (module == "server") {
		ScanServer server;
		str module ; strcpy(module,ModuleToUse.c_str());
		str card   ; strcpy(card,CardToUse.c_str());
		server.SetFile(SERVER_DATA);
		server.SetQuestion(AskForFlag);
		if (CheckQuestionOnly == 1) {
			server.CheckForQuestionOnly();
		}
		server.SetCheckForPCIVendor (CheckPCIVendor);
		server.SetXAvailable(UseXForQuestions);
		server.SetCardModule(module);
		server.SetCard(card);
		server.Scan(); PrintServerData(server);
		if (! InfoOnly) {
			server.Save();
		}
		exit(0);
	}
	// ...
	// XStuff scan
	// ---
	if (module == "xstuff") {
		str module ; strcpy(module,ModuleToUse.c_str());
		str card   ; strcpy(card,CardToUse.c_str());
		ScanXStuff stuff (AskForFlag,UseXForQuestions,card,module);
		stuff.SetFile(STUFF_DATA);
		stuff.Scan(); PrintStuffData(stuff);
		if (! InfoOnly) {
			stuff.Save();
		}
		exit(0);
	}
	// ...
	// 3D scan
	// ---
	if (module == "3d") {
		str module ; strcpy(module,ModuleToUse.c_str());
		str card   ; strcpy(card,CardToUse.c_str());
		Scan3D delta (ANSWER_3D_WITH_YES,UseXForQuestions,card,module);
		delta.SetFile(D3_DATA);
		delta.SetCheck (checkFlag);
		delta.Scan(); Print3DData(delta);
		if (! InfoOnly) {
			delta.Save();
		}
		exit(0); 
	}
	cout << "sysp: no module for: " << module << endl;
	exit(1);  
}

//============================================
// query module...
//--------------------------------------------
void QueryModule(str name) {
	string module = name;
	// ...
	// Mouse query
	// ---
	if (module == "mouse") {
		ScanMouse mouse;
		mouse.SetFile(MOUSE_DATA);
		if (mouse.Read() == 0) {
			PrintMouseData(mouse); exit(0);
		}
		cout << "sysp: no mouse information available... ";
		cout << "scan first" << endl;
		exit(1);
	}
	// ...
	// Keyboard query
	// ---
	if (module == "keyboard") {
		ScanKeyboard key;
		key.SetFile(KEYBOARD_DATA);
		if (key.Read() == 0) {
			PrintKeyboardData(key); exit(0);
		}
		cout << "sysp: no keyboard information available... ";
		cout << "scan first" << endl;
		exit(1);
	}
	// ...
	// Server query
	// ---
	if (module == "server") {
		ScanServer server;
		server.SetFile(SERVER_DATA);
		if (server.Read() == 0) {
			PrintServerData(server); exit(0);
		}
		cout << "sysp: no server information available... ";
		cout << "scan first" << endl;
		exit(1);
	}
	// ...
	// XStuff query
	// ---
	if (module == "xstuff") {
		ScanXStuff stuff;
		stuff.SetFile(STUFF_DATA);
		if (stuff.Read() == 0) {
			PrintStuffData(stuff); exit(0);
		}
		cout << "sysp: no xstuff information available... ";
		cout << "scan first" << endl;
		exit(1);
	}
	// ...
	// 3D query
	// ---
	if (module == "3d") {
		Scan3D delta;
		delta.SetFile(D3_DATA);
		if (delta.Read() == 0) {
			Print3DData(delta); exit(0);
		}
		cout << "sysp: no 3D information available... ";
		cout << "scan first" << endl;
		exit(1);
	}
	cout << "sysp: no module for: " << module << endl;
	exit(1);
}

//============================================
// remove all registry files...
//--------------------------------------------
void RemoveRegistry(void) {
	unlink(MOUSE_DATA);
	unlink(KEYBOARD_DATA);
	unlink(SERVER_DATA);
	unlink(SERVER_STUFF_DATA);
	unlink(STUFF_DATA);
	unlink(D3_DATA);
}

//============================================
// show the detected cards...
//--------------------------------------------
void ShowCards(void) {
	ScanServer server;
	int chip = 0;
	server.NoPrimaryCheck();
	server.SetCheckForPCIVendor (CheckPCIVendor);
	server.Scan();

	for (int i = server.Count(); i > 0; i--) {
		ServerData data = server.Pop();
		string name = data.vendor + " " + data.device;
		printf("Chip: %-2d is -> %-32s ",chip,name.c_str()); 
		printf("%02d:%02d:%01d ",data.bus,data.slot,data.func);
		printf("0x%04x 0x%04x ",data.vid,data.did);
		printf("%3s %s\n",data.bustype.c_str(),data.module.c_str());
		chip++;
	}
}

//============================================
// show the detected CD / DVD devices...
//--------------------------------------------
void ShowMediaDevices (void) {
	if (char* media = mediaDevices()) {
		printf ("%s\n",media);
	}
}

//============================================
// show the detected profiles...
//--------------------------------------------
void ShowProfile(void) {
	ScanServer server;
	server.SetFile(SERVER_DATA);
	str module ; strcpy(module,ModuleToUse.c_str());
	str card   ; strcpy(card,CardToUse.c_str());
	int chip = 0;
 
	if (server.Read() < 0) {
		server.SetCardModule(module);
		server.SetCard(card);
		server.SetQuestion(AskForFlag);
		server.SetXAvailable(UseXForQuestions);
		server.SetCheckForPCIVendor (CheckPCIVendor);
		server.Scan();
	}

	for (int i = server.Count(); i > 0; i--) {
		int plusDevice = 0;
		ServerData data = server.Pop();
		string profile = data.profile;
		if (profile == "") {
			profile = "unknown";
		} else {
			string file = PROFILEDIR;
			file += profile;
			string deviceCount = qx (
				PSECTION,STDOUT,1,"%s",file.c_str()
			);
			plusDevice = atoi (deviceCount.c_str());
		}
		printf("Chip:%d:%s\n",chip,profile.c_str());
		if (plusDevice) {
			chip = chip + plusDevice;
		}
		chip++;
	}
}

//============================================
// check for root priviliges...
//--------------------------------------------
void CheckRoot(void) {
	if (geteuid()) {
		cout << "sysp: only root can do this";
		cout << endl; 
		exit(1);
	} 
}

//============================================
// GetProfileDriver...
//--------------------------------------------
char* GetProfileDriver ( string profile ) {
	string profileFile = PDIR + string(profile);
	string profileDriver = qx (
		PDRIVER,STDOUT,1,"%s",profileFile.c_str()
	);
	return (char*)profileDriver.c_str();
}

//============================================
// print out mouse data...
//--------------------------------------------
void PrintMouseData(ScanMouse m) {
	MouseData data;
	int mouse = 0;
	int devices[m.Count()];
	if (m.Count() == 1) {
		//============================================
		// only one device... use it
		//--------------------------------------------
		devices[0] = 1;
	} else {
		//============================================
		// initialize and sort out serial devices
		//--------------------------------------------
		for (int i = m.Count()-1; i >= 0; i--) {
			data = m.Pop();
			devices[i] = 0;
			if ( (strstr(data.device,"ttyS")) != NULL ) {
				devices[i] = 1;
			}
		}
		m.Reset();
		//============================================
		// sort out special profiled pointers
		//--------------------------------------------
		// ...
		// these are devices which use another driver than the mouse
		// driver and therefore always needs a seperate InputDevice
		// section
		// ---
		for (int i = m.Count()-1; i >= 0; i--) {
			data = m.Pop();
			if (devices[i] == 0) {
				if (strcmp(data.driver,"mouse") != 0) {
					devices[i] = 1;
				}
			}
		}
		m.Reset();
		//============================================
		// sort out best match from the rest
		//--------------------------------------------
		int bestMatch = -1;
		for (int i = m.Count()-1; i >= 0; i--) {
			data = m.Pop();
			// ...
			// sort out device which has a profile set except those
			// profiles which use the standard mouse driver
			// ---
			if (devices[i] == 0) {
				if (bestMatch == -1) {
					bestMatch = i;
				}
				if (strcmp(data.profile,"<undefined>") != 0) {
					bestMatch = i;
					break;
				}
			}
		}
		if (bestMatch != -1) {
			devices[bestMatch] = 1;
		}
		m.Reset();
	}
	//============================================
 	// print all devices...
	//--------------------------------------------
	bool haveStandardMouse = false;
	for (int i = m.Count()-1; i >= 0; i--) {
		// ...
		// check for the standard mouse which uses the mouse driver
		// If there is no such mouse left we will add a default
		// section at the end
		// ---
		data = m.Pop();
		if (strcmp(data.driver,"mouse") == 0) {
			haveStandardMouse = true;
		}
		if (strcmp(data.driver,"vmmouse") == 0) {
			haveStandardMouse = true;
		}
		bool deviceIsANutShell = false;
		if (devices[i] != 1) {
			deviceIsANutShell = true;
			//continue;
		}
		if (mouse > 0) {
			printf("\n");
		}
		printf("Mouse%d    =>  Protocol   : %s\n",mouse,data.protocol);
		printf("Mouse%d    =>  Device     : %s\n",mouse,data.device);
		printf("Mouse%d    =>  Buttons    : %d\n",mouse,data.buttons);
		printf("Mouse%d    =>  Wheel      : %d\n",mouse,data.wheel);
		printf("Mouse%d    =>  Emulate    : %d\n",mouse,data.emulate);
		printf("Mouse%d    =>  Name       : %s\n",mouse,data.name);
		printf("Mouse%d    =>  VendorID   : %s\n",mouse,data.vid);
		printf("Mouse%d    =>  DeviceID   : %s\n",mouse,data.did);
		printf("Mouse%d    =>  Profile    : %s\n",mouse,data.profile);
		printf("Mouse%d    =>  RealDevice : %s\n",mouse,data.realdev);
		if (deviceIsANutShell) {
			printf("Mouse%d    =>  NutShell   : 1\n",mouse);
		} else {
			printf("Mouse%d    =>  NutShell   : 0\n",mouse);
		}
		mouse++;
	}
	if (! haveStandardMouse) {
		int m = mouse;
		printf("\n");
		printf("Mouse%d    =>  Protocol   : explorerps/2\n",m);
		printf("Mouse%d    =>  Device     : /dev/input/mice\n",m);
		printf("Mouse%d    =>  Buttons    : 5\n",m);
		printf("Mouse%d    =>  Wheel      : 1\n",m);
		printf("Mouse%d    =>  Emulate    : 0\n",m);
		printf("Mouse%d    =>  Name       : ImPS/2 Generic Wheel Mouse\n",m);
		printf("Mouse%d    =>  VendorID   : 0x0210\n",m);
		printf("Mouse%d    =>  DeviceID   : 0x0013\n",m);
		printf("Mouse%d    =>  Profile    : <undefined>\n",m);
		printf("Mouse%d    =>  RealDevice : <undefined>\n",m);
		printf("Mouse%d    =>  NutShell   : 0\n",m);
	}
	fflush(stdout);
}

//============================================
// print out keyboard data...
//--------------------------------------------
void PrintKeyboardData(ScanKeyboard k) {
	Keymap data;
	int kbd = 0;

	for (int i = k.Count()-1; i >= 0; i--) { 
		data = k.Pop();
		if (kbd > 0) {                                             
			printf("\n");                                             
		}
		printf("Keyboard%d =>  XkbModel   : %s\n",kbd,data.model.c_str());
		printf("Keyboard%d =>  XkbLayout  : %s\n",kbd,data.layout.c_str());
		if (data.variant != "x") {
			printf("Keyboard%d =>  XkbVariant : %s\n",kbd,data.variant.c_str());
		}
		if (data.options != "x") {
		printf("Keyboard%d =>  XkbOptions : %s\n",kbd,data.options.c_str());
		}
		if (data.keycodes != "xfree86") {
		printf("Keyboard%d =>  XkbKeyCodes: %s\n",kbd,data.keycodes.c_str()); 
		}
		if (data.leftalt != "x")    {
		printf("Keyboard%d =>  LeftAlt    : %s\n",kbd,data.leftalt.c_str());
		}
		if (data.rightalt != "x")   {
		printf("Keyboard%d =>  RightAlt   : %s\n",kbd,data.rightalt.c_str());
		}
		if (data.scrollock != "x")  {
		printf("Keyboard%d =>  ScrollLock : %s\n",kbd,data.scrollock.c_str());
		}
		if (data.rightctl != "x")   {
		printf("Keyboard%d =>  RightCtl   : %s\n",kbd,data.rightctl.c_str());
		}
		printf("Keyboard%d =>  Name       : %s\n",kbd,data.name.c_str());
		printf("Keyboard%d =>  VendorID   : %s\n",kbd,data.vid.c_str());
		printf("Keyboard%d =>  DeviceID   : %s\n",kbd,data.did.c_str());
		printf("Keyboard%d =>  Profile    : %s\n",kbd,data.profile.c_str());
		printf("Keyboard%d =>  RealDevice : %s\n",kbd,data.device.c_str());
		kbd++;
	}
	fflush(stdout);
}

//============================================
// print out server data...
//--------------------------------------------
void PrintServerData(ScanServer s) {
	ServerData data;
	int card = 0;

	for (int i = s.Count(); i > 0; i--) {
		data = s.Pop();
		if (card > 0) {
			printf("\n");
		}
		printf("Card%d     =>  DomainId   : 0x%01x\n",card,data.domain);
		printf("Card%d     =>  BusId      : 0x%01x\n",card,data.bus);
		printf("Card%d     =>  SlotId     : 0x%02x\n",card,data.slot);
		printf("Card%d     =>  FuncId     : 0x%01x\n",card,data.func);
		printf("Card%d     =>  Vendor     : %s\n"   ,card,data.vendor.c_str()); 
		printf("Card%d     =>  Device     : %s\n"   ,card,data.device.c_str());
		printf("Card%d     =>  VID        : 0x%04x\n",card,data.vid);
		printf("Card%d     =>  DID        : 0x%04x\n",card,data.did);
		printf("Card%d     =>  Module     : %s\n"    ,card,data.module.c_str());
		printf("Card%d     =>  BusType    : %s\n"   ,card,data.bustype.c_str());
		printf("Card%d     =>  Detected   : %d\n"    ,card,data.detected);
		printf("Card%d     =>  Flag       : %s\n"    ,card,data.flag.c_str());
		printf("Card%d     =>  SUB-VID    : 0x%04x\n",card,data.subvendor);
		printf("Card%d     =>  SUB-DID    : 0x%04x\n",card,data.subdevice);
		printf("Card%d     =>  DrvProfile : %s\n",card,data.drvprofile.c_str());

		card++;
	}
	fflush(stdout);
}

//============================================
// print out stuff data...
//--------------------------------------------
void PrintStuffData(ScanXStuff s) {
	StuffData data;
	int card = 0;
	for (int i = s.Count(); i > 0; i--) {
		data = s.Pop();
		if (card > 0) {
			printf("\n");
		}
		if (data.raw == "") { 
			data.raw = "None"; 
		}
		if (data.option == "") { 
			data.option = "None"; 
		}
		if (data.extension == "") { 
			data.extension = "None"; 
		}
		printf ("Card%d     =>  DDC        : %s\n",card,data.ddc[0].c_str());
		if (data.port > 1) {
		printf ("Card%d     =>  DDC[2]     : %s\n",card,data.ddc[1].c_str());
		}
		printf ("Card%d     =>  Name       : %s\n",card,data.model[0].c_str());
		printf ("Card%d     =>  Vendor     : %s\n",card,data.vendor[0].c_str());
		if (data.port > 1) {
		printf ("Card%d     =>  Name[2]    : %s\n",card,data.model[1].c_str());
        printf ("Card%d     =>  Vendor[2]  : %s\n",card,data.vendor[1].c_str());
		}
		printf ("Card%d     =>  Primary    : %s\n",card,data.primary.c_str());
		printf ("Card%d     =>  Chipset    : %s\n",card,data.chipset.c_str());
		printf ("Card%d     =>  Vsync      : %d\n",card,data.vsync[0]);
		printf ("Card%d     =>  Hsync      : %d\n",card,data.hsync[0]);
		if (data.port > 1) {
		printf ("Card%d     =>  Vsync[2]   : %d\n",card,data.vsync[1]);
		printf ("Card%d     =>  Hsync[2]   : %d\n",card,data.hsync[1]);
		}
		if (data.vesacount[0] > 0) {
		for (int n=0;n<data.vesacount[0];n++) {
			if (data.vesa[0][n].x > 0) {
				printf ("Card%d     =>  Vesa       : %d %d %d %d\n",card,
				data.vesa[0][n].x,data.vesa[0][n].y,
				data.vesa[0][n].hsync,data.vesa[0][n].vsync);
			}
		}
		}
		if (data.port > 1) {
		if (data.vesacount[1] > 0) {
		for (int n=0;n<data.vesacount[1];n++) {
			if (data.vesa[1][n].x > 0) {
				printf ("Card%d     =>  Vesa[2]    : %d %d %d %d\n",card,
				data.vesa[1][n].x,data.vesa[1][n].y,
				data.vesa[1][n].hsync,data.vesa[1][n].vsync);
			}
		}
		}
		}
		if (TvSupport()) {
			printf ("Card%d     =>  Vesa       : 768 576 36 60\n",card);
		}
		if (data.fbtiming != "") {
			printf ("Card%d     =>  FbTiming   : %s\n",card,
			data.fbtiming.c_str()); 
		} 
		printf ("Card%d     =>  Dacspeed   : %d\n",card,data.dacspeed[0]);
		if (data.port > 1) {
		printf ("Card%d     =>  Dacspeed[2]: %d\n",card,data.dacspeed[1]);
		}
		printf ("Card%d     =>  Modeline   : %s\n",
			card,data.modeline[0].c_str()
		);
		if (data.port > 1) {
		printf ("Card%d     =>  Modeline[2]: %s\n",
			card,data.modeline[1].c_str()
		);
		}
		printf ("Card%d     =>  Memory     : %ld\n",card,data.videoram);
		printf ("Card%d     =>  RandR      : %s\n",card,data.rroutput.c_str());
		printf ("Card%d     =>  Current    : %s\n",card,data.current.c_str());
		printf ("Card%d     =>  RawDef     : %s\n",card,data.raw.c_str());
		printf ("Card%d     =>  Option     : %s\n",card,data.option.c_str());
		printf ("Card%d     =>  Extension  : %s\n",card,data.extension.c_str());
		printf ("Card%d     =>  Module     : %s\n",card,data.driver.c_str());
		if (
			(data.dtype[0] == "CRT")     || 
			(data.dtype[0] == "LCD/TFT")
		) {
		printf ("Card%d     =>  Display    : %s\n",card,data.dtype[0].c_str());
		} else {
		printf ("Card%d     =>  Display    : NUL\n",card);
		}
		if (data.port > 1) {
		if (
			(data.dtype[1] == "CRT")     ||
			(data.dtype[1] == "LCD/TFT")
		) {
		printf ("Card%d     =>  Display[2] : %s\n",card,data.dtype[1].c_str());
		} else {
		printf ("Card%d     =>  Display[2] : NUL\n",card);
		}
		}
		if ((data.dpix[0] > 0) && (data.dpiy[0] > 0)) {
		printf ("Card%d     =>  Size       : %dx%d\n",
			card,data.dpix[0],data.dpiy[0]
		);
		}
		if (data.port > 1) {
		if ((data.dpix[1] > 0) && (data.dpiy[1] > 0)) {
		printf ("Card%d     =>  Size[2]    : %dx%d\n",
			card,data.dpix[1],data.dpiy[1]
		);
		}
		}
		if (data.vmdepth > 0) {
		printf ("Card%d     =>  ColorDepth : %d\n",
			card,data.vmdepth
		);
		}
		printf ("Card%d     =>  VesaBios   : %s\n",card,data.vbios.c_str());
		if (data.fbmodecount > 0) {
		for (int n=0;n<data.fbmodecount;n++) {
			if (data.boot[n].x > 0) {
				printf ("Card%d     =>  FBBoot     : %d %d %d %d\n",card,
				data.boot[n].x,data.boot[n].y,
				data.boot[n].depth,data.boot[n].mode);
			}
		}
		}
		card++;
	}
	fflush(stdout);
}

//============================================
// print out 3D data...
//--------------------------------------------
void Print3DData (Scan3D d) {
	D3Data data;
	data = d.Pop();
	printf ("Card3D0   =>  Install    : %s\n",data.install);
	printf ("Card3D0   =>  Remove     : %s\n",data.remove);
	printf ("Card3D0   =>  Packages   : %s\n",data.packs);
	printf ("Card3D0   =>  Active     : %d\n",data.active);
	printf ("Card3D0   =>  Answer     : %s\n",data.answer);
	printf ("Card3D0   =>  ScriptReal : %s\n",data.script_real);
	printf ("Card3D0   =>  ScriptSoft : %s\n",data.script_soft);
	printf ("Card3D0   =>  Flag       : %s\n",data.specialflag);
	fflush (stdout);
}

//============================================
// usage...
//--------------------------------------------
void usage (void) {
	cout << "Linux System Profiler Version 8.1 (2005-03-08)"        << endl;
	cout << "(C) Copyright 2001 SuSE GmbH"                          << endl;
	cout << endl;
	cout << "usage: sysp [ options ]"                               << endl;
	cout << "options:"                                              << endl;
	cout << " [ -s | --scan <ModuleName> ]"                         << endl;
	cout << "    Start the auto detection process for the"          << endl; 
	cout << "    given module."                                     << endl;
	cout << endl;
	cout << " [ -q | --query <ModuleName>"                          << endl;
	cout << "    Query the detection database to obtain"            << endl;
	cout << "    information about the given module"                << endl;
	cout << endl;
	cout << " [ -r | --remove ]"                                    << endl;
	cout << "    remove database"                                   << endl;
	cout << endl;
	cout << "special:"                                              << endl;
	cout << "scan options..."                                       << endl;
	cout << " [ -i | --info ]"                                      << endl;
	cout << "    add this option to your scan and you will"         << endl;
	cout << "    only be informed about the result but it is"       << endl;
	cout << "    written to the database"                           << endl;
	cout << endl;
	cout << " [ -P || --pcivendor ]"                                << endl;
	cout << "    used with server scan only. If the card is not"    << endl;
	cout << "    part of the Identity.map you can use this option"  << endl;
	cout << "    to assign a driver according to the Vendor-ID of"  << endl;
	cout << "    the card"                                          << endl;
	cout << endl;
	cout << " [ -x | --xbox ]"                                      << endl;
	cout << "    Use X11 for any questions instead of"              << endl;
	cout << "    the console"                                       << endl;
	cout << endl;
	cout << " [ -n | --needquestion ]"                              << endl;
	cout << "    for --scan server only; check if we need to anser" << endl;
	cout << "    a question if yes exit with (1) otherwhise (0)"    << endl;
	cout << endl;
	cout << " [ -A | --ask yes|no ]"                                << endl;
	cout << "    give order how to answer questions during scan"    << endl;
	cout << "    set yes to answer with yes and set no to answer"   << endl;
	cout << "    with no"                                           << endl; 
	cout << " [ -C | --chip ]"                                      << endl; 
	cout << "    add a comma seperated list of chip numbers"        << endl;
	cout << "    you want to scan. For example: 0,1"                << endl; 
	cout << " [ -M | --module ]"                                    << endl;
	cout << "    add a comma seperated list of expressions to"      << endl; 
	cout << "    set a driver modul manually. The expression"       << endl;
	cout << "    contain the chip number and the assigned module"   << endl;
	cout << "    for example: 0=mga,1=nv"                           << endl;
	cout << "    Note: The chip number you set here is assigned"    << endl;  
	cout << "    to the card shown in the --cards output"           << endl;
	cout << endl;
	cout << "get card info..."                                      << endl;
	cout << " [ -c | --cards ]"                                     << endl;
	cout << "    obtain information about the installed"            << endl;
	cout << "    cards. This is a short and fast output with"       << endl;
	cout << "    server probing"                                    << endl;
	cout << endl;
	exit(0);
}
