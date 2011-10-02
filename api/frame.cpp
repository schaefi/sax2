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
#include "frame.h"
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3Frame>
#include <QEvent>
#include <Q3VBoxLayout>
#include <QCloseEvent>
#include <QDesktopWidget>

//=====================================
// Globals
//-------------------------------------
extern QString SaXDisplay;

namespace SaXGUI {
//=====================================
// Constructor
//-------------------------------------
SCCFrame::SCCFrame (
	bool fullscreen, int mode, bool setinfo, bool checkpacs,
	bool yastmode , bool xidle, QString* xidlePID, bool middle,
	QString* requestedDialog, Qt::WFlags wflags
) : QWidget ( 0,0, wflags ) {
	// .../
	// create main frame for the complete application. This
	// constructor will first load all translations check the
	// given parameters and create the application without
	// displaying it first
	// ----
	mYaSTMode = yastmode;
	mTextPtr  = new Q3Dict<QString>;
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
	loadText();
	//=====================================
	// check text orientation
	//-------------------------------------
	if (mText["I18N"] == "RTL") {
		qApp->setReverseLayout( true );
	}
	//=====================================
	// get the middle position
	//-------------------------------------
	Q3Process* proc = new Q3Process ();
	proc -> addArgument ( XQUERY );
	proc -> addArgument ( "-M" );
	proc -> addArgument ( "--display" );
	proc -> addArgument ( SaXDisplay );
	if ( ! proc -> start() ) {
		exitSaX (1);
	}
	while (proc->isRunning()) {
		usleep (1000);
	}
	QByteArray data = proc->readStdout();
	QStringList lines = QStringList::split ("\n",data);
	QStringList position = QStringList::split (" ",lines.first());
	QStringList reslist  = QStringList::split (":",position.last());
	mX = reslist.first().toInt();
	mY = reslist.last().toInt();
	//=====================================
	// show information box if set
	//-------------------------------------
	if (setinfo) {
		int posx = mX - 225;
		int posy = mY - 75;
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::INTRO,
			"Suggestion","SuggestionCaption"
		);
		mMessageBox -> setGeometry ( posx,posy,400,150 );
		QString result = mMessageBox -> showMessage();
		if (result == mText["Cancel"]) {
			exitSaX (2);	
		}
		if (result == mText["Ok"]) {
			exitSaX (0);
		}
		exitSaX (1);
	}

	//=====================================
	// check for missing packages
	//-------------------------------------
	if (checkpacs) {
		Q3Process* proc = new Q3Process ();
		proc -> addArgument ( GET3D );
		if ( ! proc -> start() ) {
			return;
		}
		while (proc->isRunning()) {
			usleep (1000);
		}
		QByteArray data = proc->readStdout();
		QStringList lines = QStringList::split (":",data);
		QString answer,package;
		int count = 0;
		for (QStringList::Iterator it=lines.begin(); it != lines.end(); ++it) {
			QString token (*it);
			switch (count) {
				case 0: package = token; break;
				case 5: answer  = token; break;
				default:
				break;
			}
			count++;
		}
		//=====================================
		// 3D Answer was no: no check needed
		//-------------------------------------
		if (answer == "no") {
			exitSaX();
		}
		if (package != "<none>") {
			int posx = mX - 200;
			int posy = mY - 75;
			SCCMessage* mMessageBox = new SCCMessage (
				this, getTextPtr(), SaXMessage::YES_NO,
				"InstallPackage","InstallPackageCaption"
			);
			mMessageBox -> setGeometry ( posx,posy,400,150 );
			QString result = mMessageBox -> showMessage();
			if (result == mText["Yes"]) {
				Q3Process* proc = new Q3Process (); 
				proc -> addArgument ( GETINSTALLED );
				proc -> addArgument ( package );
				if ( ! proc -> start() ) { 
					return;
				}
				while (proc->isRunning()) {
					usleep (1000);
				}
			}
		}
		exitSaX();
	}

	//=====================================
	// show idle timer id requested
	//-------------------------------------
	if (xidle) {
		SCCIdleTimeElapsed* idleTimer = new SCCIdleTimeElapsed ( xidlePID );
		qApp->setMainWidget (idleTimer);
		idleTimer->show();
		qApp->exec();
	}

	//=====================================
	// set application geometry
	//-------------------------------------
	int width  = qApp->desktop()->width();
	int height = qApp->desktop()->height();
	if (! fullscreen) {
		if (middle) {
			int posx = mX - 400;
			int posy = mY - 300;
			setGeometry ( posx,posy,800,550 );
		} else {
			resize ( 800,550 );
		}
	} else {
		setGeometry ( 0,0,width,height );
	}

	//=====================================
	// install event filter
	//-------------------------------------
	installEventFilter (this);

	//=====================================
	// create layout
	//-------------------------------------
	Q3GridLayout* baseLayout = new Q3GridLayout ( this,1,1 );
	mMainFrame = new Q3Frame ( this );
	baseLayout -> addWidget ( mMainFrame,0,0 );

	//=====================================
	// create seperator
	//-------------------------------------
	mSeperator = new Q3Frame ( mMainFrame );
	mSeperator -> setFixedHeight (2);
	mSeperator -> setFrameStyle (
		Q3Frame::HLine | Q3Frame::Raised
	);

	//=====================================
	// create area panels
	//-------------------------------------
	mMainLayout = new Q3VBoxLayout  ( mMainFrame );
	mMainLayout -> setMargin  (5);
	mTitleLayout= new Q3HBoxLayout ( mMainLayout );
	mWorkLayout = new Q3HBoxLayout ( mMainLayout );
	//mMainLayout -> addSpacing (10);
	mMainLayout -> addWidget      ( mSeperator  );
	mMainLayout -> addSpacing (5);
	mDoneLayout = new Q3HBoxLayout ( mMainLayout );

	//=====================================
	// create module listbox (mWorkLayout)
	//-------------------------------------
	mModuleBox  = new Q3HBox ( mMainFrame );
	mModuleList = new Q3ListBox ( mModuleBox );
	QLabel* moduleSpace = new QLabel ( mModuleBox );
	moduleSpace -> setFixedWidth ( 10 );
	mModuleList -> setMinimumWidth ( 190 );
	mModuleList -> setFrameStyle (
		Q3Frame::Box | Q3Frame::Raised
	);

	//=====================================
	// create dialog vbox (mWorkLayout)
	//-------------------------------------
	Q3VBox* mDialogBox = new Q3VBox ( mMainFrame );
	mDialogBox -> setFrameStyle (
		Q3Frame::Box | Q3Frame::NoFrame
	);
	if (qApp->desktop()->height() > 600) {
		mModuleTitleBox  = new Q3HBox  ( mDialogBox );
		mModuleTitleIcon = new QLabel ( mModuleTitleBox );
		mModuleTitleBox -> setSpacing ( 10 );
		mModuleTitleLabel = new Q3VBox ( mModuleTitleBox );
		mModuleTitleBox -> setStretchFactor ( mModuleTitleLabel, 20 );
		mModuleTitle = new QLabel ( mModuleTitleLabel );
		QFont mTitleFont (font());
		mTitleFont.setPointSize (font().pointSize() + 5 );
		mModuleTitle -> setFont ( mTitleFont );
	}
	//=====================================
	// create widget stack (mWorkLayout)
	//-------------------------------------
	mDialogBox -> setSpacing ( 15 );
	mDialogStack = new Q3WidgetStack ( mDialogBox );
	mDialogBox -> setStretchFactor  ( mDialogStack,20 );

	//=====================================
	// create bottom btn bar (mDoneLayout)
	//-------------------------------------
	mHelper = new QPushButton ( mText["Help"]  , mMainFrame );
	mFinish = new QPushButton ( mText["Ok"]    , mMainFrame );
	mCancel = new QPushButton ( mText["Cancel"], mMainFrame );

	//=====================================
	// add widgets to the layout structure
	//-------------------------------------
	mWorkLayout  -> addWidget  ( mModuleBox );
	mWorkLayout  -> addWidget  ( mDialogBox );
	mWorkLayout  -> setStretchFactor ( mDialogBox,3 );
	mDoneLayout  -> addWidget  ( mHelper );
	mDoneLayout  -> addStretch ( 100 );
	mDoneLayout  -> addWidget  ( mFinish );
	mDoneLayout  -> addSpacing ( 10 );
	mDoneLayout  -> addWidget  ( mCancel );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect(
		mCancel  , SIGNAL (clicked   ()),
		this     , SLOT   (slotCancel())
	);
	QObject::connect(
		mFinish  , SIGNAL (clicked   ()),
		this     , SLOT   (slotFinish())
	);
	QObject::connect(
		mHelper  , SIGNAL (clicked   ()),
		this     , SLOT   (slotHelp  ())
	);
	QObject::connect(
		mModuleList, SIGNAL (selectionChanged ( Q3ListBoxItem* )),
		this       , SLOT   (slotSelected     ( Q3ListBoxItem* ))
	);

	//=====================================
	// set window title
	//-------------------------------------
	setCaption (mText["GUICaption"]);

	//=====================================
	// set import mode
	//-------------------------------------
	mGUIMode = mode;

	//=====================================
	// save dialog request if any
	//-------------------------------------
	mRequestedDialog = requestedDialog;

	//=====================================
	// setup modules
	//-------------------------------------
	setupModules();

	//=====================================
	// setup main focus
	//-------------------------------------
	mFinish -> setFocus();

	// disabled until SuSE's help strategy is clear
	mHelper -> setDisabled ( true );
	mHelper -> hide ();
}

//=====================================
// setCommonButtonWidth
//-------------------------------------
void SCCFrame::setCommonButtonWidth ( void ) {
	QPushButton* completionWidgets[] = {
		mHelper,mFinish,mCancel
	};
	int fixedWidth = 0;
	for (int i=0;i<3;i++) {
	if (completionWidgets[i]->width() > fixedWidth) {
		fixedWidth = completionWidgets[i]->width();
	}
	}
	for (int i=0;i<3;i++) {
		completionWidgets[i]->setFixedWidth (fixedWidth);
	}
	mMonitor->setCommonButtonWidth();
}

//=====================================
// setupModules
//-------------------------------------
void SCCFrame::setupModules ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());

	//=====================================
	// insert modules for selection
	//-------------------------------------
	mModuleList -> insertItem (
		QPixmap(MONITORS_PIXMAP), mText["MonitorModule"]
	);
#if 0
	mModuleList -> insertItem (
		QPixmap(POINTERS_PIXMAP), mText["MouseModule"]
	);
	mModuleList -> insertItem (
		QPixmap(KEYBOARD_PIXMAP), mText["KeyboardModule"]
	);
#endif
	mModuleList -> insertItem (
		QPixmap(XTABLETS_PIXMAP), mText["TabletModule"]
	);
	mModuleList -> insertItem (
		QPixmap(TOUCHERS_PIXMAP), mText["ToucherModule"]
	);
#if 0
	mModuleList -> insertItem (
		QPixmap(VNCSERVE_PIXMAP), mText["VNCModule"]
	);
#endif

	//=====================================
	// import all configuration data
	//-------------------------------------
	loadApplication ();

	//=====================================
	// create dialog widgets
	//-------------------------------------
	mMonitor  = new SCCMonitor     ( mDialogStack,getTextPtr(),mSection,this );
	mMouse    = new SCCMouse       ( mDialogStack,getTextPtr(),mSection,this );
	mKeyboard = new SCCKeyboard    ( mDialogStack,getTextPtr(),mSection,this );
	mTablet   = new SCCTablet      ( mDialogStack,getTextPtr(),mSection,this );
	mToucher  = new SCCTouchScreen ( mDialogStack,getTextPtr(),mSection,this );
	mVNC      = new SCCVNC         ( mDialogStack,getTextPtr(),mSection,this );

	//=====================================
	// start with monitor dialog
	//-------------------------------------
	mModuleList -> setSelected ( 0,true );
}

//=====================================
// slotSelected
//-------------------------------------
void SCCFrame::slotSelected ( Q3ListBoxItem* item ) {
	// .../
	// if the selection has changed this method is called.
	// the selection will be copied to be shown as module title
	// and the runDialog() method is called to start the dialog
	// ----
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());

	//=====================================
	// prepare module title text/pixmap
	//-------------------------------------
	QPixmap titlePixmap;
	QString selection = item -> text();
	QString titletext = selection;
	if (selection == mText["MonitorModule"]) {
		titletext   = mText["MonitorModuleTopic"];
		titlePixmap = QPixmap ( SMALL_MONITORS_PIXMAP );
	}
	if (selection == mText["MouseModule"]) {
		titletext   = mText["MouseModuleTopic"];
		titlePixmap = QPixmap ( SMALL_POINTERS_PIXMAP );
	}
	if (selection == mText["KeyboardModule"]) {
		titletext   = mText["KeyboardModuleTopic"];
		titlePixmap = QPixmap ( SMALL_KEYBOARD_PIXMAP );
	}
	if (selection == mText["TabletModule"]) {	
		titletext   = mText["TabletModuleTopic"];
		titlePixmap = QPixmap ( SMALL_XTABLETS_PIXMAP );
	}
	if (selection == mText["ToucherModule"]) {
		titletext   = mText["ToucherModuleTopic"];
		titlePixmap = QPixmap ( SMALL_TOUCHERS_PIXMAP );
	}
	if (selection == mText["VNCModule"]) {
		titletext   = mText["VNCModuleTopic"];
		titlePixmap = QPixmap ( SMALL_VNCSERVE_PIXMAP );
	}
	if (qApp->desktop()->height() > 600) {
		mModuleTitle -> setText ( titletext );
		mModuleTitleIcon -> setPixmap ( titlePixmap );
	}
	//=====================================
	// run the selected dialog
	//-------------------------------------
	runDialog ( item );
}

//=====================================
// slotRunDialog
//-------------------------------------
void SCCFrame::runDialog ( Q3ListBoxItem* item ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());

	//=====================================
	// check selected item and raise dialog
	//-------------------------------------
	QString selection = item -> text();
	if (selection == mText["MonitorModule"]) {
		mDialogStack -> raiseWidget ( mMonitor -> getDialogID() );
		if ( mMonitor -> needInit() ) {
			mMonitor -> setRequestedDialog ( mRequestedDialog );
			mMonitor -> init ();
		}
		if ( mMonitor -> needImport() ) {
			mMonitor -> import();
		}
	}
#if 0
	if (selection == mText["MouseModule"]) {
		mDialogStack -> raiseWidget ( mMouse -> getDialogID() );
		if ( mMouse -> needInit() ) {
			mMouse -> init();
		}
		if ( mMouse -> needImport() ) {
			mMouse -> import();
		}
	}
	if (selection == mText["KeyboardModule"]) {
		mDialogStack -> raiseWidget ( mKeyboard -> getDialogID() );
		if ( mKeyboard -> needInit() ) {
			mKeyboard -> init();
		}
		if ( mKeyboard -> needImport() ) {
			mKeyboard -> import();
		}
	}
#endif
	if (selection == mText["TabletModule"]) {
		mDialogStack -> raiseWidget ( mTablet -> getDialogID() );
		if ( mTablet -> needInit() ) {
			mTablet -> init();
		}
		if ( mTablet -> needImport() ) {
			mTablet -> import();
		}
	}
	if (selection == mText["ToucherModule"]) {
		mDialogStack -> raiseWidget ( mToucher -> getDialogID() );
		if ( mToucher -> needInit() ) {
			mToucher -> init();
		}
		if ( mToucher -> needImport() ) {
			mToucher -> import();
		}
	}
#if 0
	if (selection == mText["VNCModule"]) {
		mDialogStack -> raiseWidget ( mVNC -> getDialogID() );
		if ( mVNC -> needInit() ) {
			mVNC -> init();
		}
		if ( mVNC -> needImport() ) {
			mVNC -> import();
		}
	}
#endif
}

//=====================================
// slotCancel
//-------------------------------------
void SCCFrame::slotCancel ( void ) {
	//=====================================
	// reset to default keyboard
	//-------------------------------------
	mKeyboard -> applyDefault();
	//=====================================
	// show message if not in yast mode
	//-------------------------------------
	if ( ! mYaSTMode ) {
		SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::YES_NO, "ExitSaX"
		);
		QString result = mMessageBox -> showMessage();
		if (result == mText["Yes"]) {
			exitSaX (1);
		}
	} else {
		exitSaX (1);
	}
}

//=====================================
// closeEvent (WM close event)
//-------------------------------------
void SCCFrame::closeEvent ( QCloseEvent* ) {
	slotCancel();
}

//=====================================
// exitSaX
//-------------------------------------
void SCCFrame::exitSaX ( int status ) {
	exit ( status );
}

//=====================================
// hideframe
//-------------------------------------
void SCCFrame::hideFrame ( int frame ) {
	// .../
	// hide the given frame. Mostly used to hide controls
	// which are not needed if only a specific configuration
	// dialog should be opened
	// ----
	switch ( frame ) {
		case FRAME_MAIN:
			mMainFrame -> hide();
		break;
		case FRAME_MODULES:
			mModuleBox -> hide();
		break;
		default:
		break;
	}
}

//=====================================
// setMainFrame
//-------------------------------------
void SCCFrame::loadApplication ( void ) {
	// .../
	// load the configuration data refering to the contents of
	// mGUIMode this call will use libsax to read all data and
	// store the SaXImport pointers within the dictionary named
	// mSection.
	// ----
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
	//=====================================
	// check MD5 sum if available
	//-------------------------------------
	mConfig = new SaXConfig;
	if (! mConfig->isChecksumOK()) {
		log (L_INFO,"Checksum of installed configuration is invalid !\n");
	}
	//=====================================
	// create progress dialog
	//-------------------------------------
	Q3ProgressDialog mProgress (
		mText["ImportDataFiles"],mText["Cancel"],8,this,"progress",true
	);
	mProgress.setFixedWidth ( 350 );
	mProgress.setCaption (mText["ImportDataCaption"]);
	mProgress.setLabelText (mText["InitCache"]);
	int posx = mX - 175;
	int posy = mY - 75;
	mProgress.setGeometry ( posx,posy,350,150 );
	mProgress.setProgress ( 1 );
	qApp->processEvents();
	//=====================================
	// init cache if AUTO probed data used
	//-------------------------------------
	if ( mGUIMode == ISAX_AUTO ) {
		#if 0
		SaXInit init;
		if (init.needInit()) {
			log (L_INFO,"initialize cache...\n");
			init.doInit();
		}
		#endif
	}
	//=====================================
	// import all sections
	//-------------------------------------
	int importID[7] = {
		SAX_CARD,
		SAX_DESKTOP,
		SAX_POINTERS,
		SAX_KEYBOARD,
		SAX_LAYOUT,
		SAX_PATH,
		SAX_EXTENSIONS
	};
	for (int id=0; id<7; id++) {
		mProgress.setProgress ( id + 1 );
		qApp->processEvents();
		if ( mProgress.wasCanceled() ) {
			exitSaX (1);
		}
		SaXImport* import = new SaXImport ( importID[id] );
		import -> setSource ( SAX_SYSTEM_CONFIG );
		if (mGUIMode == ISAX_AUTO) {
			import -> setSource ( SAX_AUTO_PROBE );
		}
		import  -> doImport();
		mConfig -> addImport (import);
		mProgress.setLabelText (
			mText["ImportDataFiles"]+": "+mText[import->getSectionName()]
		);
		log (L_INFO,"Imported: %s\n",
			import->getSectionName().ascii()
		);
		mSection.insert (
			import->getSectionName(),import
		);
	}
	mProgress.setProgress ( 8 );
}

//=====================================
// slotFinish
//-------------------------------------
void SCCFrame::slotFinish ( void ) {
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
	QString messageText = "FinalSaX";
	if (mRequestedDialog) {
		messageText = "FinalSaXNoTest";
	}
	SCCMessage* mMessageBox = new SCCMessage (
		this, getTextPtr(), SaXMessage::FINISH,
		messageText,"MessageCaption",SaXMessage::Information,
		mRequestedDialog
	);
	QString result = mMessageBox -> showMessage();
	if (result == mText["FinalSave"]) {
		saveConfiguration();
	}
	if (result == mText["FinalTest"]) {
		testConfiguration();
	}
}

//=====================================
// prepareConfiguration
//-------------------------------------
bool SCCFrame::prepareConfiguration ( void ) {
	if ( ! mMonitor -> needInit() ) {
	if ( ! mMonitor -> exportData() ) {
		return false;
	}
	}
	if ( ! mMouse -> needInit() ) {
	if ( ! mMouse -> exportData() ) {
		return false;
	}
	}
	if ( ! mKeyboard -> needInit() ) {
	if ( ! mKeyboard -> exportData() ) {
		return false;
	}
	}
	if ( ! mTablet -> needInit() ) {
	if ( ! mTablet -> exportData() ) {
		return false;
	}
	}
	if ( ! mToucher -> needInit() ) {
	if ( ! mToucher -> exportData() ) {
		return false;
	}
	}
	if ( ! mVNC -> needInit() ) {
	if ( ! mVNC -> exportData() ) {
		return false;
	}
	}
	return true;
}

//=====================================
// saveConfiguration
//-------------------------------------
void SCCFrame::saveConfiguration ( void ) {
	if (! prepareConfiguration()) {
		return;
	}
	mConfig -> setMode (SAX_NEW);
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
	if ( ! mConfig -> createConfiguration() ) {
		QString message;
		QString library = mConfig->errorString();
		QString parser1 = mConfig->getParseError();
		QString parser2 = mConfig->getParseErrorValue();
		if (library.isEmpty()) {
			library = "No information";
		}
		if (parser1.isEmpty()) {
			parser1 = "No information";
		}
		QTextOStream (&message) <<
			mText["SavingFailed"] << "<br><br>" <<
			"SaX library: " << library << "<br>" <<
			"X configuration: " << parser1 << " " << parser2;
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::OK,
			message,"MessageCaption",SaXMessage::Critical
		);
		mMessageBox -> showMessage();
	} else {
		installConfiguration();
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::YES_NO,
			"SavingDone","MessageCaption"
		);
		QString result = mMessageBox -> showMessage();
		if (result == mText["Yes"]) {
			exitSaX();
		}
	}
}

//=====================================
// testConfiguration
//-------------------------------------
void SCCFrame::testConfiguration ( void ) {
	if (! prepareConfiguration()) {
		return;
	}
	mConfig -> setMode (SAX_NEW);
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());
	qApp->setOverrideCursor ( Qt::forbiddenCursor );
	mModuleList -> setDisabled ( true );
	mFinish -> setDisabled ( true );
	mCancel -> setDisabled ( true );
	SCCTestThread testConfiguration ( mConfig );
	testConfiguration.start();
	while (1) {
		usleep (1000);
		qApp->processEvents();
		if (! testConfiguration.running()) {
			break;
		}
	}
	qApp->restoreOverrideCursor();
	mModuleList -> setDisabled ( false );
	mFinish -> setDisabled ( false );
	mCancel -> setDisabled ( false );
	int status = testConfiguration.status();
	if ( status == -1 ) {
		QString message;
		QString library = mConfig->errorString();
		QString parser1 = mConfig->getParseError();
		QString parser2 = mConfig->getParseErrorValue();
		if (library.isEmpty()) {
			library = "No information";
		}
		if (parser1.isEmpty()) {
			parser1 = "No information";
		}
		QTextOStream (&message) <<
			mText["TestingFailed"] << "<br><br>" <<
			"SaX library: " << library << "<br>" <<
			"X configuration: " << parser1 << " " << parser2;
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::OK,
			message,"MessageCaption",SaXMessage::Critical
		);
		mMessageBox -> showMessage();
	}
	if ( status == 0 ) {
		installConfiguration();
		SCCMessage* mMessageBox = new SCCMessage (
			this, getTextPtr(), SaXMessage::YES_NO,
			"SavingDone","MessageCaption"
		);
		QString result = mMessageBox -> showMessage();
		if (result == mText["Yes"]) {
			exitSaX();
		}
	}
}

//=====================================
// installConfiguration
//-------------------------------------
void SCCFrame::installConfiguration ( void ) {
	mConfig -> commitConfiguration();
}

//=====================================
// evaluateAutoDetection
//-------------------------------------
void SCCFrame::evaluateAutoDetection ( void ) {
	// .../
	// check the imported data if there is something important
	// missing. Checked values may be DisplaySize and Monitor Name
	// ----
	// TODO
}

//=====================================
// slotHelp
//-------------------------------------
void SCCFrame::slotHelp ( void ) {
	// TODO
}

//=====================================
// startDialog
//-------------------------------------
bool SCCFrame::startDialog ( const QString& dialog ) {
	// .../
	// start the GUI with the given dialog on top and don't
	// allow switching to other dialogs
	// ----
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (getTextPtr());

	//=====================================
	// hide unneeded frames
	//-------------------------------------
	hideFrame ( FRAME_MODULES );
	hideFrame ( FRAME_TOPIC );

	//=====================================
	// raiseDialog
	//-------------------------------------
	Q3ListBoxItem* mItem = 0;
	if (dialog == "Monitor") {
		mItem = mModuleList -> findItem ( mText["MonitorModule"] );
	}
	if (dialog == "Mouse") {
		mItem = mModuleList -> findItem ( mText["MouseModule"] );
	}
	if (dialog == "Keyboard") {
		mItem = mModuleList -> findItem ( mText["KeyboardModule"] );
	}
	if (dialog == "Tablet") {
		mItem = mModuleList -> findItem ( mText["TabletModule"] );
	}
	if (dialog == "Touchscreen") {
		mItem = mModuleList -> findItem ( mText["ToucherModule"] );
	}
	if (dialog == "VNC") {
		mItem = mModuleList -> findItem ( mText["VNCModule"] );
	}
	if ( mItem ) {
		mModuleList -> setSelected ( mItem,true );
		return true;
	}
	return false;
}

//=====================================
// loadText
//-------------------------------------
void SCCFrame::loadText ( void ) {
	// .../
	// load the xapi.gtx file and translate the GTX/gettext
	// keys according to the currently used locale setting
	// ----
	SCCFile gtxHandle (GTX_FILE);
	setlocale (LC_ALL,"");
	bindtextdomain ("sax", TDOMAIN);
	textdomain ("sax");

	Q3Dict<QString> gtx = gtxHandle.readDict();
	Q3DictIterator<QString> it (gtx);
	for (; it.current(); ++it) {
		QString* translation = new QString(
			gettext((const char*)it.current()->latin1())
		);
		mTextPtr->insert (it.currentKey(),translation);
	}
}

//=====================================
// getText
//-------------------------------------
Q3Dict<QString>* SCCFrame::getTextPtr ( void ) {
	// .../
	// return a pointer to the translation dictionary
	// ----
	return mTextPtr;
}

//=====================================
// Set global event filter
//-------------------------------------
bool SCCFrame::eventFilter ( QObject* obj, QEvent* event ) {
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
} // end namespace
