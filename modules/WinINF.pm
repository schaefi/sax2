#/.../
# Copyright (c) 2000 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2003
# WinINF.pm  read windows inf file ( especially monitor inf files )
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#---[ mountMedia ]----#
sub mountMedia {
#------------------------------------------------------
# mount a disk or CD and return the .inf file location
# on the medium. If no .inf file is found undef is returned
#
	my $media  = $_[0];

	my @result  = ();
	my @devlist = "/dev/fd0";
	my $mountpoint = "/tmp/mymedia.$$";
	if (! -d $mountpoint) {
		mkdir ($mountpoint,0500);
	}
	if ($media eq "CD") {
		my $devs = qx (/usr/sbin/sysp -D); chomp $devs;
		@devlist = split (/,/,$devs);
	}
	foreach my $device (@devlist) {
		my $data  = qx (mount $device $mountpoint 2>&1);
		my $error = $? >> 8;
		if ($error) {
			next;
		}
		open (FD,"find $mountpoint -iname '*.inf'|");
		while (my $file = <FD>) {
			chomp $file;
			if ($file !~ /AUTORUN/i) {
				push (@result,$file);
			}
		}
		close FD;
		if (@result > 0) {
			return (@result);
		}
	}
	return;
}

#---[ umountMedia ]---#
sub umountMedia {
#----------------------------------------------
# unmount disk and remove the mountpoint
# directory
#
	my $mountpoint = "/tmp/mymedia.$$";
	my $data = qx (umount -l $mountpoint 2>&1);
	rmdir ($mountpoint);
}

#---[ readDisk ]---#
sub readDisk {
#----------------------------------------------
# read first .inf file from the CD or Disk and
# return a list of data lines
#
	my @data = ();
	my @files = mountMedia("CD");
	if (! @files) {
		umountMedia();
		@files = mountMedia();
	}
	if (! @files) {
		umountMedia();
		return (@data);
	}
	foreach my $file (@files) {
		if ( ! open(FD,"$file") ) {
			next;
		}
		while (<FD>) {
		if (($_ !~ /^;/) && ($_ !~ /^.$/)) {
			chomp ($_); push (@data,$_);
		}
		}
		push (@data,"[]");
		close (FD);
	}
	umountMedia();
	return (@data);
}

#---[ readData ]-----#
sub readData {
#--------------------------------------------------
# read lines from .inf file. Remove empty lines
# and comments
#
	my @data = ();
	while (<>) {
	if (($_ !~ /^;/) && ($_ !~ /^.$/)) {
		chomp ($_);
		push (@data,$_);
	}
	}
	push (@data,"[]");
	return (@data);
}

#---[ getSections ]-----#
sub getSections {
#--------------------------------------------------
# create hash containing list references to all
# [ hashkey ] sections in the .inf file
#
	my $sname;
	my %sections;
	my @data;
	my $count = 0; 
	foreach (@_) {
	if ($_ =~ /^ *\[(.*)\]/) {
		if ( $count == 1 ) {
			my @save = @data;
			$sections{$sname} = \@save;
			$sname = $1;
			$count = 0;
			@data  = ();
		}
		if ( $count == 0 ) {
			$sname = $1;
			$count++;
		}
	} else {
		push (@data,$_);
	}
	}
	return (%sections);
}

#---[ getManufacturerList ]----#
sub getManufacturerList {
#--------------------------------------------------
# get names of Manufactures in the Manufacturer
# section. Return a list with names
#
	my %sections = %{$_[0]};
	my @manulist = @{$sections{Manufacturer}};
	my @result   = ();
	foreach my $item (@manulist) {
	if ($item =~ /\%(.*)\%.*/) {
		push (@result,$1);
	}
	}
	return (@result);
}

#---[ getStringSection ]----#
sub getStringSection {
#--------------------------------------------------
# resolve string sections contents. Return a hash
# with key/value according to key=value in the Strings
# section
#
	my %sections = %{$_[0]};
	my @stringlist = @{$sections{Strings}};
	my %result = {};
	foreach my $item (@stringlist) {
	if ($item =~ /(.*)=\"(.*)\".*/) {
		$result{uc $1} = $2;
	}
	}
	return (%result);
}

#---[ getRegister ]----#
sub getRegister {
#--------------------------------------------------
# resolve AddReg section and jump to the key in
# this section, returning the data section for 
# AddReg
#
	my %sections = %{$_[0]};
	my $instkey  = $_[1];
	my @result = ();
	foreach my $reg (@{$sections{$instkey}}) {
	if ($reg =~ /^AddReg=(.*)/) {
		my @reglist = split(/,/,$1);
		my $regkey = $reglist[0];
		if (! defined $sections{$regkey}) {
			$regkey =~ s/AddReg/Addreg/;
		}
		@result = @{$sections{$regkey}};
	}
	}
	return (@result);
}

#---[ getMonitorINFData ]----#
sub getMonitorINFData {
#--------------------------------------------------
# walk through the single sections needed to provide
# monitor data within a perl hash for CDB usage
#
	my %sections = %{$_[0]};
	my @manulist = getManufacturerList (\%sections);
	my %strings  = getStringSection (\%sections);
	my %result;
	#==============================================
	# walk through all the monitor vendors
	#----------------------------------------------
	foreach my $item (@manulist) {
		my $vendor = uc ($item);
		# /.../
		# got the vendor name here...
		# print "+++++++++ $item -> $strings{$vendor}\n";
		# ---
		#my $vendorName = $strings{$vendor};
		my $vendorName = $vendor;
		#==============================================
		# walk through all the entries of a vendor
		#----------------------------------------------
		foreach my $manu (@{$sections{$item}}) {
		if ($manu =~ /^\%(.*)\%=(.*),.*Monitor\\(.......)/) {
			my $namekey = $1;
			my $instkey = $2;
			my $monikey = $3;
			if (defined ($monikey)) {
			if (defined ($strings{uc $namekey})) {
				my $bigname = uc ($namekey);
				# /.../
				# got the name and the DDC-ID here...
				# print "+++++++ $namekey -> $strings{$bigname} : $monikey\n";
				# ---
				my $productName = $strings{$bigname};
				$productName =~ s/\t//g;
				$productName =~ s/,/ /g;
				$productName =~ s/  +/ /g;
				my @data = getRegister (\%sections,$instkey);
				foreach my $sync (@data) {
				if ($sync =~ /.*Mode1,,\"(.*)\"/) {
					my $syncstr = $1;
					$syncstr =~ s/\.[0-9]+//g;
					my @synclist = split(/,/,$syncstr);
					for(my $i=0;$i<2;$i++) {
					if ($synclist[$i] !~ /.*\-.*/) {
						my $value = $synclist[$i];
						my $sendv = $value;
						if ($i == 0) {
							$sendv = $value - 5;
						}
						if ($i == 1) {
							$sendv = $value - 10;
						}
						$synclist[$i] = "$sendv-$value";
					}
					}
					# /.../
					# got hsync/vsync here...
					# print "$synclist[0] : $synclist[1]\n";
					# ---
					$result{$vendorName}{$productName}{Option} = "DPMS";
					$result{$vendorName}{$productName}{Hsync}  = $synclist[0];
					$result{$vendorName}{$productName}{Vsync}  = $synclist[1];
					$result{$vendorName}{$productName}{DDC}    = $monikey;
				}
				}
			}
			}
		}
		}
	}
	return (%result);
}

1;
