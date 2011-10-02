/**************
FILE          : spp.h
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <q3dict.h>
#include <qlist.h>
#include <qstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//==============================
// Protottypes 
//------------------------------
extern int yyparse (void);

//==============================
// Globals
//------------------------------
extern int yystop;
extern int yystdin;
extern int lineno;
extern int yyletter;
extern char* yyline;

extern Q3Dict<QString> keyHash;
extern Q3Dict<QString> varHash;

//==============================
// Defines
//------------------------------
#define ALLRW   (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

//==============================
// Class SPPParse...
//------------------------------
class SPPParse {
	public:
	bool parseLine ( const QString& );
	bool parse  ( void );
	void reload ( void );

	public:
	QList<QString> getVariables ( void );
	QList<QString> getAction ( void );

	public:
	SPPParse ( void );
};
