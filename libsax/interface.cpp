/**************
FILE          : interface.cpp
***************
PROJECT       : SaX2 - library interface
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : SaX2 - SuSE advanced X11 configuration 
              : 
              :
DESCRIPTION   : native C++ class library to access SaX2
              : functionality. Easy to use interface for
              : //.../
              : - importing/exporting X11 configurations
              : - modifying/creating X11 configurations 
              : ---
              :
              :
STATUS        : Status: Development
**************/
#include "interface.h"

//====================================
// EntryPoint
//------------------------------------
EntryPoint entrypoint = {
	name: "LML::libsax",
	version: SAX_VERSION,
	factory: (Factory) LML::SaXPluglib::factory
};

namespace LML {
//====================================
// Constructor
//------------------------------------
SaXPluglib::SaXPluglib (void) {
	// ...
}

//====================================
// factory for dynamic loader
//------------------------------------
SaXPluglib* SaXPluglib::factory (void) {
	return new SaXPluglib();
}

//====================================
// Constructor saxInit...
//------------------------------------
SaXInit* SaXPluglib::saxInit ( void ) {
	return new SaXInit ();
}

//====================================
// Constructor saxConfig...
//------------------------------------
SaXConfig* SaXPluglib::saxConfig ( int source ) {
	return new SaXConfig (source);
}

//====================================
// Constructor saxImport...
//------------------------------------
SaXImport* SaXPluglib::saxImport ( int id ) {
	return new SaXImport (id);
}

//====================================
// Constructor saxImportSysp...
//------------------------------------
SaXImportSysp* SaXPluglib::saxImportSysp ( int id ) {
	return new SaXImportSysp (id);
}

//====================================
// Constructor saxImportCDB...
//------------------------------------
SaXImportCDB* SaXPluglib::saxImportCDB  ( int id ) {
	return new SaXImportCDB (id);
}

//====================================
// Constructor saxImportProfile...
//------------------------------------
SaXImportProfile* SaXPluglib::saxImportProfile ( const char* file ) {
	return new SaXImportProfile (file);
}

//====================================
// Constructor saxManipulateDesktop...
//------------------------------------
SaXManipulateDesktop* SaXPluglib::saxManipulateDesktop (
	SaXImport* desktop, SaXImport* card , SaXImport* path, int desktopID
) {
	return new SaXManipulateDesktop (desktop,card,path,desktopID);
}

//====================================
// Constructor saxManipulateCard...
//------------------------------------
SaXManipulateCard* SaXPluglib::saxManipulateCard (
	SaXImport* in, int card
) {
	return new SaXManipulateCard (in,card);
}

//====================================
// Construct saxManipulateKeyboard...
//------------------------------------
SaXManipulateKeyboard* SaXPluglib::saxManipulateKeyboard (
	SaXImport* in, int kbd
) {
	return new SaXManipulateKeyboard (in,kbd);
}

//====================================
// Constructor saxManipulateDevices...
//------------------------------------
SaXManipulateDevices* SaXPluglib::saxManipulateDevices (
	SaXImport* desktop ,SaXImport* card ,SaXImport* layout
) {
	return new SaXManipulateDevices (desktop,card,layout);
}

//====================================
// Constructor saxManipulateDevices...
//------------------------------------
SaXManipulateDevices* SaXPluglib::saxManipulateDevices (
	SaXImport* input, SaXImport* layout
) {
	return new SaXManipulateDevices (input,layout);
}

//====================================
// Constructor saxManipulateLayout...
//------------------------------------
SaXManipulateLayout* SaXPluglib::saxManipulateLayout (
	SaXImport* layout,SaXImport* card
) {
	return new SaXManipulateLayout (layout,card);
}

//====================================
// Constructor saxManipulatePath...
//------------------------------------
SaXManipulatePath* SaXPluglib::saxManipulatePath (
	SaXImport* in, int path
) {
	return new SaXManipulatePath (in,path);
}

//====================================
// Constructor saxManipulateMice...
//------------------------------------
SaXManipulateMice* SaXPluglib::saxManipulateMice (
	SaXImport* in, int ptr
) {
	return new SaXManipulateMice (in,ptr);
}

//====================================
// Constructor saxManipulateTablets...
//------------------------------------
SaXManipulateTablets* SaXPluglib::saxManipulateTablets (
	SaXImport* pointer, SaXImport* layout, int id
) {
	return new SaXManipulateTablets (pointer,layout,id);
}

//====================================
// Constructor saxManipulateVNC...
//------------------------------------
SaXManipulateVNC* SaXPluglib::saxManipulateVNC (
	SaXImport* card,SaXImport* pointer,SaXImport* kbd,
	SaXImport* layout,SaXImport* path,int id
) {
	return new SaXManipulateVNC (card,pointer,kbd,layout,path,id);
}

//====================================
// Construct saxManipulateTouchscreens
//------------------------------------
SaXManipulateTouchscreens* SaXPluglib::saxManipulateTouchscreens (
	SaXImport* in, int ptr
) {
	return new SaXManipulateTouchscreens (in,ptr);
}

//====================================
// Construct ManipulateExtensions...
//------------------------------------
SaXManipulateExtensions* SaXPluglib::saxManipulateExtensions (
	SaXImport* in
) {
	return new SaXManipulateExtensions (in);
}

//====================================
// Enable debugging...
//------------------------------------
void SaXPluglib::setDebug ( bool enable ) {
	SaXException exception;
	exception.setDebug (enable); 
}

//====================================
// translate QString to std::string
//------------------------------------
std::string SaXPluglib::STLstring (const QString& qstr) {
	return std::string (qstr.latin1());
}

//====================================
// translate QList to std::deque
//------------------------------------
std::deque<std::string> SaXPluglib::STLdeque (const QList<QString>& qlist) {
	std::deque<std::string> list;
	QString it;
	foreach (it,qlist) {
		list.push_back (STLstring(it));
	}
	return list;
}

//====================================
// translate QDict to std::map
//------------------------------------
std::map<std::string,std::string> SaXPluglib::STLmap (
	const Q3Dict<QString>& qdict
) {
	std::map<std::string,std::string> dict;
	Q3DictIterator<QString> it (qdict);
	for (; it.current(); ++it) {
		dict.insert ( std::make_pair (
			STLstring(it.currentKey()),STLstring(*it.current())
		));
	}
	return dict;
}
} // end namespace
