#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../syslib.h"

int main (int argc,char*argv[]) {
	if (! argv[1]) {
		printf ("No logfile given... abort\n");
		exit (1);
	}
	MsgDetect* parse = PLogGetData(argv[1]);
	if (! parse) {
		printf ("Log not found... abort\n");
		exit (1);
	}
	printf ("%p %ld\n",parse,parse->memory);
	exit (0);
}
