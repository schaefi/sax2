/**************
FILE          : export.cpp
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
#include "export.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXExport::SaXExport (SaXImport* import, SaXException* to) {
	// .../
	//! An object of this type is used to create one of
	//! the section files which build the contents of the
	//! later apidata file. 
	// ----
	if (to) {
		mTo = to;
	}
	switch (import->getSectionID()) {
		case SAX_CARD:
			mFile = ISAXCARD;
		break;
		case SAX_DESKTOP:
			mFile = ISAXDESKTOP;
		break;
		case SAX_POINTERS:
			mFile = ISAXINPUT;
		break;
		case SAX_KEYBOARD:
			mFile = ISAXKEYBOARD;
		break;
		case SAX_LAYOUT:
			mFile = ISAXLAYOUT;
		break;
		case SAX_PATH:
			mFile = ISAXPATH;
		break;
		case SAX_EXTENSIONS:
			mFile = ISAXEXTENSIONS;
		break;
		default:
			excExportSectionFailed();
			qError (errorString(),EXC_EXPORTSECTIONFAILED);
			if (mTo) {
				mTo -> excExportSectionFailed();
			}
		break;
	}
	mHandle = new QFile (mFile);
	mImport = import;
}

//====================================
// doExport...
//------------------------------------
bool SaXExport::doExport (void) {
	// .../
	//! use mImport to extract the data and write it to the
	//! previously opened file. Format used is: <ID:KEY:VALUE>
	// ----
	if (! havePrivileges()) {
		excPermissionDenied ();
		qError (errorString(),EXC_PERMISSIONDENIED);
		if (mTo) {
			mTo -> excPermissionDenied ();
		}
		return false;
	}
	if (! mHandle -> open(QIODevice::WriteOnly)) {
		excFileOpenFailed ( errno );
		qError (errorString(),EXC_FILEOPENFAILED);
		if (mTo) {
			mTo -> excFileOpenFailed ( errno );
		}
		return false;
	}
	if ((flock (mHandle->handle(),LOCK_EX)) != 0) {
		excLockSetFailed ( errno );
		qError (errorString(),EXC_LOCKSETFAILED);
		if (mTo) {
			mTo -> excLockSetFailed ( errno );
		}
		mHandle -> close();
		return false;
	}
	for (int id=0; id < mImport->getCount();id++) {
		Q3Dict<QString>* data = mImport->getTablePointer (id);
		if ( ! data ) {
			excNoStorage(id);
			qError (errorString(),EXC_NOSTORAGE);
			if (mTo) {
				mTo -> excNoStorage(id);
			}
			continue;
		}
		Q3DictIterator<QString> it (*data);
		for (; it.current(); ++it) {
			QString line;
			line.sprintf ("%d : %-20s : %s\n",
				id,it.currentKey().ascii(),it.current()->ascii()
			);
			mHandle -> writeBlock (
				line.ascii(),line.length()
			);
		}
	}
	if ((flock (mHandle->handle(),LOCK_UN)) != 0) {
		excLockUnsetFailed ( errno );
		qError (errorString(),EXC_LOCKUNSETFAILED);
		if (mTo) {
			mTo -> excLockUnsetFailed ( errno );
		}
		mHandle -> close();
		return false;
	}
	mHandle -> close();
	return true;
}
} // end namespace
