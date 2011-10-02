/**************
FILE          : sysp.h
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
STATUS        : Status: Up-to-date
**************/

#ifndef SYSP_H

//===================================
// Defines...
//-----------------------------------
#define MAX_LINE_SIZE 1024
#define SYSP_H        1

#define ANSWER_3D_WITH_YES  3

//===================================
// Structures...
//-----------------------------------
struct Identity {
	string name;
	string device;
	string vid;
	string did;
	string server;
	string extension;
	string option;
	string raw;
	string profile;
	string script3d;
	string package3d;
	string flag;
	string svid;
	string sdid;
};

struct Keymap {
	string consolename;
	string model;
	string layout;
	string variant;
	string options;
	string keycodes;
	string leftalt;
	string rightalt;
	string scrollock;
	string rightctl;
	string name;
	string device;
	string vid;
	string did;
	string profile;
};

struct Input {
	string vid;
	string did;
	string profile;
	string driver;
};

struct Driver {
	string driver;
	string profile;
};

struct Keymap_S {
	str consolename;
	str model;
	str layout;
	str variant;
	str options;
	str keycodes;
	str leftalt;
	str rightalt;
	str scrollock;
	str rightctl;
	str name;
	str device;
	str vid;
	str did;
	str profile;
};

struct ServerData {
	int domain;
	int cls;
	int bus;
	int slot;
	int func;
	string vendor;
	string device;
	int vid;
	int did;
	string module;
	string bustype;
	int detected;
	string flag;
	int subvendor;
	int subdevice;
	string profile;
	string package3d;
	string script3d;
	string raw;
	string option;
	string extension;
	string drvprofile;
};

struct ServerData_S {
	int domain;
	int cls;
	int bus;
	int slot;
	int func;
	str vendor;
	str device;
	int vid;
	int did;
	str module;
	str bustype;
	int detected;
	str flag;
	int subvendor;
	int subdevice;
	str profile;
	str package3d;
	str script3d;
	str raw;
	str option;
	str extension;
	str drvprofile;
};

struct XMode {
	int x;
	int y;
	int hsync;
	int vsync;
};

struct FBMode {
	int x;
	int y;
	int mode;
	int depth;
};

struct ParseData {
	string id;
	string clock;
	string ddc;
	string dtype;
	string model;
	string vendor;
	string primary;
	int pbus;
	int pslot;
	int pfunc;
	string chipset;
	int hsmax;
	int vsmax;
	int modecount;
	map <int,XMode> modes;
	unsigned dacspeed;
	long videoram;
	string rroutput;
	int bus;
	int slot;
	int func;
	int dpix;
	int dpiy;
	int vmdepth;
};

struct ParseData_S {
	str id;
	str clock;
	str ddc;
	str dtype;
	str primary;
	int pbus;
	int pslot;
	int pfunc;
	str chipset;
	int hsmax;
	int vsmax;
	unsigned dacspeed;
	long videoram;
	str rroutput;
	int modecount;
	int bus;
	int slot;
	int func;
	int dpix;
	int dpiy;
	int vmdepth;
};

struct StuffData {
	string ddc[4];
	string primary;
	string chipset;
	string dtype[4];
	string model[4];
	string vendor[4];
	string vbios;
	int hsync[4];
	int vsync[4];
	int vesacount[4];
	int fbmodecount;
	map <int,XMode> vesa[4];
	map <int,FBMode> boot;
	unsigned dacspeed[4];
	string modeline[4];
	long videoram; 
	string rroutput;
	string current;
	string raw;
	string option;
	string extension; 
	string driver;
	string fbtiming;
	int dpix[4];
	int dpiy[4];
	int vmdepth;
	int port;
};

struct StuffData_S {
	str ddc[4];
	str dtype[4];
	str model[4];
	str vendor[4];
	str vbios;
	str primary;
	str chipset;
	int hsync[4];
	int vsync[4];
	int vesacount[4];
	int fbmodecount;
	unsigned dacspeed[4];
	str modeline[4];
	long videoram;
	str rroutput;
	str current;
	str raw;
	str option;
	str extension;
	str driver;
	str fbtiming;
	int dpix[4];
	int dpiy[4];
	int vmdepth;
	int port;
};

struct D3Data {
	str install;
	str remove;
	str script_real;
	str script_soft;
	str packs;
	str answer;
	str specialflag;
	int active;
};

//===================================
// Types...
//-----------------------------------
typedef map<string,string>  RcData;
typedef map<int,Identity>   IdentMap;
typedef map<int,ServerData> ServerDataMap;

//===================================
// Classes...
//-----------------------------------
template <class T>
class SPReadFile {
	protected:
	string file;
	Identity ident;
	Keymap kbmap;
	map<int,T> qI;
	int count;

	public:
	void Push(T& e);
	T Pop(void);
	int Count() { return count; }
	void ImportIdentity (void);
	void ImportKeymap (void);
	void ImportInputMap (void);
	void ImportDriverMap (void);
	RcData ImportRcConfig (void);
	RcData ImportVendorMap (void);
	void SetFile (const str name);
	bool SetFile (const str sysdir, const str name);
	void Reset (void);
	SPReadFile(void);
	SPReadFile(str name);
	string Transform (string s, int direction);

	public:
	class ReadLastElement : public::exception {
		public:
		virtual const char* what() const throw() {
			return "read last element...";
		}
	};
};

//===================================
// template classes member functions
//-----------------------------------
//...//

//===================================
// SPReadFile: constructor
//-----------------------------------
template <class T>
SPReadFile<T>::SPReadFile(void) {
	count = 0;
}

//===================================
// SPReadFile: constructor
//-----------------------------------
template <class T>
SPReadFile<T>::SPReadFile(str name) {
	count = 0;
	file  = name;
}

//===================================
// SPReadFile: set file name to read
//-----------------------------------
template <class T>
void SPReadFile<T>::SetFile(const str name) {
	file = name;
}

//===================================
// SPReadFile: set file name to read
//-----------------------------------
template <class T>
bool SPReadFile<T>::SetFile(const str sysdir,const str name) {
	string directory (sysdir);
	string filename  (name);
	string sysconfigName = directory + filename;
	file = sysconfigName;

	ifstream handle(file.c_str());
	if (! handle) {
		return (false);
	}
	handle.close();
	return(true);
}

//===================================
// SPReadFile: reset map count
//-----------------------------------
template <class T>
void SPReadFile<T>::Reset(void) {
	count = qI.size();
}

//===================================
// SPReadFile: read /etc/rc.config
//-----------------------------------
template <class T>
RcData SPReadFile<T>::ImportRcConfig (void) {
	ifstream handle(file.c_str());
	RcData data;
	char *line;
	char *key;
 
	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		trim(line);
		if ((line[0] == '#') || (line[0] == 0)) {
			continue;
		}
		key  = strsep(&line,"=");
		if (line != NULL) {
			trim(key); trim(line);
			if (strcmp(line,"") != 0) {
				data[key] = line;
			}
		}
	}
	return(data);
}

//===================================
// SPReadFile: read Vendor.map
//-----------------------------------
template <class T>
RcData SPReadFile<T>::ImportVendorMap (void) {
	ifstream handle(file.c_str());
	RcData data;
	char *line;
	char *key;

	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		trim(line);
		if ((line[0] == '#') || (line[0] == 0)) {
			continue;
		}
		key  = strsep(&line,":");
		if (line != NULL) {
			trim(key); trim(line);
			if (strcmp(line,"") != 0) {
				data[key] = line;
			}
		}
	}
	return(data);
}

//===================================
// SPReadFile: read Identity
//-----------------------------------
template <class T>
void SPReadFile<T>::ImportIdentity(void) {
	ifstream handle(file.c_str());
	string data;
	char *line;
	char *token;
	char *key;

	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		Identity id;
		token = strtok(line,"&");

		id.svid = "0x0000";
		id.sdid = "0x0000";

		if (token != NULL) {
			key  = strsep(&token,"=");
			trim(key);
			data = key;
			if (data == "NAME") {
				trim(token); id.name = token;
			}
		}
		while (token) {
		token = strtok(NULL,"&");
		if (token != NULL) {
			key  = strsep(&token,"=");
			trim(key);
			data = token;
			if (strcmp(token,"") == 0) {
				continue;
			}
			data = key;
			if (data == "DEVICE")    {
				trim(token); id.device = token;
			}
			if (data == "VID")       {
				trim(token); id.vid = token;
				id.vid = Transform (id.vid,0);
			}
			if (data == "DID")       {
				trim(token); id.did = token;
				id.did = Transform (id.did,0);
			}
			if (data == "SERVER")    {
				trim(token); id.server = token;
			}
			if (data == "EXT")       {
				trim(token); id.extension = token;
			}
			if (data == "OPT")       {
				trim(token); id.option = token;
			}
			if (data == "RAW")       {
				id.raw = token;
			}
			if (data == "PROFILE")   {
				trim(token); id.profile = token;
			}
			if (data == "SCRIPT3D")  {
				trim(token); id.script3d = token;
			}
			if (data == "PACKAGE3D") {
				trim(token); id.package3d = token;
			}
			if (data == "FLAG")      {
				trim(token); id.flag = token;
				id.flag = Transform (id.flag,1);
			}
			if (data == "SUBVENDOR") {
				trim(token); id.svid = token;
				id.svid = Transform (id.svid,0);
			}
			if (data == "SUBDEVICE") {
				trim(token); id.sdid = token;
				id.sdid = Transform (id.sdid,0);
			}
		}
		}
		// save the struct...
		// -------------------
		if (id.device != "") {
			Push(id);
		}
	}
	handle.clear();
	handle.close();
}

//===================================
// SPReadFile: read Keymap
//-----------------------------------
template <class T>
void SPReadFile<T>::ImportKeymap(void) {
	ifstream handle(file.c_str());
	string data;
	char *line;
	char *token;
	int n;

	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		if ((line[0] == '#') || (line[0] == 0)) {
			continue;
		}
		Keymap id;
		token = strtok(line,":");

		if (token != NULL) {
			trim(token); id.consolename = token;
		}
		n = 0;
		while (token) {
		token = strtok(NULL,":");
		if (token != NULL) {
		trim (token);
		switch(n) {
			case 0:
			id.model = token;
			break;

			case 1:
			id.layout = token;
			break;

			case 2:
			id.variant = token;
			break;

			case 3:
			id.keycodes = token;
			break;

			case 4:
			id.leftalt = token;
			break;
 
			case 5:
			id.rightalt = token;
			break;

			case 6:
			id.scrollock = token;
			break;

			case 7:
			id.rightctl = token;
			break;

			default:
			id.options   += ":";
			id.options   += token;
			break;
		}
		n++;
		}
		}
		if (id.options[0] == ':') {
			id.options.erase (0,1);
		}
		if (id.model != "") {
			Push(id);
		}
	}
	handle.clear();
	handle.close();
}

//===================================
// SPReadFile: read input map
//-----------------------------------
template <class T>
void SPReadFile<T>::ImportInputMap(void) {
	ifstream handle(file.c_str());
	string data;
	char *line;
	char *token;
	int n;

	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		if ((line[0] == '#') || (line[0] == 0)) {
			continue;
		}
		Input id;
		token = strtok(line,":");

		if (token != NULL) {
			trim(token); id.vid = token;
		}
		n = 0; 
		while (token) {
		token = strtok(NULL,":");
		if (token != NULL) {
		trim (token);
		switch(n) {
			case 0:
			id.did = token;
			break;

			case 1:
			id.profile = token;
			break;

			case 2:
			id.driver = token;
			break;

			default:
			break;
		}
		n++;
		}
		}
		Push(id);
	}
	handle.clear();
	handle.close();
}

//===================================
// SPReadFile: read driver map
//-----------------------------------
template <class T>
void SPReadFile<T>::ImportDriverMap(void) {
	ifstream handle(file.c_str());
	string data;
	char *line;
	char *token;
	int n;

	// check if file could be opened...
	// ---------------------------------- 
	if (! handle) {
		cout << "SPReadFile: could not open file: " << file << endl;
		exit(1);
	}
	// read line per line and push the data...
	// ----------------------------------------
	while(! handle.eof()) {
		line  = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
		handle.getline(line,MAX_LINE_SIZE);
		if ((line[0] == '#') || (line[0] == 0)) {
			continue;
		}
		Driver id;
		token = strtok(line,":");

		if (token != NULL) {
			trim(token); id.driver = token;
		}
		n = 0; 
		while (token) {
		token = strtok(NULL,":");
		if (token != NULL) {
		trim (token);
		switch(n) {
			case 0:
			id.profile = token;
			break;

			default:
			break;
		}
		n++;
		}
		}
		Push(id);
	}
	handle.clear();
	handle.close();
}

//===================================
// SPReadFile: incl. template to map
//-----------------------------------
template <class T>
void SPReadFile<T>::Push(T& e) {
	qI[count] = e;
	count++;
}

//===================================
// SPReadFile: get map element
//-----------------------------------
template <class T>
T SPReadFile<T>::Pop(void) {
	count--;
	if (count < 0) {
		throw ReadLastElement();
	}
	T element(qI[count]);
	return element;
}


//===================================
// SPReadFile: Transform string...
//-----------------------------------
template <class T>
string SPReadFile<T>::Transform(string s, int direction) {
	char *part = NULL;
	int  size  = 0;

	size = s.length();
	part = (char*)malloc(size+1);

	for(int n=0;n<size;n++) {
	if (direction == 0) {
		part[n] = (char)tolower(s[n]);
	} else {
		part[n] = (char)toupper(s[n]);
	}
	}
	part[size]    = '\0';
	string result = part;
	return(result);

	#if 0
	if (direction == 0) {
		transform(s.begin(),s.end(),s.begin(), tolower);
	} else {
		transform(s.begin(),s.end(),s.begin(), toupper);
	}
	#endif
}

#endif
