/**************
FILE          : import.h
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
#ifndef SAX_IMPORT_H
#define SAX_IMPORT_H 1

//====================================
// Includes...
//------------------------------------
#include <qregexp.h>
#include <qfileinfo.h>

#include "storage.h"
#include "exception.h"
#include "process.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_AUTO_PROBE     1
#define SAX_SYSTEM_CONFIG  3

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Import class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXImportIF : public SaXProcess {
	public:
	virtual void doImport  ( void )   = 0;
	virtual void setSource ( int  )   = 0;
	virtual int getSectionID ( void ) = 0;
	virtual QString getSectionName ( void ) = 0;

	public:
	virtual ~SaXImportIF ( void ) { }
};
//====================================
// Class SaXImport...
//------------------------------------
/*! \brief SaX2 -  Import class
*
* The SaXImport class provides an abstraction for the ISaX interface
* layer. The class inherits from SaXProcess which itself inherits from
* SaXStorage. the data is obtained from an isax process call and
* is stored into the protected area of a storage object. ISaX is the
* interface between the engine which is responsible to create or modify
* X11 configurations and the client side which normaly builds a
* GUI for user interactions concerning X11 issues. The ISaX interface is
* used for input and output data and is therefore the major structure to
* read, create and modify X11 configurations. The SaXImport class will cover
* only the first part which is to read from the interface. A SaXImport object
* will be constructed with one parameter which defines the section to import.
* there are seven ISAX sections available:
*
* - SAX_CARD\n
*   Provides information about the graphics card. This includes everything
*   refering to the device section of the X11 configuration file.
*
* - SAX_DESKTOP\n
*   Provides information about the monitor the resolution, colordepth, etc.
*   All of these surrounds the desktop and refers to the sections Monitor,
*   Modes and Screen of the X11 configuration file.
*
* - SAX_POINTERS\n
*   Provides information about all input devices except the core keyboard. This
*   normally includes information about the core pointer, touchpads, tablets,
*   etc. The information refers to the approprate InputDevice sections of the
*   X11 configuration file.
*
* - SAX_KEYBOARD\n
*   Provides information about the core keyboard. This information refers to
*   the approprate InputDevice section of the X11 configuration file.
*
* - SAX_LAYOUT\n
*   Provides information about the screen layout. The so called ServerLayout
*   section of the X11 configuration file defines how things fit together.
*   This includes information about mulithead arrangement as well as
*   information about the layout of input devices.
*
* - SAX_PATH\n
*   Provides information about FontPaths, server modules and flags. This
*   data refers to the sections Files Module and ServerFlags of the
*   X11 configuration file
*
* - SAX_EXTENSIONS\n
*   Provides information about the X11 extensions. The Extensions section
*   of the X11 configuration file is used to enable disable external
*   extensions.
*
* Example:
* \code
* #include <sax/sax.h>
*
* SaXImport* card = new SaXImport (SAX_CARD);
* card->doImport();
*
* if (card->getItem("Vendor")) {
*     printf ("%s\n",card->getItem("Vendor").ascii());
* }
* \endcode
*/
class SaXImport : public SaXImportIF {
	private:
	QList<const char*> mOptions;
	QString mSectionName;
	int mSection;

	public:
	void doImport  ( void );
	void setSource ( int  );
	int getSectionID ( void );
	QString getSectionName ( void );

	public:
	SaXImport ( int );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Import SYSP class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXImportSyspIF : public SaXProcess {
	public:
	virtual void doImport    ( void ) = 0;
	virtual int getSectionID ( void ) = 0;
	virtual QString getSectionName ( void ) = 0;

	public:
	virtual ~SaXImportSyspIF ( void ) { }
};
//====================================
// Class SaXImportSysp...
//------------------------------------
/*! \brief SaX2 -  Import SYSP class.
*
* The SaXImportSysp class provides an abstraction for the Sysp interface
* layer. The class inherits from SaXProcess which itself inherits from
* SaXStorage. the data is obtained from a sysp process call and
* is stored into the protected area of a storage object. sysp is the
* system profiler used with SaX2. The system profiler provides information
* about the hardware only. Stuff like PCI ID's, card memory, etc. is
* handled within Sysp. Sysp is a read only interface and according to this
* the class SaXImportSysp can only be used to obtain information.
* A SaXImportSysp object will be constructed with one parameter which defines
* the section to import. there are five SYSP sections available:
*
* - SYSP_MOUSE\n
*   Provides hardware information about mice. This includes the protocol
*   and the device as well as information about buttons and wheels. The
*   information is usefull in combination with an ISAX imported
*   SaXImport (SAX_POINTERS) object
*
* - SYSP_KEYBOARD\n
*   Provides hardware information about the core keyboard. This includes
*   the model and layout as well as information about XKB variants and/or
*   options The information is usefull in combination with an ISAX imported
*   SaXImport(SAX_KEYBOARD) object
*
* - SYSP_CARD\n
*   Provides hardware information about the graphics cards. This includes
*   the location on the PCI/AGP Bus as well as information about the vendor
*   specific PCI ID's and the needed driver information. The information is
*   usefull in combination with an ISAX imported SaXImport(SAX_CARD) object
*
* - SYSP_DESKTOP\n
*   Provides hardware information about the connected monitors. This includes
*   the data from the DDC record if available as well as information about
*   the location of the primary VGA and the name of the monitor if it has
*   been found in the CDB. The information is usefull in combination with an
*   ISAX imported SaXImport(SAX_DESKTOP) object
*
* - SYSP_3D\n
*   Provides hardware information about the 3D capabilities of the card.
*   This includes information about needed packages and some flags for
*   special cases like nvidia binary only stuff. The information is usefull
*   in combination with an ISAX imported SaXImport(SAX_DESKTOP) object 
*
* Example:
* \code
* #include <sax/sax.h>
*
* SaXImportSysp* card = new SaXImportSysp (SYSP_CARD);
* card->doImport();
*
* if (card->getItem("VID")) {
*     printf ("%s\n",card->getItem("VID").ascii());
* }
* \endcode
*/
class SaXImportSysp : public SaXImportSyspIF {
	private:
	QList<const char*> mOptions;
	QString mSectionName;
	int mSection;

	public:
	void doImport    ( void );
	int getSectionID ( void );
	QString getSectionName ( void );

	public:
	SaXImportSysp ( int );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Import CDB class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXImportCDBIF : public SaXProcess {
	public:
	virtual void doImport ( void ) = 0;

	public:
	virtual ~SaXImportCDBIF ( void ) { };
};
//====================================
// Class SaXImportCDB...
//------------------------------------
/*! \brief SaX2 -  Import CDB class.
*
* The SaXImportCDB class provides an abstraction for the CDB interface
* layer. The class inherits from SaXProcess which itself inherits from
* SaXStorage. the data is obtained from files which are created as
* export from a database and stored as part of the SaX2 engine. The CDB
* (Component Data-Base) provides high level information about hardware.
* the SaX2 CDB interface is a read only interface and according to this
* the class SaXImportCDB can only be used to obtain high level information
* about a various field of X11 related hardware. A SaXImportCDB object will
* be constructed with one parameter which defines the file ID to become
* imported. there are six CDB file ID's available:
*
* - CDB_CARDS\n
*   Provides information about supported graphics cards refering to the
*   file cdb/Cards. Each data record contains information about the
*   driver module, profiles and extensions for the use in 2D and 3D
*   environments. The information is usefull in combination with an ISAX
*   imported SaXImport(SAX_CARD) object
*
* - CDB_MONITORS\n
*   Provides information about supported monitors refering to the file
*   cdb/Monitors. Each data record contains information about the sync
*   range, profile, DDC ID, Modelines and options. The information is
*   usefull in combination with an ISAX imported SaXImport(SAX_DESKTOP)
*   object
*
* - CDB_TABLETS\n
*   Provides information about supported tablets refering to the file
*   cdb/Tablets. Each data record contains information about the various
*   options to configure the core tablet without peripherals. The information
*   is usefull in combination with an ISAX imported SaXImport(SAX_POINTERS)
*   object
*
* - CDB_PENS\n
*   Provides information about supported tablet pens refering to the
*   file cdb/Pens. Each data record contains the various options to
*   configure the pen. The information is usefull in combination with an
*   ISAX imported SaXImport(SAX_POINTERS) object
*
* - CDB_PADS\n
*   Provides information about supported tablet pads refering to the
*   file cdb/Pads. Each data record contains the various options to
*   configure the pad. The information is usefull in combination with an
*   ISAX imported SaXImport(SAX_POINTERS) object
*
* - CDB_POINTERS\n
*   Provides information about the supported mice refering to the file
*   cdb/Pointers. Each data record contains at least information about
*   the protocol and device but there are several special devices which
*   need lots of additional options which are part of the data record too.
*   The information is usefull in combination with an ISAX imported
*   SaXImport(SAX_POINTERS) object
*
* - CDB_TOUCHERS\n
*   Provides information about the supported touch screens refering to the
*   file cdb/Touchscreens. Each data record contains information about the
*   various options to configure a touch screen. The information is usefull
*   in combination with an ISAX imported SaXImport(SAX_POINTERS) object
*
* Example:
* \code
* #include <sax/sax.h>
*
* SaXImportCDB* cards = new SaXImportCDB (CDB_CARDS);
* cards->doImport();
*
* QDict< QDict<QString> > data = cards->getTablePointerCDB ();
* QDictIterator< QDict<QString> > n (data);
* for (; n.current(); ++n) {
*     printf ("%s\n",n.currentKey().ascii());
* }
* \endcode
*/
class SaXImportCDB : public SaXImportCDBIF {
	private:
	int mID;

	public:
	void doImport ( void );

	public:
	SaXImportCDB ( int );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Import Profile class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXImportProfileIF : public SaXProcess {
	public:
	virtual void setDevice ( int  ) = 0;
	virtual void doImport  ( void ) = 0;
	virtual SaXImport* getImport ( int = SAX_CARD ) = 0;

	public:
	virtual ~SaXImportProfileIF ( void ) { }
};
//====================================
// Class SaXImportProfile...
//------------------------------------
/*! \brief SaX2 -  Import Profile class.
*
* The SaXImportProfile class provides an abstraction for the ISaX interface
* layer concerning the profiles to become applied. The class inherits from
* SaXProcess which itself inherits from SaXStorage. the data is obtained from
* a /usr/share/sax/libsax/createPRO.pl script call which is able to read the given
* profile file and import the data into the automatic configuration suggestion.
* After this it will export the changes made by the profile and this data is
* stored into the protected area of a storage object. Because of the fact
* a profile can change data in any section the SaXImportProfile class will
* split the data into the appropriate sections and made it accessable using
* the getImport() method. A SaXImportProfile object will be constructed with
* one parameter which defines the profile filename to import.
*/
class SaXImportProfile : public SaXImportProfileIF {
	private:
	QList<const char*> mOptions;
	SaXImport*  mLayout;
	SaXImport*  mDesktop;
	SaXImport*  mCard;
	SaXImport*  mPointers;

	private:
	void splitImport ( void );

	public:
	void setDevice ( int  );
	void doImport  ( void );
	SaXImport* getImport ( int = SAX_CARD );
	
	public:
	SaXImportProfile ( const QString& );
};
} // end namespace
#endif
