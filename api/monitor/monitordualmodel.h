/**************
FILE          : monitordualmodel.h
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
              : - monitordualmodel.h: SaXGUI::SCCMonitor header definitions
              : - monitordualmodel.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_DUAL_MODEL_H
#define SCCMONITOR_DUAL_MODEL_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <q3vbox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <q3tabdialog.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <q3listbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "monitordual.h"

namespace SaXGUI {
//====================================
// Class SCCMonitorDualModel
//------------------------------------
class SCCMonitorDualModel : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	bool           mNeedImport;
	int            mDisplay;
	QString        mSelectedMonitorName;
	QString        mSelectedMonitorVendor;
	int            mHsyncMin;
	int            mHsyncMax;
	int            mVsyncMin;
	int            mVsyncMax;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	QList<QString> mCDBMonitorVendors;
	QList<QString> mCDBMonitorModels;
	Q3Dict<QString> mCDBMonitorData;
	Q3Dict<QString> mProfileDriverOptions;

	private:
	SaXManipulateDesktop* mSaxDesktop;

	private:
	Q3VBox*         mModelTab;
	Q3VBox*         mSyncTab;

	private:
	Q3TabDialog*    mDualModelDialog;
	Q3ButtonGroup*  mModelVendorGroup;
	Q3ListBox*      mVendorList;
	Q3ListBox*      mModelList;

	private:
	Q3ButtonGroup*  mSyncGroup;
	QLabel*        mLabelWarning;
	Q3VBox*         mSyncBox;
	QLabel*        mLabelHorizontal;
	QSpinBox*      mHSpinMin;
	QSpinBox*      mHSpinMax;
	QLabel*        mLabelHUnit;
	QLabel*        mLabelVertical;
	QSpinBox*      mVSpinMin;
	QSpinBox*      mVSpinMax;
	QLabel*        mLabelVUnit;

	public:
	void show ( void );

	public:
	void slotVerifiedOk   ( void );
	QString getVendorName ( void );
	QString getModelName  ( void );
	int getHSmin ( void );
	int getHSmax ( void );
	int getVSmin ( void );
	int getVSmax ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotOk     ( void );
	void slotName   ( Q3ListBoxItem* );
	void slotVendor ( Q3ListBoxItem* );

	public:
	SCCMonitorDualModel (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		const QString&, const QString&, int, QWidget*
	);
};
} // end namespace
#endif
