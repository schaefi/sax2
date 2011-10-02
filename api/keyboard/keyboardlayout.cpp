/**************
FILE          : keyboardlayout.cpp
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
              : - keyboardlayout.h: SaXGUI::SCCKeyboardLayout implementation
              : - keyboardlayout.cpp: configure keyboard layout system
              : ----
              :
STATUS        : Status: Development
**************/
#include "keyboardlayout.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3VBoxLayout>

#include <QStringList>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCKeyboardLayout::SCCKeyboardLayout (
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
	mPrimaryGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["KBDPrimary"],this
	);
	Q3HBox* typeBox    = new Q3HBox ( mPrimaryGroup );
	Q3HBox* layoutBox  = new Q3HBox ( mPrimaryGroup );
	Q3HBox* variantBox = new Q3HBox ( mPrimaryGroup );
	mLabelType    = new QLabel    (
		mText["KBDTypeLabel"],typeBox
	);
	mTypeBox      = new QComboBox ( FALSE,typeBox );
	mLabelLayout  = new QLabel    (
		mText["KBDLayoutLabel"],layoutBox
	);
	mLayoutBox    = new QComboBox ( FALSE,layoutBox );
	mLabelVariant = new QLabel    (
		mText["KBDVariantLabel"],variantBox
	);
	mVariantBox   = new QComboBox ( FALSE,variantBox );
	mAdditionalGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["KBDAdditional"],this
	);
	mAddLayout = new Q3ListView  ( mAdditionalGroup );
	mAddLayout -> setItemMargin ( 2 );
	mAddLayout -> setAllColumnsShowFocus (true);
	mViewStatus = mAddLayout -> addColumn ( mText["KBDStatus"] );
	mViewLayout = mAddLayout -> addColumn ( mText["KBDAddLayout"] );
	mViewKey    = mAddLayout -> addColumn ( mText["KBDKey"] );
	mViewVariant= mAddLayout -> addColumn ( mText["KBDVariant"] );
	mAddLayout -> setResizeMode  ( Q3ListView::LastColumn );
	mAddLayout -> adjustSize();
	mAdditionalGroup -> addSpace ( 10 );
	Q3HBox* addVariantBox = new Q3HBox ( mAdditionalGroup );
	mLabelAddVariant = new QLabel    (
		mText["KBDVariantLabel"],addVariantBox
	);
	mAddVariantBox   = new QComboBox ( FALSE,addVariantBox );
	QList<QLabel*> labelList;
	labelList.append ( mLabelType );
	labelList.append ( mLabelLayout );
	labelList.append ( mLabelVariant );
	labelList.append ( mLabelAddVariant );
	QFontMetrics  metrics ( font() );
	QLabel* it;
	int maxWidth = 0;
	foreach (it,labelList) {
	if (metrics.width (it->text()) > maxWidth) {
		maxWidth = metrics.width (it->text());
	}
	}
//	it.toFront();
	foreach (it,labelList) {
		it->setFixedWidth ( maxWidth + 15 );
	}
	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mLayoutBox  , SIGNAL (activated         ( int )),
		this        , SLOT   (slotLayout        ( int ))
	);
	QObject::connect (
		mTypeBox    , SIGNAL (activated         ( int )),
		this        , SLOT   (slotType          ( int ))
	);
	QObject::connect (
		mVariantBox , SIGNAL (activated         ( int )),
		this        , SLOT   (slotVariant       ( int ))
	);
	QObject::connect (
		mAddVariantBox , SIGNAL (activated      ( int )),
		this           , SLOT   (slotAddVariant ( int ))
	);
	QObject::connect(
		mAddLayout     , SIGNAL (clicked        ( Q3ListViewItem* )),
		this           , SLOT   (slotAddLayout  ( Q3ListViewItem* ))
	);
	//=====================================
	// add widgets to the layout
	//------------------------------------- 
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mPrimaryGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mAdditionalGroup );
}
//====================================
// init
//------------------------------------
void SCCKeyboardLayout::init ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// query XKB file extension
	//------------------------------------
	SaXKeyRules XKBFile;
	mModelDict  = XKBFile.getModels();
	mLayoutDict = XKBFile.getLayouts();

	//====================================
	// insert available layouts
	//------------------------------------
	QStringList layoutList;
	Q3DictIterator<QString> itLayout (mLayoutDict);
	for (; itLayout.current(); ++itLayout) {
		QString val = mText[*itLayout.current()];
		if (val.isEmpty()) {
			log (L_WARN,
				"SCCKeyboardLayout::Warning: unknown XKB key: %s\n",
				itLayout.current()->ascii()
			);
		}
		layoutList.push_back(val);
		Q3CheckListItem* item = new Q3CheckListItem (
			mAddLayout,"",Q3CheckListItem::CheckBox
		);
		item->setText ( 1, val );
		item->setText ( 2, itLayout.currentKey() );
	}
	layoutList.sort();
	mLayoutBox -> insertItems ( 0, layoutList );

	//====================================
	// insert available models
	//------------------------------------
	QStringList typeBox;
	Q3DictIterator<QString> itModel (mModelDict);
	for (; itModel.current(); ++itModel) {
		QString item = *itModel.current();
		typeBox.push_back( item );
	}
	typeBox.sort( );
	mTypeBox -> insertItems ( 0, typeBox );
	mAddLayout -> setSorting (1);
}
//====================================
// import
//------------------------------------
void SCCKeyboardLayout::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create needed manipulators
	//------------------------------------
	SaXManipulateKeyboard saxKeyboard (
		mSection["Keyboard"]
	);
	//====================================
	// get keyboard model,layout,variant
	//------------------------------------
	QList<QString> XKBLayouts  = saxKeyboard.getXKBLayout();
	QList<QString> XKBVariants = saxKeyboard.getXKBVariantList ();
	QString XKBModel = saxKeyboard.getXKBModel();

	//=====================================
	// select base keyboard model
	//-------------------------------------
	Q3DictIterator<QString> itModel (mModelDict);
	for (; itModel.current(); ++itModel) {
		if (itModel.currentKey() == XKBModel) {
			mTypeBox -> setCurrentText (*itModel.current());
		}
	}
	//=====================================
	// get layout/variant lists
	//-------------------------------------
	QString baseLayout;
	QString baseVariant;
	if (! XKBLayouts.isEmpty()) {
		foreach (baseLayout,XKBLayouts) {
			break;
		}
		XKBLayouts.removeFirst();
	}
	if (! XKBVariants.isEmpty()) {
		foreach (baseVariant,XKBVariants) {
			break;
		}
		XKBVariants.removeFirst();
	}
	int varCount = 0;
	//=====================================
	// select base/secondary layout(s)
	//-------------------------------------
	// 1) primary layout...
	Q3DictIterator<QString> itLayout (mLayoutDict);
	int found = 0;
	for (; itLayout.current(); ++itLayout) {
		if (itLayout.currentKey() == baseLayout) {
			QString val = mText[*itLayout.current()];
			if (val.isEmpty()) {
				log (L_WARN,
					"SCCKeyboardLayout::Warning: unknown XKB key: %s\n",
					itLayout.current()->ascii()
				);
			}
			mLayoutBox -> setCurrentText ( val );
			found = 1; break;
		}
	}
	if (! found) {
		itLayout.toFirst();
		baseLayout = "us";
		for (; itLayout.current(); ++itLayout) {
			if (itLayout.currentKey() == baseLayout) {
				QString val = mText[*itLayout.current()];
				if (val.isEmpty()) {
					log (L_WARN,
						"SCCKeyboardLayout::Warning: unknown XKB key: %s\n",
						itLayout.current()->ascii()
					);
				}
				mLayoutBox -> setCurrentText ( val );
			}
		}
	}
	// 2) secondary layout and variants...
	QString it;
	foreach (it,XKBLayouts) {
	Q3ListViewItemIterator itAdd (mAddLayout);
	for ( ; itAdd.current(); ++itAdd ) {
		Q3CheckListItem* item = (Q3CheckListItem*)itAdd.current();
		QString layout = itAdd.current()->text(2);
		if (layout == it) {
			item -> setOn (true);
			if (! XKBVariants.isEmpty()) {
				QString variant = XKBVariants.at (varCount);
				if ((!variant.isNull()) && (variant != "!")) {
					item -> setText ( 3 , XKBVariants.at(varCount) );
				}
			}
			mAddLayout -> setSelected (itAdd.current(), true);
			mAddLayout -> ensureItemVisible (item);
			varCount++;
		}
	}
	}
	updateVariants();

	//=====================================
	// select base variant
	//-------------------------------------
	mVariantBox -> setCurrentText (mVariantBox->text (0));
	for (int n=0;n<mVariantBox->count();n++) {
		QString item = mVariantBox->text (n);
		if (item == baseVariant) {
			mVariantBox -> setCurrentText (baseVariant);
		}
	}
	//=====================================
	// store initial apply string
	//-------------------------------------
	mDefaultLayout = getApplyString();
}
//====================================
// getDefaultLayout
//------------------------------------
QString SCCKeyboardLayout::getDefaultLayout ( void ) {
	return mDefaultLayout;
}

//====================================
// getType
//------------------------------------
QString SCCKeyboardLayout::getType ( void ) {
	Q3DictIterator<QString> itModel (mModelDict);
	for (; itModel.current(); ++itModel) {
		QString item = *itModel.current();
		if (item == mTypeBox->currentText()) {
			mXKBModel = itModel.currentKey();
		}
	}
	return mXKBModel;
}
//====================================
// getLayout
//------------------------------------
QString SCCKeyboardLayout::getLayout ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// build full qualified layout string
	//-------------------------------------
	mXKBLayout = "";
	Q3DictIterator<QString> itLayout (mLayoutDict);
	for (; itLayout.current(); ++itLayout) {
		QString val = mText[*itLayout.current()];
		if (val.isEmpty()) {
			log (L_WARN,
				"SCCKeyboardLayout::Warning: unknown XKB key: %s\n",
				itLayout.current()->ascii()
			);
		}
		if (val == mLayoutBox->currentText()) {
			mXKBLayout = itLayout.currentKey();
		}
	}
	Q3ListViewItemIterator itAdd (mAddLayout);
	for ( ; itAdd.current(); ++itAdd ) {
		Q3CheckListItem* item = (Q3CheckListItem*)itAdd.current();
		if (item->isOn()) {
			QString layout  = itAdd.current()->text(2);
			mXKBLayout.sprintf("%s,%s",
				mXKBLayout.ascii(),layout.ascii()
			);
		}
	}
	return mXKBLayout;
}
//====================================
// getVariant
//------------------------------------
QString SCCKeyboardLayout::getVariant ( void ) {
	mXKBVariant = "";
	if (!mVariantBox->currentText().isNull()) {
		mXKBVariant = mVariantBox->currentText();
	}
	Q3ListViewItemIterator itAdd (mAddLayout);
	for ( ; itAdd.current(); ++itAdd ) {
		Q3CheckListItem* item = (Q3CheckListItem*)itAdd.current();
		if (item->isOn()) {
			QString variant = "basic";
			if (!itAdd.current()->text(3).isNull()) {
				variant = itAdd.current()->text(3);
			}
			mXKBVariant.sprintf("%s,%s",
				mXKBVariant.ascii(),variant.ascii()
			);
		}
	}
	return mXKBVariant;
}
//====================================
// slotLayout
//------------------------------------
void SCCKeyboardLayout::slotLayout ( int ) {
	updateVariants();
	emit sigApply();
}
//====================================
// slotType
//------------------------------------
void SCCKeyboardLayout::slotType ( int ) {
	emit sigApply();
}
//====================================
// slotVariant
//------------------------------------
void SCCKeyboardLayout::slotVariant ( int ) {
	emit sigApply();
}
//====================================
// slotAddVariant
//------------------------------------
void SCCKeyboardLayout::slotAddVariant ( int ) {
	Q3ListViewItem* item = mAddLayout -> selectedItem();
	item -> setText (3,"");
	if (mAddVariantBox->currentText() != "basic") {
		item -> setText (3, mAddVariantBox->currentText());
		emit sigApply();
	}
}
//====================================
// slotAddLayout
//------------------------------------
void SCCKeyboardLayout::slotAddLayout ( Q3ListViewItem* ) {
	updateVariants();
	emit sigApply();
}
//====================================
// getApplyString
//------------------------------------
QString SCCKeyboardLayout::getApplyString ( void ) {
	QString XkbModel   = getType();
	QString XkbLayout  = getLayout();
	QString XkbVariant = getVariant();
	QString result;
	QTextOStream (&result)
		<< "-model " << XkbModel << " -layout " << XkbLayout;
	if (! XkbVariant.isEmpty()) {
		result.append (" -variant " + XkbVariant);
	}
	return result;
}
//====================================
// updateVariants
//------------------------------------
void SCCKeyboardLayout::updateVariants ( void ) {
	// .../
	//! this function will update the variant lists for the
	//! primary variant combobox and the additional variant
	//! combobox. Currently active variants will be resetted
	//! after the list update
	// ----
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// 1) Additional Variants...
	//------------------------------------
	mAddVariantBox -> clear();
	SaXKeyRules XKBFile;
	Q3ListViewItem* item = mAddLayout -> selectedItem();
	if (item) {
		QList<QString> list = XKBFile.getVariants(item->text (2));
		if (! list.isEmpty()) {
			mAddVariantBox -> insertStringList (translateList ( list ));
		} else {
			mAddVariantBox -> insertItem ("basic");
		}
		mAddVariantBox->setCurrentText ("basic");
		if ((!item->text(3).isNull()) && (! item->text(3).isEmpty())) {
			mAddVariantBox -> setCurrentText (item->text(3));
		}
	}
	//====================================
	// 2) Primary Variant...
	//------------------------------------
	int curItem = mVariantBox -> currentItem();
	QString curText = mVariantBox -> currentText();
	mVariantBox -> clear();
	Q3DictIterator<QString> it (mLayoutDict);
	bool emptyVariantList = true;
	for (; it.current(); ++it) {
		QString val = mText[*it.current()];
		if (val.isEmpty()) {
			log (L_WARN,
				"SCCKeyboardLayout::Warning: unknown XKB key: %s\n",
				it.current()->ascii()
			);
		}
		if (val == mLayoutBox->currentText()) {
			QList<QString> list = XKBFile.getVariants (it.currentKey());
			if (! list.isEmpty() && (list.count() > 1)) {
				mVariantBox -> insertStringList (translateList (list));
				emptyVariantList = false;
			} else {
				mVariantBox -> insertItem ("basic");
			}
			break;
		}
	}
	mVariantBox->setCurrentText ("basic");
	if (! emptyVariantList) {
	if (mVariantBox->text (curItem) == curText) {
		mVariantBox->setCurrentItem (curItem);
	}
	}
}
//====================================
// translateList
//------------------------------------
QStringList SCCKeyboardLayout::translateList ( const QList<QString>& list ) {
	QStringList result;
	QString it;
	foreach (it,list) {
		result << it;
	}
	return result;
}
} // end namespace
