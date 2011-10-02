/**************
FILE          : server.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : server PCI detection code 
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "server.h"
#include "config.h"
#include "cfg.h"
#include "mouse.h"

//======================================
// ScanServer: constructor...
//--------------------------------------
ScanServer::ScanServer(void) {
	ask      = -1;
	withx    = -1;
	pcheck   = 1;
	elements = 0;
	current  = 0;
	checkQuestion  = 0;
	checkPCIVendor = 0;
}

//======================================
// ScanServer: constructor...
//--------------------------------------
ScanServer::ScanServer(int question,int xavailable,str card,str copt) {
	ask   = question;
	withx = xavailable;
	SetCard(card);
	SetCardModule(copt);
	checkQuestion  = 0;
	checkPCIVendor = 0;
	elements = 0;
	current  = 0;
	pcheck   = 1;
}

//======================================
// ScanServer: disable primary check
//--------------------------------------
void ScanServer::NoPrimaryCheck (void) {
	pcheck   = 0;
}

//======================================
// ScanServer: do only a question check
//--------------------------------------
void ScanServer::CheckForQuestionOnly (void) {
	checkQuestion = 1;
}

//======================================
// ScanServer: set question flag
//--------------------------------------
void ScanServer::SetQuestion(int question) {
	ask = question;
}

//======================================
// ScanServer: set X11 available flag
//--------------------------------------
void ScanServer::SetXAvailable(int available) {
	withx = available;
}

//======================================
// ScanServer: set card(s) for scanning
//--------------------------------------
void ScanServer::SetCard(str card) {
	char *token;
	str work;
	int nr;
	if (strcmp(card,"all") == 0) { 
		return; 
	}
	if (strcmp(card,"")    == 0) { 
		return; 
	}
	strcpy(work,card);
	token = strtok(work,",");
	if (token != NULL) {
		trim(token); nr = atoi(token);
		scanme.push_back(nr);
	}
	while (token) {
	token = strtok(NULL,",");
	if (token != NULL) {
		trim(token); nr = atoi(token);
		scanme.push_back(nr);
	}
	}
}

//======================================
// ScanServer: set modules for card(s)
//--------------------------------------
void ScanServer::SetCardModule(str copt) {
	char *token = NULL;
	char *card  = NULL;
	str work;
	string module;
	int nr;
	if (strcmp(copt,"") == 0) { 
		return; 
	}
	if (strcmp(copt,"none") == 0) { 
		return; 
	}
	strcpy(work,copt);
	token = strtok(work,",");
	if (token != NULL) {
		card = strsep(&token,"=");
		if (token != NULL) {
			trim(card);   nr = atoi(card);
			trim(token);  module = token;
			if (module != "") {
				moduleme[nr] = module;
			}
		}
	}
	while (token) {
	token = strtok(NULL,",");
	if (token != NULL) {
		card = strsep(&token,"=");
		if (token != NULL) {
			trim(card);   nr = atoi(card);
			trim(token);  module = token;
			if (module != "") {
				moduleme[nr] = module;
			}
		}
	}
	}
}

//======================================
// ScanServer: set file for saving
//--------------------------------------
void ScanServer::SetFile(const str name) {
	file = name;
}

//======================================
// ScanServer: reset data map
//--------------------------------------
void ScanServer::Reset(void) {
	current = 0;
}

//======================================
// ScanServer: push data to map
//--------------------------------------
void ScanServer::Push(ServerData e) {
	qS[elements] = e;
	elements++;
}

//======================================
// ScanServer: retrieve data from map
//--------------------------------------
ServerData ScanServer::Pop(void) {
	if (current >= elements) {
		throw ReadLastElement();
	}
	ServerData element(qS[current]);
	current++;
	return element;
}

//======================================
// ScanServer: identify device (PRIVAT)
//--------------------------------------
int ScanServer::
IdentifyDevice (
	IdentMap m,int vid,int did,int svid,int sdid,int cards,int ask,int cardNr
) {
	int n = m.size();
	str vid_str,did_str,svid_str,sdid_str;
	string vidS,didS,svidS,sdidS;
	map <string,int> fgroup;
	map <string,int>::iterator pos;
	int identified = 0;

	sprintf(vid_str, "0x%04x",vid);   vidS  = vid_str; 
	sprintf(did_str, "0x%04x",did);   didS  = did_str;
	sprintf(svid_str,"0x%04x",svid);  svidS = svid_str;
	sprintf(sdid_str,"0x%04x",sdid);  sdidS = sdid_str;
 
	for (int i=0;i<n;i++) {
	if ( 
		(vidS  == m[i].vid)  && (didS  == m[i].did) && 
		(svidS == m[i].svid) && (sdidS == m[i].sdid) 
	) {
		fgroup[m[i].flag] = i;
		identified = 1;
	}
	}
	for (int i=0;i<n;i++) {
	if ( 
		(vidS  == m[i].vid)  && (didS  == m[i].did) && 
		(m[i].svid == "0x0000")  && (m[i].sdid == "0x0000")
	) {
		if (identified == 0) {
			fgroup[m[i].flag] = i;
		}
	}
	}
	string answer;
	int i = -1;
	pos = fgroup.find(FLAG_DEFAULT);
	if (pos == fgroup.end()) {
		return(i);
	}
	i = fgroup[FLAG_DEFAULT];
	switch (ask) {
	case -1:
		return ( fgroup[FLAG_DEFAULT] );
	case 2:
		return ( fgroup[FLAG_DEFAULT] );
	case 3:
		if ( fgroup[FLAG_3D] ) {
			return ( fgroup[FLAG_3D] );
		} else {
			// ... /
			// there is no 3D specification but we requested one
			// with the 3D answer set to yes. We will return the
			// default entry in this case
			// ---
			return ( fgroup[FLAG_DEFAULT] );
		}
	}

	// ...
	// try to find 3D entry if it exist save index
	// position
	// --
	pos = fgroup.find(FLAG_3D);

	// ...
	// if checkQuestion is set we will only check if
	// we had to answer a question or not 
	if (checkQuestion == 1) {
	if ((pos != fgroup.end()) && (cards <= 1)) {
		printf ("%s %s\n",
			m[fgroup[FLAG_3D]].name.c_str(),m[fgroup[FLAG_3D]].device.c_str()
		);
		exit (1);
	} else {
		printf ("nil\n");
		exit (0);
	}
	}

	// handle 3D flag (no 3D for multihead)...
	// -----------------------------------------
	string profilecount = "0";
	if (! m[fgroup[FLAG_3D]].profile.empty()) {
	profilecount = qx (
		PROFILECOUNT,STDOUT,1,"%s", m[fgroup[FLAG_3D]].profile.c_str()
	);
	}
	if ((pos != fgroup.end()) && (cards <= 1) && (profilecount == "0")) {
		qx (KILLDOT,STDOUT);

		if (withx != 1) {
		// ask me using the console...
		cerr << "/----/" << endl;
		cerr << "Card: " << cardNr;
		cerr << " -> "   << m[fgroup[FLAG_3D]].name; 
		cerr << " "      << m[fgroup[FLAG_3D]].device;
		cerr << endl     << TEXT_3D;
		cerr << endl;
		cerr << endl;
		cerr << "[ yes | <no> ]  ";

		getline(cin,answer);
		if (answer == "yes") {
			i = fgroup[FLAG_3D];
		}
		} else {
		#if 0
		// in former times this code was used to display a X11 based
		// message box to ask the user if he wants to enable 3D. we are
		// omitting the code of this if/else statement completely by
		// setting the answer to this question with the sysp option
		// [-A] within init.pl. So basically this code is never reached.
		// The current implementation therefore doesn't support displaying
		// a special 3D message box and that's the reason why the following
		// block is disabled
		// ---
		// ask me using X11 [Qt interface]...
		str optdrv = "--driver";
		str opt3d  = "--3d";
		str optfn  = "-fn";
		str optst  = "-style";
		str plati  = "platinum";
		str frame  = "-w";
		str width  = "2";
		str font   = 
			"-gnu-unifont-medium-r-normal--16-160-75-75-p-80-iso10646-1";
		str drv;  sprintf(drv,"%s",
			m[fgroup[FLAG_3D]].server.c_str()
		);
		str name; sprintf(name,"%s %s",
			m[fgroup[FLAG_3D]].name.c_str(),m[fgroup[FLAG_3D]].device.c_str()
		);
		string code = qx (
			QTASK,STDOUT,10,"%s %s %s %s %s %s %s %s %s %s",
				opt3d,name,optst,plati,optfn,font,optdrv,drv,frame,width
		);
		if (atoi(code.c_str()) == 0) {
			i = fgroup[FLAG_3D];
		}
		#endif
		}
	}
	// handle other flags...
	// ----------------------
	// <no flags except 3D at the moment>...
	// ---
	return(i);
}

static int
cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */
    
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

//======================================
// ScanServer: PCI/AGP hardware scan
//--------------------------------------
void ScanServer::Scan(void) {
	PciData *first = PciGetData();
	map<int,ServerData> pci;
	map<int,ServerData> graphics;
	map<int,ServerData> equals;
	map<int,ServerData> save;
	map<int,Identity>   config;
	int algorithm;
	int count = 0;
	int n     = 0;
	int mapnr = 0;

	// delete the scanserverstuff file if it exist
	// ---------------------------------------------
	unlink(SERVER_STUFF_DATA);
 
	// save the complete PCI/AGP stuff into
	// a local map called [ pci ]
	// ---------------------------
	for(PciData* p=first; p; p=p->next) {
		ServerData data;

		data.cls    = p->cls;
		data.domain = p->domain;
		data.bus    = p->bus;
		data.slot   = p->slot;
		data.func   = p->func;
		data.vid    = p->vid;
		data.did    = p->did;

		if (p->bus == 0) {
			data.bustype = "PCI";
		} else {
			data.bustype = "AGP";
		}

		data.subvendor = p->svid;
		data.subdevice = p->sdid;
		pci[count] = data;  
		count++;
	}

	// import the Identity.map, the data will be used
	// on several points up to this point
	// -----------------------------------
	SPReadFile<Identity> configI;
	// import update Identity files if there are any
	struct dirent* entry = 0;
	DIR* updateDir = 0;
	int i, fcount = 0;
	char **a = 0;
	updateDir = opendir (IDENTITY_UPDATED);
	if (updateDir) {
		while (1) {
			entry = readdir (updateDir);
			if (! entry) {
				break;
			}
			if (
				(strstr(entry->d_name,"Identity.map")) &&
				(entry->d_type != DT_DIR)
			)
			fcount++;
		}
		rewinddir(updateDir);
		a = (char **) malloc(fcount * sizeof (char*));
		i = 0;
		while(1) {
			entry = readdir (updateDir);
			if (! entry) {
				break;
			}
			if (
				(strstr(entry->d_name,"Identity.map")) &&
				(entry->d_type != DT_DIR)
			) {
				//printf("%s\n", entry->d_name);
				a[i] = (char*) malloc(strlen(entry->d_name)+1);
				strcpy(a[i], entry->d_name);
				i++;
			}
		}
	}
	closedir (updateDir);
	qsort(a, fcount, sizeof(char*), cmpstringp);
	if (a) {
		for (i = 0; i < fcount ; i++) {
			string mapFile = IDENTITY_UPDATED;
			mapFile += a[i];
			configI.SetFile ((char*)mapFile.c_str());
			//printf("Reading %s\n", a[i]);
			configI.ImportIdentity();
			free (a[i]);
		}
	}
	// import the base Identity map
	free (a);
	configI.SetFile(IDENTITY);
	configI.ImportIdentity();
	for (int i = configI.Count(); i > 0; i--) {
		config[n] = configI.Pop();
		n++;
	}

	// read the driver map to check if this driver
	// has a driver profile
	// --------------------
	SPReadFile<Driver> driverI;
	driverI.SetFile(DRIVER_MAP);
	driverI.ImportDriverMap();
	Driver drvmap;

	// identify the graphics cards in the pool 
	// of cards
	// ---------
	n = 0;
	for (int i=0;i<count;i++) {
	if (pci[i].cls == 0x300) {
		graphics[n] = pci[i];
		n++;
	} else {
		mapnr = IdentifyDevice (
			config,
			pci[i].vid,pci[i].did,
			pci[i].subvendor,pci[i].subdevice,
			NO_FLAG_QUESTION,NO_FLAG_QUESTION
		);
		if (mapnr >= 0) {
			graphics[n] = pci[i];
			n++;
		}
	}
	}

	// check how many cards we found...
	// ---------------------------------
	if (n == 0) {
		// ...
		// Ooops we didn`t find any PCI/AGP card;
		// assuming unclassified device
		// ---
		ServerData data;
		data.bustype   = "NUL";     // no bus location detected
		data.subvendor = 0x0000;
		data.subdevice = 0x0000;
		data.vid       = UNCVID;
		data.did       = UNCDID;
		data.domain    = 0x0;
		data.bus       = 0x0;       // there is a profile
		data.slot      = 0x1;       // called nobus which will
		data.func      = 0x2;       // remove the wrong BusID entry
		if (! haveFBdev()) {
		if (! haveVesaBIOS()) {
			data.module = "vga";
		} else {
			data.module = "vesa";
		}
		}
		pci[n]         = data;
		graphics[n]    = pci[n];
		n = 1;
	}
	// ...
	// check for special devices which comes with more 
	// than one chip on a single device. 
	// --> equal BUS and SLOT number
	//
	// check algorithm:
	// -----------------
	// 1) look if we can assign the function 0 device to a driver module
	//    if yes this is it and all other non function 0 devices
	//    will be removed
	//
	// 2) if the function 0 device is unknown look at the rest 
	//    stuff and try to identify them, the result remains in the
	//    pci stuff all other stuff including the function 0 device
	//    will be removed
	// --
	int bus   = 0;
	int slot  = 0;
	int group = 0;
	int card  = 0;
	int dom   = 0;
	for (int i=0;i<n;i++) {
		if (graphics[i].bus == -1) {
			continue;
		} 
		equals[group] = graphics[i];
		bus  = graphics[i].bus;
		slot = graphics[i].slot;
		dom  = graphics[i].domain;

		for(int k=0;k<n;k++) {
		if ((k != i) &&
			(graphics[k].bus==bus) && (graphics[k].slot==slot) &&
			(graphics[k].domain==dom)
		) {
			group++;
			equals[group] = graphics[k];
			graphics[k].bus = -1;
		}
		} 
		if (group == 0) {
			// ...
			// no multiple devices found...
			// -----------------------------
			save[card] = graphics[i]; card++;
		} else {
			// ...
			// multiple device found:
			// start check algorithm...
			// -------------------------- 
			algorithm = 0;
			// 1)
			for (int k=0;k<=group;k++) {
			if (equals[k].func == 0x0) {
				mapnr = IdentifyDevice (
					config,
					equals[k].vid,equals[k].did,
					equals[k].subvendor,equals[k].subdevice,
					NO_FLAG_QUESTION,NO_FLAG_QUESTION
				);
				if (mapnr >= 0) {
					save[card] = equals[k]; card++;
					algorithm  = 1;
					break;
				}
			}
			}
			// 2)
			if (algorithm == 0) {
			for (int k=0;k<=group;k++) {
			if (equals[k].func != 0x0) {
				mapnr = IdentifyDevice (
					config,
					equals[k].vid,equals[k].did,
					equals[k].subvendor,equals[k].subdevice,
					NO_FLAG_QUESTION,NO_FLAG_QUESTION
				);
				if (mapnr >= 0) {
					save[card] = equals[k]; card++;
				}
			}
			}
			}
		}
		equals.clear();
		group = 0;
	}
	// ...
	// save the detected number of cards and remap
	// data to graphics...
	// --------------------
	graphics.clear();
	card = save.size();
	for (int k=0;k<card;k++) {
		save[k].detected = card; graphics[k] = save[k];
	}
	// ...
	// check the moduleme options, if the module is set
	// manually here we will not overwrite it
	// --------------------------------------
	int modsize = moduleme.size();
	if (modsize > 0) {
	map <int,string>::iterator pos;
	for (int i=0;i<=modsize;i++) {
		pos = moduleme.find(i);
		if (pos != moduleme.end()) {
			graphics[i].module = moduleme[i];
		}
	}
	} 
	// ...
	// check the scanme options, this could change the order 
	// and amount of cards to use for configuration
	// ---------
	save.clear();
	int optcard = 0;
	int nr      = 0;
	int optsize = scanme.size();
	if (optsize > 0) {
		ServerDataMap::iterator pos;
		for (int i=0;i<optsize;i++) {
			optcard = scanme.front();
			scanme.pop_front();
			pos = graphics.find(optcard);
			if (pos != graphics.end()) {
				ServerData data = graphics[optcard];
				save[nr] = data;
				nr++;
			} else {
				// ...
				// Card is not defined... skip
				// ---
			}
		}
		card = nr;
		graphics.clear();
		graphics = save;
	}
	// ...
	// identify the card(s) fully using the 
	// Identity.map
	// ---
	int askid = -1;
	for (int i=0;i<card;i++) {
		askid = ask;
		// ...
		// do not ask and use the DEFAULT 
		// flag if vga/fbdev/vesa is set
		// ---
		int cardnumber = card;
		if (
			( graphics[i].module == "vga" )   || 
			( graphics[i].module == "fbdev" ) ||
			( graphics[i].module == "vesa" )
		) {
			cardnumber = NO_FLAG_QUESTION;
		}
		mapnr = IdentifyDevice (
			config,
			graphics[i].vid,graphics[i].did,
			graphics[i].subvendor,graphics[i].subdevice,
			cardnumber,askid,i
		);
		if (mapnr >= 0) {
		//======================================
		// get identification...
		//--------------------------------------
		graphics[i].vendor    = config[mapnr].name;
		graphics[i].device    = config[mapnr].device;
		graphics[i].flag      = config[mapnr].flag;
		graphics[i].profile   = config[mapnr].profile;
		graphics[i].raw       = config[mapnr].raw;
		graphics[i].option    = config[mapnr].option;
		graphics[i].extension = config[mapnr].extension;
		graphics[i].script3d  = config[mapnr].script3d;
		graphics[i].package3d = config[mapnr].package3d;
		} else {
		//======================================
		// did not get identification, 
		// set defaults...
		//--------------------------------------
		graphics[i].vendor    = "VESA";
		graphics[i].device    = "Framebuffer Graphics";
		graphics[i].flag      = "DEFAULT";
		graphics[i].profile   = "";
		graphics[i].raw       = "";
		graphics[i].option    = "";
		graphics[i].extension = "";
		graphics[i].script3d  = "";
		graphics[i].package3d = "";

		if (
			(graphics[i].module == "") || 
			((graphics[i].vid == UNCVID) && (graphics[i].did == UNCDID) )
		) {
			if ( haveFBdev() ) {
				graphics[i].module   = "fbdev";
			} else {
			if ( haveVesaBIOS() ) {
				graphics[i].module   = "vesa";
				graphics[i].vendor   = "VESA";
				graphics[i].device   = "Vesa-BIOS Graphics";
			} else {
				graphics[i].module   = "vga";
				graphics[i].vendor   = "Unclassified";
				graphics[i].device   = "Graphics";
			}
			}
		}
		if (checkPCIVendor) {
			// ...
			// we did not get any identification to this card, but
			// the option --pcivendor was set. We will look at the
			// vendor -> driver table and try to set a driver in this
			// case
			// ---
			str vid_str;
			RcData vendorMap;
			SPReadFile<RcData> rcVendor;
			rcVendor.SetFile (VENDOR_MAP);
			vendorMap = rcVendor.ImportVendorMap();
			sprintf(vid_str, "0x%04x",graphics[i].vid);
			string vid = vid_str;
			if (vendorMap[vid].c_str()) {
				graphics[i].module = vendorMap[vid].c_str();
			}
		}
		}
		if (graphics[i].module == "") {
			graphics[i].module  = config[mapnr].server;
		}
		graphics[i].drvprofile = "<undefined>";
		for (int x = driverI.Count(); x > 0; x--) {
			drvmap = driverI.Pop();
			if (drvmap.driver == graphics[i].module) {
				graphics[i].drvprofile = drvmap.profile;
				break;
			}
		}
		driverI.Reset();
	}
	// ...
	// Start TestServer if:
	// - primary card check is needed (multihead environment)
	// - scanme card option is set
	// - vmware driver is used
	// ---
	if (
		((card > 1) && (optsize <= 0) && (pcheck == 1)) ||
		((graphics[0].module == "vmware") && (pcheck == 1))
	) {
		str file;
		sprintf(file,"%s-%d",TMP_CONFIG,getpid());
		// ...
		// open startup config...
		// ---
		ofstream handle(file);
		if (! handle) {
			cout << "ScanServer: could not create file: ";
			cout << TMP_CONFIG << endl;
			return;
		}
		map<int,string> section;
		XF86ConfigFile cfg;
		ScanMouse mouse;
		// ...
		// scan the mouse...
		// ---
		mouse.Scan(); MouseData mdata = mouse.Pop();
		cfg.SetMouseProperties(mdata.protocol,mdata.device);
		// ...
		// create base sections...
		// ------------------------
		section[0] = cfg.DoFilesSection();
		section[1] = cfg.DoModuleSection();
		section[2] = cfg.DoInputDeviceSection();

		cfg.SetSectionID(card);
		section[3] = cfg.DoServerLayoutSection();
		// ...
		// create dynamic sections...
		// ---------------------------
		for (int n=0;n<card;n++) {
			cfg.SetSectionID(n);
			if ((cfg.SetDriver(graphics[n].module)) == 1) {
				cout << "SaX: sorry could not open /dev/fb0... abort" << endl;
				exit(1);
			}
			cfg.SetBus (
				graphics[n].domain,graphics[n].bus,
				graphics[n].slot,graphics[n].func
			);
			section[4] = section[4] + "\n" + cfg.DoMonitorSection();
			section[5] = section[5] + "\n" + cfg.DoScreenSection();
			section[6] = section[6] + "\n" + cfg.DoDeviceSection();
			section[7] = section[7] + "\n" + cfg.DoServerFlagsSection();
		}
		// ...
		// write sections to file...
		// ---------------------------
		handle << section[0] << endl;
		handle << section[1] << endl;
		handle << section[7] << endl;
		handle << section[2] << endl;
		handle << section[4] << endl;
		handle << section[5] << endl;
		handle << section[6] << endl;
		handle << section[3] << endl;
		handle.close();

		// ...
		// call CallXF86Loader...
		// ---
		cfg.CallXF86Loader(file);
		cfg.SetFile(SERVER_STUFF_DATA);
		ParseData parse;
		if (cfg.Count() > 0) {
			parse = cfg.Pop();
		}
		unlink(file);

		// check the primary device...
		// ----------------------------
		if (parse.primary != "") {
			int pbus  = parse.pbus;
			int pslot = parse.pslot;
			int pfunc = parse.pfunc;
			int id    = 0;

			for (int i=0;i<card;i++) {
				int bus  = graphics[i].bus;
				int slot = graphics[i].slot;
				int func = graphics[i].func;
				if ((bus == pbus) && (slot == pslot) && (func == pfunc)) {
					id = i; break;
				}
			}

			if (id != 0) {
				save.clear();
				save[0] = graphics[id];
				int n   = 1;
				for (int i=0;i<card;i++) {
					if (i == id) {
						continue;
					} 
					save[n] = graphics[i];
					n++;
				}
				graphics.clear();
				graphics = save;
			}
			cfg.Save();
		}
	}
	// ...
	// save the result to the object data
	// ---
	for (int i=0;i<card;i++) {
		Push(graphics[i]);
	}
}

//======================================
// ScanServer: read data from file
//--------------------------------------
int ScanServer::Read(void) {
	ifstream handle(file.c_str(),ios::binary); 
	if (! handle) {
		//cout << "ScanServer: could not open file: "
		//cout << file << endl;
		return(-1);
	}
	elements = 0;
	current  = 0;
	qS.clear();

	while(1) {
		ServerData data;
		ServerData_S input;
		handle.read((char*)&input,sizeof(input));
		// ...
		// include data to object...
		// ---
		data.cls       = input.cls;
		data.domain    = input.domain;
		data.bus       = input.bus;
		data.slot      = input.slot;
		data.func      = input.func;
		data.vendor    = input.vendor;
		data.device    = input.device;
		data.vid       = input.vid;
		data.did       = input.did;
		data.module    = input.module;
		data.bustype   = input.bustype;
		data.detected  = input.detected;
		data.flag      = input.flag;
		data.subvendor = input.subvendor;
		data.subdevice = input.subdevice;
		data.profile   = input.profile;
		data.raw       = input.raw;
		data.option    = input.option;
		data.extension = input.extension;
		data.script3d  = input.script3d;
		data.package3d = input.package3d;
		data.drvprofile= input.drvprofile;

		if (handle.eof()) {
			break;
		}
		Push(data);
	}
	handle.close(); 
	return(0);
}

//======================================
// ScanServer: save data to file
//--------------------------------------
int ScanServer::Save(void) {
	ofstream handle(file.c_str(),ios::binary);
	if (! handle) {
		cout << "ScanServer: could not create file: ";
		cout << file << endl;
		return(-1);
	}
	Reset();
	for (int i = Count(); i > 0; i--) {
		ServerData data = Pop();
		ServerData_S save;
		// ...
		// reformat to primitive types...
		// ---
		strcpy(save.vendor     , data.vendor.c_str());
		strcpy(save.device     , data.device.c_str());
		strcpy(save.module     , data.module.c_str());
		strcpy(save.bustype    , data.bustype.c_str());
		strcpy(save.flag       , data.flag.c_str());
		strcpy(save.profile    , data.profile.c_str());
		strcpy(save.raw        , data.raw.c_str());
		strcpy(save.option     , data.option.c_str());
		strcpy(save.extension  , data.extension.c_str());
		strcpy(save.script3d   , data.script3d.c_str());
		strcpy(save.package3d  , data.package3d.c_str());
		strcpy(save.drvprofile , data.drvprofile.c_str());
		save.cls               = data.cls;
		save.domain            = data.domain;
		save.bus               = data.bus;
		save.slot              = data.slot;
		save.func              = data.func;
		save.vid               = data.vid;
		save.did               = data.did;
		save.detected          = data. detected;
		save.subvendor         = data.subvendor;
		save.subdevice         = data.subdevice;
		handle.write(
			(char*)&save,sizeof(save)
		);
	}
	handle.close();
	return(0);
}

//======================================
// ScanServer: check for framebuffer
//--------------------------------------
int ScanServer::haveFBdev (void) {
	int status = open ("/dev/fb0",O_RDONLY|O_NONBLOCK);
	if (status < 0) {
		return (0);
	}
	close (status);
	return (1);
}

//======================================
// ScanServer: check for VESA BIOS
//--------------------------------------
int ScanServer::haveVesaBIOS (void) {
	char* vbios = vesaBIOS();
	if (vbios) {
		return 1;
	}
	return 0;
}

//======================================
// ScanServer: use PCI Vendor space
//--------------------------------------
void ScanServer::SetCheckForPCIVendor (int value) {
	checkPCIVendor = value;
}
