use ExtUtils::MakeMaker;

#$QTDIR=$ENV{QTDIR};
#$QTDIR='/usr/include/Qt';
$QTLIBS = `pkg-config --libs QtCore QtGui Qt3Support`;
$QTINC = `pkg-config --cflags QtCore QtGui Qt3Support`;
chomp $QTINC;
$QTINC = $QTINC.'-I/usr/include/Qt';
#print $QTINC;
$CFLAGS = $ENV{RPM_OPT_FLAGS} ." $QTINC";
$CFLAGS =~ s/-Wall//;

WriteMakefile(
  'NAME'     => 'SPP',
  'LIBS'     => ["-L/usr/X11R6/lib -L/usr/X11R6/lib64 $QTLIBS -lm -lfl -lreadline -lncurses"],
  'OBJECT'   => 'SPP_wrap.o lex.yy.o spp.tab.o spp.o',
  'INC'      => "-I/usr/include/X11 $QTINC",
  'OPTIMIZE' => "-O2 $CFLAGS",
  'CC'       => 'g++'
);

