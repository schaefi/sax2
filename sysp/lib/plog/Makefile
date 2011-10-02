#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <ms@suse.de>, 2000
# Generate plog objects used for libsysp
#

CC       = g++
LIBS     = -lfl
YACC     = bison
FLEX     = flex -i

SRC      = plog.cc parse.cc
OBJ      = parse.tab.o plog.o parse.o

# build...
# ---------
all:${OBJ}

parse.cc:
	@echo "parse.cc is not a target for me... skip"

parse.tab.o: parse.tab.cc lex.yy.c
	${CC} -c ${CFLAGS} parse.tab.cc -I.

parse.tab.cc: parse.yc
	$(YACC) -d parse.yc
	mv parse.tab.hc parse.tab.h

lex.yy.c: parse.l
	$(FLEX) parse.l

plog.o:plog.cc ../syslib.h
	${CC} -c ${CFLAGS} plog.cc

parse.o:parse.cc ../syslib.h
	${CC} -c ${CFLAGS} parse.cc

plog_test:
	${CC} plog_test.cc ${CFLAGS} -o plog_test ${OBJ} -lfl

clean:
	rm -f lex.yy.c
	rm -f parse.tab.h
	rm -f parse.tab.cc
	rm -f plog_test
	rm -f *.o

