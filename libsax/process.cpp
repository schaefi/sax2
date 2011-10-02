/**************
FILE          : process.cpp
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
#include "process.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXProcess::SaXProcess ( void ) {
	// .../
	//! An object of this type is used to read information
	//! from one of the interfaces ISAX CDB SYSP or PROFILE.
	//! Depending what information should be retrieved a new
	//! process is forked calling isax sysp or createPRO.
	//! The CDB information is based on simple file reading
	// ----
	mProc = new SaXProcessCall ();
}

//====================================
// start...
//------------------------------------
void SaXProcess::start ( QList<const char*> args, int prog ) {
	// .../
	//! This start method will check if (prog) is a valid
	//! program and call it by adding the options set in
	//! args. After the call the appropriate evaluation
	//! method is called to store the data
	// ----
	switch (prog) {
		case SAX_ISAX:
			mProc -> addArgument ( ISAX );
		break;
		case SAX_SYSP:
			mProc -> addArgument ( SYSP );
		break;
		case SAX_PROF:
			mProc -> addArgument ( PROF );
		break;
		case SAX_META:
			mProc -> addArgument ( META );
		break;
		default:
			mProc -> addArgument ( ISAX );
		break;
	}
	const char* it;
	foreach (it,args) {
		mProc->addArgument ( it );
	}
	if ( ! mProc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
	}
	switch (prog) {
		case SAX_ISAX:
			storeData();
		break;
		case SAX_SYSP:
			storeDataSysp();
		break;
		case SAX_PROF:
			storeData();
		break;
		case SAX_META:
			storeData();
		break;
		default:
			storeData();
		break;
	}
}

//====================================
// start...
//------------------------------------
void SaXProcess::start ( int fileID ) {
	// .../
	//! This start method will check if the given fileID
	//! points to a valid file and call the appropriate
	//! evaluation method to the store the data
	// ----
	switch (fileID) {
		case CDB_CARDMODULES:
			storeDataSYS (fileID);
		break;
		case CDB_TABLETMODULES:
			storeDataSYS (fileID);
		break;
		default:
			storeDataCDB (fileID);
		break;
	}
}

//====================================
// storeDataCDB...
//------------------------------------
void SaXProcess::storeDataCDB (int fileID) {
	// .../
	//! Store CDB based group records into the CDB
	//! dictionary. The given file ID must be able to
	//! become resolved into a valid CDB file
	// ----
	QString file;
	switch (fileID) {
		case CDB_CARDS:
			file = CDBCARDS;
		break;
		case CDB_MONITORS:
			file = CDBMONITORS;
		break;
		case CDB_PENS:
			file = CDBPENS;
		break;
		case CDB_PADS:
			file = CDBPADS;
		break;
		case CDB_POINTERS:
			file = CDBPOINTERS;
		break;
		case CDB_TABLETS:
			file = CDBTABLETS;
		break;
		case CDB_TOUCHERS:
			file = CDBTOUCHERS;
		break;
		default:
			excCDBFileFailed ();
			qError (errorString(),EXC_CDBFILEFAILED);
		break;
	}
	//====================================
	// search all entries for file
	//------------------------------------
	DIR* CDBDir = 0;
	struct dirent* entry = 0;
	CDBDir = opendir (CDBDIR);
	QList<QString> fileList;
	if (! CDBDir) {
		excFileOpenFailed ( errno );
		qError (errorString(),EXC_FILEOPENFAILED);
		return;
	}
	while (1) {
		entry = readdir (CDBDir);
		if (! entry) {
			break;
		}
		QString* updateFile = new QString();
		QTextOStream (updateFile) << CDBDIR << entry->d_name;
		if (*updateFile == file) {
			continue;
		}
		if ((updateFile->contains(file)) && (entry->d_type != DT_DIR)) {
			fileList.append (*updateFile);
		}
	}
	closedir (CDBDir);
	fileList.append (file);
	QString it;

	//====================================
	// read in file list
	//------------------------------------
	foreach (it,fileList) {
		QFile* handle = new QFile (it.ascii());
		if (! handle -> open(IO_ReadOnly)) {
			excFileOpenFailed ( errno );
			qError (errorString(),EXC_FILEOPENFAILED);
			return;
		}
		char line[MAX_LINE_LENGTH];
		QString group,key,val;
		while ((handle->readLine (line,MAX_LINE_LENGTH)) != 0) {
			QString string_line(line);
			string_line.truncate(string_line.length()-1);
			if ((string_line[0] == '#') || (string_line.isEmpty())) {
				if (handle->atEnd()) {
					break;
				}
				continue;
			}
			int bp = string_line.find('{');
			if (bp >= 0) {
				QStringList tokens = QStringList::split ( ":", string_line );
				QString vendor = tokens.first();
				QString name   = tokens.last();
				name.truncate(
					name.find('{')
				);
				name   = name.stripWhiteSpace();
				vendor = vendor.stripWhiteSpace();
				group = vendor+":"+name;
			} else {
				bp = string_line.find('}');
				if (bp >= 0) {
					continue;
				}
				QStringList tokens = QStringList::split ( "=", string_line );
				key = tokens.first();
				val = tokens.last();
				val = val.stripWhiteSpace();
				key = key.stripWhiteSpace();
				// ... /
				// CDB keys and ISAX keys are not the same,
				// check this and adapt to ISAX keys
				// ---
				if (key == "Hsync") {
					key = "HorizSync";
				}
				if (key == "Vsync") {
					key = "VertRefresh";
				}
				if (key == "Modeline") {
					key = "SpecialModeline";
				}
				addGroup (group,key,val);
			}
			if (handle->atEnd()) {
				break;
			}
		}
		handle -> close();
	}
}

//====================================
// storeDataSYS...
//------------------------------------
void SaXProcess::storeDataSYS (int fileID) {
	// .../
	//! Store CDB based data which is manually maintained
	//! within the SaX2 source files. Currently only files
	//! of the Format key=value are supported
	// ----
	QString file;
	switch (fileID) {
		case CDB_CARDMODULES:
			file = CDBCARDMODULES;
		break;
		case CDB_TABLETMODULES:
			file = CDBTABLETMODULES;
		break;
		default:
			excCDBFileFailed ();
			qError (errorString(),EXC_CDBFILEFAILED);
		break;
	}
	QFile* handle = new QFile (file);
	if (! handle -> open(IO_ReadOnly)) {
		excFileOpenFailed ( errno );
		qError (errorString(),EXC_FILEOPENFAILED);
		return;
	}
	char line[MAX_LINE_LENGTH];
	QString group,key,val;
	while ((handle->readLine (line,MAX_LINE_LENGTH)) != 0) {
		QString string_line(line);
		string_line.truncate(string_line.length()-1);
		if ((string_line[0] == '#') || (string_line.isEmpty())) {
			if (handle->atEnd()) {
				break;
			}
			continue;
		}
		QStringList tokens = QStringList::split ( "=", string_line );
		group  = tokens.first();
		if (group.contains("Format:(")) {
			continue;
		}
		tokens = QStringList::split ( " ", tokens.last() );
		for (QStringList::Iterator it=tokens.begin(); it!=tokens.end();++ it) {
			QString value (*it);
			QStringList tokens = QStringList::split ( ":", value );
			key = tokens.first();			
			val = tokens.last();
			addGroup (group,key,val);
		}
		if (handle->atEnd()) {
			break;
		}
	}
	handle -> close();
}

//====================================
// storeDataSysp...
//------------------------------------
void SaXProcess::storeDataSysp (void) {
	// .../
	//! Store data which has been written to STDOUT after
	//! a previous sysp process call
	// ----
	QString vesa,vesa2,fbboot;
	QList<QString> data = mProc->readStdout();
	QString line;
	foreach (line,data) {
		int id = 0;
//		QString line (in);
		if (line.isEmpty()) {
			continue;
		}
		QStringList tokens = QStringList::split ( "=>", line );
		QString idstr = tokens.first();
		QString data  = tokens.last();
		QStringList datalist = QStringList::split ( ":", data );
		QString key = datalist.first();
		QString val = datalist.last();
		QRegExp idExp ("(\\d+)");
		int rpos = idExp.search (idstr,0);
		if (rpos >= 0) {
			id = idExp.cap().toInt();
		}
		addID (id);
		val = val.stripWhiteSpace();
		key = key.stripWhiteSpace();
		//printf ("+++ %s %s\n",key.toLatin1().data(),val.toLatin1().data());
		if (key == "Vesa") {
			vesa.append (val);
			vesa.append (",");
		} else
		if (key == "Vesa[2]") {
			vesa2.append (val);
			vesa2.append (",");
		} else
		if (key == "FBBoot") {
			fbboot.append (val);
			fbboot.append (",");
		} else
		if ((! key.isEmpty()) && (! val.isEmpty())) {
			setItem (key,val);
		}
	}
	if (! vesa.isEmpty()) {
		vesa.replace (QRegExp(",$"),"");
		setItem ("Vesa",vesa);
	}
	if (! vesa2.isEmpty()) {
		vesa2.replace (QRegExp(",$"),"");
		setItem ("Vesa[2]",vesa2);
	}
	if (! fbboot.isEmpty()) {
		fbboot.replace (QRegExp(",$"),"");
		setItem ("FBBoot",fbboot);
	}
}

//====================================
// storeData...
//------------------------------------
void SaXProcess::storeData (void) {
	// .../
	//! Store data which has been written to STDOUT after
	//! a previous isax process call
	// ----
	QList<QString> data = mProc->readStdout();
	QString line;
	foreach (line,data) {
//		QString line (*in.current());
		QString cnr;
		QString key;
		QString val;
		int index = 0;
		QStringList tokens = QStringList::split ( ":", line );
		for ( QStringList::Iterator
			in = tokens.begin(); in != tokens.end(); ++in
		) {
			QString item (*in);
			item = item.stripWhiteSpace();
			switch (index) {
			case 0:
				cnr = item;
			break;
			case 1:
				key = item;
			break;
			case 2:
				val = item;
			break;
			default:
				bool isNumber = false;
				if ((key == "Screen") || (key == "Relative")) {
					QRegExp idExp ("^(\\d+)$");
					if (idExp.search (val,0) >= 0) {
						isNumber=true;
					}
				}
				if (
					(((key == "Screen") || (key == "Relative")) &&
					 (!isNumber)) || (key == "Modes")||(key == "Virtual")
				) {
					key = key+":"+val;
					val = item;
				} else {
					val = val+":"+item;
				}
			break;
			}
			index++;
		}
		if (val.isEmpty()) {
			continue;
		}
		addID   (cnr.toInt());
		setItem (key,val);
	}
}
} // end namespace
