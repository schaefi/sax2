/**************
FILE          : dots.c
***************
PROJECT       : SaX2 ( SuSE advanced X11 configuration )
              :
              :
DESCRIPTION   : simple effect generator to show the
              : loading progress. This program should be
              : run with nice level 20 
              : 
STATUS        : Status: Up-to-date
**************/
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <term.h>
#include <signal.h>
#include <unistd.h>

char *ti(const char *name);
void gotoxy(int x,int y);
void clrscr (void);
void sigterm(int s);
void sighup(int s);

char prompt[6] = "SaX: ";
int  stop      = 0;

int main (int argc, char* argv[]) {
 const char spinner[] = "\\|/-";
 char *term;
 int i;

 // install signal handler
 // -------------------------
 signal (SIGHUP  , sighup);
 signal (SIGTERM , sigterm);
 signal (SIGINT  , sigterm);

 // get and init terminal...
 // --------------------------
 term = getenv("TERM");
 setupterm(term, 1, (int *)0);

 // disable cursor...
 // -------------------
 putp((char*)ti("civis"));


 if (argc <= 1) {
  // show progress...
  // -----------------
  while(1) {
   for (i=0;i<4;i++) {
    if (stop == 0) {
     printf("%s%c",prompt,spinner[i]);
     printf("\r");
     fflush(stdout);
    }
    usleep(10000);
   }
  } 
 } else {

  printf("\r");
  printf("%s",argv[1]);
  while(1) {
   printf("."); fflush(stdout);
   sleep(1);
  }

 }
}

void sigterm(int s) {
 putp((char*)ti("cnorm"));
 exit(0);
}

void sighup(int s) {
 if (stop == 0) {
  putp((char*)ti("cnorm"));
  stop = 1;
 } else {
  putp((char*)ti("civis"));
  stop = 0;
 }
}

char *ti(const char *name) {
 char *buf_ptr;
 if ((buf_ptr = tigetstr(name)) == (char *)-1) {
  buf_ptr = NULL;
 }
 return buf_ptr;
}

void gotoxy(int x,int y) {
 printf("\033[%d;%dH",y,x);
}

void clrscr (void) {
 printf("\033[2J");
 gotoxy(0,0);
}

