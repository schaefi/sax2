//==================================
// Interface definition for SPP
//----------------------------------
%module SPP
%{
#include "spp.h"
%}

//==================================
// Typemaps
//----------------------------------
//==================================
// Allow QString return types
//----------------------------------
%typemap(out) QString {
	if (argvi >= items) EXTEND(sp, 1);
	char *data = const_cast<char*>($1.data());
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

%include "spp.h"
