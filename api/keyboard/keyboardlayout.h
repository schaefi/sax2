/**************
FILE          : keyboardlayout.h
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
              : - keyboardlayout.h: SaXGUI::SCCKeyboardLayout header defs
              : - keyboardlayout.cpp: configure keyboard layout
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCKEYBOARD_LAYOUT_H
#define SCCKEYBOARD_LAYOUT_H 1

//=====================================
// Includes
//-------------------------------------
#include <q3listview.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <q3buttongroup.h>
#include <q3listbox.h>
#include <qstringlist.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCKeyboardLayout
//------------------------------------
class SCCKeyboardLayout : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	QString          mXKBModel;
	QString          mXKBLayout;
	QString          mXKBVariant;
	QString          mDefaultLayout;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString>   mModelDict;
	Q3Dict<QString>   mLayoutDict;

	private:
	Q3BoxLayout*      mMainLayout;

	private:
	Q3ButtonGroup*    mPrimaryGroup;
	QLabel*          mLabelType;
	QComboBox*       mTypeBox;
	QLabel*          mLabelLayout;
	QComboBox*       mLayoutBox;
	QLabel*          mLabelVariant;
	QComboBox*       mVariantBox;
	Q3ButtonGroup*    mAdditionalGroup;
	Q3ListView*       mAddLayout;
	QLabel*          mLabelAddVariant;
	QComboBox*       mAddVariantBox;
	int              mViewStatus;
	int              mViewLayout;
	int              mViewKey;
	int              mViewVariant;

	private:
	void updateVariants ( void );
	QStringList translateList ( const QList<QString>& );

	public:
	QString getType    ( void );
	QString getLayout  ( void );
	QString getVariant ( void );
	QString getApplyString ( void );
	QString getDefaultLayout ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotLayout  ( int );
	void slotType    ( int );
	void slotVariant ( int );
	void slotAddVariant ( int );
	void slotAddLayout  ( Q3ListViewItem* );

	signals:
	void sigApply ( void );

	public:
	SCCKeyboardLayout (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace

#endif
