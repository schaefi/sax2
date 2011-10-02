#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# PLogData.pm saxtools
# provide parse functions for the X11 log 
# file data
#
# CVS ID:
# --------
# Status: Up-to-date
#
use PLog;

#---[ ParseLog ]-----------#
sub ParseLog {
#---------------------------------------------
# parse the log file and return the data
# pointer
#
 my $log = $_[0];
 my $ptr = PLog::PLogParse($log);
 return($ptr);
}

#---[ GetResolution ]-----------#
sub GetResolution {
#--------------------------------------------
# return the resolution list for the first
# card
#
 my $ptr = $_[0];
 my $res = PLog::PLogGetResolution($ptr);
 my @rlist = split(/:/,$res);
 return(@rlist); 
}

#---[ GetVideoRam ]-------------#
sub GetVideoRam {
#--------------------------------------------
# return the video ram in kbyte for the 
# first card
#
 my $ptr = $_[0];
 my $ram = PLog::PLogGetVideoRam($ptr);
 return($ram);
}

#---[ GetMonitorManufacturer ]-------------#
sub GetMonitorManufacturer {
#--------------------------------------------
# return the ddc ID of the primary card 
#
 my $ptr = $_[0];
 my $ddc = PLog::PLogGetMonitorManufacturer($ptr);
 return($ddc); 
}

#---[ CheckDisplayType ]-------------#
sub CheckDisplayType {
#--------------------------------------------
# return the display type of the primary 
# display. Values are: CRT or LCD/TFT 
#
 my $ptr = $_[0];
 my $type = PLog::PLogCheckDisplay($ptr);
 return($type);
}

#---[ GetVMwareColorDepth ]-------------#
sub GetVMwareColorDepth {
#--------------------------------------------
# get the color depth used in the OS which 
# is running the vmware session
#
 my $ptr = $_[0];
 my $vmdepth = PLog::PLogGetVMwareColorDepth($ptr);
 return($vmdepth);
}

#---[ GetDisplaySize ]-------------#
sub GetDisplaySize {
#--------------------------------------------
# get the real size of the display in mm
#
 my $ptr = $_[0];
 my $size = PLog::PLogGetDisplaySize($ptr);
 return($size);
}

1;
