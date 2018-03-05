#!perl -wT
# $Id: /bast/Class-Accessor-Grouped/trunk/t/basic.t 3253 2007-05-06T02:24:39.381139Z claco  $
use strict;
use warnings;

BEGIN {
    use lib 't/lib';
    use Test::More tests => 1;

    use_ok('Class::Accessor::Grouped');
};
