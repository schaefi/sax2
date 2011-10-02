sbin_PROGRAMS = xlook

WARN_CFLAGS = @WARN_CFLAGS@

xlook_SOURCES = \
	xlook.c xlook.h

xlook_CFLAGS  = -DRENDER `pkg-config --cflags xft`
xlook_CFLAGS += $(WARN_CFLAGS)

xlook_LDFLAGS = \
	-L/usr/X11R6/lib -L/usr/X11R6/lib64 \
	-lX11 -lXxf86misc -lXext -lm -lXft
