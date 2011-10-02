sbin_PROGRAMS = xw
perl_SCRIPTS = ximage wmstart

WARN_CFLAGS = @WARN_CFLAGS@

xw_SOURCES = \
	xw.c \
	xaccess.c \
	fashion.c

xw_LDFLAGS = \
	-L/usr/X11R6/lib -L/usr/X11R6/lib64 \
	-lX11 -lXext -lXmu -lm -lXft

xw_CFLAGS = `pkg-config --cflags xft`
xw_CFLAGS += $(WARN_CFLAGS)
xw_CFLAGS += -DXORG_LOADER


xw_LDADD = -lXinerama
