/**************
FILE          : monitorplot.cpp
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
#include <stdlib.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <stdio.h>
#include <qlabel.h>
#include <unistd.h>

#include "monitorplot.h"
#include "monitorfig.h"

namespace SaXGUI {
//============================================
// SCCLayoutLine Constructor
//--------------------------------------------
SCCLayoutLine::SCCLayoutLine ( void ) {
	mLeft=mRight=mTop=mBottom = -1;
}

//============================================
// SCCMatrixPoint Matrix Constructor
//--------------------------------------------
SCCMatrixPoint::SCCMatrixPoint (int index) {
	mIndex = index;
}

//============================================
// SCCMatrixInfo Constructor
//--------------------------------------------
SCCMatrixInfo::SCCMatrixInfo ( void ) {
	mLeft=mRight=mTop=mBottom = -1;
}

//============================================
// set index number for this matrix element
//--------------------------------------------
void SCCMatrixPoint::setIndex (int index) {
	mIndex = index;
}

//============================================
// get index number
//--------------------------------------------
int SCCMatrixPoint::getIndex (void) {
	return (mIndex);
}

//============================================
// SCCPlot Matrix view Constructor 
//--------------------------------------------
SCCPlot::SCCPlot (QWidget* parent,int length) : QWidget (parent) {
	setMinimumWidth  ( width() );
	setMinimumHeight ( height() );
	mParent  = parent;
	mRect    = length;
	mPainted = false;
	mID      = 0;
}

//============================================
// SCCPlot matrix resized
//--------------------------------------------
void SCCPlot::resizeEvent (QResizeEvent* e) {
	QSize size = e->size();
	setMinimumWidth  ( size.width() );
	setMinimumHeight ( size.height() );
	buildMatrix();
}

//============================================
// paint matrix view
//--------------------------------------------
void SCCPlot::paintEvent (QPaintEvent*) {
	// ...
	// draw the matrix grid and save the
	// number of columns and rows drawed
	// ---
	QPainter p ( this );
	int Width  = width();
	int Height = height();

	p.setPen(QPen(Qt::gray));
	for (int x=mRect;x<=Width;x+=mRect) {
		p.drawLine(x,0,x,Height);
	}
	for (int y=mRect;y<=Height;y+=mRect) {
		p.drawLine(0,y,Width,y);
	}
}

//============================================
// build Matrix
//--------------------------------------------
void SCCPlot::buildMatrix ( void ) {
	// ...
	// create matrix. The SCCMatrixPoint list contains an 
	// SCCMatrixPoint object for each index containing
	// the positions of the neighbours too
	// ----------------------------------- 
	mColumns   = mRows = 0;
	// ...
	// Prepare Matrix to hold up to 12 Screens
	// in one line for X/Y positions. If more than 12 screens 
	// are available we will run into trouble here
	// ---
	int Width  = FIGURE_SQUARE * MAX_FIGURES;
	int Height = FIGURE_SQUARE * MAX_FIGURES;
	int index = 0;
	int part  = mRect / 2;

	mPoint.clear();
	for (int x=mRect;x<=Width;x+=mRect) {
		mColumns++;
	}
	for (int y=mRect;y<=Height;y+=mRect) {
		mRows++;
	}
	for (int y=mRect;y<=Height;y+=mRect) {
	for (int x=mRect;x<=Width;x+=mRect) {
		SCCMatrixPoint* p = new SCCMatrixPoint (index);

		// base point...
		// ---------------
		p->mBasePoint.setX ( x - part );
		p->mBasePoint.setY ( y - part );

		// right neighbour...
		// ------------------
		p->mRight.setX ( x + part );
		p->mRight.setY ( y - part );

		// left neighbour...
        // ------------------
		p->mLeft.setX ( x - mRect - part );
		p->mLeft.setY ( y - part );

		// top neighbour...
        // ------------------
		p->mTop.setX ( x - part );
		p->mTop.setY ( y - mRect - part );

		// bottom neighbour...
        // ------------------
		p->mBottom.setX ( x - part );
		p->mBottom.setY ( y + part );		
	
		// check neighbours
		// -----------------
		if (p->mRight.x()  < 0) { p->mRight.setX  (0); }
		if (p->mRight.y()  < 0) { p->mRight.setY  (0); }
		if (p->mLeft.x()   < 0) { p->mLeft.setX   (0); }
		if (p->mLeft.y()   < 0) { p->mLeft.setY   (0); }
		if (p->mTop.x()    < 0) { p->mTop.setX    (0); }
		if (p->mTop.y()    < 0) { p->mTop.setY    (0); }
		if (p->mBottom.x() < 0) { p->mBottom.setX (0); }
		if (p->mBottom.y() < 0) { p->mBottom.setY (0); }

		mPoint.append (p);
		index++;
	}
	}
	// ...
	// adapt new index situation...
    // ----------------------------
	Q3IntDict<SCCFig> freeze = mWidget;
	Q3IntDictIterator<SCCFig> it (freeze);
	for (; it.current(); ++it) {
		QPoint pos = it.current()->getPosition();
		int oindex = it.current()->getIndex();
		int nindex = getPointIndex (pos.x(),pos.y());
		if ((oindex != nindex) && (nindex >= 0)) {
			SCCFig* widget = mWidget[oindex];

			mFree.insert   (nindex,"used");
			mFree.remove   (oindex);
			mWidget.insert (nindex, widget);
			mWidget.remove (oindex);

			widget->setIndex (nindex);
		}
	}
	mPainted = true;
}

//============================================
// get matrix rows
//--------------------------------------------
int SCCPlot::getRows ( void ) {
	return (mRows);
}

//============================================
// get matrix columns
//--------------------------------------------
int SCCPlot::getColumns ( void ) {
	return (mColumns);
}

//============================================
// remove widget with ID [x] from the matrix
//--------------------------------------------
void SCCPlot::removeWidget ( int id ) {
	QList<SCCLayoutLine*> layout = getLayout();
	SCCLayoutLine* l;
	clear();
	int col = 1;
	foreach(l,layout) {
//		SCCLayoutLine* l = it;
		if (l->ID == id) {
			continue;
		}
		addWidget(1,col,l->ID);
		col++;
	}
}

//============================================
// remove all widgets from the matrix
//--------------------------------------------
void SCCPlot::clear ( void ) {
	Q3IntDictIterator<SCCFig> it (mWidget);
	for (; it.current(); ++it) {
		delete (it.current());
		mFree.remove (it.current()->getIndex());
	}
	mWidget.clear();
	mWidgetStack.clear();
	repaint();
}

//============================================
// remove all widgets but not the mWidgetStack
//--------------------------------------------
void SCCPlot::clearInternal ( void ) {
	Q3IntDictIterator<SCCFig> it (mWidget);
	for (; it.current(); ++it) {
		delete (it.current());
		mFree.remove (it.current()->getIndex());
	}
	mWidget.clear();
	repaint();
}

//============================================
// add a widget to the matrix
//--------------------------------------------
SCCFig* SCCPlot::addWidget (
	int r,int c,int id,const QString & tip,bool multi
) {
	if (! mPainted) {
		buildMatrix();
	}
	int index = c + (r * mColumns);
	SCCMatrixPoint* p = mPoint.at (index);
	int x = p->mBasePoint.x();
	int y = p->mBasePoint.y();

	if (id < 0) {
		id = mID;
		mID++;
	}
	SCCFig *figure = new SCCFig (this,x,y,index,id,multi);
	figure -> setToolTip ( tip );
	QObject::connect(
		figure, SIGNAL  (sigReleased(SCCFig*)),
		this ,  SLOT    (gotReleased(SCCFig*))
    );
	QObject::connect(
		figure, SIGNAL  (sigPressed (int)),
		this ,  SLOT    (gotPressed (int))
	);
	//figure->show();

	mFree.insert   (index,"used");
	mWidget.insert (index,figure);
	return (figure);
}

//============================================
// add widget with location info to the stack
//--------------------------------------------
void SCCPlot::setWidget (
	int id, int left,int right,int top,int bottom,
	const QString & tip,bool multi
) {
	SCCMatrixInfo* widget = new SCCMatrixInfo;
	widget -> ID     = id;
	widget -> mLeft  = left; widget -> mRight  = right;
	widget -> mTop   = top;  widget -> mBottom = bottom;
	widget -> mTip   = tip;
	widget -> mMulti = multi;
	mWidgetStack.append ( widget );
}

//============================================
// set layout according to stack information
//--------------------------------------------
void SCCPlot::setLayout ( int startRow, int startCol ) {
	unsigned int widgetCount = mWidget.count();
	QListIterator<SCCMatrixInfo*> it (mWidgetStack);
	while (it.hasNext()) {
		SCCFig* figure = NULL;
		SCCMatrixInfo* current = it.next();
		// ...
		// check if widget with ID [current.id] exist, if yes 
		// continue to next widget
		// -----------------------
		if ((figure = searchWidget (current->ID))) {
			continue;
		}
		// ...
		// if we are including the first widget add this
		// widget to row 1 and column 1. This place may
		// be moved later
		// ---------------
		if (mWidget.isEmpty()) {
			addWidget (
				startRow,startCol,current->ID,
				current->mTip,current->mMulti
			);
			widgetCount++;
			continue;
		}
		// ...
		// at this point the [0] widget is already added
		// we will now look at the neighbours of the current
		// widget the first neighbour we found which was already
		// added to the matrix will give us the information
		// where to add the current widget to
		// ---
		int index;
		int around[4] = {
			current->mLeft,current->mRight,
			current->mTop,current->mBottom
		};
		// ...
		// check for widgets without location info (clone)
		// ---
		if (
			(around[0] == -1) && (around[1] == -1) &&
			(around[2] == -1) && (around[3] == -1)
		) {
			addWidget (
				startRow,startCol,current->ID,
				current->mTip,current->mMulti
			);
			widgetCount++;
			continue;
		}
		QPoint  drop;
		SCCMatrixPoint* neighbour;
		for (int n=0;n<4;n++) {
		if (around[n] < 0) {
			continue;
		}
		figure = searchWidget (around[n]);
		if (figure) {
			drop  = figure->getPosition();
			index = getPointIndex (drop.x(),drop.y());
			neighbour = mPoint.at (index);

			switch (n)  {
			case 0: 
			drop = neighbour->mRight;  break;
			case 1: 
			drop = neighbour->mLeft;   break;
			case 2: 
			drop = neighbour->mBottom; break;
			case 3: 
			drop = neighbour->mTop;    break;
			}

			int row,col;
			index = getPointIndex (drop.x(),drop.y());
			div_t fraction = div (index,mColumns);
			row = fraction.quot;
			col = fraction.rem;
			// ...
			// if this widget should be positioned outside
			// the drawing area we will call setLayout recursively
			// with a new row/column specifiaction until the
			// layout is done
			// ---
			if ((row <= 0) || (col <= 0)) {
				if (row <= 0) { startRow++; }
				if (col <= 0) { startCol++; }
				clearInternal();
				setLayout(startRow,startCol);
				return;
			}
			addWidget (
				row,col,current->ID,
				current->mTip,current->mMulti
			);
			widgetCount++;
			break;
		}
		}
	}
	// ...
	// It may be possible that one loop count is not enough to 
	// build the complete layout. This depends on the neighbours 
	// currently added to the layout. just iterate the function 
	// until all widgets are added
	// ---
	if (widgetCount < (unsigned int)mWidgetStack.count()) {
		setLayout(startRow,startCol);
		return;
	}
}

//============================================
// search widget object with ID [x]
//--------------------------------------------
SCCFig* SCCPlot::searchWidget ( int id ) {
	Q3IntDictIterator<SCCFig> it (mWidget);
	for (; it.current(); ++it) {
	if (it.current()->getID() == id) {
		return (it.current());
	}
	}
	return (NULL);
}

//============================================
// search widget object at position [x,y]
//--------------------------------------------
SCCFig* SCCPlot::searchWidget ( int x, int y ) {
	Q3IntDictIterator<SCCFig> it (mWidget);
	for (; it.current(); ++it) {
	QPoint pos = it.current()->getPosition();
	if ((pos.x() == x) && (pos.y() == y)) {
		return (it.current());
	}
	}
	return (NULL);
}

//============================================
// search widget object at position [x,y]
//--------------------------------------------
SCCFig* SCCPlot::searchWidget ( QPoint p ) {
	return (searchWidget (p.x(),p.y()));
}

//============================================
// get index in the matrox for the given point
//--------------------------------------------
int SCCPlot::getPointIndex (int x,int y) {
	SCCMatrixPoint* p;
	foreach (p,mPoint) {
//	SCCMatrixPoint* p = it.current();
	if ((p->mBasePoint.x() == x) && (p->mBasePoint.y() == y)) {
		return(p->getIndex());
	}
	}
	return (-1);
}

//============================================
// got press event [slot]
//--------------------------------------------
void SCCPlot::gotPressed ( int id ) {
	// log (L_INFO,"SCCPlot::gotPressed(%d) called\n",id);
	sigFigure (id);
}

//============================================
// got release event [slot]
//--------------------------------------------
void SCCPlot::gotReleased ( SCCFig* draged ) {
	int nearest  = 0;
	int mindist  = width();

	// ...
	// find the "used" widgets in the matrix and calculate
	// the distance between the draged widget and the other
	// widgets. Save the index of the nearest widget
	// ---------------------------------------------
	SCCMatrixPoint* it;
	foreach (it,mPoint) {
	int index = it->getIndex();
	if ((mFree[index]) && (draged->getIndex() != index)) {
		SCCFig *neighbour = mWidget[index];
		int dist = draged->getDistance (neighbour->getPosition());
		if (dist < mindist) {
			mindist = dist;
			nearest = index;
		}
	}
	}

	QPoint drop;
	int around[4];
	mindist = width();

	// ...
	// get the SCCMatrixPoint object for the nearest index and
	// compare the neighbours distances. Use the nearest
	// neighbour as drag location
	// ---------------------------
	SCCMatrixPoint *p = mPoint.at(nearest);
	around[0] = draged->getDistance (p->mLeft);
	around[1] = draged->getDistance (p->mRight);
	around[2] = draged->getDistance (p->mTop);
	around[3] = draged->getDistance (p->mBottom);
	for (int i=0;i<4;i++) {
	if (around[i] < 0) {
		continue;
	}
	if (around[i] < mindist) {
		mindist = around[i];
		switch (i) {
		case 0: drop = p->mLeft;   break;
		case 1: drop = p->mRight;  break;
		case 2: drop = p->mTop;    break;
		case 3: drop = p->mBottom; break;
		}	
	}
	}
	QPoint origPosition = mPoint.at (draged->getIndex()) -> mBasePoint;
	draged->moveTo (drop.x(),drop.y());

	// ...
	// update the matrix. set new index of draged
	// widget. update mFree and mWidget
	// --------------------------------
	int oldindex = draged->getIndex();
	int newindex = getPointIndex (drop.x(),drop.y());
	if (oldindex == newindex) {
		return;
	}
	mFree.remove   (oldindex);
	mWidget.remove (oldindex);
	mFree.insert   (newindex,"used");
	mWidget.insert (newindex,draged);

	draged -> setIndex (newindex);

	// ...
	// check the current layout and find those widgets
	// which are not at an allowed matrix position
	// --------------------------------------------
	int index = 0;
	int countNeighbours [mWidget.count()];
	SCCFig* countWidgets [mWidget.count()];
	for (unsigned int i=0;i<mWidget.count();i++) {
		countNeighbours[i] = 0;
	}
	Q3IntDictIterator<SCCFig> i (mWidget);
	for (; i.current(); ++i) {
	countWidgets[index] = i.current();
	QPoint pos    = i.current()->getPosition();
	QPoint left   ( pos.x() - mRect , pos.y() );
	QPoint right  ( pos.x() + mRect , pos.y() );
	QPoint top    ( pos.x() , pos.y() - mRect );
	QPoint bottom ( pos.x() , pos.y() + mRect );

	SCCFig* neighbour;
	for (int l=0;l<4;l++) {
		neighbour = NULL;
		switch (l) {
		case 0: neighbour = searchWidget (left);   break;
		case 1: neighbour = searchWidget (right);  break;
		case 2: neighbour = searchWidget (top);    break;
		case 3: neighbour = searchWidget (bottom); break;
		}
		if (neighbour) {
		countNeighbours[index]++;
		}
	}
	// ...
	// widget with no neighbour -> drag them to the next
	// one with the neares distance
	// ---
	if (countNeighbours[index] == 0) {
		gotReleased (i.current());
		return;
	}
	index++;
	}
	// ...
	// widgets with only one neighbour need a special care. We 
	// will look at each widget with only one neighbour and check 
	// the environment. Environment means the [X] positions:
	//
	// a) O X   b) X 0 
	//    X X      X X
	// 
	// An array of three members will hold if there is another
	// widget in the matrix at the [X] positions. The special
	// combination of "0 1 0" will indicate a diagonal gap 
	// ---
	bool gapDetected = false;
	for (unsigned int i=0;i<mWidget.count();i++) {
	if (countNeighbours[i] == 1) {
		int p[3];
		int pos = countWidgets[i] -> getIndex();
		p[0] = pos + 1;          // step to the right
		p[1] = p[0]+ mColumns;   // step to the bottom
		p[2] = pos + mColumns;   // step to the left
		for (int n=0;n<3;n++) {
		if (searchWidget (mPoint.at(p[n])->mBasePoint)) {
			p[n] = 1;
		} else {
			p[n] = 0;
		}
		}
		if ((p[0] == 0) && (p[1] == 1) && (p[2] == 0)) {
			gapDetected = true;
		}
		p[0] = pos + mColumns;   // step to the bottom
		p[1] = p[0]- 1;          // step to the left
		p[2] = pos - 1;          // step to the top
		for (int n=0;n<3;n++) {
		if (searchWidget (mPoint.at(p[n])->mBasePoint)) {
			p[n] = 1;
		} else {
			p[n] = 0;
		}
		}
		if ((p[0] == 0) && (p[1] == 1) && (p[2] == 0)) {
			gapDetected = true;
		}
	}
	}
	// ...
	// next check, we need to have a look at the environment 
	// of the originally position of the draged widget. If there 
	// are more than one neighbour in the horizontal/vertical line 
	// than there is a gap
	// ---
	if (! gapDetected) {
	int countNeighbours = 0;
	SCCMatrixPoint* matrixPoint = mPoint.at (oldindex);
	for (int l=0;l<2;l++) {
		QPoint env;
		switch (l) {
		case 0: env = matrixPoint->mLeft;   break;
		case 1: env = matrixPoint->mRight;  break;
		}
		if (searchWidget (env)) {
		countNeighbours++;
		}
	}
	if (countNeighbours < 2) {
	countNeighbours = 0;
	for (int l=0;l<2;l++) {
		QPoint env;
        switch (l) {
		case 0: env = matrixPoint->mTop;    break;
		case 1: env = matrixPoint->mBottom; break;
		}
		if (searchWidget (env)) {
		countNeighbours++;
		}
	}
	}
	if (countNeighbours >= 2) {
		gapDetected = true;
	}
	}
	// ...
	// if a gap was detected we will switch back to
	// the originally position of the draged widget
	// ---
	if (gapDetected) {
		draged->moveTo (origPosition.x(),origPosition.y());
		mFree.remove   (newindex);
		mWidget.remove (newindex);
		mFree.insert   (oldindex,"used");
		mWidget.insert (oldindex,draged);
		draged -> setIndex (oldindex);
	}
}

//============================================
// get the current matrix layout
//--------------------------------------------
QList<SCCLayoutLine*> SCCPlot::getLayout ( void ) {
	QList<SCCLayoutLine*> layout;
	Q3IntDictIterator<SCCFig> it (mWidget);
	int IDs[mWidget.count()];

	// ...
	// get all the ID numbers from the widgets
	// and sort them after this criterion
	// ----------------------------------
	int count = 0;
	for (; it.current(); ++it) {
		IDs[count] = it.current()->getID();
		count++;
	}
	int i,n;
	for (i=count-1;i>0;i--) {
	for (n=0;n<i;n++) {
		if (IDs[i] < IDs[n]) {
		int help = IDs[i]; IDs[i] = IDs[n]; IDs[n] = help;
		}
	}
	}

	// create the neighbour list for all the
	// widgets and save the result to layout
	// -------------------------------------
	for (i=0;i<count;i++) {
		SCCFig* widget  = searchWidget (IDs[i]);
		QPoint pos    = widget->getPosition();
		QPoint left   ( pos.x() - mRect , pos.y() );
		QPoint right  ( pos.x() + mRect , pos.y() );
		QPoint top    ( pos.x() , pos.y() - mRect );
		QPoint bottom ( pos.x() , pos.y() + mRect );

		SCCLayoutLine* env = new SCCLayoutLine;
		SCCFig* neighbour;
		for (int l=0;l<4;l++) {
		switch (l) {
		// lookup left neighbour...
		// ------------------------
		case 0: 
		if ((neighbour = searchWidget (left))) {
			env->mLeft = neighbour->getID();
		}
		break;
		// lookup right neighbour...
		// -------------------------
		case 1:
		if ((neighbour = searchWidget (right))) {
			env->mRight = neighbour->getID();
		}
		break;
		// lookup top neighbour...
		// -----------------------
		case 2: 
		if ((neighbour = searchWidget (top))) {
			env->mTop = neighbour->getID();
		}
		break;
		// lookup bottom neighbour...
		// --------------------------
		case 3: 
		if ((neighbour = searchWidget (bottom))) {
			env->mBottom = neighbour->getID();
		}
		break;
		}
		}
		env->ID = i;
		layout.append (env);
	}
	return (layout);
}

//============================================
// is the mWidget list empty, matrix empty 
//--------------------------------------------
bool SCCPlot::isClean ( void ) {
	return (mWidget.isEmpty());
}
} // end namespace
