sbin_PROGRAMS = xidle

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS = $(WARN_CFLAGS)

xidle_SOURCES = \
	xidle.c

xidle_LDFLAGS = \
	-L/usr/X11R6/lib -L/usr/X11R6/lib64 -lX11 -lXxf86misc -lXext -lm

xidle_LDADD = \
	-lXss
