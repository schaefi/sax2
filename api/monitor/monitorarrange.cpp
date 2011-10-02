/**************
FILE          : monitorarrange.cpp
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
              : - monitorarrange.h: SaXGUI::SCCMonitor header definitions
              : - monitorarrange.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitorarrange.h"
//Added by qt3to4:
#include <QPixmap>
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitorArrange::SCCMonitorArrange (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget
	//-------------------------------------
	mMainLayout = new Q3VBoxLayout ( this );

	//=====================================
	// create macro widgets
	//-------------------------------------
	mModusGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["Modus"],this  
	);
	mTraditional = new QRadioButton ( mText["ModusTraditional"], mModusGroup );
	mClone       = new QRadioButton ( mText["ModusClone"], mModusGroup );
	mXinerama    = new QRadioButton ( mText["ModusXinerama"], mModusGroup );
	mLayoutGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["Arrangement"],this
	);
	Q3ScrollView* mScroll = new Q3ScrollView ( mLayoutGroup );
	mMatrix = new SCCPlot ( this,FIGURE_SQUARE );
	mScroll -> setResizePolicy ( Q3ScrollView::AutoOneFit );
	mScroll -> addChild ( mMatrix );
	mMatrix -> buildMatrix();

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mModusGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mLayoutGroup );
}
//====================================
// init
//------------------------------------
void SCCMonitorArrange::init ( void ) {
	// nothing to be done for init
}
//====================================
// import
//------------------------------------
void SCCMonitorArrange::import ( void ) {
	//====================================
	// create manipulators
	//------------------------------------
	SaXManipulateLayout saxLayout (
		mSection["Layout"],mSection["Card"]
	);
	//====================================
	// handle multihead mode data
	//------------------------------------
	mLayoutMode = saxLayout.getMultiheadMode();
	bool isCloned      = false;
	bool isTraditional = false;
	bool isXinerama    = false;
	switch (mLayoutMode) {
		case SAX_CLONE:
			isCloned = true;
		break;
		case SAX_TRADITIONAL:
			isTraditional = true;
		break;
		case SAX_XINERAMA:
			isXinerama = true;
		break;
		default:
			log (L_WARN,
				"SCCMonitorArrange::Warning: couldn't get multihead mode\n"
			);
			log (L_WARN,
				"SCCMonitorArrange::Warning: using default clone mode\n"
			);
			isCloned = true;
		break;
	}
	//====================================
	// set widgets to imported data
	//------------------------------------
	if ( isCloned ) {
		mClone -> setChecked ( true );
	}
	if ( isTraditional ) {
		mTraditional -> setChecked ( true );
	}
	if ( isXinerama ) {
		mXinerama -> setChecked ( true );
	}
}
//====================================
// setDeviceMap
//------------------------------------
void SCCMonitorArrange::setDeviceMap (
	int devices,QList<SCCMonitorDisplay*> displays
) {
	//====================================
	// obtain enabled device ID's
	//------------------------------------
	int count = 0;
	int index = 0;
	mCardID = (int*)malloc (devices * sizeof (int));
	SCCMonitorDisplay* it;
	foreach (it,displays) {
		mCardID[count] = -1;
		if ((it->isEnabled()) && (index < devices)) {
			mCardID[index] = count;
			index++;
		}
		count++;
	}
}
//====================================
// setArrangement
//------------------------------------
void SCCMonitorArrange::setArrangement (
	int devices,QList<SCCMonitorDisplay*> displays
) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mMatrix -> clear();

	//====================================
	// create manipulators
	//------------------------------------
	SaXManipulateLayout saxLayout (
		mSection["Layout"],mSection["Card"]
	);
	//====================================
	// obtain enabled device ID's
	//------------------------------------
	setDeviceMap ( devices,displays );

	//====================================
	// setup layout
	//------------------------------------
	for (int count=0;count < devices; count++) {
		QString key; QTextOStream (&key)
			<< "Screen:Screen[" << mCardID[count] << "]";
		QList<QString> neighbours = saxLayout.getXOrgLayout ( mCardID[count] );
		if (neighbours.isEmpty()) {
			continue;
		}
		QString at;
		int indexList [4];
		int index = 0;
		foreach (at,neighbours) {
			QString item = at;
			if (item.contains("none")) {
				indexList[ index ] = -1; index++;
				continue;
			}
			item.replace (QRegExp("Screen\\["),"");
			item.replace (QRegExp("\\]"),"");
			indexList[ index ] = item.toInt();
			index++;
		}
		SCCMonitorDisplay* displayInfo = displays.at ( mCardID[count] );
		SCCMonitorModel* monitorData = displayInfo -> getMonitorData();
		QString vendor = monitorData->getVendorName();
		QString model  = monitorData->getModelName();
		QString monitorInfo;
		if ( vendor != model ) {
			QTextOStream (&monitorInfo) << "<b>[" << count << "]</b> " << 
				mText["PrimaryMonitor"] << " " << vendor << " " << model;
		} else {
			QTextOStream (&monitorInfo) << "<b>[" << count << "]</b> " <<
				mText["UnknownMonitor"];
		}
		QString toolTip;
		QTextOStream (&toolTip)
			<< "<table border=0 cellspacing=0>"
			<< "<tr>"
			<< "<th rowspan=2><img src=\"" << TIP_PIXMAP << "\"></th>"
			<< "<td><nobr>" << monitorInfo << "</nobr></td>"
			<< "</tr>"
			<< "<tr>"
			<< "<td><nobr><hr>" << mText["ConnectedTo"] << " "
			<< displayInfo->getCardName() << "</nobr></td>"
			<< "</tr>"
			<< "</table>";
		mMatrix->setWidget(
			mCardID[count],indexList[0],indexList[1],indexList[2],indexList[3],
			toolTip,displayInfo->isDualModeEnabled()
		);
	}
	mMatrix->setLayout();
	//====================================
	// update figure pixmaps
	//------------------------------------
	for (int count=0;count < devices; count++) {
		if ( mCardID[count] == -1 ) {
			continue;
		}
		SCCMonitorDisplay* displayInfo = displays.at ( mCardID[count] );
		setFigurePixmap (
			displayInfo->getDisplay(),
			displayInfo->getDualPixmap()
		);
	}
}
//====================================
// setCleanArrangement
//------------------------------------
void SCCMonitorArrange::setCleanArrangement (
	int devices,QList<SCCMonitorDisplay*> displays
) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mMatrix -> clear();

	//====================================
	// obtain enabled device ID's
	//------------------------------------
	setDeviceMap ( devices,displays );

	//====================================
	// setup layout
	//------------------------------------
	for (int count=0;count < devices; count++) {
		if ( mCardID[count] == -1 ) {
			continue;
		}
		int indexList [4] = {-1,-1,-1,-1};
		if ( count == 0) {
			indexList[0] = -1;
			indexList[1] = mCardID[count+1];
		} else if ( count == devices - 1 ) {
			indexList[1] = -1;
			indexList[0] = mCardID[count-1];
		} else {
			indexList[ 0 ] = mCardID[count-1];
			indexList[ 1 ] = mCardID[count+1];
		}
		SCCMonitorDisplay* displayInfo = displays.at ( mCardID[count] );
		SCCMonitorModel* monitorData = displayInfo -> getMonitorData();
		QString vendor = monitorData->getVendorName();
		QString model  = monitorData->getModelName();
		QString monitorInfo;
		if ( vendor != model ) {
			QTextOStream (&monitorInfo) << "<b>[" << count << "]</b> " <<
				mText["PrimaryMonitor"] << " " << vendor << " " << model;
		} else {
			QTextOStream (&monitorInfo) << "<b>[" << count << "]</b> " <<
				mText["UnknownMonitor"];
		}
		QString toolTip;
		QTextOStream (&toolTip)
			<< "<table border=0 cellspacing=0>"
			<< "<tr>"
			<< "<th rowspan=2><img src=\"" << TIP_PIXMAP << "\"></th>"
			<< "<td><nobr>" << monitorInfo << "</nobr></td>"
			<< "</tr>"
			<< "<tr>"
			<< "<td><nobr><hr>" << mText["ConnectedTo"] << " "
			<< displayInfo->getCardName() << "</nobr></td>"
			<< "</tr>"
			<< "</table>";
		mMatrix->setWidget(
			mCardID[count],indexList[0],indexList[1],indexList[2],indexList[3],
			toolTip,displayInfo->isDualModeEnabled()
		);
	}
	mMatrix->setLayout();
	//====================================
	// update figure pixmaps
	//------------------------------------
	for (int count=0;count < devices; count++) {
		if ( mCardID[count] == -1 ) {
			continue;
		}
		SCCMonitorDisplay* displayInfo = displays.at ( mCardID[count] );
		setFigurePixmap (
			displayInfo->getDisplay(),
			displayInfo->getDualPixmap()
		);
	}
}
//=====================================
// setFigurePixmap
//-------------------------------------
void SCCMonitorArrange::setFigurePixmap ( int id, QPixmap pixmap ) {
	SCCFig* mFig = mMatrix->searchWidget ( id );
	if ( mFig ) {
		mFig -> setFigurePixmap ( pixmap );
	}
}
//=====================================
// setToolTip
//-------------------------------------
void SCCMonitorArrange::setToolTip ( SCCMonitorDisplay* display ) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	SCCFig* mFig = mMatrix->searchWidget ( display->getDisplay() );
	if ( mFig ) {
		SCCMonitorModel* monitorData = display -> getMonitorData();
		QString vendor = monitorData->getVendorName();
		QString model  = monitorData->getModelName();
		QString monitorInfo;
		if ( vendor != model ) {
			QTextOStream (&monitorInfo) << "<b>[" << mFig->getID() << "]</b> "<<
				mText["PrimaryMonitor"] << " " << vendor << " " << model;
		} else {
			QTextOStream (&monitorInfo) << "<b>[" << mFig->getID() << "]</b> "<<
				mText["UnknownMonitor"];
		}
		QString toolTip;
		QTextOStream (&toolTip)
			<< "<table border=0 cellspacing=0>"
			<< "<tr>"
			<< "<th rowspan=2><img src=\"" << TIP_PIXMAP << "\"></th>"
			<< "<td><nobr>" << monitorInfo << "</nobr></td>"
			<< "</tr>"
			<< "<tr>"
			<< "<td><nobr><hr>" << mText["ConnectedTo"] << " "
			<< display->getCardName() << "</nobr></td>"
			<< "</tr>"
			<< "</table>";
		mFig -> setToolTip ( toolTip );
	}
}
//====================================
// getMultiheadMode
//------------------------------------
int SCCMonitorArrange::getMultiheadMode ( void ) {
	if ( mTraditional->isChecked() ) {
		return 0;
	}
	if ( mClone->isChecked() ) {
		return 1;
	}
	if ( mXinerama->isChecked() ) {
		return 2;
	}
	return -1;
}
//====================================
// getArrangement
//------------------------------------
Q3Dict<QString> SCCMonitorArrange::getArrangement ( void ) {
	Q3Dict<QString> result;
	QList<SCCLayoutLine*> layout = mMatrix->getLayout();
	SCCLayoutLine* l;
	foreach (l,layout) {
		QString key;
		QString* value = new QString;
		int neighbour[4] = {l->mLeft,l->mRight,l->mTop,l->mBottom};
		key.sprintf("Screen:Screen[%d]",mCardID[l->ID]);
		for (int n=0;n<4;n++) {
			if (neighbour[n] == -1) {
				value->sprintf (
					"%s <none>",value->ascii()
				);
			} else {
				value->sprintf (
					"%s Screen[%d]",value->ascii(),neighbour[n]
				);
			}
		}
		*value = value->stripWhiteSpace();
		result.insert (key,value);
	}
	return result;
}
//====================================
// slotMonitorChanged
//------------------------------------
void SCCMonitorArrange::slotMonitorChanged ( SCCMonitorDisplay* display ) {
	setToolTip ( display );
}
//====================================
// slotDualModeChanged
//------------------------------------
void SCCMonitorArrange::slotDualModeChanged ( SCCMonitorDisplay* display ) {
	setFigurePixmap (
		display->getDisplay(),
		display->getDualPixmap()
	);
}
} // end namespace
