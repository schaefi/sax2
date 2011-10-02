/**************
FILE          : exception.h
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
#ifndef SAX_EXCEPTION_H
#define SAX_EXCEPTION_H 1

//====================================
// Main Includes...
//------------------------------------
#include <qobject.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

//====================================
// namespace SaX
//------------------------------------
/*! \brief SaX -  SaX namespace.
*
* This namespace contains all objects prefixed with the string
* SaX. The namespace will be automatically used by including the
* main sax.h header file which includes interface.h which itself
* makes use of the namespace SaX
*/
namespace SaX {
//====================================
// Defines (macros)...
//------------------------------------
#define qError(msg,exc)  handle_error(__FILE__, __LINE__, msg,exc)

//====================================
// Defines (exception names)...
//------------------------------------
#define EXC_PROCESSFAILED               "saxProcessFailed"
#define EXC_IMPORTSECTIONFAILED         "saxImportSectionFailed"
#define EXC_EXPORTSECTIONFAILED         "saxExportSectionFailed"
#define EXC_FILEOPENFAILED              "saxFileOpenFailed"
#define EXC_NOSTORAGE                   "saxNoStorage"
#define EXC_CDBFILEFAILED               "saxCDBFileFailed"
#define EXC_PERMISSIONDENIED            "saxPermissionDenied"
#define EXC_PROFILENOTFOUND             "saxProfileNotFound"
#define EXC_PROFILEUNDEFINED            "saxProfileUndefined"
#define EXC_IMPORTALREADYADDED          "saxImportAlreadyAdded"
#define EXC_UNKNOWNIMPORT               "saxUnknownImport"
#define EXC_NOAPIFILEFOUND              "saxNoAPIFileFound"
#define EXC_NULLPOINTERARGUMENT         "saxNullPointerArgument"
#define EXC_DESKTOPIMPORTBINDFAILED     "saxDesktopImportBindFailed"
#define EXC_CARDIMPORTBINDFAILED        "saxCardImportBindFailed"
#define EXC_POINTERIMPORTBINDFAILED     "saxPointerImportBindFailed"
#define EXC_KEYBOARDIMPORTBINDFAILED    "saxKeyboardImportBindFailed"
#define EXC_PATHIMPORTBINDFAILED        "saxPathImportBindFailed"
#define EXC_EXTENSIONSIMPORTBINDFAILED  "saxExtensionsImportBindFailed"
#define EXC_LAYOUTIMPORTBINDFAILED      "saxLayoutImportBindFailed"
#define EXC_XKBLOADRULESFAILED          "saxXKBLoadRulesFailed"
#define EXC_CDBRECORDNOTFOUND           "saxCDBRecordNotFound"
#define EXC_WRONGINPUTFASHION           "saxWrongInputFashion"
#define EXC_POINTERFASHIONTYPEFAILED    "saxPointerFashionTypeFailed"
#define EXC_SETSTORAGEIDFAILED          "saxSetStorageIDFailed"
#define EXC_INVALIDARGUMENT             "saxInvalidArgument"
#define EXC_INVALIDARGUMENT             "saxInvalidArgument"
#define EXC_LOCKSETFAILED               "saxLockSetFailed"
#define EXC_LOCKUNSETFAILED             "saxLockUnsetFailed"
#define EXC_GETSCREENLAYOUTFAILED       "saxGetScreenLayoutFailed"
#define EXC_GETINPUTLAYOUTFAILED        "saxGetInputLayoutFailed"
#define EXC_EMPTYCDBGROUP               "saxEmptyCDBGroup"
#define EXC_NVIDIADRIVERMISSING         "saxNvidiaDriverMissing"
#define EXC_NVIDIADRIVERINSTALLED       "saxNvidiaDriverInstalled"
#define EXC_XKBLAYOUTUNDEFINED          "saxXKBLayoutUndefined"
#define EXC_DRIVERMISMATCH              "saxCDBDriverMismatch"

//====================================
// Defines (error texts)...
//------------------------------------
#define EISAXIMPORT      "No such ISaX section to import"
#define EISAXEXPORT      "No such ISaX file to export"
#define EPROFILE         "No such profile ID"
#define EFASHION         "Bad InputFashion type"
#define ECDBGROUP        "No such CDB group found"
#define ERECORD          "No such record"
#define ESCREEN          "No layout defined for screen"
#define ECDBDATA         "No CDB record found"
#define ENVIDIAMIS       "NVIDIA binary driver missing"
#define ENVIDIAINS       "NVIDIA binary driver installed"
#define EXKBLAYOUT       "Undefined XKB layout"
#define ECDBMISMATCH     "2D/3D driver from CDB doesn't match current driver"

//====================================
// Defines (error codes)...
//------------------------------------
#define EISAXIMPORTID    256
#define EISAXEXPORTID    257
#define EPROFILEID       258
#define ECDBGROUPID      259
#define EFASHIONID       260
#define ERECORDID        261
#define ESCREENID        262
#define ECDBDATAID       263
#define ENVIDIAMISID     264
#define ENVIDIAINSID     265
#define EXKBLAYOUTID     266
#define ECDBMISMATCHID   267

//====================================
// Defines (locking)...
//------------------------------------
#define ZERO_DEV        "/dev/zero"

//====================================
// Functions...
//------------------------------------
void handle_error (const char*, int , const char*, const char*);

//====================================
// pre Class definitions...
//------------------------------------
class SaXImport;

//====================================
// Interface class for dlopen ability
//------------------------------------
/*! \brief SaX2 -  Exception class interface.
*
* The interface class is provided to be able to dlopen the
* library and have all methods available in the compilers
* virtual table. For a detailed description of the class itself
* please refer to the derived class definition
*/
class SaXExceptionIF : public QObject {
	public:
	virtual void setDebug ( bool=true ) = 0;

	public:
	virtual int   errorCode      ( void ) = 0;
	virtual void  errorReset     ( void ) = 0;
	virtual bool  havePrivileges ( void ) = 0;

	public:
	virtual QString errorString ( void ) = 0;
    virtual QString errorValue  ( void ) = 0;

	public:
	virtual void excProcessFailed              ( void ) = 0;
	virtual void excImportSectionFailed        ( void ) = 0;
	virtual void excExportSectionFailed        ( void ) = 0;
	virtual void excFileOpenFailed             ( int  ) = 0;
	virtual void excNoStorage                  ( int  ) = 0;
	virtual void excCDBFileFailed              ( void ) = 0;
	virtual void excPermissionDenied           ( void ) = 0;
	virtual void excProfileNotFound            ( void ) = 0;
	virtual void excProfileUndefined           ( int  ) = 0;
	virtual void excImportAlreadyAdded         ( int  ) = 0;
	virtual void excUnknownImport              ( SaXImport* ) = 0;
	virtual void excNoAPIFileFound             ( void ) = 0;
	virtual void excNullPointerArgument        ( void ) = 0;
	virtual void excXKBLoadRulesFailed         ( void ) = 0;
    virtual void excCDBRecordNotFound          ( const char* ) = 0;
    virtual void excWrongInputFashion          ( const char* ) = 0;
    virtual void excSetStorageIDFailed         ( int ) = 0;
    virtual void excInvalidArgument            ( int ) = 0;
	virtual void excInvalidArgument            ( const char* ) = 0;
	virtual void excDesktopImportBindFailed    ( int ) = 0;
	virtual void excCardImportBindFailed       ( int ) = 0;
	virtual void excPointerImportBindFailed    ( int ) = 0;
	virtual void excKeyboardImportBindFailed   ( int ) = 0;
	virtual void excPathImportBindFailed       ( int ) = 0;
	virtual void excLayoutImportBindFailed     ( int ) = 0;
	virtual void excExtensionsImportBindFailed ( int ) = 0;
	virtual void excPointerFashionTypeFailed   ( const char* ) = 0;
	virtual void excLockSetFailed              ( int ) = 0;
	virtual void excLockUnsetFailed            ( int ) = 0;
	virtual void excGetScreenLayoutFailed      ( int ) = 0;
	virtual void excEmptyCDBGroup              ( const char* ) = 0;
	virtual void excGetInputLayoutFailed       ( void ) = 0;
	virtual void excNvidiaDriverMissing        ( void ) = 0;
	virtual void excNvidiaDriverInstalled      ( void ) = 0;
	virtual void excXKBLayoutUndefined         ( const char* ) = 0;
	virtual void excDriverMismatch             ( const char*,const char* ) = 0;

	public:
	virtual ~SaXExceptionIF ( void ) { }
};
//====================================
// Class SaXException
//------------------------------------
/*! \brief SaX2 -  Exception class
*
* The SaXException class is a container for all exceptions of the
* library. It provides methods to set and retrieve error codes and
* error strings. Within the library each class which may run into
* an error inherits from SaXException and therefore each SaX* class
* stores their errors inside the object instance. The programmer is
* able to use the methods obj->errorCode() and obj->errorString() to
* obtain possible errors or the programmer can create its own
* exception class which needs to inherit from SaXException. While
* constructing an object of this exception class the programmer
* needs to pass the SaX* object which should be watched to the constructor
* of its own class. If an error occurs the appropriate exception method is
* called. The following example will show how to make use of the libsax
* exception handling:
*
* my_excpetion.h
* \code
* class myException : public SaXException {
*    Q_OBJECT
*
*     public:
*     myException (SaXException*);
*
*     private slots:
*     void processFailed ( void );
*     void permissionDenied ( void );
* };
* \endcode
*
* my_exception.cpp
* \code
* #include "my_exception.h"
*
* myException::myException (SaXException* mException)  {
*     connect (
*         mException, SIGNAL (saxProcessFailed (void)),
*         this      , SLOT   (processFailed (void))
*     );
*     connect (
*         mException, SIGNAL (saxPermissionDenied (void)),
*         this      , SLOT   (permissionDenied (void))
*     );
* }
* 
* void myException::processFailed (void) {
*     printf ("Process failed...\n");
* }
* 
* void myException::permissionDenied (void) {
*     printf ("Permission denied...\n");
* }
* \endcode
*
* sample.cpp
* \code
* int main (void) {
*     SaXException::setDebug();
* 
*     SaXInit* init = new SaXInit;
*     myException* e = new myException ( init );
*     ...
* }
* \endcode
*/
class SaXException : public SaXExceptionIF {
	Q_OBJECT

	private:
	QString* mErrorString;
	QString* mErrorValue;
	int      mErrorCode;
	int      mLFD;

	private:
	void setErrorCode    ( int );
	void setErrorCode    ( const char*,int );
	void setErrorValue   ( int );
	void setErrorValue   ( const char* );
	void setErrorValue   ( void* );

	public:
	void setDebug ( bool=true );

	public:
	int   errorCode      ( void );
	void  errorReset     ( void );
	bool  havePrivileges ( void );

	public:
	QString errorString ( void );
	QString errorValue  ( void );

	public:
	void excProcessFailed              ( void );
	void excImportSectionFailed        ( void );
	void excExportSectionFailed        ( void );
	void excFileOpenFailed             ( int  );
	void excNoStorage                  ( int  );
	void excCDBFileFailed              ( void );
	void excPermissionDenied           ( void );
	void excProfileNotFound            ( void );
	void excProfileUndefined           ( int  );
	void excImportAlreadyAdded         ( int  );
	void excUnknownImport              ( SaXImport* );
	void excNoAPIFileFound             ( void );
	void excNullPointerArgument        ( void );
	void excXKBLoadRulesFailed         ( void );
    void excCDBRecordNotFound          ( const char* );
    void excWrongInputFashion          ( const char* );
    void excSetStorageIDFailed         ( int );
    void excInvalidArgument            ( int );
	void excInvalidArgument            ( const char* );
	void excDesktopImportBindFailed    ( int );
	void excCardImportBindFailed       ( int );
	void excPointerImportBindFailed    ( int );
	void excKeyboardImportBindFailed   ( int );
	void excPathImportBindFailed       ( int );
	void excLayoutImportBindFailed     ( int );
	void excExtensionsImportBindFailed ( int );
	void excPointerFashionTypeFailed   ( const char* );
	void excLockSetFailed              ( int );
	void excLockUnsetFailed            ( int );
	void excGetScreenLayoutFailed      ( int );
	void excEmptyCDBGroup              ( const char* );
	void excGetInputLayoutFailed       ( void );
	void excNvidiaDriverMissing        ( void );
	void excNvidiaDriverInstalled      ( void );
	void excXKBLayoutUndefined         ( const char* );
	void excDriverMismatch             ( const char*,const char* );

	signals:
	void saxProcessFailed              ( void );
	void saxImportSectionFailed        ( void );
	void saxExportSectionFailed        ( void );
	void saxFileOpenFailed             ( int  );
	void saxNoStorage                  ( int  );
	void saxCDBFileFailed              ( void );
	void saxPermissionDenied           ( void );
	void saxProfileNotFound            ( void );
	void saxProfileUndefined           ( int  );
	void saxImportAlreadyAdded         ( int  );
	void saxUnknownImport              ( SaXImport* );
	void saxNoAPIFileFound             ( void );
	void saxNullPointerArgument        ( void );
	void saxDesktopImportBindFailed    ( int  );
	void saxCardImportBindFailed       ( int  );
    void saxPointerImportBindFailed    ( int  );
    void saxKeyboardImportBindFailed   ( int  );
    void saxPathImportBindFailed       ( int  );
    void saxExtensionsImportBindFailed ( int  );
	void saxLayoutImportBindFailed     ( int  );
	void saxXKBLoadRulesFailed         ( void );
	void saxCDBRecordNotFound          ( const char* );
	void saxWrongInputFashion          ( const char* );
	void saxPointerFashionTypeFailed   ( const char* );
	void saxSetStorageIDFailed         ( int  );
	void saxInvalidArgument            ( int  );
	void saxInvalidArgument            ( const char* );
	void saxLockSetFailed              ( int  );
	void saxLockUnsetFailed            ( int  );
	void saxGetScreenLayoutFailed      ( int  );
	void saxEmptyCDBGroup              ( const char* );
	void saxGetInputLayoutFailed       ( void );
	void saxNvidiaDriverMissing        ( void );
	void saxNvidiaDriverInstalled      ( void );
	void saxXKBLayoutUndefined         ( const char* );
	void saxDriverMismatch             ( const char*,const char* );

	signals:
	void saxGlobalException            ( const char* );

	protected:
	bool setLock   ( void );
	bool unsetLock ( void );

	public:
	SaXException ( void );
};
} // end namespace
#endif
