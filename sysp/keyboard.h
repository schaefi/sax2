/**************
FILE          : keyboard.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the keyboard detection  
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
#include <algorithm>

using namespace std;

#include "lib/syslib.h"
#include "sysp.h"

//=====================================
// Class Headers
//-------------------------------------
class ScanKeyboard {
	protected:
	map<int,Keymap> qK;
	string file;
	int elements;
	int current;
 
	public:
	void SetFile (const str name);
	void Push (Keymap e);
	void Reset (void);
	void Scan (void);
	Keymap Pop (void);
	int  Count () { return elements; }
	int  Save (void);
	int  Read (void);
	ScanKeyboard (void);

	public:
	class ReadLastElement : public::exception {
		public:
		virtual const char* what() const throw() {
			return "read last element...";
		}
	};
};
