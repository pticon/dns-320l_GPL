$| = 1;

if (-f "/etc/passwd" and -d "/etc") {
   print "1..10\n";
} else {
   print "1..0 # Skipped: unexpected /etc and/or /etc/passwd\n";
   exit;
}

# relies on /etc/passwd to exist...

use Fcntl;
use IO::AIO;

IO::AIO::min_parallel 2;

sub pcb {
   while (IO::AIO::nreqs) {
      my $rfd = ""; vec ($rfd, IO::AIO::poll_fileno, 1) = 1; select $rfd, undef, undef, undef;
      IO::AIO::poll_cb;
   }
}

aio_open "/etc/passwd", O_RDONLY, 0, sub {
   print $_[0] ? "ok" : "not ok", " 1\n";

   $pwd = $_[0];
};

pcb;

aio_stat "/etc", sub {
   print -d _ ? "ok" : "not ok", " 2\n";
};

pcb;

aio_stat "/etc/passwd", sub {
   @pwd = stat _;
   print -f _ ? "ok" : "not ok", " 3\n";
   print eval  { lstat _; 1 }  ? "not ok" : "ok", " 4\n";
};

pcb;

aio_lstat "/etc/passwd", sub {
   lstat _;
   print -f _ ? "ok" : "not ok", " 5\n";
   print eval  { stat _; 1 }  ? "ok" : "not ok", " 6\n";
};

pcb;

print open (PWD, "<&" . fileno $pwd) ? "ok" : "not ok", " 7\n";

aio_stat *PWD, sub {
   print -f _ ? "ok" : "not ok", " 8\n";
   print +(join ":", @pwd) eq (join ":", stat _) ? "ok" : "not ok", " 9\n";
};

pcb;

aio_close *PWD, sub {
   print $_[0] ? "not ok" : "ok", " 10 # <@_>\n";
};

pcb;

