//==================================
// Interface definition for libsax
//----------------------------------
#define NO_OPERATOR_SUPPORT 1
%module SaX
%{
#include "../../sax.h"
%}

//==================================
// Typemaps
//----------------------------------
//==================================
// Allow QString return types
//----------------------------------
%typemap(out) QString {
	if (argvi >= items) EXTEND(sp, 1);
        QByteArray utf8 = $1.toUtf8();
	char *data = const_cast<char*>(utf8.data());
	sv_setpvn($result = sv_newmortal(), data, $1.length());
	++argvi;
}
//==================================
// Allow QString refs as parameters
//----------------------------------
%typemap(in) QString&,QString& (QString temp) {
	STRLEN len;
	temp = SvPV($input,len);
	$1 = &temp;
}
//==================================
// Allow Q3Dict<QString> return types
//----------------------------------
%typemap(out) Q3Dict<QString> {
	HV *hash;
	SV **svs;
	int i   = 0;
	int len = 0;
	hash = newHV();
	len = $1.count();
	svs = (SV**) malloc(len*sizeof(SV*));
	Q3DictIterator<QString> it ($1);
	for (; it.current(); ++it) {
		QString key = it.currentKey();
		QString* val = new QString (*it.current());
		if ( val->isNull()) {
			val = new QString("");
		}
		svs[i] = sv_newmortal();
		hv_store(hash,
			key.toAscii(), key.length(),
			newSVpv(val->toAscii(),val->length()),0
		);
		i++;
	}
	free(svs);
	$result = newRV((SV*)hash);
	sv_2mortal($result);
	argvi++;
}
//==================================
// Allow QList<QString> return types
//----------------------------------
%typemap(out) QList<QString> {
	AV *myav;
	SV **svs;
	int i = 0;
	int len = 0;
	len = $1.count();
	svs = (SV **) malloc(len*sizeof(SV *));
	for (i = 0; i < len ; i++) {
		svs[i] = sv_newmortal();
		sv_setpv((SV*)svs[i],$1.at(i).toAscii());
	};
	myav =  av_make(len,svs);
	free(svs);
	$result = newRV((SV*)myav);
	sv_2mortal($result);
	argvi++;
}
//==================================
// Allow QStringList return types
//----------------------------------
%typemap(out) QStringList {
	AV *myav;
	SV **svs;
	int i = 0;
	int len = 0;
	len = $1.count();
	svs = (SV **) malloc(len*sizeof(SV *));
	for ( QStringList::Iterator it=$1.begin(); it != $1.end(); ++it ) {
		QString item (*it);
		svs[i] = sv_newmortal();
		sv_setpv((SV*)svs[i],item.toAscii());
		i++;
	}
	myav =  av_make(len,svs);
	free(svs);
	$result = newRV((SV*)myav);
	sv_2mortal($result);
	argvi++;
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
