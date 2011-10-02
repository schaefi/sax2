use ExtUtils::MakeMaker;

#$QTDIR=$ENV{QTDIR};

$QTLIBS = `pkg-config --libs QtCore QtGui Qt3Support`;
$QTINC = `pkg-config --cflags QtCore QtGui Qt3Support`;
chomp $QTINC;
$QTINC = "$QTINC" ."-I/usr/include/Qt";
$CFLAGS = $ENV{RPM_OPT_FLAGS} ." $QTINC";
$CFLAGS =~ s/-Wall//;

$INCPATH=$QTINC;
if (-f "/usr/X11R6/include/xf86Parser.h") {
	$INCPATH=$INCPATH." -iquote /usr/X11R6/include -I.";
} else {
	$INCPATH=$INCPATH." -iquote /usr/include/xorg -I.";
}

WriteMakefile(
  'NAME'     => 'SaX',
  'LIBS'     => ["-L/usr/X11R6/lib -L/usr/X11R6/lib64 $QTLIBS -lm -lxf86config -lxkbfile"],
  'dynamic_lib' => { 'OTHERLDFLAGS' => '-L../.. -lsax' },
  'OBJECT'   => 'SaX_wrap.o',
  'INC'      => $INCPATH,
  'OPTIMIZE' => "-O2 $CFLAGS"
);

