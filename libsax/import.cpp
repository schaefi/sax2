/**************
FILE          : import.cpp
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
#include "import.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXImport::SaXImport (int section) {
	// .../
	//! An object of this type is used to create an ISAX based
	//! SaX import. Refering to the given section ID the correct
	//! isax options are set.
	// ----
	mOptions.append ((const char*)"-l");
	switch (section) {
		case SAX_CARD:
			mSectionName = "Card";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		case SAX_DESKTOP:
			mSectionName = "Desktop";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		case SAX_POINTERS:
			mSectionName = "Pointers";
			mOptions.append ((const char*)"Mouse");
		break;
		case SAX_KEYBOARD:
			mSectionName = "Keyboard";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		case SAX_LAYOUT:
			mSectionName = "Layout";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		case SAX_PATH:
			mSectionName = "Path";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		case SAX_EXTENSIONS:
			mSectionName = "Extensions";
			mOptions.append ((const char*)mSectionName.latin1());
		break;
		default:
			excImportSectionFailed();
			qError (errorString(),EXC_IMPORTSECTIONFAILED);
			mSectionName = "Undefined";
		break;
	}
	mSection = section;
}

//====================================
// doImport...
//------------------------------------
void SaXImport::doImport (void) {
	// .../
	//! call SaXProcess::start() to obtain the isax provided
	//! information
	// ----
	start (mOptions);
}

//====================================
// setSource...
//------------------------------------
void SaXImport::setSource (int s) {
	// .../
	//! set the ISAX data source which can be the current
	//! used configuration (SAX_SYSTEM_CONFIG) or the data stored
	//! inside SaX2 after the hardware detection has been called
	//! (SAX_AUTO_PROBE)
	// ----
	if ( s == SAX_AUTO_PROBE ) {
		mOptions.append ((const char*)"-b");
	}
}

//====================================
// getSectionID...
//------------------------------------
int SaXImport::getSectionID (void) {
	// .../
	//! return the section identifier this import belongs to
	// ----
	return mSection;
}

//====================================
// getSectionName...
//------------------------------------
QString SaXImport::getSectionName (void) {
	// .../
	//! return the section name corresponding to the section ID
	// ----
	return mSectionName;
}

//====================================
// Constructor...
//------------------------------------
SaXImportSysp::SaXImportSysp (int section) {
	// .../
	//! An object of this type is used to create a SYSP based
	//! SaX import. Refering to the given section ID the correct
	//! sysp.pl options are set.
	// ----
	mOptions.append ((const char*)"-q");
	switch (section) { 
		case SYSP_MOUSE:
			mSectionName = "Mouse";
			mOptions.append ((const char*)"mouse"); 
		break;
		case SYSP_KEYBOARD:
			mSectionName = "Keyboard";
			mOptions.append ((const char*)"keyboard");
		break;
		case SYSP_CARD:
			mSectionName = "Card";
			mOptions.append ((const char*)"server");
		break;
		case SYSP_DESKTOP:
			mSectionName = "Desktop";
			mOptions.append ((const char*)"xstuff");
		break;
		case SYSP_3D:
			mSectionName = "3D";
			mOptions.append ((const char*)"3d");
		break;
		default:
			excImportSectionFailed();
			qError (errorString(),EXC_IMPORTSECTIONFAILED);
			mSectionName = "Undefined";
		break;
	}
	mSection = section;
}

//====================================
// doImport...
//------------------------------------
void SaXImportSysp::doImport (void) {
	// .../
	//! call SaXProcess::start() to obtain the sysp provided
	//! information
	// ----
	start ( mOptions,SAX_SYSP );
}

//====================================
// getSectionID... 
//------------------------------------
int SaXImportSysp::getSectionID (void) {
	// .../
	//! return the section identifier this import belongs to
	// ----
	return mSection;
}

//====================================
// getSectionName...
//------------------------------------
QString SaXImportSysp::getSectionName (void) {
	// .../
	//! return the section name corresponding to the section ID
	// ----
	return mSectionName;
}

//====================================
// Constructor...
//------------------------------------
SaXImportCDB::SaXImportCDB (int fileID) {
	// .../
	//! An object of this type is used to create a CDB based
	//! SaX import. Refering to the given file ID the correct
	//! CDB file is selected.
	// ----
	mID = fileID;
}

//====================================
// doImport...
//------------------------------------
void SaXImportCDB::doImport (void) {
	// .../
	//! call SaXProcess::start() to obtain the data from
	//! the corresponding CDB file
	// ----
	start (mID);
}

//====================================
// Constructor...
//------------------------------------
SaXImportProfile::SaXImportProfile (const QString & p) {
	// .../
	//! An object of this type is used to create a PROFILE based
	//! SaX import. Refering to the given name the constructor
	//! will check if the file exists and add the correct
	//! createPRO.pl options
	// ----
	QFileInfo info (p);
	if (! info.exists()) {
		excProfileNotFound();
		qError (errorString(),EXC_PROFILENOTFOUND);
		return;
	}
	QString profile(p);
	mOptions.append ((const char*)"-p");
	mOptions.append ((const char*)profile.latin1());
	mDesktop = 0;
	mCard    = 0;
	mPointers= 0;
	mLayout  = 0;
}

//====================================
// doImport...
//------------------------------------
void SaXImportProfile::doImport (void) {
	// .../
	//! call SaXProcess::start() to obtain the createPRO.pl
	//! information. Because of the fact that a profile may
	//! handle multiple sections the data will be splitted
	//! into single sections which can be retrieved using
	//! getImport()
	// ----
	start ( mOptions,SAX_PROF );
	splitImport();
}

//====================================
// splitImport...
//------------------------------------
void SaXImportProfile::splitImport (void) {
	// .../
	//! Split the data from the profile into the appropriate
	//! ISAX sections. If the section does not exist a new one
	//! will be created. Each section can be obtained using the
	//! getImport() method. For the profile to take effect it is
	//! normally needed to merge this SaXImport objects into the
	//! basic imports which are the base for the configuration
	//! export: For example
	//! \code
	//!   SaXImport* profile = this->getImport (SAX_CARD);
	//!   baseImport->merge (profile->getTablePointerDATA());
	//! \endcode
	// ----
	for (int n=0;n < getCount();n++) {
		Q3Dict<QString>* table = getTablePointer ( n );
		Q3DictIterator<QString> it (*table);
		for (; it.current(); ++it) {
			QStringList tokens = QStringList::split ( ":", *it.current() );
			QString section = tokens.last();
			tokens.pop_back();
			QString value   = tokens.join(":");
			if (section == "Layout") {
				if (! mLayout) {
					mLayout  = new SaXImport (SAX_LAYOUT);
				}
				mLayout -> addID (n);
				mLayout -> setItem (it.currentKey(),value);
			}
			if (section == "Desktop") {
				if (! mDesktop) {
					mDesktop = new SaXImport (SAX_DESKTOP);
				}
				mDesktop -> addID (n);
				mDesktop -> setItem (it.currentKey(),value);
			}
			if (section == "Card") {
				if (! mCard) {
					mCard = new SaXImport (SAX_CARD);
				}
				mCard -> addID (n);
				mCard -> setItem (it.currentKey(),value);
			}
			if (section == "Mouse") {
				if (! mPointers) {
					mPointers = new SaXImport (SAX_POINTERS);
				 }
				mPointers -> addID (n);
				mPointers -> setItem (it.currentKey(),value);
			}
		}
	}
}

//====================================
// setDevice...
//------------------------------------
void SaXImportProfile::setDevice (int dev) {
	// .../
	//! Set the device number the profile should be applied
	//! to. This may be needed if the profile itself only set
	//! a wild card for the device number
	// ----
	QString device;
	device.sprintf ("%d",dev);
	mOptions.append ((const char*)"-d");
	mOptions.append ((const char*)device.latin1());
}

//====================================
// getImport...
//------------------------------------
SaXImport* SaXImportProfile::getImport (int section) {
	// .../
	//! Return a pointer to the SaXImport object created during
	//! the splitImport() call. Refering to the given section ID
	//! either the pointer or NULL is returned
	// ----
	SaXImport* obj = NULL;
	switch (section) {
		case SAX_CARD:
			obj = mCard;
		break;
		case SAX_DESKTOP:
			obj = mDesktop;
		break;
		case SAX_LAYOUT:
			obj = mLayout;
		break;
		case SAX_POINTERS:
			obj = mPointers;
		break;
		default:
			excProfileUndefined (section);
			qError (errorString(),EXC_PROFILEUNDEFINED);
		break;
	}
	return obj;
}
} // end namespace
