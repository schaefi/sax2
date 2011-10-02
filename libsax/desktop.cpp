/**************
FILE          : desktop.cpp
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
#include "desktop.h"
#include <X11/Xcms.h>
#include <X11/extensions/scrnsaver.h>

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulateDesktop::SaXManipulateDesktop (
	SaXImport* desktop, SaXImport* card , SaXImport* path, int desktopID
) {
	// .../
	//! An object of this type is used to configure desktop
	//! properties like resolution and colors
	// ----
	if ( (! desktop) || (! card) ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( desktop->getSectionID() != SAX_DESKTOP ) {
		excDesktopImportBindFailed ( desktop->getSectionID() );
		qError (errorString(),EXC_DESKTOPIMPORTBINDFAILED);
		return;
	}
	if ( card->getSectionID() != SAX_CARD ) {
		excCardImportBindFailed ( card->getSectionID() );
		qError (errorString(),EXC_CARDIMPORTBINDFAILED);
		return;
	}
	if ( path->getSectionID() != SAX_PATH ) {
		excPathImportBindFailed ( path->getSectionID() );
		qError (errorString(),EXC_PATHIMPORTBINDFAILED);
		return;
	}
	mDesktopID = desktopID;
	mDesktop   = desktop;
	mCard      = card;
	mPath      = path;
	mDesktop -> setID ( mDesktopID );
	mCard    -> setID ( mDesktopID );
	mPath    -> setID ( 0 );
	mCDBMonitors = 0;
	mSyspDesktop = 0;
}

//====================================
// set desktop ID
//------------------------------------
bool SaXManipulateDesktop::selectDesktop (int desktop) {
	// .../
	//! select the desktop device the changes should be applied to
	//! if the device does not exist (false) is returned
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	if ( (mDesktop->setID (desktop)) && (mCard->setID (desktop)) ) {
		mDesktopID = desktop;
		return true;
	}
	return false;
}

//====================================
// calculateModelines
//------------------------------------
void SaXManipulateDesktop::calculateModelines (bool calc) {
	// .../
	//! enable/disable the modeline calculation. This is
	//! onyl needed for non VESA modes or modes which are not
	//! part of the internal X-Server mode pool
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString val ("no");
	if (calc) {
		val = "on";
	}
	mDesktop -> setItem ( "CalcModelines",val );
}

//====================================
// setResolution
//------------------------------------
void SaXManipulateDesktop::setResolution (int c,int x,int y) {
	// .../
	//! set resolution (X)x(Y) for colordepth (c). Using this
	//! member will overwrite the current resolution setting
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString key;
	QString val;
	key.sprintf ("Modes:%d",c);
	val.sprintf ("%dx%d",x,y);
	mDesktop -> setItem ( key,val );
}

//====================================
// addResolution
//------------------------------------
void SaXManipulateDesktop::addResolution (int c,int x,int y) {
	// .../
	//! add resolution (X)x(Y) for colordepth (c). Using this
	//! member will add the resolution to the current setting
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString key;
	QString val;
	key.sprintf ("Modes:%d",c);
	if (! mDesktop -> getItem (key).isEmpty()) {
		val = mDesktop -> getItem (key);
	}
	QTextOStream (&val) << "," << x << "x" << y;
	val.replace (QRegExp("^,"),"");
	mDesktop -> setItem ( key,val );
}

//====================================
// removeResolution
//------------------------------------
void SaXManipulateDesktop::removeResolution (int c,int x,int y) {
	// .../
	//! remove the resolution (X)x(Y) for colordepth (c) from
	//! the current Modes value
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString key;
	QString val;
	key.sprintf ("Modes:%d",c);
	val.sprintf ("%dx%d",x,y);
	if (! mDesktop -> getItem (key).isEmpty()) {
		mDesktop -> removeItem (key,val);
	}
}

//====================================
// setVirtualResolution
//------------------------------------
void SaXManipulateDesktop::setVirtualResolution (int c,int x,int y) {
	// .../
	//! set virtual resolution (X)x(Y) for colordepth (c). This
	//! call will overwrite the current setting
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString key;
	QString val;
	key.sprintf ("Virtual:%d",c);
	val.sprintf ("%d %d",x,y);
	mDesktop -> setItem ( key,val );
}

//====================================
// removeVirtualResolution
//------------------------------------
void SaXManipulateDesktop::removeVirtualResolution (int c) {
	// .../
	//! remove the virtual resolution (X)x(Y) set for
	//! colordepth (c).
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString key;
	key.sprintf ("Virtual:%d",c);
	if (! mDesktop -> getItem (key).isEmpty()) {
		mDesktop -> setItem (key,"");
	}
}

//====================================
// setColorDepth
//------------------------------------
void SaXManipulateDesktop::setColorDepth (int c) {
	// .../
	//! set the default color depth. Valid values are
	//! 4,8,15,16 and 24
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString color;
	color.sprintf ("%d",c);
	mDesktop -> setItem ( "ColorDepth",color );
}

//====================================
// is3DCard
//------------------------------------
bool SaXManipulateDesktop::is3DCard (void) {
	// .../
	//! check if the card can be used with 3D hardware
	//! acceleration according to the CDB information
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (! pCard -> setID ( mDesktopID )) {
		return false;
	}
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");
	//====================================
	// retrieve CDB card list
	//------------------------------------
	SaXProcess* CDBCards = new SaXProcess ();
	CDBCards -> start (CDB_CARDS);
	Q3Dict< Q3Dict<QString> > CDBData = CDBCards -> getTablePointerCDB ();
	Q3Dict<QString>* cardData = CDBData.find( mCardName );
	if ( cardData ) {
		QString* flag = cardData -> find ("Flag");
		if (flag) {
			QString  driver = mCard -> getItem ("Driver");
			QString* driver3D = cardData -> find ("3DDriver");
			if ((driver3D) && (driver == *driver3D)) {
				//====================================
				// check 3D driver installation
				//------------------------------------
				if ( driver == "nvidia") {
					QString vendor = getVendorForDriver ( driver );
					if (vendor == "NVIDIA Corporation") {
						return true;
					}
					return false;
				}
				return true;
			}
			return false;
		}
		return false;
	}
	//====================================
	// No CDB record found for cardName
	//------------------------------------
	excEmptyCDBGroup ( mCardName.latin1() );
	qError (errorString(),EXC_EMPTYCDBGROUP);
	return false;
}

//====================================
// getDualHeadProfile
//------------------------------------
QString SaXManipulateDesktop::getDualHeadProfile ( void ) {
	// .../
	//! return the full qualified profile file name used
	//! to set the driver options for a dual head configuration.
	//! if the card is not a dual head card or the assigned
	//! profile is not a DualHead_DriverOptions profile the
	//! method will return an empty QString
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	//===================================
	// Check SaXMeta data...
	//-----------------------------------
	Q3Dict<QString> metaData = getMetaData();
	if (metaData["SAX_NO_CDB_CHECK"]) {
		QString driver  = mCard -> getItem ("Driver");
		QString profile = getDriverOptionsDualHeadProfile ( driver );
		return profile;
	}
	//====================================
	// check special laptop case
	//------------------------------------
	QString result;
	SaXManipulateCard cardInfo (mCard);
	if (cardInfo.isNoteBook()) {
		QString driver = mCard -> getItem ("Driver");
		result = getDriverOptionsDualHeadProfile ( driver );
		return result;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (! pCard -> setID ( mDesktopID )) {
		QString* nope = new QString;
		return *nope;
	}
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");
	//====================================
	// retrieve CDB card list
	//------------------------------------
	SaXProcess* CDBCards = new SaXProcess ();
	CDBCards -> start (CDB_CARDS);
	Q3Dict< Q3Dict<QString> > CDBData = CDBCards -> getTablePointerCDB ();
	Q3Dict<QString>* cardData = CDBData.find ( mCardName );
	if ( cardData ) {
		QString* profile = cardData -> find ("Profile");
		if ((profile) && (profile->contains("DualHead_DriverOptions"))) {
			//====================================
			// check if drivers do match...
			//------------------------------------
			QString syspDriver = pCard -> getItem ("Module");
			QString* CDB2DDriver = cardData -> find ("Driver");
			QString* CDB3DDriver = cardData -> find ("3DDriver");
			if (
				((CDB2DDriver) && (syspDriver == *CDB2DDriver)) ||
				((CDB3DDriver) && (syspDriver == *CDB2DDriver))
			) {
				QTextOStream (&result) << PROFILE_DIR << profile;
				return result;
			}
			//====================================
			// drivers don't match check driver...
			//------------------------------------
			result = getDriverOptionsDualHeadProfile ( syspDriver );
			return result;
		}
	}
	//====================================
	// No CDB record found for cardName
	//------------------------------------
	excEmptyCDBGroup ( mCardName.latin1() );
	qError (errorString(),EXC_EMPTYCDBGROUP);
	QString* nope = new QString;
	return *nope;
}

//====================================
// isXineramaMode
//------------------------------------
bool SaXManipulateDesktop::isXineramaMode (void) {
	// .../
	// check the meta data information if the configuration
	// workflow requested to use Xinerama instead of the
	// default dual mode workflow based on MergedFB
	// ----
	Q3Dict<QString> metaData = getMetaData();
	if (metaData["SAX_NO_DUAL_MODE"]) {
		return true;
	}
	return false;
}

//====================================
// isDualHeadCard
//------------------------------------
bool SaXManipulateDesktop::isDualHeadCard (void) {
	// .../
	//! check if the card is a dual head card The check
	//! is based on a profile check. If the card is bound
	//! to a DualHead profile the method will return true
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	//===================================
	// Check SaXMeta data...
	//-----------------------------------
	Q3Dict<QString> metaData = getMetaData();
	if (metaData["SAX_NO_CDB_CHECK"]) {
		QString driver  = mCard -> getItem ("Driver");
		QString profile = getDriverOptionsDualHeadProfile ( driver );
		if (! profile.isEmpty()) {
			return true;
		}
		return false;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (! pCard -> setID ( mDesktopID )) {
		return false;
	}
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");
	//====================================
	// retrieve CDB card list
	//------------------------------------
	SaXProcess* CDBCards = new SaXProcess ();
	CDBCards -> start (CDB_CARDS);
	Q3Dict< Q3Dict<QString> > CDBData = CDBCards -> getTablePointerCDB ();
	Q3Dict<QString>* cardData = CDBData.find ( mCardName );
	if ( cardData ) {
		QString* profile = cardData -> find ("Profile");
		if ((profile) && (profile->contains("DualHead_DriverOptions"))) {
			//====================================
			// check if drivers do match...
			//------------------------------------
			QString syspDriver = pCard -> getItem ("Module");
			QString* CDB2DDriver = cardData -> find ("Driver");
			QString* CDB3DDriver = cardData -> find ("3DDriver");
			if (
				((CDB2DDriver) && (syspDriver != *CDB2DDriver)) &&
				((CDB3DDriver) && (syspDriver != *CDB2DDriver))
			) {
				*profile = getDriverOptionsDualHeadProfile ( syspDriver );
			}
			if (profile->isEmpty()) {
				return false;
			}
			//====================================
			// ask profile for changes if dynamic
			//------------------------------------
			SaXProcessCall* proc = new SaXProcessCall ();
			proc -> addArgument ( SAX_PROFILE_CHECK );
			proc -> addArgument ( *profile );
			if ( ! proc -> start() ) {
				excProcessFailed();
				qError (errorString(),EXC_PROCESSFAILED);
				return false;
			}
			if (proc->exitStatus() == 0) {
				return true;
			}
			return false;
		}
		return false;
	}
	//====================================
	// No CDB record found for cardName
	//------------------------------------
	excEmptyCDBGroup ( mCardName.latin1() );
	qError (errorString(),EXC_EMPTYCDBGROUP);
	return false;
}

//====================================
// enable3D
//------------------------------------
bool SaXManipulateDesktop::enable3D (void) {
	// .../
	//! enable 3D system. This method will include the needed
	//! extension module to activate 3D. It is necessary to restart
	//! the X-Server after the configuration has been written. The
	//! method will take care about the special nvidia setup which
	//! requires the nvidia installer to uninstall -> disable or
	//! install -> enable the 3D subsystem. NOTE: libsax will not
	//! install or uninstall anything but an exception will be throwed
	//! if the binary nvidia driver is not installed
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (! pCard -> setID ( mDesktopID )) {
		return false;
	}
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");

	//====================================
	// retrieve CDB card list
	//------------------------------------
	SaXProcess* CDBCards = new SaXProcess ();
	CDBCards -> start (CDB_CARDS);
	Q3Dict< Q3Dict<QString> > CDBData = CDBCards -> getTablePointerCDB ();

	//====================================
	// get Extensions for the active card
	//------------------------------------
	SaXManipulateCard cardInfo (mCard,mDesktopID);
	Q3Dict<QString>* cardData = CDBData.find ( mCardName );
	if ( cardData ) {
		QString* driver3D= cardData -> find ("3DDriver");
		QString* driver2D= cardData -> find ("Driver");
		QString* extends = cardData -> find ("Extension");
		QString* flag    = cardData -> find ("Flag");
		//========================================
		// check 3D flag
		//----------------------------------------
		if (! flag) {
			return false;
		}		
		//========================================
		// nvidia drv. needed, check if installed
		//----------------------------------------
		if ((driver3D) && (*driver3D == "nvidia")) {
			bool foundBinaryNVidiaDriver = false;
			if ((getVendorForDriver(*driver3D)) == "NVIDIA Corporation") {
				foundBinaryNVidiaDriver = true;
			}
			if (! foundBinaryNVidiaDriver) {
				excNvidiaDriverMissing();
				qError (errorString(),EXC_NVIDIADRIVERMISSING);
				return false;
			}
		}
		//========================================
		// have extension, add it
		//----------------------------------------
		if (extends) {
			SaXManipulatePath pathInfo (mPath);
			pathInfo.removeDisableModule (*extends);
			pathInfo.removeLoadableModule (*extends);
			pathInfo.addLoadableModule (*extends);
		}
		//========================================
		// set driver to use with 3D
		//----------------------------------------
		if (driver3D) {
			QString currentDriver = cardInfo.getCardDriver();
			if ((currentDriver != *driver3D) && (currentDriver != *driver2D)) {
				excDriverMismatch (driver3D->ascii(),currentDriver.ascii());
				qError (errorString(),EXC_DRIVERMISMATCH);
			} else {
				cardInfo.setCardDriver (*driver3D);
			}
		}
	}
	//====================================
	// Import profiles if there are any
	//------------------------------------
	if (cardData) {
		QStringList profiles;
		QString* profile3D = cardData -> find ("3DProfile");
		QString* driver3D  = cardData -> find ("3DDriver");
		QString* driver2D  = cardData -> find ("Driver");
		if ((profile3D) && (! profile3D->contains("DualHead"))) {
			profiles += *profile3D;
		}
		if ((driver3D) && (*driver2D != *driver3D)) {
			SaXFile mapHandle (MAP_DIR + QString("Driver.map"));
			Q3Dict<QString> driverMap = mapHandle.readDict();
			QString driver = cardInfo.getCardDriver();
			if ((! driver.isEmpty()) && (driverMap[driver])) {
				QStringList items = QStringList::split ( ",", *driverMap[driver] );
				profiles += items;
			}
		}
		for (
			QStringList::Iterator it=profiles.begin();
			it != profiles.end(); ++it
		) {
			QString profile (*it);
			SaXImportProfile* pProfile = new SaXImportProfile (
				PROFILE_DIR + profile
			);
			pProfile -> doImport();
			//====================================
			// handle Card Options
			//------------------------------------
			SaXImport* mImportCard = pProfile -> getImport ( SAX_CARD );
			if ( mImportCard ) {
				Q3Dict<QString> profileDriverOptions;
				SaXManipulateCard saxProfileCard ( mImportCard );
				profileDriverOptions = saxProfileCard.getOptions();
				Q3DictIterator<QString> it ( profileDriverOptions );
				for (; it.current(); ++it) {
					QString key = it.currentKey();
					QString val = *it.current();
					cardInfo.addCardOption (key,val);
				}
			}
			//====================================
			// handle Desktop Options
			//------------------------------------
			SaXImport* mImportDesktop = pProfile -> getImport ( SAX_DESKTOP );
			if ( mImportDesktop ) {
				Q3Dict<QString> profileDriverOptions;
				SaXManipulateDesktop saxProfileDesktop (
					mImportDesktop,mCard,mPath
				);
				//====================================
				// Colordepth...
				//------------------------------------
				QString color = saxProfileDesktop.getColorDepth();
				if (! color.isEmpty()) {
					setColorDepth (color.toInt());
				}
			}
		}
		return true;
	}
	//====================================
	// No CDB record found for mCardName
	//------------------------------------
	excEmptyCDBGroup ( mCardName.latin1() );
	qError (errorString(),EXC_EMPTYCDBGROUP);
	return false;
}

//====================================
// disable3D
//------------------------------------
bool SaXManipulateDesktop::disable3D (void) {
	// ....
	//! remove the needed extension module(s) related to the
	//! 3D subsystem. This method will handle the special nvidia
	//! case as well. An exception will be throwed if the nvidia
	//! driver is installed and needs to get uninstalled to
	//! deactivate 3D in this case
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (! pCard -> setID ( mDesktopID )) {
		return false;
	}
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");

	//====================================
	// retrieve CDB card list
	//------------------------------------
	SaXProcess* CDBCards = new SaXProcess ();
	CDBCards -> start (CDB_CARDS);
	Q3Dict< Q3Dict<QString> > CDBData = CDBCards -> getTablePointerCDB ();

	//====================================
	// get Extensions for the active card
	//------------------------------------
	SaXManipulateCard cardInfo (mCard);
	Q3Dict<QString>* cardData = CDBData.find ( mCardName );
	if ( cardData ) {
		QString* driver3D= cardData -> find ("3DDriver");
		QString* driver2D= cardData -> find ("Driver");
		QString* extends = cardData -> find ("Extension");
		QString* flag    = cardData -> find ("Flag");
		//========================================
		// check 3D flag
		//----------------------------------------
		if (! flag) {
			return false;
		}
		//========================================
		// nvidia drv. needed, check if installed
		//----------------------------------------
		if ((driver3D) && (*driver3D == "nvidia")) {
			if ((getVendorForDriver(*driver3D)) == "NVIDIA Corporation") {
				excNvidiaDriverInstalled();
				qError (errorString(),EXC_NVIDIADRIVERINSTALLED);
				return false;
			}
		}
		//========================================
		// have extension, remove it
		//----------------------------------------
		if (extends) {
			SaXManipulatePath pathInfo (mPath);
			pathInfo.removeLoadableModule (*extends);
			pathInfo.removeDisableModule (*extends);
			pathInfo.addDisableModule (*extends);
		}
		//========================================
		// set driver to use with 2D
		//----------------------------------------
		if ((driver2D) && (driver3D)) {
			QString currentDriver = cardInfo.getCardDriver();
			if ((currentDriver != *driver3D) && (currentDriver != *driver2D)) {
				excDriverMismatch (driver2D->ascii(),currentDriver.ascii());
				qError (errorString(),EXC_DRIVERMISMATCH);
			} else {
				cardInfo.setCardDriver (*driver2D);
			}
		}
	}
	//====================================
	// Reset profiles if there are any
	//------------------------------------
	if (cardData) {
		QStringList profiles;
		QString* profile3D = cardData -> find ("3DProfile");
		QString* driver3D  = cardData -> find ("3DDriver");
		QString* driver2D  = cardData -> find ("Driver");
		if ((profile3D) && (! profile3D->contains("DualHead"))) {
			profiles += *profile3D;
		}
		if ((driver3D) && (*driver2D != *driver3D)) {
			SaXFile mapHandle (MAP_DIR + QString("Driver.map"));
			Q3Dict<QString> driverMap = mapHandle.readDict();
			if ((driver3D) && (driverMap[*driver3D])) {
				QStringList items = QStringList::split ( ",",*driverMap[*driver3D] );
				profiles += items;
			}
		}
		for (
			QStringList::Iterator it=profiles.begin();
			it != profiles.end(); ++it
		) {
			QString profile (*it);
			SaXImportProfile* pProfile = new SaXImportProfile (
				PROFILE_DIR + profile
			);
			pProfile -> doImport();
			//====================================
			// remove Card Options
			//------------------------------------
			SaXImport* mImportCard = pProfile -> getImport ( SAX_CARD );
			if ( mImportCard ) {
				Q3Dict<QString> profileDriverOptions;
				SaXManipulateCard saxProfileCard ( mImportCard );
				profileDriverOptions = saxProfileCard.getOptions();
				Q3DictIterator<QString> it ( profileDriverOptions );
				for (; it.current(); ++it) {
					QString key = it.currentKey();
					cardInfo.removeCardOption (key);
				}
			}
		}
		return true;
	}
	//====================================
	// No CDB record found for mCardName
	//------------------------------------
	excEmptyCDBGroup ( mCardName.latin1() );
	qError (errorString(),EXC_EMPTYCDBGROUP);
	return false;
}

//====================================
// setDisplaySize
//------------------------------------
void SaXManipulateDesktop::setDisplaySize (int width,int height) {
	// .../
	//! set the DisplaySize in [mm] for the width and height
	//! of the currently selected display
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString size;
	size.sprintf ("%d %d",width,height);
	mDesktop -> setItem ( "DisplaySize",size );
}

//====================================
// setDisplayRatioAndTraversal
//------------------------------------
void SaXManipulateDesktop::setDisplayRatioAndTraversal (
	double traversal,int aspect, int ratio
) {
	// .../
	//! set the DisplaySize in [mm] for the traversal and ratio
	//! of the currently selected display
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	traversal = traversal * 25.4;
	double ar = (double)aspect / (double)ratio;
	double y = sqrt ( (traversal * traversal) / (ar * ar + 1.0) );
	double x = ar * y;
	setDisplaySize (
		(int)(round(x)),(int)(round(y))
	);
}

//====================================
// setHsyncRange
//------------------------------------
void SaXManipulateDesktop::setHsyncRange (double hsmin,double hsmax) {
	// .../
	//! set the horizontal sync range in [Khz] of the currently
	//! selected display.
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString range;
	range.sprintf ("%.0f-%.0f",hsmin,hsmax);
	mDesktop -> setItem ( "HorizSync",range );
}

//====================================
// setVsyncRange
//------------------------------------
void SaXManipulateDesktop::setVsyncRange (double vsmin,double vsmax) {
	// .../
	//! set the vertical sync range in [Hz] of the currently
	//! selected display.
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString range;
	range.sprintf ("%.0f-%.0f",vsmin,vsmax);
	mDesktop -> setItem ( "VertRefresh",range );
}

//====================================
// setPreferredMode
//------------------------------------
void SaXManipulateDesktop::setPreferredMode (const QString& mode) {
	// .../
	//! set the PreferredMode which is required because randr12
	//! based drivers will ignore any Modes setup, great
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	mDesktop -> setItem ( "PreferredMode",mode);
}

//====================================
// enableDPMS
//------------------------------------
void SaXManipulateDesktop::enableDPMS (void) {
	// .../
	//! enable monitor option DPMS, currently there is only
	//! one monitor option so we will set the value and overwrite
	//! the current setting
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
    }
	mDesktop -> setItem ( "MonitorOptions","DPMS" );
}

//====================================
// disableDPMS
//------------------------------------
void SaXManipulateDesktop::disableDPMS (void) {
	// .../
	//! disable monitor option DPMS by removing the complete
	//! entry. This works because there is only one option available
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	mDesktop -> setItem ( "MonitorOptions","" );
}

//====================================
// setMonitorVendor
//------------------------------------
void SaXManipulateDesktop::setMonitorVendor (const QString& vendor) {
	// .../
	//! set monitor vendor name. This method will only set
	//! the vendor string to identify the monitor
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	mDesktop -> setItem ( "VendorName",vendor );
}

//====================================
// setMonitorName
//------------------------------------
void SaXManipulateDesktop::setMonitorName (const QString& name) {
	// .../
	//! set the monitor model name. This method will only set
	//! the model string to identify the monitor
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	mDesktop -> setItem ( "ModelName",name );
}

//====================================
// is3DEnabled
//------------------------------------
bool SaXManipulateDesktop::is3DEnabled (void) {
	// .../
	//! check if 3D is enabled. first we will have a look at
	//! the driver used. In case of nvidia 3D is always enabled
	//! in any other case the dri module must be used to enable
	//! the 3D subsystem
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	if ( mCard -> getCount() > 1 ) {
		return false;
	}
	QString driver = mCard -> getItem ("Driver");
	if (driver == "nvidia") {
		QString vendor = getVendorForDriver ( driver );
		if (vendor == "NVIDIA Corporation") {
			return true;
		}
		return false;
	}
	SaXManipulatePath pathInfo (mPath);
	QList<QString> modules = pathInfo.getModules();
	QString it;
	foreach (it,modules) {
	if (it == "dri") {
		return true;
	}
	}
	return false;
}

//====================================
// getResolutions
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutions (int color) {
	// .../
	//! return a list of resolutions refering to the given
	//! color depth. If there is no resolution defined for the given
	//! color an empty list is returned
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString modes;
	modes.sprintf ("Modes:%d",color);
	QString resolutions = mDesktop -> getItem (modes);
	if (resolutions.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result;
	QStringList resList = QStringList::split ( ",", resolutions );
	for (QStringList::Iterator it=resList.begin(); it!=resList.end();++ it) {
		result.append (*it);
	}
	return result;
}

//====================================
// getResolutionsFromServer
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutionFromServer ( void ) {
	// .../
	//! return the resolution the currently running server is using
	//! This call will only work if there is a server which can be
	//! asked for the resolution. The xquery -r call is used to obtain
	//! the resolution
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> defaultList;
	defaultList.append (QString("800x600"));
	SaXProcessCall* proc = new SaXProcessCall();
	proc -> addArgument ( XQUERY );
	proc -> addArgument ( "-r" );
	if ( ! proc -> start() ) {
		return defaultList;
	}
	QList<QString> data = proc->readStdout();
	QString in;
	foreach (in,data) {
		QRegExp modeExp ("(\\d+) (.*)");
		int rpos = modeExp.search (in,0);
		if (rpos >= 0) {
			int id = modeExp.cap(1).toInt();
			if (id == mDesktopID) {
				QList<QString> result;
				result.append (QString (modeExp.cap(2)));
				return result;
			}
		}
	}
	return defaultList;
}

//====================================
// getColorDepthFromServer
//------------------------------------
int SaXManipulateDesktop::getColorDepthFromServer (void) {
	Display* dpy = XOpenDisplay (0);
	return DisplayPlanes( dpy,0 );
}

//====================================
// getFBKernelMode
//------------------------------------
int SaXManipulateDesktop::getFBKernelMode (const QString& res,int depth) {
	if (! mSyspDesktop) {
		mSyspDesktop = new SaXImportSysp (SYSP_DESKTOP);
	}
	mSyspDesktop -> setID    ( mDesktopID );
	mSyspDesktop -> doImport ();
	if ( mSyspDesktop->getItem("FBBoot").isNull()) {
		return 0;
	}
	QString val = mSyspDesktop->getItem("FBBoot");
	QStringList items = QStringList::split ( ",", val );
	for (
		QStringList::Iterator it=items.begin();
		it != items.end(); ++it
	) {
		QString* resolution = new QString();
		QString mode (*it);
		QStringList record = QStringList::split ( " ", mode );
		QTextOStream (resolution) << record.at(0) << "x" << record.at(1);
		QString color = record.at(2);
		if ((*resolution == res) && (color.toInt() == depth)) {
			QString mode = record.at(3);
			return mode.toInt();
		}
	}
	return 0;
}

//====================================
// setFBKernelMode
//------------------------------------
int SaXManipulateDesktop::setFBKernelMode (int mode) {
	QString* modeString = new QString();
	QTextOStream (modeString) << mode;
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( SAX_GRUB_UPDATE );
	proc -> addArgument ( *modeString );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		return false;
	}
	if (proc->exitStatus() == 0) {
		return true;
	}
	return false;
}

//====================================
// getResolutionsFromDDC
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutionsFromDDC (const QString& key) {
	QList<QString> result;
	Q3Dict<QString> hashed;
	SaXImportSysp* desktop = new SaXImportSysp (SYSP_DESKTOP);
	desktop -> setID    ( mDesktopID );
	desktop -> doImport ();
	if ( desktop->getItem(key).isNull()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	if ((key != "FBBoot") && (desktop->getItem("DDC") == "<undefined>")) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString val = desktop->getItem(key);
	QStringList items = QStringList::split ( ",", val );
	for (
		QStringList::Iterator it=items.begin();
		it != items.end(); ++it
	) {
		QString* data = new QString();
		QString mode (*it);
		QStringList resolution = QStringList::split (" ", mode);
		QTextOStream (data) << resolution.at(0) << "x" << resolution.at(1);
		if (! hashed[*data]) {
			hashed.insert (*data,data);
		}
	}
	QString* default1 = new QString ("800x600");
	QString* default2 = new QString ("1024x768");
	hashed.insert (*default1,default1);
	hashed.insert (*default2,default2);
	Q3DictIterator<QString> ir (hashed);
	for (; ir.current(); ++ir) {
		result.append (*ir.current());
	}
	return result;
}

//====================================
// getResolutionsFromDDC1
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutionsFromDDC1 (void) {
	return getResolutionsFromDDC ("Vesa");
}

//====================================
// getResolutionsFromDDC2
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutionsFromDDC2 (void) {
	return getResolutionsFromDDC ("Vesa[2]");
}

//====================================
// getResolutionsFromFrameBuffer
//------------------------------------
QList<QString> SaXManipulateDesktop::getResolutionsFromFrameBuffer (void) {
	return getResolutionsFromDDC ("FBBoot");
}

//====================================
// getDisplaySize
//------------------------------------
QList<QString> SaXManipulateDesktop::getDisplaySize (void) {
	// .../
	//! get the values for the display size wheras the
	//! first entry in the list refers to the X value in mm
	//! and the second value refers to the Y value in mm. If
	//! there is no display size defined an empty list is
	//! returned
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString size = mDesktop -> getItem ("DisplaySize");
	if (size.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result;
	QStringList sizeList = QStringList::split ( " ", size );
	result.append (
		QString(sizeList.first())
	);
	result.append (
		QString(sizeList.last())
	);
	return result;
}

//====================================
// getDisplayTraversal
//------------------------------------
QString SaXManipulateDesktop::getDisplayTraversal (void) {
	// .../
	//! get the traversal length according to the information
	//! from the getDisplaySize() method
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	QList<QString> size = getDisplaySize();
	if (size.isEmpty()) {
		QString* nope = new QString;
		return *nope;
	}
	int x = size.at(0).toInt();
	int y = size.at(1).toInt();
	double traversal = sqrt (x*x + y*y) / 25.4;
	QString result;
	QTextOStream (&result) << traversal;
	return result;
}

//====================================
// getDisplayRatio
//------------------------------------
QList<QString> SaXManipulateDesktop::getDisplayRatio (void) {
	// .../
	//! get the aspect ratio x/y values according to the
	//! information from the getDisplaySize() method
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString* setX = new QString ("4");
	QString* setY = new QString ("3");
	QList<QString> result;
	QList<QString> size = getDisplaySize();
	if (size.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	int x = size.at(0).toInt();
	int y = size.at(1).toInt();
	double ar = (double)x / (double)y;
	if ( ar > 1.4 ) {
		*setX = "16";
		*setY = "10";
	} else if (( ar <= 1.4 ) && ( ar > 1.3 )) {
		*setX = "4";
		*setY = "3";
	} else if ( ar <= 1.3 ) {
		*setX = "5";
		*setY = "4";
	}
	result.append (*setX);
	result.append (*setY);
	return result;
}

//====================================
// getHsyncRange
//------------------------------------
QList<QString> SaXManipulateDesktop::getHsyncRange (void) {
	// .../
	//! get the values for the horizontal sync range in Khz
	//! the first entry specify the start value and the second
	//! value specify the end of the range
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString range = mDesktop -> getItem ("HorizSync");
	if (range.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result;
	QStringList rangeList = QStringList::split ( "-", range );
	result.append (
		QString(rangeList.first())
	);
	result.append (
		QString(rangeList.last())
	);
	return result;
}

//====================================
// getVsyncRange
//------------------------------------
QList<QString> SaXManipulateDesktop::getVsyncRange (void) {
	// .../
	//! get the values for the vertical sync range in Khz
	//! the first entry specify the start value and the second
	//! value specify the end of the range
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString range = mDesktop -> getItem ("VertRefresh");
	if (range.isEmpty()) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> result;
	QStringList rangeList = QStringList::split ( "-", range );
	result.append (
		QString(rangeList.first())
	);
	result.append (
		QString(rangeList.last())
	);
	return result;
}

//====================================
// DPMSEnabled
//------------------------------------
bool SaXManipulateDesktop::DPMSEnabled (void) {
	// .../
	//! check if DPMS is enabled by checking the value for
	//! MonitorOptions. Currently there is only one monitor
	//! option available so the check is rather simple
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	QString options = mDesktop -> getItem ("MonitorOptions");
	if (options == "DPMS") {
		return true;
	}
	return false;
}

//====================================
// getMonitorVendor
//------------------------------------
QString SaXManipulateDesktop::getMonitorVendor (void) {
	// .../
	//! get the name of the monitor vendor for the
	//! currently selected desktop
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	return mDesktop -> getItem ("VendorName");
}

//====================================
// getMonitorName
//------------------------------------
QString SaXManipulateDesktop::getMonitorName (void) {
	// .../
	//! get the name of the monitor model for the
	//! currently selected desktop
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	return mDesktop -> getItem ("ModelName");
}

//====================================
// getColorDepth
//------------------------------------
QString SaXManipulateDesktop::getColorDepth (void) {
	// .../
	//! get the currently set default color depth for
	//! the active desktop
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	return mDesktop -> getItem ("ColorDepth");
}

//====================================
// getModelineAlgorithm
//------------------------------------
QString SaXManipulateDesktop::getModelineAlgorithm (void) {
	// .../
	//! get the currently used modeline algorithm method
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	return mDesktop -> getItem ("CalcAlgorithm");
}

//====================================
// willCalculateModelines
//------------------------------------
bool SaXManipulateDesktop::willCalculateModelines (void) {
	// .../
	//! check if the modeline calculation is enabled or not
	//! The method will return true if enabled otherwise false
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return false;
	}
	QString calculate = mDesktop -> getItem ("CalcModelines");
	if ((calculate == "on") || (calculate == "yes")) {
		return true;
	}
	return false;
}

//====================================
//getVirtualResolution
//------------------------------------
QString SaXManipulateDesktop::getVirtualResolution (int color) {
	// .../
	//! get the virtual resolution if defined for the given
	//! color depth. If there is no virtual resolution a (null)
	//! string is returned
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		QString* nope = new QString;
		return *nope;
	}
	QString key;
	QString val;
	key.sprintf ("Virtual:%d",color);
	val = mDesktop -> getItem (key);
	if (val.isEmpty()) {
		QString* nope = new QString;
		return *nope;
	}
	QStringList resList = QStringList::split ( " ", val );
	QString result (resList.join("x"));
	return result;
}

//====================================
// set monitor record from the CDB
//------------------------------------
void SaXManipulateDesktop::setCDBMonitor ( const QString& group ) {
	// .../
	//! set all monitor data associated with the given group name to
	//! the current desktop data. The group name consists of the
	//! vendor and model name separated by a colon
	// ----
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBMonitors -> getTablePointerCDB_DATA (group);
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return;
	}
	QStringList nameList = QStringList::split ( ":", group );
	setMonitorVendor ( nameList.first() );
	setMonitorName	 ( nameList.last() );
	mDesktop -> merge ( data );
}

//====================================
// get list of CDB monitors
//------------------------------------
QList<QString> SaXManipulateDesktop::getCDBMonitorVendorList ( void ) {
	// .../
	//! retrieve a list of supported monitor vendors. Each item
	//! contains the vendor name as it is stored in the CDB
	// ----
	mCDBMonitorList.clear();
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMonitors -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		QStringList vnlist = QStringList::split ( ":", it.currentKey() );
		QString* vendorName = new QString (vnlist.first());
		int hasVendor = false;
		QString io;
		foreach(io,mCDBMonitorList) {
		if (io == *vendorName ) {
			hasVendor = true;
			break;
		}
		}
		if (! hasVendor ) {
			mCDBMonitorList.append ( *vendorName );
		}
	}
	return mCDBMonitorList;
}

//====================================
// get CDB monitor models per vendor
//------------------------------------
QList<QString> SaXManipulateDesktop::getCDBMonitorModelList (
	const QString& vendor
) {
	// .../
	//! retrieve a list of supported monitor models. Each item
	//! contains the vendor name as it is stored in the CDB
	// ----
	mCDBMonitorList.clear();
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMonitors -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		QStringList vnlist = QStringList::split ( ":", it.currentKey() );
		QString vendorName = vnlist.first();
		QString* modelName = new QString (vnlist.last());
		if ( vendorName == vendor ) {
			mCDBMonitorList.append ( *modelName );
		}
	}
	return mCDBMonitorList;
}

//====================================
// get data dict for a CDB monitor
//------------------------------------
Q3Dict<QString> SaXManipulateDesktop::getCDBMonitorData (
	const QString& vendor, const QString& name
) {
	// .../
	//! return the monitor data dictionary associated with the
	//! given vendor and model name.
	// ----
	QString key;
	QTextOStream (&key) << vendor << ":" << name;
	mCDBMonitorData.clear();
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMonitors -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		if ( it.currentKey() == key ) {
			mCDBMonitorData = *it.current();
			break;
		}
	}
	return mCDBMonitorData;
}
//====================================
// get data dict for a CDB monitor
//------------------------------------
Q3Dict<QString> SaXManipulateDesktop::getCDBMonitorIDData (
	const QString& id
) {
	// .../
	//! return the monitor data dictionary associated with the
	//! given DDC id.
	// ----
	mCDBMonitorData.clear();
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMonitors -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	bool found = false;
	for (; it.current(); ++it) {
		mCDBMonitorData = *it.current();
		if ((mCDBMonitorData["DDC"]) && (*mCDBMonitorData["DDC"] == id)) {
			mCDBMonitorData.insert ("Name",new QString(it.currentKey()));
			found = true;
			break;
		}
	}
	if (! found) {
		mCDBMonitorData.clear();
	}
	return mCDBMonitorData;
}
//====================================
// add data dict to CDB (temporarly)
//------------------------------------
void SaXManipulateDesktop::setCDBMonitorData (
	const QString& vendor, const QString& name,
	const QString& key, const QString& value
) {
	// .../
	//! add a CDB group to the current CDB monitor table of
	//! this manipulator instance. Note the information is not
	//! part of the CDB directly it is only stored temporarly
	//! as long as the object instance exists
	// ----
	QString group;
	QTextOStream (&group) << vendor << ":" << name;
	if ( ! mCDBMonitors ) {
		mCDBMonitors = new SaXProcess ();
		mCDBMonitors -> start (CDB_MONITORS);
	}
	mCDBMonitors -> addGroup (
		group,key,value
	);
}
//====================================
// getVendorForDriver
//------------------------------------
QString SaXManipulateDesktop::getVendorForDriver ( const QString& driver ) {
	// .../
	//! obtain the SYSP vendor string referring to the given
	//! driver name
	// ----
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( SYSP_VENDOR );
	proc -> addArgument ( driver );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
	}
	QList<QString> data = proc -> readStdout();
	return data.first();
}
//====================================
// setExtraModelineString
//------------------------------------
void SaXManipulateDesktop::setExtraModelineString (
	const QString & mode
) {
	// .../
	//! This method includes one modeline specified by mode
	//! without checking if it fits into the current range.
	// This method should be used carefully
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	mDesktop -> setItem ( "SpecialModeline",mode );
}
//====================================
// setExtraModeline
//------------------------------------
void SaXManipulateDesktop::setExtraModeline (
	int x,int y,int refresh,int hsync
) {
	// .../
	//! This method includes one modeline XxY with refresh Hz but without
	//! checking if it fits into the current range. This method should
	//! be used carefully
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString mode = calculateModeline ( x,y,refresh,hsync );
	mDesktop -> setItem ( "SpecialModeline",mode );
}
//====================================
// addExtraModeline
//------------------------------------
void SaXManipulateDesktop::addExtraModeline (
	int x,int y,int refresh,int hsync
) {
	// .../
	//! This method add a modeline XxY with refresh Hz but without
	//! checking if it fits into the current range. This method should
	//! be used carefully
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString val;
	QString key = "SpecialModeline";
	QString mode = calculateModeline ( x,y,refresh,hsync );
	if (! mDesktop -> getItem (key).isEmpty()) {
		val = mDesktop -> getItem (key);
	}
	QTextOStream (&val) << "," << mode;
	val.replace (QRegExp("^,"),"");
	mDesktop -> setItem ( key,val );
}
//====================================
// removeExtraModeline
//------------------------------------
void SaXManipulateDesktop::removeExtraModeline ( int x,int y) {
	// .../
	//! This function removes a modeline XxY from the
	//! SpecialModeline value
	// ----
	if ((! mDesktop) || (! mCard) || (! mPath)) {
		return;
	}
	QString val;
	QString key = "SpecialModeline";
	val.sprintf ("\"%dx%d\"",x,y);
	QString current = mDesktop -> getItem (key);
	QStringList lines = QStringList::split ( ",", current );
	for (
		QStringList::Iterator it=lines.begin();
		it != lines.end(); ++it
	) {
		QString mode (*it);
		if (mode.contains (val)) {
			mDesktop -> removeItem (key,mode);
		}
	}
}
//====================================
// calculateModeline
//------------------------------------
QString SaXManipulateDesktop::calculateModeline (
	int x,int y,int refresh,int hsync
) {
	// .../
	//! This function calculates a modeline which fits
	//! into refresh and hsync whereas the line is bound
	//! to the refresh value
	// ----
	QString result;
	for (int r=refresh;r >= 50;r--) {
		SaXProcessCall* proc = new SaXProcessCall ();
		proc -> addArgument ( XMODE );
		proc -> addArgument ( "-x" );
		proc -> addArgument ( x );
		proc -> addArgument ( "-y" );
		proc -> addArgument ( y );
		proc -> addArgument ( "-r" );
		proc -> addArgument ( r + 2 );
		if ( ! proc -> start() ) {
			excProcessFailed();
			qError (errorString(),EXC_PROCESSFAILED);
		}
		QList<QString> data = proc->readStdout();
		int hs = data.at(0).toInt();
		result = *data.at(2);
		if (hs <= hsync) {
			break;
		}
	}
	result.replace (QRegExp("^Modeline "),"");
	return result;
}
//====================================
// getDriverOptionsDualHeadProfile
//------------------------------------
QString SaXManipulateDesktop::getDriverOptionsDualHeadProfile (
	const QString& driver
) {
	// .../
	//! This private function assigns a DriverOptions profile
	//! to the current driver. If there is no mapping possible
	//! an empty string is returned
	// ----
	QString result;
	if ((driver == "i810") || (driver == "i915")) {
		QTextOStream (&result)
			<< PROFILE_DIR << "Intel_DualHead_DriverOptions";
	}
	if (driver == "intel") {
		QTextOStream (&result)
			<< PROFILE_DIR << "IntelNext_DualHead_DriverOptions";
	}
	if (driver == "nvidia") {
		QTextOStream (&result)
			<< PROFILE_DIR << "NVidia_DualHead_DriverOptions";
	}
	if (driver == "radeonhd") {
		QTextOStream (&result)
			<< PROFILE_DIR << "RadeonHD_DualHead_DriverOptions";
	}
	if (driver == "radeon") {
		QTextOStream (&result)
			<< PROFILE_DIR << "Radeon_DualHead_DriverOptions";
	}
	if (driver == "fglrx") {
		QTextOStream (&result)
			<< PROFILE_DIR << "FGLRX_DualHead_DriverOptions";
	}
	if (driver == "mga") {
		QTextOStream (&result)
			<< PROFILE_DIR << "Matrox_DualHead_DriverOptions";
	}
	if (driver == "sis") {
		QTextOStream (&result)
			<< PROFILE_DIR << "SiS_DualHead_DriverOptions";
	}
	return result;
}
//====================================
// getMetaData
//------------------------------------
Q3Dict<QString> SaXManipulateDesktop::getMetaData ( void ) {
	// .../
	//! This private function returns the dictionary asociated
	//! with the SaXMeta data used in profiles. The data is used
	//! internally only and affects the behaviour of the methods
	//! isDualHeadCard and getDualHeadProfile
	//! ----
	QString cardID;
	QTextOStream (&cardID) << mDesktopID;
	QList<const char*> metaOptions;
	metaOptions.append ((const char*) "-c" );
	metaOptions.append ( (const char*)cardID.latin1() );
	SaXProcess* proc = new SaXProcess ();
	proc->start ( metaOptions , SAX_META );
	Q3Dict<QString> metaData = proc->getCurrentTable();
	return metaData;
}
} // end namespace
