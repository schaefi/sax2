#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <ms@suse.de>, 2000
#
# sysp Makefile to generate the SaX2 system profiler
# used for creating the SaX2 hardware registry
#

RPM_OPT_FLAGS ?= -O2

CC       = g++
CFLAGS   = $(RPM_OPT_FLAGS) -O2 -fpic -Wall
LIBS     = -lfl

SRC      = plog/plog.c plog/parse.c
OBJ      = plog.o parse.o y.tab.o
AIM      = libsysp.a

PLOG     = ./plog
PCI      = ./pci
FBSET    = ./fbset
HW       = ./hw
COMMON   = ./common

export CFLAGS

all:${AIM}

libsysp.a:
	${MAKE} -C ${PLOG}   all
	${MAKE} -C ${PCI}    all
	${MAKE} -C ${FBSET}  -f Makefile.swig all
	${MAKE} -C ${HW}     all
	${MAKE} -C ${COMMON} all

	ar rcs $@ \
		${PLOG}/*.o ${PCI}/*.o ${PCI}/lib/*.o ${FBSET}/*.o \
		${HW}/*.o ${COMMON}/*.o
	ranlib $@
	${CC} -shared -o libsysp.so \
		${PLOG}/*.o ${PCI}/*.o ${PCI}/lib/*.o ${FBSET}/*.o \
		${HW}/*.o ${COMMON}/*.o

clean:
	rm -f libsysp*
	rm -f pci/lib/libpci.a
	${MAKE} -C ${PLOG}   clean
	${MAKE} -C ${PCI}    clean
	${MAKE} -C ${FBSET}  -f Makefile.swig clean
	${MAKE} -C ${HW}     clean
	${MAKE} -C ${COMMON} clean

