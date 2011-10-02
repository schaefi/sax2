use ExtUtils::MakeMaker;

$CFLAGS = $ENV{RPM_OPT_FLAGS};
$CFLAGS =~ s/-Wall//;

WriteMakefile(
  'NAME'     => 'FBSet',
  'LIBS'     => ['-lm'],
  'OBJECT'   => 'FBSet_wrap.o fbset.o',
  'OPTIMIZE' => "-O2 $CFLAGS",
  'CC'       => 'g++',
  'LD'       => 'g++'
);
