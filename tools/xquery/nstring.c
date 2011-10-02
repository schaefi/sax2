/***************
FILE          : string.c
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : xquery
              :
BESCHREIBUNG  : string functions for xquery to split
              : parts of the modelines 
              :
              :
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "nstring.h"

//=========================================
// make single string "hello" => hello
//-----------------------------------------
void strcut(str string) {
 str save="";
 int l,a=0;
 int n=0;

 l=strlen(string);
 if (string[0]=='"')
 {
   for (n=1;n<l-1;n++)
   {
    save[a]=string[n];
    a++;
   }
   save[a]='\0';
   strcpy(string,save);
 }
}

//=========================================
// Transform string to lower case
//-----------------------------------------
void strtolower(str string) {
 int l=strlen(string);
 int x=0;
 for (x=0;x<l;x++)
 {
  if ((char)string[x] >= 65 && (char)string[x] <= 90)
  string[x]=string[x]+32;
 }
}

//=========================================
// Transform string to upper case
//-----------------------------------------
void strtoupper(str string) {
 int l=strlen(string);
 int x=0;
 for (x=0;x<l;x++)
 {
  if ((char)string[x] >= 97 && (char)string[x] <= 122)
  string[x]=string[x]-32;
 }
}

//=========================================
// split the string according to a delimiter
//-----------------------------------------
int strsplit(str string,char deli,str left,str right) {
 int l=strlen(string);
 int start=0;
 str part1="";
 str part2="";
 int found=0;
 int n;

 for (n=start;n<=l;n++)
 {
  if (string[n] != deli) part1[n]=string[n];
   else { start=n+1; part1[n]='\0'; found=1; break; }
 }
 for (n=start;n<=l;n++)
  part2[n-start] = string[n];

 part2[l+1] = '\0';
 strcpy(left,part1);
 strcpy(right,part2);
 if (found == 0) return(-1);
  else return(0);
}

