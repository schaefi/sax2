/**************
FILE          : exception.cpp
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
#include "exception.h"
#include <QTextOStream>

namespace SaX {
//====================================
// Globals...
//------------------------------------
volatile bool DEBUG = false;

//====================================
// Functions...
//------------------------------------
void handle_error (
	const char *file, int lineno, const char *msg, const char* exc
) {
	// .../
	//! debugging function called within the library sources
	//! to know the location of the error
	// ----
	if (! DEBUG) {
		return;
	}
	flockfile ( stderr );
	fprintf ( stderr, "*** libsax: %s:%i: Qt: *** %s *** delivered\n",
		file, lineno - 1,exc
	);
	fprintf ( stderr, "*** libsax: %s:%i: error: %s:\n",
		file, lineno , msg
	);
	funlockfile ( stderr );
}

//====================================
// Constructor...
//------------------------------------
SaXException::SaXException (void) {
	// .../
	//! An object of this type is used to create a container
	//! for handling and providing error messages
	// ----
	mErrorCode   = 0;
	mErrorString = new QString (strerror (mErrorCode));
	mErrorValue  = new QString ("(null)");
}

//====================================
// Enable debugging...
//------------------------------------
void SaXException::setDebug ( bool enable ) {
	// .../
	//! Enable debugging for internal libsax messages
	// ----
	DEBUG = enable;
}

//====================================
// setErrorCode...
//------------------------------------
void SaXException::setErrorCode (int id) {
	// .../
	//! private method to set the error code and the
	//! coresponding string concering to errno.h
	// ----
	mErrorCode   = id;
	*mErrorString = "";
	QTextOStream (mErrorString) << strerror (mErrorCode);
}

//====================================
// setErrorCode...
//------------------------------------
void SaXException::setErrorCode (const char* msg, int id) {
	// .../
	//! private method to set the error code provided by <id>
	//! and the error string provided by <msg>
	// ----
	mErrorCode   = id;
	*mErrorString = "";
	QTextOStream (mErrorString) << msg;
}

//====================================
// setErrorValue...
//------------------------------------
void SaXException::setErrorValue ( int data ) {
	// .../
	//! private method to set an integer error value
	// ----
	QTextOStream (mErrorValue) << data;
}

//====================================
// setErrorValue...
//------------------------------------
void SaXException::setErrorValue ( const char* data ) {
	// .../
	//! private method to set a character error value
	// ----
	QTextOStream (mErrorValue) << data;
}

//====================================
// setErrorValue...
//------------------------------------
void SaXException::setErrorValue ( void* data ) {
	// .../
	//! private method to set a pointer error value
	// ----
	QTextOStream (mErrorValue) << data;
}

//====================================
// errorReset...
//------------------------------------
void SaXException::errorReset ( void ) {
	// .../
	//! reset all error items including the error code the
	//! error message and the error value
	// ----
	mErrorCode   = 0;
	*mErrorString = "";
	QTextOStream (mErrorString) << strerror (mErrorCode);
	QTextOStream (mErrorValue)  << "(null)";
}

//====================================
// get last error code
//------------------------------------
int SaXException::errorCode ( void ) {
	// .../
	//! obtain currently set error code, on success 0
	//! is returned
	// ----
	return mErrorCode;
}

//====================================
// get last error string
//------------------------------------
QString SaXException::errorString ( void ) {
	// .../
	//! obtain currently set error message, on success
	//! the string "Success" is returned
	// ----
	return QString::fromLocal8Bit(*mErrorString);
}

//====================================
// get last error value
//------------------------------------
QString SaXException::errorValue ( void ) {
	// .../
	//! obtain currently set error value, if no value
	//! was defined the string "(null)" is returned
	// ----
	if (mErrorValue->isEmpty()) {
		return "(null)";
	}
	return QString::fromLocal8Bit(*mErrorValue);
}

//====================================
// havePrivileges...
//------------------------------------
bool SaXException::havePrivileges (void) {
	// .../
	//! check for root privileges, on success true
	//! is returned
	// ----
	if (! getuid() == 0) {
		return false;
	}
	return true;
}

//====================================
// setLock...
//------------------------------------
bool SaXException::setLock ( void ) {
	// .../
	//! Apply an advisory lock on the open file specified by mFD
	//! for this call /dev/zero is used as file reference
	// ----
	mLFD = open (ZERO_DEV,O_RDONLY);
	if ((flock (mLFD,LOCK_EX)) != 0) {
		excLockSetFailed (errno);
		qError (errorString(),EXC_LOCKSETFAILED);
		return false;
	}
	return true;
}

//====================================
// unsetLock...
//------------------------------------
bool SaXException::unsetLock ( void ) {
	// .../
	//! Remove an advisory lock on the open file specified by mFD
	//! for this call /dev/zero is used as file reference
	// ----
	if ((flock (mLFD,LOCK_UN)) != 0) {
		excLockUnsetFailed (errno);
		qError (errorString(),EXC_LOCKUNSETFAILED);
		return false;
	}
	close (mLFD);
	return true;
}


//====================================
// Exceptions...
//------------------------------------
//! process did not start
void SaXException::excProcessFailed (void) {
	setErrorCode (EAGAIN);
	emit saxGlobalException (EXC_PROCESSFAILED);
	emit saxProcessFailed();
}

//! import cannot initialize section option
void SaXException::excImportSectionFailed (void) {
	setErrorCode (EISAXIMPORT,EISAXIMPORTID);
	emit saxGlobalException (EXC_IMPORTSECTIONFAILED);
	emit saxImportSectionFailed();
}

//! export cannot initialize section file
void SaXException::excExportSectionFailed (void) {
	setErrorCode (EISAXEXPORT,EISAXEXPORTID);
	emit saxGlobalException (EXC_EXPORTSECTIONFAILED);
	emit saxExportSectionFailed();
}

//! file open failed
void SaXException::excFileOpenFailed (int e) {
	setErrorCode (ENOENT);
	setErrorValue (e);
	emit saxGlobalException (EXC_FILEOPENFAILED);
	emit saxFileOpenFailed (e);
}

//! there is no data in the storage for ID X
void SaXException::excNoStorage (int id) {
	setErrorCode (ENODATA);
	setErrorValue (id);
	emit saxGlobalException (EXC_NOSTORAGE);
	emit saxNoStorage (id);
}

//! CDB file does not exist
void SaXException::excCDBFileFailed (void) {
	setErrorCode (ENOENT);
	emit saxGlobalException (EXC_CDBFILEFAILED);
	emit saxCDBFileFailed ();
}

//! root privileges required
void SaXException::excPermissionDenied (void) {
	setErrorCode (EACCES);
	emit saxGlobalException (EXC_PERMISSIONDENIED);
	emit saxPermissionDenied ();
}

//! profile file not found
void SaXException::excProfileNotFound (void) {
	setErrorCode (ENOENT);
	emit saxGlobalException (EXC_PROFILENOTFOUND);
	emit saxProfileNotFound ();
}

//! profile does not include the requested section
void SaXException::excProfileUndefined (int id) {
	setErrorCode (EPROFILE,EPROFILEID);
	setErrorValue (id);
	emit saxGlobalException (EXC_PROFILEUNDEFINED);
	emit saxProfileUndefined (id);
}

//! a SaXImport object of this type was already added
void SaXException::excImportAlreadyAdded (int id) {
	setErrorCode (EALREADY);
	setErrorValue (id);
	emit saxGlobalException (EXC_IMPORTALREADYADDED);
	emit saxImportAlreadyAdded (id);
}

//! the section ID of this SaXImport object is unknown
void SaXException::excUnknownImport (SaXImport* in) {
	setErrorCode (EBADF);
	setErrorValue ((void*)in);
	emit saxGlobalException (EXC_UNKNOWNIMPORT);
	emit saxUnknownImport (in);
}

//! the /var/lib/sax/apidata file does not exist
void SaXException::excNoAPIFileFound (void) {
	setErrorCode (ENOENT);
	emit saxGlobalException (EXC_NOAPIFILEFOUND);
	emit saxNoAPIFileFound ();
}

//! a NULL pointer was used as argument
void SaXException::excNullPointerArgument (void) {
	setErrorCode (EINVAL);
	emit saxGlobalException (EXC_NULLPOINTERARGUMENT);
	emit saxNullPointerArgument ();
}

//! the wrong import object was used for this manipulator (desktop)
void SaXException::excDesktopImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_DESKTOPIMPORTBINDFAILED);
	emit saxDesktopImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (card)
void SaXException::excCardImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_CARDIMPORTBINDFAILED);
	emit saxCardImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (pointers)
void SaXException::excPointerImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_POINTERIMPORTBINDFAILED);
	emit saxPointerImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (keyboard)
void SaXException::excKeyboardImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_KEYBOARDIMPORTBINDFAILED);
	emit saxKeyboardImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (path)
void SaXException::excPathImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_PATHIMPORTBINDFAILED);
	emit saxPathImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (extensions)
void SaXException::excExtensionsImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_EXTENSIONSIMPORTBINDFAILED);
	emit saxExtensionsImportBindFailed (id);
}

//! the wrong import object was used for this manipulator (layout)
void SaXException::excLayoutImportBindFailed (int id) {
	setErrorCode (EBADF);
	setErrorValue (id);
	emit saxGlobalException (EXC_LAYOUTIMPORTBINDFAILED);
	emit saxLayoutImportBindFailed (id);
}

//! the /usr/lib/X11/xkb/rules/(rule) file couldn't be opened
void SaXException::excXKBLoadRulesFailed (void) {
	setErrorCode (ENOENT);
	emit saxGlobalException (EXC_XKBLOADRULESFAILED);
	emit saxXKBLoadRulesFailed ();
}

//! the group search for a CDB data record has failed
void SaXException::excCDBRecordNotFound (const char* group) {
	setErrorCode (ECDBGROUP,ECDBGROUPID);
	setErrorValue (group);
	emit saxGlobalException (EXC_CDBRECORDNOTFOUND);
	emit saxCDBRecordNotFound (group);
}

//! the given InputFashion type is invalid refering to pointers.h
void SaXException::excWrongInputFashion (const char* fashion) {
	setErrorCode (EFASHION,EFASHIONID);
	setErrorValue (fashion);
	emit saxGlobalException (EXC_WRONGINPUTFASHION);
	emit saxWrongInputFashion (fashion);
}

//! setID could not find a record with ID (id)
void SaXException::excSetStorageIDFailed (int id) {
	setErrorCode (ERECORD,ERECORDID);
	setErrorValue (id);
	emit saxGlobalException (EXC_SETSTORAGEIDFAILED);
	emit saxSetStorageIDFailed (id);
}

//! wrong input fashion type, expected SAX_INPUT_PEN or SAX_INPUT_ERASER
void SaXException::excPointerFashionTypeFailed (const char* fashion) {
	setErrorCode (EFASHION,EFASHIONID);
	setErrorValue (fashion);
	emit saxGlobalException (EXC_POINTERFASHIONTYPEFAILED);
	emit saxPointerFashionTypeFailed (fashion);
}

//! wrong argument set for member call
void SaXException::excInvalidArgument (int id) {
	setErrorCode (EINVAL);
	setErrorValue (id);
	emit saxGlobalException (EXC_INVALIDARGUMENT);
	emit saxInvalidArgument (id);
}

//! wrong argument set for member call
void SaXException::excInvalidArgument (const char* arg) {
	setErrorCode (EINVAL);
	setErrorValue (arg);
	emit saxGlobalException (EXC_INVALIDARGUMENT);
	emit saxInvalidArgument (arg);
}

//! the call to flock LOCK_EX failed
void SaXException::excLockSetFailed (int error) {
	setErrorCode (error);
	emit saxGlobalException (EXC_LOCKSETFAILED);
	emit saxLockSetFailed (error);
}

//! the call to flock LOCK_UN failed
void SaXException::excLockUnsetFailed (int error) {
	setErrorCode (error);
	emit saxGlobalException (EXC_LOCKUNSETFAILED);
	emit saxLockUnsetFailed (error);
}

//! empty screen definition in layout section for screen ID (id)
void SaXException::excGetScreenLayoutFailed (int id) {
	setErrorCode (ESCREEN,ESCREENID);
	setErrorValue (id);
	emit saxGlobalException (EXC_GETSCREENLAYOUTFAILED);
	emit saxGetScreenLayoutFailed (id);
}

//! empty InputDevice definition in layout section
void SaXException::excGetInputLayoutFailed (void) {
	setErrorCode (ENODEV);
	emit saxGlobalException (EXC_GETINPUTLAYOUTFAILED);
	emit saxGetInputLayoutFailed();
}

//! No CDB record found for cardName
void SaXException::excEmptyCDBGroup (const char* name) {
	setErrorCode (ECDBDATA,ECDBDATAID);
	setErrorValue (name);
	emit saxGlobalException (EXC_EMPTYCDBGROUP);
	emit saxEmptyCDBGroup (name);
}

//! The binary NVIDIA driver is missing
void SaXException::excNvidiaDriverMissing ( void ) {
	setErrorCode (ENVIDIAMIS,ENVIDIAMISID);
	emit saxGlobalException (EXC_NVIDIADRIVERMISSING);
	emit saxNvidiaDriverMissing();
}

//! The binary NVIDIA driver is installed
void SaXException::excNvidiaDriverInstalled ( void ) {
	setErrorCode (ENVIDIAINS,ENVIDIAINSID);
	emit saxGlobalException (EXC_NVIDIADRIVERINSTALLED);
	emit saxNvidiaDriverInstalled();
}

//! layout not defined/used -> variant cannot be set
void SaXException::excXKBLayoutUndefined ( const char* layout ) {
	setErrorCode (EXKBLAYOUT,EXKBLAYOUTID);
	emit saxGlobalException (EXC_XKBLAYOUTUNDEFINED);
	emit saxXKBLayoutUndefined (layout);
}

//! 2D/3D driver from CDB doesn't match current driver
void SaXException::excDriverMismatch ( const char* cdb,const char* cur) {
	setErrorCode (ECDBMISMATCH,ECDBMISMATCHID);
	setErrorValue ((QString(cdb)+" -> "+QString(cur)).latin1());
	emit saxGlobalException (EXC_DRIVERMISMATCH);
	emit saxDriverMismatch (cdb,cur);
}
} // end namespace
