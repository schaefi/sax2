/**************
FILE          : monitorcard.h
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
              : - monitorcard.h: SaXGUI::SCCMonitor header definitions
              : - monitorcard.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMONITOR_CARD_H
#define SCCMONITOR_CARD_H 1

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
#include <q3listbox.h>
#include <qspinbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCMonitorCard
//------------------------------------
class SCCMonitorCard : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	QString        mRotateSetup;
	Q3Dict<QString> mSelectedOptions;
	bool           mNeedImport;
	int            mDisplay;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString> mProfileDriverOptions;
	Q3Dict<QString> mOptDict;

	private:
	Q3HBox*         mCardTab;

	private:
	Q3TabDialog*    mCardDialog;
	Q3ButtonGroup*  mOptionGroup;
	Q3ListBox*      mOption;
	Q3ButtonGroup*  mRotateGroup;
	QRadioButton*  mRotateNot;
	QRadioButton*  mRotateLeft;
	QRadioButton*  mRotateRight;

	public:
	void show ( void );

	public:
	Q3Dict<QString> getOptions ( void );
	int getRotate ( void );
	void setTitle ( const QString& );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotOk     ( void );
	void slotOption ( Q3ListBoxItem* );

	public:
	SCCMonitorCard (
		Q3Dict<QString>*,Q3Dict<SaXImport>,
		const QString&,int,QWidget*
	);
};
} // end namespace
#endif
