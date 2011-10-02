//==================================
// Interface definition for libsax
//----------------------------------
#define NO_OPERATOR_SUPPORT 1
%module SaX
%{
#include "../../sax.h"
%}

//==================================
// SWIG includes
//----------------------------------
%include exception.i

//==================================
// Typemaps
//----------------------------------
//==================================
// Allow QString return types
//----------------------------------
%typemap(jni)    QString "jstring"
%typemap(jtype)  QString "String"
%typemap(jstype) QString "String"
%typemap(out) QString {
	$result = jenv->NewStringUTF($1.toUtf8().constData());
}
%typemap(javain)  QString  "$javainput"
%typemap(javaout) QString {
    return $jnicall;
}
%typemap(typecheck) QSstring  = char*;

//==================================
// Allow QString refs as parameters
//----------------------------------
%typemap(jni)    QString & "jstring"
%typemap(jtype)  QString & "String"
%typemap(jstype) QString & "String"
%typemap(in) QString&,QString& {
	$1 = NULL;
	if ($input) {
		const char *pstr = (const char *)jenv->GetStringUTFChars($input,0);
		if (!pstr) {
			return $null;
		}
		$1 = new QString (pstr);
		jenv->ReleaseStringUTFChars($input, pstr);
	} else {
		SWIG_JavaThrowException(
			jenv, SWIG_JavaNullPointerException, "null string"
		);
		return $null;
	}
}
%typemap(javain)  QString& "$javainput"
%typemap(javaout) QString& {
    return $jnicall;
}
%typemap(typecheck) QSstring& = char*;

//==================================
// Allow QDict<QString> return types
//----------------------------------
%typemap(jni)    QDict<QString> "jobjectArray"
%typemap(jtype)  QDict<QString> "String[]"
%typemap(jstype) QDict<QString> "String[]"
%typemap(out) QDict<QString> {
	int i = 0;
	jstring temp_string;
	const jclass clazz = JCALL1(FindClass, jenv, "java/lang/String");
	jresult = JCALL3 (NewObjectArray, jenv, $1.count() * 2, clazz, NULL);
	QDictIterator<QString> it ($1);
	for (; it.current(); ++it) {
		QString* key = new QString (it.currentKey());
		QString* val = new QString (*it.current());
		temp_string = JCALL1 (NewStringUTF, jenv, (char*)key->ascii());
		JCALL3(SetObjectArrayElement, jenv, jresult, i, temp_string);
		JCALL1(DeleteLocalRef, jenv, temp_string);
		i++;
		temp_string = JCALL1 (NewStringUTF, jenv, (char*)val->ascii());
		JCALL3(SetObjectArrayElement, jenv, jresult, i, temp_string);
		JCALL1(DeleteLocalRef, jenv, temp_string);
		i++;
	}
}
%typemap(javain)  QDict<QString> "$javainput"
%typemap(javaout) QDict<QString>  {
	return $jnicall;
}

//==================================
// Allow QList<QString> return types
//----------------------------------
%typemap(jni)    QList<QString> "jobjectArray"
%typemap(jtype)  QList<QString> "String[]"
%typemap(jstype) QList<QString> "String[]"
%typemap(out) QList<QString> {
	int i = 0;
	jstring temp_string;
	const jclass clazz = JCALL1(FindClass, jenv, "java/lang/String");
	jresult = JCALL3 (NewObjectArray, jenv, $1.count(), clazz, NULL);
	QListIterator<QString> it ($1);
	for (; it.current(); ++it) {
		temp_string = JCALL1 (NewStringUTF, jenv, (char*)it.current()->ascii());
		JCALL3(SetObjectArrayElement, jenv, jresult, i, temp_string);
		JCALL1(DeleteLocalRef, jenv, temp_string);
		i++;
	}
	$result = jresult;
}
%typemap(javain)  QList<QString> "$javainput"
%typemap(javaout) QList<QString>  {
	return $jnicall;
}

//==================================
// Exception class wrapper...
//----------------------------------
class SaXException {
	public:
	int   errorCode          ( void );
	bool  havePrivileges     ( void );
	void  errorReset         ( void );

	public:
	QString errorString  ( void );
	QString errorValue   ( void );

	public:
	void setDebug ( bool = true );
};

//==================================
// ANSI C/C++ declarations...
//----------------------------------
%include "../../storage.h"
%include "../../export.h"
%include "../../process.h"
%include "../../import.h"
%include "../../init.h"
%include "../../config.h"
%include "../../card.h"
%include "../../keyboard.h"
%include "../../pointers.h"
%include "../../desktop.h"
%include "../../extensions.h"
%include "../../layout.h"
%include "../../path.h"

%include "../../sax.h"
