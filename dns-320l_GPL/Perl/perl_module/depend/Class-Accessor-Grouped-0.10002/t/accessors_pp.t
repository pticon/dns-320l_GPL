use strict;
use warnings;
use FindBin qw($Bin);
use File::Spec::Functions;
use File::Spec::Unix (); # need this for %INC munging
use Test::More;
use lib 't/lib';

BEGIN {
  eval { require Devel::Hide };
  if ($@) {
    eval { require Sub::Name };
    plan skip_all => "Devel::Hide required for this test in presence of Sub::Name"
      if ! $@;
  }
  else {
    Devel::Hide->import('Sub/Name.pm');
  }
  require Class::Accessor::Grouped;
}

# rerun the regular 3 tests under the assumption of no Sub::Name
for my $tname (qw/accessors.t accessors_ro.t accessors_wo.t/) {

  subtest "$tname without Sub::Name (pass $_)" => sub {
    my $tfn = catfile($Bin, $tname);

    delete $INC{$_} for (
      qw/AccessorGroups.pm AccessorGroupsRO.pm AccessorGroupsSubclass.pm AccessorGroupsWO.pm/,
      File::Spec::Unix->catfile ($tfn),
    );

    local $SIG{__WARN__} = sub { warn @_ unless $_[0] =~ /subroutine .+ redefined/i };

    do($tfn);

  } for (1 .. 2);
}

done_testing;
