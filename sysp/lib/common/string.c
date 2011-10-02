/**************
FILE          : xtest.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : common used functions: 
              : GetBusFormat -> get busid as DEC data
              : GetDValue    -> format hex string to dec value
              :
STATUS        : development
**************/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../syslib.h"

//====================================
// GetDValue: format hex string to D
//------------------------------------
int GetDValue(char *hex) {
 int  i,n;
 int  base  = 16;
 int  value = 0;
 int  sum   = 0;

 n = 0;
 for (i=strlen(hex)-1;i>=0;i--) {
  switch(hex[i]) {
   case '0' : value = 0;  break;
   case '1' : value = 1;  break;
   case '2' : value = 2;  break;
   case '3' : value = 3;  break;
   case '4' : value = 4;  break;
   case '5' : value = 5;  break;
   case '6' : value = 6;  break;
   case '7' : value = 7;  break;
   case '8' : value = 8;  break;
   case '9' : value = 9;  break;
   case 'A' : value = 10; break;
   case 'a' : value = 10; break;
   case 'B' : value = 11; break;
   case 'b' : value = 11; break;
   case 'C' : value = 12; break;
   case 'c' : value = 12; break;
   case 'D' : value = 13; break;
   case 'd' : value = 13; break;
   case 'E' : value = 14; break;
   case 'e' : value = 14; break;
   case 'F' : value = 15; break;
   case 'f' : value = 15; break;
   default: return(-1);
  }
  sum = sum + (value * (int)pow(base,n));
  n++;
 }
 return(sum);
}

//====================================
// GetBusFormat: format busid string
//------------------------------------
void GetBusFormat(char *idstr,int *bus,int* slot,int *func) {
 int i;
 char bus_str[3]  = "";
 char slot_str[3] = "";
 char func_str[2] = "";

 for (i=0;i<(int)strlen(idstr);i++) {
  if (idstr[i] == ' ') {
   bus_str[0]  = idstr[i+1];
   bus_str[1]  = idstr[i+2];
   bus_str[2]  = '\0';

   slot_str[0] = idstr[i+4];
   slot_str[1] = idstr[i+5];
   slot_str[2] = '\0';

   func_str[0] = idstr[i+7];
   func_str[1] = '\0';
   break;
  }
 }
 *bus  = GetDValue(bus_str);
 *slot = GetDValue(slot_str);
 *func = GetDValue(func_str);
}

