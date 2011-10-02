/**************
FILE          : file.cpp
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
              : - file.h: SaXGUI::SCCFile header definitions
              : - file.cpp: provide access to GUI files like xapi.gtx
              : ----
              :
STATUS        : Status: Development
**************/
#include "file.h"

namespace SaXGUI {
//====================================
// SCCFile constructor...
//------------------------------------
SCCFile::SCCFile (const QString& name) {
	// .../
	// create a new QFile handle and open the file. If the
	// open call will fail the program terminates with (1)
	// ----
	mHandle = new QFile (name);
	fileOpen ();
}

//====================================
// fileOpen...
//------------------------------------
void SCCFile::fileOpen (void) {
	// .../
	// open the file stored in the QFile handle and exit
	// on error. The file is opened in IO_ReadOnly mode
	// ----
	if (! mHandle -> isOpen()) {
	if (! mHandle -> open(QIODevice::ReadOnly)) {
		log (L_ERROR,
			"SCCFile::fileOpen: open failed on: %s -> %s\n",
			mHandle->name().ascii(),strerror(errno)
		);
		exit(1);
	}
	}
}

//====================================
// readDict...
//------------------------------------
Q3Dict<QString> SCCFile::readDict (void) {
	// .../
	// read the contents of the file whereas the file format
	// has to use a KEY=VALUE description. Comments starting
	// with a "#" are allowed
	// ----
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
		QStringList tokens = QStringList::split ( "=", string_line );
		key = tokens.first();
		*val = tokens.last();
		*val = val->stripWhiteSpace();
		key = key.stripWhiteSpace();
		int bslash = val->find ('\\');
		if (bslash >= 0) {
			*val = val->replace (bslash,2,"\n");
			while ( 1 ) {
				bslash = val->find ('\\');
				if (bslash >= 0) {
					*val = val->replace (bslash,2,"\n");
				} else {
					break;
				}
			}
		}
		gtx.insert (key,val);
		if (mHandle->atEnd()) {
			break;
		}
	}
	mHandle->close();
	return (gtx);
}

//====================================
// readList...
//------------------------------------
QList<QString> SCCFile::readList (void) {
	// .../
	// read the contents of the file whereas the file format
	// can be a simple list. Comments starting with a "#" are
	// allowed
	// ----
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
		gtxlist.append ( string_line);
		if (mHandle->atEnd()) {
			break;
		}
	}
	mHandle->close();
	return (gtxlist);
}

//====================================
// XFile return GTX dictionary...
//------------------------------------
Q3Dict<QString> SCCFile::getDataDict (void) {
	// .../
	// return the data dictionary without rereading the
	// information from file
	// ----
	return (gtx);
}

//====================================
// XFile return GTX list...
//------------------------------------
QList<QString> SCCFile::getDataList (void) {
	// .../
	// return the data dictionary without rereading the
	// information from file
	// ----
	return (gtxlist);
}
} // end namespace
