/**************
FILE          : tabletpenproperty.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 GUI system using libsax to provide
              : configurations for a wide range of X11 capabilities
              : //.../
              : - tabletpenproperty.h: SaXGUI::SCCTabletPenProperty headers
              : - tabletpenproperty.cpp: configure tablets
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTABLET_PEN_PROPERTY_H
#define SCCTABLET_PEN_PROPERTY_H 1

//=====================================
// Includes
//-------------------------------------
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <q3tabdialog.h>
#include <q3vbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCTabletPenProperty
//------------------------------------
class SCCTabletPenProperty : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	Q3Dict<QString> mSelectedOptions;
	int            mPenMode;
	bool           mNeedImport;
	int            mDisplay;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString> mOptionDict;

	private:
	SaXManipulateTablets* mSaxTablet;

	private:
	Q3VBox*         mTabletPenTab;

	private:
	Q3TabDialog*    mTabletPenDialog;
	Q3ButtonGroup*  mModeGroup;
	QRadioButton*  mRelative;
	QRadioButton*  mAbsolute;
	Q3ButtonGroup*  mOptionGroup;
	Q3ListBox*      mOptionList;

	public:
	void show ( void );
	
	public:
	int getPenMode ( void );
	Q3Dict<QString> getPenOptions ( void );
	void setupPen ( const QString&,const QString& );
	void setID ( int );
	int getID ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotOk     ( void );
	void slotOption ( Q3ListBoxItem* );

	public:
	SCCTabletPenProperty (
		Q3Dict<QString>*,Q3Dict<SaXImport>,
		const QString&,int,QWidget*
	);
};
} // end namespace
#endif
