/**************
FILE          : monitordisplay.cpp
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
              : - monitordisplay.h: SaXGUI::SCCMonitor header definitions
              : - monitordisplay.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitordisplay.h"
//Added by qt3to4:
#include <QLabel>
#include <QPixmap>
#include <Q3VBoxLayout>

//====================================
// Globals
//------------------------------------
extern SaXGUI::SCCWidgetProfile* SaXWidgetProfile;

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitorDisplay::SCCMonitorDisplay (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	int display, QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mMainLayout = new Q3VBoxLayout ( this );

	//=====================================
	// create macro widgets
	//-------------------------------------
	// toplevel activate check box
	mCheckEnable = new QCheckBox ( mText["ActivateDisplay"],this );
	// first group with card and monitor name
	mCardMonitorGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,"",this
	);
	Q3HBox* CMBox = new Q3HBox ( mCardMonitorGroup );
	Q3VBox* leftCMBox  = new Q3VBox ( CMBox );
	leftCMBox -> setSpacing ( 10 );
	Q3VBox* rightCMBox = new Q3VBox ( CMBox );
	rightCMBox -> setSpacing ( 10 );
	mLabelCardText    = new QLabel ( mText["Card"]+": ",leftCMBox );
	mLabelMonitorText = new QLabel ( mText["Monitor"]+": ",leftCMBox );
	Q3HBox* cardBox = new Q3HBox ( rightCMBox );
	Q3HBox* moniBox = new Q3HBox ( rightCMBox );
	mLabelCardName = new QLabel ( cardBox );
	mCardOptions   = new QPushButton ( mText["Options"],cardBox);
	cardBox -> setStretchFactor ( mLabelCardName, 20 );
	mLabelMonitorName = new QLabel ( moniBox );
	mChangeMonitor    = new QPushButton ( mText["Change"],moniBox);
	moniBox -> setStretchFactor ( mLabelMonitorName, 20 );
	// second group with properties
	mPropertyGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["Properties"],this
	);
	Q3VBox* resBox = new Q3VBox  ( mPropertyGroup );
	QLabel* space = new QLabel ( mPropertyGroup );
	space -> setFixedWidth ( 10 );
	Q3VBox* colBox = new Q3VBox  ( mPropertyGroup );
	mLabelResolution = new QLabel ( mText["Resolution"],resBox );
	resBox -> setSpacing ( 10 );
	mResolution  = new QComboBox ( false, resBox );
	mLabelColors = new QLabel ( mText["Colors"],colBox );
	colBox -> setSpacing ( 10 );
	mColors      = new QComboBox ( false, colBox );
	// third group with dual head mode
	mDualHeadGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["DualHeadMode"],this
	);
	Q3HBox* dualBox = new Q3HBox ( mDualHeadGroup );
	Q3HBox* infoBox = new Q3HBox ( mDualHeadGroup );
	mCheckDualHead = new QCheckBox ( mText["ActivateDualHead"],dualBox );
	mConfigureDualHead = new QPushButton ( mText["Configure"],dualBox );
	dualBox -> setStretchFactor ( mCheckDualHead, 20 );
	QLabel* infoSpace  = new QLabel ( infoBox );
	infoSpace -> setFixedWidth ( 21 );
	mDualHeadInfoLabel = new QLabel ( infoBox );
	
	//=====================================
	// create toplevel dialogs
	//-------------------------------------
	SaXManipulateCard saxCard ( mSection["Card"],display );
	QString cardTitle (saxCard.getCardVendor() + " " + saxCard.getCardModel());
	mCardOptionDialog = new SCCMonitorCard (
		text,section,cardTitle,
		display,this
	);
	QString monitorTitle ( mText["MonitorForCard"]+" "+cardTitle );
	mChangeMonitorDialog = new SCCMonitorModel (
		text,section,monitorTitle,mText["Sync"],mText["DisplaySize"],
		display,this
	);
	QString dualMonitorTitle ( cardTitle );
	mConfigureDualHeadDialog = new SCCMonitorDual (
		text,section,dualMonitorTitle,
		display,this
	);
	//=====================================
	// initialize toplevel dialogs
	//-------------------------------------
	mCardOptionDialog -> init();
	mCardOptionDialog -> import();
	mChangeMonitorDialog -> init();
	mChangeMonitorDialog -> import();
	mConfigureDualHeadDialog -> init();
	mConfigureDualHeadDialog -> import();

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mConfigureDualHead , SIGNAL ( clicked               ( void )),
		this               , SLOT   ( slotConfigureDualHead ( void ))
	);
	QObject::connect (
		mChangeMonitor     , SIGNAL ( clicked               ( void )),
		this               , SLOT   ( slotChangeMonitor     ( void ))
	);
	QObject::connect (
		mCardOptions       , SIGNAL ( clicked               ( void )),
		this               , SLOT   ( slotChangeCardOptions ( void ))
	);
	QObject::connect (
		mCheckEnable       , SIGNAL ( clicked               ( void )),
		this               , SLOT   ( slotActivateDisplay   ( void ))
	);
	QObject::connect (
		mCheckDualHead     , SIGNAL ( clicked               ( void )),
		this               , SLOT   ( slotActivateDualHead  ( void ))
	);
	QObject::connect (
		mResolution        , SIGNAL ( activated             ( int )),
		this               , SLOT   ( slotResolution        ( int ))
	);
	QObject::connect (
		mColors            , SIGNAL ( activated             ( int )),
		this               , SLOT   ( slotColors            ( int ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mCheckEnable );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mCardMonitorGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mPropertyGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mDualHeadGroup );
	mMainLayout -> addStretch ( 10 );

	//=====================================
	// Show display size message y/n
	//-------------------------------------
	mShowDisplaySizeMessage = true;

	//=====================================
	// save display number
	//-------------------------------------
	mDisplay = display;
}
//====================================
// init
//------------------------------------
void SCCMonitorDisplay::init ( void ) {
	//====================================
	// create card a. desktop manipulators
	//------------------------------------
	mSaxCard = new SaXManipulateCard (
		mSection["Card"]
	);
	mSaxDesktop = new SaXManipulateDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// select card and desktop
	//------------------------------------
	mSaxDesktop -> selectDesktop ( mDisplay );
	mSaxCard -> selectCard ( mDisplay );

	//====================================
	// get virtual card count
	//------------------------------------
	int usedDevices = mSaxCard->getDevices();

	//====================================
	// check activate box
	//------------------------------------
	mCheckEnable -> setChecked ( true );
	if (usedDevices == 1) {
		mCheckEnable -> hide();
		mEnabled = true;
	} else {
		mCheckEnable -> setChecked ( true );
		mEnabled = true;
	}
	//====================================
	// check multihead group
	//------------------------------------
	if (! mSaxCard->isNoteBook()) {
		if (! mSaxDesktop->isDualHeadCard()) {
			mDualHeadGroup -> hide();
		}
	} else {
		if (mSaxDesktop->getDualHeadProfile().isEmpty()) {
			mDualHeadGroup -> setDisabled ( true );
		}
	}
	if (mSaxDesktop->isXineramaMode()) {
		mDualHeadGroup -> hide();
	}
	//====================================
	// insert available resolutions
	//------------------------------------
	SCCFile resHandle ( RES_FILE );
	mResolutionDict = resHandle.readDict();
	//====================================
	// Create reference list for DDC or FB
	//------------------------------------
	QList<QString> reference;
	if ((mSaxCard->getCardDriver() == "fbdev") ||
		(mSaxCard->getCardDriver() == "vesa")
	) {
		log (L_INFO,"SCCMonitorDisplay::using FBDEV reference resolutions\n");
		reference = mSaxDesktop->getResolutionsFromFrameBuffer();
		mIsFbdevBased = true;
	} else {
		// disabled due to bug: #358346
		//log (L_INFO,"SCCMonitorDisplay::using DDC reference resolutions\n");
		//reference = mSaxDesktop->getResolutionsFromDDC1();
		mIsFbdevBased = false;
	}
	//====================================
	// check reference list
	//------------------------------------
	QList<QString> toBeRemoved;
	if (! reference.isEmpty()) {
		mResolutionDict.setAutoDelete(true);
		Q3DictIterator<QString> ir (mResolutionDict);
		for (; ir.current(); ++ir) {
			bool foundInReference = false;
			QString dd;
			foreach (dd,reference) {
				if (dd == ir.currentKey()) {
					foundInReference = true;
					break;
				}
			}
			if (foundInReference) {
				log (L_INFO,"SCCMonitorDisplay::resolution %s:\tYES\n",
					ir.currentKey().ascii()
				);
				continue;
			}
			log (L_INFO,"SCCMonitorDisplay::resolution %s:\tNO\n",
				ir.currentKey().ascii()
			);
			toBeRemoved.append (QString(ir.currentKey()));
		}
	}
	QString il;
	foreach (il,toBeRemoved) {
		mResolutionDict.remove(il);
	}
	//====================================
	// insert remaining resolutions
	//------------------------------------
	Q3Dict<QString> metaResolution;
	long sortResolution[mResolutionDict.count()];
	Q3DictIterator<QString> ir (mResolutionDict);
	int count = 0;
	for (; ir.current(); ++ir) {
		QStringList tokens = QStringList::split ( "x", ir.currentKey() );
		int x = tokens.first().toInt();
		int y = tokens.last().toInt();
		long pixels = x * y;
		QString* key = new QString();
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
		QString* key = new QString();
		QTextOStream (key) << sortResolution[n];
		mResolution -> insertItem ( *metaResolution[*key] );
	}
	//====================================
	// insert available colors
	//------------------------------------
	SCCFile colHandle ( COL_FILE );
	mColorDict = colHandle.readDict();
	Q3DictIterator<QString> ic (mColorDict);
	int index  = 0;
	int maxcol = 0;
	int colKey[mColorDict.count()];
	for (; ic.current(); ++ic) {
		colKey[index] = ic.currentKey().toInt();
		if (maxcol > colKey[index]) {
			maxcol = colKey[index];
		}
		index++;
	}
	for (unsigned int n=0;n<mColorDict.count();n++) {
	for (unsigned int x=n;x<mColorDict.count();x++) {
		if (colKey[n] < colKey[x]) {
			int save  = colKey[n];
			colKey[n] = colKey[x];
			colKey[x] = save;
		}
	}
	}
	for (unsigned int n=0;n<mColorDict.count();n++) {
		QString key; QTextOStream (&key) << colKey[n];
		mColors -> insertItem ( *mColorDict[key] );
	}
}
//====================================
// import
//------------------------------------
void SCCMonitorDisplay::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create needed manipulators
	//------------------------------------
	SaXManipulateCard saxCard (
		mSection["Card"]
	);
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// select card and desktop
	//------------------------------------
	saxDesktop.selectDesktop ( mDisplay );
	saxCard.selectCard ( mDisplay );

	//====================================
	// get options defined for the card
	//------------------------------------
	Q3Dict<QString> mOptions = saxCard.getOptions();

	//====================================
	// handle card name
	//------------------------------------
	mCardModel  = saxCard.getCardModel();
	mCardVendor = saxCard.getCardVendor();

	//====================================
	// handle monitor name
	//------------------------------------
	mMonitorModel  = saxDesktop.getMonitorName();
	mMonitorVendor = saxDesktop.getMonitorVendor();

	//====================================
	// handle color depth
	//------------------------------------
	mSelectedColor = saxDesktop.getColorDepth().toInt();
	if ((mSelectedColor == 0) || (mSelectedColor == 32)) {
		mSelectedColor = saxDesktop.getColorDepthFromServer();
		if (mSelectedColor == 0) {
			log (L_WARN,
				"SCCMonitorDisplay::No colordepth set using 16bit default\n"
			);
			mSelectedColor = 16;
		}
		if (mSelectedColor == 32) {
			log (L_WARN,
				"SCCMonitorDisplay::Invalid depth 32 bit changed to 24bit\n"
			);
			mSelectedColor = 24;
		}
	}

	//====================================
	// handle resolution list
	//------------------------------------
	mSelectedResolution = saxDesktop.getResolutions (mSelectedColor);
	if (mSelectedResolution.isEmpty()) {
		mSelectedResolution = saxDesktop.getResolutionFromServer();
		log (L_WARN,"SCCMonitorDisplay::No Modes for %d bit, asking server\n",
			mSelectedColor
		);
		log (L_WARN,"SCCMonitorDisplay::Mode from server: %s\n",
			mSelectedResolution.at(0).ascii()
		);
	}

	//====================================
	// handle dualhead mode
	//------------------------------------
	if (mOptions["SaXDualHead"]) {
		mCheckDualHead -> setChecked (true);
		mConfigureDualHead -> setDisabled (false);
		QString* dualMode = mOptions["SaXDualMode"];
		QString* dualResolution = mOptions["SaXDualResolution"];
		if ((*dualMode != "off") && ( *dualResolution != "off")) {
			QString translation = mText["ActivateInfo"];
			translation.replace (
				QRegExp("\%1"),*dualMode
			);
			translation.replace (
				QRegExp("\%2"),*dualResolution
			);
			mDualInfo = translation;
		} else {
			mDualInfo = mText["Activated"];
		}
	} else {
		mDualHeadInfoLabel -> setDisabled (true);
		mConfigureDualHead -> setDisabled (true);
		mDualInfo = mText["NoConfigAvailable"];
	}
	//====================================
	// set widgets to imported data
	//------------------------------------
	// set card and monitor name...
	mLabelCardName -> setText (
		mCardVendor+" "+mCardModel
	);
	if (mMonitorVendor.contains ("Unknown",false) ||
	   (mMonitorModel.contains("Unknown",false))
	) {
		mLabelMonitorName -> setText (
			mText["UnknownMonitorPleaseChange"]
		);
	} else {
		mLabelMonitorName -> setText (
			mMonitorVendor+" "+mMonitorModel
		);
	}
	// set tab titles for toplevel dialogs...
	mChangeMonitorDialog -> setTitle (
		mText["MonitorForCard"]+" "+mCardVendor+" "+mCardModel
	);
	mCardOptionDialog -> setTitle (
		mCardVendor+" "+mCardModel
	);
	mConfigureDualHeadDialog -> setTitle (
		mCardVendor+" "+mCardModel
	);
	// set info text for dual mode
	mDualHeadInfoLabel -> setText (mDualInfo);
	// set color depth
	QString colorKey;
	QTextOStream (&colorKey) << mSelectedColor;
	mColors -> setCurrentText (*mColorDict[colorKey]);
	// set primary resolution
	QString resolutionKey = mSelectedResolution.at(0);
	if (! mResolutionDict[resolutionKey]) {
		log (L_WARN,
			"SCCMonitorDisplay::Warning: res key %s is missing\n",
			resolutionKey.ascii()
		);
		log (L_WARN,
			"SCCMonitorDisplay::Warning: reset to default 640x480 key\n"
		);
		resolutionKey="800x600";
	}
	mResolution -> setCurrentText (*mResolutionDict[resolutionKey]);
	slotResolution ( mResolution->currentItem() );
}
//====================================
// isEnabled
//------------------------------------
bool SCCMonitorDisplay::isEnabled ( void ) {
	return mEnabled;
}
//====================================
// isDualModeEnabled
//------------------------------------
bool SCCMonitorDisplay::isDualModeEnabled ( void ) {
	return mCheckDualHead->isChecked();
}
//====================================
// isDualHeadDisplay
//------------------------------------
bool SCCMonitorDisplay::isDualHeadDisplay ( void ) {
	return mDualHeadGroup->isShown();
}
//====================================
// getResolution
//------------------------------------
QList<QString> SCCMonitorDisplay::getResolution ( void ) {
	return mSelectedResolution;
}
//====================================
// getColorDepth
//------------------------------------
int SCCMonitorDisplay::getColorDepth ( void ) {
	return mSelectedColor;
}
//====================================
// getMonitorData
//------------------------------------
SCCMonitorModel* SCCMonitorDisplay::getMonitorData ( void ) {
	return mChangeMonitorDialog;
}
//====================================
// getCardData
//------------------------------------
SCCMonitorCard* SCCMonitorDisplay::getCardData ( void ) {
	return mCardOptionDialog;
}
//====================================
// getDualData
//------------------------------------
SCCMonitorDual* SCCMonitorDisplay::getDualData ( void ) {
	return mConfigureDualHeadDialog;
}
//====================================
// getCardName
//------------------------------------
QString SCCMonitorDisplay::getCardName ( void ) {
	return mLabelCardName->text();
}
//====================================
// getMonitorName
//------------------------------------
QString SCCMonitorDisplay::getMonitorName ( void ) {
	return mLabelMonitorName->text();
}
//====================================
// getDisplay
//------------------------------------
int SCCMonitorDisplay::getDisplay ( void ) {
	return mDisplay;
}
//====================================
// getDualPixmap
//------------------------------------
QPixmap SCCMonitorDisplay::getDualPixmap ( void ) {
	if (isDualModeEnabled()) {
		return getDualData()->getLayoutPixmap();
	} else {
		return QPixmap ( FIGURE_SINGLE );
	}
}
//====================================
// setMonitorResolution
//------------------------------------ 
void SCCMonitorDisplay::setMonitorResolution ( const QString& res ) {
	if (mResolutionDict[res]) {
		mResolution -> setCurrentText (*mResolutionDict[res]);
		slotResolution ( mResolution->currentItem() );
	}
}
//====================================
// setMonitorName
//------------------------------------
void SCCMonitorDisplay::setMonitorName ( const QString& name ) {
	mLabelMonitorName -> clear();
	mLabelMonitorName -> setText ( name );
}
//====================================
// setDualModeInfo
//------------------------------------
void SCCMonitorDisplay::setDualModeInfo ( const QString& info ) {
	mDualHeadInfoLabel -> clear();
	mDualHeadInfoLabel -> setText ( info );
}
//====================================
// slotConfigureDualHead 
//------------------------------------
void SCCMonitorDisplay::slotConfigureDualHead ( void ) {
	mConfigureDualHeadDialog -> show();
}
//====================================
// slotChangeMonitor
//------------------------------------
void SCCMonitorDisplay::slotChangeMonitor ( void ) {
	mChangeMonitorDialog -> show();
}
//====================================
// slotChangeCardOptions
//------------------------------------
void SCCMonitorDisplay::slotChangeCardOptions ( void ) {
	mCardOptionDialog -> show();
}
//====================================
// setEnabled
//------------------------------------
void SCCMonitorDisplay::setEnabled ( bool status ) {
	mCheckEnable -> setChecked ( status );
	slotActivateDisplay();
}
//====================================
// slotActivateDisplay
//------------------------------------
void SCCMonitorDisplay::slotActivateDisplay ( void ) {
	if (mCheckEnable -> isChecked()) {
		mCardMonitorGroup -> setDisabled (false);
		mPropertyGroup    -> setDisabled (false);
		mDualHeadGroup    -> setDisabled (false);
		SaXManipulateDesktop saxDesktop (
			mSection["Desktop"],mSection["Card"],mSection["Path"]
		);
		saxDesktop.selectDesktop ( mDisplay );
		if (saxDesktop.getDualHeadProfile().isEmpty()) {
			mDualHeadGroup -> setDisabled ( true );
		}
		mEnabled = true;
	} else {
		mCardMonitorGroup -> setDisabled (true);
		mPropertyGroup    -> setDisabled (true);
		mDualHeadGroup    -> setDisabled (true);
		mEnabled = false;
	}
	emit sigActivate();
}
//====================================
// slotActivateDualHead
//------------------------------------
void SCCMonitorDisplay::slotActivateDualHead ( void ) {
	if (mCheckDualHead -> isChecked()) {
		mDualHeadEnabled = true;
		mDualHeadInfoLabel -> setDisabled (false);
		mConfigureDualHead -> setDisabled (false);
		setCombinedDisplaySize (false);
	} else {
		mDualHeadEnabled = false;
		mDualHeadInfoLabel -> setDisabled (true);
		mConfigureDualHead -> setDisabled (true);
		setCombinedDisplaySize (false);
	}
	emit sigDualModeChanged ( this );
}
//====================================
// slotResolution
//------------------------------------
void SCCMonitorDisplay::slotResolution ( int index ) {
	//====================================
	// Check Framebuffer combination
	//------------------------------------
	QString selected = mResolution->text(index);
	if (mIsFbdevBased) {
		Q3DictIterator<QString> it (mResolutionDict);
		QString baseResolution;
		for (; it.current(); ++it) {
		if (*it.current() == selected) {
			baseResolution = (it.currentKey());
		}
		}
		if (!baseResolution.isNull()) {
			int mode = mSaxDesktop->getFBKernelMode (
				baseResolution,mSelectedColor
			);
			if (! mode > 0) {
				SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
				SCCMessage* mMessageBox = new SCCMessage (
					this, mTextPtr, SaXMessage::OK,
					mText["FBWarning"],"MessageCaption",SaXMessage::Warning
				);
				mMessageBox -> showMessage();
				mResolution->setCurrentText(
					*mResolutionDict.find(mSelectedResolution.at(0))
				);
				return;
			}
		}
	}
	//====================================
	// find first startup resolution
	//------------------------------------
	Q3DictIterator<QString> it (mResolutionDict);
	long basePixels  = 0;
	int  basePixelsX = 0;
	int  basePixelsY = 0;
	mSelectedResolution.clear();
	for (; it.current(); ++it) {
		if (*it.current() == selected) {
			QString baseResolution = it.currentKey();
			QStringList tokens = QStringList::split ( "x", baseResolution );
			int x = tokens.first().toInt();
			int y = tokens.last().toInt();
			basePixels  = x * y;
			basePixelsX = x;
			basePixelsY = y;
			mSelectedResolution.append ( baseResolution );
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
		long pixelSpace = x * y;
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
		QString* key = new QString();
		QTextOStream (key) << sortResolution[n];
		mSelectedResolution.append ( *metaResolution[*key] );
	}
}
//====================================
// slotColors
//------------------------------------
void SCCMonitorDisplay::slotColors ( int index ) {
	//====================================
	// Check Framebuffer combination
	//------------------------------------
	QString selected = mColors->text(index);
	if (mIsFbdevBased) {
		Q3DictIterator<QString> it (mColorDict);
		int color = 0;
		for (; it.current(); ++it) {
		if (*it.current() == selected) {
			color = it.currentKey().toInt();
		}
		}
		if (color) {
			int mode = mSaxDesktop->getFBKernelMode (
				mSelectedResolution.at(0),color
			);
			if (! mode > 0) {
				SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
				SCCMessage* mMessageBox = new SCCMessage (
					this, mTextPtr, SaXMessage::OK,
					mText["FBWarning"],"MessageCaption",SaXMessage::Warning
				);
				mMessageBox -> showMessage();
				QString* color = new QString();
				QTextOStream (color) << mSelectedColor;
				mColors->setCurrentText (
					*mColorDict.find (*color)
				);
				return;
			}
		}
	}
	//====================================
	// Set color depth
	//------------------------------------
	Q3DictIterator<QString> it (mColorDict);
	for (; it.current(); ++it) {
		if (*it.current() == selected) {
			mSelectedColor = it.currentKey().toInt();
			break;
		}
	}
}
//====================================
// setCommonButtonWidth
//------------------------------------
void SCCMonitorDisplay::setCommonButtonWidth ( void ) {
	QPushButton* thisButtonWidgets[] = {
		mCardOptions,mChangeMonitor,mConfigureDualHead
	};
	int fixedWidth = 0;
	for (int i=0;i<3;i++) {
	if (thisButtonWidgets[i]->width() > fixedWidth) {
		fixedWidth = thisButtonWidgets[i]->width();
	}
	}
	for (int i=0;i<3;i++) {
		thisButtonWidgets[i]->setFixedWidth (fixedWidth);
	}
}
//====================================
// setCombinedDisplaySize
//------------------------------------
void SCCMonitorDisplay::setCombinedDisplaySize ( bool combinedSize ) {
	QList<QString> combined;
	SaXImportSysp* desktop = new SaXImportSysp (SYSP_DESKTOP);
	QString* xs = new QString();
	QString* ys = new QString();
	desktop -> setID (mDisplay);
	desktop -> doImport();
	QString size = desktop->getItem("Size");
	if (size.isNull()) {
		return;
	}
	QString displaySize1 = desktop->getItem("Size");
	QStringList tokens1 = QStringList::split ( "x", displaySize1 );
	int x = tokens1.first().toInt();
	int y = tokens1.last().toInt();
	QTextOStream (xs) << x; combined.append (*xs);
	QTextOStream (ys) << y; combined.append (*ys);
	if (! combinedSize) {
		getMonitorData() -> setDisplaySize ( combined );
	} else {
		if (desktop->getItem(QString("Size[2]")).isNull()) {
			log (L_WARN,"SCCMonitorDisplay::No DDC2 size info available\n");
			return;
		}
		QString displaySize2 = desktop->getItem("Size[2]");
		QStringList tokens2 = QStringList::split ( "x", displaySize2 );
		int x2 = tokens2.first().toInt();
		int y2 = tokens2.last().toInt();
		SCCMonitorDual* dualData = getDualData();
		switch (dualData->getLayout()) {
			case DUAL_LEFTOF:
				x = x + x2;
			break;
			case DUAL_RIGHTOF:
				x = x + x2;
			break;
			case DUAL_ABOVEOF:
				y = y + y2;
			break;
			case DUAL_BELOWOF:
				y = y + y2;
			break;
			default:
			break;
		}
		combined.clear();
		QTextOStream (xs) << x; combined.append (*xs);
		QTextOStream (ys) << y; combined.append (*ys);
		getMonitorData() -> setDisplaySize ( combined );
	}
	if ((mShowDisplaySizeMessage) && (isVisible())) {
		SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
		SCCMessage* mMessageBox = new SCCMessage (
			this, mTextPtr, SaXMessage::OK,
			mText["DisplaySizeInfo"],"MessageCaption",SaXMessage::Information
		);
		mMessageBox -> showMessage();
		mShowDisplaySizeMessage = false;
	}
}
} // end namespace
