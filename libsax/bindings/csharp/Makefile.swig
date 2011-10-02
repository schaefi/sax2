all:
	swig -Wall -c++ -csharp SaX.i

clean:
	rm -f SaX*.cs SaX_* libSaX.so SaX.dll.mdb
	rm -f SWIGTYPE* *.exe SaX.dll
