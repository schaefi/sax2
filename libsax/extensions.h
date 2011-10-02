/**************
FILE          : extensions.h
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
#ifndef SAX_EXTENSIONS_H
#define SAX_EXTENSIONS_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"
#include "device.h"
#include "pointers.h"
#include "keyboard.h"
#include "card.h"
#include "path.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_CREATE_VNC_PWD  "/usr/share/sax/libsax/createVNC.sh"
#define SAX_VNC_MOUSE       0
#define SAX_VNC_KEYBOARD    1

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Extensions class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateExtensionsIF : public SaXException {
	public:
	virtual ~SaXManipulateExtensionsIF ( void ) { }
};
//====================================
// Class SaXManipulateExtensions...
//------------------------------------
/*! \brief SaX2 -  Extensions class
*
* *** Currently no implementation for X11 extensions configuration ***
*/
class SaXManipulateExtensions : public SaXManipulateExtensionsIF {
	private:
	SaXImport* mImport;
	int mExtensions;

	public:
	SaXManipulateExtensions ( SaXImport* );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  VNC class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateVNCIF : public SaXManipulateCard {
	public:
	virtual void addVNCKeyboard    ( void ) = 0;
	virtual void addVNCMouse       ( void ) = 0;
	virtual void removeVNCMouse    ( void ) = 0;
	virtual void removeVNCKeyboard ( void ) = 0;

	public:
	virtual void enableVNC   ( void ) = 0;
	virtual void disableVNC  ( void ) = 0;
	virtual void setPassword ( const QString& ) = 0;
	virtual void enablePasswordProtection  ( const QString& ) = 0;
	virtual void disablePasswordProtection ( void ) = 0;
	virtual void enableHTTPAccess  ( int = 5800 ) = 0;
	virtual void disableHTTPAccess ( void ) = 0;
	virtual void allowMultipleConnections ( bool = true ) = 0;
	virtual int  getHTTPPort ( void ) = 0;

	public:
	virtual bool isVNCEnabled           ( void ) = 0;
	virtual bool isHTTPAccessEnabled    ( void ) = 0;
	virtual bool isMultiConnectEnabled  ( void ) = 0;
	virtual bool isPwdProtectionEnabled ( void ) = 0;

	public:
	virtual ~SaXManipulateVNCIF ( void ) { }

	protected:
	SaXManipulateVNCIF (SaXImport* in,int id= 0) : SaXManipulateCard (in,id) { }
};
//====================================
// Class SaXManipulateVNC...
//------------------------------------
/*! \brief SaX2 -  VNC class.
*
* The VNC manipulator requires five import objects (Card,Pointers,Keyboard,
* Layout and Path) to become created. Once created the manipulator object
* can enable/disable the exporting of the display via VNC protocol. The
* following example demonstrate how to enable/disable VNC for the current
* configuration:
*
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
*     int importID[] = {
*         SAX_CARD,
*         SAX_POINTERS,
*         SAX_KEYBOARD,
*         SAX_LAYOUT,
*         SAX_PATH
*     };
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     for (int id=0; id<5; id++) {
*         SaXImport* import = new SaXImport ( importID[id] );
*         import->setSource ( SAX_SYSTEM_CONFIG );
*         import->doImport();
*         config->addImport (import);
*         section.insert (
*             import->getSectionName(),import
*         );
*     }
*     printf ("Exporting display for VNC access...\n");
*     SaXManipulateVNC mVNC (
*         section["Card"],section["Pointers"],section["Keyboard"],
*         section["Layout"],section["Path"]
*     );
*     if (! mVNC.VNCEnabled()) {
*         mVNC.enableVNC();
*         mVNC.addVNCKeyboard();
*         mVNC.addVNCMouse();
*     }
*     printf ("writing configuration\n");
*     config->setMode (SAX_MERGE);
*     if ( ! config->createConfiguration() ) {
*         printf ("%s\n",config->errorString().ascii());
*         printf ("%s\n",config->getParseErrorValue().ascii());
*         return 1;
*     }
*     return (0);
* }
* \endcode
*/
class SaXManipulateVNC : public SaXManipulateVNCIF {
	private:
	int mVNCMouse;
	int mVNCKeyboard;

	private:
	SaXManipulateDevices*  mManipKeyboard;
	SaXManipulateDevices*  mManipPointer;
	SaXManipulatePath*     mManipModules;
	SaXImport*             mLayout;
	SaXImport*             mPointer;
	SaXImport*             mKeyboard;

	private:
	int searchVNCDevice    ( int );

	public:
	void addVNCKeyboard    ( void );
	void addVNCMouse       ( void );
	void removeVNCMouse    ( void );
	void removeVNCKeyboard ( void );

	public:
	void enableVNC   ( void );
	void disableVNC  ( void );
	void setPassword ( const QString& );
	void enablePasswordProtection  ( const QString& );
	void disablePasswordProtection ( void );
	void enableHTTPAccess  ( int = 5800 );
	void disableHTTPAccess ( void );
	void allowMultipleConnections ( bool = true );
	int  getHTTPPort ( void );

	public:
	bool isVNCEnabled           ( void );
	bool isHTTPAccessEnabled    ( void );
	bool isMultiConnectEnabled  ( void );
	bool isPwdProtectionEnabled ( void );

	public:
	SaXManipulateVNC (
		SaXImport*, SaXImport*, SaXImport*,
		SaXImport*, SaXImport*, int = 0
	);
};
} // end namespace
#endif
