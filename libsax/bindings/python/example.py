#!/usr/bin/python

import SaX

config   = SaX.SaXConfig;
keyboard = SaX.SaXImport ( SaX.SAX_KEYBOARD );

keyboard.doImport();

manip  = SaX.SaXManipulateKeyboard (keyboard);
models = manip.getModels();

for (key, item) in models.items():
	print "Key: [%s] Value: [%s]" % (key, item)

