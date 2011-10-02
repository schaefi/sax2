/**************
FILE          : keyboard.h
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
#ifndef SAX_KEYBOARD_H
#define SAX_KEYBOARD_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>
#include <X11/extensions/XKBgeom.h>
#include <X11/extensions/XKM.h>
#include <X11/extensions/XKBfile.h>

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_DEFAULT_RULE   "xorg"
#define XKB_LEFT_ALT       "LeftAlt"
#define XKB_RIGHT_ALT      "RightAlt"
#define XKB_SCROLL_LOCK    "ScrollLock"
#define XKB_RIGHT_CTL      "RightCtl"

#define XKB_MAP_META       "Meta"
#define XKB_MAP_COMPOSE    "Compose"
#define XKB_MAP_MODESHIFT  "ModeShift"
#define XKB_MAP_MODELOCK   "ModeLock"
#define XKB_MAP_SCROLLLOCK "ScrollLock"
#define XKB_MAP_CONTROL    "Control"

#define SAX_CORE_KBD       0
#define SAX_NEXT_KBD(x)    ((x)+2)

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  XKB key rules class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXKeyRulesIF : public SaXException {
	public:
	virtual Q3Dict<QString> getModels   ( void ) = 0;
	virtual Q3Dict<QString> getLayouts  ( void ) = 0;
	virtual Q3Dict<QString> getOptions  ( void ) = 0;
	virtual QList<QString> getVariants ( const QString& ) = 0;

	public:
	virtual ~SaXKeyRulesIF ( void ) { }
};
//====================================
// Class SaXKeyRules...
//------------------------------------
/*! \brief SaX2 -  XKB key rules class
*
* The key rules class has been implemented to provide access to the
* XKB file extensions. This extensions provides information about the
* supported keyboard models layout and variants
*/
class SaXKeyRules : public SaXKeyRulesIF {
	protected:
	void loadRules ( QString );

	private:
	Q3Dict<QString> mModels;
	Q3Dict<QString> mLayouts;
	Q3Dict<QString> mOptions;
	Q3Dict<QStringList> mVarLists;
	QString mX11Dir;

	public:
	Q3Dict<QString> getModels   ( void );
	Q3Dict<QString> getLayouts  ( void );
	Q3Dict<QString> getOptions  ( void );
	QList<QString> getVariants ( const QString& );

	public:
	SaXKeyRules ( QString rule=SAX_DEFAULT_RULE );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Keyboard class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateKeyboardIF : public SaXKeyRules {
	public:
	virtual void setXKBModel       ( const QString& ) = 0;

	public:
	virtual void setXKBLayout      ( const QString& ) = 0;
	virtual void addXKBLayout      ( const QString& ) = 0;
	virtual void removeXKBLayout   ( const QString& ) = 0;

	public:
	virtual void setXKBOption      ( const QString& ) = 0;
	virtual void addXKBOption      ( const QString& ) = 0;
	virtual void removeXKBOption   ( const QString& ) = 0;

	public:
	virtual void setXKBVariant     ( const QString& , const QString& ) = 0;
	virtual void removeXKBVariant  ( const QString& ) = 0;

	public:
	virtual void setMapping        ( const QString& , const QString& ) = 0;

	public:
	virtual QString getXKBVariant ( const QString& ) = 0;
	virtual QString getXKBModel   ( void  ) = 0;

	public:
	virtual QList<QString> getXKBOptionList  ( void ) = 0;
	virtual QList<QString> getXKBLayout      ( void ) = 0;
	virtual QList<QString> getXKBVariantList ( void ) = 0;
	virtual QString getDriver ( void ) = 0;

	public:
	virtual bool selectKeyboard ( int ) = 0;

	public:
	virtual ~SaXManipulateKeyboardIF ( void ) { }

	protected:
	SaXManipulateKeyboardIF ( void ) : SaXKeyRules() { }
};
//====================================
// Class SaXManipulateKeyboard...
//------------------------------------
/*! \brief SaX2 -  Keyboard class.
*
* The keyboard manipulator requires one import object (Keyboard) to become
* created. Once created the manipulator object is able to get/set specific
* keyboard options and is able to access the XKB file extension to know
* about the supported keyboards. The following example will demonstrate
* how to add an additional keyboard layout to the core layout:
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
*     SaXImport* import = new SaXImport ( SAX_KEYBOARD );
*     import->setSource ( SAX_SYSTEM_CONFIG );
*     import->doImport();
*     config->addImport (import);
*     section.insert (
*         import->getSectionName(),import
*     );
*     printf ("Add czech keyboard layout...\n");
*     SaXManipulateKeyboard mKeyboard (
*         section["Keyboard"]
*     );
*     if (mKeyboard.selectKeyboard (SAX_CORE_KBD)) {
*         mKeyboard.addXKBLayout ("cz");
*     }
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
class SaXManipulateKeyboard : public SaXManipulateKeyboardIF {
	private:
	SaXImport* mImport;
	int mKeyboard;

	private:
	int findLayout            ( const QString& );
	QList<QString> createList ( const QString& );

	public:
	virtual void setXKBModel       ( const QString& );

	public:
	void setXKBLayout      ( const QString& );
	void addXKBLayout      ( const QString& );
    void removeXKBLayout   ( const QString& );

	public:
	void setXKBOption      ( const QString& );
	void addXKBOption      ( const QString& );
	void removeXKBOption   ( const QString& );

	public:
	void setXKBVariant     ( const QString& , const QString& );
	void removeXKBVariant  ( const QString& );

	public:
	void setMapping        ( const QString& , const QString& );

	public:
	QString getXKBVariant ( const QString& );
	QString getXKBModel   ( void  );

	public:
	QList<QString> getXKBOptionList  ( void );
	QList<QString> getXKBLayout      ( void );
	QList<QString> getXKBVariantList ( void );
	QString getDriver ( void );

	public:
	bool selectKeyboard ( int );

	public:
	SaXManipulateKeyboard ( SaXImport*, int = 0 );
};
} // end namespace
#endif
