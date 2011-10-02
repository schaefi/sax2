/**************
FILE          : frame.h
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
              : - frame.h: SaXGUI::SCCFrame header definitions
              : - frame.cpp: builds basic layout structure
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCFRAME_H
#define SCCFRAME_H 1

//====================================
// Includes...
//------------------------------------
#include <qwidget.h>
#include <q3frame.h>
#include <q3dict.h>
#include <qstring.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <q3widgetstack.h>
#include <q3progressdialog.h>
#include <q3listview.h>
#include <q3process.h>
#include <qlabel.h>
#include <q3vbox.h>
#include <q3dict.h>
//Added by qt3to4:
#include <QEvent>
#include <QCloseEvent>

#include "../libsax/sax.h"

//====================================
// Includes...
//------------------------------------
#include "file.h"
#include "xapi.h"
#include "message.h"
#include "wrapper.h"
#include "mouse.h"
#include "monitor.h"
#include "touchscreen.h"
#include "keyboard.h"
#include "tablet.h"
#include "vnc.h"
#include "xidle.h"

namespace SaXGUI {
//====================================
// Defines...
//------------------------------------
#define FRAME_MAIN         1
#define FRAME_MODULES      3
#define FRAME_TOPIC        4

//====================================
// Defines...
//------------------------------------
#define TDOMAIN            "/usr/share/locale"
#define INFDIR             "/usr/share/sax/api/data/"
#define MACRODIR           "/usr/share/sax/api/macros/"
#define PIXDIR             "/usr/share/sax/api/figures"
#define XQUERY             "/usr/sbin/xquery"

//====================================
// Defines...
//------------------------------------
#define GTX_FILE           INFDIR   "xapi.gtx"
#define GET3D              MACRODIR "get3D"
#define GETINSTALLED       MACRODIR "getInstalled"

//====================================
// Defines...
//------------------------------------
#define TITLE_PIXMAP           PIXDIR "/logo.png"
#define POINTERS_PIXMAP        PIXDIR "/mouse.png"
#define KEYBOARD_PIXMAP        PIXDIR "/keyboard.png"
#define MONITORS_PIXMAP        PIXDIR "/monitor.png"
#define VNCSERVE_PIXMAP        PIXDIR "/vnc.png"
#define TOUCHERS_PIXMAP        PIXDIR "/touch.png"
#define XTABLETS_PIXMAP        PIXDIR "/tablet.png"

#define SMALL_POINTERS_PIXMAP  PIXDIR "/small_mouse.png"
#define SMALL_KEYBOARD_PIXMAP  PIXDIR "/small_keyboard.png"
#define SMALL_MONITORS_PIXMAP  PIXDIR "/small_monitor.png"
#define SMALL_VNCSERVE_PIXMAP  PIXDIR "/small_vnc.png"
#define SMALL_TOUCHERS_PIXMAP  PIXDIR "/small_touch.png"
#define SMALL_XTABLETS_PIXMAP  PIXDIR "/small_tablet.png"

//====================================
// Class SCCFrame
//------------------------------------
class SCCFrame : public QWidget {
	Q_OBJECT

	private:
	Q3Dict<QString>* mTextPtr;

	private:
	QString*         mRequestedDialog;
	Q3Dict<SaXImport> mSection;
	SaXConfig*       mConfig;
	bool mYaSTMode;
	int  mGUIMode;
	int mX;
	int mY;

	private:
	Q3Frame*       mMainFrame;
	Q3Frame*       mSeperator;
	Q3BoxLayout*   mMainLayout;
	Q3BoxLayout*   mTitleLayout;
	Q3BoxLayout*   mWorkLayout;
	Q3BoxLayout*   mDoneLayout;

	private:
	Q3ListBox*     mModuleList;
	Q3HBox*        mModuleTitleBox;
	Q3VBox*        mModuleTitleLabel;
	QLabel*       mModuleTitleIcon;
	QLabel*       mModuleTitle;
	Q3WidgetStack* mDialogStack;
	Q3HBox*        mModuleBox;

	private:
	QPushButton*  mHelper;
	QPushButton*  mCancel;
	QPushButton*  mFinish;

	private:
	SCCMouse*       mMouse;
	SCCKeyboard*    mKeyboard;
	SCCMonitor*     mMonitor;
	SCCTablet*      mTablet;
	SCCTouchScreen* mToucher;
	SCCVNC*         mVNC;

	private:
	void runDialog ( Q3ListBoxItem* );
	void hideFrame ( int = FRAME_MODULES );
	void loadText  ( void );
	void exitSaX   ( int = 0 );
	void setupModules    ( void );
	void loadApplication ( void );
	void saveConfiguration ( void );
	void testConfiguration ( void );
	void installConfiguration ( void );
	bool prepareConfiguration ( void );

	public:
	void evaluateAutoDetection ( void );
	void setCommonButtonWidth ( void );
	bool startDialog ( const QString& );
	Q3Dict<QString>* getTextPtr ( void );

	protected:
	virtual bool eventFilter ( QObject*, QEvent* );
	virtual void closeEvent  ( QCloseEvent* );

	public slots:
	void slotHelp   ( void );
	void slotCancel ( void );
	void slotFinish ( void );

	public slots:
	void slotSelected ( Q3ListBoxItem* );
	
	public:
	SCCFrame (
		bool, int, bool, bool,
		bool, bool, QString*, bool, QString*, Qt::WFlags=Qt::WType_TopLevel
	);
};
} // end namespace
#endif
