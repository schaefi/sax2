#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;

	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	SaXImport* import = new SaXImport ( SAX_CARD );
	import -> setSource ( SAX_SYSTEM_CONFIG );
	import -> doImport();
	config -> addImport (import);
	section.insert (
		import->getSectionName(),import
	);
	printf ("Overwrite PanelSize option...\n");
	SaXManipulateCard mCard (
		section["Card"]
	);
	if (mCard.selectCard (0)) {
		mCard.addCardOption ("PanelSize","1280x1024");
	}
	printf ("Writing configuration\n");
	config -> setMode (SAX_MERGE);
	if ( ! config -> createConfiguration() ) {
		printf ("%s\n",config->errorString().ascii());
		printf ("%s\n",config->getParseErrorValue().ascii());
		return 1;
	}
	return 0;
}
