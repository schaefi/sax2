/**************
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
#include "pointers.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulatePointers::SaXManipulatePointers (SaXImport* in, int ptr) {
	// .../
	//! This baseclass is used to provide common pointer
	//! manipulation methods for the devices Mice Tablets and Touchscreens.
	//! it provides a protected constructor only. Therefore it is not
	//! possible to create an object of this type
	// ----
	if ( ! in ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( in->getSectionID() != SAX_POINTERS ) {
		excPointerImportBindFailed ( in->getSectionID() );
		qError (errorString(),EXC_POINTERIMPORTBINDFAILED);
		return;
	}
	mImport  = in;
	mPointer = ptr;
	mImport -> setID ( mPointer );
}

//====================================
// set card ID
//------------------------------------
bool SaXManipulatePointers::selectPointer (int ptr) {
	// .../
	//! select the pointer device the changes should be applied to
	//! if the device does not exist (false) is returned
	// ----
	if (! mImport) {
		return false;
	}
	if (mImport -> setID ( ptr )) { 
		mPointer = ptr;
		return true;
	}
	return false;
}

//====================================
// setDriver
//------------------------------------
void SaXManipulatePointers::setDriver (const QString& driver) {
	// .../
	//! set global mouse driver to use with this configuration.
	//! the default mouse driver is named "mouse"
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "Driver",driver );
}

//====================================
// setDevice
//------------------------------------
void SaXManipulatePointers::setDevice (const QString& device) {
	// .../
	//! set pointer device which provide the raw data for
	//! the pointer device. Some drivers support some sort
	//! of autodetection in this case a keywoard instead of
	//! the /dev/... path can be used
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "Device",device );
}

//====================================
// setProtocol
//------------------------------------
void SaXManipulatePointers::setProtocol (const QString& protocol) {
	// .../
	//! set the protocol name for this pointer device
	//! some drivers support some sort of AUTO protocols
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ( "Protocol",protocol );
}

//====================================
// setVendor
//------------------------------------
void SaXManipulatePointers::setNamePair (
	const QString& vendor , const QString& name
) {
	// .../
	//! set a name tag to the configuration which consists of a
	//! vendor and a model name separated by a semicolon. xorg's
	//! config file does not provide native keywords for vendor or
	//! name tags, but they are needed to be able so select a
	//! component within a user interface.
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QTextOStream (&val) << vendor << ";" << name;
	mImport -> setItem ( "Name",val );
}

//====================================
// setOption
//------------------------------------
void SaXManipulatePointers::setOption (
	const QString& keyword, const QString& value
) {
	// .../
	//! set a pointer option to the currently selected
	//! pointer device. If the value is set to NULL the
	//! keyword "Option" is used otherwise "RawOption"
	//! will be set.
	// ----
	if (! mImport) {
		return;
	}
	QString key (keyword);
	QString val (value);
	if (value.isNull()) {
		//====================================
		// set bool option
		//------------------------------------
		mImport->setItem ("Option",key);
	} else {
		//====================================
		// set value added option
		//------------------------------------
		mImport->setRawItem ("RawOption",
			"\""+key+"\"","\""+val+"\""
		);
	}
}

//====================================
// addOption
//------------------------------------
void SaXManipulatePointers::addOption (
	const QString& keyword, const QString& value
) {
	// .../
	//! add a pointer option to the currently selected
	//! pointer device. If the value is set to NULL the
	//! "Option" list will be enhances otherwise the
	//! "RawOption" list will be increased.
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
		mImport->addRawItem ("RawOption",
			"\""+key+"\"","\""+val+"\""
		);
	}
}

//====================================
// removeOption
//------------------------------------
void SaXManipulatePointers::removeOption (const QString& value) {
	// .../
	//! remove a pointer option from the currently selected
	//! pointer device. If the value is set to NULL the
	//! value will be removed from the "Option" list otherwise
	//! the option is removed from the "RawOption" list 
	// ----
	if (! mImport) {
		return;
	}
	QString val (value);
	QString str = mImport->getItem("RawOption");
	if (! str.contains ("\""+val+"\"")) {
		//====================================
		// remove bool option
		//------------------------------------
		mImport->removeItem ("Option",val);
	} else {
		//====================================
		// remove value added option
		//------------------------------------
		mImport->removeRawItem ("RawOption","\""+val+"\"");
	}
}

//====================================
// getDevice
//------------------------------------
QString SaXManipulatePointers::getDevice ( void ) {
	// .../
	//! return the device name the pointer device is
	//! connected to. Note this may be an alias name
	//! concerning the type of the pointer device
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Device");
}

//====================================
// getDriver
//------------------------------------
QString SaXManipulatePointers::getDriver ( void ) {
	// .../
	//! return the driver used for this pointer device
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Driver");
}

//====================================
// getProtocol
//------------------------------------
QString SaXManipulatePointers::getProtocol ( void ) {
	// .../
	//! return the protocol used for this pointer device
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Protocol");
}

//====================================
// getOptions
//------------------------------------
Q3Dict<QString> SaXManipulatePointers::getOptions (void) {
	// .../
	//! retrieve an option list of all options set for this pointer
	//! device. The storage is a dictionary saving the
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
	QString rawOptions = mImport -> getItem ("RawOption");
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
	QStringList rawlist = QStringList::split ( ",", rawOptions );
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
// Constructor...
//------------------------------------
SaXManipulateMice::SaXManipulateMice (
	SaXImport* in, int ptr
) : SaXManipulateMiceIF (in,ptr) {
	// .../
	//! An object of this type is used to configure simple
	//! mouse pointer devices
	// ----
	mCDBMice = 0;
}

//====================================
// enableWheelEmulation
//------------------------------------
void SaXManipulateMice::enableWheelEmulation (int btn) {
	// .../
	//! for mice without a wheel one button can be set
	//! to emulate a wheel while pressed. This method will
	//! setup the wheel emulation on button (btn)
	// ----
	if (! mImport) {
		return;
	}
	QString button;
	button.sprintf ("%d",btn);
	mImport -> setItem ("EmulateWheel",button);
}

//====================================
// disableWheelEmulation
//------------------------------------
void SaXManipulateMice::disableWheelEmulation (void) {
	// .../
	//! release the wheel emulation button used for
	//! emulating a wheel. This is done by removing the
	//! EmulateWheel item completely
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("EmulateWheel","");
}

//====================================
// enable3ButtonEmulation
//------------------------------------
void SaXManipulateMice::enable3ButtonEmulation (void) {
	// .../
	//! for mice with only two buttons the third button
	//! can be emulated by pressing both the left and right
	//! button simultaneously
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("Emulate3Buttons","on");
}

//====================================
// disable3ButtonEmulation
//------------------------------------
void SaXManipulateMice::disable3ButtonEmulation (void) {
	// .../
	//! disable the emulation of the third mouse button
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("Emulate3Buttons","");
}

//====================================
// enableWheel
//------------------------------------
void SaXManipulateMice::enableWheel  (void) {
	// .../
	//! enable the mouse wheel. This function assumes that
	//! the wheel is mapped to the buttons 4 and 5 which is
	//! in almost all cases true even for mice with more than
	//! 3 buttons
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("ZAxisMapping","4 5");
}

//====================================
// enableWheel
//------------------------------------
void SaXManipulateMice::enableWheelOn  (int b1, int b2) {
	// .../
	//! enable the mouse wheel. This function uses b1 and b2
	//! as buttons to map the wheel on
	// ----
	if (! mImport) {
		return;
	}
	QString val;
	QTextOStream (&val) << b1 << " " << b2;
	mImport -> setItem ("ZAxisMapping",val);
}

//====================================
// enableXAxisInvertation
//------------------------------------
void SaXManipulateMice::enableXAxisInvertation (void) {
	// .../
	//! enable the mouse X axis invertation by setting the
	//! option InvX entry to on
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("InvX","on");
}
                                              
//====================================
// enableYAxisInvertation
//------------------------------------
void SaXManipulateMice::enableYAxisInvertation (void) {
	// .../
	//! enable the mouse Y axis invertation by setting the
	//! option InvY entry to on
	// ----                                               
	if (! mImport) {                                       
		return;                                              
	}
	mImport -> setItem ("InvY","on");
}                   

//====================================
// disableWheel
//------------------------------------
void SaXManipulateMice::disableWheel (void) {
	// .../
	//! disable the mouse wheel. The X-Server will report
	//! the events as button events without a Z-axs
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("ZAxisMapping","");
}

//====================================
// disableXAxisInvertation
//------------------------------------
void SaXManipulateMice::disableXAxisInvertation (void) {
	// .../
	//! disable the mouse X axis invertation by removing the
	//! option InvX entry
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("InvX","");
}

//====================================
// disableYAxisInvertation
//------------------------------------
void SaXManipulateMice::disableYAxisInvertation (void) {
	// .../
	//! disable the mouse Y axis invertation by removing the
	//! option InvY entry
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("InvY","");
}

//====================================
// enableLeftHandMapping
//------------------------------------
void SaXManipulateMice::enableLeftHandMapping  ( void ) {
	// .../
	//! Activate the left hand button mapping for the core pointer
	//! This is done by adding the option ButtonMapping
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("ButtonMapping","3 2 1");
}

//====================================
// disableLeftHandMapping
//------------------------------------
void SaXManipulateMice::disableLeftHandMapping ( void ) {
	// .../
	//! Deactivate the left hand button mapping for the core pointer
	//! This is done by removing the ButtonMapping option
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("ButtonMapping","");
}

//====================================
// isMouse
//------------------------------------
bool SaXManipulateMice::isMouse (void) {
	// .../
	//! check if the current device is a simply mouse. this is
	//! done by checking the driver name. If the driver "mouse"
	//! is used it is a simple device. Any other input devices
	//! will set the InputFashion parameter to become checked
	// ----
	if (! mImport) {
		return false;
	}
	QString driver = getDriver();
	QString fashion = mImport -> getItem ("InputFashion");
	if ((driver == "mouse") || (driver == "evdev")) {
		if (fashion != "MultiSeat") {
			return true;
		}
		return false;
	}
	if ((fashion != "Tablet")     &&
		(fashion != "Pen")        &&
		(fashion != "Eraser")     &&
		(fashion != "Touch")      &&
		(fashion != "Pad")        &&
		(fashion != "VNC")        &&
		(fashion != "Keyboard")   &&
		(fashion != "Touchpanel")
	) {
		return true;
	}
	return false;
}

//====================================
// isWheelEnabled
//------------------------------------
bool SaXManipulateMice::isWheelEnabled (void) {
	// .../
	//! check if the wheel functions are active. This is done
	//! by checking the ZAxisMapping keyword
	// ----
	if (! mImport) {
		return false;
	}
	QString zaxis = mImport -> getItem ("ZAxisMapping");
	if (! zaxis.isEmpty()) {
		return true;
	}
	return false;
}

//====================================
// isLeftHandEnabled
//------------------------------------
bool SaXManipulateMice::isLeftHandEnabled (void) {
	// .../
	//! check if the left hand mapping is active. This is done
	//! by checking the ButtonMapping keyword
	// ----
	if (! mImport) {
		return false;
	}
	QString mapping = mImport -> getItem ("ButtonMapping");
	if (mapping.contains("3 2 1")) {
		return true;
	}
	return false;
}

//====================================
// isXAxisInverted
//------------------------------------
bool SaXManipulateMice::isXAxisInverted (void) {
	// .../
	//! check if the mouse X axis has been inverted. This is done
	//! by checking the InvX keyword
	// ----
	if (! mImport) {
		return false;
	}
	QString xaxis = mImport -> getItem ("InvX");
	if (xaxis == "on") {                      
		return true; 
	}                                                            
	return false;
}

//====================================
// isYAxisInverted
//------------------------------------
bool SaXManipulateMice::isYAxisInverted (void) {
	// .../
	//! check if the mouse Y axis has been inverted. This is done
	//! by checking the InvY keyword
	// ----
	if (! mImport) {
		return false;                 
	}
	QString yaxis = mImport -> getItem ("InvY");
	if (yaxis == "on") {  
		return true;                                   
	}
	return false;
}

//====================================
// isWheelEmulated
//------------------------------------
bool SaXManipulateMice::isWheelEmulated (void) {
	// .../
	//! check if the wheel functions are emulated using
	//! a special button which enables the emulation
	// ----
	if (! mImport) {
		return false;
	}
	QString emulate = mImport -> getItem ("EmulateWheel");
	if (! emulate.isEmpty()) {
		return true;
	}
	return false;
}

//====================================
// isButtonEmulated
//------------------------------------
bool SaXManipulateMice::isButtonEmulated (void) {
	// .../
	//! check if the third button emulation has been activated.
	//! This is done by checking the Emulate3Buttons keyword
	// ----
	if (! mImport) {
		return false;
	}
	QString emulate = mImport -> getItem ("Emulate3Buttons");
	if (emulate == "on") {
		return true;
	}
	return false;
}

//====================================
// getWheelEmulatedButton
//------------------------------------
int SaXManipulateMice::getWheelEmulatedButton (void) {
	// .../
	//! check if the wheel functions are emulated using
	//! a special button which enables the emulation and 
	//! return the number of this button
	// ----
	if (! isWheelEmulated()) {
		return -1;
	}
	int btn = mImport -> getItem ("EmulateWheel").toInt();
	return btn;
}

//====================================
// getMouseList
//------------------------------------
QList<QString> SaXManipulateMice::getMouseList (void) {
	// .../
	//! retrieve a list of supported standard mouse groups. Each item
	//! contains the vendor and the model in its name separated
	//! by a colon
	// ----
	mCDBMouseList.clear();
	if ( ! mCDBMice ) {
		mCDBMice = new SaXProcess ();
		mCDBMice -> start (CDB_POINTERS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMice -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		mCDBMouseList.append (QString (it.currentKey()));
	}
	return mCDBMouseList;
}

//====================================
// getMouseVendorList
//------------------------------------
QList<QString> SaXManipulateMice::getMouseVendorList (void) {
	// .../
	//! retrieve a list of supported standard mouse
	//! vendor names
	// ----
	QList<QString> mouseList = getMouseList();
	mCDBMouseList.clear();
	QString it;
	foreach (it,mouseList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString* vendorName = new QString (vnlist.first());
		int hasVendor = false;
		QString io;
		foreach (io,mCDBMouseList) {
		if ( io == *vendorName ) {
			hasVendor = true;
			break;
		}
		}
		if (! hasVendor ) {
			mCDBMouseList.append ( *vendorName );
		}
	}
	return mCDBMouseList;
}

//====================================
// getMouseModelList
//------------------------------------
QList<QString> SaXManipulateMice::getMouseModelList (
	const QString& vendor
) {
	// .../
	//! retrieve a list of supported standard mouse
	//! model names
	// ----
	QList<QString> mouseList = getMouseList();
	mCDBMouseList.clear();
	QString it;
	foreach (it,mouseList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString vendorName = vnlist.first();
		QString* modelName = new QString (vnlist.last());
		if ( vendorName == vendor ) {
			mCDBMouseList.append ( *modelName );
		}
	}
	return mCDBMouseList;
}

//====================================
// setMouse
//------------------------------------
void SaXManipulateMice::setMouse ( const QString& group ) {
	// .../
	//! set all mouse data associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon
	// ----
	if ( ! mCDBMice ) {
		mCDBMice = new SaXProcess ();
		mCDBMice -> start (CDB_POINTERS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBMice -> getTablePointerCDB_DATA (
		group
	);
	// .../
	// move the data record to the correct position
	// refering to the section ID -> mPointer
	// ----
	Q3Dict<QString>* record = data.take(0);
	for (int n=0;n < mPointer;n++) {
		data.append(new Q3Dict<QString>());
	}
	data.append ( record );
	// .../
	// merge the data into the current section now
	// ----
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return;
	}
	mImport -> merge ( data );
	// .../
	// set vendor and name tag
	// ----
	QStringList nameList = QStringList::split ( ":", group );
	mImport -> setItem ( "Vendor", nameList.first() );
	mImport -> setItem ( "Name"  , nameList.last()  );
}

//====================================
// setMouse
//------------------------------------
void SaXManipulateMice::setMouse (
	const QString& vendor, const QString& model
) {
	// .../
	//! set all mouse data associated with the given vendor
	//! and model name to the current pointer data.
	// ----
	setMouse (vendor+":"+model);
}

//====================================
// getMouseData
//------------------------------------
Q3Dict<QString> SaXManipulateMice::getMouseData (
	const QString& group
) {
	// .../
	//! return the mouse data dictionary associated with the
	//! given CDB group name.
	// ----
	mCDBMouseData.clear();
	if ( ! mCDBMice ) {
		mCDBMice = new SaXProcess ();
		mCDBMice -> start (CDB_POINTERS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBMice -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		if ( it.currentKey() == group ) {
			mCDBMouseData = *it.current();
			break;
		}
	}
	return mCDBMouseData;
}

//====================================
// getMouseData
//------------------------------------
Q3Dict<QString> SaXManipulateMice::getMouseData (
	const QString& vendor, const QString& name
) {
	// .../
	//! return the mouse data dictionary associated with the
	//! given vendor and model name.
	// ----
	return getMouseData (vendor+":"+name);
}


//====================================
// Constructor...
//------------------------------------
SaXManipulateTablets::SaXManipulateTablets (
	SaXImport* pointer, SaXImport* layout, int id
) : SaXManipulateTabletsIF (pointer,id) {
	// .../
	//! An object of this type is used to configure
	//! tablet pointer devices including pens and erasers
	// ----
	mManipInputDevices = new SaXManipulateDevices (pointer,layout);
	mCDBTabletModules = 0;
	mCDBTablets = 0;
	mCDBPens = 0;
	mCDBPads = 0;
}

//====================================
// setType
//------------------------------------
void SaXManipulateTablets::setType (const QString& type) {
	// .../
	//! set the tablet type for the currently selected
	//! pointer device
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("TabletType",type);
}

//====================================
// setMode
//------------------------------------
void SaXManipulateTablets::setMode (const QString& mode) {
	// .../
	//! set the tablet mode for the currently selected
	//! pointer device
	// ----
	if (! mImport) {
		return;
	}
	mImport -> setItem ("TabletMode",mode);
}

//====================================
// isTablet
//------------------------------------
bool SaXManipulateTablets::isTablet (void) {
	// .../ 
	//! check if the current device is a tablet. This is
	//! done by checking the InputFashion parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Tablet") {
		return true;
	}
	return false;
}

//====================================
// isPen
//------------------------------------
bool SaXManipulateTablets::isPen (void) {
	// .../
	//! check if the current device is a tablet pen. This is
	//! done by checking the InputFashion parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Pen") {
		return true;
	}
	return false;
}

//====================================
// isEraser
//------------------------------------
bool SaXManipulateTablets::isEraser (void) {
	// .../
	//! check if the current device is a tablet eraser.
	//! This is done by checking the InputFashion
	//! parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Eraser") {
		return true;
	}
	return false;
}

//====================================
// isTouch
//------------------------------------
bool SaXManipulateTablets::isTouch (void) {
	// .../
	//! check if the current device is a tablet touch device.
	//! This is done by checking the InputFashion
	//! parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Touch") {
		return true;
	}
	return false;
}

//====================================
// isPad
//------------------------------------
bool SaXManipulateTablets::isPad (void) {
	// .../
	//! check if the current device is a tablet pad. This is
	//! done by checking the InputFashion parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Pad") {
		return true;
	}
	return false;
}

//====================================
// getName
//------------------------------------
QString SaXManipulateTablets::getName (void) {
	// .../
	//! return the CDB model name of this tablet
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Name");
}

//====================================
// getVendor
//------------------------------------
QString SaXManipulateTablets::getVendor (void) {
	// .../
	//! return the CDB vendor name of this tablet
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("Vendor");
}

//====================================
// getType
//------------------------------------
QString SaXManipulateTablets::getType (void) {
	// .../
	//! return the type of this pointer device which 
	//! can be: cursor,stylus or eraser
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("TabletType");
}

//====================================
// getMode
//------------------------------------
QString SaXManipulateTablets::getMode (void) {
	// .../
	//! return the mode of this pointer device which 
	//! can be either Absolute or Relative
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("TabletMode");	
}

//====================================
// getTabletDrivers
//------------------------------------
QList<QString> SaXManipulateTablets::getTabletDrivers (void) {
	// .../
	//! retrieve a list of supported tablet modules from the
	//! TabletModules file
	// ----
	mCDBTabletDrivers.clear();
	if ( ! mCDBTabletModules ) {
		mCDBTabletModules = new SaXProcess ();
		mCDBTabletModules -> start (CDB_TABLETMODULES);
	}
	Q3Dict< Q3Dict<QString> > CDBModules=mCDBTabletModules->getTablePointerCDB();
	Q3DictIterator< Q3Dict<QString> > it (CDBModules);
	for (; it.current(); ++it) {
		mCDBTabletDrivers.append (QString(it.currentKey()));
	}
	return mCDBTabletDrivers;
}

//====================================
// getTabletOptions
//------------------------------------
Q3Dict<QString> SaXManipulateTablets::getTabletOptions (const QString& driver) {
	// .../
	//! retrieve a list of tablet options refering to the tablet
	//! driver specified by (driver). The result is a dictionary
	//! saving the option name as key and the optional value as value.
	//! the value specify the type of the option (bool,any,string) and
	//! may contain additional information if type=string
	// ----
	mCDBTabletOptions.clear();
	if ( ! mCDBTabletModules ) {
		mCDBTabletModules = new SaXProcess ();
		mCDBTabletModules -> start (CDB_TABLETMODULES);
	}
	Q3PtrList< Q3Dict<QString> > opts;
	opts = mCDBTabletModules -> getTablePointerCDB_DATA (
		driver
	);
	if (opts.isEmpty()) {
		excCDBRecordNotFound (driver);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		Q3Dict<QString>* nope = new Q3Dict<QString>;
		return *nope;
	}
	mCDBTabletOptions = *opts.at(0);
	return mCDBTabletOptions;
}

//====================================
// getTabletList
//------------------------------------
QList<QString> SaXManipulateTablets::getTabletList (void) {
	// .../
	//! retrieve a list of supported tablet groups. Each item
	//! contains the vendor and the model in its name separated
	//! by a colon
	// ----
	mCDBTabletList.clear();
	if ( ! mCDBTablets ) {
		mCDBTablets = new SaXProcess ();
		mCDBTablets -> start (CDB_TABLETS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBTablets -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		mCDBTabletList.append (QString (it.currentKey()));
	}
	return mCDBTabletList;
}

//====================================
// getTabletVendorList
//------------------------------------
QList<QString> SaXManipulateTablets::getTabletVendorList (void) {
	// .../
	//! retrieve a list of supported tablet
	//! vendor names
	// ----
	QList<QString> tabletList = getTabletList();
	mCDBTabletList.clear();
	QString it;
	foreach (it,tabletList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString* vendorName = new QString (vnlist.first());
		int hasVendor = false;
		QString io;
		foreach (io,mCDBTabletList) {
		if ( io == *vendorName ) {
			hasVendor = true;
			break;
		}
		}
		if (! hasVendor ) {
			mCDBTabletList.append ( *vendorName );
		}
	}
	return mCDBTabletList;
}

//====================================
// getTabletModelList
//------------------------------------
QList<QString> SaXManipulateTablets::getTabletModelList (
	const QString& vendor
) {
	// .../
	//! retrieve a list of supported tablet
	//! model names
	// ----
	QList<QString> tabletList = getTabletList();
	mCDBTabletList.clear();
	QString it;
	foreach (it,tabletList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString vendorName = vnlist.first();
		QString* modelName = new QString (vnlist.last());
		if ( vendorName == vendor ) {
			mCDBTabletList.append ( *modelName );
		}
	}
	return mCDBTabletList;
}

//====================================
// getTabletData
//------------------------------------
Q3Dict<QString> SaXManipulateTablets::getTabletData ( const QString& group ) {
	// .../
	//! return the tablet data dictionary associated with the
	//! given CDB group name.
	// ----
	mCDBTabletData.clear();
	if ( ! mCDBTablets ) {
		mCDBTablets = new SaXProcess ();
		mCDBTablets -> start (CDB_TABLETS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBTablets -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		if ( it.currentKey() == group ) {
			mCDBTabletData = *it.current();
			break;
		}
	}
	return mCDBTabletData;
}

//====================================
// getTabletData
//------------------------------------
Q3Dict<QString> SaXManipulateTablets::getTabletData (
	const QString& vendor,const QString& name
) {
	// .../
	//! return the tablet data dictionary associated with the
	//! given vendor and model name.
	// ----
	return getTabletData (vendor+":"+name);
}

//====================================
// getPenData
//------------------------------------
Q3Dict<QString> SaXManipulateTablets::getPenData ( const QString& group ) {
	// .../
	//! return the pen data dictionary associated with the
	//! given CDB group name.
	// ----
	mCDBTabletData.clear();
	if ( ! mCDBPens ) {
		mCDBPens = new SaXProcess ();
		mCDBPens -> start (CDB_PENS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBPens -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		if ( it.currentKey() == group ) {
			mCDBTabletData = *it.current();
			break;
		}
	}
	return mCDBTabletData;
}

//====================================
// getPenData
//------------------------------------
Q3Dict<QString> SaXManipulateTablets::getPenData (
	const QString& vendor,const QString& name
) {
	// .../
	//! return the pen data dictionary associated with the
	//! given vendor and model name.
	// ----
	return getPenData (vendor+":"+name);
}

//====================================
// getPenList
//------------------------------------
QList<QString> SaXManipulateTablets::getPenList (void) {
	// .../
	//! retrieve a list of supported Pen/Eraser groups. Each item
	//! contains the vendor and the model in its name separated
	//! by a colon
	// ----
	mCDBPenList.clear();
	if ( ! mCDBPens ) {
		mCDBPens = new SaXProcess ();
		mCDBPens -> start (CDB_PENS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBPens -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		mCDBPenList.append (QString (it.currentKey()));
	}
	return mCDBPenList;
}

//====================================
// setTablet
//------------------------------------
void SaXManipulateTablets::setTablet (const QString& group) {
	// .../
	//! set all tablet data associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon
	// ----
	if ( ! mCDBTablets ) {
		mCDBTablets = new SaXProcess ();
		mCDBTablets -> start (CDB_TABLETS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBTablets -> getTablePointerCDB_DATA (
		group
	);
	// .../
	// move the data record to the correct position
	// refering to the section ID -> mPointer
	// ----
	Q3Dict<QString>* record = data.take(0);
	for (int n=0;n < mPointer;n++) {
		data.append(new Q3Dict<QString>());
	}
	data.append ( record );
	// .../
	// merge the data into the current section now
	// ----
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return;
	}
	mImport -> merge ( data );
	// .../
	// set vendor and name tag
	// ----
	QStringList nameList = QStringList::split ( ":", group );
	mImport -> setItem ( "Vendor", nameList.first() );
	mImport -> setItem ( "Name"  , nameList.last()  );
}
//====================================
// setTablet
//------------------------------------
void SaXManipulateTablets::setTablet (
	const QString& vendor, const QString& model
) {
	// .../
	//! set all tablet data associated with the given vendor and model
	//! name to the current pointer data.
	// ----
	setTablet (vendor+":"+model);
}

//====================================
// addPad
//------------------------------------
int SaXManipulateTablets::addPad (const QString& group) {
	// .../
	//! add a new Pad associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon. The contents of the
	//! data record will set the InputFashion type for this addon
	//! pointer device
	// ----
	if ( ! mCDBPads ) {
		mCDBPads = new SaXProcess ();
		mCDBPads -> start (CDB_PADS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBPads -> getTablePointerCDB_DATA (
		group
	);
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return -1;
	}
	// .../
	// set input fashion type for the selected pad
	// ----
	Q3Dict<QString> penData = *data.at(0);
	QString* type = penData["TabletType"];
	if (! type) {
		excPointerFashionTypeFailed ("undefined");
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	if (*type != "pad") {
		excPointerFashionTypeFailed (*type);
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	QString fashion (SAX_INPUT_PAD);
	// .../
	// create new input device for the pad and make
	// it the current pointer device
	// ----
	int newPad = mManipInputDevices->addInputDevice (fashion);
	if ( ! selectPointer (newPad)) {
		return -1;
	}
	// .../
	// move the data record to the correct position
	// refering to the section ID -> mPointer
	// ----
	Q3Dict<QString>* record = data.take(0);
	for (int n=0;n < mPointer;n++) {
		data.append(new Q3Dict<QString>());
	}
	data.append ( record );
	// .../
	// merge the pad data now into the current section
	// ----
	mImport -> merge ( data );
	// .../
	// set vendor and name tag
	// ----
	QStringList nameList = QStringList::split ( ":", group );
	mImport -> setItem ( "Vendor", nameList.first() );
	mImport -> setItem ( "Name"  , nameList.last()  );
	return newPad;
}
//====================================
// addPad
//------------------------------------
int SaXManipulateTablets::addPad (
	const QString& vendor, const QString& model
) {
	// .../
	//! add a new Pad associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon. The contents of the
	//! data record will set the InputFashion type for this addon
	//! pointer device
	// ----
	return addPad (vendor+":"+model);
}   

//====================================
// addPen
//------------------------------------
int SaXManipulateTablets::addPen (const QString& group) {
	// .../
	//! add a new Pen or Eraser associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon. The contents of the
	//! data record will set the InputFashion type for this addon
	//! pointer device
	// ----
	if ( ! mCDBPens ) {
		mCDBPens = new SaXProcess ();
		mCDBPens -> start (CDB_PENS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBPens -> getTablePointerCDB_DATA (
		group
	);
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return -1;
	}
	// .../
	// set input fashion type for the selected pen
	// ----
	Q3Dict<QString> penData = *data.at(0);
	QString* type = penData["TabletType"];
	if (! type) {
		excPointerFashionTypeFailed ("undefined");
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	if ((*type != "stylus") && (*type != "eraser") && (*type != "touch")) {
		excPointerFashionTypeFailed (*type);
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	QString fashion (SAX_INPUT_PEN);
	if (*type == "eraser") {
		fashion = SAX_INPUT_ERASER;
	}
	if (*type == "touch") {
		fashion = SAX_INPUT_TOUCH;
	}
	// .../
	// create new input device for the pen and make
	// it the current pointer device
	// ----
	int newPen = mManipInputDevices->addInputDevice (fashion);
	if ( ! selectPointer (newPen)) {
		return -1;
	}
	// .../
	// move the data record to the correct position
	// refering to the section ID -> mPointer
	// ----
	Q3Dict<QString>* record = data.take(0);
	for (int n=0;n < mPointer;n++) {
		data.append(new Q3Dict<QString>());
	}
	data.append ( record );
	// .../
	// merge the pen data now into the current section
	// ----
	mImport -> merge ( data );
	// .../
	// set vendor and name tag
	// ----
	QStringList nameList = QStringList::split ( ":", group );
	mImport -> setItem ( "Vendor", nameList.first() );
	mImport -> setItem ( "Name"  , nameList.last()  );
	return newPen;
}
//====================================
// addPen
//------------------------------------
int SaXManipulateTablets::addPen (
	const QString& vendor, const QString& model
) {
	// .../
	//! add a new Pen or Eraser associated with the given vendor and model
	//! name to the current pointer data. The contents of the
	//! data record will set the InputFashion type for this addon
	//! pointer device
	// ----
	return addPen (vendor+":"+model);
}

//====================================
// removePen
//------------------------------------
int SaXManipulateTablets::removePen ( int id ) {
	// .../
	//! remove the Pen or Eraser located at section ID (id)
	//! If the InputFashion type is a valid SAX_INPUT_PEN or
	//! SAX_INPUT_ERASER the method will remove the pointer
	//! device and return the new current ID
	// ----
	if (! selectPointer ( id )) {
		return -1;
	}
	QString fashion (
		mImport->getItem("InputFashion")
	);
	if (
		(fashion != QString(SAX_INPUT_PEN ))   &&
		(fashion != QString(SAX_INPUT_ERASER))
	) {
		excPointerFashionTypeFailed (fashion);
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	return mManipInputDevices->removeInputDevice (id);
}

//====================================
// removeTouch
//------------------------------------
int SaXManipulateTablets::removeTouch ( int id ) {
	// .../
	//! remove the Touch located at section ID (id)
	//! If the InputFashion type is a valid SAX_INPUT_TOUCH
	//! the method will remove the pointer device and return
	//! the new current ID
	// ----
	if (! selectPointer ( id )) {
		return -1;
	}
	QString fashion (
		mImport->getItem("InputFashion")
	);
	if (fashion != QString(SAX_INPUT_TOUCH)) {
		excPointerFashionTypeFailed (fashion);
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	return mManipInputDevices->removeInputDevice (id);
}

//====================================
// removePad
//------------------------------------
int SaXManipulateTablets::removePad ( int id ) {
	// .../
	//! remove the Pad located at section ID (id)
	//! If the InputFashion type is a valid SAX_INPUT_PAD
	//! the method will remove the pointer device and return
	//! the new current ID
	// ----
	if (! selectPointer ( id )) {
		return -1;
	}
	QString fashion (
		mImport->getItem("InputFashion")
	);
	if (fashion != QString(SAX_INPUT_PAD)) {
		excPointerFashionTypeFailed (fashion);
		qError (errorString(),EXC_POINTERFASHIONTYPEFAILED);
		return -1;
	}
	return mManipInputDevices->removeInputDevice (id);
}

//====================================
// Constructor...
//------------------------------------
SaXManipulateTouchscreens::SaXManipulateTouchscreens (
	SaXImport* in, int ptr
) : SaXManipulateTouchscreensIF (in,ptr) {
	// .../
	//! An object of this type is used to configure simple
	//! touchpanel pointer devices for predefined supported
	//! panels
	// ----
	mCDBPanels = 0;
}

//====================================
// isTouchpanel
//------------------------------------
bool SaXManipulateTouchscreens::isTouchpanel (void) {
	// .../
	//! check if the current device is a touchpanel.
	//! This is done by checking the InputFashion
	//! parameter.
	// ----
	if (! mImport) {
		return false;
	}
	QString fashion = mImport -> getItem ("InputFashion");
	if (fashion == "Touchpanel") {
		return true;
	}
	return false;
}

//====================================
// getName
//------------------------------------
QString SaXManipulateTouchscreens::getName ( void ) {
	// .../
	//! return the model name of this touchscreen pointer
	//! device. The vendor and name is stored as one string
	//! separated by a semi-colon
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	QString name = mImport -> getItem ("Name");
	QStringList vnlist = QStringList::split ( ";", name );
	return vnlist.last();
}

//====================================
// getVendor
//------------------------------------
QString SaXManipulateTouchscreens::getVendor ( void ) {
	// .../
	//! return the vendor name of this touchscreen pointer
	//! device. The vendor and name is stored as one string
	//! separated by a semi-colon
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	QString name = mImport -> getItem ("Name");
	QStringList vnlist = QStringList::split ( ";", name );
	return vnlist.first();
}

//====================================
// getType
//------------------------------------
QString SaXManipulateTouchscreens::getType (void) {
	// .../
	//! return the type of this pointer device.
	//! Note the type parameter of a touchscreen or a tablet
	//! is handled with the same key: TabletType
	// ----
	if (! mImport) {
		QString* nope = new QString;
		return *nope;
	}
	return mImport -> getItem ("TabletType");
}

//====================================
// getCDBPanelList
//------------------------------------
QList<QString> SaXManipulateTouchscreens::getPanelList (void) {
	// .../
	//! retrieve a list of supported panel groups. Each item
	//! contains the vendor and the model in its name separated
	//! by a colon
	// ----
	mCDBPanelList.clear();
	if ( ! mCDBPanels ) {
		mCDBPanels = new SaXProcess ();
		mCDBPanels -> start (CDB_TOUCHERS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBPanels -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		mCDBPanelList.append (QString (it.currentKey()));
	}
	return mCDBPanelList;
}

//====================================
// getCDBPanelVendorList
//------------------------------------
QList<QString> SaXManipulateTouchscreens::getPanelVendorList (void) {
	// .../
	//! retrieve a list of supported panel
	//! vendor names
	// ----
	QList<QString> panelList = getPanelList();
	mCDBPanelList.clear();
	QString it;
	foreach (it,panelList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString* vendorName = new QString (vnlist.first());
		int hasVendor = false;
		QString io;
		foreach (io,mCDBPanelList) {
		if ( io == *vendorName ) {
			hasVendor = true;
			break;
		}
		}
		if (! hasVendor ) {
			mCDBPanelList.append ( *vendorName );
		}
	}
	return mCDBPanelList;
}

//====================================
// getCDBPanelModelList
//------------------------------------
QList<QString> SaXManipulateTouchscreens::getPanelModelList (
	const QString& vendor
) {
	// .../
	//! retrieve a list of supported panel
	//! model names
	// ----
	QList<QString> panelList = getPanelList();
	mCDBPanelList.clear();
	QString it;
	foreach (it,panelList) {
		QStringList vnlist = QStringList::split ( ":", it );
		QString vendorName = vnlist.first();
		QString* modelName = new QString (vnlist.last());
		if ( vendorName == vendor ) {
			mCDBPanelList.append ( *modelName );
		}
	}
	return mCDBPanelList;
}

//====================================
// getPanelData
//------------------------------------
Q3Dict<QString> SaXManipulateTouchscreens::getPanelData (
	const QString& group
) {
	// .../
	//! return the touchers data dictionary associated with the
	//! given CDB group name.
	// ----
	mCDBPanelData.clear();
	if ( ! mCDBPanels ) {
		mCDBPanels = new SaXProcess ();
		mCDBPanels -> start (CDB_TOUCHERS);
	}
	Q3Dict< Q3Dict<QString> > CDBData = mCDBPanels -> getTablePointerCDB ();
	Q3DictIterator< Q3Dict<QString> > it (CDBData);
	for (; it.current(); ++it) {
		if ( it.currentKey() == group ) {
			mCDBPanelData = *it.current();
			break;
		}
	}
	return mCDBPanelData;
}

//====================================
// getPanelData
//------------------------------------
Q3Dict<QString> SaXManipulateTouchscreens::getPanelData (
	const QString& vendor,const QString& name
) {
	// .../
	//! return the touchers data dictionary associated with the
	//! given vendor and model name.
	// ----
	return getPanelData (vendor+":"+name);
}

//====================================
// setTouchPanel
//------------------------------------
void SaXManipulateTouchscreens::setTouchPanel (const QString& group) {
	// .../
	//! set all panel data associated with the given group name to
	//! the current pointer data. The group name consists of the
	//! vendor and model name separated by a colon
	// ----
	if ( ! mCDBPanels ) {
		mCDBPanels = new SaXProcess ();
		mCDBPanels -> start (CDB_TOUCHERS);
	}
	Q3PtrList< Q3Dict<QString> > data;
	data = mCDBPanels -> getTablePointerCDB_DATA (
		group
	);
	if (data.isEmpty()) {
		excCDBRecordNotFound (group);
		qError (errorString(),EXC_CDBRECORDNOTFOUND);
		return;
	}
	// .../
	// move the data record to the correct position
	// refering to the section ID -> mPointer
	// ----
	Q3Dict<QString>* record = data.take(0);
	for (int n=0;n < mPointer;n++) {
		data.append(new Q3Dict<QString>());
	}
	data.append ( record );
	// .../
	// merge the toucher data into the current section
	// ----
	mImport -> merge ( data );
	// .../
	// set vendor and name tag
	// ----
	QStringList nameList = QStringList::split ( ":", group );
	setNamePair (
		nameList.first(),
		nameList.last()
	);
}

//====================================
// setTouchPanel
//------------------------------------
void SaXManipulateTouchscreens::setTouchPanel (
	const QString& vendor, const QString& model
) {
	// .../
	//! set all panel data associated with the given vendor and model
	//! name to the current pointer data.
	// ----
	setTouchPanel (vendor+":"+model);
}
} // end namespace
