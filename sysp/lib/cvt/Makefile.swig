all: swig

swig:
	swig -Wall -perl5 CVT.i
	perl Makefile.PL

clean:
	rm -f *.o
	rm -rf blib
	rm -f  Makefile
	rm -f  Makefile.old pm_to_blib
	rm -f CVT.pm CVT.bs
	rm -f CVT_*
