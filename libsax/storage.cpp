/**************
FILE          : storage.cpp
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
#include "storage.h"
//Added by qt3to4:
#include <Q3PtrList>

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXStorage::SaXStorage (void) {
	// .../
	//! An object of this type is used to create a storage
	//! object saving all the data provided by the interfaces
	//! ISAX,SYSP,CDB,PROFILE
	// ----
	mCurrentID = 0;

	mData.insert (mCurrentID, new Q3Dict<QString>);
}

//====================================
// Set standard key/value item...
//------------------------------------
void SaXStorage::setItem ( const QString & key, const QString & val ) {
	// .../
	//! set key value pairs to the current data dictionary
	//! reached via mCurrentID
	// ----
	QString* data = new QString (val);
	mData.at (mCurrentID) -> replace (key,data);
}

//====================================
// add standard key/value item...
//------------------------------------
void SaXStorage::addItem ( const QString & key, const QString & val ) {
	// .../
	//! add a value to the current value of (key) seperated by
	//! the comma sign. If there is no data behind (key) nothing
	//! will happen
	// ----
	QString* currentValue = mData.at (mCurrentID) -> take (key);
	if ((currentValue) && (! currentValue->isEmpty())) {
		QString newValue;
		QTextOStream(&newValue) << *currentValue << "," << val;
		newValue.replace (QRegExp("^,"),"");
		setItem (key,newValue);
	} else {
		setItem (key,val);
	}
}

//====================================
// remove standard key/value item...
//------------------------------------
void SaXStorage::removeItem ( const QString & key, const QString & val ) {
	// .../
	//! remove the value (val) from the current value list stored
	//! behind the key (key). If the value is not found the list
	//! won't be changed
	// ----
	QString* currentValue = mData.at (mCurrentID) -> take (key);
	if (currentValue) {
		QStringList optlist = QStringList::split ( ",", *currentValue );
		QStringList result;
		for ( QStringList::Iterator
			in = optlist.begin(); in != optlist.end(); ++in
		) {
			QString item (*in);
			if (item != val) {
				result.append (item);
			}
		}
		QString newValue = result.join (",");
		setItem (key,newValue);
	}
}

//====================================
// remove key/value entry...
//------------------------------------
void SaXStorage::removeEntry ( const QString & key ) {
	// .../
	//! remove a complete entry from the data dictionary
	//! The entry is searched as key named (key)
	// ----
	mData.at (mCurrentID) -> remove (key);
}

//====================================
// Get copy of contents of item key...
//------------------------------------
QString SaXStorage::getItem ( const QString & key ) {
	// .../
	//! returns a copy of the value of key refering
	//! to the current data record
	// ----
	if (! mData.at (mCurrentID) -> operator[] (key)) {
		QString* nope = new QString;
		return *nope;
	}
	return *mData.at (mCurrentID) -> operator[] (key);
}

//====================================
// Set vendor;name item...
//------------------------------------
void SaXStorage::setDenomination (
	const QString & key, const QString & vendor,const QString & name 
) {
	// .../
	//! set special item value normaly used for Vendor and Name
	//! specifications. The vendor and name string is concatenated
	//! using the ";" sign
	// ----
	QString value (vendor+";"+name);
	setItem (key,value);
}

//====================================
// Set raw item...
//------------------------------------
void SaXStorage::setRawItem (
	const QString & key, const QString & optname,const QString & optval
) {
	// .../
	//! set special item value used for options including a value
	//! if the key is some sort of Raw* the value behind this key
	//! is a comma separated list of key value pairs separated by
	//! a space each. This method will set such a value pair
	// ----
	QString value (optname+" "+optval);
	setItem (key,value);
}

//====================================
// Add to raw item...
//------------------------------------
void SaXStorage::addRawItem (
	const QString & key, const QString & optname,const QString & optval
) {
	// .../
	//! set special item value used for options including a value
	//! if the key is some sort of Raw* the value behind this key
	//! is a comma separated list of key value pairs separated by
	//! a space each. This method will add such a value pair
	// ----
	QString* currentValue = mData.at (mCurrentID) -> take (key);
	if ((currentValue) && (! currentValue->isEmpty())) {
		QString newValue;
		QString newOptVal (optname+" "+optval);
		QTextOStream(&newValue) << *currentValue << "," << newOptVal;
		newValue.replace (QRegExp("^,"),"");
		setItem (key,newValue);
	} else {
		setRawItem (key,optname,optval);
	}
}

//====================================
// Delete from raw item...
//------------------------------------
void SaXStorage::removeRawItem (
	const QString & key, const QString & opt
) {
	// .../
	//! set special item value used for options including a value
	//! if the key is some sort of Raw* the value behind this key
	//! is a comma separated list of key value pairs separated by
	//! a space each. This method will remove such a value pair
	// ----
	QString optname = opt;
	QString expression (",");
	if (key == "RawData") {
		expression = ",Option";
		optname.replace (QRegExp("^Option"),"");
	}
	QString* currentValue = mData.at (mCurrentID) -> take (key);
	if (currentValue) {
		QStringList optlist = QStringList::split ( expression, *currentValue );
		QStringList result;
		for ( QStringList::Iterator
			in = optlist.begin(); in != optlist.end(); ++in
		) {
			QString item (*in);
			if (! item.contains(optname)) {
				result.append (item);
			}
		}
		QString newValue = result.join (expression);
		QRegExp rx ("^Option");
		if (rx.search (newValue) == -1) {
			newValue = "Option" + newValue;
		}
		if (newValue == "Option") {
			setItem (key,"");
		} else {
			setItem (key,newValue);
		}
	}
}

//====================================
// merge data into object...
//------------------------------------
void SaXStorage::merge (Q3PtrList< Q3Dict<QString> > data) {
	// .../
	//! merge the data records from the list (data) into
	//! the corresponding data records of the object. If
	//! a record does not exist it will be created 
	// ----
	for (unsigned int n=0;n<data.count();n++) {
		Q3Dict<QString>* table = data.at(n);
		if ((! table) || (table->isEmpty())) {
			continue;
		}
		Q3DictIterator<QString> it (*table);
		addID (n);
		setID (n);
		for (; it.current(); ++it) {
			setItem (it.currentKey(),*it.current());
		}
	}
}

//====================================
// add new section ID...
//------------------------------------
bool SaXStorage::addID ( int id ) {
	// .../
	//! add a new data record without gaps. If the
	//! record already exists the function will return false
	//! otherwise true
	// ----
	if (! mData.at (id)) {
		while (mCurrentID < id) {
			mData.append ( new Q3Dict<QString>);
			mCurrentID = mData.at();
		}
		return true;
	}
	mCurrentID = id;
	return false;
}

//====================================
// remove and reorganize section ID...
//------------------------------------
bool SaXStorage::delID ( int id ) {
	// .../
	//! remove a data record and adapt the Identifier strings
	//! to provide consistency
	// ----
	if ((! mData.at (id)) || (mData.at(id)->isEmpty())) {
		return false;
	}
	int step = 1;
	int type = SAX_DESKTOP_TYPE;
	QString ident = *mData.at(id)->find ("Identifier");
	if (ident.contains ("Mouse")) {
		type = SAX_POINTER_TYPE;
		step = 2;
	}
	if (ident.contains ("Keyboard")) {
		type = SAX_KEYBOARD_TYPE;
		step = 2;
	}
	int index = -1;
	Q3Dict<QString>*  data;
	foreach (data,mData) {
		index++;
//		Q3Dict<QString>* data = in;
		QString* ident = data->find ("Identifier");
		if (! ident) {
			continue;
		}
		int curType = SAX_DESKTOP_TYPE;
		if (ident->contains("Mouse")) {
			curType = SAX_POINTER_TYPE;
		}
		if (ident->contains("Keyboard")) {
			curType = SAX_KEYBOARD_TYPE;
		}
		if ((data->isEmpty()) || (index <= id) || (curType != type)) {
			continue;
		}
		QString oIDstr;
		QString nIDstr;
		oIDstr.sprintf ("%d",index);
		nIDstr.sprintf ("%d",index - step);
		QString mouseIDstr    ("Mouse["   + oIDstr +"]");
		QString keyboardIDstr ("Keyboard["+ oIDstr +"]");
		QString deviceIDstr   ("Device["  + oIDstr +"]");
		QString monitorIDstr  ("Monitor[" + oIDstr +"]");
		QString screenIDstr   ("Screen["  + oIDstr +"]");
		Q3DictIterator<QString> it (*data);
		for (; it.current(); ++it) {
			QString val = *it.current();
			QString key = it.currentKey();
			if (val == mouseIDstr) {
				QString* nMouseIDstr = new QString ("Mouse["+nIDstr+"]");
				data -> replace (key,nMouseIDstr);
			}
			if (val == keyboardIDstr) {
				QString* nKbdIDstr = new QString ("Keyboard["+nIDstr+"]");
				data -> replace (key,nKbdIDstr);
			}
			if (val == deviceIDstr) {
				QString* nDeviceIDstr = new QString ("Device["+nIDstr+"]");
				data -> replace (key,nDeviceIDstr);
			}
			if (val == monitorIDstr) {
				QString* nMonitorIDstr = new QString ("Monitor["+nIDstr+"]");
				data -> replace (key,nMonitorIDstr);
			}
			if (val == screenIDstr) {
				QString* nScreenIDstr = new QString ("Screen["+nIDstr+"]");
				data -> replace (key,nScreenIDstr);
			}
			if ((key == "Screen") && (val == oIDstr)) {
				QString* nScreenIDstr = new QString (nIDstr);
				data -> replace (key,nScreenIDstr);
			}
		}
	}
	mData.remove (id);
	if ((mData.at(id)) && (mData.at(id)->isEmpty())) {
		mData.remove (id);
	}
	return true;
}

//====================================
// set section ID...
//------------------------------------
bool SaXStorage::setID ( int id ) {
	// .../
	//! change the current data record ID to the new ID (id)
	//! If there is no data for ID (id) an exception is throwed
	//! and the current ID won't become changed
	// ----
	if (! mData.at (id)) {
		excSetStorageIDFailed (id);
		qError (errorString(),EXC_SETSTORAGEIDFAILED);
		return false;
	}
	mCurrentID = id;
	return true;
}

//====================================
// Get current section ID...
//------------------------------------
int SaXStorage::getCurrentID ( void ) {
	// .../
	//! return the current section ID value
	// ----
	return mCurrentID;
}

//====================================
// Get dict for section ID X...
//------------------------------------
Q3Dict<QString> SaXStorage::getTable ( int id ) {
	// .../
	//! return a copy of the data dictionary for the given ID (id)
	// ----
	if (mData.at (id)) {
		return *mData.at (id);
	} else {
		Q3Dict<QString>* nope = new Q3Dict<QString>;
		return *nope;
	}
}

//====================================
// Get dict for current section ID...
//------------------------------------
Q3Dict<QString> SaXStorage::getCurrentTable ( void ) {
	// .../
	//! return a copy of the data dictionary for the current ID
	// ----
	return *mData.at (mCurrentID);
}

//====================================
// Get dict ptr for section ID X...
//------------------------------------
Q3Dict<QString>* SaXStorage::getTablePointer ( int id ) {
	// .../
	//! return a pointer to the data dictionary at ID (id)
	// ----
	return mData.at (id);
}

//====================================
// Get dict ptr for current section ID
//------------------------------------
Q3Dict<QString>* SaXStorage::getCurrentTablePointer ( void ) {
	// .../
	//! return a pointer to the data dictionary at the current ID
	// ----
	return mData.at (mCurrentID);
}

//====================================
// Get number of elements
//------------------------------------
int SaXStorage::getCount (bool noEmptyItem) {
	// .../
	//! if noEmptyItem is set to true this method will calculate
	//! the number of non empty data records. If noEmptyItem is set
	//! to false which is the default the method will return the
	//! number of elements stored in the mData data record list 
	// ----
	int count = 0;
	if (noEmptyItem) {
		Q3Dict<QString>* it;
		foreach (it,mData) {
		if (! it->isEmpty()) {
			count++;
		}
		}
	} else {
		count = mData.count();
	}
	return count;
}

//====================================
// add data to CDB dict
//------------------------------------
void SaXStorage::addGroup (
	const QString & group,const QString & key, const QString & value
) {
	// .../
	//! A method for the CDB interface only. This function will
	//! use (group) as first key and (key) as second key to store 
	//! the value (value) in a two dimensional data dictionary 
	// ----
	if ( ! mCDB[group] ) {
		mCDB.insert (group, new Q3Dict<QString>);
	}
	mCDB[group]->insert (key,new QString(value));
}


//====================================
// return CDB data pointer
//------------------------------------
Q3Dict< Q3Dict<QString> > SaXStorage::getTablePointerCDB ( void ) {
	// .../
	//! A method for the CDB interface only. Return a copy of the 
	//! two dimensional CDB data dictionary
	// ----
	return mCDB;
}

//====================================
// return CDB entry pointer 
//------------------------------------
Q3PtrList< Q3Dict<QString> > SaXStorage::getTablePointerCDB_DATA (
	const QString & group
) {
	// .../
	//! A method for the CDB interface only. Returns a pointer
	//! to the dictionary found under the key (group). The pointer
	//! is appended to a list because in most cases the return
	//! value is used as parmeter to the merge() method
	// ----
	Q3PtrList< Q3Dict<QString> > list;
	if ( mCDB[group] ) {
		list.append (mCDB[group]);
	}
	return list;
}

//====================================
// return mData data pointer
//------------------------------------
Q3PtrList< Q3Dict<QString> > SaXStorage::getTablePointerDATA ( void ) {
	// .../
	//! return a pointer list of the complete data dictionary
	//! including all ISAX data records
	// ----
	return mData;
}
} // end namespace
