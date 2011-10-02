/**************
FILE          : xquery.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning tool)
              : run xquery -command and deliver the result
              : in a XQuery object
              : 
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XQUERY_H
#define XQUERY_H 1

#include <qwidget.h>
#include <QString>
#include <qlist.h>
#include <X11/Xlib.h>

#include "config.h"
#include "common/qx.h"

class XQuery : public QWidget {
	private:
	QString* mResult;
	QString mOpt;
	QString mArg;
	bool errorCheck;

	public:
	XQuery ( const QString&,const QString& );
	XQuery ( void );
	QString* run ( void );
	void setArguments ( const QString& );
	void setOption    ( const QString& );
	void useErrorHandler (void);
};

class XStringList {
	private:
	QList<const char*> mList;
	QString*    mSeperator;
	QString*    mText;

	public:
	XStringList  ( const QString& = "" );
	void setText ( const QString& );
	void setSeperator ( const QString& );
	QList<const char*> getList ( void );
	void clear ( void );
};

#endif
