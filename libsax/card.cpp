/**************
FILE          : card.cpp
***************
PROJECT       : SaX2 - library interface
              :
AUTHOR        : Marcus Schäfer <ms@@suse.de>
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
#include "card.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulateCard::SaXManipulateCard (SaXImport* in, int card) {
	// .../
	//! An object of this type is used to manipulate the graphics
	//! card settings refering to the X11 Device sections. An
	//! import of type SAX_CARD is required with this class
	// ----
	mImport = 0;
	if ( ! in ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( in->getSectionID() != SAX_CARD ) {
		excCardImportBindFailed ( in->getSectionID() );
		qError (errorString(),EXC_CARDIMPORTBINDFAILED);
		return;
	}
	mImport = in;
	mCard   = card;
	mImport -> setID ( mCard );
	mCDBCardModules = 0;
}

//====================================
// set card ID
//------------------------------------
bool SaXManipulateCard::selectCard (int card) {
	// .../
	//! select the card device the changes should be applied to
	//! if the device does not exist (false) is returned
	// ----
	if (! mImport) {
		return false;
	}
	if (mImport -> setID ( card )) {
		mCard = card;
		return true;
	}
	return false;
}

//====================================
// setCardDriver
//------------------------------------
void SaXManipulateCard::setCardDriver (const QString& driver ) {
	// .../
	//! set the card driver to use with the selected device
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("Driver",driver);
}

//====================================
// addCardExternal
//------------------------------------
void SaXManipulateCard::addCardExternal ( const QString& ident ) {
	// .../
	//! add raw option value for SaXExternal which cause the
	//! system to add a Monitor section with the given identifier
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	Q3Dict<QString> options = getOptions();
	if ( options["SaXExternal"] ) {
		val = *options["SaXExternal"]+"+"+"Identifier&"+ident;
	} else {
		val = "Identifier&"+ident;
	}
	mImport->removeRawItem ("RawData","Option \"SaXExternal\"");
	mImport->addRawItem ("RawData","Option \"SaXExternal\"","\""+val+"\"");
}

//====================================
// addCardExternalOption
//------------------------------------
void SaXManipulateCard::addCardExternalOption (
	const QString& keyword, const QString& value
) {
	// .../
	//! add raw option for SaXExternal value. This includes
	//! the given key/value pair as option to the appropriate
	//! Monitor section
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	Q3Dict<QString> options = getOptions();
	if ( options["SaXExternal"] ) {
		val = *options["SaXExternal"]+"+"+keyword+"&"+value;
	} else {
		val = keyword+"&"+value;
	}
	mImport->removeRawItem ("RawData","Option \"SaXExternal\"");
	mImport->addRawItem ("RawData","Option \"SaXExternal\"","\""+val+"\"");
}

//====================================
// setCardOption
//------------------------------------
void SaXManipulateCard::setCardOption (
	const QString& keyword, const QString& value
) {
	// .../
	//! set an option to the graphics card used with the
	//! currently selected device
	// ----
	if (! mImport) {
		return;
	}
	QString key (keyword);
	QString val (value);
	if ( value.isNull()) {
		//====================================
		// set bool option
		//------------------------------------
		mImport->setItem ("Option",key);
	} else {
		//====================================
		// set value added option
		//------------------------------------
		mImport->setRawItem ("RawData","Option \""+key+"\"","\""+val+"\"");
	}
}

//====================================
// addCardOption 
//------------------------------------
void SaXManipulateCard::addCardOption (
	const QString& keyword, const QString& value
) {
	// .../
	//! add an option to the graphics card used with the
	//! currently selected device
	// ----
	if (! mImport) {
		return;
	}
	QString key (keyword);
	QString val (value);
	if ( value.isNull()) {
		//====================================
		// add bool option
		//------------------------------------
		mImport->addItem ("Option",key);
	} else {
		//====================================
		// add value added option
		//------------------------------------
		mImport->addRawItem ("RawData","Option \""+key+"\"","\""+val+"\"");
	}
}

//====================================
// removeCardOption
//------------------------------------
void SaXManipulateCard::removeCardOption (const QString& value) {
	// .../
	//! removes an option which has been set to the currently
	//! selected device. If the option does not exist nothing
	//! will happen
	// ----
	if (! mImport) {
		return;
	}
	QString val (value);
	QString str = mImport->getItem("RawData");
	if (! str.contains ("\""+val+"\"")) {
		//====================================
		// remove bool option
		//------------------------------------
		mImport->removeItem ("Option",val);
	} else {
		//====================================
		// remove value added option
		//------------------------------------
		mImport->removeRawItem ("RawData","Option \""+val+"\"");
	}
}

//====================================
// setBusID
//------------------------------------
void SaXManipulateCard::setBusID  (const QString& busid) {
	// .../
	//! set busID location for the selected graphics device
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("BusID",busid);
}

//====================================
// setScreenID
//------------------------------------
void SaXManipulateCard::setScreenID (int screen) {
	// .../
	//! set screen identification for the selected graphics device
	//! this is only useful for multihead cards to separate the
	//! heads
	// ----
	if (! mImport) {
		return;
	}
	QString screenID; screenID.sprintf ("%d",screen);
	mImport -> setItem ("Screen",screenID);
}

//====================================
// setRotate
//------------------------------------
void SaXManipulateCard::setRotate (int direction) {
	// .../
	//! set rotation direction for the selected graphics device
	//! rotation works only for drivers which supports this
	// ----
	if (! mImport) {
		return;
	}
	if (direction == SAX_ROTATE_LEFT) {
		mImport -> setItem ("Rotate","CCW");
	}
	if (direction == SAX_ROTATE_RIGHT) {
		mImport -> setItem ("Rotate","CW");
	}
}

//====================================
// getCardName
//------------------------------------
QString SaXManipulateCard::getCardName (void) {
	// .../
	//! retrieve the card vendor and name as one string
	//! separated by a colon from the currently selected card
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	//====================================
	// search name in SYSP data
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (pCard -> setID ( mCard )) {
		QString mCardName;
		QTextOStream (&mCardName) <<
			pCard->getItem("Vendor") << ":" << pCard->getItem("Device");
		return mCardName;
	}
	//====================================
	// search name in ISAX data
	//------------------------------------
	QString mCardName;
	QTextOStream (&mCardName) <<
		mImport->getItem("Vendor") << ":" << mImport->getItem("Name");
	return mCardName;
}

//====================================
// getCardVendor
//------------------------------------
QString SaXManipulateCard::getCardVendor (void) {
	// .../
	//! retrieve the card vendor from the SYSP_CARD
	//! interface
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	//====================================
	// search vendor in SYSP data
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (pCard -> setID ( mCard )) {
		return pCard->getItem("Vendor");
	}
	//====================================
	// search vendor in ISAX data
	//------------------------------------
	return mImport->getItem("Vendor");
}

//====================================
// getCardModel
//------------------------------------
QString SaXManipulateCard::getCardModel (void) {
	// .../
	//! retrieve the card model from the SYSP_CARD
	//! interface
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	//====================================
	// search model in SYSP data
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (pCard -> setID ( mCard )) {
		return pCard->getItem("Device");
	}
	//====================================
	// search model in ISAX data
	//------------------------------------
	return mImport->getItem("Name");
}

//====================================
// getBusID
//------------------------------------
QString SaXManipulateCard::getBusID (void) {
	// .../
	//! retrieve the cards busID location from the currently
	//! selected card
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("BusID");
}

//====================================
// getCardDriver
//------------------------------------
QString SaXManipulateCard::getCardDriver (void) {
	// .../
	//! retrieve the cards driver name from the currently
	//! selected card
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Driver");
}

//====================================
// getScreenID
//------------------------------------
QString SaXManipulateCard::getScreenID (void) {
	// .../
	//! retrieve the card screen identification from the currently
	//! selected card. This value is always 0 for non multihead cards
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Screen");
}

//====================================
// getRotationDirection
//------------------------------------
QString SaXManipulateCard::getRotationDirection (void) {
	// .../
	//! retrieve the current value of the Rotate option
	//! if defined. If there is no rotate option given an
	//! empty QString is returned
	// ---- 
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Rotate");
}

//====================================
// getOptions
//------------------------------------
Q3Dict<QString> SaXManipulateCard::getOptions (void) {
	// .../
	//! retrieve an option list of all options set for the
	//! selected card. The storage is a dictionary saving the
	//! option name as key and the options value as value for
	//! this key. If there is no value set for the option
	//! (bool options) the value for the key is the (null) string
	// ----
	if (! mImport) {
		Q3Dict<QString>* nope = new Q3Dict<QString>;
		return *nope;
	}
	Q3Dict<QString> result;
	QString stdOptions = mImport -> getItem ("Option");
	QString rawOptions = mImport -> getItem ("RawData");
	//====================================
	// remove commented options from data
	//------------------------------------
	rawOptions.replace(
		QRegExp("#Option  \"(.*)\" \"(.*)\""),""
	);
	//====================================
	// handle bool options
	//------------------------------------
	QStringList stdlist = QStringList::split ( ",", stdOptions );
	for ( QStringList::Iterator
		in = stdlist.begin(); in != stdlist.end(); ++in
	) {
		QString item (*in);
		result.insert (item,new QString);
	}
	//====================================
	// handle value added options
	//------------------------------------
	QStringList rawlist = QStringList::split ( "Option", rawOptions );
	for ( QStringList::Iterator
		in = rawlist.begin(); in != rawlist.end(); ++in
	) {
		QString item (*in);
		QRegExp itemExp ("\"(.*)\" \"(.*)\"");
		int pos = itemExp.search (item);
		if (pos >= 0) {
			QString key = itemExp.cap(1);
			QString* val = new QString (itemExp.cap(2));
			result.insert (key,val);
		}
	}
	return result;
}

//====================================
// getCardDrivers
//------------------------------------
QList<QString> SaXManipulateCard::getCardDrivers ( void ) {
	// .../
	//! retrieve a list of X11 card driver names. The items
	//! if the list can be used to retrieve the coresponding
	//! option list using the getCardOptions() method
	// ----
	mCDBCardDrivers.clear();
	if ( ! mCDBCardModules ) {
		mCDBCardModules = new SaXProcess ();
		mCDBCardModules -> start (CDB_CARDMODULES);
	}
	Q3Dict< Q3Dict<QString> > CDBModules = mCDBCardModules->getTablePointerCDB();
	Q3DictIterator< Q3Dict<QString> > it (CDBModules);
	for (; it.current(); ++it) {
		mCDBCardDrivers.append (QString(it.currentKey()));
	}
	return mCDBCardDrivers;
}

//====================================
// getCardOptions
//------------------------------------
Q3Dict<QString> SaXManipulateCard::getCardOptions ( const QString& driver ) {
	// .../
	//! retrieve a list of card options for the given driver
	//! name (driver). A list of drivers can be obtained using the
	//! getCardDrivers() member method
	// ----
	mCDBCardOptions.clear();
	if ( ! mCDBCardModules ) {
		mCDBCardModules = new SaXProcess ();
		mCDBCardModules -> start (CDB_CARDMODULES);
	}
	Q3PtrList< Q3Dict<QString> > opts;
	opts = mCDBCardModules -> getTablePointerCDB_DATA (
		driver
	);
	if (opts.isEmpty()) {
		excCDBRecordNotFound (driver);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		Q3Dict<QString>* nope = new Q3Dict<QString>;
		return *nope;
	}
	Q3PtrList< Q3Dict<QString> > xaaopts;
	xaaopts = mCDBCardModules -> getTablePointerCDB_DATA (
		"xaa"
	);
	mCDBCardOptions = *opts.at(0);
	if (!xaaopts.isEmpty()) {
		Q3DictIterator<QString> it (*xaaopts.at(0));
		for (; it.current(); ++it) {
			mCDBCardOptions.insert (it.currentKey(),it.current());
		}
	}
	return mCDBCardOptions;
}

//====================================
// isNoteBook
//------------------------------------
bool SaXManipulateCard::isNoteBook ( void ) {
	// .../
	//! check if the graphics card is working in a NoteBook.
	//! The check is based on the battery count in /proc/acpi/battery/
	//! if there is a battery we will asumme a NoteBook
	// ----
	struct dirent* entry = NULL;
	DIR* batteryDir = NULL;
	batteryDir = opendir (ACPI_BATTERY);
	if (! batteryDir) {
		return false;
	}
	int BATs = 0;
	while (1) {
		entry = readdir (batteryDir);
		if (! entry) {
			break;
		}
		QString file (entry->d_name);
		if ((file == ".") || (file == "..")) {
			continue;
		}
		BATs++;
	}
	if (BATs == 0) {
		return false;
	}
	return true;
}

//====================================
// getCards
//------------------------------------
int SaXManipulateCard::getCards ( void ) {
	// .../
	//! returns the number of installed graphics cards. This
	//! refers to the real value of different graphics devices
	//! located on the BUS
	// ----
	if (! mImport) {
		return -1;
	}
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	if (!(pCard->getItem("Detected")).isNull()) {
		return pCard->getItem("Detected").toInt();
	}
	return -1;
}

//====================================
// getDevices
//------------------------------------
int SaXManipulateCard::getDevices ( void ) {
	// .../
	//! returns the number of configured device sections
	//! the result should be always the same as calling
	//! getHeads(), but getHeads will refer to the CDB data
	//! whereas getDevices is counting the included X11
	//! Device sections.
	// ----
	if (! mImport) {
		return -1;
	}
	return mImport->getCount();
}

//====================================
// getHeads
//------------------------------------
int SaXManipulateCard::getHeads ( void ) {
	// .../
	//! returns the number of configurable VGA heads This value may
	//! differ from the getCards() return value because there are
	//! possibly more than one head available on one card
	// ----
	if (! mImport) {
		return -1;
	}
	int mBoards  = getCards();
	int mDevices = mImport -> getCount();
	//====================================
	// check for NoteBooks, assume *2
	//------------------------------------
	if ( isNoteBook() ) {
		return mBoards * 2;
	}
	//====================================
	// get sysp card name
	//------------------------------------
	SaXImportSysp* pCard = new SaXImportSysp (SYSP_CARD);
	pCard -> doImport();
	QString mCardName;
	QTextOStream (&mCardName) <<
		pCard->getItem("Vendor") << ":" << pCard->getItem("Device");
	//====================================
	// import CDB DB for profile names
	//------------------------------------
	SaXProcess* pCDB = new SaXProcess();
	pCDB -> start ( CDB_CARDS );
	Q3Dict< Q3Dict<QString> > CDBData = pCDB -> getTablePointerCDB();
	//====================================
	// check profile names
	//------------------------------------
	int headCount = 0;
	int realCount = 0;
	int currentCard = mCard;
	while (1) {
		if (realCount >= mDevices) {
			selectCard (currentCard);
			return headCount;
		}
		selectCard (realCount);
		if ( CDBData[mCardName] ) {
			Q3Dict<QString> CDBTable = *CDBData[mCardName];
			if (CDBTable["Profile"]) {
				QString mProfile = *CDBTable["Profile"];
				if (mProfile == "NVidia_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "NVidia_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "Radeon_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "Radeon_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "FGLRX_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "FGLRX_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "Intel_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "Intel_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "IntelNext_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "IntelNext_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}	
				if (mProfile == "Matrox_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "Matrox_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
				if (mProfile == "SiS_DualHead_DriverOptions") {
					headCount += 2;
					realCount += 1;
					continue;
				}
				if (mProfile == "SiS_DualHead") {
					headCount += 2;
					realCount += 2;
					continue;
				}
			}
		}
		realCount++;
		headCount++;
	}
	//====================================
	// return board count, last chance
	//------------------------------------
	return mBoards;
}
} // end namespace
