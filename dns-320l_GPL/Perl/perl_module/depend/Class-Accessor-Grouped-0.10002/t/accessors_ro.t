use Test::More tests => 48;
use Test::Exception;
use strict;
use warnings;
use lib 't/lib';

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

use AccessorGroupsRO;

my $obj = AccessorGroupsRO->new;

{
    my $warned = 0;

    local $SIG{__WARN__} = sub {
        if  (shift =~ /DESTROY/i) {
            $warned++;
        };
    };

    no warnings qw/once/;
    local *AccessorGroupsRO::DESTROY = sub {};

    $obj->mk_group_ro_accessors('warnings', 'DESTROY');

    ok($warned);
};

my $test_accessors = {
    singlefield => {
        is_xs => $use_xs,
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

    can_ok($obj, $name, $alias);

    ok(!$obj->can($field))
      if $field ne $name;

    is($obj->$name, undef);
    is($obj->$alias, undef);

    # get via name
    $obj->{$field} = 'a';
    is($obj->$name, 'a');

    # alias gets same as name
    is($obj->$alias, 'a');

    my $ro_regex = $test_accessors->{$name}{is_xs}
        ? qr/Usage\:.+$name.*\(self\)/
        : qr/cannot alter the value of '\Q$field\E'/
    ;

    # die on set via name/alias
    throws_ok {
        $obj->$name('b');
    } $ro_regex;

    throws_ok {
        $obj->$alias('b');
    } $ro_regex;

    # value should be unchanged
    is($obj->$name, 'a');
    is($obj->$alias, 'a');
};

#important
1;
