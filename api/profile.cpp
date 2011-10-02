/**************
FILE          : profile.cpp
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
              : - profile.h: SaXGUI::SCCWidgetProfile header definitions
              : - profile.cpp: provide access to libsax profile data
              : ----
              :
STATUS        : Status: Development
**************/
#include "profile.h"

namespace SaXGUI {
//=====================================
// Constructor
//-------------------------------------
SCCWidgetProfile::SCCWidgetProfile ( void ) {
	// ... 
}   
    
//=====================================
// widgetProfile
//-------------------------------------
SaXImportProfile* SCCWidgetProfile::getProfile ( const QString& profile ) {
	SaXImportProfile* pProfile = mProfile.find (profile);
	if ( ! pProfile ) {
		pProfile = new SaXImportProfile ( profile );
		pProfile -> doImport();
		mProfile.replace (profile,pProfile);
	}
	return pProfile;
}   
}
