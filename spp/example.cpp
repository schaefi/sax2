/**************
FILE          : example.cpp
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

int main (void) {
	SPPParse p;

	#if 1
	p.parseLine("Device->0->DDD=22");
	QList<QString> action = p.getAction();
	if (!(action.at(0).isNull())) {
		printf ("Action: %s : %s\n",
			action.at(0).toAscii(),action.at(1).toAscii()
		);
	} else {
		printf ("no action\n");
	}
	#endif


	#if 0
	while (p.parse()) {
		printf ("fine\n");
		QList<QString> action = p.getAction();
		if (action.at(0)) {
			printf ("Action: %s : %s\n",
				action.at(0)->ascii(),action.at(1)->ascii()
			);
		} else {
			printf ("no action\n");
		}
	}
	#endif

	return 0;
}
