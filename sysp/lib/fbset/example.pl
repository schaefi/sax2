#use lib 'blib/arch/auto/FBSet';

use FBSet;

my $data = FBSet::FbGetData();
my $x = $data->swig_x_get();

print "$x\n";
