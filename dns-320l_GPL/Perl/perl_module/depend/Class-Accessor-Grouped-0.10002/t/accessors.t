use Test::More tests => 117;
use strict;
use warnings;
use lib 't/lib';
use B qw/svref_2object/;

# we test the pure-perl versions only, but allow overrides
# from the accessor_xs test-umbrella
# Also make sure a rogue envvar will not interfere with
# things
my $use_xs;
BEGIN {
    $Class::Accessor::Grouped::USE_XS = 0
        unless defined $Class::Accessor::Grouped::USE_XS;
    $ENV{CAG_USE_XS} = 1;
    $use_xs = $Class::Accessor::Grouped::USE_XS;
};

use AccessorGroupsSubclass;

{
    my $obj = AccessorGroupsSubclass->new;
    my $class = ref $obj;
    my $name = 'multiple1';
    my $alias = "_${name}_accessor";

    my $warned = 0;
    local $SIG{__WARN__} = sub {
        if  (shift =~ /DESTROY/i) {
            $warned++;
        };
    };

    no warnings qw/once/;
    local *AccessorGroupsSubclass::DESTROY = sub {};

    $class->mk_group_accessors('warnings', 'DESTROY');
    ok($warned);
};

my $obj = AccessorGroupsSubclass->new;

my $test_accessors = {
    singlefield => {
        is_xs => $use_xs,
        has_extra => 1,
    },
    runtime_around => {
        # even though this accessor is simple it will *not* be XSified
        # due to the runtime 'around'
        is_xs => 0,
        has_extra => 1,
    },
    multiple1 => {
    },
    multiple2 => {
    },
    lr1name => {
        custom_field => 'lr1;field',
    },
    lr2name => {
        custom_field => "lr2'field",
    },
};

for my $name (sort keys %$test_accessors) {
    my $alias = "_${name}_accessor";
    my $field = $test_accessors->{$name}{custom_field} || $name;
    my $extra = $test_accessors->{$name}{has_extra};

    can_ok($obj, $name, $alias);
    ok(!$obj->can($field))
      if $field ne $name;

    for my $meth ($name, $alias) {
        my $cv = svref_2object( $obj->can($meth) );
        is($cv->GV->NAME, $meth, "$meth accessor is named");
        is($cv->GV->STASH->NAME, 'AccessorGroups', "$meth class correct");
    }

    is($obj->$name, undef);
    is($obj->$alias, undef);

    # get/set via name
    is($obj->$name('a'), 'a');
    is($obj->$name, 'a');
    is($obj->{$field}, $extra ? 'a Extra tackled on' : 'a');

    # alias gets same as name
    is($obj->$alias, 'a');

    # get/set via alias
    is($obj->$alias('b'), 'b');
    is($obj->$alias, 'b');
    is($obj->{$field}, $extra ? 'b Extra tackled on' : 'b');

    # alias gets same as name
    is($obj->$name, 'b');

    for my $meth ($name, $alias) {
        my $cv = svref_2object( $obj->can($meth) );
        is($cv->GV->NAME, $meth, "$meth accessor is named after operations");
        is(
          $cv->GV->STASH->NAME,
          # XS lazyinstalls install into each caller, not into the original parent
          $test_accessors->{$name}{is_xs} ? 'AccessorGroupsSubclass' :'AccessorGroups',
          "$meth class correct after operations",
        );
    }
};

# important
1;
