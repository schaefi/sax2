/**************
FILE          : keyboard.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : keyboard detection code 
              : 
              :
STATUS        : Status: Up-to-date
**************/

#include "keyboard.h"
#include "config.h"

//======================================
// Definitions...
//--------------------------------------
#define KEYTABLE  "KEYTABLE"

//======================================
// ScanKeyboard: constructor...
//--------------------------------------
ScanKeyboard::ScanKeyboard(void) {
	elements = 0;
	current  = 0;
}

//======================================
// ScanKeyboard: set file for saving
//--------------------------------------
void ScanKeyboard::SetFile(const str name) {
	file = name;
}

//======================================
// ScanKeyboard: push KBD data to map
//--------------------------------------
void ScanKeyboard::Push (Keymap e) {
	qK[elements] = e;
	elements++;
}

//======================================
// ScanKeyboard: get the keyboard data
//--------------------------------------
Keymap ScanKeyboard::Pop (void) {
	if (current >= elements) {       
		throw ReadLastElement();
	}
	Keymap element(qK[current]);
	current++;
	return element;
}

//======================================
// ScanKeyboard: reset map counter...
//--------------------------------------
void ScanKeyboard::Reset (void) {
	current = 0;                        
} 

//======================================
// ScanKeyboard: hw scan of keyboard
//--------------------------------------
void ScanKeyboard::Scan (void) {
	KbdData* pKBD = KeyboardGetData();
	SPReadFile<Input>  rcinput;
	SPReadFile<RcData> rcdata;
	SPReadFile<Keymap> rckeymap;
	RcData config;
	Keymap keymap;
	Input  input;

	// ...
	// assign profile to keyboard device using Input.map
	// ---
	rcinput.SetFile(INPUT_MAP);
	rcinput.ImportInputMap();
	for (KbdData* kbd=pKBD; kbd; kbd=kbd->next) {
		strcpy (kbd->profile,"<undefined>");
		for (int i = rcinput.Count(); i > 0; i--) {
			input = rcinput.Pop();
			if ((input.did == kbd->did) && (input.vid == kbd->vid)) {
				strcpy (kbd->profile,input.profile.c_str());
				break;
			}
		}
		rcinput.Reset();
	}
	// ...
	// setup console config files...
	// ----------------------------- 
	rcdata.SetFile(RC_SYSCONFIG,"keyboard");
	rckeymap.SetFile(KBD_MAP);

	// ...
	// import sysconfig and sax map file...
	// ------------------------------------
	config = rcdata.ImportRcConfig();
	rckeymap.ImportKeymap();

	// ...                              
	// check KEYTABLE variable...
	// ---------------------------
	string check = qx(CHECKMAP,STDOUT,1,"%s",config[KEYTABLE].c_str());
	config[KEYTABLE] = check;

	// ...
	// get machine architecture...
	// ------------------------------
	string arch = qx("/bin/arch",STDOUT);

	// ...
	// save data from hw scan...
	// --------------------------
	for (KbdData* kbd=pKBD; kbd; kbd=kbd->next) {
		Keymap kp;
		kp.variant = "";
		kp.device = kbd->device;              
		kp.name   = kbd->name;
		kp.did    = kbd->did;
		kp.vid    = kbd->vid;
		kp.profile= kbd->profile;
		if (strstr((char*)arch.c_str(), ARCH_SPARC) != NULL) {
			kp.model  = kbd->model;
			kp.layout = kbd->layout;
		} else {
			kp.model  = "undef";
			kp.layout = "undef";
		}
		rckeymap.Reset();
		for (int i = rckeymap.Count(); i > 0; i--) {
			keymap = rckeymap.Pop();
			if (keymap.consolename == config[KEYTABLE]) {
				// found console keymap adapter...
				// -----------------------------------
				if (kp.model  == "undef") {
					kp.model  = keymap.model;
				}
				if (kp.layout == "undef") {
					kp.layout = keymap.layout;
				}
				kp.variant     = keymap.variant;
				kp.consolename = config[KEYTABLE]; 
				kp.keycodes    = keymap.keycodes;
				kp.leftalt     = keymap.leftalt;
				kp.rightalt    = keymap.rightalt;
				kp.scrollock   = keymap.scrollock;
				kp.rightctl    = keymap.rightctl;
				kp.options     = keymap.options;
				break;
			}
		}
		// ...
		// handle special cases...
		// ------------------------
		// 1)
		// PPC protocol detection via script
		// ----------------------------------
		if (arch == ARCH_PPC) {
		string model = qx(MACHINE,STDOUT); 
		if (model != "") {
			kp.model = model;
		}
		}
		Push(kp);
	}
}

//======================================
// ScanKeyboard: save data to file
//--------------------------------------
int ScanKeyboard::Save (void) {
	ofstream handle(file.c_str(),ios::binary);
	if (! handle) {
		cout << "ScanKeyboard: could not create file: ";
		cout << file << endl;
		return(-1);
	}
	Reset();
	for (int i = Count(); i > 0; i--) {
		Keymap kp = Pop();
		Keymap_S save;
		// reformat to primitive types...
		// -------------------------------
		strcpy(save.consolename , kp.consolename.c_str());
		strcpy(save.model       , kp.model.c_str());
		strcpy(save.layout      , kp.layout.c_str());
		strcpy(save.variant     , kp.variant.c_str());
		strcpy(save.options     , kp.options.c_str());
		strcpy(save.keycodes    , kp.keycodes.c_str());
		strcpy(save.leftalt     , kp.leftalt.c_str());
		strcpy(save.rightalt    , kp.rightalt.c_str());
		strcpy(save.scrollock   , kp.scrollock.c_str());
		strcpy(save.rightctl    , kp.rightctl.c_str());
		strcpy(save.device      , kp.device.c_str());
		strcpy(save.name        , kp.name.c_str());
		strcpy(save.did         , kp.did.c_str());
		strcpy(save.vid         , kp.vid.c_str());
		strcpy(save.profile     , kp.profile.c_str());

		handle.write((char*)&save,sizeof(save));
	}
	handle.close();
	return(0);
}

//======================================
// ScanKeyboard: retrieve data from file
//--------------------------------------
int ScanKeyboard::Read (void) {
	ifstream handle(file.c_str(),ios::binary);
	if (! handle) {
		//cout << "ScanKeyboard: could not open file: ";
		//cout << file << endl;
		return(-1);
	}
	elements = 0;                  
	current  = 0;                             
	qK.clear();

	while (1) {
		Keymap kp;
		Keymap_S input;
		handle.read((char*)&input,sizeof(input));

		// save to object data
		// ---------------------
		kp.consolename = input.consolename;
		kp.model       = input.model;
		kp.layout      = input.layout;
		kp.variant     = input.variant;
		kp.options     = input.options;
		kp.keycodes    = input.keycodes;
		kp.leftalt     = input.leftalt;
		kp.rightalt    = input.rightalt;
		kp.scrollock   = input.scrollock;
		kp.rightctl    = input.rightctl;
		kp.name        = input.name;
		kp.device      = input.device;
		kp.did         = input.did;
		kp.vid         = input.vid;
		kp.profile     = input.profile;

		if (handle.eof()) {
			break;                                   
		}
		Push (kp);
	}
	handle.close();
	return(0);
}
