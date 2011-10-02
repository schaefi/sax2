#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;

	printf ("Importing data...\n");
	SaXConfig* config = new SaXConfig;
	SaXImport* import = new SaXImport ( SAX_KEYBOARD );
	import -> setSource ( SAX_SYSTEM_CONFIG );
	import -> doImport();
	config -> addImport (import);
	section.insert (
		import->getSectionName(),import
	);
	printf ("Add czech keyboard layout...\n");
	SaXManipulateKeyboard mKeyboard (
		section["Keyboard"]
	);
	if (mKeyboard.selectKeyboard (SAX_CORE_KBD)) {
		QDict<QString> mOptions = mKeyboard.getOptions();
		QDictIterator<QString> it (mOptions);
		for (; it.current(); ++it) {
			printf ("%s : %s\n",it.currentKey().ascii(),it.current()->ascii());
		}
		mKeyboard.addXKBLayout ("cz");
		mKeyboard.getXKBVariant ("bla");
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
