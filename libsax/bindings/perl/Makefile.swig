all:
	swig -Wall -c++ -perl5 SaX.i
	perl Makefile.PL

clean:
	rm -rf blib
	rm -f  Makefile
	rm -f  SaX_wrap.cxx
	rm -f  Makefile.old pm_to_blib
	rm -f  SaX.bs SaX.pm
	rm -f  SaX_wrap.o	
