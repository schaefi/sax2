RPM_OPT_FLAGS ?= -O2

CC = g++
OPT=-O2 -fomit-frame-pointer -fpic
CFLAGS=$(RPM_OPT_FLAGS) $(OPT) -Wall -W -Wno-parentheses

VERSION=2.1.99-test9
DATE=2005-08-23

PREFIX=/usr
SBINDIR=$(PREFIX)/sbin
SHAREDIR=$(PREFIX)/share
IDSDIR=$(SHAREDIR)
MANDIR:=$(shell if [ -d $(PREFIX)/share/man ] ; then echo $(PREFIX)/share/man ; else echo $(PREFIX)/man ; fi)
INSTALL=install
DIRINSTALL=install -d
PCILIB=lib/libpci.a
PCIINC=lib/config.h lib/header.h lib/pci.h lib/types.h lib/sysdep.h

ifeq ($(shell uname),NetBSD)
PCILIB=libpciutils.a
LDFLAGS+=-lpci
else
ifeq ($(shell uname),AIX)
CFLAGS=-g
INSTALL=installbsd
DIRINSTALL=mkdir -p
endif
endif

HOST=
RELEASE=

export

all: lib lspci.o common.o

lib: lib/config.h
	$(MAKE) -C lib all

lib/config.h:
	cd lib && ./configure $(IDSDIR) $(VERSION) $(HOST) $(RELEASE)

lspci.o: lspci.c pciutils.h lib/libpci.a
common.o: common.c pciutils.h lib/libpci.a

clean:
	rm -f *.o
	rm -f lib/config.* lib/*.o

