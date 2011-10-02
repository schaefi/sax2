/**************
FILE          : monitorplot.h
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
              : - monitorplot.h: SaXGUI::SCCPlot header definitions
              : - monitorplot.cpp: matrix widget for layout setup
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCPLOT_H
#define SCCPLOT_H 1

#include <qapplication.h>
#include <qlist.h>
#include <qwidget.h>
#include <q3intdict.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QPaintEvent>

#include "monitorfig.h"

namespace SaXGUI {
//============================================
// Defines...
//--------------------------------------------
#define MAX_FIGURES 12

//============================================
// Class SCCLayoutLine...
//--------------------------------------------
class SCCLayoutLine {
	public:
	int ID;
	int mLeft,mRight,mTop,mBottom;

	public:
	SCCLayoutLine ( void );
};


//============================================
// Class SCCMatrixInfo...
//--------------------------------------------
class SCCMatrixInfo {
	public:
	int ID;
	int mLeft,mRight;
	int mTop,mBottom;
	QString mTip;
	bool mMulti;

	public:
	SCCMatrixInfo ( void );
};

//============================================
// Class SCCMatrixPoint...
//--------------------------------------------
class SCCMatrixPoint {
	private:
	int mIndex;

	public:
	QPoint mBasePoint;
	QPoint mLeft;
	QPoint mRight;
	QPoint mTop;
	QPoint mBottom;

	public:
	SCCMatrixPoint (int = 0);
	void setIndex ( int  );
	int  getIndex ( void );
};

//============================================
// Class SCCPlot...
//--------------------------------------------
class SCCPlot : public QWidget {
	Q_OBJECT

	private:
	int mID;
	int mRect;
	int mRows;
	int mColumns;
	QWidget* mParent;
	QList<SCCMatrixPoint*>  mPoint;
	Q3IntDict<char> mFree;
	Q3IntDict<SCCFig> mWidget;
	bool mPainted;
	QList<SCCMatrixInfo*> mWidgetStack;

	public:
	SCCPlot ( QWidget* = 0,int = 40 );
	SCCFig* addWidget ( int,int,int=-1,const QString& = NULL,bool=false );
	void setWidget ( int,int,int,int,int,const QString& = NULL,bool=false );
	void setLayout ( int=1,int=1 );
	SCCFig* searchWidget ( int );
	SCCFig* searchWidget ( int,int );
	SCCFig* searchWidget ( QPoint );
	void removeWidget ( int );
	void buildMatrix  ( void );
	void clear ( void );
	int getPointIndex ( int,int );
	int getRows ( void );
	int getColumns ( void );
	QList<SCCLayoutLine*> getLayout ( void );
	bool isClean ( void );

	private:
	void clearInternal ( void );

	protected:
	virtual void paintEvent  ( QPaintEvent* );
	virtual void resizeEvent ( QResizeEvent* );

	public slots:
	void gotReleased ( SCCFig* );
	void gotPressed  ( int );

	signals:
	void sigFigure ( int );
};
} // end namespace
#endif
