/**************
FILE          : spp.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 profile parser 
              :
			  :
STATUS        : development
**************/
#include "spp.h"
#include <qstring.h>

#undef XS
//=====================================
// Constructor...
//-------------------------------------
SPPParse::SPPParse ( void ) {
	// .../
	// Constructing a SPPParse object will setup the
	// readline history and provide access to member
	// functions for parsing the SaX profile syntax
	// ----
	using_history();
}

//=====================================
// getVariables
//-------------------------------------
QList<QString> SPPParse::getVariables ( void ) {
	// .../
	// This method returns the variable dictionary
	// ----
	QList<QString> result;
	Q3DictIterator<QString> it (varHash);
	for (; it.current(); ++it) {
		result.append (QString(it.currentKey()));
		result.append (QString(*it.current()));
	}
	varHash.clear();
	return result;
}

//=====================================
// getAction
//-------------------------------------
QList<QString> SPPParse::getAction ( void ) {
	// .../
	// This method will return a list with the action to
	// be done for the current parse line. The list contains
	// the key and a value whereas the value may be a real
	// value or a request to do something different
	// ----
	QList<QString> result;
	Q3DictIterator<QString> it (keyHash);
	for (; it.current(); ++it) {
		result.append (QString(it.currentKey()));
		result.append (QString(*it.current()));
	}
	keyHash.clear();
	return result;
}

//=====================================
// parse
//-------------------------------------
bool SPPParse::parse ( void ) {
	// .../
	// This method is used to interactively parse one
	// line. A readline featured shell prompt will appear
	// and you can enter some data from stdin
	// ----
	yyparse();
	if (yystop == 1) {
		yystop = 0;
		return true;
	}
	return false;
}

//=====================================
// parse
//-------------------------------------
bool SPPParse::parseLine ( const QString& line ) {
	// .../
	// This method is used to parse the given string
	// by writing a temp file and assigning it to stdin
	// ----
	if (line.isEmpty()) {
		return true;
	}
	yyline = (char*)malloc (sizeof(char)*line.length()+1);
	strcpy (yyline,(line.toLatin1()));
	yystdin = 0;
	bool status = parse();
	yystdin = 1;
	return status;
}

//=====================================
// reload
//-------------------------------------
void SPPParse::reload ( void ) {
	// .../
	// This method is used to reset the parser to its
	// default values
	// ----
	lineno   = 1;
	yyletter = 0;
	yystop   = 0;
	yyline   = 0;
	yystdin  = 1;
	keyHash.clear();
	varHash.clear();
}

