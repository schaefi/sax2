/**************
FILE          : config.h
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
#ifndef SAX_CONFIG_H
#define SAX_CONFIG_H 1

//====================================
// Includes...
//------------------------------------
#include <stdarg.h>
#include <dirent.h>
#include <stdlib.h>
extern "C" {
#include "xf86Parser.h"
}

#include "export.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_NEW         20
#define SAX_MERGE       21
#define SAX_CREATE_API  "/usr/share/sax/libsax/createAPI.pl"
#define SAX_TEST_CONFIG "/usr/share/sax/libsax/createTST.pl"
#define SAX_API_FILE    "/var/lib/sax/apidata"
#define SAX_API_CONFIG  "/var/lib/sax/xorg.conf"
#define SAX_API_MD5     "/var/lib/sax/xorg.conf.md5"
#define SAX_SYS_MD5     "/etc/X11/xorg.conf.md5"
#define SAX_SYS_CONFIG  "/etc/X11/xorg.conf"
#define SAX_SYS_CSAVED  "/etc/X11/xorg.conf.saxsave"
#define SAX_X11_LOADER  "/usr/X11R6/bin/Xorg"
#define SAX_XFINE_CACHE "/var/cache/xfine"
#define SAX_MD5_SUM     "/usr/bin/md5sum"
#define CONFPATH        "%A,%R,/etc/%R,%P/etc/X11/%R,%E,%F,/etc/X11/%F"
#define CONFERRORLINE   80

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 - Configuration class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXConfigIF : public SaXException {
	public:
	virtual void setParseErrorValue ( char* ) = 0;
	virtual void setParseError ( char* ) = 0;
	virtual void addImport ( SaXImport* ) = 0;
	virtual void setMode ( int ) = 0;

	public:
	virtual QString getParseErrorValue ( void ) = 0;
	virtual QString getParseError ( void ) = 0;

	public:
	virtual bool createConfiguration  ( void ) = 0;
	virtual void commitConfiguration  ( void ) = 0;
	virtual int  testConfiguration    ( void ) = 0;
	virtual bool isChecksumOK         ( void ) = 0;

	public:
	virtual ~SaXConfigIF ( void ) { }
};
//====================================
// Class SaXConfig...
//------------------------------------
/*! \brief SaX2 - Configuration class
*
* A SaXConfig object is mainly used as a container for SaXImport objects.
* Different import objects describing a complete configuration or only parts
* of it can be added using the addImport() method. A complete new
* configuration requires the following import ID's to be added: 
*
* - SAX_CARD
* - SAX_DESKTOP
* - SAX_POINTERS
* - SAX_KEYBOARD
* - SAX_LAYOUT
* - SAX_PATH
* - SAX_EXTENSIONS
*
* If not all of these are provided it is necessary to set the config mode
* to SAX_MERGE using the setMode() method otherwise the configuration will be
* broken. If merging is used you may recognize problems while trying to remove
* complete devices. For example you want to remove one mouse out of two but
* your changes do not have any effect. This is because while merging the
* currently existing information is still there. As result of that you need to
* write a complete new configuration using the SAX_NEW mode to prevent existing
* sections from beeing imported again.
*
* Calling createConfiguration() will create a preliminary configuration file
* named /var/lib/sax/xorg.conf. libsax will automatically check the syntax of
* the new created file and the programmer is able to get possible syntax errors
* using the getParseError* methods
* 
* \code
* #include <sax/sax.h>
*
* int importID[7] = {
*     SAX_CARD,
*     SAX_DESKTOP,
*     SAX_POINTERS,
*     SAX_KEYBOARD,
*     SAX_LAYOUT,
*     SAX_PATH,
*     SAX_EXTENSIONS
* };
* SaXConfig* config = new SaXConfig;
* for (int id=0; id < 7; id++) {
*     SaXImport* import = new SaXImport ( importID[id] );
*     import->doImport();
*     config->addImport (import);
* }
* config->setMode (SAX_NEW);
* if ( ! config->createConfiguration() ) {
*     printf ("%s\n",config->getParseErrorValue().ascii());
* }
* \endcode
*/
class SaXConfig : public SaXConfigIF {
	private:
	QList <SaXImport*> mImportList;
	SaXImport* mDesktop;
	int configMode;

	private:
	QString* mParseErrorString;
	QString* mParseErrorValue;

	private:
	bool gotCard;
	bool gotDesktop;
	bool gotPointers;
	bool gotKeyboard;
	bool gotLayout;
	bool gotPath;
	bool gotExtensions;

	private:
	void enableXFineCache (bool = true);
	void removeXFineCache (void);

	public:
	void setParseErrorValue ( char* );
	void setParseError ( char* );
	void addImport ( SaXImport* );
	void setMode ( int );
	
	public:
	QString getParseErrorValue ( void );
	QString getParseError ( void );

	public:
	bool createConfiguration  ( void );
	void commitConfiguration  ( void );
	int  testConfiguration    ( void );
	bool isChecksumOK         ( void );

	public:
	SaXConfig ( int = SAX_MERGE );
};
} // end namespace
#endif
