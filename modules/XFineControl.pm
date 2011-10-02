#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# XFineControl.pm configuration level 3
# provide functions to handle the changes result from 
# xfine
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib             '/usr/share/sax/modules';

use ParseConfig;
use CreateSections;


#---[ IncorpModelines ]------#
sub IncorpModelines {
#---------------------------------------------
# this function may be used to create the
# config hash format of the modelines within
# a XFree4.0 formated configuration file
#
	my $file = $_[0];
	my $ptr;       # XF86ConfigPtr
	my %parse;     # parse hash result from ParseModesSection
	my @idlist;    # list of card numbers
	my %var;       # result hash
	my $card;      # card number 
	my $id;        # key string like Modeline,...
	my $modenr;    # number of Modeline
	my $modename;  # name of Modeline
	my @list;      # helper list for timing values
	my $timing;    # timing string
 
	$ptr = ReadConfig($file);
	%parse  = ParseModesSection($ptr);
	if ($parse ne "null") { 
		@idlist = ();
		foreach (keys %{$parse{Modes}}) {
		if ($_ ne "") {
			push(@idlist,$_);
		}
		}
		foreach $card (@idlist) {
		foreach $id (keys %{$parse{Modes}{$card}}) {
			$value = $parse{Modes}{$card}{$id};
			if ( $value ne "" ) {
			SWITCH: for ($id) {

			/^Modeline/i    && do {
			foreach $modenr (keys %{$parse{Modes}{$card}{$id}}) {
				@list = split(/ +/,$parse{Modes}{$card}{$id}{$modenr});
				$modename = shift(@list);
				$timing   = join(" ",@list);
				$var{Monitor}{$card}{Modeline}{$modenr}{$modename} = $timing; 
			}
			last SWITCH;
			};

			}
			}
		}
		}
	}
	return(%var);
}


#---[ ExcorpModelines ]-------#
sub ExcorpModelines {
#-----------------------------------------
# this function is used to exchange the
# all modes sections against the given one
#
	my @section = @_;
	my $file    = pop(@section);
	my @config;
	my $start;
	my $done;

	$start = 0;
	$done  = 0;

	open(FD,"$file") || return(-1);
	while($line=<FD>) {
		$line =~ s/\n+//g;
		if ($line eq "") {
			next;
		}
		# begin section modes...
		# -----------------------
		if ($line =~ /Section.*Modes.*/i) {
		if ($done == 0) {
		foreach (@section) {
			$_ =~ s/\n+//g;
			if ($_ ne "") {
				push(@config,"$_\n");
			}
		}
		$done  = 1;
		}
		$start = 1;
		}
		# /.../
		# if not modes section write it...
		# ---------------------------------
		if ($start == 0) {
			push(@config,"$line\n");
		}
		# end of any section...
		# ----------------------
		if ($line =~ /EndSection/i) {
			$start = 0;
		} 
	}
	close(FD);

	open(FD,">$file") || return(-1);
	foreach (@config) { 
		print FD $_;
		if ($_ =~ /EndSection/i) {
			print FD "\n";
		}
	}
	close(FD);
	return(0);
}

#---[ CreateTuneHash ]------#
sub CreateTuneHash {
#-------------------------------------------
# this function create the tune hash which 
# is the base hash for saving old and new
# timing values
#
# tune{screen}{modenr}{modename}{origtiming} = oldtiming:newtiming
#
	my %var    = %{$_[0]};
	my $screen;     # screen number
	my $modenr;     # mode number
	my $modename;   # mode name
	my %tune;       # tune hash
	my $t;          # modeline timing including dacspeed
	my $timing;     # real timings 8 values 

	@TuneIn = ();
	foreach $screen (keys %{$var{Monitor}}) {
	$tune{$screen}{0}{dummy}{dummy} = "";

	foreach $modenr (keys %{$var{Monitor}{$screen}{Modeline}}) {
	foreach $modename (keys %{$var{Monitor}{$screen}{Modeline}{$modenr}}) {
		$t = $var{Monitor}{$screen}{Modeline}{$modenr}{$modename};
		# /.../
		# create tune hash with origtiming = newtiming...
		# ------------------------------------------------
		@list   = split(/ +/,$t);
		$timing = join(" ",@list);
		$timing =~ s/\"//g;
		$timing =~ s/^ +//g;
		$timing =~ s/ +$//g;
		$tune{$screen}{$modenr}{$modename}{$timing} = $timing;
		$TuneIn[$screen][$modename]++;
	}
	}
	}
	return(%tune);
}

#---[ CheckTuneHash ]------#
sub CheckTuneHashOnScreen {
#---------------------------------------
# this function returns the current
# number of Modelines in the tune hash
# per screen
#
	my $screen = $_[0];
	my $name   = $_[1];
	return ($TuneIn[$screen][$name]);
}


#---[ UpdateModelinesOnScreen ]----#
sub UpdateModelinesOnScreen {
#--------------------------------------------
# this function is called to incorp the 
# eventually made changes to a modeline
#
	my %tune     = %{$_[0]};           # the tune hash
	my $file     = $_[1];              # file name of xfine output file
	my $XfineDir = "/var/cache/xfine"; # xfine directory 

	my $screen;  # the screen number
	my $ot;      # the original timing
	my $nt;      # the new timing
	my $s;
	my $orig;
	my $dac;
	my @pretiming;
	my @cmptiming;
	my $modename;
	my $modenr;
	my $new;
	my $res;
	my $xfine;
	my $t;
	my @list;
	my $count;
	my $x;
	my $y;
	my $maxdac;
	my $nr;
	my $o;
	my %delete;

	# read xfine file
	# ----------------
	$file =~ /$XfineDir\/(.*):(.*)/;
	$screen = $1;
	$res    = $2;      
	open(FD,$file);
	while($line=<FD>) {
		chomp($line);
		$xfine = $line;
	}
	close(FD);
	$xfine =~ /(.*):(.*):(.*):(.*)/;
	$ot     = $2;
	$nt     = $3;
	$maxdac = $4;

	$count = CheckTuneHashOnScreen($screen,$res);
	# print STDERR "$screen: $count\n";

	# find and replace...
	# --------------------
	if ($count ne "") {
		#====================================================
		# ok there are modelines which have to be updated...
		# ---------------------------------------------------
		$haschanged = 0;
		foreach $s (keys %tune) {
		if ($s == $screen) {
			foreach $modenr (keys %{$tune{$s}}) {
			foreach $modename (keys %{$tune{$s}{$modenr}}) {
			foreach $orig (keys %{$tune{$s}{$modenr}{$modename}}) {

			# look for something to replace...
			# ---------------------------------
			@list = split(/ +/,$orig);
			$dac = shift(@list);

			$t = $tune{$s}{$modenr}{$modename}{$orig};
			@pretiming = split(/ +/,$t);       # saved in the tune hash
			shift(@pretiming);
			@cmptiming = split(/ +/,$ot);      # from xfine

			if (CheckSum(@pretiming) == CheckSum(@cmptiming)) { 
				$new = join(" ",$nt);
				$new = "$dac $new";
				# print STDERR "++++ $modename $orig -> $new\n";
				$tune{$s}{$modenr}{$modename}{$orig} = $new;
				$haschanged = 1;
			} else {
				# mark all other unchanged modelines for deleting...
				# ---------------------------------------------------
				if ($modename eq $res) { 
					$delete{$s}{$modenr}{$modename}{$orig} = "marked";
				}
			}
			# done...
			}
			}
			}
		}
	}
	# realy delete the unused Modelines...
	# -------------------------------------
	if ($haschanged == 1) {
	foreach $s (keys %delete) {
		foreach $modenr (keys %{$delete{$s}}) {
		foreach $modename (keys %{$delete{$s}{$modenr}}) { 
		foreach $orig (keys %{$delete{$s}{$modenr}{$modename}}) {
			# print STDERR "---- $modename $s $modenr $orig -> deleted\n";
			$tune{$s}{$modenr}{$modename}{$orig} = ""; 
		}
		}
		}
	}
	}
	# ...
	} else {
		# ====================================================
		# Hmm, I think we didn`t have any modelines to update
		# just copy them to %tune...
		# ----------------------------------------------------
		$res =~ /(.*)x(.*)/;
		$x = $1;
		$y = $2;
		$modenr = $x + $y;
		$modenr = int($modenr / 100);
	
		$new  = join(" ",$nt);
		$new  = "$maxdac $new";
		$tune{$screen}{$modenr}{$res}{$new} = $new;
		$TuneIn[$screen]++;
	}
	return(%tune);
}


#---[ CheckSum ]-----#
sub CheckSum {
#----------------------------------
# create a checksum of the timings 
#
	my @list = @_;
	my $erg = 0;
	my $i;

	for($i=0;$i<8;$i++) {
		$erg = $erg + $list[$i];
	}
	return($erg);
}


#---[ CompareModelineOnScreen ]-----#
sub CompareModelineOnScreen {
#----------------------------------------
# this function is used to compare the
# tune hash with the configuration hash
# if the modelines are equal return -> 0
# else return -> 1
#
# $var  {Monitor} {$card}  {Modeline} {$modenr}    {$modename} = $timing
# $tune {screen}  {modenr} {modename} {origtiming}             = newtiming
#
	my %tune   = %{$_[0]};
	my %config = %{$_[1]};
	my $screen = $_[2];
	my $s;
	my $modenr;
	my $modename;
	my $t;
	my @tunelist;
	my @configlist;
	my $timi;
	my @list;
	my $orig;
	my $allscreens;
	my $search;
	my $found;
	my $tdim;
	my $cdim;

	$allscreens = 0;
	if ($screen eq "") {
		$allscreens = 1;
	}
	# /.../
	# look if the dimesion has changed...
	# -------------------------------------
	@list = keys(%tune);
	$tdim = @list;
	@list = keys(%{$config{Monitor}});
	$cdim = @list;
	if ($tdim != $cdim) {
		return(1);
	}
	# /.../
	# create modeline list for tune hash...
	# --------------------------------------
	@list = ();
	foreach $s (keys %tune) {
	if ($s == $screen) {
		foreach $modenr (keys %{$tune{$s}}) {
		foreach $modename (keys %{$tune{$s}{$modenr}}) {
		foreach $orig (keys %{$tune{$s}{$modenr}{$modename}}) {
			$t = $orig;
			@list = split(/ +/,$t);
			$timi="$list[0] $list[1] $list[2] $list[3] $list[4] $list[5] $list[6] $list[7] $list[8]";
			push(@tunelist,$timi);
		}
		}
		}
	}
	}
	@tunelist = sort(@tunelist);

	# /.../
	# create modeline list for config hash...
	# ----------------------------------------
	@list = ();
	foreach $s (keys %{$config{Monitor}}) {
	if ($s == $screen) {
		foreach $modenr (keys %{$config{Monitor}{$s}{Modeline}}) {
		foreach $modename (keys %{$config{Monitor}{$s}{Modeline}{$modenr}}) {
			$t = $config{Monitor}{$s}{Modeline}{$modenr}{$modename};
			$t =~ s/\"//g;
			$t =~ s/^ +//g;
			$t =~ s/ +$//g; 
			@list = split(/ +/,$t);
			$timi="$list[0] $list[1] $list[2] $list[3] $list[4] $list[5] $list[6] $list[7] $list[8]";
			push(@configlist,$timi);
		}
		}
	}
	}
	@configlist = sort(@configlist);
	$cdim = @configlist;

	# /.../
	# empty config list...
	# ---------------------
	if ($cdim == 0) { return(1); }

	# /.../
	# test if there is a timing in config which is not in tune 
	# ---------------------------------------------------------
	foreach $search (@configlist) {
		$found  = 0;
		foreach (@tunelist) {  
		if ($_ eq $search) {
			$found = 1; 
		}      
		}
		if ($found == 0) {
			return(1);
		}
	}
	return(0);
}

#---[ TransformTuneHashOnScreen ]-----#
sub TransformTuneHashOnScreen {
#----------------------------------------------
# this function is used to transform the tune
# hash into the config hash format to be able
# to create a new Modes section
#
	my %tune   = %{$_[0]};
	my $screen = $_[1];
 
	my $s;
	my $modenr;
	my $modename;
	my $orig;
	my $t;
	my @list;
	my %var;
	my $allscreens;

	$allscreens = 0;
	if ($screen eq "") {
		$allscreens = 1;
	}

	# create the new Modeline lines...
	# --------------------------------
	foreach $s (keys %tune) {
	if (($s == $screen) || ($allscreens == 1)) {
		foreach $modenr (keys %{$tune{$s}}) {
		foreach $modename (keys %{$tune{$s}{$modenr}}) {
		foreach $orig (keys %{$tune{$s}{$modenr}{$modename}}) {
			$t = $tune{$s}{$modenr}{$modename}{$orig};
			if ($t ne "") {
				$var{Monitor}{$s}{Modeline}{$modenr}{$modename} = $t;
			}
		}
		}
		}
	}
	}
	return(%var);
}

1;
