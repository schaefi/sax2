/**************
FILE          : desktop.h
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
#ifndef SAX_DESKTOP_H
#define SAX_DESKTOP_H 1

//====================================
// Includes...
//------------------------------------
#include <math.h>
#include "import.h"
#include "card.h"
#include "path.h"
#include "file.h"

namespace SaX {
//====================================
// Defines...
//------------------------------------
#define XMODE             "/usr/sbin/xmode"
#define XQUERY            "/usr/sbin/xquery"
#define SAX_PROFILE_CHECK "/usr/share/sax/libsax/createCHK.sh"
#define SAX_GRUB_UPDATE   "/usr/share/sax/libsax/createGRB.pl"
#define PROFILE_DIR       "/usr/share/sax/profile/"
#define MAP_DIR           "/usr/share/sax/sysp/maps/"
#define SYSP_VENDOR       "/usr/share/sax/sysp/script/vendor.pl"

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Desktop manipulator class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXManipulateDesktopIF : public SaXException {
	public:
	virtual void calculateModelines ( bool ) = 0;
	virtual void setExtraModelineString ( const QString& ) = 0;
	virtual void setExtraModeline ( int,int,int,int ) = 0;
	virtual void addExtraModeline ( int,int,int,int ) = 0;
	virtual void removeExtraModeline ( int,int ) = 0;
	virtual void setResolution ( int,int,int ) = 0;
	virtual void addResolution ( int,int,int ) = 0;
	virtual void removeResolution ( int,int,int ) = 0;
	virtual void setVirtualResolution ( int,int,int ) = 0;
	virtual void removeVirtualResolution ( int ) = 0;
	virtual void setColorDepth ( int ) = 0;
	virtual bool enable3D  ( void ) = 0;
	virtual bool disable3D ( void ) = 0;
	virtual void setDisplaySize ( int,int ) = 0;
	virtual void setDisplayRatioAndTraversal ( double ,int, int ) = 0;
	virtual void setHsyncRange ( double,double ) = 0;
	virtual void setVsyncRange ( double,double ) = 0;
	virtual void setPreferredMode ( const QString& ) = 0;
	virtual void enableDPMS  ( void ) = 0;
	virtual void disableDPMS ( void ) = 0;
	virtual void setMonitorVendor ( const QString& ) = 0;
	virtual void setMonitorName   ( const QString& ) = 0;
	virtual void setCDBMonitor ( const QString& ) = 0;
	virtual QList<QString> getCDBMonitorVendorList ( void ) = 0;
	virtual QList<QString> getCDBMonitorModelList  ( const QString& ) = 0;
	virtual Q3Dict<QString> getCDBMonitorData (
		const QString&,const QString&
	) = 0;
	virtual Q3Dict<QString> getCDBMonitorIDData (
		const QString&
	) = 0;
	virtual void setCDBMonitorData (
		const QString&, const QString&,
		const QString&, const QString&
	) = 0;

	public:
	virtual QList<QString> getResolutions ( int  ) = 0;
	virtual QList<QString> getResolutionsFromFrameBuffer (void) = 0;
	virtual QList<QString> getResolutionFromServer ( void ) = 0;
	virtual QList<QString> getResolutionsFromDDC1  ( void ) = 0;
	virtual QList<QString> getResolutionsFromDDC2  ( void ) = 0;
	virtual QList<QString> getDisplaySize ( void ) = 0;
	virtual QList<QString> getDisplayRatio( void ) = 0;
	virtual QString getDisplayTraversal   ( void ) = 0;
	virtual QList<QString> getHsyncRange  ( void ) = 0;
	virtual QList<QString> getVsyncRange  ( void ) = 0;
	virtual bool is3DEnabled    ( void ) = 0;
	virtual bool is3DCard       ( void ) = 0;
	virtual bool isDualHeadCard ( void ) = 0;
	virtual bool isXineramaMode ( void ) = 0;
	virtual bool DPMSEnabled    ( void ) = 0;
	virtual int getFBKernelMode ( const QString&,int ) = 0;
	virtual int setFBKernelMode ( int ) = 0;
	virtual int getColorDepthFromServer ( void ) = 0;
	virtual QString getMonitorVendor ( void ) = 0;
	virtual QString getMonitorName   ( void ) = 0;
	virtual QString getColorDepth    ( void ) = 0;
	virtual QString getVirtualResolution ( int  ) = 0;
	virtual QString getDualHeadProfile   ( void ) = 0;
	virtual QString getModelineAlgorithm ( void ) = 0;
	virtual bool willCalculateModelines  ( void ) = 0;

	public:
	virtual bool selectDesktop (int) = 0;

	public:
	virtual ~SaXManipulateDesktopIF ( void ) { }
};
//====================================
// Class SaXManipulateDesktop...
//------------------------------------
/*! \brief SaX2 -  Desktop manipulator class.
*
* The desktop manipulator requires three import objects to become
* created:
*
* - Desktop
* - Card
* - Path
*
* Once created the manipulator object is able to get/set desktop related
* information like resolutions color depth monitor specs or 3D. The
* following example shows how to use the desktop manipulator for adding
* a new standard resolution in 24 bit color depth:
*
* \code
* #include <sax/sax.h>
*
* int main (void) {
*     SaXException().setDebug (true);
*     QDict<SaXImport> section;
*     int importID[] = {
*         SAX_CARD,
*         SAX_DESKTOP,
*         SAX_PATH,
*     };
*     printf ("Importing data...\n");
*     SaXConfig* config = new SaXConfig;
*     for (int id=0; id<3; id++) {
*         SaXImport* import = new SaXImport ( importID[id] );
*         import->setSource ( SAX_SYSTEM_CONFIG );
*         import->doImport();
*         config->addImport (import);
*         section.insert (
*             import->getSectionName(),import
*         );
*     }
*     printf ("Setting up resolution...\n");
*     SaXManipulateDesktop mDesktop (
*         section["Desktop"],section["Card"],section["Path"]
*     );
*     if (mDesktop.selectDesktop (0)) {
*         mDesktop.addResolution (24,1600,1200);
*     }
*     printf ("Writing configuration\n");
*     config->setMode (SAX_MERGE);
*     if ( ! config->createConfiguration() ) {
*         printf ("%s\n",config->errorString());
*         printf ("%s\n",config->getParseErrorValue());
*         return 1;
*     }
*     return 0;
* }
* \endcode
*/
class SaXManipulateDesktop : public SaXManipulateDesktopIF {
	private:
	SaXImport*     mDesktop;
	SaXImport*     mCard;
	SaXImport*     mPath;
	SaXImportSysp* mSyspDesktop;
	SaXProcess*    mCDBMonitors;
	QList<QString> mCDBMonitorList;
	Q3Dict<QString> mCDBMonitorData;
	int            mDesktopID;

	private:
	QString getVendorForDriver ( const QString& );
	QString calculateModeline  ( int,int,int,int );
	QString getDriverOptionsDualHeadProfile ( const QString& );
	Q3Dict<QString> getMetaData ( void );
	QList<QString> getResolutionsFromDDC ( const QString& );

	public:
	void calculateModelines ( bool );
	void setExtraModelineString ( const QString& );
	void setExtraModeline ( int,int,int,int );
    void addExtraModeline ( int,int,int,int );
    void removeExtraModeline ( int,int );
	void setResolution ( int,int,int );
	void addResolution ( int,int,int );
	void removeResolution ( int,int,int );
	void setVirtualResolution ( int,int,int );
	void removeVirtualResolution ( int );
	void setColorDepth ( int );
	bool enable3D  ( void );
	bool disable3D ( void );
	void setDisplaySize ( int,int );
	void setDisplayRatioAndTraversal ( double ,int, int );
	void setHsyncRange ( double,double );
	void setVsyncRange ( double,double );
	void setPreferredMode ( const QString& );
	void enableDPMS  ( void );
	void disableDPMS ( void );
	void setMonitorVendor ( const QString& );
	void setMonitorName   ( const QString& );
	void setCDBMonitor ( const QString& );
	QList<QString> getCDBMonitorVendorList ( void );
	QList<QString> getCDBMonitorModelList  ( const QString& );
	Q3Dict<QString> getCDBMonitorData ( const QString&, const QString& );
	Q3Dict<QString> getCDBMonitorIDData ( const QString& );
	void setCDBMonitorData (
		const QString&, const QString&,
		const QString&, const QString&
	);

	public:
	QList<QString> getResolutions ( int  );
	QList<QString> getResolutionsFromFrameBuffer (void);
	QList<QString> getResolutionFromServer ( void );
	QList<QString> getResolutionsFromDDC1 ( void );
	QList<QString> getResolutionsFromDDC2 ( void );
	QList<QString> getDisplaySize ( void );
	QList<QString> getDisplayRatio( void );
	QString getDisplayTraversal   ( void );
	QList<QString> getHsyncRange  ( void );
	QList<QString> getVsyncRange  ( void );
	bool is3DEnabled    ( void );
	bool is3DCard       ( void );
	bool isDualHeadCard ( void );
	bool isXineramaMode ( void );
	bool DPMSEnabled    ( void );
	int getFBKernelMode ( const QString&,int );
	int setFBKernelMode ( int );
	int getColorDepthFromServer ( void );
	QString getMonitorVendor ( void );
	QString getMonitorName   ( void );
	QString getColorDepth    ( void );
	QString getVirtualResolution ( int  );
	QString getDualHeadProfile   ( void );
	QString getModelineAlgorithm ( void );
	bool willCalculateModelines  ( void );

	public:
	bool selectDesktop (int);

	public:
	SaXManipulateDesktop (
		SaXImport*, SaXImport*, SaXImport*, int = 0
	);
};
} // end namespace
#endif
