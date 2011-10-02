/**************
FILE          : processcall.cpp
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
#include "processcall.h"
#include <QTextOStream>
    
namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXProcessCall::SaXProcessCall ( void ) {
	mExitCode  = -1;
}

//====================================
// addArgument
//------------------------------------
void SaXProcessCall::addArgument ( const QString& arg ) {
	QString* argument = new QString;
	QTextOStream (argument) << "'" << arg << "'";
	mArguments.append ( *argument );
}

//====================================
// addArgument
//------------------------------------
void SaXProcessCall::addArgument ( int arg ) {
	QString argString;
	QTextOStream (&argString) << arg;
	addArgument ( argString );
}

//====================================
// clearArguments
//------------------------------------
void SaXProcessCall::clearArguments ( void ) {
	mArguments.clear();
	mExitCode  = -1;
}

//====================================
// exitStatus
//------------------------------------
int SaXProcessCall::exitStatus ( void ) {
	return mExitCode;
}

//====================================
// start
//------------------------------------
bool SaXProcessCall::start ( void ) {
	//====================================
	// create program call string
	//------------------------------------
	QString program;
	QString it;
	foreach (it,mArguments) {
		program.append (it + " ");
	}
	//====================================
	// start program and connect stream
	//------------------------------------
	char buf[LINESIZE];
	//fprintf (stderr,"++++ DEBUG: %s\n",program.ascii());
	FILE* fp = popen (program.ascii(),"r");
	if ( ! fp ) {
		return false;
	}
	while (NULL != (fgets(buf,sizeof(buf),fp))) {
		int line = strlen(buf);
		buf[line-1] = '\0';
		//fprintf (stderr,"__%s__\n",buf);
		mData.append (QString(buf));
	}
	mExitCode = pclose(fp);
	return true;
}

//====================================
// readStdout
//------------------------------------
QList<QString> SaXProcessCall::readStdout ( void ) {
	return mData;
}
} // end namespace
