/**************
FILE          : pointers.h
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
#ifndef SAX_POINTERS_H
#define SAX_POINTERS_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"
#include "device.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_CORE_POINTER     1
#define SAX_NEXT_POINTER(x)  ((x)+2)

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Base pointer class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulatePointersIF : public SaXException {
	public:
	virtual void setDriver    ( const QString& ) = 0;
	virtual void setDevice    ( const QString& ) = 0;
	virtual void setProtocol  ( const QString& ) = 0;
	virtual void setNamePair  ( const QString& , const QString& ) = 0;
	virtual void setOption    ( const QString& , const QString& = 0 ) = 0;
	virtual void addOption    ( const QString& , const QString& = 0 ) = 0;
	virtual void removeOption ( const QString& ) = 0;

	public:
	virtual QString getDevice ( void ) = 0;
	virtual QString getDriver ( void ) = 0;
	virtual QString getProtocol ( void ) = 0;
	virtual Q3Dict<QString> getOptions ( void ) = 0;
	
	public:
	virtual bool selectPointer ( int ) = 0;

	public:
	virtual ~SaXManipulatePointersIF ( void ) { }
};
//====================================
// Class SaXManipulatePointers...
//------------------------------------
/*! \brief SaX2 -  Base pointer class.
*
* The manipulate pointers class is the base class for all supported
* pointer devices Mice, Tablets and Touchscreens. It is not possible
* to create an object of type SaXManipulatePointers because the class
* contains only methods which are common to all pointer devices and
* can be used within the derived classes.
*/
class SaXManipulatePointers : public SaXManipulatePointersIF {
	protected:
	SaXImport* mImport;
	int mPointer;

	public:
	void setDriver    ( const QString& );
	void setDevice    ( const QString& );
	void setProtocol  ( const QString& );
	void setNamePair  ( const QString& , const QString& );
	void setOption    ( const QString& , const QString& = 0 );
	void addOption    ( const QString& , const QString& = 0 );
	void removeOption ( const QString& );

	public:
	QString getDevice ( void );
	QString getDriver ( void );
	QString getProtocol ( void );
	Q3Dict<QString> getOptions ( void );
	
	public:
	bool selectPointer ( int );

	protected:
	SaXManipulatePointers ( SaXImport*, int = 1 );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Mouse manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateMiceIF : public SaXManipulatePointers {
	public:
	virtual void setMouse ( const QString& ) = 0;
	virtual void setMouse ( const QString&,const QString& ) = 0;
	virtual void enableWheelEmulation    ( int  ) = 0;
	virtual void disableWheelEmulation   ( void ) = 0;
	virtual void enable3ButtonEmulation  ( void ) = 0;
	virtual void disable3ButtonEmulation ( void ) = 0;
	virtual void enableWheel   ( void ) = 0;
	virtual void enableWheelOn ( int,int ) = 0;
	virtual void disableWheel  ( void ) = 0;
	virtual void enableXAxisInvertation  ( void ) = 0;
	virtual void enableYAxisInvertation  ( void ) = 0;
	virtual void disableXAxisInvertation ( void ) = 0;
	virtual void disableYAxisInvertation ( void ) = 0;
	virtual void enableLeftHandMapping  ( void ) = 0;
	virtual void disableLeftHandMapping ( void ) = 0;

	public:
	virtual QList<QString> getMouseList       ( void ) = 0;
	virtual QList<QString> getMouseVendorList ( void ) = 0;
	virtual QList<QString> getMouseModelList  ( const QString& ) = 0;
	virtual Q3Dict<QString> getMouseData ( const QString& ) = 0;
	virtual Q3Dict<QString> getMouseData ( const QString&,const QString& ) = 0;
	virtual int getWheelEmulatedButton  ( void ) = 0;

	public:
	virtual bool isWheelEnabled    ( void ) = 0;
	virtual bool isWheelEmulated   ( void ) = 0;
	virtual bool isButtonEmulated  ( void ) = 0;
	virtual bool isXAxisInverted   ( void ) = 0;
	virtual bool isYAxisInverted   ( void ) = 0;
	virtual bool isLeftHandEnabled ( void ) = 0;
	virtual bool isMouse ( void ) = 0;

	public:
	virtual ~SaXManipulateMiceIF ( void ) { }

	protected:
	SaXManipulateMiceIF (SaXImport*in,int id): SaXManipulatePointers (in,id) { }
};
//====================================
// Class SaXManipulateMice...
//------------------------------------
/*! \brief SaX2 -  Mouse manipulator class.
*
* The mice manipulator requires one import object (Pointers) to become
* created. Once created the manipulator object is able to get/set standard
* mouse configuration information like wheel and button handling. The
* following example demonstrate how to disable the mouse wheel.
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
*     SaXImport* import = new SaXImport ( SAX_POINTERS );
*     import->setSource ( SAX_SYSTEM_CONFIG );
*     import->doImport();
*     config->addImport (import);
*     section.insert (
*         import->getSectionName(),import
*     );
*     printf ("Disable core pointer mouse wheel...\n");
*     SaXManipulateMice mMouse (
*         section["Pointers"]
*     );
*     if (mMouse.selectPointer (SAX_CORE_POINTER)) {
*     if (mMouse.wheelEnabled()) {
*         mMouse.disableWheel();
*     }
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
class SaXManipulateMice : public SaXManipulateMiceIF {
	private:
	SaXProcess*    mCDBMice;
	QList<QString> mCDBMouseList;
	Q3Dict<QString> mCDBMouseData;

	public:
	void setMouse ( const QString& );
	void setMouse ( const QString&,const QString& );
	void enableWheelEmulation    ( int  );
	void disableWheelEmulation   ( void );
	void enable3ButtonEmulation  ( void );
	void disable3ButtonEmulation ( void );
	void enableWheel   ( void );
	void enableWheelOn ( int,int );
	void disableWheel  ( void );
	void enableXAxisInvertation  ( void );
	void enableYAxisInvertation  ( void );
	void disableXAxisInvertation ( void );
	void disableYAxisInvertation ( void );
	void enableLeftHandMapping  ( void );
	void disableLeftHandMapping ( void );

	public:
	QList<QString> getMouseList       ( void );
	QList<QString> getMouseVendorList ( void );
	QList<QString> getMouseModelList  ( const QString& );
	Q3Dict<QString> getMouseData ( const QString& );
	Q3Dict<QString> getMouseData ( const QString&,const QString& );
	int getWheelEmulatedButton  ( void );

	public:
	bool isWheelEnabled    ( void );
	bool isWheelEmulated   ( void );
	bool isButtonEmulated  ( void );
	bool isXAxisInverted   ( void );
	bool isYAxisInverted   ( void );
	bool isLeftHandEnabled ( void );
	bool isMouse ( void );

	public:
	SaXManipulateMice ( SaXImport*, int = 1 );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Tablet manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateTabletsIF : public SaXManipulatePointers {
	public:
	virtual Q3Dict<QString> getTabletData ( const QString& ) = 0;
	virtual Q3Dict<QString> getTabletData ( const QString&,const QString& ) = 0;
	virtual Q3Dict<QString> getPenData ( const QString& ) = 0;
	virtual Q3Dict<QString> getPenData ( const QString&,const QString& ) = 0;
	virtual QList<QString> getTabletVendorList ( void ) = 0;
	virtual QList<QString> getTabletModelList  ( const QString& ) = 0;
	virtual QList<QString> getTabletList ( void ) = 0;
	virtual QList<QString> getPenList    ( void ) = 0;
	virtual QList<QString> getTabletDrivers ( void ) = 0;
	virtual Q3Dict<QString> getTabletOptions ( const QString& ) = 0;
	virtual void setTablet ( const QString&,const QString& ) = 0;
	virtual void setTablet ( const QString& ) = 0;
	virtual void setType   ( const QString& ) = 0;
	virtual void setMode   ( const QString& ) = 0;
	virtual int  addPen    ( const QString& ) = 0;
	virtual int  addPen    ( const QString&,const QString& ) = 0;
	virtual int  addPad    ( const QString& ) = 0;
	virtual int  addPad    ( const QString&,const QString& ) = 0;
	virtual int  removePen ( int ) = 0;
	virtual int  removePad ( int ) = 0;

	public:
	virtual QString getName   ( void ) = 0;
	virtual QString getVendor ( void ) = 0;
	virtual QString getType   ( void ) = 0;
	virtual QString getMode   ( void ) = 0;
	virtual bool isTablet     ( void ) = 0;
	virtual bool isPen        ( void ) = 0;
	virtual bool isEraser     ( void ) = 0;
	virtual bool isTouch      ( void ) = 0;
	virtual bool isPad        ( void ) = 0;

	public:
	virtual ~SaXManipulateTabletsIF ( void ) { }

	protected:
	SaXManipulateTabletsIF (
		SaXImport*in,int id
	): SaXManipulatePointers (in,id) { }
};
//====================================
// Class SaXManipulateTablets...
//------------------------------------
/*! \brief SaX2 -  Tablet manipulator class.
*
* The tablet manipulator requires two import object (Pointers and Layout)
* to become created. Once created the manipulator object is able to get/set
* tablet configuration information. To set up a tablet a new input device
* needs to created first which get transformed into a tablet using the
* setTablet() method. Based on this tablet device the manipulator is able
* to add additional input devices using the addPen() method which results
* in a stylus or an eraser pointer for this tablet. The following example
* demonstrate how to add a tablet with one stylus-pen applied
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
*     for (int id=0; id<2; id++) {
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
*     printf ("ID: %d is [SAX_INPUT_TABLET]: added\n",tabletID);
* 
*     printf ("Setting up tablet data... ");
*     SaXManipulateTablets pointer (
*         section["Pointers"],section["Layout"]
*     );
*     if (pointer.selectPointer (tabletID)) {
*         QList<QString> tabletList = pointer.getTabletList();
*         QList<QString> penList = pointer.getPenList();
*        QString* myTablet = tabletList.at (3);
*         QString* myPen = penList.at(3);
*         pointer.setTablet( *myTablet );
*         pointer.addPen ( *myPen );
*         printf ("Tablet: [%s] with pen: [%s] configured\n",
*             myTablet->ascii(),myPen->ascii()
*         );
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
class SaXManipulateTablets : public SaXManipulateTabletsIF {
	private:
	SaXProcess*    mCDBTablets;
	SaXProcess*    mCDBTabletModules;
	QList<QString> mCDBTabletList;
	Q3Dict<QString> mCDBTabletData;
	SaXProcess*    mCDBPens;
	SaXProcess*    mCDBPads;
	QList<QString> mCDBPenList;
	QList<QString> mCDBTabletDrivers;
	Q3Dict<QString> mCDBTabletOptions;

	private:
	SaXManipulateDevices* mManipInputDevices;

	public:
	Q3Dict<QString> getTabletData ( const QString& );
	Q3Dict<QString> getTabletData ( const QString&,const QString& );
	Q3Dict<QString> getPenData ( const QString& );
	Q3Dict<QString> getPenData ( const QString&,const QString& );
	QList<QString> getTabletVendorList ( void );
	QList<QString> getTabletModelList  ( const QString& ); 
	QList<QString> getTabletList ( void );
	QList<QString> getPenList    ( void );
	QList<QString> getTabletDrivers ( void );
	Q3Dict<QString> getTabletOptions ( const QString& );
	void setTablet ( const QString&,const QString& );
	void setTablet ( const QString& );
	void setType   ( const QString& );
	void setMode   ( const QString& );
	int  addPen    ( const QString& );
	int  addPen    ( const QString&,const QString& );
	int  addPad    ( const QString& );
	int  addPad    ( const QString&,const QString& );
	int  removePen   ( int );
	int  removeTouch ( int );
	int  removePad   ( int );

	public:
	QString getName   ( void );
	QString getVendor ( void );
	QString getType   ( void );
	QString getMode   ( void );
	bool isTablet     ( void );
	bool isPen        ( void );
	bool isEraser     ( void );
	bool isTouch      ( void );
	bool isPad        ( void );

	public:
	SaXManipulateTablets ( SaXImport*, SaXImport* , int = 1 );
};

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Touchscreen manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateTouchscreensIF : public SaXManipulatePointers {
	public:
	virtual QList<QString> getPanelList ( void ) = 0;
	virtual QList<QString> getPanelVendorList ( void ) = 0;
	virtual QList<QString> getPanelModelList  ( const QString& ) = 0;
	virtual Q3Dict<QString> getPanelData ( const QString& ) = 0;
	virtual Q3Dict<QString> getPanelData ( const QString&,const QString& ) = 0;
	virtual void setTouchPanel ( const QString&,const QString& ) = 0;
	virtual void setTouchPanel ( const QString& ) = 0;

	public:
	virtual bool isTouchpanel ( void ) = 0;
	virtual QString getName   ( void ) = 0;
	virtual QString getVendor ( void ) = 0;
	virtual QString getType   ( void ) = 0;

	public:
	virtual ~SaXManipulateTouchscreensIF ( void ) { }

	protected:
	SaXManipulateTouchscreensIF (
		SaXImport*in,int id
	): SaXManipulatePointers (in,id) { }
};
//====================================
// Class SaXManipulateTouchscreens...
//------------------------------------
/*! \brief SaX2 -  Touchscreen manipulator class
*
* The touchscreen manipulator requires one import object (Pointers) to
* become created. Once created the manipulator object is able to transform
* a currently existing InputDevice into a touchpanel. In almost all cases
* it is needed to create this InputDevice first which means there is the
* need for the device manipulator as well. The following example
* demonstrate how to add a touch panel to the configuration.
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
*     for (int id=0; id<2; id++) {
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
*     int panelID = dev.addInputDevice (SAX_INPUT_TOUCHPANEL);
*     printf ("ID: %d is [SAX_INPUT_TOUCHPANEL]: added\n",panelID);
* 
*     printf ("Setting up touchpanel data... ");
*     SaXManipulateTouchscreens pointer (
*         section["Pointers"]
*     );
*     if (pointer.selectPointer (panelID)) {
*         QList<QString> panelList = pointer.getPanelList();
*         QString* myPanel = panelList.at(0);
*         pointer.setTouchPanel( *myPanel );
*         printf ("TouchPanel: [%s] configured\n", myPanel->ascii());
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
class SaXManipulateTouchscreens : public SaXManipulateTouchscreensIF {
	private:
	SaXProcess*    mCDBPanels;
	QList<QString> mCDBPanelList;
	Q3Dict<QString> mCDBPanelData;

	public:
	QList<QString> getPanelList ( void );
	QList<QString> getPanelVendorList ( void );
	QList<QString> getPanelModelList  ( const QString& );
	Q3Dict<QString> getPanelData ( const QString& );
	Q3Dict<QString> getPanelData ( const QString&,const QString& );
	void setTouchPanel ( const QString&,const QString& );
	void setTouchPanel ( const QString& );

	public:
	bool isTouchpanel ( void );
	QString getName   ( void );
	QString getVendor ( void );
	QString getType   ( void );

	public:
	SaXManipulateTouchscreens ( SaXImport*, int = 1 );
};
} // end namespace
#endif
