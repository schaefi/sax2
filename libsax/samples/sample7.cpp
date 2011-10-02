#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;

	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	SaXImport* import = new SaXImport ( SAX_PATH );
	import -> setSource ( SAX_SYSTEM_CONFIG );
	import -> doImport();
	config -> addImport (import);
	section.insert (
		import->getSectionName(),import
	);
	printf ("Disable VT switching...\n");
	SaXManipulatePath mPath (
		section["Path"]
	);
	mPath.addServerFlag ("DontVTSwitch");

	printf ("Writing configuration\n");
	config -> setMode (SAX_MERGE);
	if ( ! config -> createConfiguration() ) {
		printf ("%s\n",config->errorString().ascii());
		printf ("%s\n",config->getParseErrorValue().ascii());
		return 1;
	}
	return 0;
}
