/**************
FILE          : spp.l
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 profile parser lexical analysis
              : ----
              :
STATUS        : Status: Development
**************/
%{
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <qlist.h>
#include <q3dict.h>
#include <qstring.h>

#include "spp.tab.hc"

//===================================
// Defines
//-----------------------------------
#define copylv strcpy(yylval.string,yytext)
#define inclet yyletter+=yyleng
#define YY_NO_UNPUT 1

//===================================
// Global Variables
//-----------------------------------
int   yystdin  = 1;
int   lineno   = 1;
int   yyletter = 0;
int   yystop   = 0;
char* yyline   = 0;

//===================================
// Global Variables
//-----------------------------------
Q3Dict<QString> keyHash;
Q3Dict<QString> varHash;

//===================================
// Functions
//-----------------------------------
int evalGetInput (char *buf, int maxlen) {
	int result = 0;
	if (yystop)  {
		buf[0]=EOF;
		return 0;
	}
	if (yystdin) {
		char prompt[80];
		sprintf (prompt,"(%d) SaX: ",lineno);
		yyline = readline (prompt);
		if ((! yyline) || ((strcmp(yyline,"exit") == 0))) {
			buf[0]=EOF;
			return 0;
		}
		result = strlen(yyline);
		if (result > 0) {
			add_history (yyline);
		}
		strncpy (buf,yyline,result);
		buf[result]='\n';
		result++;
	} else {
		if ((! yyline) || ((strcmp(yyline,"exit") == 0))) {
			buf[0]=EOF;
			return 0;
		}
		result = strlen(yyline);
		strncpy (buf,yyline,result);
		buf[result]='\n';
		result++;
	}
	return result;
}

//===================================
// Defines
//-----------------------------------
#undef YY_INPUT
#define YY_INPUT(buf, retval, maxlen) (retval = evalGetInput(buf, maxlen))

%}
 /*=================================*/
 /* Patterns                        */
 /*---------------------------------*/
L [ \t]+
S [a-zA-Z0-9\" \{\}\$\[\]\.,\-_;:#\+\/\@\~]*$
K [a-zA-Z0-9_]*
E [a-zA-Z0-9_ ]*$
V ^\$[a-zA-Z0-9]+

%%

 /*=================================*/
 /* Lokal Variables                 */
 /*---------------------------------*/
 /*int letter = 0;*/

 /*=================================*/
 /* ignore empty lines              */
 /*---------------------------------*/
^{L}*\n         { lineno++; copylv; return (RET); }

 /*=================================*/
 /* remove EOL at end of line       */
 /*---------------------------------*/
\n              { lineno++; copylv; return (RET); }

 /*=================================*/
 /* remove tabs and spaces          */
 /*---------------------------------*/
{L}             { inclet; };

 /*=================================*/
 /* setup input tokens              */
 /*---------------------------------*/
"->"            { copylv; inclet; return(ARROW);    }
"="             { copylv; inclet; return(EQUALS);   }
"!help"         { copylv; inclet; return(HELP);     }
"!remove"       { copylv; inclet; return(REMOVE);   }
{K}             { copylv; inclet; return(KEY);      }
{E}             { copylv; inclet; return(ENDKEY);   }
{S}             { copylv; inclet; return(STRING);   }
"\{"            { copylv; inclet; return(OB);       }
"\}"            { copylv; inclet; return(CB);       }
{V}             { copylv; inclet; return(VARIABLE); }
.               { copylv; inclet; return(TRASH);    }

%%
