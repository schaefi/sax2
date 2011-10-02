/**************
FILE          : monitorcard.cpp
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
              : - monitorcard.h: SaXGUI::SCCMonitor header definitions
              : - monitorcard.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitorcard.h"

//====================================
// Globals
//------------------------------------
extern SaXGUI::SCCWidgetProfile* SaXWidgetProfile;

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitorCard::SCCMonitorCard (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	const QString& cardname, int display,QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mCardTab = new Q3HBox ( this );

	//=====================================
	// prepare tabed dialog
	//-------------------------------------
	mCardDialog = new Q3TabDialog ( this,0,true );
	mCardDialog -> setCaption  ( mText["CardCaption"] );
	mCardDialog -> setOkButton ( mText["Ok"] );
	mCardDialog -> setCancelButton ( mText["Cancel"] );
	mCardDialog -> addTab ( mCardTab, cardname );

	//=====================================
	// set import flag
	//-------------------------------------
	mNeedImport = true;
	mDisplay = display;

	//=====================================
	// create macro widget
	//-------------------------------------
	mCardTab -> setMargin ( 15 );
	mOptionGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["CardOptions"],mCardTab
	);
	mOption = new Q3ListBox ( mOptionGroup );
	mOption  -> setSelectionMode ( Q3ListBox::Multi );
	mCardTab -> setSpacing ( 15 );
	mRotateGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["Rotate"],mCardTab
	);
	mCardTab -> setStretchFactor ( mOptionGroup,10 );
	mRotateNot   = new QRadioButton (
		mText["RotateNone"], mRotateGroup
	);
	mRotateLeft  = new QRadioButton (
		mText["RotateLeft"], mRotateGroup
	);
	mRotateRight = new QRadioButton (
		mText["RotateRight"],mRotateGroup
	);

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mCardDialog , SIGNAL (applyButtonPressed ( void )),
		this        , SLOT   (slotOk ( void ))
	);
	QObject::connect (
		mOption     , SIGNAL (clicked    ( Q3ListBoxItem*)),
		this        , SLOT   (slotOption ( Q3ListBoxItem*))
	);
}
//=====================================
// show tabed dialog
//-------------------------------------
void SCCMonitorCard::show (void) {
	mCardDialog->setGeometry  ( 0,0,640,480 );
	mCardDialog->move (
		getPosition (mCardDialog->width(),mCardDialog->height())
	);
	import();
	mCardDialog->show();
	mCardDialog->raise();
}
//====================================
// init
//------------------------------------
void SCCMonitorCard::init ( void ) {
	//====================================
	// create manipulators
	//------------------------------------
	SaXManipulateCard saxCard (
		mSection["Card"]
	);
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// select card
	//------------------------------------
	saxDesktop.selectDesktop ( mDisplay );
	saxCard.selectCard ( mDisplay );

	//====================================
	// get card options and fill the list
	//------------------------------------
	// create profile option dict...
	QString profile = saxDesktop.getDualHeadProfile();
	if ((! profile.isEmpty()) && (! saxDesktop.isXineramaMode())) {
		SaXImportProfile* pProfile = SaXWidgetProfile->getProfile ( profile );
		SaXImport* mImport = pProfile -> getImport ( SAX_CARD );
		if ( mImport ) {
			SaXManipulateCard saxProfileCard ( mImport );
			mProfileDriverOptions = saxProfileCard.getOptions();
		}
	}
	// fill list except dual head profile used settings...
	bool hasRotateSupport = false;
	QString driver = saxCard.getCardDriver();
	mOptDict = saxCard.getCardOptions ( driver );
	Q3DictIterator<QString> it (mOptDict);
	for (; it.current(); ++it) {
	if (! mProfileDriverOptions[it.currentKey()]) {
		mOption -> insertItem ( it.currentKey() );
		if (it.currentKey() == "Rotate") {
			hasRotateSupport = true;
		}
	}
	}
	mOption -> sort();
	if (mOption -> count() == 0) {
		mOptionGroup -> setDisabled (true);
	}
	//====================================
	// check for rotate support
	//------------------------------------
	mRotateNot -> setChecked (true);
	if ( ! hasRotateSupport ) {
		mRotateGroup -> setDisabled ( true );
	}
}
//====================================
// import
//------------------------------------
void SCCMonitorCard::import ( void ) {
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
		// handle card options
		//------------------------------------
		mSelectedOptions = saxCard.getOptions();

		//====================================
		// handle card rotate state
		//------------------------------------
		QString rotate = saxCard.getRotationDirection();
		if (rotate.isEmpty()) {
			mRotateSetup = "None";
		} else {
			mRotateSetup = rotate;
		}
		//====================================
		// import done set flag
		//------------------------------------
		mNeedImport = false;
	}
	//====================================
	// setup rotate widget
	//------------------------------------
	mRotateNot -> setChecked (true);
	if (mRotateSetup != "None") {
		if (mRotateSetup == "CW") {
			mRotateRight -> setChecked (true);
		}
		if (mRotateSetup == "CCW") {
			mRotateLeft  -> setChecked (true);
		}
	}
	//====================================
	// setup option listbox widget
	//------------------------------------
	mOption -> clear();
	Q3DictIterator<QString> io (mOptDict);
	Q3DictIterator<QString> it (mSelectedOptions);
	for (; io.current(); ++io) {
	if (! mProfileDriverOptions[io.currentKey()]) {
		mOption -> insertItem ( io.currentKey() );
	}
	}
	mOption -> sort();
	for (; it.current(); ++it) {
		QString opt;
		if ( it.current()->isEmpty()) {
			QTextOStream (&opt) << it.currentKey();
		} else {
			QTextOStream (&opt) << it.currentKey() << " >>> " << it.current();
		}
		if ( Q3ListBoxItem* item = mOption->findItem (it.currentKey()) ) {
			int index = mOption->index (item);
			mOption -> changeItem  ( opt, index );
			mOption -> setSelected ( index, true );
		}
	}
	mOption -> ensureCurrentVisible();
}
//=====================================
// getRotate
//-------------------------------------
int SCCMonitorCard::getRotate ( void ) {
	if ( mRotateNot->isChecked() ) {
		return 0;
	}
	if ( mRotateLeft->isChecked() ) {
		return 1;
	}
	if ( mRotateRight->isChecked() ) {
		return 2;
	}
	return -1;
}
//=====================================
// getOptions
//-------------------------------------
Q3Dict<QString> SCCMonitorCard::getOptions ( void ) {
	return mSelectedOptions;
}
//=====================================
// setTitle
//-------------------------------------
void SCCMonitorCard::setTitle ( const QString & title ) {
	mCardDialog -> changeTab ( mCardTab,title );
}
//=====================================
// slotOption
//-------------------------------------
void SCCMonitorCard::slotOption ( Q3ListBoxItem* boxItem ) {
	if (! boxItem) {
		return;
	}
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	
	//=====================================
	// prepare selection information 
	//-------------------------------------
	int index = mOption->currentItem();
	QString current = mOption->currentText();
	QStringList tokens = QStringList::split ( " >>> ", current );
	QString option = tokens.first();
	QString value  = tokens.last();
	QString flag   = *mOptDict[option];
	QString message= mText["ValueRequired"];
	message.replace (
		QRegExp("\%1"),option
	);
	QString msgValue;
	QString msgResult;
	//=====================================
	// ask for an option value if needed
	//-------------------------------------
	if (mOption->isSelected (index)) {
		if (flag == "integer") {
			//=====================================
			// option requires an integer value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				mCardDialog, mTextPtr, SaXMessage::OPT_INT,message,
				"MessageCaption", SaXMessage::Information
			);
			mMessageBox -> setSpinValue ( value.toInt() );
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				QTextOStream (&msgValue) << mMessageBox -> getSpinValue();
			} else {
				mOption->setSelected (index,false );
			}
		} else if ((flag == "string") || (flag == "any")) {
			//=====================================
			// option requires a string value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				mCardDialog, mTextPtr, SaXMessage::OPT_ANY,message,
				"MessageCaption", SaXMessage::Information
			);
			if (value != option) {
				mMessageBox -> setLineValue ( value );
			}
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				msgValue = mMessageBox -> getLineValue();
			} else {
				mOption->setSelected (index,false );
			}
		}
	}
	//=====================================
	// update option value if needed
	//-------------------------------------
	if (! msgValue.isEmpty()) {
		QString updateOption;
		QTextOStream (&updateOption) << option << " >>> " << msgValue;
		mOption -> changeItem  ( updateOption,index );
		mOption -> setSelected ( index,true );
	}
}
//=====================================
// slotOk
//-------------------------------------
void SCCMonitorCard::slotOk ( void ) {
	//=====================================
	// save rotation state
	//-------------------------------------
	if (mRotateNot -> isChecked()) {
		mRotateSetup = "None";
	}
	if (mRotateRight -> isChecked()) {
		mRotateSetup = "CW";
	}
	if (mRotateLeft -> isChecked()) {
		mRotateSetup = "CCW";
	}
	//=====================================
	// save options
	//-------------------------------------
	mSelectedOptions.clear();
	for (unsigned int i=0;i<mOption->count();i++) {
		Q3ListBoxItem* item = mOption->item (i);
		if (item->isSelected()) {
			QStringList tokens = QStringList::split ( " >>> ", item->text() );
			QString option = tokens.first();
			QString* value = new QString(tokens.last());
			if (tokens.count() > 1) {
				mSelectedOptions.insert (option,value);
			} else {
				mSelectedOptions.insert (option,new QString);
			}
		}
	}
}
} // end namespace
