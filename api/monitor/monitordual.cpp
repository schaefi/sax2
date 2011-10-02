/**************
FILE          : monitordual.cpp
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
              : - monitordual.h: SaXGUI::SCCMonitor header definitions
              : - monitordual.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitordual.h"
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>

//====================================
// Globals
//------------------------------------
extern SaXGUI::SCCWidgetProfile* SaXWidgetProfile;

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitorDual::SCCMonitorDual (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section, const QString& cardname,
	int display,QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mCardTab = new Q3VBox ( this );

	//=====================================
	// prepare tabed dialog
	//-------------------------------------
	mDualHeadDialog = new Q3TabDialog ( this,0,true );
	mDualHeadDialog -> setCaption  ( mText["DualHeadCaption"] );
	mDualHeadDialog -> setOkButton ( mText["Ok"] );
	mDualHeadDialog -> setCancelButton ( mText["Cancel"] );
	mDualHeadDialog -> addTab ( mCardTab, cardname );

	//=====================================
	// set import flag
	//-------------------------------------
	mNeedImport = true;
	mDisplay = display;

	//=====================================
	// create macro widgets
	//-------------------------------------
	mCardTab -> setMargin ( 15 );
	mChangeGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,"",mCardTab
	);
	mCardTab -> setSpacing ( 15 );
	mMoniBox = new Q3HBox ( mChangeGroup );
	mLabelSecondMonitor = new QLabel ( mText["SecondMonitor"],mMoniBox );
	mChangeMonitor = new QPushButton ( mText["Change"],mMoniBox);
	mMoniBox -> setStretchFactor ( mLabelSecondMonitor, 20 );
	mResBox = new Q3HBox ( mChangeGroup );
	mLabelSecondResolution = new QLabel (mText["SecondResolution"]+":",mResBox);
	mResolution = new QComboBox ( false, mResBox );
	mResBox -> setStretchFactor ( mLabelSecondResolution, 20 );
	mResolution -> setFixedWidth ( 200 );
	mChangeMonitor -> setFixedWidth ( 200 );
	Q3HBox* toolBox = new Q3HBox ( mCardTab );
	mCardTab -> setSpacing ( 15 );
	mModeGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["DualHeadMode"],toolBox
	);
	toolBox -> setSpacing ( 15 );
    mClone       = new QRadioButton ( mText["ModusClone"], mModeGroup );
    mXinerama    = new QRadioButton ( mText["ModusXinerama"], mModeGroup );
	mOrientationGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["Arrange"],toolBox
	);
	Q3ButtonGroup* alignGroup = new Q3ButtonGroup ( mOrientationGroup );
	#if QT_VERSION > 0x040100
	alignGroup -> setFrameStyle (
		Q3Frame::NoFrame
	);
	#endif
	alignGroup -> setFixedSize ( 200,200 );
	Q3GridLayout* orientationLayout = new Q3GridLayout ( alignGroup,2,2 );

	QPixmap leftOfPixmap  = QPixmap ( LEFT_OF_PIXMAP  );
	QPixmap rightOfPixmap = QPixmap ( RIGHT_OF_PIXMAP );
	QPixmap aboveOfPixmap = QPixmap ( ABOVE_OF_PIXMAP );
	QPixmap belowOfPixmap = QPixmap ( BELOW_OF_PIXMAP );

	mLeftOfPrimary  = new SCCButton ( "L",alignGroup );
	mRightOfPrimary = new SCCButton ( "R",alignGroup );
	mAboveOfPrimary = new SCCButton ( "A",alignGroup );
	mBelowOfPrimary = new SCCButton ( "B",alignGroup );

	mLeftOfPrimary  -> setToggleButton (true);
	mLeftOfPrimary  -> setPixmap ( leftOfPixmap );
	mRightOfPrimary -> setToggleButton (true);
	mRightOfPrimary -> setPixmap ( rightOfPixmap );
	mAboveOfPrimary -> setToggleButton (true);
	mAboveOfPrimary -> setPixmap ( aboveOfPixmap );
	mBelowOfPrimary -> setToggleButton (true);
	mBelowOfPrimary -> setPixmap ( belowOfPixmap );

	orientationLayout -> addWidget ( mLeftOfPrimary ,0,0,Qt::AlignCenter );
	orientationLayout -> addWidget ( mRightOfPrimary,1,0,Qt::AlignCenter );
	orientationLayout -> addWidget ( mAboveOfPrimary,0,1,Qt::AlignCenter );
	orientationLayout -> addWidget ( mBelowOfPrimary,1,1,Qt::AlignCenter );

	mCardTab -> setStretchFactor ( toolBox,20 );

	//=====================================
	// create toplevel dialogs
	//-------------------------------------
	QString cardTitle (mText["SecondMonitorForCard"] + " " + cardname);
	mChangeMonitorDualModelDialog = new SCCMonitorDualModel (
		text,section,cardTitle,mText["Sync"],
		display,this
	);

	//=====================================
	// initialize toplevel dialogs
	//-------------------------------------
	mChangeMonitorDualModelDialog -> init();
	mChangeMonitorDualModelDialog -> import();

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mDualHeadDialog , SIGNAL (applyButtonPressed ( void )),
		this            , SLOT   (slotOk  ( void ))
    );
	QObject::connect (
		mClone          , SIGNAL (toggled   ( bool )),
		this            , SLOT   (slotClone ( bool ))
	);
	QObject::connect (
		mLeftOfPrimary  , SIGNAL (clickedButton ( QPushButton* )),
		this            , SLOT   (slotDualHead  ( QPushButton* ))
	);
	QObject::connect (
		mRightOfPrimary , SIGNAL (clickedButton ( QPushButton* )),
		this            , SLOT   (slotDualHead  ( QPushButton* ))
	);
	QObject::connect (
		mAboveOfPrimary , SIGNAL (clickedButton ( QPushButton* )),
		this            , SLOT   (slotDualHead  ( QPushButton* ))
	);
	QObject::connect (
		mBelowOfPrimary , SIGNAL (clickedButton ( QPushButton* )),
		this            , SLOT   (slotDualHead  ( QPushButton* ))
	);
	QObject::connect (
		mChangeMonitor  , SIGNAL (clicked       ( void )),
		this            , SLOT   (slotMonitor   ( void ))
	);
}
//=====================================
// show tabed dialog
//-------------------------------------
void SCCMonitorDual::show (void) {
	mDualHeadDialog->setGeometry  ( 0,0,640,480 );
	mDualHeadDialog->move (
		getPosition (mDualHeadDialog->width(),mDualHeadDialog->height())
	);
	import();
	mDualHeadDialog->show();
	mDualHeadDialog->raise();
}
//====================================
// init
//------------------------------------
void SCCMonitorDual::init ( void ) {
	//====================================
	// create manipulators...
	//------------------------------------
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// select desktop
	//------------------------------------
	saxDesktop.selectDesktop ( mDisplay );

	//====================================
	// insert available resolutions
	//------------------------------------
	SCCFile resHandle ( RES_FILE );
	mResolutionDict = resHandle.readDict();
	Q3Dict<QString> metaResolution;
	long sortResolution[mResolutionDict.count()];
	Q3DictIterator<QString> ir (mResolutionDict);
	int count = 0;
	for (; ir.current(); ++ir) {
		QStringList tokens = QStringList::split ( "x", ir.currentKey() );
		int x = tokens.first().toInt();
		int y = tokens.last().toInt();
		long pixels = x * y;
		QString* key = new QString;
		QTextOStream (key) << pixels;
		metaResolution.insert (*key,ir.current());
		sortResolution[count] = pixels;
		count++;
	}
	for (int n=0;n<count;n++) {
	for (int i=n+1;i<count;i++) {
		if (sortResolution[n] > sortResolution[i]) {
			long save = sortResolution[n];
			sortResolution[n] = sortResolution[i];
			sortResolution[i] = save;
		}
	}
	}
	for (int n=0;n<count;n++) {
		QString key;
		QTextOStream (&key) << sortResolution[n];
		mResolution -> insertItem ( *metaResolution[key] );
	}
	//====================================
	// setup default layout and mode
	//------------------------------------
	mLeftOfPrimary -> setOn    ( true );
	mClone -> setChecked ( true );

	//====================================
	// import dualhead profile if set
	//------------------------------------
	QString profile = saxDesktop.getDualHeadProfile();
	if ((! profile.isEmpty()) && (! saxDesktop.isXineramaMode())) {
		SaXImportProfile* pProfile = SaXWidgetProfile->getProfile ( profile );
		SaXImport* mImport = pProfile -> getImport ( SAX_CARD );
		if ( mImport ) {
			SaXManipulateCard saxProfileCard ( mImport );
			mProfileDriverOptions = saxProfileCard.getOptions();
			//====================================
			// check avaliable config groups
			//------------------------------------	
			Q3DictIterator<QString> it ( mProfileDriverOptions );
			bool hasHSyncSetup = true;
			bool hasVSyncSetup = true;
			bool hasResSetup   = true;
			for (;it.current();++it) {
				QString key = it.currentKey();
				QString val = *it.current();
				if ((key == "SaXDualOrientation") && (val == "off")) {
					mOrientationGroup -> setDisabled ( true );
				}
				if ((key == "SaXDualMode") && (val == "off")) {
					mModeGroup -> setDisabled ( true );
				}
				if ((key == "SaXDualResolution") && (val == "off")) {
					hasResSetup = false;
					mResBox -> setDisabled ( true );
				}
				if ((key == "SaXDualHSync") && (val == "off")) {
					hasHSyncSetup = false;
				}
				if ((key == "SaXDualVSync") && (val == "off")) {
					hasVSyncSetup = false;
				}
			}
			if ((! hasHSyncSetup) && (! hasVSyncSetup)) {
				mMoniBox -> setDisabled ( true );
			}
			if ((! hasHSyncSetup) && (! hasVSyncSetup) && (! hasResSetup)) {
				mChangeGroup -> setDisabled ( true );
			}
		}
	}
}
//====================================
// import
//------------------------------------
void SCCMonitorDual::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	if ( mNeedImport ) {
		//====================================
		// create needed manipulators
		//------------------------------------
		SaXManipulateCard saxCard (
			mSection["Card"]
		);
		//====================================
		// select card
		//------------------------------------
		saxCard.selectCard ( mDisplay );

		//====================================
		// get options defined for the card
		//------------------------------------
		Q3Dict<QString> mOptions = saxCard.getOptions();

		//====================================
		// handle monitor vendor/model name
		//------------------------------------
		if ((mOptions["SaXDualMonitorVendor"]) && 
			(mOptions["SaXDualMonitorModel"])
		) {
			// imported config data...
			mMonitorVendor = *mOptions["SaXDualMonitorVendor"];
			mMonitorModel  = *mOptions["SaXDualMonitorModel"];
		} else
		if ((mProfileDriverOptions["SaXDualMonitorVendor"]) &&
			(mProfileDriverOptions["SaXDualMonitorModel"])
		) {
			// profile default data...
			mMonitorVendor = *mProfileDriverOptions["SaXDualMonitorVendor"];
			mMonitorModel  = *mProfileDriverOptions["SaXDualMonitorModel"];
		} else {
			// default data...
			mMonitorVendor = mText["UnknownMonitorPleaseChange"];
			mMonitorModel  = mText["UnknownMonitorPleaseChange"];
		}
		//====================================
		// handle resolution
		//------------------------------------
		if (mOptions["SaXDualResolution"]) {
			// imported config data...
			mSelectedResolution = *mOptions["SaXDualResolution"];
		} else
		if (mProfileDriverOptions["SaXDualResolution"]) {
			// profile default data...
			mSelectedResolution = *mProfileDriverOptions["SaXDualResolution"];
		} else {
			// default data...
			mSelectedResolution = "640x480";
		}
		//====================================
		// handle dualhead mode
		//------------------------------------
		if (mOptions["SaXDualMode"]) {
			// imported config data...
			QString mode = *mOptions["SaXDualMode"];
			if (mode == QString(DUAL_CLONE_KEY)) {
				mDualHeadMode = DUAL_CLONE;
			}
			if (mode == QString(DUAL_XINERAMA_KEY)) {
				mDualHeadMode = DUAL_XINERAMA;
			}
		} else
		if (mProfileDriverOptions["SaXDualMode"]) {
			// profile default data...
			QString mode = *mProfileDriverOptions["SaXDualMode"];
			if (mode == QString(DUAL_CLONE_KEY)) { 
				mDualHeadMode = DUAL_CLONE;
			}
			if (mode == QString(DUAL_XINERAMA_KEY)) { 
				mDualHeadMode = DUAL_XINERAMA;
			}
		} else {
			// default data...
			mDualHeadMode = DUAL_TRADITIONAL;
		}
		//====================================
		// handle dualhead arrangement
		//------------------------------------
		if (mOptions["SaXDualOrientation"]) {
			// imported config data..
			QString layout = *mOptions["SaXDualOrientation"];
			if (layout == QString (DUAL_ABOVEOF_KEY)) {
				mDualHeadArrangement = DUAL_ABOVEOF;
			}
			if (layout == QString (DUAL_RIGHTOF_KEY)) {
				mDualHeadArrangement = DUAL_RIGHTOF;
			}
			if (layout == QString (DUAL_BELOWOF_KEY)) {
				mDualHeadArrangement = DUAL_BELOWOF;
			}
		} else
		if (mProfileDriverOptions["SaXDualOrientation"]) {
			// profile default data...
			QString layout = *mProfileDriverOptions["SaXDualOrientation"];
			if (layout == QString (DUAL_ABOVEOF_KEY)) {
				mDualHeadArrangement = DUAL_ABOVEOF;
			}
			if (layout == QString (DUAL_RIGHTOF_KEY)) {
				mDualHeadArrangement = DUAL_RIGHTOF;
			}
			if (layout == QString (DUAL_BELOWOF_KEY)) {
				mDualHeadArrangement = DUAL_BELOWOF;
			}
		} else {
			// default data...
			mDualHeadArrangement = DUAL_LEFTOF;
		}
		//====================================
		// import done set flag
		//------------------------------------
		mNeedImport = false;
	}
	//====================================
	// setup monitor vendor/name label
	//------------------------------------
	if ((mMonitorVendor == mText["UnknownMonitorPleaseChange"]) ||
		(mMonitorModel  == mText["UnknownMonitorPleaseChange"])
	) {
		mLabelSecondMonitor -> setText (
			mText["SecondMonitor"] + " " + mText["UnknownMonitorPleaseChange"]
		);
	} else {
		mLabelSecondMonitor -> setText (
			mText["SecondMonitor"] + " " + mMonitorVendor+" "+mMonitorModel
		);
	}
	//====================================
	// setup resolution ComboBox
	//------------------------------------
	if (! mResolutionDict[mSelectedResolution]) {
		mSelectedResolution = "640x480";
	}
	mResolution -> setCurrentText (
		*mResolutionDict[mSelectedResolution]
	);
	//====================================
	// setup dualhead mode RadioButton
	//------------------------------------
	switch (mDualHeadMode) {
		case DUAL_CLONE:
			mClone -> setChecked ( true );
		break;
		case DUAL_XINERAMA:
			mXinerama -> setChecked ( true );
		break;
		default:
			mClone -> setChecked ( true );
		break;
	};
	//====================================
	// setup dualhead arrangement toggles
	//------------------------------------
	slotDualHead ( 0 );
	switch (mDualHeadArrangement) {
		case DUAL_LEFTOF:
			mLeftOfPrimary -> setOn ( true );
		break;
		case DUAL_ABOVEOF:
			mAboveOfPrimary -> setOn ( true );
		break;
		case DUAL_RIGHTOF:
			mRightOfPrimary -> setOn ( true );
		break;
		case DUAL_BELOWOF:
			mBelowOfPrimary -> setOn ( true );
		break;
		default:
			mLeftOfPrimary -> setOn ( true );
		break;
	};
}
//====================================
// getMode
//------------------------------------
int SCCMonitorDual::getMode ( void ) {
	return mDualHeadMode;
}
//====================================
// getResolution
//------------------------------------
QString SCCMonitorDual::getResolution ( void ) {
	return mSelectedResolution;
}
//====================================
// getResolutionList
//------------------------------------
QList<QString> SCCMonitorDual::getResolutionList ( void ) {
	return setupList ();
}
//====================================
// getLayout
//------------------------------------
int SCCMonitorDual::getLayout ( void ) {
	return mDualHeadArrangement;
}
//====================================
// getLayoutPixmap
//------------------------------------
QPixmap SCCMonitorDual::getLayoutPixmap ( void ) {
	if (mLeftOfPrimary -> isOn() && mLeftOfPrimary->pixmap()) {
		return *mLeftOfPrimary->pixmap();
	}
	if (mRightOfPrimary -> isOn() && mRightOfPrimary->pixmap() ) {
		return *mRightOfPrimary->pixmap();
	}
	if (mAboveOfPrimary -> isOn() && mAboveOfPrimary->pixmap() ) {
		return *mAboveOfPrimary->pixmap();
	}
	if (mBelowOfPrimary -> isOn() && mBelowOfPrimary->pixmap() ) {
		return *mBelowOfPrimary->pixmap();
	}
	return QPixmap ( FIGURE_SINGLE );
}
//====================================
// getDualModelData
//------------------------------------
SCCMonitorDualModel* SCCMonitorDual::getDualModelData ( void ) {
	return mChangeMonitorDualModelDialog;
}
//=====================================
// setTitle
//-------------------------------------
void SCCMonitorDual::setTitle ( const QString & title ) {
	mDualHeadDialog -> changeTab ( mCardTab,title );
}
//====================================
// setMonitorName
//------------------------------------
void SCCMonitorDual::setMonitorName (
	const QString& vendor, const QString& model
) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// update monitor label
	//-------------------------------------
	QString monitorName;
	QTextOStream (&monitorName) <<
		mText["SecondMonitor"] << " " << vendor << " " << model;
	mLabelSecondMonitor -> clear();
	mLabelSecondMonitor -> setText ( monitorName );
}
//====================================
// setResolution
//------------------------------------
void SCCMonitorDual::setResolution ( const QString & resolution ) {
	if (! mResolutionDict[resolution]) {
    	return;
	}
	mResolution -> setCurrentText (
		*mResolutionDict[resolution]
	);
}
//=====================================
// slotClone
//-------------------------------------
void SCCMonitorDual::slotClone ( bool on ) {
	mOrientationGroup -> setDisabled ( on );
}
//=====================================
// slotOk
//-------------------------------------
void SCCMonitorDual::slotOk ( void ) {
	//=====================================
	// call Ok handler for DualModel
	//-------------------------------------
	mChangeMonitorDualModelDialog -> slotVerifiedOk ();

	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// get parent SCCMonitorDisplay ptr
	//-------------------------------------
	SCCMonitorDisplay* pDisplay = (SCCMonitorDisplay*)mParent;

	//=====================================
	// save resolution state
	//-------------------------------------
	QString selected = mResolution->currentText();
	Q3DictIterator<QString> it (mResolutionDict);
	for (; it.current(); ++it) {
		if (*it.current() == selected) {
			mSelectedResolution = it.currentKey();
			break;
		}
	}
	//=====================================
	// save state of dualhead modus
	//-------------------------------------
	if (mClone -> isChecked()) {
		mDualHeadMode = DUAL_CLONE;
	}
	if (mXinerama -> isChecked()) {
		mDualHeadMode = DUAL_XINERAMA;
	}
	//=====================================
	// save state of dualhead arrangement
	//-------------------------------------
	if (mLeftOfPrimary -> isOn()) {
		mDualHeadArrangement = DUAL_LEFTOF;
	}
	if (mRightOfPrimary -> isOn()) {
		mDualHeadArrangement = DUAL_RIGHTOF;
	}
	if (mAboveOfPrimary -> isOn()) {
		mDualHeadArrangement = DUAL_ABOVEOF;
	}
	if (mBelowOfPrimary -> isOn()) {
		mDualHeadArrangement = DUAL_BELOWOF;
	}
	//=====================================
	// save state of second monitor
	//-------------------------------------
	mMonitorVendor = mChangeMonitorDualModelDialog->getVendorName();
	mMonitorModel  = mChangeMonitorDualModelDialog->getModelName();

	//=====================================
	// update parents dual info label
	//-------------------------------------
	QString translation = mText["ActivateInfo"];
	QString dualResolution = mResolution -> currentText();
	QString dualMode = mText["Traditional"];
	if ( mClone -> isChecked() ) {
		dualMode = mText["Clone"];
	}
	if ( mXinerama -> isChecked() ) {
		dualMode = mText["Xinerama"];
	}
	translation.replace ( QRegExp("\%1"),dualMode );
	translation.replace ( QRegExp("\%2"),dualResolution );
	pDisplay -> setDualModeInfo ( translation );
	pDisplay -> setCombinedDisplaySize (false);

	//=====================================
	// update layout pixmap
	//-------------------------------------
	emit sigDualModeChanged ( pDisplay );
}
//=====================================
// slotMonitor
//-------------------------------------
void SCCMonitorDual::slotMonitor ( void ) {
	mChangeMonitorDualModelDialog -> show();
}
//=====================================
// slotDualHead
//-------------------------------------
void SCCMonitorDual::slotDualHead ( QPushButton* btn ) {
	if ( btn != mLeftOfPrimary ) {
		mLeftOfPrimary -> setOn ( false );
	}
	if ( btn != mRightOfPrimary ) {
		mRightOfPrimary -> setOn ( false );
	}
	if ( btn != mAboveOfPrimary ) {
		mAboveOfPrimary -> setOn ( false );
	}
	if ( btn != mBelowOfPrimary ) {
		mBelowOfPrimary -> setOn ( false );
	}
}
//=====================================
// setupList
//-------------------------------------
QList<QString> SCCMonitorDual::setupList ( void ) {
	//====================================
	// find first startup resolution
	//------------------------------------
	QString selected = mResolution->currentText();
	Q3DictIterator<QString> it (mResolutionDict);
	long basePixels  = 0;
	int  basePixelsX = 0;
	int  basePixelsY = 0;
	mSelectedResolutionList.clear();
	for (; it.current(); ++it) {
		if (*it.current() == selected) {
			QString* baseResolution = new QString (it.currentKey());
			QStringList tokens = QStringList::split ( "x", *baseResolution );
			int x = tokens.first().toInt();
			int y = tokens.last().toInt();
			basePixels  = x * y;
			basePixelsX = x;
			basePixelsY = y;
			mSelectedResolutionList.append ( *baseResolution );
		}
	}
	//====================================
	// append lower resolutions
	//------------------------------------
	it.toFirst();
	int count = 0;
	Q3Dict<QString> metaResolution;
	long sortResolution[mResolutionDict.count()];
	for (; it.current(); ++it) {
		QString* addResolution = new QString (it.currentKey());
		QStringList tokens = QStringList::split ( "x", *addResolution );
		int x = tokens.first().toInt();
		int y = tokens.last().toInt();
		int pixelSpace = x * y;
		if ((pixelSpace < basePixels) && (x<=basePixelsX) && (y<=basePixelsY)) {
			QString key;
			QTextOStream (&key) << pixelSpace;
			metaResolution.insert ( key,addResolution );
			sortResolution[count] = pixelSpace;
			count++;
		}
	}
	for (int n=0;n<count;n++) {
	for (int i=n+1;i<count;i++) {
		if (sortResolution[n] < sortResolution[i]) {
			long save = sortResolution[n];
			sortResolution[n] = sortResolution[i];
			sortResolution[i] = save;
		}
	}
	}
	for (int n=0;n<count;n++) {
		QString key;
		QTextOStream (&key) << sortResolution[n];
		mSelectedResolutionList.append ( *metaResolution[key] );
	}
	return mSelectedResolutionList;
} 
} // end namespace
