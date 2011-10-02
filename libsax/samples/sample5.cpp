#include "../sax.h"

int main (void) {
	SaXException().setDebug (true);
	QDict<SaXImport> section;
	int importID[] = {
		SAX_LAYOUT,
		SAX_CARD
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
	printf ("checking multihead mode... ");
	SaXManipulateLayout layout (
		section["Layout"],section["Card"]
	);
	switch (layout.getMultiheadMode()) {
		case SAX_SINGLE_HEAD:
			printf ("single head mode\n");
		break;
		case SAX_XINERAMA:
			printf ("Xorg Xinerama mode\n");
		break;
		case SAX_TRADITIONAL:
			printf ("Xorg traditional multihead\n");
		break;
		case SAX_CLONE:
			printf ("Xorg cloned multihead\n");
		break;
		case SAX_TWINVIEW:
			printf ("Nvidia Twinview mode\n");
		break;
		case SAX_TWINVIEW_CLONE:
			printf ("Nvidia Twinview clone mode\n");
		break;
		default:
			printf ("unknown mode\n");
		break;
	}
	return (0);
}
