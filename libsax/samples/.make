#!/bin/sh
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
# SaX (sax) configuration level [xapi]
#
# CVS ID:
# --------
# Status: Up-to-date
#
if [ -d /usr/share/qt/mkspecs/linux-g++ ];then
	export QMAKESPEC=/usr/share/qt/mkspecs/linux-g++/
	$QTDIR/bin/qmake -makefile -unix -o Makefile testlib.pro
else
	export QMAKESPEC=/usr/share/qt4/mkspecs/linux-g++/
	$QTDIR/bin/qmake -makefile -unix -o Makefile testlib.pro
fi
