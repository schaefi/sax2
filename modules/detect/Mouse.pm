#----[ AutoDetectMouse ]---------#
sub AutoDetectMouse {
#---------------------------------------------------
# this function calls the sysp -q mouse  routine 
# to get mouse information       
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my %query;             # the query string
	my @prot_list;         # list of protocols 
	my @dev_list;          # list of devices
	my @btn_list;          # list of button numbers
	my @whl_list;          # list of wheel numbers
	my @emu_list;          # list of emulate btn flags
	my @name_list;         # list of Mouse names
	my @prof_list;         # list of Mouse profiles
	my $mouse;             # mouse key in query hash
	my $value;             # hash value 
	my $i;                 # hash count
 
	%query = GetHotQuery("mouse",\%spec);
	if ($QueryStatus == -1) {
		return(-1);
	}
	foreach $mouse (sort numbers keys %query) {
	if ($mouse ne "") {
		push (@dev_list ,$query{$mouse}{Device});
		push (@prot_list,$query{$mouse}{Protocol});
		push (@btn_list, $query{$mouse}{Buttons});
		push (@whl_list, $query{$mouse}{Wheel});
		push (@emu_list, $query{$mouse}{Emulate});
		push (@name_list,$query{$mouse}{Name});
		push (@prof_list,$query{$mouse}{Profile});
		push (@nut_list, $query{$mouse}{NutShell});
	}
	}
	# /.../
	# set detected mouse protocols...
	# ------------------------------- 
	$index = 1;
	for($i=0;$i<@prot_list;$i++) {
		# /.../
		# check if the mouse device is a combined device handled
		# by one multiplexer device. The field NutShell = 0 indicates
		# the device needs to be handled in an InputDevice section
		# if NutShell != 0 the device is handled in the kernel. At least
		# one NutShell = 0 exists when a mouse is connected
		# ----
		if ($nut_list[$i] != 0) {
			next;
		}
		$var{InputDevice}{$index}{Identifier}       = "Mouse\[$index\]"; 
		$var{InputDevice}{$index}{Option}{Vendor}   = "Sysp";
		$var{InputDevice}{$index}{Option}{Name}     = $name_list[$i];
		$var{InputDevice}{$index}{Option}{Protocol} = $prot_list[$i];
		if ($prot_list[$i] =~ /AUTO/i) {
			$var{InputDevice}{$index}{Option}{AutoSoft}  = "on";
		}
		# this enables Emulate3Buttons per default...
		# --------------------------------------------
		# $var{InputDevice}{$index}{Option}{Emulate3Buttons} = "on";
		$index = $index + 2;
	}
	# /.../
	# set detected mouse devices...
	# -----------------------------
	$index = 1;
	for($i=0;$i<@dev_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		$var{InputDevice}{$index}{Option}{Device} = $dev_list[$i];
		$index = $index + 2;
	}
	# /.../
	# set detected mouse profiles...
	# -------------------------------
	$index = 1;
	for($i=0;$i<@dev_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		$idp{MouseProfile}{$index} = $prof_list[$i];
		$index = $index + 2;
	}
	# /.../
	# set detected wheel options...
	# -------------------------------
	$index = 1;
	for($i=0;$i<@whl_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		if ($whl_list[$i] > 0) { 
			$var{InputDevice}{$index}{Option}{ZAxisMapping} = "4 5";
		}
		$index = $index + 2;
	}
	# /.../
	# set detected button number, enable wheel emulation if needed...
	# ---------------------------------------------------------------
	$index = 1;
	for($i=0;$i<@btn_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		if ($btn_list[$i] > 3) {
			$var{InputDevice}{$index}{Option}{Buttons} = $btn_list[$i];
		}
		if (($btn_list[$i] == 4) && ($whl_list[$i] == 0)) {
			$var{InputDevice}{$index}{Option}{EmulateWheel} = "on";
			$var{InputDevice}{$index}{Option}{EmulateWheelButton} = 4;
		}
		$index = $index + 2;
	}
	# /.../
	# set detected emulate 3 button option...
	# -----------------------------------------
	$index = 1;
	for($i=0;$i<@emu_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		if ($emu_list[$i] == 1) {
			$var{InputDevice}{$index}{Option}{Emulate3Buttons} = "on";
		}
		$index = $index + 2;
	}
	# /.../
	# set detected core layout...
	# -----------------------------
	my %entity;
	$index = 1;
	for($i=0;$i<@dev_list;$i++) {
		if ($nut_list[$i] != 0) {
			next;                 
		}
		$var{InputDevice}{$index}{Driver} = "mouse";
		if ($index >= 3) {
			my $driver = $var{InputDevice}{$index}{Driver};
			my $device = $var{InputDevice}{$index}{Option}{Device};
			my $profile= $prof_list[$i];
			# /.../
			# send core events only if driver and device are
			# not yet in use !
			# ----
			if (! defined $entity{$driver}{$device}{$profile}) {
				my $l = "ServerLayout";
				$var{$l}{all}{InputDevice}{$index}{id} = "Mouse[$index]";
				$var{$l}{all}{InputDevice}{$index}{usage} = "SendCoreEvents";
			}
			$var{InputDevice}{$index}{Identifier};
			$entity{$driver}{$device}{$profile} = $index;
		}
		$index = $index + 2;
	}
	return(%var);
}

1;
