/**************
FILE          : frame.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning too)
              : frame is the base application window widget
              : which is opened on each screen available
              :
              :
STATUS        : Status: Up-to-date
**************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <q3mainwindow.h>
#include <q3popupmenu.h>
#include <q3toolbar.h>
#include <qstatusbar.h>
#include <qregexp.h>
#include <qmenubar.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3Frame>
#include <QEvent>
#include <Q3VBoxLayout>
#include <locale.h>
#include <libintl.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <X11/Xlib.h>

#ifndef LONG64
#define LONG64
#include <X11/extensions/xf86vmode.h>
#else
#include <X11/extensions/xf86vmode.h>
#endif

#include "frame.h"
#include "xquery.h"

//=====================================
// XFineWindow Constructor...
//-------------------------------------
XFineWindow::XFineWindow (int x,int y,int screen) : 
	Q3MainWindow(0,"XFine2",Qt::WDestructiveClose) {
	// ...
	// load the gettext hash for international
	// language support
	// ---
	mAdjustable = true;
	mTextPtr = new Q3Dict<QString>;
	loadText ();

	// ...
	// check if the screen is adjustable via vidmode
	// extension
	// ---
	XQuery adjustable;
	adjustable.setOption ("-j");
	QString* data = adjustable.run();
	XStringList adjustData (*data);
	adjustData.setSeperator ("\n");
	QList<const char*> adjustList = adjustData.getList();
	QString currentAdjustable (
		adjustList.at (screen)
	);
	currentAdjustable = currentAdjustable.right(
		currentAdjustable.length() - 2
	);
	if (currentAdjustable == "no") {
		mAdjustable = false;
	}

	// ...
	// create the GUI for one screen
	// ---
	bool has3D = hasDirectRendering ( screen );
	setFrame (x,y,mAdjustable,has3D);
	installEventFilter (this);

	// ...
	// init GUI with currently active screen
	// setup paramenters
	// ---
	mScreen = screen;
	slotNormal ();
	initScreen (
		true , mAdjustable
	);
}

//=====================================
// XFineWindow create and init GUI...
//-------------------------------------
void XFineWindow::setFrame (
	int x,int y, bool adjustable,bool has3D
) {
	XWrapText< Q3Dict<QString> > mText (mTextPtr);
	setCaption ("XFine2");
	setGeometry (
		x - 225, y - 150, 450,300
	);
	delete (statusBar());
	step = new Q3PopupMenu (this);
	//----------------------------------------------
	// create step menu and menu slots
	//==============================================
	menuBar()->insertItem( mText["Stepsize"],step );
	step -> polish();
	step -> setCheckable ( true );
	for (int i=0;i<4;i++) {
		stepST[i] = false;
	}
	stepID[0] = step -> insertItem (mText["small"] , this,
		SLOT ( slotSmall(void) )
	);
	stepID[1] = step -> insertItem (mText["normal"], this,
		SLOT ( slotNormal(void) )
	);
	stepID[2] = step -> insertItem (mText["big"]   , this,
		SLOT ( slotBig(void) )
	);
	stepID[3] = step -> insertItem (mText["huge"]  , this,
		SLOT ( slotHuge(void) )
	);
	//----------------------------------------------
	// create layout structure...
	//==============================================
	mFrame = new Q3Frame (this);
	// topview...
	//=====================================
	Q3BoxLayout*  layer1  = new Q3VBoxLayout (mFrame);
	layer1 -> setMargin (10);

	// main view, seperator and buttonbar
	// ====================================
	Q3BoxLayout*  layer2  = new Q3HBoxLayout (layer1);
	layer1 -> addSpacing (10);
    Q3BoxLayout*  layer3  = new Q3HBoxLayout (layer1);
	layer1 -> addSpacing (10);
	Q3BoxLayout*  layer4  = new Q3HBoxLayout (layer1);

	// tab and status bar
	// ====================================
	Q3VBox* size = new Q3VBox (mFrame);
	Q3VBox* pos  = new Q3VBox (mFrame);
	Q3Frame* posFrame  = new Q3Frame (pos);
	Q3Frame* sizeFrame = new Q3Frame (size);
	Q3BoxLayout*  layer6  = new Q3VBoxLayout (layer2);
	Q3GridLayout* layer7  = new Q3GridLayout (sizeFrame,3,3);
	Q3GridLayout* layer8  = new Q3GridLayout (posFrame,3,3);

	//----------------------------------------------
	// create widgets...
	//==============================================
	// 3D label...
	QLabel* sizeIndicator3D = new QLabel (sizeFrame);
	QLabel* posIndicator3D  = new QLabel (posFrame);
	if (has3D) {
		QPixmap* DRIenabled  = new QPixmap (DRI_ENABLED);
		sizeIndicator3D -> setPixmap (*DRIenabled);
		posIndicator3D  -> setPixmap (*DRIenabled);
	} else {
		QPixmap* DRIdisabled = new QPixmap (DRI_DISABLED);
		sizeIndicator3D -> setPixmap (*DRIdisabled);
		posIndicator3D  -> setPixmap (*DRIdisabled);
	}
	// seperator...
	Q3Frame *seperator = new Q3Frame(mFrame);
	seperator -> setFixedHeight(2);
	seperator -> setFrameStyle(Q3Frame::HLine|Q3Frame::Raised);
	// button bar...
	mCancel = new QPushButton (mText["Cancel"], mFrame);
	mSave   = new QPushButton (mText["Save"], mFrame);
	mSave -> setDefault (true);
	mSave -> setFocus();
	// tab widget...
	mTab = new QTabWidget ( mFrame );
	mTab -> setMargin (10);
	mTab -> addTab ( pos,  mText["ChangePosition"] );
	mTab -> addTab ( size, mText["ChangeSize"] );
	// status bar...
	mStatus = new QStatusBar ( mFrame );
	// size buttons...
	QPushButton* narrower = new QPushButton (sizeFrame);
	narrower -> setFixedSize (50,50);
	narrower -> setPixmap (
		QPixmap(PIXNARROWER)
	);
	QPushButton* wider    = new QPushButton (sizeFrame);
	wider -> setFixedSize (50,50);
	wider -> setPixmap (
		QPixmap(PIXWIDER)
	);
	QPushButton* taller   = new QPushButton (sizeFrame);
	taller -> setFixedSize (50,50);
	taller -> setPixmap (
		QPixmap(PIXTALLER)
	);
	QPushButton* shorter  = new QPushButton (sizeFrame);
	shorter -> setFixedSize (50,50);
	shorter -> setPixmap (
		QPixmap(PIXSHORTER)
	);
	// position buttons...
	QPushButton* left  = new QPushButton (posFrame);
	left -> setFixedSize (50,50);
	left -> setPixmap (
		QPixmap(PIXLEFT)
	);
	QPushButton* right = new QPushButton (posFrame);
	right -> setFixedSize (50,50);
	right -> setPixmap (
		QPixmap(PIXRIGHT)
	);
	QPushButton* up    = new QPushButton (posFrame);
	up -> setFixedSize (50,50);
	up -> setPixmap (
		QPixmap(PIXUP)
	);
	QPushButton* down  = new QPushButton (posFrame);
	down -> setFixedSize (50,50);
	down -> setPixmap (
		QPixmap(PIXDOWN)
	);

	//----------------------------------------------
	// connect widgets...
	//==============================================
	QObject::connect(
		mCancel  , SIGNAL (clicked     ()),
		this     , SLOT   (slotCancel  ())
	);
	QObject::connect(
		mSave    , SIGNAL (clicked     ()),
		this     , SLOT   (slotSave    ())
	);
	QObject::connect(
		narrower , SIGNAL (clicked     ()),
		this     , SLOT   (slotNarrower())
	);
	QObject::connect(
		wider    , SIGNAL (clicked     ()),
		this     , SLOT   (slotWider   ())
	);
	QObject::connect(
		taller   , SIGNAL (clicked     ()),
		this     , SLOT   (slotTaller  ())
	);
	QObject::connect(
		shorter  , SIGNAL (clicked     ()),
		this     , SLOT   (slotShorter ())
	);
	QObject::connect(
		left     , SIGNAL (clicked     ()),
		this     , SLOT   (slotLeft    ())
	);
	QObject::connect(
		right    , SIGNAL (clicked     ()),
		this     , SLOT   (slotRight   ())
	);
	QObject::connect(
		up       , SIGNAL (clicked     ()),
		this     , SLOT   (slotUp      ())
	);
	QObject::connect(
		down     , SIGNAL (clicked     ()),
		this     , SLOT   (slotDown    ())
	);

	//==============================================
	// add widgets to the layout...
	//----------------------------------------------
	layer3 -> addWidget  ( seperator );
	layer4 -> addWidget  ( mCancel );
	layer4 -> addStretch ( 10 );
	layer4 -> setSpacing ( 10 );
	layer4 -> addWidget  ( mSave );
	layer6 -> addWidget  ( mTab );
	layer6 -> addWidget  ( mStatus );
	layer7 -> addWidget  ( narrower,1,0 );
	layer7 -> addWidget  ( wider,   1,2 );
	layer7 -> addWidget  ( taller,  0,1 );
	layer7 -> addWidget  ( shorter, 2,1 );
	layer8 -> addWidget  ( left  ,  1,0 ); 
	layer8 -> addWidget  ( right ,  1,2 );
	layer8 -> addWidget  ( up    ,  0,1 );
	layer8 -> addWidget  ( down  ,  2,1 );

	//==============================================
	// set 3D indicators to the middle of the grid
	//----------------------------------------------
	layer7 -> addWidget  ( sizeIndicator3D , 1,1 );
	layer8 -> addWidget  ( posIndicator3D  , 1,1 );

	//==============================================
	// disable dialog if not vidmode adjustable...
	//----------------------------------------------
	if ( ! adjustable) {
		menuBar() -> setDisabled (true);
		mTab -> setDisabled (true);
	}

	setCentralWidget ( mFrame );
}

//=====================================
// XFineWindow setup screen...
//-------------------------------------
void XFineWindow::initScreen (bool startup, bool adjustable) {
	XWrapText< Q3Dict<QString> > mText (mTextPtr);
	
	XQuery frq;
	frq.setOption ("-f");
	// ...
	// get currently used frequencies
	// ---
	QString* data = frq.run();
	XStringList frqData (*data);
	frqData.setSeperator ("\n");
	QList<const char*> frqList = frqData.getList();
	QString currentFrequency (
		frqList.at (mScreen)
	);
	currentFrequency = currentFrequency.right(
		currentFrequency.length() - 2
	);
	// ...
	// get currently used modeline timings
	// ---
	XQuery mode;
	mode.setOption ("-m");
	data = mode.run();
	data->replace (QRegExp("\""),"");
	XStringList modeData (*data);
	modeData.setSeperator ("\n");
	QList<const char*> modeList = modeData.getList();
	QString currentMode (
		modeList.at (mScreen)
	);
	if (currentMode.isEmpty()) {
		log (L_INFO,
			"XFineWindow::No mode information for screen: %d\n",mScreen
		);
		exit (0);
	}
	XStringList modeline (currentMode);
	modeline.setSeperator (" ");
	QList<const char*> timing = modeline.getList();
	// ...
	// disable next screen button if only one
	// screen is available
	// ---
	if (startup) {
	if ((int)(frqList.count() -1) == mScreen) {
		// ...
		// activate xidle timer by sending SIGUSR2
		// to all xapi --xidle processes
		// ---
		QString sig  ("-12");
		QString proc ("xapi");
		qx (
			KILLALL,STDERR,2,"%s %s",sig.ascii(),proc.ascii()
		);
	}
	// ...
	// save currently used modeline timings
	// ---
	QList<const char*> timing = modeline.getList();
	for (int count=0;count < (int)timing.count();count++) {
	int value = QString (timing.at(count)).toInt();
	switch (count) {
		case 0:
			mFileName.sprintf ("%s/%s:%s",
				CACHE,timing.at(count),timing.at(count+1)
			);
		break;
		case 2:
			mClock.sprintf("%s",timing.at(count));
		break;
		case 3:
			mHDisplay       = value;
			mHDisplaySave   = value;
		break;
		case 4:
			mHSyncStart     = value;
			mHSyncStartSave = value;
		break;	
		case 5:
			mHSyncEnd       = value;
			mHSyncEndSave   = value;
		break;
		case 6:
			mHTotal         = value;
			mHTotalSave     = value;
		break;
		case 7:
			mVDisplay       = value;
			mVDisplaySave   = value;
		break;
		case 8:
			mVSyncStart     = value;
			mVSyncStartSave = value;
		break;
		case 9:
			mVSyncEnd       = value;
			mVSyncEndSave   = value;
		break;
		case 10:
			mVTotal         = value;
			mVTotalSave     = value;
		break;
	}
	}
	}
	QString infoMessage;
	if (adjustable) {
		QTextOStream (&infoMessage)
			<< mText["Screen"] << " [ " << mScreen << " ] : " 
			<< timing.at(1) << " @ " << currentFrequency;
	} else {
		QTextOStream (&infoMessage)
			<< mText["Screen"] << " [ " << mScreen << " ] : "
			<< mText["NoVidtune"];
	}
	mStatus -> message ( infoMessage );
}

//=====================================
// XFineWindow save mode to CACHE...
//-------------------------------------
void XFineWindow::saveMode (void) {
	if ( ! mAdjustable ) {
		log (L_INFO,
			"XFineWindow::saveMode: not adjustable, abort saving"
		);
		return;
	}
	if (mFileName.isEmpty()) {
		return;
	}
	QFile* mHandle = new QFile (mFileName);
	if (! mHandle -> open(QIODevice::WriteOnly)) {
	log (L_ERROR,
		"XFineWindow::open failed on: %s -> %s\n",
		mFileName.ascii(),strerror(errno)
	);
	return;
	}
	QString line;
	line.sprintf ("%d:%d %d %d %d %d %d %d %d:%d %d %d %d %d %d %d %d:%s",
		mScreen,
		mHDisplaySave,mHSyncStartSave,mHSyncEndSave,mHTotalSave,
		mVDisplaySave,mVSyncStartSave,mVSyncEndSave,mVTotalSave,
		mHDisplay,mHSyncStart,mHSyncEnd,mHTotal,
		mVDisplay,mVSyncStart,mVSyncEnd,mVTotal,
		mClock.ascii()
	);
	mHandle -> writeBlock (
		line.ascii(),line.length()
	);
	mHandle -> close();
}

//=====================================
// XFineWindow save...
//-------------------------------------
void XFineWindow::slotSave (void) {
	kill (getppid(),SIGHUP);
}

//=====================================
// XFineWindow abort...
//-------------------------------------
void XFineWindow::slotCancel (void) {
	kill (getppid(),SIGTERM);
}

//=====================================
// XFineWindow small steps...
//-------------------------------------
void XFineWindow::slotSmall (void) {
	updateStep(0);
}

//=====================================
// XFineWindow normal steps...
//-------------------------------------
void XFineWindow::slotNormal (void) {
	updateStep(1);
}

//=====================================
// XFineWindow big steps...
//-------------------------------------
void XFineWindow::slotBig (void) {
	updateStep(2);
}

//=====================================
// XFineWindow huge steps...
//-------------------------------------
void XFineWindow::slotHuge (void) {
	updateStep(3);
}

//=====================================
// XFineWindow only one step selected...
//-------------------------------------
void XFineWindow::updateStep (int item) {
	for (int i=0;i<4;i++) {
		step -> setItemChecked (stepID[i],false);
	}
	if (! stepST[item]) {
		stepST[item] = true;
	} else {
		stepST[item] = false;
	}
	step -> setItemChecked ( 
		stepID[item],true
	);
	mStep = (item * 8) + 8;
}

//=====================================
// XFineWindow slotNarrower...
//-------------------------------------
void XFineWindow::slotNarrower (void) {
	resize (XNarrower);
}

//=====================================
// XFineWindow slotWider...
//-------------------------------------
void XFineWindow::slotWider (void) {
	resize (XWider);
}

//=====================================
// XFineWindow slotTaller...
//-------------------------------------
void XFineWindow::slotTaller (void) {
	resize (XTaller);
}

//=====================================
// XFineWindow slotShorter...
//-------------------------------------
void XFineWindow::slotShorter (void) {
	resize (XShorter);
}

//=====================================
// XFineWindow slotLeft...
//-------------------------------------
void XFineWindow::slotLeft (void) {
	resize (XLeft);
}

//=====================================
// XFineWindow slotRight...
//-------------------------------------
void XFineWindow::slotRight (void) {
	resize (XRight);
}

//=====================================
// XFineWindow slotUp...
//-------------------------------------
void XFineWindow::slotUp (void) {
	resize (XUp);
}

//=====================================
// XFineWindow slotDown...
//-------------------------------------
void XFineWindow::slotDown (void) {
	resize (XDown);
}

//=====================================
// XFineWindow gettext i18n...
//-------------------------------------
void XFineWindow::loadText (void) {
	Q3Dict<char> gtx;
	XFile gtxHandle (XFINEGTX);

	setlocale (LC_ALL,"");
	bindtextdomain ("sax", TDOMAIN);
	textdomain ("sax");

	gtx = gtxHandle.gtxRead();
	Q3DictIterator<char> it(gtx);
	for (; it.current(); ++it) {
		QString* translation = new QString(
			gettext((const char*)it.current())
		);
		mTextPtr->insert (it.currentKey(),translation);
	}
}

//=====================================
// XFineWindow resize screen...
//-------------------------------------
void XFineWindow::resize ( Direction flag ) {
	QString mode;
	int HDisplaySave   = mHDisplay;
	int HSyncStartSave = mHSyncStart;
	int HSyncEndSave   = mHSyncEnd;
	int HTotalSave     = mHTotal;
	int VDisplaySave   = mVDisplay;
	int VSyncStartSave = mVSyncStart;
	int VSyncEndSave   = mVSyncEnd;
	int VTotalSave     = mVTotal;
	switch (flag) {
		case XLeft:
			mHSyncStart += mStep;
			mHSyncEnd   += mStep;
		break;
		case XRight:
			mHSyncStart -= mStep;
            mHSyncEnd   -= mStep;
		break;
		case XUp:
			mVSyncStart += mStep;
			mVSyncEnd   += mStep;
		break;
		case XDown:
			mVSyncStart -= mStep;
			mVSyncEnd   -= mStep;
		break;
		case XNarrower:
			mHTotal     += mStep;
		break;
		case XTaller:
			mVTotal     -= mStep;
		break;
		case XWider:
			mHTotal     -= mStep;
		break;
		case XShorter:
			mVTotal     += mStep;
		break;
		case XAbort:
			mHDisplay    = mHDisplaySave;
			mHSyncStart  = mHSyncStartSave;
			mHSyncEnd    = mHSyncEndSave;
			mHTotal      = mHTotalSave;
			mVDisplay    = mVDisplaySave;
			mVSyncStart  = mVSyncStartSave;
			mVSyncEnd    = mVSyncEndSave;
			mVTotal      = mVTotalSave;
		break;
	}
	mode.sprintf("%d %d %d %d %d %d %d %d %d",
		mScreen,mHDisplay,mHSyncStart,mHSyncEnd,mHTotal,
		mVDisplay,mVSyncStart,mVSyncEnd,mVTotal
	);
	XQuery apply;
	apply.setOption ("-a");
	apply.setArguments (mode);
	apply.useErrorHandler();
	QString *error = apply.run();
	bool invalid = false;
	if (
		mHSyncStart < mHDisplay   ||
		mHSyncEnd   < mHSyncStart ||
		mHTotal     < mHSyncEnd   ||
		mVSyncStart < mVDisplay   ||
		mVSyncEnd   < mVSyncStart ||
		mVTotal     < mVSyncEnd
	) {
		invalid = true;
	}
	if (! invalid) {
		XF86VidModeMonitor monitor;
		if (XF86VidModeGetMonitor (x11Display(),mScreen,&monitor)) {
			int dot_clock;
			float hsmax = monitor.hsync[0].hi;
			float hsmin = monitor.hsync[0].lo;
			float vsmax = monitor.vsync[0].hi;
			float vsmin = monitor.vsync[0].lo;
			XF86VidModeModeLine mode_line;
			if (!XF86VidModeGetModeLine (
				x11Display(), mScreen, &dot_clock, &mode_line)
			) {
				invalid = true;
			}
			double dc = (double)dot_clock * 1000;
			double hs = dc / (double)mHTotal;
			double vs = dc / (double)(mHTotal * mVTotal);
			hs = hs / 1000;
			if ((hs > hsmax) || (hs < hsmin)) {
				invalid = true;
			}
			if ((vs > vsmax) || (vs < vsmin)) {
				invalid = true;
			}
		}
	}
	if ((! error -> isEmpty()) || (invalid)) {
		mHDisplay    = HDisplaySave;
		mHSyncStart  = HSyncStartSave;
		mHSyncEnd    = HSyncEndSave;
		mHTotal      = HTotalSave;
		mVDisplay    = VDisplaySave;
		mVSyncStart  = VSyncStartSave;
		mVSyncEnd    = VSyncEndSave;
		mVTotal      = VTotalSave;
		return;
	}
	if (flag != XAbort) {
		initScreen (false,mAdjustable);
	}
}

//=====================================
// XFrame set global event filter
//-------------------------------------
bool XFineWindow::eventFilter ( QObject* obj, QEvent* event ) {
	if ((QPushButton*)obj != NULL) {
		QPushButton* btn = (QPushButton*)qApp->focusWidget();
		if (! btn) {
			return (false);
		}
		switch (event->type()) {
		//======================================
		// handle press event
		//--------------------------------------
		case 6:
		if (((QKeyEvent*)event)->key() == Qt::Key_Return) {
			QKeyEvent spacePress ( (QEvent::Type)6, Qt::Key_Space, 32 ,0 );
			QApplication::sendEvent ( (QObject*)btn,&spacePress );
			return (true);
		}
		break;
		//======================================
		// handle release event
		//--------------------------------------
		case 7:
		if (((QKeyEvent*)event)->key() == Qt::Key_Return) {
			QKeyEvent spaceRelease ( (QEvent::Type)7, Qt::Key_Space, 32 ,0 );
			QApplication::sendEvent ( (QObject*)btn,&spaceRelease );
			return (true);
		}
		break;
		default:
		break;
		}
	}
	return (false);
}

