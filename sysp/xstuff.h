/**************
FILE          : xstuff.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the server Stuff detection  
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
#include <list>

using namespace std;

#include "lib/syslib.h"
#include "sysp.h"

//==========================================
// Class Headers
//------------------------------------------
class ScanXStuff {
	protected:
	map<int,StuffData> qX;
	string file;
	int elements;
	int current;
	int question;
	int withx;
	str card;
	str cardopt;

	public:
	void SetFile (const str name);
	void Push (StuffData e);
	void Reset (void);
	void Scan (void);
	StuffData Pop (void);
	int  Count () { return elements; }
	int  Save (void);
	int  Read (void);
	ScanXStuff (void);
	ScanXStuff (int ask,int xavailable,str cardnr,str copt);

	private:
	int FindParseData (map<int,ParseData> m,int bus,int slot,int func);
	char* createProbeonlyConfig (XF86ConfigFile*,int,map<int,ServerData>);
  
	public:
	class ReadLastElement : public::exception {
		public:
		virtual const char* what() const throw() {
			return "read last element...";
		}
	};
};
