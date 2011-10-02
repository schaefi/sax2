/**************
FILE          : d3.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : 3D package/environment detection code 
              : 
              :
STATUS        : Status: Up-to-date
**************/

#include "d3.h"
#include "config.h"
#include "server.h"

//===========================================
// Scan3D: constructor...
//-------------------------------------------
Scan3D::Scan3D (void) {
	question  = -1;
	withx     = -1;
	checkflag = 1;
	strcpy (card,"all");
	strcpy (cardopt,"");
}

//===========================================
// Scan3D: constructor...
//-------------------------------------------
Scan3D::Scan3D (int ask,int xavailable,str cardnr,str copt) {
	question = ask;
	withx    = xavailable;
	strcpy(card,cardnr);
	strcpy(cardopt,copt);
}

//===========================================
// Scan3D: set flag to call vendor.pl or not
//-------------------------------------------
void Scan3D::SetCheck (int status) {
	checkflag = status;
}

//======================================
// Scan3D: set file name to save
//--------------------------------------
void Scan3D::SetFile(const str name) {
	file = name;
}

//======================================
// Scan3D: check package for install
//--------------------------------------
int Scan3D::Installed(str pac) {
	string check = qx(INSTALLED,STDOUT,1,"%s",pac);
	if (check == "yes") { return(1); }
	return(0);
}

//======================================
// Scan3D: check package for install
//--------------------------------------
int Scan3D::Installed(string pac) {
	str package ; strcpy(package,pac.c_str());
	string check = qx(INSTALLED,STDOUT,1,"%s",package);
	if (check == "yes") { 
		return(1); 
	}
	return(0);
}

//===========================================
// Scan3D: scan data according to server...
//-------------------------------------------
void Scan3D::Scan (void) {
	int device    = 0;
	int chip      = 0;
	str none      = "<none>";
	//str soft3dpac = "mesasoft";
	//str soft3dscr = "switch2mesasoft";
	str soft3dpac = "<none>";
	str soft3dscr = "<none>";
	str work      = "";
	str card      = "0";
	str D3Answer  = "no";
	string flag   = "<none>";
	map <int,string> package;
	map <int,string> scripts;
	map <int,string> paclist;
	char *token   = NULL;

	//======================================
	// prepare 3D environment
	//--------------------------------------
	ServerData graphics;
	ScanServer lookup (question,withx,card,cardopt);
	lookup.SetFile (SERVER_DATA);
	if (lookup.Read() >= 0) {
		device = lookup.Count();
		graphics = lookup.Pop();

		if (graphics.flag == "3D") {
			sprintf (D3Answer,"yes");
		}
		if ((device == 1) && (graphics.detected > 1)) {
			// ...
			// more than one card installed, but only one
			// card used for this setup. We need to pass
			// the chipnumer for the server scan
			// ---
			ScanServer allCards;
			allCards.NoPrimaryCheck();
			allCards.Scan();
			for (int i = allCards.Count(); i > 0; i--) {
				ServerData data = allCards.Pop();
				if (
				(data.bus  == graphics.bus)  &&
				(data.slot == graphics.slot) &&
				(data.func == graphics.func)
				) {
					break;
				}
				chip++;
			}
			sprintf (card,"%d",chip);
		}
	}
	// ...
	// Obtain the driver used if the answer for the 3D
	// question whould have been set to: "yes". If there is
	// only one card I can call server.Scan() and can be sure
	// there will be no X-Server test because we have only
	// one card. If more than one card is installed the 3D
	// system is switched off so this information isn't
	// important then
	// ---
	string driverName = graphics.module;
	if ((device == 1) && (graphics.module != "vmware")) {
		ScanServer server;
		server.SetQuestion (ANSWER_3D_WITH_YES);
		server.SetCard (card);
		server.Scan();
		for (int i = server.Count(); i > 0; i--) {
			ServerData data = server.Pop();
			package[0] = data.package3d;
			scripts[0] = data.script3d;
			driverName = data.module;
		}
	}
	// ...
	// call vendor.pl to check if the driver used is not an
	// X11 driver and set special flags according to
	// this driver:
	// ---
	// . At the moment only nvidia is affected. 
	// . If the checkFlag variable isn't set we will use
	//   the previous value for the special flags
	// ---
	if (checkflag) {
		if (driverName == "nvidia") {
		string vendor = qx(
			GETVENDOR,STDOUT,1,"%s",driverName.c_str()
		);
		if (vendor == "NVIDIA Corporation") {
			flag = "NVReal";
		}
		if (vendor == MODULE_VENDOR) {
			flag = "NVDummy";
		}
		}
	} else {
		Scan3D previous;
		previous.SetFile(D3_DATA);
		if (previous.Read() == 0) {
			D3Data data; data = previous.Pop();
			flag = data.specialflag;
		}
	}
	// ...
	// check special flags and decide which software
	// script we need to call. In normal case we will use
	// the switch2mesasoft script but there may be a
	// cases which doesn't allow to call this script
	// ---
	if (flag == "NVReal") {
		sprintf (soft3dscr,"%s",none);
		sprintf (soft3dpac,"%s",none);
	}

	//======================================
	// setup 3D data now
	//--------------------------------------
	if ((device > 1) || (graphics.module == "vmware")) {
		// ...
		// Unfortunatelly X11 does not support 3D for
		// multihead we will generally disable this feature on
		// multihead requests...
		// -----------------------
		scripts[0] = soft3dscr;
		package[0] = soft3dpac;
		if (Installed(soft3dpac)) {
			package[0] = none;
		}
		strcpy(data.install,package[0].c_str());
		strcpy(data.script_soft ,scripts[0].c_str());
		strcpy(data.script_real ,none);
		strcpy(data.remove ,none);
		strcpy(data.packs  ,none);
	} else {
		// ...
		// OK, we are trying to setup 3D for a single
		// card, this may be possible :-))
		// ----------------------------------
		if (package[0] == "") package[0] = soft3dpac;
		if (scripts[0] == "") scripts[0] = soft3dscr;

		int n = 1;
		strcpy(work,package[0].c_str());
		strcpy(data.packs,work);
		token = strtok(work,",");
		paclist[0] = token;
		while (token) {
			token = strtok(NULL,",");
			if (token != NULL) {
				paclist[n] = token; n++;
			}
		}
		string install;
		for(int i=0;i<n;i++) {
		if (! Installed(paclist[i])) {
		if (install == "") {
			install = paclist[i];
		} else {
			install = install + "," + paclist[i];
		}
		}
		}
		if (install == "") {
			package[0] = none;
		} else {
			package[0] = install;
		}
		strcpy(data.install,package[0].c_str());
		strcpy(data.script_real ,scripts[0].c_str());
		strcpy(data.script_soft ,soft3dscr);
		strcpy(data.remove ,none);
	}
	// ...
	// use libsax xcmd program to check for the
	// current 3D usability state
	// ---
	data.active = 0;
	string option ("--status3D");
	string diagString = qx ( XCMD,STDOUT,1,"%s",option.c_str() );
	int substr = diagString.find("enabled");
	if (substr >= 0) {
		data.active = 1;
	}
	strcpy (data.answer,D3Answer);
	strcpy (data.specialflag,flag.c_str());
}

//===========================================
// Scan3D: return D3Data data struct...
//-------------------------------------------
D3Data Scan3D::Pop (void) {
	return(data);
}

//===========================================
// Scan3D: save data to file...
//-------------------------------------------
int Scan3D::Save (void) {
	ofstream handle(file.c_str(),ios::binary);

	if (! handle) {
		cout << "Scan3D: could not create file: ";
		cout << file << endl;
		return(-1);
	}
	handle.write((char*)&data,sizeof(data));
	handle.close();
	return(0);
}

//===========================================
// Scan3D: retrieve data from file...
//-------------------------------------------
int Scan3D::Read (void) {
	ifstream handle(file.c_str(),ios::binary);

	if (! handle) {
		//cout << "Scan3D: could not open file: "
		//cout << file << endl;
		return(-1);
	}
	handle.read((char*)&data,sizeof(data));
	handle.close();
	return(0);
}
