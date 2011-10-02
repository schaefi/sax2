#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;
	int importID[] = {
		SAX_CARD,
		SAX_POINTERS,
		SAX_KEYBOARD,
		SAX_LAYOUT,
		SAX_PATH
	};
	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	for (int id=0; id<5; id++) {
		SaXImport* import = new SaXImport ( importID[id] );
		import -> setSource ( SAX_SYSTEM_CONFIG );
		import -> doImport();
		config -> addImport (import);
		section.insert (
			import->getSectionName(),import
		);
	}
	printf ("Exporting display for VNC access...\n");
	SaXManipulateVNC mVNC (
		section["Card"],section["Pointers"],section["Keyboard"],
		section["Layout"],section["Path"]
	);
	if (! mVNC.VNCEnabled()) {
		mVNC.enableVNC();
		mVNC.addVNCKeyboard();
		mVNC.addVNCMouse();
	}
	printf ("writing configuration\n");
	config -> setMode (SAX_MERGE);
	if ( ! config -> createConfiguration() ) {
		printf ("%s\n",config->errorString().ascii());
		printf ("%s\n",config->getParseErrorValue().ascii());
		return 1;
	}
	return (0);
}
