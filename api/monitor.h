/**************
FILE          : monitor.h
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
              : - monitor.h: SaXGUI::SCCMonitor header definitions
              : - monitor.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_H
#define SCCMONITOR_H 1

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

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "monitor/monitordisplay.h"
#include "monitor/monitorarrange.h"

//=====================================
// Defines...
//-------------------------------------
#define DTA_DIR   "/usr/share/sax/api/data"
#define RES_FILE  DTA_DIR "/MonitorResolution"
#define COL_FILE  DTA_DIR "/MonitorColors"

namespace SaXGUI {
//====================================
// Class SCCMonitor
//------------------------------------
class SCCMonitor : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	int  mCard;
	bool m3DEnabled;
	bool mShow3DMessage;
	QString* mRequestedDialog;

	private:
	QList<SCCMonitorDisplay*>  mMonitorDisplay;

	private:
	QTabWidget*        mMonitorTab;
	SCCMonitorArrange* mMonitorArrange;
	QCheckBox*         mCheck3D;

	public:
	void setCommonButtonWidth ( void );
	void setRequestedDialog ( QString* );
	bool exportData ( void );
	void init   ( void );
	void import ( void );

	public slots:
	void slotActivate   ( void );
	void slotActivate3D ( void );

	private:
	int compareResolution ( const QString&,const QString& );
	void fixBrokenCards ( SaXManipulateCard& );

	public:
	SCCMonitor (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
