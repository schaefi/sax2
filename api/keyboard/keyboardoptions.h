/**************
FILE          : keyboardoptions.h
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
              : - keyboardoptions.h: SaXGUI::SCCKeyboardOptions header defs
              : - keyboardoptions.cpp: configure XKB keyboard options
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCKEYBOARD_OPTIONS_H
#define SCCKEYBOARD_OPTIONS_H 1

//=====================================
// Includes
//-------------------------------------
#include <q3listview.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCKeyboardOptions
//------------------------------------
class SCCKeyboardOptions : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	QString          mXKBOption;
	QString          mDefaultOption;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	Q3Dict<QString>   mOptionDict;

	private:
	Q3BoxLayout*      mMainLayout;

	private:
	Q3ListView*       mXKBOptionView;
	Q3CheckListItem*  mIDGroupShiftLock;
	Q3CheckListItem*  mIDKbdLedToShow;
	Q3CheckListItem*  mIDCompose;
	Q3CheckListItem*  mIDControl;
	Q3CheckListItem*  mIDThirdLevel;
	Q3CheckListItem*  mIDAltWinKey;
	Q3CheckListItem*  mIDEuroSign;
	Q3CheckListItem*  mIDEuroSignNone;
	Q3CheckListItem*  mIDCapsLock;
	Q3CheckListItem*  mIDCapsLockNone;
	Q3CheckListItem*	 mIDAltWinKeyNone;
	Q3CheckListItem*  mIDControlNone;

	private:
	void updateTree ( Q3ListViewItem*,bool );
	QString findOption ( const QString& );
	QString translateOption ( const QString& );

	public:
	QString getOptions     ( void );
	QString getApplyString ( void );
	QString getDefaultOption ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotXKBOption ( Q3ListViewItem* );

	signals:
	void sigApply ( void );

	public:
	SCCKeyboardOptions (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace

#endif
