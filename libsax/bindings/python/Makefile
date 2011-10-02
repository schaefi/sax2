DESTDIR = /
BINDLIB = lib
PYVER   = `python -c "import sys; v=sys.version_info[:2]; print '%d.%d'%v"`
PACKDIR = ${DESTDIR}/usr/${BINDLIB}/python${PYVER}/site-packages
MODDIR  = ${PACKDIR}/SaX

RPM_OPT_FLAGS ?= -O2
CFLAGS = $(RPM_OPT_FLAGS)
CFLAGS += $(shell pkg-config --cflags QtCore QtGui Qt3Support)
CFLAGS += -I/usr/include/Qt
QTLIBS += $(shell pkg-config --libs QtCore QtGui Qt3Support)

export LD_LIBRARY_PATH=../../

all:
	g++ -c $(CFLAGS) -I/usr/X11R6/include -I/usr/include/xorg \
		-I/usr/include/python${PYVER} \
		-O2 -fPIC -fno-strict-aliasing -pipe SaX_wrap.cxx
	g++ -shared SaX_wrap.o -o _SaX.so \
		-L/usr/X11R6/${BINDLIB} ${QTLIBS} \
		-L../../ -lm -lxf86config -lsax -lxkbfile
	python -c "import SaX"
	python -O -c "import SaX"

install_vendor:
	install -d -m 755      ${MODDIR}
	install -m 755 _SaX.so ${MODDIR}
	install -m 644 SaX.py  ${MODDIR}
	install -m 644 SaX.pyc ${MODDIR}
	install -m 644 SaX.pth ${PACKDIR}

clean:
	rm -f  SaX_wrap.cxx
	rm -f  Makefile.old
	rm -f  SaX.py
	rm -f  SaX_wrap.o
	rm -f  SaX.pyc SaX.pyo
	rm -f  _SaX.so
