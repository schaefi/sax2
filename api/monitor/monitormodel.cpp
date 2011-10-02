/**************
FILE          : monitormodel.cpp
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
              : - monitormodel.h: SaXGUI::SCCMonitor header definitions
              : - monitormodel.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitormodel.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3Frame>

#define UNIT_SIZE 128

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitorModel::SCCMonitorModel (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	const QString& cardname, const QString& sync, const QString& aspect,
	int display,QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mModelTab  = new Q3VBox ( this );
	mAspectTab = new Q3VBox ( this );
	mSyncTab   = new Q3VBox ( this );

	//=====================================
	// prepare tabed dialog
	//-------------------------------------
	mModelDialog = new Q3TabDialog  ( this,0,true );
	mModelDialog -> setCaption  ( mText["MonitorCaption"] );
	mModelDialog -> setOkButton ( mText["Ok"] );
	mModelDialog -> setCancelButton ( mText["Cancel"] );
	mModelDialog -> addTab ( mModelTab, cardname );
	mModelDialog -> addTab ( mAspectTab, aspect );
	mModelDialog -> addTab ( mSyncTab, sync );

	//=====================================
	// set import flag
	//-------------------------------------
	mSaxDesktop = 0;
	mNeedImport = true;
	mDisplay = display;

	//=====================================
	// create macro widgets [Model]
	//-------------------------------------
	mModelTab -> setMargin ( 20 );
	mModelVendorGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["MonitorSelection"],mModelTab
	);
	mVendorList = new Q3ListBox ( mModelVendorGroup );
	mModelList  = new Q3ListBox ( mModelVendorGroup );
	mModelTab -> setSpacing ( 15 );
	mToolBox = new Q3HBox ( mModelTab );
	mCheckDPMS = new QCheckBox ( mText["DPMS"],mToolBox );
	mDisk = new QPushButton ( mText["DiskInfo"],mToolBox );
	mToolBox -> setStretchFactor ( mCheckDPMS, 10 );

	//=====================================
	// create macro widgets [Sync]
	//-------------------------------------
	mSyncTab -> setMargin ( 20 );
	mSyncGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["SyncSelection"],mSyncTab
	);
	Q3VBox* textBox = new Q3VBox ( mSyncGroup );
	mLabelWarning  = new QLabel ( mText["FrqNote"],textBox );
	mLabelWarning -> setFixedWidth ( 200 );
	mLabelWarning -> setWordWrap( true );
	QLabel* latent1 = new QLabel ( textBox );
	textBox -> setStretchFactor ( latent1,10 );

	Q3VBox* seperatorBox = new Q3VBox ( mSyncGroup );
	Q3Frame* seperator   = new Q3Frame ( seperatorBox );
	seperator -> setFixedWidth (2);
	seperator -> setFrameStyle (
		Q3Frame::VLine | Q3Frame::Raised
	);
	seperatorBox -> setStretchFactor ( seperator,10 );

	mSyncBox = new Q3VBox ( mSyncGroup );
	Q3HBox* mHSyncBox = new Q3HBox ( mSyncBox );
	mLabelHorizontal  = new QLabel ( mText["Horizontal"],mHSyncBox );
	mHSyncBox -> setSpacing  ( 10 );
	mHSpinMin = new QSpinBox ( 30,300,1,mHSyncBox );
	mHSyncBox -> setSpacing  ( 5 );
	QLabel* hbar = new QLabel ( mHSyncBox );
	hbar -> setText ("-");
	mHSyncBox -> setSpacing  ( 5 );
	mHSpinMax = new QSpinBox ( 30,300,1,mHSyncBox );
	mHSyncBox -> setSpacing  ( 10 );
	mLabelHUnit = new QLabel ( mText["Khz"],mHSyncBox );
	mLabelHUnit -> setFixedWidth ( 50 );
	mHSyncBox -> setStretchFactor ( mHSpinMin,10 );
	mHSyncBox -> setStretchFactor ( mHSpinMax,10 );

	mSyncBox -> setSpacing ( 15 );
	Q3HBox* mVSyncBox = new Q3HBox ( mSyncBox );
	mLabelVertical  = new QLabel ( mText["Vertical"],mVSyncBox );
	mVSyncBox -> setSpacing  ( 10 );
	mVSpinMin = new QSpinBox ( 50,300,1,mVSyncBox );
	mVSyncBox -> setSpacing  ( 5 );
	QLabel* vbar = new QLabel ( mVSyncBox );
	vbar -> setText ("-");
	mVSyncBox -> setSpacing  ( 5 );
	mVSpinMax = new QSpinBox ( 60,300,1,mVSyncBox );
	mVSyncBox -> setSpacing  ( 10 );
	mLabelVUnit = new QLabel ( mText["Hz"],mVSyncBox );
	mLabelVUnit -> setFixedWidth ( 50 );
	mVSyncBox -> setStretchFactor ( mVSpinMin,10 );
	mVSyncBox -> setStretchFactor ( mVSpinMax,10 );

	QLabel* latent2 = new QLabel ( mSyncBox );
	mSyncBox -> setStretchFactor ( latent2,10 );
	QFontMetrics metrics ( font() );
	int charVS = metrics.width (mText["Vertical"]);
	int charHS = metrics.width (mText["Horizontal"]);
	if ( charVS < charHS ) {
		mLabelHorizontal->setFixedWidth ( charHS );
		mLabelVertical->setFixedWidth ( charHS );
	} else {
		mLabelHorizontal->setFixedWidth ( charVS );
		mLabelVertical->setFixedWidth ( charVS );
	}

	//=====================================
	// create macro widgets [Aspect]
	//-------------------------------------
	mAspectTab -> setMargin ( 20 );
	mAspectGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["AspectSelection"],mAspectTab
	);
	Q3VBox* aspectBox = new Q3VBox ( mAspectGroup );
	mAspectGroup -> addSpace ( 20 );
	Q3VBox* ratioBox  = new Q3VBox ( mAspectGroup );
	mLabelAspect = new QLabel ( mText["Diagonal"],aspectBox );
	aspectBox -> setSpacing ( 15 );
	mTraversal = new QComboBox ( aspectBox );
	mLabelRatio  = new QLabel ( mText["Aspect"],ratioBox );
	ratioBox  -> setSpacing ( 10 );
	mAspect = new QComboBox ( ratioBox );
	mAspectTab -> setSpacing ( 15 );
	mDisplaySizeGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["SizeSelection"],mAspectTab
	);
	Q3VBox* xBox = new Q3VBox ( mDisplaySizeGroup );
	mDisplaySizeGroup -> addSpace ( 20 );
	Q3VBox* yBox = new Q3VBox ( mDisplaySizeGroup );
	mLabelX = new QLabel ( mText["XSize"],xBox );
	xBox -> setSpacing ( 15 );
	mSizeX = new QSpinBox ( 0,1000,1,xBox );
	mLabelY = new QLabel ( mText["YSize"],yBox );
	yBox -> setSpacing ( 15 );
	mSizeY = new QSpinBox ( 0,1000,1,yBox );
	
	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mModelDialog , SIGNAL ( applyButtonPressed ( void )),
		this         , SLOT   ( slotOk ( void ))
	);
	QObject::connect (
		mDisk        , SIGNAL ( clicked  ( void )),
		this         , SLOT   ( slotDisk ( void ))
	);
	QObject::connect (
		mVendorList  , SIGNAL ( selectionChanged ( Q3ListBoxItem* )),
		this         , SLOT   ( slotVendor       ( Q3ListBoxItem* ))
	);
	QObject::connect (
		mModelList   , SIGNAL ( selectionChanged ( Q3ListBoxItem* )),
		this         , SLOT   ( slotName         ( Q3ListBoxItem* ))
	);
	QObject::connect (
		mAspect      , SIGNAL ( activated     ( int )),
		this         , SLOT   ( slotRatio     ( int ))
	);
	QObject::connect (
		mTraversal   , SIGNAL ( activated     ( int )),
		this         , SLOT   ( slotTraversal ( int ))
	);
}
//=====================================
// show tabed dialog
//-------------------------------------
void SCCMonitorModel::show (void) {
	mModelDialog->setGeometry  ( 0,0,640,480 );
	mModelDialog->move (
		getPosition (mModelDialog->width(),mModelDialog->height())
	);
	import();
	mModelDialog->show();
	mModelDialog->raise();
}
//====================================
// init
//------------------------------------
void SCCMonitorModel::init ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create manipulators...
	//------------------------------------
	mSaxDesktop = new SaXManipulateDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// select desktop
	//------------------------------------
	mSaxDesktop -> selectDesktop ( mDisplay );
	
	//====================================
	// insert CDB monitors
	//------------------------------------
	mCDBMonitorVendors = mSaxDesktop->getCDBMonitorVendorList();
	QString it;
	foreach (it,mCDBMonitorVendors) {
		mVendorList -> insertItem (it);
	}
	mVendorList -> sort();
	
	//====================================
	// insert aspect and diagonal data
	//------------------------------------
	mAspect    -> insertItem ( mText["NotConfigured"] );
	mTraversal -> insertItem ( mText["NotConfigured"] );
	SCCFile diagonalFile ( DIAGONAL_FILE );
	mTraversalList = diagonalFile.readList();
	QString io;
	foreach (io,mTraversalList) {
		QString item;
		QTextOStream (&item) << io << " " << mText["UnitZoll"];
		mTraversal -> insertItem ( item );
	}
	SCCFile aspectFile ( ASPECT_FILE );
	mAspectList = aspectFile.readList();
	QString ia;
	foreach (ia,mAspectList) {
		mAspect -> insertItem (ia);
	}
}
//====================================
// import
//------------------------------------
void SCCMonitorModel::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	if ( mNeedImport ) {
		//=====================================
		// check manipulator access
		//-------------------------------------
		if (! mSaxDesktop ) {
			return;
		}
		//====================================
		// handle monitor vendor/model name
		//------------------------------------
		mSelectedMonitorVendor = mSaxDesktop -> getMonitorVendor();
		mSelectedMonitorName   = mSaxDesktop -> getMonitorName();

		//====================================
		// handle DPMS status
		//------------------------------------
		mDPMSStatus = mSaxDesktop -> DPMSEnabled();

		//====================================
		// handle display size values
		//------------------------------------
		QList<QString> displaySize = mSaxDesktop -> getDisplaySize();
		if (! displaySize.isEmpty()) {
			mDisplaySizeX = displaySize.at(0).toInt();
			mDisplaySizeY = displaySize.at(1).toInt();
		} else {
			mDisplaySizeX = 305;
			mDisplaySizeY = 230;
		}

		//====================================
		// handle hsync/vsync values
		//------------------------------------
		QList<QString> hsyncRange = mSaxDesktop -> getHsyncRange();
		QList<QString> vsyncRange = mSaxDesktop -> getVsyncRange();
		if (! hsyncRange.isEmpty()) {
			mHsyncMin = hsyncRange.at(0).toInt();
			mHsyncMax = hsyncRange.at(1).toInt();
		}
		if (! vsyncRange.isEmpty()) {
			mVsyncMin = vsyncRange.at(0).toInt();
			mVsyncMax =	vsyncRange.at(1).toInt();
		}

		//====================================
		// import done set flag
		//------------------------------------
		mNeedImport = false;
	}
	//====================================
	// setup vendor name listboxes
	//------------------------------------
	Q3ListBoxItem* vendor = mVendorList -> findItem ( mSelectedMonitorVendor );
	if ( vendor ) {
		mVendorList -> setSelected ( vendor, true );
		slotVendor ( vendor );
		Q3ListBoxItem* name = mModelList -> findItem ( mSelectedMonitorName );
		if ( name ) {
			mModelList -> setSelected ( name, true );
			slotName ( name );
		}
	}
	//====================================
	// setup DPMS
	//------------------------------------
	mCheckDPMS -> setChecked ( mDPMSStatus );
	
	//====================================
	// setup display size widgets
	//------------------------------------
	mSizeX -> setValue ( mDisplaySizeX );
	mSizeY -> setValue ( mDisplaySizeY );
	if ((mDisplaySizeX > 0) && (mDisplaySizeY > 0)) {
		char traversal[UNIT_SIZE];
		snprintf (traversal,UNIT_SIZE,
			"%.1f",getTraversal (mDisplaySizeX,mDisplaySizeY)
		);
		QPoint ratio = getRatio (mDisplaySizeX,mDisplaySizeY);
		QString infoTraversal;
		QString infoRatio;
		QTextOStream (&infoTraversal)
			<< traversal << " " << mText["UnitZoll"];
		mTraversal -> setCurrentText (infoTraversal);
		QTextOStream (&infoRatio)
			<< ratio.x() << "/" << ratio.y();
		mAspect -> setCurrentText (infoRatio);
	} else {
		mAspect    -> insertItem ( mText["NotConfigured"] );
		mTraversal -> insertItem ( mText["NotConfigured"] );
	}

	//====================================
	// setup hsync/vsync widgets
	//------------------------------------
	mHSpinMin -> setValue ( mHsyncMin );
	mHSpinMax -> setValue ( mHsyncMax );
	mVSpinMin -> setValue ( mVsyncMin );
	mVSpinMax -> setValue ( mVsyncMax );
}
//=====================================
// isDPMSEnabled
//-------------------------------------
bool SCCMonitorModel::isDPMSEnabled ( void ) {
	return mDPMSStatus;
}
//=====================================
// getVendorName
//-------------------------------------
QString SCCMonitorModel::getVendorName ( void ) {
	return mSelectedMonitorVendor;
}
//=====================================
// getModelName
//-------------------------------------
QString SCCMonitorModel::getModelName  ( void ) {
	return mSelectedMonitorName;
}
//=====================================
// getSizeX
//-------------------------------------
int SCCMonitorModel::getSizeX ( void ) {
	return mDisplaySizeX;
}
//=====================================
// getSizeY
//-------------------------------------
int SCCMonitorModel::getSizeY ( void ) {
	return mDisplaySizeY;
}
//=====================================
// getHSmin
//-------------------------------------
int SCCMonitorModel::getHSmin ( void ) {
	return mHsyncMin;
}
//=====================================
// getHSmax
//-------------------------------------
int SCCMonitorModel::getHSmax ( void ) {
	return mHsyncMax;
}
//=====================================
// getVSmin
//-------------------------------------
int SCCMonitorModel::getVSmin ( void ) {
	return mVsyncMin;
}
//=====================================
// getVSmax 
//-------------------------------------
int SCCMonitorModel::getVSmax ( void ) {
	return mVsyncMax;
}
//=====================================
// getExtraModeline
//-------------------------------------
QString SCCMonitorModel::getExtraModeline ( void ) {
	return mExtraModeline;
}
//=====================================
// setTitle
//-------------------------------------
void SCCMonitorModel::setTitle ( const QString & title ) {
	mModelDialog -> changeTab ( mModelTab,title );
}
//=====================================
// setDisplaySize
//-------------------------------------
void SCCMonitorModel::setDisplaySize ( QList<QString> size ) {
	mDisplaySizeX = size.at(0).toInt();
	mDisplaySizeY = size.at(1).toInt();
	mSizeX -> setValue ( mDisplaySizeX );
	mSizeY -> setValue ( mDisplaySizeY );
}
//=====================================
// slotOk
//-------------------------------------
void SCCMonitorModel::slotOk ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// get parent SCCMonitorDisplay ptr
	//-------------------------------------
	SCCMonitorDisplay* pDisplay = (SCCMonitorDisplay*)mParent;

	//=====================================
	// save data to dialog export variables
	//-------------------------------------
	mDPMSStatus = mCheckDPMS -> isChecked();
	mDisplaySizeX = mSizeX -> value();
	mDisplaySizeY = mSizeY -> value();
	mHsyncMin = mHSpinMin -> value();
	mHsyncMax = mHSpinMax -> value();
	mVsyncMin = mVSpinMin -> value();
	mVsyncMax = mVSpinMax -> value();

	//=====================================
	// save data to dialog export variables
	//-------------------------------------
	if ((! mVendorList->selectedItem()) || (! mModelList->selectedItem())) {
		return;
	}
	mSelectedMonitorVendor = mVendorList -> selectedItem()->text();
	mSelectedMonitorName   = mModelList  -> selectedItem()->text();

	//=====================================
	// update monitor name
	//-------------------------------------
	QString monitorName;
	QTextOStream (&monitorName) <<
		mSelectedMonitorVendor << " " << mSelectedMonitorName;
	pDisplay -> setMonitorName ( monitorName );

	//=====================================
	// update monitor resolution
	//-------------------------------------
	QRegExp resExp ("[0-9]+x[0-9]+");
	resExp.setCaseSensitive (false);
	int rpos = resExp.search (monitorName,0);
	if (rpos >= 0) {
		QString res = monitorName.lower();
		int npos = 0;
		for (int n=rpos;n<(int)res.length();n++) {
		if ((res.at(n) == 'x') || (res.at(n).isDigit())) {
			npos++;
		} else {
			break;
		}
		}
		QString substr(res.mid (rpos,npos));
		pDisplay -> setMonitorResolution ( substr );
	}
	//=====================================
	// update layout tooltip
	//-------------------------------------
	emit sigMonitorChanged ( pDisplay );
}
//=====================================
// slotDisk
//-------------------------------------
void SCCMonitorModel::slotDisk ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
    SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// show info message box
	//-------------------------------------
	SCCMessage* mMessageBox = new SCCMessage (
		mModelDialog, mTextPtr, SaXMessage::OK_CANCEL,"DiskText",
		"MessageCaption", SaXMessage::Information
	);
	QString msgResult = mMessageBox -> showMessage();
	if ( msgResult == mText["Cancel"] ) {
		return;
	}
	//=====================================
	// read .inf file using getINF script
	//-------------------------------------
	qApp->setOverrideCursor ( Qt::forbiddenCursor );
	Q3Process* proc = new Q3Process ();
	proc -> addArgument ( GETINF );
	if ( ! proc -> start() ) {
		qApp->restoreOverrideCursor();
		return;
	}
	while (proc->isRunning()) {
		usleep (1000);
	}
	//=====================================
	// process data lines...
	//-------------------------------------
	QByteArray data = proc->readStdout();
	QStringList lines = QStringList::split ("\n",data);
	if (! lines.isEmpty()) {
		QString diskVendor;
		QString diskModel;
		for ( QStringList::Iterator
			in = lines.begin(); in != lines.end(); ++in
		) {
			QString line (*in);
			QStringList tokens = QStringList::split ( ",", line );
			diskVendor = tokens[0];
			diskModel  = tokens[1];
			mSaxDesktop -> setCDBMonitorData (
				diskVendor,diskModel,"VertRefresh",tokens[2]
			);
			mSaxDesktop -> setCDBMonitorData (
				diskVendor,diskModel,"HorizSync",tokens[3]
			);
		}
		//=====================================
		// update vendor listbox
		//-------------------------------------
		mVendorList -> clear();
		mCDBMonitorVendors = mSaxDesktop->getCDBMonitorVendorList();
		QString it;
		foreach (it,mCDBMonitorVendors) {
			mVendorList -> insertItem (it);
		}
		mVendorList -> sort();
		//=====================================
		// select and apply item
		//-------------------------------------
		Q3ListBoxItem* vendorItem = mVendorList -> findItem ( diskVendor );
		if ( vendorItem ) {
			mVendorList -> setSelected ( vendorItem, true );
			slotVendor ( vendorItem );
			Q3ListBoxItem* nameItem = mModelList -> findItem ( diskModel );
			if ( nameItem ) {
				mModelList -> setSelected ( nameItem, true );
				slotName ( nameItem );
			}
		}
		qApp->restoreOverrideCursor();
	} else {
		//=====================================
		// show error message box
		//-------------------------------------
		qApp->restoreOverrideCursor();
		SCCMessage* mMessageBox = new SCCMessage (
			mModelDialog, mTextPtr, SaXMessage::OK,"DiskError",
			"MessageCaption", SaXMessage::Critical
		);
		mMessageBox -> showMessage();
	}
}
//=====================================
// slotVendor
//-------------------------------------
void SCCMonitorModel::slotVendor ( Q3ListBoxItem* item ) {
	if (! mSaxDesktop ) {
		return;
	}
	mModelList -> clear();
	mCDBMonitorModels = mSaxDesktop->getCDBMonitorModelList (
		item->text()
	);
	QString it;	
	foreach (it,mCDBMonitorModels) {
		mModelList -> insertItem (it);
	}
	mModelList -> sort();
}
//=====================================
// slotName
//-------------------------------------
void SCCMonitorModel::slotName ( Q3ListBoxItem* item ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// check manipulator access
	//-------------------------------------
	if (! mSaxDesktop ) {
		return;
	}
	mCDBMonitorData = mSaxDesktop->getCDBMonitorData (
		mVendorList->selectedItem()->text(),item->text()
	);
	mCheckDPMS -> setChecked ( false );
	mHSpinMin  -> setValue   ( 30 );
	mHSpinMax  -> setValue   ( 50 );
	mVSpinMin  -> setValue   ( 50 );
	mVSpinMax  -> setValue   ( 60 );
	mExtraModeline = "";
	Q3DictIterator<QString> it (mCDBMonitorData);
	for (; it.current(); ++it) {
		QString key = it.currentKey();
		QString val = *it.current();
		if (key == "VertRefresh") {
			QStringList tokens = QStringList::split ( "-", val );
			mVSpinMin -> setValue ( tokens.first().toInt() );
			mVSpinMax -> setValue ( tokens.last().toInt() );
		}
		if (key == "HorizSync") {
			QStringList tokens = QStringList::split ( "-", val );
			mHSpinMin -> setValue ( tokens.first().toInt() );
			mHSpinMax -> setValue ( tokens.last().toInt() );
		}
		if (key == "Option") {
		if (val == "DPMS") {
			mCheckDPMS -> setChecked ( true );
		}
		}
		if (key == "DPI") {
			QStringList tokens = QStringList::split ( "x", val );
			int x = tokens.first().toInt();
			int y = tokens.last().toInt();
			mSizeX -> setValue ( x );
			mSizeY -> setValue ( y );
			char traversal[UNIT_SIZE];
			snprintf (traversal,UNIT_SIZE,
				"%.1f",getTraversal (mDisplaySizeX,mDisplaySizeY)
			);
			QPoint ratio = getRatio (x,y);
			QString infoTraversal;
			QString infoRatio;
			QTextOStream (&infoTraversal)
				<< traversal << " " << mText["UnitZoll"];
			mTraversal -> setCurrentText (infoTraversal);
			QTextOStream (&infoRatio)
				<< ratio.x() << "/" << ratio.y();
			mAspect -> setCurrentText (infoRatio);
		}
		if (key == "SpecialModeline") {
			mExtraModeline = val;
		}
	}
}
//=====================================
// slotRatio
//-------------------------------------
void SCCMonitorModel::slotRatio ( int ) {
	slotTraversal (0);
}
//=====================================
// slotTraversal
//-------------------------------------
void SCCMonitorModel::slotTraversal ( int ) {
	// .../
	//! this function is called if you select a traversal 
	//! from the combobox located in the properties dialog
	// ---
	QString selected = mTraversal -> currentText();
	QStringList selectList = QStringList::split ( " ", selected );
	double traversal = selectList.first().toDouble();
	selected = mAspect -> currentText();
	selectList = QStringList::split ( "/", selected );
	int aspect = selectList.first().toInt();
	int ratio  = selectList.last().toInt();
	QPoint n = translateTraversal (traversal,aspect,ratio);
	mSizeX -> setValue (n.x());
	mSizeY -> setValue (n.y());
}
//=====================================
// translateTraversal...
//-------------------------------------
QPoint SCCMonitorModel::translateTraversal (
	double traversal,int aspect, int ratio
) {
	// .../
	//! calculate x and y sizes in mm refering to traversal and
	//! aspect / ratio
	// ---
	QPoint result;
	traversal = traversal * 25.4;
	double ar = (double)aspect / (double)ratio;
	double y = sqrt ( (traversal * traversal) / (ar * ar + 1.0) );
	double x = ar * y;
	result.setX ( (int)(round(x)) );
	result.setY ( (int)(round(y)) );
	return result;
}
//=====================================
// getTraversal...
//-------------------------------------
double SCCMonitorModel::getTraversal (int x,int y) {
	// .../
	//! calculate the length of the traversal refering to
	//! the length of x and y
	// ----
	return (sqrt (x*x + y*y) / 25.4);
}
//=====================================
// getRatio...
//-------------------------------------
QPoint SCCMonitorModel::getRatio (int x,int y) {
	// .../
	//! calculate the aspect/ratio refering to
	//! the length of x and y
	// ----
	QPoint result;
	result.setX (4);
	result.setY (3);
	double ar = (double)x / (double)y;
	if ( ar > 1.4 ) {
		result.setX (16);
		result.setY (10);
	} else if (( ar <= 1.4 ) && ( ar > 1.3 )) {
		result.setX (4);
		result.setY (3);
	} else if ( ar <= 1.3 ) {
		result.setX (5);
		result.setY (4);
	}
	return result;
}
} // end namespace
