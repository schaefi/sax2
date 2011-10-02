/**************
FILE          : device.h
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
#ifndef SAX_DEVICE_H
#define SAX_DEVICE_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_INPUT_TOUCHPANEL  "Touchpanel"
#define SAX_INPUT_TABLET      "Tablet"
#define SAX_INPUT_PEN         "Pen"
#define SAX_INPUT_PAD         "Pad"
#define SAX_INPUT_ERASER      "Eraser"
#define SAX_INPUT_TOUCH       "Touch"
#define SAX_INPUT_MOUSE       "Mouse"
#define SAX_INPUT_VNC         "VNC"
#define SAX_INPUT_KEYBOARD    "Keyboard"

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Device manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateDevicesIF : public SaXException {
	public:
	virtual int addDesktopDevice ( void ) = 0;
	virtual int addInputDevice   ( const char* ) = 0;

	public:
	virtual int removeDesktopDevice ( int ) = 0;
	virtual int removeInputDevice   ( int ) = 0;

	public:
	virtual ~SaXManipulateDevicesIF ( void ) { }
};
//====================================
// Class SaXManipulateDevices...
//------------------------------------
/*! \brief SaX2 -  Device manipulator class.
*
* The device manipulator is either used to create a new desktop- or
* a new input-Device. A new input-Device needs to be transformed into a
* standard Keyboard,Mouse,Tablet or Touchscreen device before it can
* be handled by the appropriate manipulator for the device. A new
* desktop-Device instead can be handled by the DesktopManipulator
* directly after the device has been created. To create a new input
* device the device manipulator requires two imports:
*
* - Pointers
* - Layout
*
* whereas the constructor to create a new desktop device requires
* three imports:
*
* - Desktop
* - Card
* - Layout
*
* The following example will show how to use the device manipulator
* for adding a new Tablet to the configuration:
*
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
*     int importID[] = {
*         SAX_POINTERS,
*         SAX_LAYOUT
*     };
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     for (int id=0; id < 2; id++) {
*         SaXImport* import = new SaXImport ( importID[id] );
*         import->setSource ( SAX_SYSTEM_CONFIG );
*         import->doImport();
*         config->addImport (import);
*         section.insert (
*             import->getSectionName(),import
*         );
*     }
*     printf ("Adding new pointer device... ");
*     SaXManipulateDevices dev (
*         section["Pointers"],section["Layout"]
*     );
*     int tabletID = dev.addInputDevice (SAX_INPUT_TABLET);
*     printf ("ID: %d [SAX_INPUT_TABLET] added\n",tabletID);
* 
*     printf ("Setting up tablet data... ");
*     SaXManipulateTablets pointer (
*         section["Pointers"],section["Layout"]
*     );
*     if (pointer.selectPointer (tabletID)) {
*         QList<QString> tabletList = pointer.getTabletList();
*         QString* myTablet = tabletList.at (3);
*         pointer.setTablet( *myTablet );
*         printf ("[%s]\n",myTablet->ascii());
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
class SaXManipulateDevices : public SaXManipulateDevicesIF {
	private:
	SaXImport* mDesktop;
	SaXImport* mCard;
	SaXImport* mInput;
	SaXImport* mLayout;

	private:
	bool desktopHandlingAllowed;
	bool inputHandlingAllowed;

	private:
	void updateLayout (int);

	public:
	int addDesktopDevice ( void );
	int addInputDevice   ( const char* );

	public:
	int removeDesktopDevice ( int );
	int removeInputDevice   ( int );

	public:
	SaXManipulateDevices ( SaXImport*,SaXImport*,SaXImport* );
	SaXManipulateDevices ( SaXImport*,SaXImport* );
};
} // end namespace
#endif
