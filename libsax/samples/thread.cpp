#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include "../sax.h"
#include "../exception.h"

void* myFunction (void*);

int main (void) {
	pthread_t outThreadID1;
	pthread_t outThreadID2;
	printf ("starting first thread\n");
	pthread_create (&outThreadID1, 0, myFunction, 0);

	usleep (500000);

	printf ("starting second thread\n");
	pthread_create (&outThreadID2, 0, myFunction, 0);

	pthread_join (outThreadID1,NULL);
	pthread_join (outThreadID2,NULL);

	return 0;
}

void* myFunction (void*) {
	QDict<SaXImport> section;
	int importID[7] = {
        SAX_CARD,
        SAX_DESKTOP,
        SAX_POINTERS,
        SAX_KEYBOARD,
        SAX_LAYOUT,
        SAX_PATH,
        SAX_EXTENSIONS
    };
	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	for (int id=0; id<7; id++) {
		SaXImport* import = new SaXImport ( importID[id] );
		import -> setSource ( SAX_SYSTEM_CONFIG );
		import -> doImport();
		config -> addImport (import);
		printf ("_______Importing %s\n",import->getSectionName().ascii());
		section.insert (
			import->getSectionName(),import
		);
	}
	SaXManipulateDesktop mDesktop (
		section["Desktop"],section["Card"],section["Path"]
	);
	QString vendor = mDesktop.getMonitorVendor();
	printf ("Monitor Vendor is: %s\n",vendor.ascii());
	pthread_exit (0);
}
