#!/usr/bin/perl

use Test;
use IO::AIO;

# this is a lame test, but....

BEGIN { plan tests => 9 }

IO::AIO::min_parallel 2;

aio_nop sub {
   print "ok 6\n";
};

print "ok 1\n";

if (open FH, "-|") {
   print while <FH>;
   aio_stat "/", sub {
      print "ok 7\n";
   };
   print "ok 5\n";
   IO::AIO::poll while IO::AIO::nreqs;
   print "ok 8\n";
} else {
   print "ok 2\n";
   aio_stat "/", sub {
      print "ok 3\n";
   };
   IO::AIO::poll while IO::AIO::nreqs;
   print "ok 4\n";
   exit;
}

print "ok 9\n";

