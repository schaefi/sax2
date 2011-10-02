all:
	swig -Wall -c++ -java SaX.i

clean:
	rm -f SaX[^_]*.java SaX.so *.class SaX.java
	rm -f SWIGTYPE* SaX.jar SaX_wrap*
