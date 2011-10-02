CC  = g++ -Wall -L/usr/X11R6/lib -L/usr/X11R6/lib64 -DXORG_LOADER
LIB = ./lib/libsysp.a -lhd -lfl -lstdc++ -lX11 -lXrandr -lhal -lXxf86misc

OBJ = sysp.o mouse.o keyboard.o server.o cfg.o xstuff.o d3.o
SYS = ./lib

RPM_OPT_FLAGS ?= -O2
CFLAGS  = $(RPM_OPT_FLAGS)

export CFLAGS

all: sysp

sysp: ${OBJ}
	${MAKE} -C ${SYS} all
	${CC} -o sysp \
		${OBJ} \
		${LIB}

d3.o: d3.c d3.h
	${CC} -c $(CFLAGS) d3.c

xstuff.o: xstuff.c xstuff.h sysp.h
	${CC} -c $(CFLAGS) xstuff.c

cfg.o: cfg.c cfg.h sysp.h
	${CC} -c $(CFLAGS) cfg.c

server.o: server.c server.h sysp.h
	${CC} -c $(CFLAGS) server.c

keyboard.o: keyboard.c keyboard.h sysp.h
	${CC} -c $(CFLAGS) keyboard.c

sysp.o: sysp.c sysp.h
	${CC} -c $(CFLAGS) sysp.c

mouse.o: mouse.c mouse.h sysp.h
	${CC} -c $(CFLAGS) mouse.c

clean:
	rm -f *.o
	${MAKE} -C ${SYS} clean

