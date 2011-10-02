/**************
FILE          : d3.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the 3D package and 
              : environment detection  
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

//=====================================
// Class Headers
//-------------------------------------
class Scan3D {
	protected:
	D3Data data;
	int question;
	int withx;
	int checkflag;
	string file;
	str card;
	str cardopt;

	public:
	Scan3D (void);
	Scan3D (int ask,int xavailable,str cardnr,str copt);
	void SetFile  (const str name);
	void SetCheck (int);
	void Scan  (void);
	int  Save  (void);
	int  Read  (void);
	D3Data Pop (void);
	int Installed(str pac);
	int Installed(string pac);
};

