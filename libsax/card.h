/**************
FILE          : card.h
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
#ifndef SAX_CARD_H
#define SAX_CARD_H 1

//====================================
// Includes...
//------------------------------------
#include <stdarg.h>
#include <dirent.h>
#include "import.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define ACPI_BATTERY     "/proc/acpi/battery"
#define SAX_ROTATE_LEFT  1
#define SAX_ROTATE_RIGHT 2

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 - Card manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateCardIF : public SaXException {
	public:
	virtual void setCardDriver ( const QString& ) = 0;
	virtual void setCardOption ( const QString& , const QString& ) = 0;
	virtual void addCardOption ( const QString& , const QString& ) = 0;
	virtual void removeCardOption ( const QString& ) = 0;
	virtual void addCardExternal  ( const QString& ) = 0;
	virtual void addCardExternalOption ( const QString& , const QString& ) = 0;
	virtual void setBusID ( const QString& ) = 0;
	virtual void setScreenID ( int ) = 0;
	virtual void setRotate   ( int ) = 0;

	public:
	virtual QList<QString> getCardDrivers ( void ) = 0;
	virtual Q3Dict<QString> getCardOptions ( const QString& ) = 0;
	virtual Q3Dict<QString> getOptions ( void ) = 0;
	virtual QString getCardDriver ( void ) = 0;
	virtual QString getCardName   ( void ) = 0;
	virtual QString getCardVendor ( void ) = 0;
	virtual QString getCardModel  ( void ) = 0;
	virtual QString getBusID      ( void ) = 0;
	virtual QString getScreenID   ( void ) = 0;
	virtual QString getRotationDirection ( void ) = 0;
	virtual int getCards   ( void ) = 0;
	virtual int getHeads   ( void ) = 0;
	virtual int getDevices ( void ) = 0;

	public:
	virtual bool isNoteBook ( void ) = 0;

	public:
	virtual bool selectCard ( int ) = 0;

	public:
	virtual ~SaXManipulateCardIF ( void ) { }
};

//====================================
// Class SaXManipulateCard...
//------------------------------------
/*! \brief SaX2 - Card manipulator class.
*
* The card manipulator requires one import object (Card) to become
* created. Once created the manipulator object is able to get/set
* hardware related information like graphics card driver or options
* to use with this driver. The following example shows how to use the
* card manipulator to force setting up the PanelSize on a radeon based
* NoteBook which reports the wrong size in its DDC record
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
*
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     SaXImport* import = new SaXImport ( SAX_CARD );
*     import->setSource ( SAX_SYSTEM_CONFIG );
*     import->doImport();
*     config->addImport (import);
*     section.insert (
*         import->getSectionName(),import
*     );
*     printf ("Overwrite PanelSize option...\n");
*     SaXManipulateCard mCard (
*         section["Card"]
*     );
*     if (mCard.selectCard (0)) {
*         mCard.addCardOption ("PanelSize","1280x1024");
*     }
*     printf ("Writing configuration\n");
*     config->setMode (SAX_MERGE);
*     if ( ! config->createConfiguration() ) {
*         printf ("%s\n",config->errorString());
*         printf ("%s\n",config->getParseErrorValue());
*         return 1;
*     }
*     return 0;
* }
* \endcode
*/
class SaXManipulateCard : public SaXManipulateCardIF {
	protected:
	SaXImport*     mImport;
	SaXProcess*    mCDBCardModules;
	QList<QString> mCDBCardDrivers;
	Q3Dict<QString> mCDBCardOptions;
	int            mCard;

	public:
	void setCardDriver ( const QString& );
	void setCardOption ( const QString& , const QString& );
	void addCardOption ( const QString& , const QString& );
	void removeCardOption ( const QString& );
	void addCardExternal  ( const QString& );
	void addCardExternalOption ( const QString& , const QString& );
	void setBusID ( const QString& );
	void setScreenID ( int );
	void setRotate   ( int );

	public:
	QList<QString> getCardDrivers ( void );
	Q3Dict<QString> getCardOptions ( const QString& );
	Q3Dict<QString> getOptions ( void );
	QString getCardDriver ( void );
	QString getCardName   ( void );
	QString getCardVendor ( void );
	QString getCardModel  ( void );
	QString getBusID      ( void );
	QString getScreenID   ( void );
	QString getRotationDirection ( void );
	int getCards   ( void );
	int getHeads   ( void );
	int getDevices ( void );

	public:
	bool isNoteBook ( void );

	public:
	bool selectCard ( int );

	public:
	SaXManipulateCard ( SaXImport*, int = 0 );
};
} // end namespace
#endif
