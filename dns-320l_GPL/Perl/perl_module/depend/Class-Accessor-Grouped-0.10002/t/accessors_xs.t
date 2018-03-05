use strict;
use warnings;
use FindBin qw($Bin);
use File::Spec::Functions;
use File::Spec::Unix (); # need this for %INC munging
use Test::More;
use lib 't/lib';

BEGIN {
    require Class::Accessor::Grouped;
    my $xsa_ver = $Class::Accessor::Grouped::__minimum_xsa_version;
    eval {
        require Class::XSAccessor;
        Class::XSAccessor->VERSION ($xsa_ver);
    };
    plan skip_all => "Class::XSAccessor >= $xsa_ver not available"
      if $@;
}

# rerun the regular 3 tests under XSAccessor
$Class::Accessor::Grouped::USE_XS = 1;
for my $tname (qw/accessors.t accessors_ro.t accessors_wo.t/) {

  subtest "$tname with USE_XS (pass $_)" => sub {
    my $tfn = catfile($Bin, $tname);

    for (
      qw|AccessorGroups.pm AccessorGroups/BeenThereDoneThat.pm AccessorGroupsRO.pm AccessorGroupsSubclass.pm AccessorGroupsWO.pm|,
      File::Spec::Unix->catfile ($tfn),
    ) {
      delete $INC{$_};
      no strict 'refs';
      if (my ($mod) = $_ =~ /(.+)\.pm$/ ) {
        %{"${mod}::"} = ();
      }
    }

    local $SIG{__WARN__} = sub { warn @_ unless $_[0] =~ /subroutine .+ redefined/i };

    do($tfn);

  } for (1 .. 2);
}

done_testing;
