/**************
FILE          : tabletpadproperty.h
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
              : - tabletpadproperty.h: SaXGUI::SCCTabletPadProperty headers
              : - tabletpadproperty.cpp: configure tablets
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTABLET_PAD_PROPERTY_H
#define SCCTABLET_PAD_PROPERTY_H 1

//=====================================
// Includes
//-------------------------------------
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <q3tabdialog.h>
#include <q3vbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCTabletPenProperty
//------------------------------------
class SCCTabletPadProperty : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	int            mDisplay;	

	public:
	void setID ( int );
	int getID ( void );

	public:
	SCCTabletPadProperty (
		Q3Dict<QString>*,Q3Dict<SaXImport>,
		const QString&,int,QWidget*
	);
};
} // end namespace
#endif
