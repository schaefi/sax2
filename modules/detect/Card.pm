#----[ AutoDetectServer ]--------#
sub AutoDetectServer {
#---------------------------------------------------
# this function calls the sysp -q server  
# routine to get Server information      
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my @query;             # the query string
	my @dom_list;          # list of domain id's
	my @bus_list;          # list of bus id`s
	my @slot_list;         # list of slot id`s
	my @func_list;         # list of function id`s
	my @vendor_list;       # list of vendor names
	my @device_list;       # list of device names
	my @module_list;       # list of modules
	my @drvprofile_list;   # list of Driver profiles
	my $bus;               # bus  id DEZ
	my $slot;              # slot id DEZ 
	my $func;              # func id DEZ
	my $dom;               # domain id DEZ
	my $glx = "open";      # 3D and rendering support

	@query = GetQuery("server",\%spec);
	if ($QueryStatus == -1) {
		return(-1);
	}

	for ($i=0;$i<@query;$i+=15) {
		push (@dom_list,$query[$i]);
	}
	for ($i=1;$i<@query;$i+=15) {
		push (@bus_list,$query[$i]);
	}
	for ($i=2;$i<@query;$i+=15) {
		push (@slot_list,$query[$i]);
	}
	for ($i=3;$i<@query;$i+=15) {
		push (@func_list,$query[$i]);
	}
	for ($i=4;$i<@query;$i+=15) {
		push (@vendor_list,$query[$i]);
	}
	for ($i=5;$i<@query;$i+=15) {
		push (@device_list,$query[$i]);
	}
	for ($i=8;$i<@query;$i+=15) {
		push (@module_list,$query[$i]);
	}
	for ($i=14;$i<@query;$i+=15) {
		push (@drvprofile_list,$query[$i]);
	}

	# set detected busid strings...
	# ------------------------------- 
	for($i=0;$i<@bus_list;$i++) {
	if (($bus_list[$i] !~ /unknown/i) && ($module_list[$i] ne "fbdev")) {
		$bus  = hex($bus_list[$i]); 
		$slot = hex($slot_list[$i]);
		$func = hex($func_list[$i]);
		$dom  = hex($dom_list[$i]);
		$arch = qx(/bin/arch);
		chomp $arch;
		# specify domain if > 0 (bnc #583359)
		if ($dom > 0) {
			$busid = "PCI:$bus\@$dom:$slot:$func";
		} else {
			$busid = "$bus:$slot:$func";
		}
		$var{Device}{$i}{BusID} = $busid;
	} else {
		$var{Device}{$i}{BusID} = "";
	}
	}

	# set detected vendor names...
	# ----------------------------
	for($i=0;$i<@vendor_list;$i++) {
		$var{Device}{$i}{VendorName} = $vendor_list[$i];
	}

	# set detected device names...
	# ----------------------------
	for($i=0;$i<@device_list;$i++) {
		$var{Device}{$i}{BoardName} = $device_list[$i];
	}

	# set detected modules...
	# ------------------------
	for($i=0;$i<@module_list;$i++) {
		$var{Device}{$i}{Driver} = $module_list[$i];
		if ($module_list[$i] eq "vga") {
			$var{Screen}{$i}{Depth}{4}{Visual} = "PseudoColor";
		}
	}

	# set detected driver profiles...
	# --------------------------------
	for($i=0;$i<@drvprofile_list;$i++) {
		$xdp{DriverProfile}{$i} = $drvprofile_list[$i];
	}

	# set identifier...
	# -----------------
	for($i=0;$i<@module_list;$i++) {
		$var{Device}{$i}{Identifier} = "Device\[$i\]";
	}
	return(%var);
}

1;
