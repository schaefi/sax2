#/.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# Makefile to build sysp libhw part
# --
# Status: Up-to-date
#
CC     = g++ -Wall -fpic -O2
OBJ    = mouse.o tv.o keyboard.o monitor.o memory.o vbios.o cdrom.o framebuffer.o

all: ${OBJ}

mouse.o:mouse.c hwdata.h
	${CC} -c ${CFLAGS} mouse.c

keyboard.o:keyboard.c hwdata.h
	${CC} -c ${CFLAGS} keyboard.c

monitor.o:monitor.c hwdata.h
	${CC} -c ${CFLAGS} monitor.c

memory.o:memory.c hwdata.h
	${CC} -c ${CFLAGS} memory.c

tv.o:tv.c hwdata.h
	${CC} -c ${CFLAGS} tv.c

vbios.o:vbios.c hwdata.h
	${CC} -c ${CFLAGS} vbios.c

cdrom.o:cdrom.c hwdata.h
	${CC} -c ${CFLAGS} cdrom.c

framebuffer.o:framebuffer.c hwdata.h
	${CC} -c ${CFLAGS} framebuffer.c

clean:
	rm -f *.o
