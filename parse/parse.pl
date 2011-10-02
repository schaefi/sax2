#!/usr/bin/perl
# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

use lib './blib/arch/auto/XFree';

BEGIN { $| = 1; print "1..1\n"; }
END {print "not ok 1\n" unless $loaded;}
use XFree;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$ptr = XFree::ReadConfigFile("/etc/X11/xorg.conf");
#$ptr = XFree::ReadConfigFile("/suse/ms/xorg.conf");

# print "+++ $ptr\n";

$ver = XFree::GetParserVersion();
print "Parser Version: $ver\n";

#$erg = XFree::GetModulePath($ptr);
#print "$erg\n";

$erg = XFree::GetFontPath($ptr);
print "$erg\n";

#$erg = XFree::GetModuleSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetModuleSubSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetFlags($ptr);
#print "$erg\n";

#$erg = XFree::GetInputSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetMonitorSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetModesSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetDeviceSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetLayoutSpecs($ptr);
#print "$erg\n";

#$erg = XFree::GetDisplaySpecs($ptr);
#print "$erg\n"; 

