/**************
FILE          : cfg.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the config file class
              : to start and parse a probeonly server 
              : 
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
#include <sys/types.h>
#include <signal.h>
#include <algorithm>
#include <list>
#include <math.h>

using namespace std;

#include "lib/syslib.h"
#include "sysp.h"

//========================================
// Class Headers
//----------------------------------------
class XF86ConfigFile {
	protected:
	string mouseprotocol;
	string mousedevice;
	string deviceopt;
	string busid;
	map <int,int> bus;
	map <int,int> slot;
	map <int,int> func;
	string driver;
	int memory;
	int colordepth;
	int id;
	map <int,ParseData> qP;
	int elements;
	int current;
	string file;

	private:
	int idc;

	public:
	void SetMouseProperties (str prot,str dev);
	void SetColorDepth (int depth);
	void SetBus (int d,int b,int s,int f);
	int  SetDriver (string module);
	void SetVideoRam (int mem);
	void CallXF86Loader (str file="/etc/X11/xorg.conf");
	void ShutdownServer (int spid, int disp);
	string CallRandR (str file="/etc/X11/xorg.conf");
	ParseData Pop (void);
	void Push (ParseData e);
	XF86ConfigFile (void);
	void Reset (void);
	int  Count (void) { return elements; }
	void SetSectionID (int nr);
	void SetFile (const str name);
	void SetDeviceOption (string opt);
	int  Save (void);
	int  Read (void);
  
	public:
	string DoFilesSection (void);
	string DoModuleSection (void);
	string DoInputDeviceSection (void);
	string DoMonitorSection (void);
	string DoScreenSection (void);
	string DoDeviceSection (void);
	string DoServerLayoutSection (void);
	string DoServerFlagsSection (void);

	public:
	class ReadLastElement : public::exception {
		public:
		virtual const char* what() const throw() {
			return "read last element...";
		}
	};
};

