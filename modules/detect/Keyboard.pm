#----[ AutoDetectKeyboard ]-------#
sub AutoDetectKeyboard {
#---------------------------------------------------
# this function calls the sysp -q keyboard routine 
# to get Keyboard information. NOTE: only one keyboard 
# may be detected from sysp 
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my $kbd = 0;           # number of keyboard
	my %query;             # the query string
	my $i;
	my $value;

	%query = GetHotQuery("keyboard",\%spec);
	if ($QueryStatus == -1) {
		return(-1);
	}

	foreach $i (sort keys %{$query{$kbd}}) {
		$value = $query{$kbd}{$i};
		$value =~ s/^ +//g;
		$value =~ s/ +$//g;
		if ($i =~ /XkbVariant/)  {
			$var{InputDevice}{0}{Option}{XkbVariant}  = $value;
		}
		if ($i =~ /XkbLayout/)   {
			$var{InputDevice}{0}{Option}{XkbLayout}   = $value;
		}
		if ($i =~ /XkbModel/)    {
			$var{InputDevice}{0}{Option}{XkbModel}    = $value;
		}
		if ($i =~ /XkbRules/)    {
			$var{InputDevice}{0}{Option}{XkbRules}    = $value;
		}
		if ($i =~ /MapName/)     {
			$var{InputDevice}{0}{Option}{MapName}     = $value;
		}
		if ($i =~ /Protocol/)    {
			$var{InputDevice}{0}{Option}{Protocol}    = $value;
		}
		if ($i =~ /XkbOptions/)  {
			$var{InputDevice}{0}{Option}{XkbOptions}  = $value;
		}
		if ($i =~ /XkbKeyCodes/) {
			$var{InputDevice}{0}{Option}{XkbKeyCodes} = $value;
		}
		if ($i =~ /LeftAlt/)     {
			$var{InputDevice}{0}{Option}{LeftAlt}     = $value;
		}
		if ($i =~ /RightAlt/)    {
			$var{InputDevice}{0}{Option}{RightAlt}    = $value;
		}
		if ($i =~ /ScrollLock/)  {
			$var{InputDevice}{0}{Option}{ScrollLock}  = $value;
		}
		if ($i =~ /RightCtl/)    {
			$var{InputDevice}{0}{Option}{RightCtl}    = $value;
		}
		if ($i =~ /Profile/)     {
			$idp{KeyboardProfile}{0} = $value;
		}
		if ($i =~ /evBits/)      {
			$var{InputDevice}{0}{Option}{evBits}      = $value;
		}
		if ($i =~ /absBits/)      {
			$var{InputDevice}{0}{Option}{absBits}     = $value;
		}
		if ($i =~ /keyBits/)     {
			$var{InputDevice}{0}{Option}{keyBits}     = $value;
		}
		if ($i =~ /Pass/)        {
			$var{InputDevice}{0}{Option}{Pass}        = $value;
		}
		if ($i =~ /HWHEELRelativeAxisButtons/) {
			$var{InputDevice}{0}{Option}{HWHEELRelativeAxisButtons} = $value;
		}
		if ($i =~ /evendor/)     {
			$var{InputDevice}{0}{Option}{evendor}     = $value;
		}
		if ($i =~ /product/)     {
			$var{InputDevice}{0}{Option}{product}     = $value;
		}
	}
	return(%var);
}

1;
