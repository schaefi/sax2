TEMPLATE  += app

SOURCES   += frame.cpp
SOURCES   += xfine.cpp
SOURCES   += xquery.cpp
SOURCES   += file.cpp
SOURCES   += common/log.cpp
SOURCES   += common/qx.cpp
SOURCES   += common/xlock.cpp

HEADERS   += frame.h
HEADERS   += xfine.h
HEADERS   += xquery.h
HEADERS   += file.h
HEADERS   += common/log.h
HEADERS   += common/qx.h
HEADERS   += common/xlock.h

RPM_OPT_FLAGS ?= -O2

QT += qtcore
QT += qtgui
QT += qt3support

QMAKE_CXXFLAGS = $(RPM_OPT_FLAGS)
CONFIG    += thread warn_on release
unix:LIBS += -lm -lXss -lXinerama -lGL -lXxf86vm
TARGET    = xfine
unix:LIBS += -L/usr/X11R6/lib
