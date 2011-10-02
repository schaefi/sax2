#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <ms@suse.de>, 2000
# Generate plog objects used for libsysp
#
RPM_OPT_FLAGS ?= -O2
CFLAGS = $(RPM_OPT_FLAGS)
CFLAGS += $(shell pkg-config --cflags QtCore QtGui Qt3Support)
CFLAGS += -I/usr/include/Qt
QTLIBS += $(shell pkg-config --libs QtCore QtGui Qt3Support)

all:spp

swig:spp.o spp.tab.o lex.yy.o

spp:spp.tab.o lex.yy.o spp.o example.o
	g++ -Wall -O2 $(CFLAGS) $(CFLAGSS) -o example example.o spp.o spp.tab.o lex.yy.o \
		-lfl -lreadline $(QTLIBS) -lncurses

example.o:example.cpp
	g++ -Wall -O2 -fpic $(CFLAGS) -c example.cpp -I. 

spp.o:spp.cpp spp.h
	g++ -Wall -O2 -fpic $(CFLAGS) -c spp.cpp -I. 

spp.tab.o: spp.tab.cc lex.yy.c
	g++ -Wall -O2 -fpic $(CFLAGS) -c spp.tab.cc -I. 

lex.yy.o: lex.yy.c
	g++ -Wall -O2 -fpic $(CFLAGS) -c lex.yy.c -I. 

spp.tab.cc: spp.yc
	bison -d spp.yc
	grep -qi "goto yyerrlab1;" spp.tab.cc || \
	cat spp.tab.cc | sed -e s@yyerrlab1:@@ > spp.tab.cc.new
	test -f spp.tab.cc.new && mv spp.tab.cc.new spp.tab.cc || true

lex.yy.c: spp.l spp.tab.cc
	#flex --nounput -i spp.l
	flex -i spp.l

clean:
	rm -f lex.yy.c
	rm -f spp.tab.hc
	rm -f spp.tab.cc
	rm -f *.o
	rm -f example

