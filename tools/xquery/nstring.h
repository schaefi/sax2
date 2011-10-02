/**************
FILE          : string.h
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : xquery
              :
BESCHREIBUNG  : string functions for xquery to split
              : parts of the modelines  [header]
              :
              :
STATUS        : Status: Up-to-date
**************/
#ifndef STRING_H
#define STRING_H 1

//========================
// Types...
//------------------------
typedef char str[256];

//=====================
// Prototypes...
//---------------------
void strcut(str string);
void strtolower(str string);
void strtoupper(str string);
int strsplit(str string,char deli,str left,str right);

#endif
