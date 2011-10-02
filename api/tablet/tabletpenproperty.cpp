/**************
FILE          : tabletpenproperty.cpp
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
              : - tabletpenproperty.h: SaXGUI::SCCTabletPenProperty implement.
              : - tabletpenproperty.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#include "tabletpenproperty.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTabletPenProperty::SCCTabletPenProperty (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	const QString& penname, int display,QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mTabletPenTab = new Q3VBox ( this );

	//=====================================
	// prepare tabed dialog
	//-------------------------------------
	mTabletPenDialog = new Q3TabDialog ( this,0,true );
	mTabletPenDialog -> setCaption  ( mText["TabletCaption"] );
	mTabletPenDialog -> setOkButton ( mText["Ok"] );
	mTabletPenDialog -> setCancelButton ( mText["Cancel"] );
	mTabletPenDialog -> addTab ( mTabletPenTab, penname );

	//=====================================
	// set import flag
	//-------------------------------------
	mNeedImport = true;
	mDisplay = display;

	//=====================================
	// create macro widget
	//-------------------------------------
	mTabletPenTab -> setMargin ( 20 );
	mModeGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["PenTabletMode"],mTabletPenTab
	);
	mRelative = new QRadioButton (
		mText["Relative"], mModeGroup
	);
	mAbsolute = new QRadioButton (
		mText["Absolute"], mModeGroup
	);
	mTabletPenTab -> setSpacing ( 15 );
	mOptionGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["TabletOptions"],mTabletPenTab
	);
	mOptionList = new Q3ListBox ( mOptionGroup );
	mOptionList -> setSelectionMode ( Q3ListBox::Multi );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mTabletPenDialog , SIGNAL (applyButtonPressed ( void )),
		this             , SLOT   (slotOk ( void ))
	);
	QObject::connect (
		mOptionList , SIGNAL (clicked    ( Q3ListBoxItem*)),
		this        , SLOT   (slotOption ( Q3ListBoxItem*))
	);
}
//=====================================
// show tabed dialog
//-------------------------------------
void SCCTabletPenProperty::show (void) {
	mTabletPenDialog->setGeometry  ( 0,0,640,480 );
	mTabletPenDialog->move (
		getPosition (mTabletPenDialog->width(),mTabletPenDialog->height())
	);
	import();
	mTabletPenDialog->show();
	mTabletPenDialog->raise();
}
//====================================
// init
//------------------------------------
void SCCTabletPenProperty::init ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------
	mSaxTablet = new SaXManipulateTablets (
		mSection["Pointers"],mSection["Layout"]
	);
	//=====================================
	// select tablet pen/eraser
	//-------------------------------------
	mSaxTablet -> selectPointer ( mDisplay );

	//=====================================
	// set default tablet mode to relative
	//------------------------------------- 
	mRelative -> setChecked ( true );
}
//====================================
// import
//------------------------------------
void SCCTabletPenProperty::import ( void ) {
	//=====================================
	// check if there is data...
	//-------------------------------------
	if ( mDisplay == 0 ) {
		return;
	}
	//=====================================
	// select tablet pen/eraser
	//-------------------------------------
	mSaxTablet -> selectPointer ( mDisplay );

	//=====================================
	// import tablet pen/eraser data
	//-------------------------------------
	if ( mNeedImport ) {
		//====================================
		// handle pen/eraser mode
		//------------------------------------
		QString mode = mSaxTablet->getMode();
		if (mode == "Absolute") {
			mPenMode = 1;
		} else {
			mPenMode = 0;
		}
		//====================================
		// handle pen/eraser options
		//------------------------------------
		mSelectedOptions = mSaxTablet->getOptions();

		//====================================
		// import done set flag
		//------------------------------------
		mNeedImport = false;
	}
	//====================================
	// setup pen/eraser mode
	//------------------------------------
	if (mPenMode == 0) {
		mRelative -> setChecked ( true );
	} else {
		mAbsolute -> setChecked ( true );
	}
	//====================================
	// setup pen/eraser options
	//------------------------------------
	if (! mOptionDict.isEmpty()) {
		mOptionList->clear();
		Q3DictIterator<QString> it (mOptionDict);
		for (; it.current(); ++it) {
			mOptionList->insertItem ( it.currentKey() );
		}
	}
	Q3DictIterator<QString> it (mSelectedOptions);
	for (; it.current(); ++it) {
		QString opt;
		if ( it.current()->isEmpty()) {
			QTextOStream (&opt) << it.currentKey();
		} else {
			QTextOStream (&opt) << it.currentKey() << " >>> " << it.current();
		}
		if ( Q3ListBoxItem* item = mOptionList->findItem (it.currentKey()) ) {
			int index = mOptionList->index (item);
			mOptionList -> changeItem  ( opt, index );
			mOptionList -> setSelected ( index, true );
		}
	}
	mOptionList -> ensureCurrentVisible();
}
//====================================
// getID
//------------------------------------
int SCCTabletPenProperty::getID ( void ) {
	return mDisplay;
}
//====================================
// setID
//------------------------------------
void SCCTabletPenProperty::setID ( int id ) {
	mDisplay = id;
}
//====================================
// setupPen
//------------------------------------
void SCCTabletPenProperty::setupPen (
	const QString& vendor, const QString& name
) {
	//====================================
	// retrieve data record for tablet
	//------------------------------------
	Q3Dict<QString> tabletDict = mSaxTablet->getPenData ( vendor,name );

	//====================================
	// set tablet mode
	//------------------------------------
	if (tabletDict["TabletMode"]) {
		QString mode = *tabletDict["TabletMode"];
		if ( mode == "Absolute" ) {
			mAbsolute -> setChecked ( true );
		} else {
			mRelative -> setChecked ( true );
		}
	}
	//====================================
	// set tablet driver specific options
	//------------------------------------
	if (tabletDict["Driver"]) {
		mOptionList->clear();
		mOptionDict = mSaxTablet->getTabletOptions (
			*tabletDict["Driver"]
		);
		Q3DictIterator<QString> it (mOptionDict);
		for (; it.current(); ++it) {
			mOptionList->insertItem ( it.currentKey() );
		}
	}
	//====================================
	// activate tablet options [Bool]
	//------------------------------------
	if (tabletDict["Option"]) {
		QStringList tokens = QStringList::split (",",*tabletDict["Option"]);
		for ( QStringList::Iterator
			in = tokens.begin(); in != tokens.end(); ++in
		) {
			QString opt (*in);
			if ( Q3ListBoxItem* item = mOptionList->findItem (opt) ) {
				int index = mOptionList->index (item);
				mOptionList -> setSelected ( index, true );
			}
		}
	}
	//====================================
	// activate tablet options [Raw]
	//------------------------------------
	if (tabletDict["RawOption"]) {
		QStringList tokens = QStringList::split (",",*tabletDict["RawOption"]);
		for ( QStringList::Iterator
			in = tokens.begin(); in != tokens.end(); ++in
		) {
			QString opt (*in);
			QString set;
			QStringList items = QStringList::split (" ",opt);
			QString key = items.first();
			QString val = items.last();
			key.replace(QRegExp("\""),"");
			val.replace(QRegExp("\""),"");
			QTextOStream (&set) << key << " >>> " << val;
			if ( Q3ListBoxItem* item = mOptionList->findItem (key) ) {
				int index = mOptionList->index (item);
				mOptionList -> changeItem  ( set, index );
				mOptionList -> setSelected ( index, true );
			}
		}
	}
	//====================================
	// save state...
	//------------------------------------
	slotOk();
}
//====================================
// slotOption
//------------------------------------
void SCCTabletPenProperty::slotOption ( Q3ListBoxItem* ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// prepare selection information 
	//-------------------------------------
	int index = mOptionList->currentItem();
	QString current = mOptionList->currentText();
	QStringList tokens = QStringList::split ( " >>> ", current );
	QString option = tokens.first();
	QString value  = tokens.last();
	QString flag   = *mOptionDict[option];
	QString message= mText["ValueRequired"];
	message.replace (
		QRegExp("\%1"),option
	);
	QString msgValue;
	QString msgResult;

	//=====================================
	// ask for an option value if needed
	//-------------------------------------
	if (mOptionList->isSelected (index)) {
		if (flag == "integer") {
			//=====================================
			// option requires an integer value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				mTabletPenDialog, mTextPtr, SaXMessage::OPT_INT,message,
				"MessageCaption", SaXMessage::Information
			);
			mMessageBox -> setSpinValue ( value.toInt() );
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				QTextOStream (&msgValue) << mMessageBox -> getSpinValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		} else if (flag == "any") {
			//=====================================
			// option requires a string value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				mTabletPenDialog, mTextPtr, SaXMessage::OPT_ANY,message,
				"MessageCaption", SaXMessage::Information
			);
			if (value != option) {
				mMessageBox -> setLineValue ( value );
			}
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				msgValue = mMessageBox -> getLineValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		} else if (flag.contains("string")) {
			//=====================================
			// option requires a predefined value
			//-------------------------------------
			QStringList tokens = QStringList::split ( "%", flag );
			QStringList values = QStringList::split ( ",", tokens.last() );
			SCCMessage* mMessageBox = new SCCMessage (
				mTabletPenDialog, mTextPtr, SaXMessage::OPT_STRING,message,
				"MessageCaption", SaXMessage::Information
			);
			mMessageBox -> setComboList ( values );
			if (value != option) {
				mMessageBox -> setComboValue ( value );
			}
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				msgValue = mMessageBox -> getComboValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		}
	}
	//=====================================
	// update option value if needed
	//-------------------------------------
	if (! msgValue.isEmpty()) {
		QString updateOption;
		QTextOStream (&updateOption) << option << " >>> " << msgValue;
		mOptionList -> changeItem  ( updateOption,index );
		mOptionList -> setSelected ( index,true );
	}
}
//====================================
// getPenMode
//------------------------------------
int SCCTabletPenProperty::getPenMode ( void ) {
	return mPenMode;
}
//====================================
// getPenOptions
//------------------------------------
Q3Dict<QString> SCCTabletPenProperty::getPenOptions ( void ) {
	return mSelectedOptions;
}
//====================================
// slotOk
//------------------------------------
void SCCTabletPenProperty::slotOk ( void ) {
	//=====================================
	// save pen mode
	//-------------------------------------
	mPenMode = 0;
	if ( mAbsolute -> isChecked() ) {
		mPenMode = 1;
	}
	//=====================================
	// save selected options
	//-------------------------------------
	mSelectedOptions.clear();
	for (unsigned int i=0;i<mOptionList->count();i++) {
		Q3ListBoxItem* item = mOptionList->item (i);
		if (item->isSelected()) {
			QStringList tokens = QStringList::split ( " >>> ", item->text() );
			QString option = tokens.first();
			QString* value = new QString (tokens.last());
			if (tokens.count() > 1) {
				mSelectedOptions.insert (option,value);
			} else {
				mSelectedOptions.insert (option,new QString);
			}
		}
	}
}
} // end namespace
