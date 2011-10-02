/**************
FILE          : mouse.cc
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : mouse detection code 
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mouse.h"
#include "config.h"

//======================================
// ScanMouse: constructor...
//--------------------------------------
ScanMouse::ScanMouse(void) {
	elements = 0;
	current  = 0;
}

//======================================
// ScanMouse: set file name to save
//--------------------------------------
void ScanMouse::SetFile(const str name) {
	file = name;
}

//======================================
// ScanMouse: push MouseData to map
//--------------------------------------
void ScanMouse::Push (MouseData e) {
	qM[elements] = e;
	elements++;
}

//======================================
// ScanMouse: read MouseData from map
//--------------------------------------
MouseData ScanMouse::Pop (void) {
	if (current >= elements) {
		throw ReadLastElement();
	}
	MouseData element(qM[current]);
	current++; 
	return element;
}

//======================================
// ScanMouse: reset map counter...
//--------------------------------------
void ScanMouse::Reset (void) {
	current = 0;
}

//======================================
// ScanMouse: hw scan of the mice
//--------------------------------------
void ScanMouse::Scan (void) {
	mp = MouseGetData();
	SPReadFile<Input> rcinput;
	rcinput.SetFile(INPUT_MAP);
	rcinput.ImportInputMap();
	Input input;

	// ...
	// assign profile to input device using Input.map
	// ---
	for (MouseData* lp=mp; lp; lp=lp->next) {
		strcpy (lp->profile,"<undefined>");
		strcpy (lp->driver,"mouse");
		for (int i = rcinput.Count(); i > 0; i--) {
			input = rcinput.Pop();
			if ((input.did == lp->did) && (input.vid == lp->vid)) {
				strcpy (lp->profile,input.profile.c_str());
				strcpy (lp->driver,input.driver.c_str());
				break;
			}
		}
		rcinput.Reset();
		Push(*lp);
	}
	// ...
	// if no mouse could be detected we will add the generic
	// AUTO protocol entry
	// ---
	if (mp == NULL) {
		MouseData* define = NULL;
		define = (MouseData*)malloc(sizeof(MouseData));
		strcpy(define->protocol,"Auto");
		strcpy(define->device  ,"/dev/input/mice");
		define->emulate = 1;
		define->buttons = 2;
		define->wheel   = 0;
		strcpy(define->did,"0");
		strcpy(define->vid,"0");
		strcpy(define->profile,"<undefined>");
		strcpy(define->name,"<undefined>");
		strcpy(define->realdev,"<undefined>");
		Push (*define);
	}
}

//======================================
// ScanMouse: serialize data to file
//--------------------------------------
int ScanMouse::Save (void) {
	MouseData data;
	ofstream handle(file.c_str(),ios::binary);
	if (! handle) {
		cout << "ScanMouse: could not create file: ";
		cout << file << endl;
		return(-1);
	}
	Reset();
	for (int i = Count(); i > 0; i--) {
		data = Pop();
		handle.write((char*)&data,sizeof(data));
	}
	handle.close();
	return(0); 
}

//======================================
// ScanMouse: retrieve data from file
//--------------------------------------
int ScanMouse::Read (void) {
	MouseData data;
	ifstream handle(file.c_str(),ios::binary);
	if (! handle) {
		//cout << "ScanMouse: could not open file: "
		//cout << file << endl;
		return(-1);
	}
	elements = 0;
	current  = 0;
	qM.clear();

	while(1) {
		handle.read((char*)&data,sizeof(data));
		if (handle.eof()) {
			break;
		}
		Push(data);
	}
	handle.close();
	return(0);
}
