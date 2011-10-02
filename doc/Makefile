# /.../
# Copyright (c) 2005 SUSE LINUX Products GmbH, Germany. All rights reserved.
#
# Author: Marcus Schäfer <ms@suse.de>, 2005
#
# Makefile for building the system draft for the SaX
# project
#
all:
	touch sax-index/sax.ind
	latex sax.tex
	latex sax.tex
	mv sax.idx ./sax-index
	( cd ./sax-index && makeindex -g -s sax.ist sax.idx )
	latex sax.tex
	latex sax.tex
	dvipdf sax.dvi

dvi:
	xdvi sax.dvi
	
clean:
	rm -f sax-index/sax.ilg
	rm -f sax-index/sax.ind
	rm -f sax-index/sax.idx
	rm -f *.dvi
	rm -f *.idx
	rm -f *.toc
	rm -f *.log
	rm -f *.bmt
	rm -f *.aux
	rm -f *.mtc*
	rm -f *.out
