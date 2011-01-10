use strict;

my(@V);
my(@VN);
my(@F);

while (<>) {
	split;
	my($cmd) = shift @_;
	if ($cmd eq 'v') {
		push @V, [@_];
	} elsif ($cmd eq 'vn') {
		push @VN, [@_];
	} elsif ($cmd eq 'f') {
		my($vertices) = [];
		for (@_) {
			my($iv, $ivt, $ivn) = split('/');
			push @$vertices, { v => $iv, vn => $ivn };
		}
		push @F, $vertices;
	} else {
		# ignore anything else
	}
}

print scalar(@F), "\n";

for (@F) {
	print "9 ", scalar(@$_), "\n"; # polygon=9
	
	for (@$_) {
		my($v) = @V[$_->{v} - 1];
		my($vn) = @VN[$_->{vn} - 1];
		print "$v->[0] $v->[1] $v->[2] $vn->[0] $vn->[1] $vn->[2]\n";
	}
}
