/**************
FILE          : file.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning tool)
              : header definitions for file.c
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XFILE_H
#define XFILE_H 1

#include <qapplication.h>
#include <q3progressdialog.h>
#include <qwidget.h>
#include <stdio.h>
#include <q3dict.h>
#include <qlist.h>
#include <q3dict.h>
#include <qlist.h>
#include <qfile.h>

#include "../api/common/log.h"
#include "config.h"

#define MAX_LINE_LENGTH    8192

class XFile : public QFile {
	private:
	QString* mFileName;
	QFile*   mHandle;
	int      mFileSize;
	char*    mRaw;

	private:
	Q3Dict<char>   gtx;  // gettext key file

	private:
	void fileOpen (void);

	public:
	XFile (const char* name);
	~XFile (void);
	Q3Dict<char> gtxRead (void);
	int fileSize    (void);
	int filePos     (void);
	Q3Dict<char> getGTX    (void);
};

#endif
