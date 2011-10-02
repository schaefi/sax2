/**************
FILE          : xquery.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning too)
              : run xquery -command and deliver the result
              : in a XQuery object
              :
              :
STATUS        : Status: Up-to-date
**************/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xquery.h"

//=====================================
// XQuery Constructor...
//-------------------------------------
XQuery::XQuery (void) {
	mResult = new QString();
	errorCheck = false;
}

//=====================================
// XQuery Constructor...
//-------------------------------------
XQuery::XQuery (const QString& option,const QString& arg) {
	mResult = new QString();
	errorCheck = false;
	mOpt = option;
	mArg = arg;
}

//=====================================
// XQuery set xquery Option...
//-------------------------------------
void XQuery::setOption (const QString& option) {
	mOpt = option;
}

//=====================================
// XQuery set argument for option...
//-------------------------------------
void XQuery::setArguments (const QString& arguments) {
	mArg = arguments;
}

//=====================================
// XQuery need STDERR output...
//-------------------------------------
void XQuery::useErrorHandler (void) {
	errorCheck = true;
}

//=====================================
// XQuery run query...
//-------------------------------------
QString* XQuery::run (void) {
	QString optd ("-d");
	QString dpy  (DisplayString (x11Display()));
	if (mArg.isEmpty()) {
		if (! errorCheck) {
		*mResult = qx ( XQUERY,STDOUT,3,"%s %s %s",
			optd.ascii(),dpy.ascii(),mOpt.ascii()
		);
		} else {
		*mResult = qx ( XQUERY,STDERR,3,"%s %s %s",
			optd.ascii(),dpy.ascii(),mOpt.ascii()
		);
		}
	} else {
		if (! errorCheck) {
		*mResult = qx ( XQUERY,STDOUT,4,"%s %s %s %s",
			optd.ascii(),dpy.ascii(),mOpt.ascii(),mArg.ascii()
		);
		} else {
		*mResult = qx ( XQUERY,STDERR,4,"%s %s %s %s",
			optd.ascii(),dpy.ascii(),mOpt.ascii(),mArg.ascii()
		);
		}
	}
	return (mResult);
}


//=====================================
// XStringList Constructor...
//-------------------------------------
XStringList::XStringList (const QString& text) {
	mSeperator = new QString (" ");
	mText      = new QString (text);
}

//=====================================
// XStringList setSeperator...
//-------------------------------------
void XStringList::setSeperator (const QString& sep) {
	mSeperator->sprintf("%s",sep.ascii());
}

//=====================================
// XStringList getList...
//-------------------------------------
QList<const char*> XStringList::getList (void) {
	if (! mText->isEmpty()) {
		char* item = NULL;
		char* itemlist = (char*) malloc (mText->length() + 1);
		sprintf (itemlist,"%s",mText->latin1());
		item = strtok (itemlist,mSeperator->ascii());
		while (1) {
			if (! item) {
				break;
			}
			QString* insert = new QString ( item );
			mList.append ( insert->latin1() );
			item = strtok (NULL,mSeperator->latin1());
		}
	}
	return (mList);
}

//=====================================
// XStringList setText...
//-------------------------------------
void XStringList::setText (const QString& text) {
	mText->sprintf("%s",text.ascii());
	mList.clear();
}

//=====================================
// XStringList clear...
//-------------------------------------
void XStringList::clear (void) {
	mList.clear();
}

