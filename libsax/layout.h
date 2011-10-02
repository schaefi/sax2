/**************
FILE          : layout.h
***************
PROJECT       : SaX2 - library interface [header]
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
#ifndef SAX_LAYOUT_H
#define SAX_LAYOUT_H 1

//====================================
// Includes...
//------------------------------------
#include "import.h"
#include "card.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define SAX_SINGLE_HEAD     300
#define SAX_XINERAMA        301
#define SAX_TRADITIONAL     302
#define SAX_CLONE           303
#define SAX_MERGED_FB       304
#define SAX_MERGED_FB_CLONE 305
#define SAX_UNKNOWN_MODE    306

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Layout class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateLayoutIF : public SaXException {
	public:
	virtual void setXOrgMultiheadMode ( int ) = 0;
	virtual bool setXOrgLayout ( int, int, int, int, int ) = 0;
	virtual bool setRelative ( int, int, int, int ) = 0;
	virtual bool removeRelative ( int ) = 0;

	public:
	virtual int getMultiheadMode ( void ) = 0;
	virtual QList<QString> getXOrgLayout  ( int ) = 0;
	virtual QList<QString> getInputLayout ( void ) = 0;
	virtual void addInputLayout    ( int ) = 0;
	virtual void removeInputLayout ( int ) = 0;

	public:
	virtual ~SaXManipulateLayoutIF ( void ) { }
};
//====================================
// Class SaXManipulateLayout...
//------------------------------------
/*! \brief SaX2 -  Layout class.
*
* The layout manipulator requires two import objects (Layout and Card)
* to become created. Once created the manipulator object is able to get/set
* layout related information. The layout specifies the relationship
* and cooperation between the screens. The following example demonstrate
* how to check for the multihead mode currently used:
*
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
*     int importID[] = {
*         SAX_LAYOUT,
*         SAX_CARD
*     };
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     for (int id=0; id<2; id++) {
*         SaXImport* import = new SaXImport ( importID[id] );
*         import->setSource ( SAX_SYSTEM_CONFIG );
*         import->doImport();
*         config->addImport (import);
*         section.insert (
*             import->getSectionName(),import
*         );
*     }
*     printf ("checking multihead mode... ");
*     SaXManipulateLayout layout (
*         section["Layout"],section["Card"]
*     );
*     switch (layout.getMultiheadMode()) {
*         case SAX_SINGLE_HEAD:
*             printf ("single head mode\n");
*         break;
*         case SAX_XINERAMA:
*             printf ("Xorg Xinerama mode\n");
*         break;
*         case SAX_TRADITIONAL:
*             printf ("Xorg traditional multihead\n");
*         break;
*         case SAX_CLONE:
*             printf ("Xorg cloned multihead\n");
*         break;
*         case SAX_TWINVIEW:
*             printf ("Nvidia Twinview mode\n");
*         break;
*         case SAX_TWINVIEW_CLONE:
*             printf ("Nvidia Twinview clone mode\n");
*         break;
*         default:
*             printf ("unknown mode\n");
*         break;
*     }
*     return (0);
* }
* \endcode
*/
class SaXManipulateLayout : public SaXManipulateLayoutIF {
	private:
	SaXImport* mLayout;
	SaXImport* mCard;

	public:
	void setXOrgMultiheadMode ( int );
	bool setXOrgLayout ( int, int, int, int, int );
	bool setRelative ( int, int, int, int );
	bool removeRelative ( int );


	public:
	int getMultiheadMode ( void );
	QList<QString> getXOrgLayout  ( int );
	QList<QString> getInputLayout ( void );
	void addInputLayout    ( int );
	void removeInputLayout ( int );

	public:
	SaXManipulateLayout ( SaXImport*,SaXImport* );
};
} // end namespace
#endif
