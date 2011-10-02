/**************
FILE          : wrapper.h
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
              : - wrapper.h: wrap pointer to object type
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCWRAP_H
#define SCCWRAP_H 1

#include <qthread.h>
#include <qprogressdialog.h>
#include "../libsax/sax.h"

//===================================
// Class SCCWrapPointer [ template ]
//-----------------------------------
template <class T>
class SCCWrapPointer {
	private:
	T* pM;

	public:
	SCCWrapPointer (void) { }
	SCCWrapPointer (T* typ) {
		pM = typ;
	}
	void init (T* typ) {
		pM = typ;
	}
	QString operator[] (const QString& key) {
		if (pM->operator[](key)) {
			return(QString::fromLocal8Bit( *pM->operator[](key) ));
		}
		QString* nope = new QString;
		return *nope;
	}
	void insert (const QString & key,QString* val) {
		pM->insert (key,val);
	}
};

//===================================
// Class SCCTestThread
//-----------------------------------
class SCCTestThread : public QThread {
	private:
	SaXConfig* mConfig;
	int        mStatus;

	public:
	void run ( void ) {
		mStatus = mConfig -> testConfiguration();
	}

	public:
	int status ( void ) {
		return mStatus;
	}

	public:
	SCCTestThread ( SaXConfig* config ) {
		mConfig = config;
		mStatus = -1;
	}
};
#endif
