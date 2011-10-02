/**************
FILE          : message.h
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
              : - message.h: SaXGUI::SCCMessage header definitions
              : - message.cpp: provide support for popup windows
              : ----
              :
STATUS        : Status: Development
**************/
#include "message.h"
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QCloseEvent>
#include <QShowEvent>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>
#include <QKeyEvent>
#include <QEvent>
#include <Q3VBoxLayout>

namespace SaXGUI {
//============================================
// XPM global definitions
//--------------------------------------------
static const char * const information_xpm[]={
"32 32 5 1", 
". c None",
"c c #000000",
"* c #999999",
"a c #ffffff",
"b c #0000ff",
"...........********.............",
"........***aaaaaaaa***..........",
"......**aaaaaaaaaaaaaa**........",
".....*aaaaaaaaaaaaaaaaaa*.......",
"....*aaaaaaaabbbbaaaaaaaac......",
"...*aaaaaaaabbbbbbaaaaaaaac.....",
"..*aaaaaaaaabbbbbbaaaaaaaaac....",
".*aaaaaaaaaaabbbbaaaaaaaaaaac...",
".*aaaaaaaaaaaaaaaaaaaaaaaaaac*..",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaaac*.",
"*aaaaaaaaaabbbbbbbaaaaaaaaaaac*.",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
"..*aaaaaaaaaabbbbbaaaaaaaaac***.",
"...caaaaaaabbbbbbbbbaaaaaac****.",
"....caaaaaaaaaaaaaaaaaaaac****..",
".....caaaaaaaaaaaaaaaaaac****...",
"......ccaaaaaaaaaaaaaacc****....",
".......*cccaaaaaaaaccc*****.....",
"........***cccaaaac*******......",
"..........****caaac*****........",
".............*caaac**...........",
"...............caac**...........",
"................cac**...........",
".................cc**...........",
"..................***...........",
"...................**..........."};
/* XPM */
static const char* const warning_xpm[]={
"32 32 4 1",
". c None",
"a c #ffff00",
"* c #000000",
"b c #999999",
".............***................",
"............*aaa*...............",
"...........*aaaaa*b.............",
"...........*aaaaa*bb............",
"..........*aaaaaaa*bb...........",
"..........*aaaaaaa*bb...........",
".........*aaaaaaaaa*bb..........",
".........*aaaaaaaaa*bb..........",
"........*aaaaaaaaaaa*bb.........",
"........*aaaa***aaaa*bb.........",
".......*aaaa*****aaaa*bb........",
".......*aaaa*****aaaa*bb........",
"......*aaaaa*****aaaaa*bb.......",
"......*aaaaa*****aaaaa*bb.......",
".....*aaaaaa*****aaaaaa*bb......",
".....*aaaaaa*****aaaaaa*bb......",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"...*aaaaaaaaa***aaaaaaaaa*bb....",
"...*aaaaaaaaaa*aaaaaaaaaa*bb....",
"..*aaaaaaaaaaa*aaaaaaaaaaa*bb...",
"..*aaaaaaaaaaaaaaaaaaaaaaa*bb...",
".*aaaaaaaaaaaa**aaaaaaaaaaa*bb..",
".*aaaaaaaaaaa****aaaaaaaaaa*bb..",
"*aaaaaaaaaaaa****aaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaa**aaaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
".*aaaaaaaaaaaaaaaaaaaaaaaaa*bbbb",
"..*************************bbbbb",
"....bbbbbbbbbbbbbbbbbbbbbbbbbbb.",
".....bbbbbbbbbbbbbbbbbbbbbbbbb.."};
/* XPM */
static const char* const critical_xpm[]={
"32 32 4 1",
". c None",
"a c #999999",
"* c #ff0000",
"b c #ffffff",
"...........********.............",
".........************...........",
".......****************.........",
"......******************........",
".....********************a......",
"....**********************a.....",
"...************************a....",
"..*******b**********b*******a...",
"..******bbb********bbb******a...",
".******bbbbb******bbbbb******a..",
".*******bbbbb****bbbbb*******a..",
"*********bbbbb**bbbbb*********a.",
"**********bbbbbbbbbb**********a.",
"***********bbbbbbbb***********aa",
"************bbbbbb************aa",
"************bbbbbb************aa",
"***********bbbbbbbb***********aa",
"**********bbbbbbbbbb**********aa",
"*********bbbbb**bbbbb*********aa",
".*******bbbbb****bbbbb*******aa.",
".******bbbbb******bbbbb******aa.",
"..******bbb********bbb******aaa.",
"..*******b**********b*******aa..",
"...************************aaa..",
"....**********************aaa...",
"....a********************aaa....",
".....a******************aaa.....",
"......a****************aaa......",
".......aa************aaaa.......",
".........aa********aaaaa........",
"...........aaaaaaaaaaa..........",
".............aaaaaaa............"};

//=========================================
// Constructor
//-----------------------------------------
SCCButton::SCCButton(
	const QString &text, QWidget*parent, const char*name
): QPushButton (text,parent,name) {
	QObject::connect (
		this , SIGNAL ( clicked     (void) ),
		this,  SLOT   ( slotClicked (void) )
	);
	mButtonText = text;
}

//=========================================
// slotClicked
//-----------------------------------------
void SCCButton::slotClicked (void) {
	clickedButton (this);
}

//=====================================
// Constructor
//-------------------------------------
SCCMessage::SCCMessage (
	QWidget* parent, Q3Dict<QString>* textPtr, SaXMessage::Type type,
	const QString& message, const QString& caption, SaXMessage::Icon icon,
	QString* requestedDialog, bool modal, const char* name
) : QDialog (
	parent,name,modal,
	Qt::WStyle_Customize | Qt::WStyle_DialogBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu
) {
	//=====================================
	// save translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (textPtr);
	if ( mText[message].isEmpty() ) {
		mMessage = message;
	} else {
		mMessage = mText[message];
	}
	mTextPtr = textPtr;
	mIcon    = icon;

	//=====================================
	// install event filter
	//-------------------------------------
	installEventFilter (this);

	//=====================================
	// set message box title
	//-------------------------------------
	if ( caption.isEmpty() ) {
		setCaption ( mText["MessageCaption"] );
	} else {
		setCaption ( mText[caption] );
	}

	//=====================================
	// create layout
	//-------------------------------------
	Q3GridLayout* baseLayout = new Q3GridLayout ( this,1,1 );
	mMainFrame = new Q3Frame ( this );
	baseLayout -> addWidget ( mMainFrame,0,0 );

	//=====================================
	// check type
	//-------------------------------------
	switch ( type ) {
		case SaXMessage::OK:
			createOkBox();
		break;
		case SaXMessage::OK_CANCEL:
			createOkCancelBox();
		break;
		case SaXMessage::YES_NO:
			createYesNoBox();
		break;
		case SaXMessage::OPT_ANY:
			createOptionAnyBox();
		break;
		case SaXMessage::OPT_INT:
			createOptionIntBox();
		break;
		case SaXMessage::OPT_STRING:
			createOptionStringBox();
		break;
		case SaXMessage::INTRO:
			createIntroBox();
		break;
		case SaXMessage::FINISH:
			createFinishBox (requestedDialog);
		break;
		default:
			log (L_ERROR,"No such message box");
			logExit(); exit (1);
		break;
	}

	//=====================================
	// set minimum size
	//-------------------------------------
	if (
		(type == SaXMessage::OPT_INT) ||
		(type == SaXMessage::OPT_ANY) ||
		(type == SaXMessage::OPT_STRING)
	) {
		setMinimumSize ( 400,100 );
	} else {
		setMinimumSize ( 300,100 );
	}
}

//=====================================
// addMainLayout
//-------------------------------------
void SCCMessage::addMainLayout ( void ) {
	mMainLayout  = new Q3HBoxLayout ( mMainFrame );
	mMainLayout -> setMargin  (10);
	mIconLayout  = new Q3VBoxLayout ( mMainLayout );
	mMainLayout -> addSpacing (10);
	mWorkLayout  = new Q3VBoxLayout ( mMainLayout );
	mTitleLayout = new Q3HBoxLayout ( mWorkLayout );
	mTitleLayout -> addSpacing (10);
	mWorkLayout  -> addSpacing (15);
	mWorkLayout  -> addStretch (10);
	mDoneLayout  = new Q3HBoxLayout ( mWorkLayout );
	mMainLayout -> setStretchFactor ( mWorkLayout, 20 );
}

//=====================================
// addSpecialLayout
//-------------------------------------
void SCCMessage::addSpecialLayout ( void ) {
	mMainLayout  = new Q3HBoxLayout ( mMainFrame );
	mMainLayout -> setMargin  (10);
	mIconLayout  = new Q3VBoxLayout ( mMainLayout );
	mMainLayout -> addSpacing (10);
	mWorkLayout  = new Q3VBoxLayout ( mMainLayout );
	mTitleLayout = new Q3VBoxLayout ( mWorkLayout );
	mTitleLayout -> addSpacing (10);
	mWorkLayout  -> addSpacing (15);
	mWorkLayout  -> addStretch (10);
	mDoneLayout  = new Q3HBoxLayout ( mWorkLayout );
	mMainLayout -> setStretchFactor ( mWorkLayout, 20 );
}

//=====================================
// setIcon
//-------------------------------------
void SCCMessage::setIcon ( SaXMessage::Icon icon ) {
	const char * const * xpmData;
	QPixmap* pixmap = new QPixmap();
	switch ( icon ) {
		case SaXMessage::Information:
			xpmData = information_xpm;
		break;
		case SaXMessage::Warning:
			xpmData = warning_xpm;
		break;
		case SaXMessage::Critical:
			xpmData = critical_xpm;
		break;
		default:
			xpmData = 0;
		break;
	}
	if ( xpmData ) {
		QImage image( (const char **) xpmData);
		pixmap->convertFromImage (image);
	}
	QLabel* boxIcon = new QLabel ( mMainFrame );
	boxIcon -> setPixmap ( *pixmap );
	mIconLayout -> addWidget ( boxIcon );
	mIconLayout -> addStretch ( 20 );	
}

//=====================================
// setText
//-------------------------------------
void SCCMessage::setText ( const QString& text ) {
	QLabel* boxText = new QLabel ( mMainFrame );
	boxText -> setText ( text );
	boxText -> setWordWrap( true );
	mTitleLayout -> addWidget ( boxText );
}

//=====================================
// createOkBox
//-------------------------------------
void SCCMessage::createOkBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addMainLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// create Ok button
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 8 );
	SCCButton* mButton = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButton );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButton , SIGNAL ( clickedButton (QPushButton*) ),
		this    , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();
}

//=====================================
// createYesNoBox
//-------------------------------------
void SCCMessage::createYesNoBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addMainLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// create Yes/No buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonYes = new SCCButton (mText["Yes"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonYes );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonNo = new SCCButton (mText["No"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonNo );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonYes, SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonNo , SIGNAL ( clickedButton (QPushButton*) ),
		this    , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();
}

//=====================================
// createOkCancelBox
//-------------------------------------
void SCCMessage::createOkCancelBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addMainLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// create Ok/Cancel buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonOk = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonOk );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCc = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCc );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonOk , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCc , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();
}

//=====================================
// createIntroBox
//-------------------------------------
void SCCMessage::createIntroBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addMainLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// create Yes/No buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonOk = new SCCButton (mText["Start"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonOk );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCc = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCc );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCh = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCh );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonOk , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCc , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCh , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();
}

//=====================================
// createFinishBox
//-------------------------------------
void SCCMessage::createFinishBox ( QString* requestedDialog ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addMainLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// create Test,Save,Cancel buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonTest = new SCCButton (mText["FinalTest"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonTest );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonSave = new SCCButton   (mText["FinalSave"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonSave );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCancel = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCancel );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonTest   , SIGNAL ( clickedButton (QPushButton*) ),
		this          , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonSave   , SIGNAL ( clickedButton (QPushButton*) ),
		this          , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCancel , SIGNAL ( clickedButton (QPushButton*) ),
		this          , SLOT   ( slotButton    (QPushButton*) )
	);

	//=====================================
	// hide test button in dialog mode 
	//-------------------------------------
	if (requestedDialog) {
		mButtonTest -> hide();
	}
	adjustSize();
}

//=====================================
// createOptionAnyBox
//-------------------------------------
void SCCMessage::createOptionStringBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addSpecialLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// Add ComboBox for value list
	//-------------------------------------
	mComboBox = new QComboBox  ( mMainFrame );
	mTitleLayout -> addSpacing ( 10 );
	mTitleLayout -> addWidget  ( mComboBox );

	//=====================================
	// create Ok/Cancel buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonOk = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonOk );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCc = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCc );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonOk , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCc , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();
}

//=====================================
// createOptionIntBox
//-------------------------------------
void SCCMessage::createOptionIntBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addSpecialLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// Add Spinbox for int values
	//-------------------------------------
	mSpinBox = new QSpinBox (0,100,1,mMainFrame);
	mTitleLayout -> addSpacing ( 10 );
	mTitleLayout -> addWidget  ( mSpinBox );

	//=====================================
	// create Ok/Cancel buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonOk = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonOk );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCc = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCc );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonOk , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCc , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();	
}

//=====================================
// createOptionStringBox
//-------------------------------------
void SCCMessage::createOptionAnyBox ( void ) {
	//=====================================
	// create layout add Icon and Text
	//-------------------------------------
	addSpecialLayout ();
	setIcon ( mIcon );
	setText ( mMessage );

	//=====================================
	// Add Spinbox for int values
	//-------------------------------------
	mLineEdit = new QLineEdit  ( mMainFrame );
	mTitleLayout -> addSpacing ( 10 );
	mTitleLayout -> addWidget  ( mLineEdit );

	//=====================================
	// create Ok/Cancel buttons
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mDoneLayout -> addStretch ( 5 );
	SCCButton* mButtonOk = new SCCButton (mText["Ok"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonOk );
	mDoneLayout -> addSpacing ( 10 );
	SCCButton* mButtonCc = new SCCButton (mText["Cancel"],mMainFrame);
	mDoneLayout -> addWidget  ( mButtonCc );
	mDoneLayout -> addStretch ( 10 );

	//=====================================
	// connect buttons
	//-------------------------------------
	QObject::connect (
		mButtonOk , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	QObject::connect (
		mButtonCc , SIGNAL ( clickedButton (QPushButton*) ),
		this      , SLOT   ( slotButton    (QPushButton*) )
	);
	adjustSize();	
}

//=====================================
// setSpinValue
//-------------------------------------
void SCCMessage::setSpinValue ( int value ) {
	mSpinBox->setValue ( value );
}

//=====================================
// setLineValue
//-------------------------------------
void SCCMessage::setLineValue ( const QString& text ) {
	mLineEdit -> setText ( text );
}

//=====================================
// setComboList
//-------------------------------------
void SCCMessage::setComboList ( const QStringList& list ) {
	mComboBox -> insertStringList ( list );
}

//=====================================
// setComboValue
//-------------------------------------
void SCCMessage::setComboValue ( const QString& text ) {
	mComboBox -> setCurrentText ( text );
}

//=====================================
// getSpinValue
//-------------------------------------
int SCCMessage::getSpinValue ( void ) {
	return mSpinBox->value();
}

//=====================================
// setLineValue
//-------------------------------------
QString SCCMessage::getLineValue ( void ) {
	return mLineEdit -> text();
}

//=====================================
// setComboValue
//-------------------------------------
QString SCCMessage::getComboValue ( void ) {
	return mComboBox -> currentText();
}

//=====================================
// showMessage
//-------------------------------------
QString SCCMessage::showMessage ( void ) {
	exec();
	return mClickedText;
}

//============================================
// global button click handler
//--------------------------------------------
void SCCMessage::slotButton ( QPushButton* btn ) {
	mClickedText = btn->text();
	hide();
}

//============================================
// Close event from WM
//--------------------------------------------
void SCCMessage::closeEvent ( QCloseEvent* ) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mClickedText = mText["Cancel"];
	hide();
}

//=====================================
// Set global event filter
//-------------------------------------
bool SCCMessage::eventFilter ( QObject* obj, QEvent* event ) {
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
//============================================
// Center window according to parent
//--------------------------------------------
void SCCMessage::showEvent ( QShowEvent* ) {
	int w  = width();
	int h  = height();
	if ( qApp->mainWidget() ) {
		int x0 = qApp->mainWidget()->x();
		int y0 = qApp->mainWidget()->y();
		move (
			x0 + ((qApp->mainWidget()->width()  - w) / 2),
			y0 + ((qApp->mainWidget()->height() - h) / 2)
		);
	}
}
} // end namespace
