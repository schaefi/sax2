/**************
FILE          : glinfo.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : commandline configuration tool for the X
              : window system released under the GPL license
              :
DESCRIPTION   : SaX2 commandline system using libsax to provide
              : functions for setting the default color depth
              : //.../
              : - glinfo.cpp: get 3D status information
              : - glinfo.h: header file for glinfo.cpp
              : ----
              :
STATUS        : Status: Development 
**************/
#include "glinfo.h"

//=====================================
// get3DStatus...
//-------------------------------------
bool get3DStatus ( Q3Dict<SaXImport> section, int desktop ) {
	SaXManipulateDesktop mDesktop (
		section["Desktop"],section["Card"],section["Path"]
	);
	if (! mDesktop.selectDesktop ( desktop )) {
		return false;
	}
	if (mDesktop.is3DEnabled()) {
		return true;
	}
	return false;
}

