/**************
FILE          : monitorfig.h
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
              : - monitorfig.h: SaXGUI::SCCFig header definitions
              : - monitorfig.cpp: figure objects of a matrix widget
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCFIG_H
#define SCCFIG_H 1

#include <qapplication.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>

namespace SaXGUI {
//====================================
// Defines...
//------------------------------------
#define FIGURE_SQUARE 60

//====================================
// Defines...
//------------------------------------
#define PIXDIR         "/usr/share/sax/api/figures"
#define FIGURE_SINGLE  PIXDIR "/monitor_single.png"
#define FIGURE_MULTI   PIXDIR "/monitor_multi.png"

//====================================
// Class SCCFig...
//------------------------------------
class SCCFig : public QPushButton {
	Q_OBJECT

	private:
	int mID;
	int mIndex;
	int mStartx;
	int mStarty;
	QRect mStartg;
	QPixmap* mPixmap;
	QString* tipText;

	private:
	float mCurrentx;
	float mCurrenty;

	public:
	~SCCFig (void);
	SCCFig ( QWidget *p,int = 0,int = 0,int = 0, int = 0,bool=false );
	QPoint getPosition ( void );
	const char* getTip ( void );
	int  getDistance ( int,int );
	void moveTo      ( int,int );
	void setToolTip  ( const QString& );
	void setFigurePixmap ( QPixmap ); 
	int  getDistance ( QPoint  );
	int  getIndex ( void );
	void setIndex ( int  );
	int  getID    ( void );

	protected:
	virtual void mouseMoveEvent ( QMouseEvent* );
	virtual void mouseDoubleClickEvent ( QMouseEvent* );
	virtual void mousePressEvent ( QMouseEvent* );
	virtual void mouseReleaseEvent ( QMouseEvent* );
	virtual void paintEvent ( QPaintEvent* );

	signals:
	void sigReleased ( SCCFig* );
	void sigPressed  ( int );
};
} // end namespace
#endif
