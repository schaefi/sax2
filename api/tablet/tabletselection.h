/**************
FILE          : tabletselection.h
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
              : - tabletselection.h: SaXGUI::SCCTabletSelection header defs
              : - tabletselection.cpp: configure tablets
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTABLET_SELECTION_H
#define SCCTABLET_SELECTION_H 1

//=====================================
// Includes
//-------------------------------------
#include <qcheckbox.h>
#include <qlabel.h>
#include <q3buttongroup.h>
#include <q3listbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCTabletSelection
//------------------------------------
class SCCTabletSelection : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	bool           mEnabled;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	QList<QString> mCDBTabletVendors;
	QList<QString> mCDBTabletModels;

	private:
	SaXManipulateTablets* mSaxTablet;

	private:
	Q3BoxLayout*      mMainLayout;

	private:
	QCheckBox*       mCheckEnable;
	Q3ButtonGroup*    mModelVendorGroup;
	Q3ListBox*        mVendorList;
	Q3ListBox*        mModelList;

	public:
	QString getVendor ( void );
	QString getModel  ( void );
	void selectTablet ( const QString&, const QString& );
	void setEnabled   ( bool );
	bool isEnabled    ( void );
	

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotActivateTablet ( void );
	void slotVendor ( Q3ListBoxItem* );
	void slotName   ( Q3ListBoxItem* );

	signals:
	void sigActivate ( bool );
	void sigTablet   ( const QString&,const QString& );

	public:
	SCCTabletSelection (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace

//=====================================
// Includes
//-------------------------------------
#include "tablet.h"

#endif
