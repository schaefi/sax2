/**************
FILE          : file.h
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
#ifndef SAX_FILE_H
#define SAX_FILE_H 1

//====================================
// Includes
//------------------------------------
#include <qfile.h>
#include <q3dict.h>
#include <qstringlist.h>

#include "exception.h"

namespace SaX {
//====================================
// Defines
//------------------------------------
#define MAX_LINE_LENGTH    8192

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 - File class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXFileIF : public SaXException {
	public:
	virtual Q3Dict<QString> readDict    ( void ) = 0;
	virtual Q3Dict<QString> getDataDict ( void ) = 0;

	public:
	virtual ~SaXFileIF ( void ) { }
};

//====================================
// Class SaXFile
//------------------------------------
/*! \brief SaX2 - File class.
*
* SaXFile is used to read the SYSP map files. There are several
* mapping tables which could be imported using a SaXFile object.
* Currently only the Driver.map file is used within the 3D
* enable/disable methods. The following example illustrates
* how to use SaXFile:
*
* \code
* #include <sax/sax.h>
*
* #define MAP_FILE "..."
*
* SaXFile mapHandle ( MAP_FILE );
* QDict<QString> driverMap = mapHandle.readDict();
* \endcode
*/
class SaXFile : public SaXFileIF {
	private:
	QFile* mHandle;
	Q3Dict<QString> mDict;

	private:
	void fileOpen ( void );
	
	public:
	Q3Dict<QString> readDict    ( void );
	Q3Dict<QString> getDataDict ( void );

	public:
	SaXFile ( const QString& );
};
} // end namespace
#endif
