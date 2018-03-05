BEGIN {
   if (exists $ENV{AUTOMATED_TESTING}) {
      print "1..0 # Skipped: Too many broken cpan tester setups.\n";
      exit;
   }
}
BEGIN { $| = 1; print "1..12\n"; }

# a surprisingly effective test by brandon black

no warnings;
use strict;

use EV;

{
   my $a = EV::timer 0.6, 0, sub { print "not ok 2\n"; EV::unloop };
   my $b = EV::timer 0.3, 0, sub { print "ok 2\n"; EV::unloop };

   print "ok 1\n";
   EV::loop;
   print "ok 3\n";
}

{
   my $b = EV::timer 0.3, 0, sub { print "ok 5\n"; EV::unloop };
   my $a = EV::timer 0.6, 0, sub { print "not ok 5\n"; EV::unloop };

   print "ok 4\n";
   EV::loop;
   print "ok 6\n";
}

{
   my $a = EV::timer 0.9, 0, sub { print "not ok 8\n"; EV::unloop };
   my $b = EV::timer 0.6, 0, sub { print "not ok 8\n"; EV::unloop };
   my $c = EV::timer 0.3, 0, sub { print "ok 8\n"; EV::unloop };

   print "ok 7\n";
   EV::loop;
   print "ok 9\n";
}
{

   my $a = EV::timer 0.6, 0, sub { print "not ok 11\n"; EV::unloop };
   my $b = EV::timer 0.3, 0, sub { print "ok 11\n"; EV::unloop };
   my $c = EV::timer 0.9, 0, sub { print "not ok 11\n"; EV::unloop };

   print "ok 10\n";
   EV::loop;
   print "ok 12\n";
}
