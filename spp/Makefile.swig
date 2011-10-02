all:
	${MAKE} -f Makefile.lex swig
	swig -Wall -c++ -perl5 SPP.i
	perl Makefile.PL

clean:
	${MAKE} -f Makefile.lex clean
	rm -rf blib
	rm -f  Makefile
	rm -f  SPP_wrap.cxx
	rm -f  Makefile.old pm_to_blib
	rm -f  SPP.bs SPP.pm
	rm -f  SPP_wrap.o	
