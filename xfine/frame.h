/**************
FILE          : frame.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : XFine2 (display fine tuning tool)
              : header definitions for frame.c
              :
STATUS        : Status: Up-to-date
**************/
#ifndef XFRAME_H
#define XFRAME_H 1

#undef Bool


#include <qapplication.h>
#include <q3mainwindow.h>
#include <qpushbutton.h>
#include <q3scrollview.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qstatusbar.h>
#include <q3vbox.h>
#include <qlist.h>
//Added by qt3to4:
#include <QEvent>
#include <Q3Frame>

#include <Q3PopupMenu>
#include <stdlib.h>

#include "xfine.h"
#include "file.h"
#include "config.h"

//====================================
// Types...
//------------------------------------

enum Direction { 
 
	XLeft     = 10, 
	XRight    = 20, 
	XUp       = 30,
	XDown     = 40, 
	XNarrower = 50, 
	XWider    = 60,
	XTaller   = 70,
	XShorter  = 80,
	XAbort    = 90
};

//====================================
// Class headers...
//------------------------------------
class XFineWindow : public Q3MainWindow {
	Q_OBJECT

	public:
	XFineWindow (int,int,int);
	void loadText (void);

	private:
	QString       mFileName;
    QString       mClock;
    int           mHDisplay,mHDisplaySave;
    int           mHSyncStart,mHSyncStartSave;
    int           mHSyncEnd,mHSyncEndSave;
    int           mHTotal,mHTotalSave;
    int           mVDisplay,mVDisplaySave;
    int           mVSyncStart,mVSyncStartSave;
    int           mVSyncEnd,mVSyncEndSave;
    int           mVTotal,mVTotalSave;
	Q3Dict<QString>*  mTextPtr;
	int           mScreen;
	int           mStep;

	private:
	int           stepID[4];
	bool          stepST[4];
	bool          mAdjustable;
	Q3PopupMenu*   step;
	Q3Frame*       mFrame;
	QPushButton*  mCancel;
	QPushButton*  mSave;
	QTabWidget*   mTab;
	QStatusBar*   mStatus;

	public:
	void saveMode   ( void );
	void resize     ( Direction );

	private:
	void updateStep ( int );
	void setFrame   ( int,int,bool,bool );
	void initScreen ( bool,bool );

	private slots:
	void slotSave   ( void );
	void slotCancel ( void );
	void slotSmall  ( void );
	void slotNormal ( void );
	void slotBig    ( void );
	void slotHuge   ( void );
	void slotNarrower (void);
	void slotWider    (void);
	void slotTaller   (void);
	void slotShorter  (void);
	void slotLeft  (void);
	void slotRight (void);
	void slotUp    (void);
	void slotDown  (void);

	protected:
	virtual bool eventFilter ( QObject*, QEvent* );
};

#endif
