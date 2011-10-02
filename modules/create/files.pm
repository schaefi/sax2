# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Files section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#
use FileHandle;

#---[ CreateFilesSection ]----#
sub CreateFilesSection {
#------------------------------------------------
# create the files section...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)  
	my $speachtype;              # font according to language
	my $lang;                    # YaST used language codes
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $firstpath;               # first path according to language
	my $language;                # formated LANG string
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter

	# create header line...
	@result = ();
	push(@result,"Section \"Files\"\n");

	# create body...
	foreach $i (sort keys %{$var{Files}{0}}) {
		$setting = $var{Files}{0}{$i};
		if ($setting ne "") {
			$setting =~ s/ +//g;  # remove blanks
			$setting =~ s/\t+//g; # remove tabs
			$setting =~ s/\"//g;  # remove " signs
			$setting =~ s/ +//g;  # remove rest blanks
			$setting =~ s/\n//g;  # remove returns

			@list = split(/,/,$setting);
			$size = @list;

			SWITCH: for ($i) {

			# ModulePath setting...
			# --------------------
			/^ModulePath/ && do {
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			last SWITCH;
			};

			# FontPath setting...
			# -------------------
			/^FontPath/   && do {
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			last SWITCH;
			};

			# InputDevices setting...
			# -----------------------
			/^InputDevices/   && do {
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			last SWITCH;
			};

			# RgbPath setting...
			# -------------------
			/^RgbPath/    && do { 
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			last SWITCH;
			};

			# LogFile setting...
			# ------------------ 
			/^LogFile/    && do {
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			last SWITCH;
			};
			}
		}
	}
	# ...
	# add InputDevice devices which are valid for
	# re-apply (security reasons)
	# ---
	my $io = new FileHandle;
	if ($io->open("/usr/share/sax/api/data/PointerDevice")) {
		while (my $device = <$io>) {
		chomp ($device);
		if ($device =~ /^\//) {
			push (@result,PrintLine("InputDevices","\"$device\""));
		}
		}
	}
	# create footer line...
	push (@result,"EndSection\n");
	return (@result);
}

1;
