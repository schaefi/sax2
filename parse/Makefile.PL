use ExtUtils::MakeMaker;

$CFLAGS = $ENV{RPM_OPT_FLAGS};
$CFLAGS =~ s/-Wall//;

$XORGHD = "NO_XORG_SERVER_HEADER";
if (-f "/usr/include/xorg/xorg-server.h") {
	$XORGHD="HAVE_XORG_SERVER_HEADER";
}

$FLAG1="HAVE_EXTENSIONS_SECTION";
$FLAG2=$XORGHD;

WriteMakefile(
  'NAME'     => 'XFree',
  'LIBS'     => ['-L/usr/X11R6/lib -L/usr/X11R6/lib64 -lm -lxf86config'],
  'OBJECT'   => 'parse.o XFree_wrap.o',
  'DEFINE'   => "-DNEW_INPUT -DNDEBUG -D$FLAG1 -D$FLAG2",
  'INC'      => '-iquote /usr/X11R6/include -iquote /usr/include/xorg -I.',
  'OPTIMIZE' => "-O2 $CFLAGS"
); 
