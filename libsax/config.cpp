/**************
FILE          : config.cpp
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
#include "config.h"
#if QT_VERSION > 0x040100
#include <Q3TextStream>
#else
typedef QTextStream Q3TextStream;
#endif

namespace SaX {
//====================================
// Globals...
//------------------------------------
SaXConfig* self = NULL;

//====================================
// Constructor...
//------------------------------------
SaXConfig::SaXConfig (int mode) {
	// .../
	//! An object of this type is used to create and verify
	//! the basic X11 configuration file /etc/X11/xorg.conf
	// ----
	configMode = mode;
	gotCard       = false;
    gotDesktop    = false;
    gotPointers   = false;
    gotKeyboard   = false;
    gotLayout     = false;
    gotPath       = false;
    gotExtensions = false;
	mDesktop      = NULL;
	self = this;
	// .../
	// remove all ISaX output files building apidata
	// ----
	if ( setLock() ) {
		unlink (ISAXCARD);
		unlink (ISAXDESKTOP);
		unlink (ISAXINPUT);
		unlink (ISAXKEYBOARD);
		unlink (ISAXLAYOUT);
		unlink (ISAXPATH);
		unlink (ISAXEXTENSIONS);
		unsetLock();
	}
	mParseErrorString = new QString();
	mParseErrorValue  = new QString();
}

//====================================
// addImport...
//------------------------------------
void SaXConfig::addImport ( SaXImport* in ) {
	// .../
	//! add previously imported data using a SaXImport reference
	//! multiple addImport() calls are possible to add all
	//! needed sections
	// ----
	int ID = in->getSectionID();
	switch (ID) {
		case SAX_CARD:
		if (gotCard) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotCard = true;
		break;
		case SAX_DESKTOP:
		if (gotDesktop) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotDesktop = true;
		mDesktop   = in;
		break;
		case SAX_POINTERS:
		if (gotPointers) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotPointers = true;
		break;
		case SAX_KEYBOARD:
		if (gotKeyboard) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotKeyboard = true;
		break;
		case SAX_LAYOUT:
		if (gotLayout) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotLayout = true;
		break;
		case SAX_PATH:
		if (gotPath) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotPath = true;
		break;
		case SAX_EXTENSIONS:
		if (gotExtensions) {
			excImportAlreadyAdded (ID);
			qError (errorString(),EXC_IMPORTALREADYADDED);
			return;
		}
		gotExtensions = true;
		break;
		default:
			excUnknownImport (in);
			qError (errorString(),EXC_UNKNOWNIMPORT);
			return;
		break;
	}
	mImportList.append (in);
}

//====================================
// enableXFineCache...
//------------------------------------
void SaXConfig::enableXFineCache (bool enable) {
	// .../
	//! private member function to enable the XFine cache.
	//! the cache is created when xfine is running. xfine is
	//! started while testing the new configuration. If the test
	//! has been finished using the "Save" button the XFine cache
	//! will be imported to incorp the modeline changes
	// ----
	if (! mDesktop) {
		return;
	}
	mDesktop -> setID (0);
	if (enable) {
		mDesktop -> setItem ("ImportXFineCache","yes");
	} else {
		mDesktop -> removeEntry ("ImportXFineCache");
	}
}

//====================================
// setMode...
//------------------------------------
void SaXConfig::setMode (int mode) {
	// .../
	//! set the ISAX mode for creating the configuration
	//! file. Possible modes are: SAX_NEW or SAX_MERGE (default).
	//! If SAX_NEW is specified a complete set of imports must
	//! be added to create a valid configuration file.
	// ----
	configMode = mode;
}

//====================================
// createConfiguration...
//------------------------------------
bool SaXConfig::createConfiguration (void) {
	// .../
	//! create the preliminary configuration file and verify the
	//! syntax of the file. If everything is ok the method will
	//! return true otherwise false
	// ----
	if (! setLock()) {
		return false;
	}
	SaXImport* it;
	foreach (it,mImportList) {
		SaXExport exportconfig (it,this);
		if (! exportconfig.doExport()) {
			//excExportSectionFailed ();
			//qError (errorString(),EXC_EXPORTSECTIONFAILED);
			unsetLock();
			return false;
		}
	}
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( SAX_CREATE_API );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
	}
	QFileInfo api (SAX_API_FILE);
	if (! api.exists()) {
		excNoAPIFileFound ();
		qError (errorString(),EXC_NOAPIFILEFOUND);
		unsetLock();
		return false;
	}
	proc -> clearArguments ();
	proc -> addArgument ( ISAX );
	proc -> addArgument ( "-f" );
	proc -> addArgument ( SAX_API_FILE );
	proc -> addArgument ( "-c" );
	proc -> addArgument ( SAX_API_CONFIG );
	
	if (configMode == SAX_MERGE) {
		proc -> addArgument ( "-m" );
	}
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
	}
	if (! xf86openConfigFile (CONFPATH,SAX_API_CONFIG,0)) {
		excFileOpenFailed (0);
		qError (errorString(),EXC_FILEOPENFAILED);
		unsetLock();
		return false;
	}
	if (! xf86readConfigFile()) {
		unsetLock();
		return false;
	} else {
		xf86closeConfigFile();
	}
	removeXFineCache();
	unsetLock();
	return true;
}

//====================================
// commitConfiguration...
//------------------------------------
void SaXConfig::commitConfiguration (void) {
	// .../
	//! install the configuration file as /etc/X11/xorg.conf
	//! installing the file should only be done when
	//! createConfiguration() returned succesfully
	// ----
	QFile apiConfig (SAX_API_CONFIG);
	QFile curConfig (SAX_SYS_CONFIG);
	QFile secConfig (SAX_SYS_CSAVED);
	QFile apiMD5 (SAX_API_MD5);
	QFile curMD5 (SAX_SYS_MD5);

	//====================================
	// read api created config file
	//------------------------------------
	QStringList al;
	if ( apiConfig.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &apiConfig );
		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine();
			al += line;
		}
		apiConfig.close();
	} else {
		return;
	}
	//====================================
	// read api created MD5 sum
	//------------------------------------
	QStringList ml;
	if ( apiMD5.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &apiMD5 );
		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine();
			ml += line;
		}
		apiMD5.close();
	} else {
		return;
	}
	//====================================
	// read current config file
	//------------------------------------
	QStringList cl;
	if ( curConfig.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &curConfig );
		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine();
			cl += line;
		}
		curConfig.close();
	
		//====================================
		// create a backup copy
		//------------------------------------
		if ( secConfig.open( QIODevice::WriteOnly ) ) {
			Q3TextStream stream ( &secConfig );
			for (QStringList::Iterator it = cl.begin(); it != cl.end();++it) {
				stream << *it << "\n";
			}
			secConfig.close();
		}
	}
	//====================================
	// install to system
	//------------------------------------
	if ( curConfig.open( QIODevice::WriteOnly ) ) {
		Q3TextStream stream ( &curConfig );
		for (QStringList::Iterator it = al.begin(); it != al.end();++it) {
			stream << *it << "\n";
		}
		curConfig.close();
	}
	//====================================
	// install MD5 sum to system
	//------------------------------------
	if ( curMD5.open( QIODevice::WriteOnly ) ) {
		Q3TextStream stream ( &curMD5 );
		for (QStringList::Iterator it = ml.begin(); it != ml.end();++it) {
			stream << *it << "\n";
		}
		curMD5.close();
	}
	//====================================
	// create symbolic loader links
	//------------------------------------
	// The links are only created if the
	// /usr/X11/bin/Xorg server exists, for xorg v7 the server
	// is located in /usr/bin and doesn't need to be linked
	// ----
	QFile loader (SAX_X11_LOADER);
	if ((loader.exists()) && (access ("/var/X11R6/bin/X",F_OK) != 0)) {
		unlink  ("/usr/X11R6/bin/X");
		unlink  ("/var/X11R6/bin/X");
		if ((symlink ("/var/X11R6/bin/X","/usr/X11R6/bin/X")) != 0) {
			excFileOpenFailed ( errno );
			qError (errorString(),EXC_FILEOPENFAILED);
		}
		if ((symlink (SAX_X11_LOADER    ,"/var/X11R6/bin/X")) != 0) {
			excFileOpenFailed ( errno );
			qError (errorString(),EXC_FILEOPENFAILED);
		}
	}
}

//====================================
// testConfiguration...
//------------------------------------
int SaXConfig::testConfiguration (void) {
	// .../
	//! test the configuration by starting a new X-Server using
	//! the preliminary configuration file.
	// ----
	if (! createConfiguration()) {
		return -1;
	}
	SaXProcessCall* test = new SaXProcessCall ();
	test -> addArgument ( SAX_TEST_CONFIG );
	test -> addArgument ( "-d"  );
	test -> addArgument ( ":99" );
	if ( ! test -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return -1;
	}
	int exitCode = 0;
	QList<QString> data = test -> readStdout();
	QString line;
	foreach (line,data) {
//		QString line (*in.current());
		exitCode = line.toInt();
	}
	switch (exitCode) {
	case 0:
		//====================================
		// test has failed for some reasons
		//------------------------------------
		return -1;
	break;
	case 2:
		//====================================
		// test successful + changes saved
		//------------------------------------
		enableXFineCache();
		if (! createConfiguration()) {
			enableXFineCache (false);
			return false;
		}
		return 0;
	break;
	default:
		//====================================
		// test successful + changes canceled
		//------------------------------------
		return 1;
	break;
	}
}

//====================================
// isChecksumOK...
//------------------------------------
bool SaXConfig::isChecksumOK (void) {
	// .../
	//! check if the installed configuration has a valid
	//! checksum which means there are no manual made changes.
	//! if there is no installed config file or the checksum
	//! doesn't exist the function will return true. Otherwise
	//! the result of the checksum test determines the return
	//! value
	// ---
	QFile curConfig (SAX_SYS_CONFIG);
	QFile curMDFile (SAX_SYS_MD5);
	if (! curConfig.exists()) {
		return true;
	}
	//====================================
	// import stored MD5 sum (sum1)
	//------------------------------------
	if ( ! curMDFile.open( QIODevice::ReadOnly ) ) {
		return true;
	}
	Q3TextStream stream( &curMDFile );
	QString MDSum1 = stream.readLine();
	curMDFile.close();

	//====================================
	// create current MD5 sum (sum2)
	//------------------------------------
	SaXProcessCall* md5 = new SaXProcessCall ();
	md5 -> addArgument ( SAX_MD5_SUM );
	md5 -> addArgument ( SAX_SYS_CONFIG );
	if ( ! md5 -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return true;     
	}
	QList<QString> data = md5 -> readStdout();
	QString MDSum2 = data.first();

	//====================================
	// check sum1 and sum2
	//------------------------------------
	if (MDSum2.contains(MDSum1)) {
		return true;
	}
	return false;
}

//====================================
// setParseErrorValue...
//------------------------------------
void SaXConfig::setParseErrorValue (char* data) {
	// .../
	//! private member which stores the parse error value
	//! for later acces in getParseErrorValue()
	// ---
	QTextOStream (mParseErrorValue) << data;
}

//====================================
// setParseError...
//------------------------------------
void SaXConfig::setParseError (char* data) {
	// .../
	//! private member which stores the parse error
	//! for later acces in getParseError()
	// ----
	QTextOStream (mParseErrorString) << data;
	*mParseErrorString = mParseErrorString->stripWhiteSpace();
}

//====================================
// getParseErrorValue...
//------------------------------------
QString SaXConfig::getParseErrorValue (void) {
	// .../
	//! obtain parse error value if there is any. If no
	//! error occured an empty string is returned
	// ----
	if (mParseErrorValue->isEmpty()) {
		QString* nope = new QString("");
		return *nope;
	}
	return *mParseErrorValue;
}

//====================================
// getParseError...
//------------------------------------
QString SaXConfig::getParseError (void) {
	// .../
	//! obtain parse error if there is any. If no
	//! error occured an empty string is returned
	// ----
	if (mParseErrorString->isEmpty()) {
		QString* nope = new QString("");
		return *nope;
	}
	return *mParseErrorString;
}

//====================================
// removeXFineCache
//------------------------------------
void SaXConfig::removeXFineCache (void) {
	// .../
	//! remove the /var/cache/xfine contents to avoid
	//! importing outdated modeline changes
	// ----
	struct dirent* entry = NULL;
	DIR* cacheDir = NULL;
	cacheDir = opendir (SAX_XFINE_CACHE);
	while ( cacheDir ) {
		entry = readdir (cacheDir);
		if (! entry) {
			break;
		}
		QString file (entry->d_name);
		if ((file == ".") || (file == "..")) {
			continue;
		}
		unlink (file.ascii());
	}
}

//====================================
// error functions for libxf86config...
//------------------------------------
extern "C" {
void VErrorF (const char *f, va_list args) {
	static int n,size = CONFERRORLINE;
	char* data = (char*)malloc (sizeof(char) * size);
	while (1) {
		n = vsnprintf (data, size, f, args);
		if ((n > -1) && (n < size)) {
			break;
		}
		if (n > -1) {
			size = n+1;
		} else {
			size *= 2;
		}
		data = (char*)realloc (data, size);
	}
	if (self) {
	if (strcmp(data,"\n") != 0) {
		self -> setParseErrorValue (data);
	}
	}
}
void ErrorF (const char *f, ...) {
	static int n,size = CONFERRORLINE;
	char* data = (char*)malloc (sizeof(char) * size);
	va_list args;
	while (1) {
		va_start(args, f);
		n = vsnprintf (data, size, f, args);
		va_end (args);
		if ((n > -1) && (n < size)) {
			break;
		}
		if (n > -1) {
			size = n+1;
		} else {
			size *= 2;
		}
		data = (char*)realloc (data, size);
	}
	if (self) {
	if (strcmp(data,"\n") != 0) {
		self -> setParseError (data);
	}
	}
}
}
} // end namespace
