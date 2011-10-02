#!/usr/bin/perl

use lib '/usr/share/sax/modules';

use PLogData;

$logfile = $ARGV[0];
if (! -f $logfile) {
	die "PlogData: $!";
}

$ptr = ParseLog ($logfile);
$mem = GetVideoRam($ptr);
print "$mem\n";

@res = GetResolution($ptr);
print "@res\n";

$ddc = GetMonitorManufacturer($ptr);
print "$ddc\n";

$type = CheckDisplayType($ptr);
print "$type\n";

$vmd  = GetVMwareColorDepth($ptr);
print "$vmd\n";

$siz  = GetDisplaySize($ptr);
print "$siz\n";
