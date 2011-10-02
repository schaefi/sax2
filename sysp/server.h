/**************
FILE          : server.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : header file for the server PCI detection  
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
#include <stdarg.h>
#include <dirent.h>

using namespace std;

#include "lib/syslib.h"
#include "sysp.h"

//=====================================
// Class Headers
//-------------------------------------
class ScanServer {
	protected:
	ServerData* sp;
	map<int,ServerData> qS;
	string file;
	int ask;
	int withx;
	list<int> scanme;
	map<int,string> moduleme;
	int elements;
	int current;
	int pcheck;
	int checkQuestion;
	int checkPCIVendor;

	public:
	void SetFile (const str name);
	void Push (ServerData e);
	void Reset (void);
	void Scan (void);
	ServerData Pop (void);
	int  Count () { return elements; }
	int  Save (void);
	int  Read (void);
	ScanServer (void);
	ScanServer (int question,int xavailable,str card,str copt);
	void SetQuestion(int question=-1);
	void CheckForQuestionOnly (void);
	void SetCheckForPCIVendor (int);
	void SetXAvailable(int available=-1);
	void SetCard(str card="all");
	void SetCardModule(str copt);
	void NoPrimaryCheck(void);
	int  haveFBdev (void);
	int  haveVesaBIOS (void);

	private:
	int IdentifyDevice(
		IdentMap m,int vid,int did,int svid,int sdid,
		int cards,int ask=-1,int cardNr=0
	);

	public:
	class ReadLastElement : public::exception {
		public:
		virtual const char* what() const throw() {
			return "read last element...";
		}
	};
};
