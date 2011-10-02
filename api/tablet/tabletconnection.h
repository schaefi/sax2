/**************
FILE          : tabletconnection.h
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
              : - tabletconnection.h: SaXGUI::SCCTabletConnection header defs
              : - tabletconnection.cpp: configure tablets
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTABLET_CONNECTION_H
#define SCCTABLET_CONNECTION_H 1

//=====================================
// Includes
//-------------------------------------
#include <qradiobutton.h>
#include <qlabel.h>
#include <q3buttongroup.h>
#include <qcombobox.h>
#include <q3listbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCTabletConnection
//------------------------------------
class SCCTabletConnection : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	Q3Dict<QString>   mSelectedOptions;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString>   mOptionDict;

	private:
	SaXManipulateTablets* mSaxTablet;

	private:
	Q3BoxLayout*      mMainLayout;

	private:
	Q3ButtonGroup*    mPortGroup;
	QComboBox*       mPortBox;
	Q3ButtonGroup*    mOptionGroup;
	Q3ListBox*        mOptionList;
	Q3ButtonGroup*    mModeGroup;
	QRadioButton*    mRelative;
	QRadioButton*    mAbsolute;

	public:
	QString getPortName ( void );
	bool isAutoPort     ( void );
	int getTabletMode   ( void );
	Q3Dict<QString> getOptions ( void );

	public:
	void setMode ( const QString& );
	void setPort ( const QString& );
	void setOptions ( const Q3Dict<QString>& );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotTablet ( const QString&,const QString& );
	void slotOption ( Q3ListBoxItem* );

	public:
	SCCTabletConnection (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace
#endif
