/**************
FILE          : mousedisplay.h
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
              : - mousedisplay.h: SaXGUI::SCCMouseDisplay header defs
              : - mousedisplay.cpp: configure mouse system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMOUSE_DISPLAY_H
#define SCCMOUSE_DISPLAY_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <q3vbox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <qlabel.h>
#include <qspinbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "mousetest.h"

namespace SaXGUI {
//=====================================
// Pre-class definitions
//-------------------------------------
class SCCMouseModel;

//====================================
// Class SCCMouseDisplay
//------------------------------------
class SCCMouseDisplay : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	int              mPointerID;
	int              mDisplay;
	bool             mEnabled;

	private:
	SaXManipulateMice* mSaxMouse;

	private:
	Q3BoxLayout*      mMainLayout;
	int              mMouseOptID;
    int              mSynapOptID;

	private:
	QCheckBox*       mCheckEnable;
	Q3ButtonGroup*    mMouseNameGroup;
	QLabel*          mLabelMouseName;
	QPushButton*     mChangeMouse;
	Q3ButtonGroup*    mMouseOptionGroup;
	Q3ButtonGroup*    mSynapOptionGroup;
	QCheckBox*       mCheck3BtnEmulation;
	QCheckBox*       mCheckMouseWheel;
	QCheckBox*       mCheckEmulateWheel;
	QCheckBox*       mCheckInvertXAxis;
	QCheckBox*       mCheckInvertYAxis;
	QCheckBox*       mCheckLeftHand;
	QSpinBox*        mEmulateWheelButton;
	Q3ButtonGroup*    mMouseTestGroup;
	Q3ButtonGroup*    mSynapTestGroup;
	SCCMouseTest*    mTestField;
	SCCMouseModel*   mChangeMouseModelDialog;
	Q3WidgetStack*    mOptionStack;

	public:
	bool isEnabled  ( void );
	void setEnabled ( bool = true );
	int  getDisplay ( void );
	bool isButtonEmulationEnabled ( void );
	bool isWheelEmulationEnabled  ( void );
	bool isWheelEnabled ( void );
	bool isXInverted    ( void );
	bool isYInverted    ( void );
	bool isLeftHanded   ( void );
	int  getWheelButton ( void );
	void setButtonEmulationEnabled ( bool );
	void setWheelEmulationEnabled  ( bool );
	void setWheelEnabled ( bool );
	void setWheelButton ( int );
	void setMouseName   ( const QString& );
	void setMouseOptionState ( bool );
	SCCMouseModel* getModelData ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotChangeMouse ( void );
	void slotActivateDisplay ( void );
	void slotWheelEmulation  ( void );
	void slotLeftHand ( void );

	signals:
	void sigActivate ( void );

	public:
	SCCMouseDisplay (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace

//=====================================
// Includes
//-------------------------------------
#include "mousemodel.h"

#endif
