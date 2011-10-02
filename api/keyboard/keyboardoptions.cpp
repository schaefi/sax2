/**************
FILE          : keyboardoptions.cpp
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
              : - keyboardoptions.h: SaXGUI::SCCKeyboardOptions implementation
              : - keyboardoptions.cpp: configure XKB keyboard options
              : ----
              :
STATUS        : Status: Development
**************/
#include "keyboardoptions.h"
//Added by qt3to4:
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCKeyboardOptions::SCCKeyboardOptions (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	int, QWidget* parent
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
	mXKBOptionView = new Q3ListView (this);
	mXKBOptionView -> addColumn(mText["XKBOptions"]);
	mXKBOptionView -> setResizeMode (Q3ListView::LastColumn);
	mXKBOptionView -> setRootIsDecorated(false);
	mXKBOptionView -> setSorting ( -1 );

	//=====================================
	// add eurosign: tagged options
	//-------------------------------------
	mIDEuroSign = new Q3CheckListItem (
		mXKBOptionView,mText["XKBEuroSign"]
	);
	mIDEuroSignNone = new Q3CheckListItem (
		mIDEuroSign,mText["XKBNone"],Q3CheckListItem::RadioButton
	);
	//=====================================
	// add caps: tagged options
	//-------------------------------------
	mIDCapsLock = new Q3CheckListItem (
		mXKBOptionView,mText["XKBCapsLock"]
	);
	mIDCapsLockNone = new Q3CheckListItem (
		mIDCapsLock,mText["XKBNone"],Q3CheckListItem::RadioButton
	);
	//=====================================
	// add altwin: tagged options
	//-------------------------------------
	mIDAltWinKey = new Q3CheckListItem (
		mXKBOptionView,mText["XKBAltWinKey"]
	);
	mIDAltWinKeyNone = new Q3CheckListItem (
		mIDAltWinKey,mText["XKBNone"],Q3CheckListItem::RadioButton
	); 
	//=====================================
	// add lv3: tagged options
	//-------------------------------------
	mIDThirdLevel = new Q3CheckListItem (
		mXKBOptionView,mText["XKBThirdLevelChooser"],Q3CheckListItem::CheckBox
	);
	//=====================================
	// add ctrl: tagged options
	//-------------------------------------
	mIDControl = new Q3CheckListItem (
		mXKBOptionView,mText["XKBControlKey"]
	);
	mIDControlNone = new Q3CheckListItem (
		mIDControl,mText["XKBNone"],Q3CheckListItem::RadioButton
	);
	//=====================================
	// add compose: tagged options
	//-------------------------------------
	mIDCompose = new Q3CheckListItem (
		mXKBOptionView,mText["XKBComposeKey"],Q3CheckListItem::CheckBox
	);
	//=====================================
	// add grp_led: tagged options
	//-------------------------------------
	mIDKbdLedToShow = new Q3CheckListItem (
		mXKBOptionView,mText["XKBUseKbdLed"],Q3CheckListItem::CheckBox
	);
	//=====================================
	// add grp: tagged options
	//-------------------------------------	
	mIDGroupShiftLock = new Q3CheckListItem (
		mXKBOptionView,mText["XKBShiftLockGroup"],Q3CheckListItem::CheckBox
	);
	//=====================================
	// opening tree
	//-------------------------------------
	mIDGroupShiftLock -> setOpen ( true );
	mIDKbdLedToShow   -> setOpen ( true );
	mIDCompose        -> setOpen ( true );
	mIDControl        -> setOpen ( true );
	mIDThirdLevel     -> setOpen ( true );
	mIDAltWinKey      -> setOpen ( true );
	mIDCapsLock       -> setOpen ( true );
	mIDEuroSign       -> setOpen ( true );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect(
		mXKBOptionView , SIGNAL (clicked       ( Q3ListViewItem* )),
		this           , SLOT   (slotXKBOption ( Q3ListViewItem* ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin ( 15 );
	mMainLayout -> addWidget ( mXKBOptionView );
}
//====================================
// init
//------------------------------------
void SCCKeyboardOptions::init ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// query XKB file extension
	//------------------------------------
	SaXKeyRules XKBFile;
	mOptionDict = XKBFile.getOptions();

	//====================================
	// insert available XKB options
	//------------------------------------
	Q3DictIterator<QString> it (mOptionDict);
	for (; it.current(); ++it) {
		QString key = it.currentKey();
		QString val = mText[*it.current()];
		if (val.isEmpty()) {
			log (L_WARN,
				"SCCKeyboardOptions::Warning: unknown XKB key: %s\n",
				it.current()->ascii()
			);
		}
		if (key.contains("eurosign:")) {
			new Q3CheckListItem (mIDEuroSign,val,Q3CheckListItem::RadioButton);
		}
		if (key.contains("caps:")) {
			new Q3CheckListItem (mIDCapsLock,val,Q3CheckListItem::RadioButton);
		}
		if (key.contains("altwin:")) {
			new Q3CheckListItem (mIDAltWinKey,val,Q3CheckListItem::RadioButton);
		}
		if (key.contains("lv3:")) {
			new Q3CheckListItem (mIDThirdLevel,val,Q3CheckListItem::CheckBox);
		}
		if (key.contains("ctrl:")) {
			new Q3CheckListItem (mIDControl,val,Q3CheckListItem::RadioButton);
		}
		if (key.contains("compose:")) {
			new Q3CheckListItem (mIDCompose,val,Q3CheckListItem::CheckBox);
		}
		if (key.contains("grp_led:")) {
			new Q3CheckListItem (mIDKbdLedToShow,val,Q3CheckListItem::CheckBox);
		}
		if (key.contains("grp:")) {
			new Q3CheckListItem (mIDGroupShiftLock,val,Q3CheckListItem::CheckBox);
		}
	}
}
//====================================
// import
//------------------------------------
void SCCKeyboardOptions::import ( void ) {
	//====================================
	// create needed manipulators
	//------------------------------------
	SaXManipulateKeyboard saxKeyboard (
		mSection["Keyboard"]
	);
	//====================================
	// get keyboard option list
	//------------------------------------
	QList<QString> XKBOptions = saxKeyboard.getXKBOptionList();

	//====================================
	// handle options
	//------------------------------------
	QString it;
	foreach (it,XKBOptions) {
		QString XKBOptionTranslation = translateOption (it);
		Q3ListViewItemIterator io (mXKBOptionView);
		for (; io.current(); ++io ) {
			Q3CheckListItem* item = (Q3CheckListItem*)io.current();
			if (item->text() == XKBOptionTranslation) {
				item->setOn   ( true );
				slotXKBOption ( item );
				break;
			}
		}
	}
	//=====================================
	// store initial apply string
	//-------------------------------------
	mDefaultOption = getApplyString();
}
//====================================
// getDefaultOption
//------------------------------------
QString SCCKeyboardOptions::getDefaultOption ( void ) {
	return mDefaultOption;
}

//====================================
// slotXKBOption
//------------------------------------
void SCCKeyboardOptions::slotXKBOption ( Q3ListViewItem* item ) {
	if ((item == mIDCompose)        ||
		(item == mIDThirdLevel)     ||
		(item == mIDKbdLedToShow)   || 
		(item == mIDGroupShiftLock)
	) {
		updateTree ( item, true  );
	} else {
		updateTree ( item, false );
	}
	QList<QString> usedOptions;
	Q3ListViewItemIterator it (mXKBOptionView);
	for (; it.current(); ++it ) {
		Q3CheckListItem* item = (Q3CheckListItem*)it.current();
		if ((item) && (item->isOn())) {
			usedOptions.append (QString(item->text()));
		}
	}
	mXKBOption = "";
	QString io;
	foreach (io,usedOptions) {
		QString XKBOptionName = findOption (io);
		if (! XKBOptionName.isEmpty()) {
			mXKBOption.sprintf ("%s,%s",
				mXKBOption.ascii(),XKBOptionName.ascii()
			);
		}
	}
	mXKBOption.replace(QRegExp("^,"),"");
	emit sigApply();
}
//====================================
// updateTree
//------------------------------------
void SCCKeyboardOptions::updateTree ( Q3ListViewItem* item,bool rootItem ) {
	if ( rootItem ) {
		//=====================================
		// root item selected...
		//-------------------------------------
		Q3ListViewItemIterator it (mXKBOptionView);
		for (; it.current(); ++it ) {
			Q3CheckListItem* subItem = (Q3CheckListItem*)it.current();
			if ((subItem) && (subItem->parent() == item)) {
				Q3CheckListItem* baseItem = (Q3CheckListItem*)item;
				subItem->setOn (baseItem->isOn());
			}
		}
	} else {
		//=====================================
		// sub-item selected...
		//-------------------------------------
		Q3CheckListItem* parent   = (Q3CheckListItem*)item->parent();
		Q3CheckListItem* thisItem = (Q3CheckListItem*)item;
		if (thisItem -> isOn()) {
			if (! parent -> isOn()) {
				parent -> setOn ( true );
			}	
			return;
		}
		if ((parent == mIDCompose)        ||
			(parent == mIDThirdLevel)     ||
			(parent == mIDKbdLedToShow)   ||
			(parent == mIDGroupShiftLock)
		) {
			Q3ListViewItemIterator it (mXKBOptionView);
			for (; it.current(); ++it ) {
				Q3CheckListItem* subItem  = (Q3CheckListItem*)it.current();
				Q3CheckListItem* baseItem = (Q3CheckListItem*)subItem->parent();
				if ((baseItem == parent) && subItem->isOn() ) {
					return;
				}
			}
			parent -> setOn ( false );
		}
	}
}
//====================================
// findOption
//------------------------------------
QString SCCKeyboardOptions::findOption ( const QString& option ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// search option
	//-------------------------------------
	Q3DictIterator<QString> it (mOptionDict);
	QString* result = new QString("");
	for (; it.current(); ++it) {
		QString key = it.currentKey();
		if (key == "caps") {
			continue;
		}
		if (key == "altwin") {
			continue;
		}
		if (key == "lv3") {
			continue;
		}
		if (key == "ctrl") {
			continue;
		}
		if (key == "compose") {
			continue;
		}
		if (key == "grp_led") {
			continue;
		}
		if (key == "grp") {
			continue;
		}
		QString val = mText[*it.current()];
		if (val == option) {
			QTextOStream (result) << key;
			break;
		}
	}
	return *result;
}
//====================================
// translateOption
//------------------------------------
QString SCCKeyboardOptions::translateOption ( const QString& option ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// translate option
	//-------------------------------------
	QString* result = new QString("");
	if (mOptionDict[option]) {
		QTextOStream (result) << mText[*mOptionDict[option]];
	}
	return *result;
}
//====================================
// getOptions
//------------------------------------
QString SCCKeyboardOptions::getOptions ( void ) {
	return mXKBOption;
}
//====================================
// getApplyString
//------------------------------------
QString SCCKeyboardOptions::getApplyString ( void ) {
	QString XkbOptions = getOptions();
	QString result;
	if (! XkbOptions.isEmpty()) {
		QTextOStream (&result) << "-option " << XkbOptions;
	}
	return result;
}
} // end namespace
