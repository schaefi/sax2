/**************
FILE          : sax.h
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
#ifndef SAX_SAX_H
#define SAX_SAX_H 1

/*! \mainpage libsax documentation
*
* \section Introduction
*
* libsax is a X11 configuration library for XOrg version v6.x.
* Between the SaX GUI and the SaX engine an interface exists to
* transport the information from the GUI into the engine which
* is then able to create or modify the X11 configuration. This
* interface is called ISaX. A complete explanation about
* how SaX2 is structured can be found within the documentation
* at /usr/share/doc/packages/sax2. The ISaX interface is the basis
* for the C++ library explained here. The library is based on the
* following major topics:
*
* - Initialization\n
*   Provide session cache
*
* - Imports\n
*   Provide classes to obtain all necessary information
*
* - Manipulators\n
*   Provide classes to manipulate imported data
*
* - Export/Commit\n
*   Provide classes to create or modify the X11-Configuration
*
* The programmer starts with an init() sequence to be able to
* access the automatically generated configuration suggestion which is
* based on the hardware detection. After this it is possible to
* import,manipulate and export information.
*
* \section Language Bindings
*
* A language binding enables the programmer to use libsax in another
* language than C++. Currently the following language bindings exist:
*
* - Perl
* - Python
* - Java
* - CSharp
*
* To give you an overview how simple it is to use libsax in another
* language have a look at the following perl example which is used
* to change the default color depth of the current configuration to
* a value of 24 bit
*
* \code
* #!/usr/bin/perl
*
* use SaX;
*
* sub main {
*     my %section;
*     my @importID = (
*         $SaX::SAX_CARD, $SaX::SAX_DESKTOP, $SaX::SAX_PATH
*     );
*     my $config = new SaX::SaXConfig;
*     foreach my $id (@importID) {
*         $import = new SaX::SaXImport ( $id );
*         $import->setSource ( $SaX::SAX_SYSTEM_CONFIG );
*         $import->doImport();
*         $config->addImport ( $import );
*         $section{$import->getSectionName()} = $import;
*     }
*     my $mDesktop = new SaX::SaXManipulateDesktop (
*         $section{Desktop},$section{Card},$section{Path}
*     );
*     if ($mDesktop->selectDesktop (0)) {
*         $mDesktop->setColorDepth (24);
*     }
*     $config->setMode ($SaX::SAX_MERGE);
*     $config->createConfiguration();
* }
*
* main();
* \endcode
*/

//====================================
// Defines...
//------------------------------------
#define SAX_VERSION "8.1"

//====================================
// Includes...
//------------------------------------
#include "storage.h"
#include "export.h"
#include "import.h"
#include "init.h"
#include "config.h"

//====================================
// Manipulators...
//------------------------------------
#include "keyboard.h"
#include "pointers.h"
#include "desktop.h"
#include "extensions.h"
#include "layout.h"
#include "path.h"
#include "card.h"

//====================================
// Interface...
//------------------------------------
#include "interface.h"

#endif
