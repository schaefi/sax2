use ExtUtils::MakeMaker;

$CFLAGS = $ENV{RPM_OPT_FLAGS};
$CFLAGS =~ s/-Wall//;

WriteMakefile(
  'NAME'     => 'CVT',
  'LIBS'     => ['-lm'],
  'OBJECT'   => 'CVT_wrap.o cvt.o',
  'OPTIMIZE' => "-O2 $CFLAGS",
  'CC'       => 'gcc',
  'LD'       => 'gcc'
);
