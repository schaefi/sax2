#================================
# Globals...
#-------------------------------- 
$ldnr = 0;

#---[ ResetLDNr ]------#
sub ResetLDNr {
#-------------------------------------------------
# reset ldnr number for Modeline stuff
#
	$ldnr = 0;
}

#----[ AutoDetectSyncRanges ]-----#
sub AutoDetectSyncRanges {
#-------------------------------------------------
# detecting sync ranges the name of the monitor
# and the Identifier for the monitor
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my %query;             # the query hash
	my @vsync_list;        # Vsync list
	my @hsync_list;        # Hsync list
	my @ddc_list;          # list of monitor manufacturer ID`s
	my @model_list;        # list of monitor names
	my @vendor_list;       # list of monitor vendor names
	my @vesa_list;         # vesa mode list
	my @dtype_list;        # display type list
	my $size;              # number of cards
	my $card;              # current card number in loop
	my $i;                 # loop counter
	my $detectvesa;        # detecting vesa modes y/n
	my $hsmin;             # hsync min
	my $hsmax;             # hsync max
	my $vsmin;             # vsync min
	my $vsmax;             # vsync max
	my @record;            # DDC record list 
	my $rsize;             # size of record list
	my @res;               # VESA resolution list
	my $res;               # selected resolution from list
	my @vesa;              # special modeline list
	my $ml;                # special modeline from vesa list
	my $tim;               # special modeline timing
	my $resolutionSet = 0; # is resolution set by CDB Monitor entry 

	$size  = 0;
	%query = GetHotQuery("xstuff",\%spec);
	foreach $card (sort numbers keys %query) {
	if ($card ne "") {
		$detectvesa = 0;
		foreach $i (sort keys %{$query{$card}}) {
		SWITCH: for ($i) {
			# vertical sync ranges...
			# -------------------------
			/^Vsync$/      && do {
			push (@vsync_list,$query{$card}{$i});
			last SWITCH;
			};
			# horizontal sync ranges...
			# ---------------------------
			/^Hsync$/      && do {
			push (@hsync_list,$query{$card}{$i});
			last SWITCH;
			};
			# DDC Id list...
			# ---------------
			/^DDC$/        && do {
			push (@ddc_list,$query{$card}{$i});
			last SWITCH;
			};
			# Model Name list...
			# ------------------
			/^Name$/       && do {
			push (@model_list,$query{$card}{$i});
			last SWITCH;
			};
			# Vendor Name list...
			# ---------------------
			/^Vendor$/     && do {
			push (@vendor_list,$query{$card}{$i});
			last SWITCH;
			};
			# display type list...
			# ---------------------
			/^Display$/    && do {
			push (@dtype_list,$query{$card}{$i});
			last SWITCH;
			};
			# vesa frequencies list...
			# -------------------------
			/^Vesa$/       && do {
			$detectvesa = 1;
			push(@vesa_list,"true");
			last SWITCH;
			};
		}
		}
		if ($detectvesa == 0) {
			push (@vesa_list,"false");
		}
		$size++;
	}
	}
	# set detected monitor HorizSync setting...
	# ------------------------------------------
	for($i=0;$i<@hsync_list;$i++) {
		$hsmax = int($hsync_list[$i]);
		$hsmin = 15;
		$var{Monitor}{$i}{HorizSync}  = "$hsmin-$hsmax";
	}
 
	# set detected monitor VertRefresh setting...
	# ------------------------------------------
	for($i=0;$i<@vsync_list;$i++) {
		$vsmax = int($vsync_list[$i]);
		$vsmin = 50;
		$var{Monitor}{$i}{VertRefresh} = "$vsmin-$vsmax";
	}

	# set detected monitor Identifier and a default
	# name according to the contents of the vesa list
	# ------------------------------------------------
	for($i=0;$i<@vsync_list;$i++) {
	$var{Monitor}{$i}{Identifier}   = "Monitor\[$i\]";
	$var{Monitor}{$i}{VendorName}   = "Unknown";
	$var{Monitor}{$i}{ModelName}    = "Unknown";
  
	if (($vesa_list[$i] ne "true") || ($var{Device}{$i}{Driver} eq "vga")) {
		# we did not get any VESA information therefore we set the 
		# sync ranges to save values to avoid any monitor damage
		# ----------------------------------------------------------
		$var{Monitor}{$i}{VertRefresh} = "50-75";
		$var{Monitor}{$i}{HorizSync}   = "30-40"; 
		$var{Monitor}{$i}{VendorName}  = "Unknown";
		$var{Monitor}{$i}{ModelName}   = "Unknown";
	}
	} 

	# set detected DDC capabilities.
	# this may overwrite the Monitor Vendor/Name and sync values
	# /.../
	# - Set Vendor and Name ( or VESA / LCD )
	# - Set sync ranges according to the vendor
	# - Calculate one modeline for each vesa resolution if 
	#   the monitor is identified via real DDC-ID
	# ------------------------------------------------
	%MonitorList = GetMonitorList($spec{Monitors});
	for($i=0;$i<@ddc_list;$i++) {
	if ($var{Device}{$i}{Driver} eq "vga") {
		next;
	}
	$ddc_list[$i] =~ s/ +//g;
	@record = GetDDCRecord(
		$ddc_list[$i],$model_list[$i],$vendor_list[$i],
		$hsync_list[$i],$vsync_list[$i],$spec{Monitors}
	);
	$rsize  = @record;

	if ($rsize > 0) {
		# =======================================================
		# We got a real DDC-ID from the monitor...
		# ------------------------------------------------------- 
		$var{Monitor}{$i}{VendorName}  = "$record[0]";
		$var{Monitor}{$i}{ModelName}   = "$record[1]"; 
		$var{Monitor}{$i}{HorizSync}   = "$record[2]-$record[3]";
		$var{Monitor}{$i}{VertRefresh} = "$record[4]-$record[5]";
		if ($record[8] ne "") {
		foreach my $bpp (8,15,16,24) {
			$var{Screen}{$i}{Depth}{$bpp}{Modes}  = $record[8];
			$resolutionSet = 1;
		}
		}

		# look for modeline defined...
		# -----------------------------
		%var = LookupSpecialModeline(\%var,$record[6],$i);

		# look if there is a profile for the monitor
		# -------------------------------------------
		$mop{MonitorProfile}{$i}  = "unknown";
		if ($record[7] ne "") {
			$mop{MonitorProfile}{$i} = $record[7];
		}
	} else { 
		# =======================================================
		# We did not get a DDC-ID try VESA or Panel selection...
		# -------------------------------------------------------
		@res = ();
		foreach $vesanr (sort keys %{$query{$i}{Vesa}}) {
			@vesa   = split(/ /,$query{$i}{Vesa}{$vesanr});
			$mode   = join("x",$vesa[0],$vesa[1]);
			$lookup{$mode} = $vesa[0]."X".$vesa[1]."\@$vesa[3]HZ";
			push(@res,$mode);
		}
		@res = sortres(@res);
		$res = $lookup{$res[0]};

		if ($dtype_list[$i] eq "CRT") {
		# =======================================================
		# We are using a CRT monitor try VESA...
		# -------------------------------------------------------
		@record = GetGroupRecord("--> VESA",$res);
		$rsize  = @record;
		if ($rsize > 0) {
			$var{Monitor}{$i}{VendorName}  = "$record[0]";
			$var{Monitor}{$i}{ModelName}   = "$record[1]";
			$var{Monitor}{$i}{HorizSync}   = "$record[2]-$record[3]";
			$var{Monitor}{$i}{VertRefresh} = "$record[4]-$record[5]";
			# /.../
			# look for modeline defined...
			# -----------------------------
			%var = LookupSpecialModeline(\%var,$record[6],$i);
		}
		} else {
		# =======================================================
		# We are using a Panel try LCD...
		# -------------------------------------------------------
		@record = GetGroupRecord("--> LCD",$res);
		$rsize  = @record;
		if ($rsize > 0) {
			$var{Monitor}{$i}{VendorName}  = "$record[0]";
			$var{Monitor}{$i}{ModelName}   = "$record[1]";
			$var{Monitor}{$i}{HorizSync}   = "$record[2]-$record[3]";
			$var{Monitor}{$i}{VertRefresh} = "$record[4]-$record[5]";
			# /.../
			# look for modeline defined...
			# -----------------------------
			%var = LookupSpecialModeline(\%var,$record[6],$i);
		}
		}
	} 
	}
	return ($resolutionSet);
}

#----[ GetDDCRecord ]----------#
sub GetDDCRecord {
#-----------------------------------------------
# this function returns a list including CDB data
# according to a DDC ID. The list has the 
# following structure:
#
# 0 -> vendor
# 1 -> name
# 2 -> min HS
# 3 -> max HS
# 4 -> min VS
# 5 -> max VS
# 6 -> modeline if exist
# 7 -> profile  if exist
# 8 -> resolution if exist
#
	my $id      = $_[0];
	my $model   = $_[1];
	my $vendstr = $_[2];
	my $hsync   = $_[3];
	my $vsync   = $_[4];
	my $CDBFile = $_[5];
	my %result  = %MonitorList;

	my @ddc;
	my $vendor;
	my $name;
	my $value;
	my $found;
	my $max;

	#=========================================
	# look for the ID in the Monitor hash...
	#-----------------------------------------
	$found = 0;
	if ($id ne "<undefined>") {
	$id =~ s/ +//g; $id = lc($id);
	foreach $vendor (keys %result) {
	if ($found) { last; }
	foreach $name (keys %{$result{$vendor}}) {
		if (! defined $result{$vendor}{$name}{DDC}) {
			next;
		}
		$value = $result{$vendor}{$name}{DDC};
		$value =~ s/ +//g;
		$value = lc($value);
		if ($value eq "") {
			next;
		}
		if ($value eq $id) {
			push(@ddc,$vendor);
			push(@ddc,$name);
			$result{$vendor}{$name}{Hsync} =~ /(.*)-(.*)/;
			push(@ddc,$1);
			push(@ddc,$2);
			$result{$vendor}{$name}{Vsync} =~ /(.*)-(.*)/;
			push(@ddc,$1);
			push(@ddc,$2);
			push(@ddc,$result{$vendor}{$name}{Modeline});
			push(@ddc,$result{$vendor}{$name}{Profile});
			push(@ddc,$result{$vendor}{$name}{Resolution});
			$found = 1;
			last;
		}
	}
	}
	if (defined $ddc[0]) {
		if (! defined $ddc[6]) { $ddc[6] = ""; }
		if (! defined $ddc[7]) { $ddc[7] = ""; }
	}
	if (! defined $ddc[8]) {
		$ddc[8] = "";
	}
	if (defined $ddc[0]) {
		return (@ddc);
	}
	}
	#=========================================
	# If ID wasn't found but detected, add it
	#-----------------------------------------
	if (($id ne "<undefined>") && ($hsync > 30) && ($vsync > 43)) {
		if (! open (FD,">>$CDBFile")) {
			return;
		}
		$id      = uc ($id);
		$vendstr = uc ($vendstr);
		$model   = uc ($model);
		print FD "#==============================================\n";
		print FD "# Entry created from init stage\n";
		print FD "#----------------------------------------------\n";
		print FD "$vendstr:$model \{\n";
		print FD " DDC=$id\n";
		print FD " Hsync=30-$hsync\n";
		print FD " Vsync=43-$vsync\n";
		print FD "\}\n";
		close FD;
		%MonitorList = GetMonitorList ( $CDBFile );
		return ( GetDDCRecord (
			$id,$model,$vendstr,$hsync,$vsync,$CDBFile
		));
	}
	return;
}

#----[ GetGroupRecord ]------------#
sub GetGroupRecord {
#-------------------------------------------------
# this function will search the Monitor record 
# for the given name within the --> VESA section
#
# 0 -> vendor
# 1 -> name
# 2 -> min HS
# 3 -> max HS
# 4 -> min VS
# 5 -> max VS
# 6 -> Modeline
#
	my $vendor = $_[0];
	my $name   = $_[1];
	my %result = %MonitorList;

	my @ddc;

	if (defined $result{$vendor}{$name}) {
		push(@ddc,$vendor);
		push(@ddc,$name);
		$result{$vendor}{$name}{Hsync} =~ /(.*)-(.*)/;
		push(@ddc,$1);
		push(@ddc,$2);
		$result{$vendor}{$name}{Vsync} =~ /(.*)-(.*)/;
		push(@ddc,$1);
		push(@ddc,$2);
		push(@ddc,$result{$vendor}{$name}{Modeline});
	}
	return(@ddc);
}

#----[ LookupSpecialModeline ]----#
sub LookupSpecialModeline {
#------------------------------------------------------
# look if there is a modeline defined in the Monitors
# file according to made selection
#
	my (%var)     = %{$_[0]};
	my $record    = $_[1];
	my $i         = $_[2];

	my @vesa;     # list of modelines
	my $ml;       # one line from the modeline list
	my $res;      # the resolution of the line
	my $tim;      # the timing of the line

	if ($record ne "") {
	$var{Desktop}{$i}{SpecialModeline} = $record;
	@vesa = split(/,/,$record);
	foreach $ml (@vesa) {
		$ml =~ /\"(.*)\" (.*)/;
		$res   = $1;
		$tim   = $2;
		$var{Monitor}{$i}{Modeline}{$ldnr}{$res} = $tim;
		$ldnr++;
	}
	}
	return(%var);
}

#----[ GetMonitorList ]----------#
sub GetMonitorList {
#---------------------------------------------
# use this function to incorp data which is 
# handled from CDB output
#
	my $filename = $_[0];
	my %result;
	my $line;
	my $vendor;
	my $name;
	my $option;
	my $value;
	my @file;
	my $p;
	my $base;
	my $reading;

	@file = ();
	open("DATA",$filename);
	while($line=<DATA>) {
		chomp($line);
		push(@file,$line);
		$filesize++;
	}
	close(DATA);

	foreach $line (@file) {
	if (($line =~ /^#/) || ($line eq "")) {
		next;
	}
	if ($line =~ /(.*):(.*)\{/) {
		# get the topic line...
		# ----------------------
		$vendor = $1;
		$name   = $2;
		$vendor =~ s/^ +//g; $vendor =~ s/ +$//g;
		$name   =~ s/^ +//g; $name   =~ s/ +$//g;
	} elsif ($line =~ /(.*)=(.*)/) {
		# get the values behind this topic...
		# ------------------------------------
		$option = $1;
		$value  = $2;
		$option =~ s/^ +//g; $option =~ s/ +$//g;
		$value  =~ s/^ +//g; $value  =~ s/ +$//g;

		if ((defined $name) && (defined $vendor)) {
			$result{$vendor}{$name}{$option} = $value;
		}
	}
	}
	return(%result);
}

1;
