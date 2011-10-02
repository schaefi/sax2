DESTDIR = /
BINDLIB = lib
PLUGSDIR=${DESTDIR}/usr/${BINDLIB}
MONODLLD=${DESTDIR}/usr/${BINDLIB}/mono/gac/SaX

RPM_OPT_FLAGS ?= -O2
CFLAGS = $(RPM_OPT_FLAGS)
CFLAGS += $(shell pkg-config --cflags QtCore QtGui Qt3Support)
CFLAGS += -I/usr/include/Qt
QTLIBS += $(shell pkg-config --libs QtCore QtGui Qt3Support)

all:/usr/bin/mcs
	g++ -c $(CFLAGS) -I/usr/X11R6/include \
		-I/usr/include/xorg \
		-fno-strict-aliasing -pipe -fPIC SaX_wrap.cxx
	g++ -shared SaX_wrap.o -o libSaX.so \
		-L/usr/X11R6/${BINDLIB} ${QTLIBS} \
		-L../../ -lm -lxf86config -lsax -lxkbfile
	mcs -nologo -debug -out:SaX.dll -t:library SaX*.cs SWIGTYPE* && \
		rm -f SaX*.cs SWIGTYPE*

install_vendor:
	install -d -m 755 ${PLUGSDIR}
	install -d -m 755 ${MONODLLD}
	install -m 755 ./libSaX.so   ${PLUGSDIR}
	install -m 755 ./SaX.dll     ${MONODLLD}
	install -m 644 ./SaX.dll.mdb ${MONODLLD}

clean:
	rm -f SaX*.cs SaX_* libSaX.so SaX.dll.mdb
	rm -f SWIGTYPE* *.exe SaX.dll*
