#----[ AutoDetectServerLayout ]--------#
sub AutoDetectServerLayout {
#-----------------------------------------------------
# detecting ServerLayout according to vendor ID list
# NOTE: this layout is validated and adapted within
# the CreateSections.pm module
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my @vid_list;          # list of vendor ID`s
	my @idquery;           # query list for vendor ID`s
	my $i;                 # loop counter
	my $size;              # number of cards detected

	@idquery = GetQuery("server",\%spec);
	for ($i=6;$i<@idquery;$i+=15) {
		push(@vid_list,$idquery[$i]);
	}
	$size = @vid_list;

	# set detected ServerLayout setting...
	# -------------------------------------
	if ($size > 1) {
	for($i=0;$i<@vid_list;$i++) {
		$var{ServerLayout}{all}{Screen}{$i}{id}       = "Screen\[$i\]";
		$var{ServerLayout}{all}{Screen}{$i}{top}      = "";
		$var{ServerLayout}{all}{Screen}{$i}{bottom}   = "";
		if ($i == 0) {
		$scr = $i + 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}   = "";
		$var{ServerLayout}{all}{Screen}{$i}{left}    = "Screen\[$scr\]";
		} elsif ($i == $size - 1) {
		$scr = $i - 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}   = "Screen\[$scr\]";
		$var{ServerLayout}{all}{Screen}{$i}{left}    = "";
		} else {
		$scr = $i + 1;
		$var{ServerLayout}{all}{Screen}{$i}{left}    = "Screen\[$scr\]";
		}
	}
	}
}

1;
