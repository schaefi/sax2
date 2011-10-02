/**************
FILE          : interface.h
***************
PROJECT       : SaX2 - library interface [header]
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
#ifndef SAX_INTERFACE_H
#define SAX_INTERFACE_H 1

//====================================
// Includes STL...
//------------------------------------
#include <deque>
#include <string>
#include <map>

//====================================
// Includes...
//------------------------------------
#include <q3dict.h>
#include <qlist.h>
#include <errno.h>

#include "sax.h"

typedef void* (*Factory) ();
//====================================
// Entrypoint...
//------------------------------------
/*! \brief SaX2 - Entrypoint structure to find symbol "entrypoint"
*
* To be able to load libsax via dlopen it is needed to provide
* a global entrypoint symbol which can be found using dlsym().
* Once the entrypoint has been created the structure variable
* provides access to a function named factory() which creates a
* global saxPluglib object while called. Using the saxPluglib
* object will provide access to all constructors of all
* interface class types implemented in libsax. See the following
* loader.cpp example for information how to dynamically load libsax
*
* \code
* #include <stdio.h>
* #include <stdlib.h>
* #include <dlfcn.h>
*
* #include <sax/sax.h>
*
* using namespace LML;
*
* saxPluglib* loadLibrary (void) {
*     void* handle = dlopen (
*         "/usr/lib/libsax.so", RTLD_LAZY | RTLD_GLOBAL
*     );
*     if (! handle) {
*         printf ("%s\n", dlerror ());
*         exit ( EXIT_FAILURE );
*     }
*     EntryPoint* entrypoint = (EntryPoint*)dlsym (handle, "entrypoint");
*     if (! entrypoint) {
*         printf ("%s\n", dlerror ());
*         exit ( EXIT_FAILURE );
*     }
*     saxPluglib* LiMal = (saxPluglib*)entrypoint->factory();
*     return LiMal;
* }
*
* int main (void) {
*    saxPluglib* LiMal = loadLibrary();
*    LiMal->setDebug();
*    int importID[7] = {
*         SAX_CARD,
*         SAX_DESKTOP,
*         SAX_POINTERS,
*         SAX_KEYBOARD,
*         SAX_LAYOUT,
*         SAX_PATH,
*         SAX_EXTENSIONS
*     };
*     QList<SaXImport*> section;
*     SaXConfig* config = LiMal->saxConfig();
*     for (int id=0; id<7; id++) {
*         printf ("Importing data...\n");
*         SaXImport* import = LiMal->saxImport (importID[id]);
*         import->setSource (SAX_AUTO_PROBE);
*         import->doImport();
*         config->addImport (import);
*         section.append (import);
*     }
*     printf ("writing configuration\n");
*     config->setMode (SAX_NEW);
*     if ( ! config->createConfiguration() ) {
*         printf ("%s\n",config->getParseErrorValue().ascii());
*     }
*     delete LiMal;
*     return 0;
* }
* \endcode
*
* Important Note:
*
* Because of the fact that libsax is using Qt a wide set of Q-Objects are
* created. To know about the constructors of those Q-Objects it is needed
* to link against qt-mt while compiling the loader. This of course results
* in a loader binary which is linked against qt-mt which you should have
* in mind and first check if that fits into your needs:
*
* \code
* g++ loader.cpp -o loader \
*     -Wall -O2 -I$QTDIR/include -I/usr/X11/include \
*     -L$QTDIR/lib/ -ldl -lqt-mt
* \endcode
*/
struct EntryPoint {
	const char* name;
	const char* version;
	Factory factory;
};

//====================================
// use namespace SaX for interface
//------------------------------------
using namespace SaX;

//====================================
// namespace LML
//------------------------------------
/*! \brief LML -  LiMal namespace.
*
* This namespace contains the factory for all constructors
* implemented within libsax. LML should be exclusively used
* for dynamic loading of libsax
*/
namespace LML {
//====================================
// Class saxPluglib abstract...
//------------------------------------
/*! \brief SaX2 - Abstract interface class saxPluglib.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class saxPluglib {
	public:
	virtual SaXInit*    saxInit   ( void ) = 0;
	virtual SaXConfig*  saxConfig ( int = SAX_MERGE ) = 0;
	virtual SaXImport*  saxImport ( int ) = 0;
	virtual SaXImportSysp* saxImportSysp ( int ) = 0;
	virtual SaXImportCDB*  saxImportCDB  ( int ) = 0;
	virtual SaXImportProfile* saxImportProfile ( const char* ) = 0;

	public:
	virtual SaXManipulateDesktop* saxManipulateDesktop (
		SaXImport*,SaXImport*,SaXImport*,int = 0
	) = 0;
	virtual SaXManipulateCard* saxManipulateCard (
		SaXImport*,int = 0
	) = 0;
	virtual SaXManipulateKeyboard* saxManipulateKeyboard (
		SaXImport*,int = 0
	) = 0;
	virtual SaXManipulateDevices* saxManipulateDevices (
		SaXImport*,SaXImport*,SaXImport*
	) = 0;
	virtual SaXManipulateDevices* saxManipulateDevices (
		SaXImport*,SaXImport*
	) = 0;
	virtual SaXManipulateLayout* saxManipulateLayout (
		SaXImport*,SaXImport*
	) = 0;
	virtual SaXManipulatePath* saxManipulatePath (
		SaXImport*,int = 0
	) = 0;
	virtual SaXManipulateMice* saxManipulateMice (
		SaXImport*,int = 1
	) = 0;
	virtual SaXManipulateTablets* saxManipulateTablets (
		SaXImport*,SaXImport*,int = 1
	) = 0;
	virtual SaXManipulateVNC* saxManipulateVNC (
		SaXImport*,SaXImport*,SaXImport*,SaXImport*,SaXImport*,int = 0
	) = 0;
	virtual SaXManipulateTouchscreens* saxManipulateTouchscreens (
		SaXImport*,int = 1
	) = 0;
	virtual SaXManipulateExtensions* saxManipulateExtensions (
		SaXImport*
	) = 0;

	public:
	virtual std::string STLstring ( const QString& ) = 0;
	virtual std::deque<std::string> STLdeque ( const QList<QString>& ) = 0;
	virtual std::map<std::string,std::string> STLmap (
		const Q3Dict<QString>&
	) = 0;

	public:
	virtual void setDebug  ( bool=true ) = 0;

	public:
	virtual ~saxPluglib ( void ) { }
};
//====================================
// Class SaXPluglib...
//------------------------------------
/*! \brief SaX2 - Interface class saxPluglib.
*
* The SaXPluglib class is a wrapper class for all constructors
* implemented in libsax. An object of this class is normally only
* used when libsax has been loaded with dlopen.
*/
class SaXPluglib : public saxPluglib {
	public:
	SaXPluglib ( void );
	static SaXPluglib* factory ( void );

	public:
	SaXInit*    saxInit   ( void );
	SaXConfig*  saxConfig ( int = SAX_MERGE );
	SaXImport*  saxImport ( int );
	SaXImportSysp* saxImportSysp ( int );
	SaXImportCDB*  saxImportCDB  ( int );
	SaXImportProfile* saxImportProfile ( const char* );

	public:
	SaXManipulateDesktop* saxManipulateDesktop (
		SaXImport*,SaXImport*,SaXImport*,int = 0
	);
	SaXManipulateCard* saxManipulateCard (
		SaXImport*,int = 0
	);
	SaXManipulateKeyboard* saxManipulateKeyboard (
		SaXImport*,int = 0
	);
	SaXManipulateDevices* saxManipulateDevices (
		SaXImport*,SaXImport*,SaXImport*
	);
	SaXManipulateDevices* saxManipulateDevices (
		SaXImport*,SaXImport*
	);
	SaXManipulateLayout* saxManipulateLayout (
		SaXImport*,SaXImport*
	);
	SaXManipulatePath* saxManipulatePath (
		SaXImport*,int = 0
	);
	SaXManipulateMice* saxManipulateMice (
		SaXImport*,int = 1
	);
	SaXManipulateTablets* saxManipulateTablets (
		SaXImport*,SaXImport*,int = 1
	);
	SaXManipulateVNC* saxManipulateVNC (
		SaXImport*,SaXImport*,SaXImport*,SaXImport*,SaXImport*,int = 0
	);
	SaXManipulateTouchscreens* saxManipulateTouchscreens (
		SaXImport*,int = 1
	);
	SaXManipulateExtensions* saxManipulateExtensions (
		SaXImport*
	);

	public:
	std::string STLstring ( const QString& );
	std::deque<std::string> STLdeque ( const QList<QString>& );
	std::map<std::string,std::string> STLmap ( const Q3Dict<QString>& );

	public:
	void setDebug  ( bool=true );
};
} // end namespace
#endif
