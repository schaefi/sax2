#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;
	int importID[] = {
		SAX_POINTERS,
		SAX_LAYOUT
	};
	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	for (int id=0; id<2; id++) {
		SaXImport* import = new SaXImport ( importID[id] );
		import -> setSource ( SAX_SYSTEM_CONFIG );
		import -> doImport();
		config -> addImport (import);
		section.insert (
			import->getSectionName(),import
		);
	}
	printf ("Adding new pointer device... ");
	SaXManipulateDevices dev (
		section["Pointers"],section["Layout"]
	);
	int tabletID = dev.addInputDevice (SAX_INPUT_TABLET);
	printf ("ID: %d is [SAX_INPUT_TABLET]: added\n",tabletID);

	printf ("Setting up tablet data... ");
	SaXManipulateTablets pointer (
		section["Pointers"],section["Layout"]
	);
	if (pointer.selectPointer (tabletID)) {
		QList<QString> tabletList = pointer.getTabletList();
		QList<QString> penList = pointer.getPenList();
		QString* myTablet = tabletList.at (3);
		QString* myPen = penList.at(3);
		pointer.setTablet( *myTablet );
		pointer.addPen ( *myPen );
		printf ("Tablet: [%s] with pen: [%s] configured\n",
			myTablet->ascii(),myPen->ascii()
		);
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
