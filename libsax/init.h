/**************
FILE          : init.h
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
#ifndef SAX_INIT_H
#define SAX_INIT_H 1

//====================================
// Includes...
//------------------------------------
#include <stdlib.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include "processcall.h"
#include "exception.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_INIT        "/usr/share/sax/init.pl"
#define CACHE_CONFIG    "/var/cache/sax/files/config"
#define GET_PRIMARY     "/usr/sbin/getPrimary"
#define GET_BASH        "/bin/bash"

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Init class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXInitIF : public SaXException {
	public:
	virtual bool needInit (void) = 0;
	virtual void ignoreProfile (void) = 0;
	virtual void setPrimaryChip (void) = 0;
	virtual void setValidBusID (void) = 0;

	public:
	virtual void doInit (void) = 0;

	public:
	virtual ~SaXInitIF ( void ) { }
};
//====================================
// Class SaXInit...
//------------------------------------
/*! \brief SaX2 -  Init class.
*
* SaXInit is used to create the SaX cache files. SaX will provide a
* configuration suggestion which is the basis for all actions. To be
* sure not to make use of an outdated cache every program using libsax
* should create this cache first. The following example illustrates
* that more detailed:
*
* \code
* #include <sax/sax.h>
*
* SaXInit init;
* if (init.needInit()) {
*     printf ("initialize cache...\n");
*     //init.ignoreProfile();
*     init.doInit();
* }
* \endcode
*/
class SaXInit : public SaXInitIF {
	private:
	QList<const char*> mOptions;

	public:
	bool needInit (void);
	void ignoreProfile  (void);
	void setPrimaryChip (void);
	void setValidBusID  (void);

	public:
	void doInit (void);

	public:
	SaXInit ( QList<const char*> );
	SaXInit ( void );
};
} // end namespace
#endif
