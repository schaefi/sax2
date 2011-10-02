/**************
FILE          : file.cpp
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
#include "file.h"

namespace SaX {
//====================================
// SaXFile constructor...
//------------------------------------
SaXFile::SaXFile (const QString& name) {
	// .../
	//! create a new QFile handle and open the file. If the
	//! open call will fail the program terminates with (1)
	// ----
	mHandle = new QFile (name);
	fileOpen ();
}

//====================================
// fileOpen...
//------------------------------------
void SaXFile::fileOpen (void) {
	// .../
	//! open the file stored in the QFile handle and exit
	//! on error. The file is opened in IO_ReadOnly mode
	// ----
	if (! mHandle -> isOpen()) {
	if (! mHandle -> open(QIODevice::ReadOnly)) {
		excFileOpenFailed ( errno );
		qError (errorString(),EXC_FILEOPENFAILED);
	}
	}
}

//====================================
// readDict...
//------------------------------------
Q3Dict<QString> SaXFile::readDict (void) {
	// .../
	//! read the contents of the file whereas the file format
	//! has to use a KEY:VALUE description. Comments starting
	//! with a "#" are allowed
	// ----
	if (! mHandle -> isOpen()) {
		Q3Dict<QString>* nope = new Q3Dict<QString>;
		return *nope;
	}
	char line[MAX_LINE_LENGTH];
	while ((mHandle->readLine(line,MAX_LINE_LENGTH)) != 0) {
		QString string_line(line);
		string_line.truncate(string_line.length()-1);
		if ((string_line[0] == '#') || (string_line.isEmpty())) {
			if (mHandle->atEnd()) {
				break;
			}
			continue;
		}
		QString key;
		QString* val = new QString();
		QStringList tokens = QStringList::split ( ":", string_line );
		key = tokens.first();
		*val = tokens.last();
		*val = val->stripWhiteSpace();
		key = key.stripWhiteSpace();
		mDict.insert (key,val);
		if (mHandle->atEnd()) {
			break;
		}
	}
	mHandle->close();
	return (mDict);
}

//====================================
// SaXFile return dictionary...
//------------------------------------
Q3Dict<QString> SaXFile::getDataDict (void) {
	// .../
	//! return the data dictionary without rereading the
	//! information from file
	// ----
	return (mDict);
}
} // end namespace
