#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../sax.h"

using namespace LML;

saxPluglib* loadLibrary (void) {
	void* handle = dlopen (
		"../libsax.so", RTLD_LAZY | RTLD_GLOBAL
	);
	if (! handle) {
		printf ("%s\n", dlerror ());
		exit ( EXIT_FAILURE );
	}
	EntryPoint* entrypoint = (EntryPoint*)dlsym (handle, "entrypoint");
	if (! entrypoint) {
		printf ("%s\n", dlerror ());
		exit ( EXIT_FAILURE );
	}
	saxPluglib* LiMal = (saxPluglib*)entrypoint->factory();
	return LiMal;
}

int main (void) {
	saxPluglib* LiMal = loadLibrary();
	LiMal -> setDebug();
	int importID[7] = {
		SAX_CARD,
		SAX_DESKTOP,
		SAX_POINTERS,
		SAX_KEYBOARD,
		SAX_LAYOUT,
		SAX_PATH,
		SAX_EXTENSIONS
	};
	QList<SaXImport*> section;
	SaXConfig* config = LiMal->saxConfig();
	for (int id=0; id<7; id++) {
		printf ("Importing data...\n");
		SaXImport* import = LiMal->saxImport (importID[id]);
		import -> setSource (SAX_AUTO_PROBE);
		import -> doImport();
		config -> addImport (import);
		section.append (import);
	}
	printf ("writing configuration\n");
	config -> setMode (SAX_NEW);
	if ( ! config -> createConfiguration() ) {
		printf ("%s\n",config->getParseErrorValue().ascii());
	}
	delete LiMal;
	return 0;
}
