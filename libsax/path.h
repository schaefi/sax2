/**************
FILE          : path.h
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
#ifndef SAX_PATH_H
#define SAX_PATH_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"

namespace SaX {
//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Path class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulatePathIF : public SaXException {
	public:
	virtual void setFontPath ( const QString& ) = 0;
	virtual void addFontPath ( const QString& ) = 0;
	virtual void removeFontPath ( const QString& ) = 0;

	public:
	virtual void setLoadableModule ( const QString& ) = 0;
	virtual void addLoadableModule ( const QString& ) = 0;
	virtual void removeLoadableModule ( const QString& ) = 0;
	virtual void setDisableModule ( const QString& ) = 0;
	virtual void addDisableModule ( const QString& ) = 0;
	virtual void removeDisableModule ( const QString& ) = 0;

	public:
	virtual void setServerFlag ( const QString& ) = 0;
	virtual void addServerFlag ( const QString& ) = 0;
	virtual void removeServerFlag ( const QString& ) = 0;

	public:
	virtual QList<QString> getFontPaths   ( void ) = 0;
	virtual QList<QString> getModules     ( void ) = 0;
	virtual QList<QString> getServerFlags ( void ) = 0;

	public:
	virtual ~SaXManipulatePathIF ( void ) { }
};
//====================================
// Class SaXManipulatePath...
//------------------------------------
/*! \brief SaX2 -  Path class.
*
* The path manipulator requires one import object (Path) to become
* created. Once created the manipulator object is able to get/set information
* related to server-flags server-modules and search paths. The following
* example demonstrates how to add a server flag to the configuration. The
* DontVTSwitch server flag disallows the use of the  Ctrl+Alt+Fn  sequence
* (where  Fn refers  to one of the numbered function keys).  That sequence is
* normally used to switch to another "virtual terminal" on operating systems
* that  have  this  feature.   When  this  option is enabled, that key
* sequence has no special meaning and is  passed to clients.
*
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
* 
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     SaXImport* import = new SaXImport ( SAX_PATH );
*     import->setSource ( SAX_SYSTEM_CONFIG );
*     import->doImport();
*     config->addImport (import);
*     section.insert (
*         import->getSectionName(),import
*     );
*     printf ("Disable VT switching...\n");
*     SaXManipulatePath mPath (
*         section["Path"]
*     );
*     mPath.addServerFlag ("DontVTSwitch");
* 
*     printf ("Writing configuration\n");
*     config->setMode (SAX_MERGE);
*     if ( ! config->createConfiguration() ) {
*         printf ("%s\n",config->errorString().ascii());
*         printf ("%s\n",config->getParseErrorValue().ascii());
*         return 1;
*     }
*     return 0;
* }
* \endcode
*/
class SaXManipulatePath : public SaXManipulatePathIF {
	private:
	SaXImport* mImport;
	int mPath;

	private:
	QList<QString> createList ( const QString& );

	public:
	void setFontPath ( const QString& );
	void addFontPath ( const QString& );
	void removeFontPath ( const QString& );

	public:
	void setLoadableModule ( const QString& );
	void addLoadableModule ( const QString& );
	void removeLoadableModule ( const QString& );
	void setDisableModule ( const QString& );
	void addDisableModule ( const QString& );
	void removeDisableModule ( const QString& );

	public:
	void setServerFlag ( const QString& );
	void addServerFlag ( const QString& );
	void removeServerFlag ( const QString& );

	public:
	QList<QString> getFontPaths   ( void );
	QList<QString> getModules     ( void );
	QList<QString> getServerFlags ( void );

	public:
	SaXManipulatePath ( SaXImport*, int = 0 );
};
} // end namespace
#endif
