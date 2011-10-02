/**************
FILE          : init.cpp
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
#include "init.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXInit::SaXInit ( QList<const char*> opt ) {
	// .../
	//! An object of this type is used to initialize the SaX2
	//! cache. The cache provide the current configuration suggestion
	//! which can be used to create an automatic X11 config file.
	//! If additional options are used this constructor will apply it
	// ----
	mOptions.append ((const char*)"--quiet");
	mOptions.append ((const char*)"no");
	const char* it;
	foreach (it,opt) {
		mOptions.append ( it );
	}
}

//====================================
// Constructor...
//------------------------------------
SaXInit::SaXInit ( void ) {
	// .../
	//! An object of this type is used to initialize the SaX2
	//! cache. The cache provide the current configuration suggestion
	//! which can be used to create an automatic X11 config file.
	// ----
	mOptions.append ((const char*)"--quiet");
	mOptions.append ((const char*)"no");
}

//====================================
// needInit...
//------------------------------------
bool SaXInit::needInit (void) {
	// .../
	//! check if the cache is up to date. This member function will
	//! check if the cache file exists and it will check if there is
	//! hardware which has been changed
	// ----
	if (! setLock()) {
		return false;
	}
	QFileInfo info (CACHE_CONFIG);
	if (! info.exists()) {
		unsetLock();
		return true;
	}
	unsetLock();
	return false;
}

//====================================
// ignoreProfile...
//------------------------------------
void SaXInit::ignoreProfile (void) {
	// .../
	//! ignore any automatically applied profiles during initializing
	//! this will prevent any multihead cards to become configured as
	//! multihead card
	// ----
	mOptions.append ((const char*)"-i");
}

//====================================
// setValidBusID...
//------------------------------------
void SaXInit::setValidBusID (void) {
	// .../
	//! apply the nobus profile if only one card has been
	//! detected. For single card configurations the busID
	//! should not be set
	// ----
	SaXProcessCall* proc = new SaXProcessCall ();
	QString sysp = "/usr/sbin/sysp -c | wc -l";
	proc -> addArgument ( GET_BASH );
	proc -> addArgument ( "-c" );
	proc -> addArgument ( sysp.ascii() );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return;
	}
	QString optc;
	QString data = proc->readStdout().at(0);
	int count = data.toInt();
	if (count < 2) {
		mOptions.append ((const char*)"-b");
		mOptions.append ((const char*)"nobus");
	}
}

//====================================
// setPrimaryChip...
//------------------------------------
void SaXInit::setPrimaryChip (void) {
	// .../
	//! pass the option -c to init.pl and use only the primary
	//! graphics chip for the configuration. In that case also
	//! apply the nobus profile and prevent a busID to be written
	//! on single card configuration
	// ----
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( GET_PRIMARY );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return;
	}
	QString* optc = new QString;
	QString data = proc->readStdout().at(0);
	int id = data.toInt();
	QTextOStream (optc) << id;
	mOptions.append ((const char*)"-c");
	mOptions.append ((const char*)optc->ascii());
	setValidBusID();
}

//====================================
// doInit...
//------------------------------------
void SaXInit::doInit (void) {
	// .../
	//! start the init.pl SaX2 startup sequence to create the
	//! cache which is the base for all further actions
	// ----
	if (! havePrivileges()) {
		excPermissionDenied();
		qError (errorString(),EXC_PERMISSIONDENIED);
		return;
	}
	if (setenv ("HW_UPDATE","1",1) != 0) {
		excInvalidArgument ( errno );
		qError (errorString(),EXC_INVALIDARGUMENT);
		return;
	}
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( SAX_INIT );
	const char* it;
	foreach (it,mOptions) {
		proc -> addArgument ( it );
	}
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return;
	}
}
} // end namespace
