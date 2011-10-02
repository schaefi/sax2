/**************
FILE          : monitorarrange.h
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
              : - monitorarrange.h: SaXGUI::SCCMonitor header definitions
              : - monitorarrange.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_ARRANGE_H
#define SCCMONITOR_ARRANGE_H 1

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
#include <q3scrollview.h>
//Added by qt3to4:
#include <QPixmap>

//=====================================
// Includes
//-------------------------------------
#include "monitordisplay.h"
#include "monitorplot.h"
#include "dialog.h"

//=====================================
// Defines...
//-------------------------------------
#define TIP_PIXDIR   "/usr/share/sax/api/figures"
#define TIP_PIXMAP   TIP_PIXDIR "/tooltip.png"

namespace SaXGUI {
//====================================
// Class SCCMonitorArrange
//------------------------------------
class SCCMonitorArrange : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	int  mLayoutMode;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	int* mCardID;

	private:
	Q3BoxLayout*    mMainLayout;

	private:
	Q3ButtonGroup*  mModusGroup;
	QRadioButton*  mTraditional;
	QRadioButton*  mClone;
	QRadioButton*  mXinerama;
	Q3ButtonGroup*  mLayoutGroup;
	SCCPlot*       mMatrix;

	public:
	void setArrangement (
		int,QList<SCCMonitorDisplay*>
	);
	void setCleanArrangement (
		int,QList<SCCMonitorDisplay*>
	);
	void setFigurePixmap  (
		int, QPixmap
	);
	void setToolTip ( SCCMonitorDisplay* );
	int  getMultiheadMode ( void );
	Q3Dict<QString> getArrangement ( void );

	public:
	void init   ( void );
	void import ( void );

	private:
	void setDeviceMap (
		int,QList<SCCMonitorDisplay*>
	);

	public slots:
	void slotMonitorChanged  ( SCCMonitorDisplay* );
	void slotDualModeChanged ( SCCMonitorDisplay* );

	public:
	SCCMonitorArrange (
		Q3Dict<QString>*, Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
