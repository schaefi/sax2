/**************
FILE          : mousemodel.h
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
              : - mousemodel.h: SaXGUI::SCCMouseModel header defs
              : - mousemodel.cpp: configure mouse model
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMOUSE_MODEL_H
#define SCCMOUSE_MODEL_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <q3tabdialog.h>
#include <q3vbox.h>
#include <q3buttongroup.h>
#include <q3listbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "mousedisplay.h"

namespace SaXGUI {
//====================================
// Class SCCMouseModel
//------------------------------------
class SCCMouseModel : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	QString        mSelectedMouseName;
	QString        mSelectedMouseVendor;
	bool           mOptionState;
	bool           mNeedImport;
	int            mDisplay;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	QList<QString> mCDBMouseVendors;
	QList<QString> mCDBMouseModels;

	private:
	SaXManipulateMice* mSaxMouse;

	private:
	Q3VBox*         mModelTab;

	private:
	Q3TabDialog*    mModelDialog;
	Q3ButtonGroup*  mModelVendorGroup;
	Q3ListBox*      mVendorList;
	Q3ListBox*      mModelList;

	public:
	void show ( void );

	public:
	QString getVendorName ( void );
	QString getModelName  ( void );
	bool getOptionState ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotOk     ( void );
    void slotVendor ( Q3ListBoxItem* );

	public:
	SCCMouseModel (
		Q3Dict<QString>*, Q3Dict<SaXImport>, const QString&,
		int, QWidget*
	);
};
} // end namespace

#endif
