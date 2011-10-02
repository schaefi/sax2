/**************
FILE          : monitordual.h
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
              : - monitordual.h: SaXGUI::SCCMonitor header definitions
              : - monitordual.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_DUAL_H
#define SCCMONITOR_DUAL_H 1

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
//Added by qt3to4:
#include <QPixmap>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "monitordisplay.h"

//=====================================
// Defines...
//-------------------------------------
#define PIXDIR           "/usr/share/sax/api/figures"
#define LEFT_OF_PIXMAP   PIXDIR "/dualleftof.png"
#define RIGHT_OF_PIXMAP  PIXDIR "/dualrightof.png"
#define ABOVE_OF_PIXMAP  PIXDIR "/dualaboveof.png"
#define BELOW_OF_PIXMAP  PIXDIR "/dualbelowof.png"

//=====================================
// Defines...
//-------------------------------------
#define DUAL_TRADITIONAL_KEY "Traditional"
#define DUAL_CLONE_KEY       "Clone"
#define DUAL_XINERAMA_KEY    "Xinerama"
#define DUAL_TRADITIONAL     0
#define DUAL_CLONE           1
#define DUAL_XINERAMA        2

//=====================================
// Defines...
//-------------------------------------
#define DUAL_LEFTOF_KEY         "LeftOf"
#define DUAL_ABOVEOF_KEY        "Above"
#define DUAL_RIGHTOF_KEY        "RightOf"
#define DUAL_BELOWOF_KEY        "Below"
#define DUAL_FGLRX_LEFTOF_KEY   "Horizontal"
#define DUAL_FGLRX_ABOVEOF_KEY  "Vertical"
#define DUAL_FGLRX_RIGHTOF_KEY  "Horizontal,Reverse"
#define DUAL_FGLRX_BELOWOF_KEY  "Vertical,Reverse"

#define DUAL_LEFTOF          0
#define DUAL_ABOVEOF         1
#define DUAL_RIGHTOF         2
#define DUAL_BELOWOF         3

namespace SaXGUI {
//=====================================
// Pre-class definitions...
//-------------------------------------
class SCCMonitorDualModel;

//====================================
// Class SCCMonitorDual
//------------------------------------
class SCCMonitorDual : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	bool           mNeedImport;
	int            mDisplay;
	QString        mMonitorModel;
	QString        mMonitorVendor;
	QString        mSelectedResolution;
	QList<QString> mSelectedResolutionList;
	int            mDualHeadMode;
	int            mDualHeadArrangement;

	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString> mResolutionDict;
	Q3Dict<QString> mProfileDriverOptions;

	private:
	Q3VBox*         mCardTab;
	
	private:
	Q3TabDialog*    mDualHeadDialog;
	Q3ButtonGroup*  mChangeGroup;
	Q3HBox*         mMoniBox;
	QLabel*        mLabelSecondMonitor;
	QPushButton*   mChangeMonitor;
	Q3HBox*         mResBox;
	QLabel*        mLabelSecondResolution;
	QComboBox*     mResolution;
	Q3ButtonGroup*  mModeGroup;
	QRadioButton*  mClone;
	QRadioButton*  mXinerama;
	Q3ButtonGroup*  mOrientationGroup;
	QPushButton*   mLeftOfPrimary;
	QPushButton*   mRightOfPrimary;
	QPushButton*   mAboveOfPrimary;
	QPushButton*   mBelowOfPrimary;
	SCCMonitorDualModel* mChangeMonitorDualModelDialog;

	public:
    void show ( void );

	public:
	int getMode   ( void );
	int getLayout ( void );
	QString getResolution   ( void );
	QList<QString> getResolutionList ( void );
	QPixmap getLayoutPixmap ( void );
	SCCMonitorDualModel* getDualModelData ( void );
	void setTitle ( const QString& );
	void setMonitorName ( const QString&,const QString& );
	void setResolution ( const QString& );

	private:
	QList<QString> setupList ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotDualHead ( QPushButton* );
	void slotMonitor  ( void );
	void slotOk       ( void );
	void slotClone    ( bool );

	signals:
	void sigDualModeChanged (SCCMonitorDisplay*);

	public:
	SCCMonitorDual (
		Q3Dict<QString>*,Q3Dict<SaXImport>,
		const QString&,int,QWidget*
	);
};
} // end namespace

//=====================================
// Includes
//-------------------------------------
#include "monitordualmodel.h"

#endif
