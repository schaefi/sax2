/**************
FILE          : dialog.h
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
              : - dialog.h: SaXGUI::SCCDialog header definitions
              : - dialog.cpp: build base class for all dialogs
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCDIALOG_H
#define SCCDIALOG_H 1

#include <qwidget.h>
#include <q3widgetstack.h>
#include <qlayout.h>
#include <q3dict.h>
#include <q3process.h>
//Added by qt3to4:
#include <Q3Frame>

#include "../libsax/sax.h"

#include "common/log.h"
#include "wrapper.h"
#include "message.h"
#include "profile.h"
#include "file.h"

namespace SaXGUI {
//====================================
// Class SCCDialog
//------------------------------------
class SCCDialog : public QWidget {
	Q_OBJECT

	private:
	Q3WidgetStack* mDialogStack;
	int mDialogID;

	protected:
	QWidget*         mParent;
	Q3Dict<QString>*  mTextPtr;
	Q3Dict<SaXImport> mSection;
	Q3BoxLayout*      mDialogLayout;
	Q3Frame*          mDialogFrame;

	protected:
	int  mGUIMode;
	bool needImportData;
	bool needInitData;

	public:
	virtual void init   ( void );
	virtual void import ( void );

	public:
	int  getDialogID   ( void );
	bool needInit      ( void );
	bool needImport    ( void );
	QPoint getPosition ( int,int );

	public:
	SCCDialog (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget* = 0
	);
};
} // end namespace
#endif
