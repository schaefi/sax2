all:
	swig -Wall -c++ -python SaX.i

clean:
	rm -f  SaX_wrap.cxx
	rm -f  Makefile.old
	rm -f  SaX.py
	rm -f  SaX_wrap.o
	rm -f  SaX.pyc SaX.pyo
	rm -f  _SaX.so
