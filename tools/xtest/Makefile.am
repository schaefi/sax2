sbin_PROGRAMS = testX

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS = $(WARN_CFLAGS)

testX_SOURCES = \
	../xwrapper/xaccess.c \
	xtest.c

testX_LDFLAGS = \
	-L/usr/X11R6/lib -L/usr/X11R6/lib64 -lX11 -lXmu

INCLUDES = -I../xwrapper
