/**************
FILE          : storage.h
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
#ifndef SAX_STORAGE_H
#define SAX_STORAGE_H 1

//====================================
// Includes...
//------------------------------------
#include <Q3Dict>
#include <Q3PtrList>
#include <qstring.h>
#include <QTextOStream>
#include <qstringlist.h>
#include <qregexp.h>

#include "exception.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define NULL_STRING        ""
#define SAX_DESKTOP_TYPE   1
#define SAX_KEYBOARD_TYPE  2
#define SAX_POINTER_TYPE   3

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Storage class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXStorageIF : public SaXException {
	public:
	virtual void setItem ( const QString &, const QString & ) = 0;
	virtual void addItem ( const QString &, const QString & ) = 0;
	virtual void removeItem  ( const QString &, const QString & ) = 0;
	virtual void removeEntry ( const QString & ) = 0;
	virtual void setDenomination (
		const QString &, const QString &,const QString &
	) = 0;
	virtual void setRawItem (
		const QString &, const QString &,const QString &
	) = 0;
	virtual void addRawItem (
		const QString &, const QString &,const QString &
	) = 0;
	virtual void removeRawItem ( const QString &, const QString & ) = 0;
	virtual QString getItem ( const QString & ) = 0;

	public:
	virtual bool setID ( int ) = 0;
	virtual bool addID ( int ) = 0;
	virtual bool delID ( int ) = 0;
	virtual int getCurrentID ( void ) = 0;
	virtual Q3Dict<QString> getTable ( int ) = 0;
	virtual Q3Dict<QString>  getCurrentTable ( void ) = 0;
	virtual Q3Dict<QString>* getTablePointer ( int  ) = 0;
    virtual Q3Dict<QString>* getCurrentTablePointer ( void ) = 0;
	virtual void merge ( Q3PtrList< Q3Dict<QString> > ) = 0;
	virtual int getCount ( bool = false ) = 0;

	public:
	virtual void addGroup (
		const QString &,const QString &, const QString &
	) = 0;
	virtual Q3Dict< Q3Dict<QString> > getTablePointerCDB  ( void ) = 0;
	virtual Q3PtrList< Q3Dict<QString> > getTablePointerCDB_DATA (
		const QString &
	) = 0;
	virtual Q3PtrList< Q3Dict<QString> > getTablePointerDATA ( void ) = 0;

	public:
	virtual ~SaXStorageIF ( void ) { }
};
//====================================
// Class SaXStorage...
//------------------------------------
/*! \brief SaX2 -  Storage class
*
* The SaXStorage class provides all the structures and member functions
* to store, delete/retrieve and change configuration related
* information. The internal data structure is based on a list of hashes
* or a an hash of hashes. The standard data is stored in a dictionary
* list whereas the special CDB data is stored in a dictionary of
* dictionaries
*
* \code
* #include <sax/sax.h>
*
* SaXStorage container;
* container.setItem ("Identifier","Monitor[0]");
* container.setDenomination ("01","Belinea","102030");
* container.setRawItem ("RawOption","opt1","val1");
* container.addRawItem ("RawOption","opt2","val2");
* container.removeRawItem ("RawOption","opt1");
*
* //QDict<QString> section = container.getCurrentTable();
* QDict<QString> section = container[0];
*
* QString* data = section.take ("RawOption");
* printf ("%s\n",data->ascii());
*
* printf ("%s\n",container["RawOption"].ascii());
* \endcode
*/
class SaXStorage : public SaXStorageIF {
	protected:
	Q3PtrList< Q3Dict<QString> > mData;
	Q3Dict< Q3Dict<QString> > mCDB;
	int mCurrentID;

	public:
	void setItem ( const QString &, const QString & );
	void addItem ( const QString &, const QString & );
	void removeItem  ( const QString &, const QString & );
	void removeEntry ( const QString & );
	void setDenomination (
		const QString &, const QString &,const QString &
	);
	void setRawItem (
		const QString &, const QString &,const QString &
	);
	void addRawItem (
		const QString &, const QString &,const QString &
	);
	void removeRawItem ( const QString &, const QString & );
	QString getItem ( const QString & );

	public:
	bool setID ( int );
	bool addID ( int );
	bool delID ( int );
	int getCurrentID ( void );
	Q3Dict<QString> getTable ( int );
	Q3Dict<QString>  getCurrentTable ( void );
	Q3Dict<QString>* getTablePointer ( int  );
	Q3Dict<QString>* getCurrentTablePointer ( void );
	void merge ( Q3PtrList< Q3Dict<QString> > );
	int getCount ( bool = false );

	public:
	void addGroup (
		const QString &,const QString &, const QString &
	);
	Q3Dict< Q3Dict<QString> > getTablePointerCDB  ( void );
	Q3PtrList< Q3Dict<QString> > getTablePointerCDB_DATA ( const QString & );
	Q3PtrList< Q3Dict<QString> > getTablePointerDATA ( void );

	#ifndef NO_OPERATOR_SUPPORT
	public:
	Q3Dict<QString> operator[] ( int id ) {
		return getTable (id);
	}
	QString operator[] ( const QString & key ) {
		return getItem (key);
	}
	#endif

	public:
	SaXStorage ( void );
};
} // end namespace
#endif
