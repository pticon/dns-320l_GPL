use strict;
use warnings;
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

use AccessorGroupsSubclass;
$Class::Accessor::Grouped::USE_XS = 1;

my $obj = AccessorGroupsSubclass->new;
my $obj2 = AccessorGroups->new;
my $deferred_stub = AccessorGroupsSubclass->can('singlefield');

my @w;
{
  local $SIG{__WARN__} = sub { push @w, @_ };
  is ($obj->$deferred_stub(1), 1, 'Set');
  is ($obj->$deferred_stub, 1, 'Get');
  is ($obj->$deferred_stub(2), 2, 'ReSet');
  is ($obj->$deferred_stub, 2, 'ReGet');

  is ($obj->singlefield, 2, 'Normal get');
  is ($obj2->singlefield, undef, 'Normal get on unrelated object');
}

is (@w, 3, '3 warnings total');

is (
  scalar (grep { $_ =~ /^\QDeferred version of method AccessorGroups::singlefield invoked more than once/ } @w),
  3,
  '3 warnings produced as expected on cached invocation during testing',
) or do {
  require Data::Dumper;
  diag "\n \$0 is: " . Data::Dumper->new([$0])->Useqq(1)->Terse(1)->Dump;
};

done_testing;
