#!/usr/bin/perl

use lib './blib/arch/auto/SaX';

use SaX;

$exc = new SaX::SaXException;
$exc -> setDebug (1);

$init = new SaX::SaXInit;
if ( $init -> needInit() ) {
	print ("initialize cache...\n");
	$init -> doInit();
}
$status = $init -> errorString();
print "++++ $status\n";

$config  = new SaX::SaXConfig;
$desktop = new SaX::SaXImport ( $SaX::SAX_DESKTOP );
$card    = new SaX::SaXImport ( $SaX::SAX_CARD );
$path    = new SaX::SaXImport ( $SaX::SAX_PATH );
$kbd     = new SaX::SaXImport ( $SaX::SAX_KEYBOARD );
$desktop -> doImport();
$card    -> doImport();
$path    -> doImport();
$kbd     -> doImport();

$manip2 = new SaX::SaXManipulateKeyboard ($kbd);
$manip2 -> getXKBVariant ("lala");
#%models = %{$manip2 -> getModels()};
#foreach (keys %models) {
#	print "_________ $_ -> $models{$_}\n";
#}
$status = $manip2 -> errorString();
@bla = @{$manip2 -> getVariants ("cz")};
print "##################### @bla\n";
print "++++ $status\n";


$manip = new SaX::SaXManipulateDesktop ($desktop,$card,$path);
$manip -> setColorDepth (24);

$config -> addImport ($desktop);
$config -> setMode ($SaX::SAX_MERGE);
$config -> createConfiguration();

$code = $config -> errorCode();
$msg  = $config -> errorString ();
print "++++ $code : $msg\n";


$manip = new SaX::SaXManipulateCard ($card);
$listref = $manip -> getCardDrivers();
@list = @{$listref};
print "@list\n";

%name = %{$manip -> getCardOptions("i810")};
foreach my $key (keys %name) {
	print "++++ $key -> $name{$key}\n";
}

$cnam = $manip -> getCardName();
print "_____ $cnam\n";

exit (0);
