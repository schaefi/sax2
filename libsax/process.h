/**************
FILE          : process.h
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
#ifndef SAX_PROCESS_H
#define SAX_PROCESS_H 1

//====================================
// Includes...
//------------------------------------
#include <qregexp.h>
#include <qfile.h>
#include <sys/types.h>
#include <dirent.h>

#include "storage.h"
#include "exception.h"
#include "processcall.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define ISAX               "/usr/sbin/isax"
#define SYSP               "/usr/sbin/sysp"
#define PROF               "/usr/share/sax/libsax/createPRO.pl"
#define META               "/usr/share/sax/libsax/createMTA.pl"

#define CDBDIR             "/usr/share/sax/api/data/cdb/"
#define DATADIR            "/usr/share/sax/api/data/"
#define CDBCARDS           CDBDIR "Cards"
#define CDBMONITORS        CDBDIR "Monitors"
#define CDBPOINTERS        CDBDIR "Pointers"
#define CDBPENS            CDBDIR "Pens"
#define CDBPADS            CDBDIR "Pads"
#define CDBTABLETS         CDBDIR "Tablets"
#define CDBTOUCHERS        CDBDIR "Touchscreens"
#define CDBCARDMODULES     DATADIR "CardModules"
#define CDBTABLETMODULES   DATADIR "TabletModules"

#define SAX_CARD           4
#define SAX_DESKTOP        5
#define SAX_POINTERS       6
#define SAX_KEYBOARD       7
#define SAX_LAYOUT         8
#define SAX_PATH           9
#define SAX_EXTENSIONS     10
#define SAX_ISAX           11
#define SAX_SYSP           12
#define SAX_CDB            13
#define SAX_PROF           14
#define SAX_META           15

#define CDB_CARDS          16
#define CDB_MONITORS       17
#define CDB_PENS           18
#define CDB_POINTERS       19
#define CDB_TABLETS        20
#define CDB_TOUCHERS       21
#define CDB_CARDMODULES    22
#define CDB_TABLETMODULES  23
#define CDB_PADS           24

#define SYSP_MOUSE         30
#define SYSP_KEYBOARD      31
#define SYSP_CARD          32
#define SYSP_DESKTOP       33
#define SYSP_3D            34

#define MAX_LINE_LENGTH    8192

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  SaX process execution class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXProcessIF : public SaXStorage {
	public:
	virtual void start ( QList<const char*>,int = SAX_ISAX ) = 0;
	virtual void start ( int ) = 0;

	public:
	virtual ~SaXProcessIF ( void ) { }
};
//====================================
// Class SaXProcess...
//------------------------------------
/*! \brief SaX2 -  SaX process execution class.
*
* The SaXProcess class provides an interface to call external programs
* including output parsing. Concerning libsax only the output formats
* of CDB files isax and sysp calls are handled. The internal structure
* is based on SaXStorage objects. All access and modifaction members are
* provided by the SaXStorage class.
*
* \code
* #include <sax/sax.h>
*
* QList<const char*> sysp_options;
* sysp_options.append ( "-q" );
* sysp_options.append ( "server" );
* SaXProcess* proc = new SaXProcess ();
* proc->start ( sysp_options , SAX_SYSP );
* if (proc->getItem("VID")) {
*     printf ("%s\n",proc->getItem("VID").ascii());
* }
* \endcode
*/
class SaXProcess : public SaXProcessIF {
	private:
	SaXProcessCall* mProc;

	private:
	void storeData     ( void );
	void storeDataSysp ( void );
	void storeDataCDB  ( int  );
	void storeDataSYS  ( int  );

	public:
	void start ( QList<const char*>, int = SAX_ISAX );
	void start ( int );

	public:
	SaXProcess ( void );
};
} // end namespace
#endif
