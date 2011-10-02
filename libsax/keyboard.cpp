/**************
FILE          : keyboard.cpp
***************
PROJECT       : SaX2 - library interface
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : SaX2 - SuSE advanced X11 configuration 
              : 
              :
DESCRIPTION   : native C++ class library to access SaX2
              : functionality. Easy to use interface for
              : //.../
              : - importing/exporting X11 configurations
              : - modifying/creating X11 configurations 
              : ---
              :
              :
STATUS        : Status: Development
**************/
#include "keyboard.h"
#if QT_VERSION > 0x040100
#include <Q3TextStream>
#else
typedef QTextStream Q3TextStream;
#endif

namespace SaX {
//====================================
// Constructor
//------------------------------------
SaXKeyRules::SaXKeyRules (QString rule): mLayouts(80) {
	// .../
	//! An object of this type is used to access the xkbfile
	//! library to be able to know about the possible XKB
	//! map settings
	// ----
	mX11Dir = "/usr/lib/X11/";
	if (! access ("/usr/share/X11/xkb",R_OK|X_OK)) {
		mX11Dir = "/usr/share/X11/";
	}
	loadRules (mX11Dir + QString("xkb/rules/%1").arg(rule));
}

//====================================
// loadRules
//------------------------------------
void SaXKeyRules::loadRules(QString file) {
	// .../
	//! Load the models, layouts and options from the
	//! given XKB rules file
	// ----
	static struct {
		const char * locale;
		const char * layout;
	} fixedLayouts[] = {
		{ "ben", "Bengali" },
		{ "ar" , "Arabic"  },
		{ "ir" , "Farsi"   },
		{ 0    , 0 }
	};
	char* locale = setlocale (LC_ALL,NULL);
	XkbRF_RulesPtr rules = XkbRF_Load (
		QFile::encodeName(file).data(),
		locale, true, true
	);
	if (! rules) {
		excXKBLoadRulesFailed();
		qError (errorString(),EXC_XKBLOADRULESFAILED);
		return;
	}
	int i;
	for (i = 0; i < rules->models.num_desc; ++i) {
		QString* item = new QString (rules->models.desc[i].desc);
		if ( item->length() > 50 ) {
			item->truncate ( 50 ); item->append ("...");
		}
		mModels.replace (
			rules->models.desc[i].name,item
		);
	}
	for (i = 0; i < rules->layouts.num_desc; ++i) {
		mLayouts.replace (
			rules->layouts.desc[i].name,
			new QString (rules->layouts.desc[i].desc)
		);
	}
	for (i = 0; i < rules->options.num_desc; ++i) {
		mOptions.replace (
			rules->options.desc[i].name,
			new QString (rules->options.desc[i].desc)
		);
	}
	XkbRF_Free(rules, true);
	// ...
	// Knowing which variants we have won't help us
	// We want to know which particular variants are available for
	// each language
	// ---
	for(int i=0; fixedLayouts[i].layout != 0; i++ ) {
		mLayouts.replace (
			fixedLayouts[i].locale, new QString (fixedLayouts[i].layout)
		);
	}
}

//====================================
// getVariants
//------------------------------------
QList<QString> SaXKeyRules::getVariants (const QString& layout) {
	// .../
	//! obtain a list of all variants belonging to the
	//! given baselayout.
	// ----
	if ( layout.isEmpty() || ! getLayouts().find(layout) ) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result1;
	QStringList* r1 = mVarLists[layout];
	if ( r1 ) {
		for ( QStringList::Iterator it=r1->begin(); it != r1->end(); ++it ) {
			result1.append (*it);
		}
		return result1;
	}
	QList<QString> result2;
	QStringList* result = new QStringList();
	QString file = mX11Dir + "xkb/symbols/" + layout;
	QFile f(file);
	if (f.open(QIODevice::ReadOnly)) {
		Q3TextStream ts(&f);
		QString line;
		QString prev_line;
		while (!ts.atEnd()) {
			prev_line = line;
			line = ts.readLine().simplifyWhiteSpace();
			if (line[0] == '#' || line.left(2) == "//" || line.isEmpty()) {
				continue;
			}
			int pos = line.find("xkb_symbols");
			if (pos < 0) {
				continue;
			}
			if ( prev_line.find("hidden") >=0 ) {
				continue;
			}
			pos = line.find('"', pos) + 1;
			int pos2 = line.find('"', pos);
			if ( pos < 0 || pos2 < 0 ) {
				continue;
			}
			result->append (
				line.mid(pos, pos2-pos)
			);
		}
		f.close();
	}
	mVarLists.insert(layout, result);
	for ( QStringList::Iterator it=result->begin(); it != result->end();++it) {
		result2.append (*it);
	}
	return result2;
}

//====================================
// getModels
//------------------------------------
Q3Dict<QString> SaXKeyRules::getModels  (void) {
	return mModels;
}

//====================================
// getLayouts
//------------------------------------
Q3Dict<QString> SaXKeyRules::getLayouts (void) {
	return mLayouts;
}

//====================================
// getOptions
//------------------------------------
Q3Dict<QString> SaXKeyRules::getOptions (void) {
	return mOptions;
}

//====================================
// Constructor...
//------------------------------------
SaXManipulateKeyboard::SaXManipulateKeyboard (
	SaXImport* in, int kbd
) : SaXManipulateKeyboardIF () {
	// .../
	//! An object of this type is used to manipulate the
	//! keyboard settings refering to the X11 InputDevice sections.
	//! In contrast to pointer devices the keyboards are enumerated
	//! as 0,2,4,6,etc
	// ----
	if ( ! in ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( in->getSectionID() != SAX_KEYBOARD ) {
		excKeyboardImportBindFailed ( in->getSectionID() );
		qError (errorString(),EXC_KEYBOARDIMPORTBINDFAILED);
		return;
	}
	mImport = in;
	mKeyboard = kbd;
	mImport -> setID ( mKeyboard );
}

//====================================
// set keyboard ID
//------------------------------------
bool SaXManipulateKeyboard::selectKeyboard (int ptr) {
	// .../
	//! select the keyboard device the changes should be applied to
	//! if the device does not exist (false) is returned
	// ----
	if (! mImport) {
		return false;
	}
	if (mImport -> setID ( ptr )) {
		mKeyboard = ptr;
		return true;
	}
	return false;
}

//====================================
// setXKBModel
//------------------------------------
void SaXManipulateKeyboard::setXKBModel (const QString& model) {
	// .../
	//! set global keyboard model to define the basic
	//! keyboard hardware type
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "XkbModel",model );
}

//====================================
// setXKBLayout
//------------------------------------
void SaXManipulateKeyboard::setXKBLayout (const QString& layout) {
	// .../
	//! set the basic keyboard layout. This method will
	//! overwrite the current layout setting
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "XkbLayout",layout );
}

//====================================
// addXKBLayout
//------------------------------------
void SaXManipulateKeyboard::addXKBLayout (const QString& layout) {
	// .../
	//! add a keyboard layout to the basic layout to be
	//! able to switch between different layouts. The first
	//! layout in the list defines the basic layout
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("XkbLayout");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << layout;
	mImport -> setItem ( key,val );
}

//====================================
// removeXKBLayout
//------------------------------------
void SaXManipulateKeyboard::removeXKBLayout (const QString& layout) {
	// .../
	//! remove the given layout (layout) from the current
	//! list of keyboard layouts
	// ----
	if (! mImport) {
		return;
	}
	QString val (layout);
	QString key ("XkbLayout");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// setXKBOption
//------------------------------------
void SaXManipulateKeyboard::setXKBOption (const QString& option) {
	// .../
	//! set keyboard option. This will overwrite the current
	//! options list used for all keyboards
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "XkbOptions",option );
}

//====================================
// addXKBOption
//------------------------------------
void SaXManipulateKeyboard::addXKBOption (const QString& option) {
	// .../
	//! add a keyboard option to the current list of
	//! keyboard options
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("XkbOptions");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << option;
	mImport -> setItem ( key,val );
}

//====================================
// removeXKBOption
//------------------------------------
void SaXManipulateKeyboard::removeXKBOption (const QString& option) {
	// .../
	//! remove a keyboard option. If the option (option)
	//! does not exist nothing will happen
	// ----
	if (! mImport) {
		return;
	}
	QString val (option);
	QString key ("XkbOptions");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// setXKBVariant
//------------------------------------
void SaXManipulateKeyboard::setXKBVariant (
	const QString& layout, const QString& variant
) {
	// .../
	//! set a variant for the given keyboard layout. If the layout
	//! has not been set an exception will be thrown.
	// ----
	if (! mImport) {
		return;
	}
	int layoutCount = findLayout (layout);
	if (layoutCount < 0) {
		excXKBLayoutUndefined (layout);
		qError (errorString(),EXC_XKBLAYOUTUNDEFINED);
		return;
	}
	QList<QString> vList = getXKBVariantList();
	QList<QString> lList = getXKBLayout();
	QStringList result;
	QString it;
	for (int varCount = 0; varCount < lList.count(); varCount++) {
		QString item;
		if (!vList.value(varCount).isNull()) {
			item = vList.value(varCount);
		}
		if (varCount == layoutCount) {
			item = variant;
		}
		result += item;
	}
	mImport -> setItem ("XkbVariant",result.join(","));
}

//====================================
// removeXKBVariant
//------------------------------------
void SaXManipulateKeyboard::removeXKBVariant ( const QString& layout ) {
	// .../
	//! remove the variant set for the layout (layout). This
	//! method simply call setXKBVariant() with an empty variant
	// ----
	setXKBVariant (layout,"");
}

//====================================
// setMapping
//------------------------------------
void SaXManipulateKeyboard::setMapping (
	const QString& type,const QString& mapping
) {
	// .../
	//! set mapping for the special keys Left/Right-Alt Scroll-Lock
	//! and Right Ctrl key. If wrong type or mappings are set an
	//! invalid argument exception will be thrown
	// ----
	if (! mImport) {
		return;
	}
	QString key (type);
	QString map (mapping);
	if (
		(key != XKB_LEFT_ALT)       &&
		(key != XKB_RIGHT_ALT)      &&
		(key != XKB_SCROLL_LOCK)    &&
		(key != XKB_RIGHT_CTL)
	) {
		excInvalidArgument (type.latin1());
		qError (errorString(),EXC_INVALIDARGUMENT);
		return;
	}
	if (
		(map != XKB_MAP_META)       &&
		(map != XKB_MAP_COMPOSE)    &&
		(map != XKB_MAP_MODESHIFT)  &&
		(map != XKB_MAP_MODELOCK)   &&
		(map != XKB_MAP_SCROLLLOCK) &&
		(map != XKB_MAP_CONTROL)
	) {
		excInvalidArgument (mapping.latin1());
		qError (errorString(),EXC_INVALIDARGUMENT);
		return;
	}
	mImport -> setItem ( type,mapping );
}

//====================================
// getDriver
//------------------------------------
QString SaXManipulateKeyboard::getDriver (void) {
	// .../
	//! return the driver used for this keyboard device
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Driver");
}

//====================================
// getXKBOptionList
//------------------------------------
QList<QString> SaXManipulateKeyboard::getXKBOptionList (void) {
	// .../
	//! return a list of XKB options set for the core keyboard
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString options = mImport -> getItem ("XkbOptions");
	return createList (options);
}

//====================================
// getXKBLayout
//------------------------------------
QList<QString> SaXManipulateKeyboard::getXKBLayout (void) {
	// .../
	//! return a list of XKB layouts set for the core keyboard
	//! the first element of the list os the core layout
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString layouts = mImport -> getItem ("XkbLayout");
	return createList (layouts);
}

//====================================
// getXKBVariantList
//------------------------------------
QList<QString> SaXManipulateKeyboard::getXKBVariantList (void) {
	// .../
	//! return a list of variants following the list of
	//! defined keyboard layouts
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString variants = mImport -> getItem ("XkbVariant");
	return createList (variants);
}

//====================================
// getXKBVariant for layout X
//------------------------------------
QString SaXManipulateKeyboard::getXKBVariant ( const QString& layout ) {
	// .../
	//! return the variant defined for the given layout (layout)
	//! if there is no variant defined for the layout an empty string
	//! is returned. if the layout doesn't exist and exception will
	//! be throwed and an empty string is returned
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	int layoutCount = findLayout (layout);
	if (layoutCount < 0) {
		excXKBLayoutUndefined (layout);
		qError (errorString(),EXC_XKBLAYOUTUNDEFINED);
		QString* nope = new QString;
		return *nope;
	}
	int varCount = 0;
	QList<QString> vList = getXKBVariantList();
	QString it;
	foreach (it,vList) {
		if (varCount == layoutCount) {
			return it;
		}
		varCount++;
	}
	QString* nope = new QString;
	return *nope;
}

//====================================
// getXKBModel
//------------------------------------
QString SaXManipulateKeyboard::getXKBModel (void) {
	// .../
	//! return the keyboard model name used for this
	//! keyboard. Only one model can be defined and active
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("XkbModel");
}

//====================================
// findLayout
//------------------------------------
int SaXManipulateKeyboard::findLayout (const QString& layout) {
	// .../
	//! find the position of the given layout (layout) in the
	//! current layout list and return the position or -1 if the
	//! layout does not exist
	// ----
	int count  = 0;
	bool found = false;
	QString layoutData = mImport -> getItem ("XkbLayout");
	QStringList layouts = QStringList::split ( ",", layoutData );
	for (QStringList::Iterator it=layouts.begin(); it!=layouts.end();++ it) {
		QString item (*it);
		if (item == layout) {
			found = true; break;
		}
		count++;
	}
	if (! found) {
		return -1;
	}
	return count;
}

//====================================
// transform string to Qlist
//------------------------------------
QList<QString> SaXManipulateKeyboard::createList ( const QString& data) {
	// .../
	//! return an item list from a comma separated data string
	//! empty items (,,) are allowed using this method
	// ----
	if (data.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result; 
	QStringList dataList = QStringList::split ( ",", data, true );
	for (QStringList::Iterator it=dataList.begin(); it!=dataList.end();++ it) {
		result.append (*it);
	}
	return result;
}
} // end namespace
