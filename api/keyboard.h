/**************
FILE          : keyboard.h
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
              : - keyboard.h: SaXGUI::SCCKeyboard header definitions
              : - keyboard.cpp: configure keyboard system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCKEYBOARD_H
#define SCCKEYBOARD_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QLabel>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "keyboard/keyboardlayout.h"
#include "keyboard/keyboardoptions.h"

namespace SaXGUI {
//====================================
// Class SCCKeyboard
//------------------------------------
class SCCKeyboard : public SCCDialog {
	Q_OBJECT

	private:
	QTabWidget*         mKeyboardTab;
	SCCKeyboardOptions* mKeyboardOptions;
	SCCKeyboardLayout*  mKeyboardLayout;
	QLabel*             mTestLabel;
	QLineEdit*          mTestField;

	public:
	bool exportData ( void );
	void init   ( void );
	void import ( void );

	public slots:
	void apply  ( void );
	void applyDefault ( void );

	public:
	SCCKeyboard (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
