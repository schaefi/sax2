/**************
FILE          : monitormodel.h
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
              : - monitormodel.h: SaXGUI::SCCMonitor header definitions
              : - monitormodel.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_MODEL_H
#define SCCMONITOR_MODEL_H 1

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
#include <math.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "monitordisplay.h"

//=====================================
// Defines
//-------------------------------------
#define DTA_DIR        "/usr/share/sax/api/data"
#define MACROS_DIR     "/usr/share/sax/api/macros"
#define ASPECT_FILE    DTA_DIR    "/MonitorRatio"
#define DIAGONAL_FILE  DTA_DIR    "/MonitorTraversal"
#define GETINF         MACROS_DIR "/getINF"

namespace SaXGUI {
//====================================
// Class SCCMonitorModel
//------------------------------------
class SCCMonitorModel : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	bool           mNeedImport;
	int            mDisplay;
	QString        mSelectedMonitorName;
	QString        mSelectedMonitorVendor;
	bool           mDPMSStatus;
	int            mDisplaySizeX;
	int            mDisplaySizeY;
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
	QList<QString> mTraversalList;
	QList<QString> mAspectList;
	QString        mExtraModeline;

	private:
	SaXManipulateDesktop* mSaxDesktop;

	private:
	Q3VBox*         mModelTab;
	Q3VBox*         mAspectTab;
	Q3VBox*         mSyncTab;

	private:
	Q3TabDialog*    mModelDialog;
	Q3ButtonGroup*  mModelVendorGroup;
	Q3ListBox*      mVendorList;
	Q3ListBox*      mModelList;
	Q3HBox*         mToolBox;
	QCheckBox*     mCheckDPMS;
	QPushButton*   mDisk;

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

	private:
	Q3ButtonGroup*  mAspectGroup;
	QLabel*        mLabelAspect;
	QLabel*        mLabelRatio;
	QComboBox*     mAspect;
	QComboBox*     mTraversal;
	Q3ButtonGroup*  mDisplaySizeGroup;
	QLabel*        mLabelX;
	QLabel*        mLabelY;
	QSpinBox*      mSizeX;
	QSpinBox*      mSizeY;

	public:
	void show ( void );

	public:
	bool isDPMSEnabled ( void );
	QString getVendorName ( void );
	QString getModelName  ( void );
	QString getExtraModeline ( void );
	void setTitle ( const QString& );
	void setDisplaySize ( QList<QString> );
	int getSizeX ( void );
	int getSizeY ( void );
	int getHSmin ( void );
	int getHSmax ( void );
	int getVSmin ( void );
	int getVSmax ( void ); 

	public:
	void init   ( void );
	void import ( void );

	private:
	double getTraversal ( int,int );
	QPoint getRatio     ( int,int );
	QPoint translateTraversal ( double,int,int );

	public slots:
	void slotOk   ( void );
	void slotDisk ( void );
	void slotName   ( Q3ListBoxItem* );
	void slotVendor ( Q3ListBoxItem* );
	void slotRatio     ( int );
	void slotTraversal ( int );

	signals:
	void sigMonitorChanged ( SCCMonitorDisplay* );

	public:
	SCCMonitorModel (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		const QString&,const QString&,const QString&,
		int,QWidget*
	);
};
} // end namespace
#endif
