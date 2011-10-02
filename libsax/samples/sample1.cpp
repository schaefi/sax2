#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	SaXInit init;
	if (init.needInit()) {
		printf ("initialize cache...\n");
		init.doInit();
	}
	QDict<SaXImport> section;
	int importID[] = {
		SAX_CARD,
		SAX_DESKTOP,
		SAX_PATH,
	};
	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	for (int id=0; id<3; id++) {
		SaXImport* import = new SaXImport ( importID[id] );
		import -> setSource ( SAX_SYSTEM_CONFIG );
		import -> doImport();
		config -> addImport (import);
		section.insert (
			import->getSectionName(),import
		);
	}
	printf ("Setting up resolution...\n");
	SaXManipulateDesktop mDesktop (
		section["Desktop"],section["Card"],section["Path"]
	);
	if (mDesktop.selectDesktop (0)) {
		mDesktop.addResolution (24,1600,1200);
		mDesktop.setExtraModeline (1800,2000,90,50);
	}
	// test what happens if we intentionally add an error...
	// section["Desktop"]->removeEntry("Identifier");
	printf ("Writing configuration\n");
	config -> setMode (SAX_NEW);
	if ( ! config -> createConfiguration() ) {
		printf ("\"%s\"\n",config->errorString().ascii());
		printf ("\"%s\"\n",config->getParseErrorValue().ascii());
		printf ("\"%s\"\n",config->getParseError().ascii());
		return 1;
	}
	return 0;
}
