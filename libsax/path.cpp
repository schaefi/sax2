/**************
FILE          : path.cpp
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
#include "path.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulatePath::SaXManipulatePath (SaXImport* in, int path) {
	// .../
	//! An object of this type is used to configure the
	//! Files,Module and ServerFlags specifications including
	//! path descriptions for Fonts loadable module settings
	//! and server flags
	// ----
	if ( ! in ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( in->getSectionID() != SAX_PATH ) {
		excPathImportBindFailed ( in->getSectionID() );
		qError (errorString(),EXC_PATHIMPORTBINDFAILED);
		return;
	}
	mImport = in;
	mPath   = path;
	mImport -> setID ( mPath );
}

//====================================
// setFontPath
//------------------------------------
void SaXManipulatePath::setFontPath (const QString& path) {
	// .../
	//! set global FontPath for this configuration. Using
	//! this function will overwrite the current FontPath setting
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "FontPath",path );
}

//====================================
// addFontPath
//------------------------------------
void SaXManipulatePath::addFontPath (const QString& path) {
	// .../
	//! add FontPath to the current list of font pathes. The
	//! comma separator is used for each item
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("FontPath");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << path;
	mImport -> setItem ( key,val );
}

//====================================
// removeFontPath
//------------------------------------
void SaXManipulatePath::removeFontPath (const QString& path) {
	// .../
	//! remove the given font path (path) from the current
	//! list of font pathes. If the path doesn't exist nothing
	//! will be changed
	// ----
	if (! mImport) {
		return;
	}
	QString val (path);
	QString key ("FontPath");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// setLoadableModule
//------------------------------------
void SaXManipulatePath::setLoadableModule (const QString& module) {
	// .../
	//! set global module to load for this configuration. Using
	//! this function will overwrite the current module list
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "ModuleLoad",module );
}

//====================================
// setDisableModule
//------------------------------------
void SaXManipulatePath::setDisableModule (const QString& module) {
	// .../
	//! set global module to be disabled for this configuration. Using
	//! this function will overwrite the current module list
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "ModuleDisable",module );
}

//====================================
// addLoadableModule
//------------------------------------
void SaXManipulatePath::addLoadableModule (const QString& module) {
	// .../
	//! add module name to the current list of modules. The
	//! comma separator is used for each item
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("ModuleLoad");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << module;
	mImport -> setItem ( key,val );
}

//====================================
// addDisableModule
//------------------------------------
void SaXManipulatePath::addDisableModule (const QString& module) {
	// .../
	//! add module name to the current list of disabled modules. The
	//! comma separator is used for each item
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("ModuleDisable");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << module;
	mImport -> setItem ( key,val );
}

//====================================
// removeLoadableModule
//------------------------------------
void SaXManipulatePath::removeLoadableModule (const QString& module) {
	// .../
	//! remove the given module (module) from the current
	//! list of modules. If the module doesn't exist nothing
	//! will be changed
	// ----
	if (! mImport) {
		return;
	}
	QString val (module);
	QString key ("ModuleLoad");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// removeDisableModule
//------------------------------------
void SaXManipulatePath::removeDisableModule (const QString& module) {
	// .../
	//! remove the given module (module) from the current
	//! list of disabled modules. If the module doesn't exist nothing
	//! will be changed
	// ----
	if (! mImport) {
		return;
	}
	QString val (module);
	QString key ("ModuleDisable");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// setServerFlag
//------------------------------------
void SaXManipulatePath::setServerFlag (const QString& flag) {
	// .../
	//! set global server flag for this configuration. Using
	//! this function will overwrite the current flag list
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "ServerFlags",flag );
}

//====================================
// addServerFlag
//------------------------------------
void SaXManipulatePath::addServerFlag (const QString& flag) {
	// .../
	//! add server flag to the current list of flags. The
	//! comma separator is used for each item
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QString key ("ServerFlags");
	if (! mImport -> getItem (key).isEmpty()) {
		val = mImport -> getItem (key);
	}
	QTextOStream (&val) << "," << flag;
	mImport -> setItem ( key,val );
}

//====================================
// removeServerFlag
//------------------------------------
void SaXManipulatePath::removeServerFlag (const QString& flag) {
	// .../
	//! remove the given flag (flag) from the current
	//! list of flags. If the flag doesn't exist nothing
	//! will be changed
	// ----
	if (! mImport) {
		return;
	}
	QString val (flag);
	QString key ("ServerFlags");
	if (! mImport -> getItem (key).isEmpty()) {
		mImport -> removeItem (key,val);
	}
}

//====================================
// getFontPaths
//------------------------------------
QList<QString> SaXManipulatePath::getFontPaths (void) {
	// .../
	//! return a list of server font paths defined for
	//! this X11 configuration
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString paths = mImport -> getItem ("FontPath");
	return createList (paths);
}

//====================================
// getModules
//------------------------------------
QList<QString> SaXManipulatePath::getModules (void) {
	// .../
	//! return a list of server modules defined for this
	//! X11 configuration
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString modules = mImport -> getItem ("ModuleLoad");
	return createList (modules);
}

//====================================
// getServerFlags 
//------------------------------------
QList<QString> SaXManipulatePath::getServerFlags (void) {
	// .../
	//! return a list of server flags defined for this
	//! X11 configuration
	// ----
	if (! mImport) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString modules = mImport -> getItem ("ServerFlags");
	return createList (modules);
}

//====================================
// transform string to Qlist
//------------------------------------
QList<QString> SaXManipulatePath::createList ( const QString& data) {
	// .../
	//! return an item list from a comma separated data string
	//! empty items will be ignored using this method
	// ----
	if (data.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result; 
	QStringList dataList = QStringList::split ( ",", data );
	for (QStringList::Iterator it=dataList.begin(); it!=dataList.end();++ it) {
		QString item(*it);
		result.append (item);
	}
	return result;
}
} // end namespace
