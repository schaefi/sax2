#!/bin/sh
# Copyright (c) 2005 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2005
# libsax script: create VNC password using the vncp program
# --
#
# CVS ID:
# --------
# Status: Up-to-date
#
LC_ALL="POSIX"
VNCDIR="/root/.vnc"

if [ ! -d $VNCDIR ];then
    mkdir -p $VNCDIR && chmod 700 $VNCDIR
fi
echo $1 | /usr/sbin/vncp > $VNCDIR/passwd
