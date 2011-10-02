#!/bin/sh
# Copyright (c) 2000 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 1999
# SaX (sax) configuration level 3
#
# CVS ID:
# --------
# Status: Up-to-date
#
#==================================
# Variables...
#----------------------------------
API="/usr/sbin/xapi"
DOTS="/usr/sbin/dots"
INIT="/usr/share/sax/init.pl"
XC="/usr/share/sax/xc.pl"
PCI="/usr/share/sax/pci.pl"
XFT="/var/cache/xfine/*"
ERR="/var/log/SaX.log"
REF="/var/cache/sax/files/xorg.conf.first"
SVNB="/usr/share/sax/svnbuild"

#==================================
# Init option variables
#----------------------------------
CMDLINE=$*
FASTPATH=""
QUICK_START=""
GPMSTATUS=3
### no longer write BusID by default if there is only one possible
### primary device
if [ $(/usr/sbin/sysp -c | wc -l) -lt 2 ]; then 
	BATCH_MODE="-b nobus"
else
	BATCH_MODE=""
fi
CMD_LINE=""
AUTO_CONF=""
LOW_RES=""
XMODE=""
MODLIST=""
VESA=""
AUTOMODE=""
### single card setup by default now; use primary VGA card
CHIP="-c $(/usr/sbin/getPrimary)"
GPM=""
NODE=""
TYPE=""
NOSCAN=""
SYS_CONFIG=""
DBMNEW=""
IGNORE_PROFILE=""
DIALOG=""
REINIT=0
FULLSCREEN=""
### obsoleted by single card setup, which we use as default now
NOINTELMAGIC="--nointelmagic"

#==================================
# Functions...
#----------------------------------
function killProc() {
	kill `pidof $1` 2>/dev/null
}
function quit() {
	killProc $DOTS
	exit 1
}
function privilege() {
	if [ ! $UID = "0" ];then
		echo "SaX: only root can do this"
		exit 0
	fi
}
function StartGPM() {
	if [ "$GPMSTATUS" = 0 ];then 
	if [ -f "/etc/init.d/gpm" ];then
		/etc/init.d/gpm start 2>/dev/null >/dev/null
	fi
	fi
}
function StopGPM() {
	if [ ! -f "/etc/init.d/gpm" ];then
		return
	fi
	pidof gpm 2>/dev/null >/dev/null
	GPMSTATUS=$?
	if [ "$GPMSTATUS" = 0 ];then
		/etc/init.d/gpm stop 2>/dev/null >/dev/null
	fi
}
function version() {
	ID='$Id: sax.sh,v 1.49 2003/03/17 13:39:51 ms Exp $'
	ID=`echo $ID | cut -f3-4 -d" "`
	echo "SaX2 version 8.1 - SVN Release: $ID" 
}
function needHardwareUpdate() {
	if [ ! -f "/var/cache/sax/files/config" ];then
		return 1
	fi
	LA="lib"
	AC=`uname -i`
	if [ $AC = "x86_64" ] || [ $AC = "ia64" ];then
		LA="lib64";
	fi
	CF="/etc/X11/xorg.conf"
	MD="/usr/X11R6/$LA/modules/drivers";
	if [ ! -d $MD ];then
		MD="/usr/$LA/xorg/modules/drivers";
	fi
	for i in `/usr/sbin/isax -l Card | grep Driver | cut -f3 -d:`;do
		echo "SaX: Checking update status for $i driver"
		case $i in
			nvidia)
			if [ "$MD/${i}_drv.so" -nt $CF ];then
				echo "SaX: NVidia driver is newer than config -> calling reinit"
				return 1
			fi
			;;
			radeon)
			if [ "$MD/fglrx_drv.so" -nt $CF ];then
				echo "SaX: FireGL driver is newer than config -> calling reinit"
				return 1
			fi
			;;
		esac
	done
	return 0
}
function usage() {
	killProc $DOTS
	# ...
	echo "Linux SaX Version 8.1 (2005-03-22)"
	echo "(C) Copyright 2002 - SuSE GmbH <Marcus Schaefer sax@suse.de>"
	echo 
	echo "usage: SaX [ options ]"
	echo "options:"
	echo "[ -h | --help ]"
	echo "  show this message and exit"
	echo
	echo "[ -b | --batchmode [ filename,filename,... ]]"
	echo "  Activate the SaX2 batch modus. If no filename is given an"
	echo "  interactive shell is started to set/overwrite configuration"
	echo "  parameters. If the filename doesn't contain any / characters"
	echo "  it is assumed to be found in /usr/share/sax/profile"
	echo
	echo "[ -a | --auto ]"
	echo "  enable automatic configuration." 
	echo
	echo "[ -l | --lowres ]"
	echo "  use only 800x600@60 Hz standard mode."
	echo "  DDC detection is switched off in this case"
	echo
	echo "[ -V | --vesa ]"
	echo "  This option will set a given resolution and vertical sync"
	echo "  value (in Hz) as VESA standard resolution for a specific"
	echo "  card. Format:  Card:XxY@VSync"
	echo "  Example: 0:1024x768@85";
	echo
	echo "[ -m | --modules ]"
	echo "  comma seperated list of X-Server modules"
	echo "  for example: -m 0=mga,1=nv"
	echo
	echo "[ -c | --chip ]"
	echo "  chip number(s) to scan"
	echo
	echo "[ -x | --xmode ]"
	echo "  use server build in Modelines"
	echo
	echo "[ -u | --automode ]"
	echo "  use server mode suggestion"
	echo 
	echo "[ -p | --pci ]"
	echo "  show PCI/AGP information"
	echo
	echo "[ -n | --node ]"
	echo "  set device node to use for the core pointer"
	echo
	echo "[ -t | --type ]"
	echo "  set protocol type to use for the core pointer"
	echo
	echo "[ -g | --gpm ]"
	echo "  use gpm as repeater of mouse events"
	echo
	echo "[ -s | --sysconfig ]"
	echo "  this option tell SaX2 to import the system wide"
	echo "  config file even if SaX2 was started from a textconsole"
	echo "  which normaly will import the SaX2 HW detection"
	echo "  data"
	echo
	echo "[ -i | --ignoreprofile ]"
	echo "  do not include profiles which are normally applied"
	echo "  automatically"
	echo 
	echo "[ -r | --reinit ]"
	echo "  remove detection database and re-init the"
	echo "  hardware database"
	echo
	echo "[ -f | --fullscreen ]"
	echo "  start in fullscreen mode"
	echo
	echo "[ -v | --version ]"
	echo "  print version information and exit"
	echo
	echo "[ -C | --cmdline=\"commandline-options\"]"
	echo "  activate the commandline mode. The given value to this"
	echo "  option is provided as input for the commandline."
	echo "  For help about the current commandline options type"
	echo "  sax2 --cmdhelp"
}

#==================================
# get options
#----------------------------------
TEMP=`getopt -o gb::alhxm:uc:pn:t:vsrV:dO:ifC:HI --long gpm,batchmode::,auto,lowres,help,xmode,modules:,automode,chip:,pci,node:,type:,version,sysconfig,reinit,vesa:,dbmnew,ignoreprofile,dialog:,fullscreen,cmdline:,cmdhelp,nointelmagic \
-n 'SaX' -- "$@"`

if [ $? != 0 ] ; then usage ; exit 1 ; fi
eval set -- "$TEMP"

while true ; do
	case "$1" in
	-C|--cmdline)               # activate commandline mode
		CMD_LINE=$2             # use value as options for xcmd call
	shift 2 ;;                  

	-b|--batchmode)             # set batch mode and apply file or give
		BATCH_MODE="-b"         # us an interactive shell
	shift 2 ;;

	-a|--auto)                  # create automatic configuration and exit
		AUTO_CONF="-a"          # registry is used if -q is used
	shift ;;

	-r|--reinit)                # remove /var/cache/sax/files to reinit
		REINIT=1                # hardware database
	shift ;;

	-I|--nointelmagic)          # don't check for Intel card
		NOINTELMAGIC="--nointelmagic"
	shift ;;

	-f|--fullscreen)            # start in fullscreen mode
		FULLSCREEN="-f"
	shift ;;

	-d|--dbmnew)                # use the config.new file instead of the
		DBMNEW="--dbmnew"       # normal config storable file
	shift;;

	-i|--ignoreprofile)         # ignore profiles automatically set
		IGNORE_PROFILE="-i"     # via CDB information
	shift;;

	-O|--dialog)                # start with custom dialog
		DIALOG="-O $2"
	shift 2 ;;

	-l|--lowres)                # enable 800x600 virtual screen to make
		LOW_RES="-v"            # sure this mode is used for config
	shift ;;

	-s|--sysconfig)             # read the system installed config file
		SYS_CONFIG="-s"         # instead of the HW detection data
	shift ;;                    # only take effect if SaX2 start its own server

	-u|--automode)              # do not set my mode suggestion, let X11
		AUTOMODE="-u"           # select the resolution
	shift ;;

	-H|--cmdhelp)
		$API -- --help          # get help about commandline mode...
		exit 0 ;;

	-h|--help)                  # get help message...
		usage ;  exit 0 ;;

	-v|--version)               # print version information...
		version; exit 0 ;;

	-x|--xmode)                 # do not use my modelines use the X11
		XMODE="-x"; shift ;;    # mode pool instead

	-m|--modules)               # set module(x) to use for card X...
		MODLIST="-m $2"  
	shift 2 ;;

	-V|--vesa)                  # set resolution and vsync value to
		VESA="--vesa $2"        # build a special profile for activating
	shift 2 ;;                  # this resolution @ vsync

	-c|--chip)                  # set chip(s) to configure...
		CHIP="-c $2"   
		### we still need BusID for Multicard setups
		if echo "$2" | grep -q ","; then
			if [ "$BATCH_MODE" == "-b nobus" ]; then
				BATCH_MODE=""
			fi 
		fi
	shift 2 ;;

	-n|--node)                  # set device node for primary mouse
		NODE="-n $2"            # no multiple mouse device support
	shift 2 ;;

	-t|--type)                  # set primary mouse protocol type
		TYPE="-t $2"            # no multiple mouse device support
	shift 2 ;;

	-p|--pci)                   # show the detected cards and exit  
		privilege; $PCI 
	exit 0 ;;

	-g|--gpm)                   # use gpm as repeater for the mouse
		GPM="-g"                # GPM use /dev/gpmdata and MouseSystems 
	shift ;;

	--)
	if [ "$BATCH_MODE" = "-b" ];then
		BATCH_FILE=$2
		if [ -z $BATCH_FILE ];then
			BATCH_MODE="-b"	
		else
			BATCH_MODE="-b $BATCH_FILE"
		fi
	fi
	shift;
	break;;

	*) 
	echo "SaX: Internal error!" ; exit 1 ;;
	esac
done

#==================================
# prove root privilege
#----------------------------------
privilege

#==================================
# setup QT home dir...
#----------------------------------
temp_QT_HOME_DIR="`eval echo ~$USER/.qt/`"
if test -r "$temp_QT_HOME_DIR/qtrc"; then
	export QT_HOME_DIR="$temp_QT_HOME_DIR"
fi

#==================================
# run the dots...
#----------------------------------
nice -n 19 $DOTS &
sleep 1

#==================================
# clean sweep...
#----------------------------------
rm -f  $REF
rm -rf $XFT $ERR
if [ "$REINIT" = 1 ];then
	rm -f /var/cache/sax/files/*
fi

#==================================
# add ver. and commandline to log
#----------------------------------
BNR="unknown"
if [ -f "$SVNB" ];then
	BNR=`cat $SVNB`
fi
SVN=`version`
DATE=`LANG=POSIX date`
echo "/*************"                                                   >> $ERR
echo "SaX2 log     : $SVN"                                              >> $ERR
echo "**************"                                                   >> $ERR
echo "SVN RELEASE  : $BNR"                                              >> $ERR
echo "             :"                                                   >> $ERR
echo "DESCRIPTION  : X11 configuration log file to collect information" >> $ERR
echo "             : about detection, startup and configuration."       >> $ERR
echo "             : There are three parts of logging:"                 >> $ERR
echo "             : ---"                                               >> $ERR
echo "             : 1) INIT     ( detection, 3D )"                     >> $ERR
echo "             : 2) STARTUP  ( xorg.conf, X11 log, glxinfo )"       >> $ERR
echo "             : 3) CONFIG   ( config actions )"                    >> $ERR
echo "             : ---"                                               >> $ERR
echo "             :"                                                   >> $ERR
echo "VERSION      : SaX2 compiled for: [SuSE-Linux]"                   >> $ERR
if [ -z "$CMDLINE" ];then
	echo "PARAMETER    : no parameter(s) given"                         >> $ERR
else
	echo "PARAMETER    : $CMDLINE"                                      >> $ERR
fi
echo "             :"                                                   >> $ERR
echo "LOG DATE     : $DATE"                                             >> $ERR
echo "*************/"                                                   >> $ERR
echo "============================"                                     >> $ERR
echo "Framebuffer Info:"                                                >> $ERR
echo "----------------------------"                                     >> $ERR
head -n 0 /dev/fb0 >/dev/null 2>&1
if [ "$?" -ne 0 ];then
	echo "Framebuffer not active"                                       >> $ERR
else
	echo "Framebuffer is active"                                        >> $ERR
fi
echo                                                                    >> $ERR

#==================================
# remove hardware.chg if exist...
#----------------------------------
rm -f "/var/cache/sax/files/hardware.chg"

#==================================
# set option strings...
#----------------------------------
XC_OPT="$XMODE $AUTO_CONF $SYS_CONFIG $DIALOG $FULLSCREEN"
IN_OPT="$LOW_RES $BATCH_MODE $MODLIST $AUTOMODE $CHIP $NOINTELMAGIC"
IN_OPT="$IN_OPT $GPM $NODE $TYPE $VESA $DBMNEW $IGNORE_PROFILE"

#==================================
# setup init call trigger
#----------------------------------
IN_TRIGGER=0
IN_OPT=`echo $IN_OPT`
if [ ! -f $API ];then
	SYS_CONFIG=""
	IN_TRIGGER=1
fi
if [ $IN_TRIGGER = 0 ];then
	needHardwareUpdate
	if [ ! $? = 0 ];then
		IN_TRIGGER=1
	fi
fi
export HW_UPDATE=$IN_TRIGGER

#==================================
# check for auto config
#----------------------------------
if [ "$AUTO_CONF" = "-a" ];then
	export IGNORE_GUI_CHECK=1
fi

#==================================
# call init now...
#----------------------------------
StopGPM
killProc $DOTS
echo -ne "\r"

if [ $IN_TRIGGER = 1 ];then
	echo "SaX: initializing please wait..."
	if [ -f /etc/X11/xorg.conf ];then
		if [ "$SYS_CONFIG" = "-s" ];then
			echo "SaX: will use current configuration"
		else
			echo "SaX: your current configuration will not be read in"
		fi
		echo
	fi
	$INIT $IN_OPT --quiet no
	if [ ! $? = 0 ];then
		quit
	fi
else
	echo "SaX: initialization already done"
	echo "SaX: call [ sax2 -r ] if your system has changed !"
fi

#==================================
# check for commandline call
#----------------------------------
echo
echo "SaX: startup"
if [ ! -z "$CMD_LINE" ];then
	$API -- $CMD_LINE 2>>$ERR
	exit $?
fi
 
#==================================
# start X11 manager... 
#----------------------------------
if [ -z "$AUTO_CONF" ];then
	$XC $XC_OPT 2>>$ERR
else
	echo; echo "SaX: creating config file please wait..."
	echo
	$XC $XC_OPT 2>>$ERR
fi
exitCode=$?

#==================================
# restart GPM and exit
#----------------------------------
StartGPM; echo
exit $exitCode

