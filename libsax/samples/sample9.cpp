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
	int panelID = dev.addInputDevice (SAX_INPUT_TOUCHPANEL);
	printf ("ID: %d is [SAX_INPUT_TOUCHPANEL]: added\n",panelID);

	printf ("Setting up touchpanel data... ");
	SaXManipulateTouchscreens pointer (
		section["Pointers"]
	);
	if (pointer.selectPointer (panelID)) {
		QList<QString> panelList = pointer.getPanelList();
		QString* myPanel = panelList.at(0);
		pointer.setTouchPanel( *myPanel );
		printf ("TouchPanel: [%s] configured\n", myPanel->ascii());
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
