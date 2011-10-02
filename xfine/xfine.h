/**************
FILE          : xfine.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning tool)
              : header definitions for xfine.c
              : 
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XFINE_H
#define XFINE_H 1

#include "config.h"
#include "../api/common/log.h"

//===================================
// Prototypes...
//-----------------------------------
void usage (void);
bool hasDirectRendering (int);
bool accessAllowed (void);
void signalActivateSave (int);
void signalQuit (int);
void signalSave (int);
void signalReset (int);

//===================================
// Templates...
//-----------------------------------
template <class T>
class XWrapText {
	private:
	T* pM;

	public:
	XWrapText (void) { }
	XWrapText (T* typ) {
		pM = typ;
	}
	void init (T* typ) {
		pM = typ;
	}
	QString operator[] (const QString& key) {
		if (pM->operator[](key)) {
			return(QString::fromLocal8Bit( *pM->operator[](key) ));
		}
		return QString("");
	}
	void insert (const QString & key,QString* val) {
		pM->insert (key,val);
	}
};

#endif
