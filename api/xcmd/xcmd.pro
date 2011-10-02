TEMPLATE  = app
SOURCES   += xcmd.cpp
SOURCES   += color.cpp
SOURCES   += glinfo.cpp
SOURCES   += ../common/log.cpp

HEADERS   += xcmd.h
HEADERS   += color.h
HEADERS   += glinfo.h
HEADERS   += ../common/log.h

CONFIG    += qt warn_on release
TARGET    = xcmd

RPM_OPT_FLAGS ?= -O2
QT += qt3support
QT += qtcore

QMAKE_CXXFLAGS = $(RPM_OPT_FLAGS) -fno-strict-aliasing
unix:INCLUDEPATH += /usr/X11R6/include
unix:INCLUDEPATH += /usr/include/X11
unix:INCLUDEPATH += /usr/include/xorg
unix:LIBS += -L/usr/X11R6/lib ../../libsax/libsax.so 
