# $Id: /bast/Class-Accessor-Grouped/trunk/Makefile.PL 25394 2010-12-19T01:15:39.532979Z ribasushi  $
use strict;
use warnings;
use inc::Module::Install 1;

name 'Class-Accessor-Grouped';
license 'perl';
perl_version '5.006002';
all_from 'lib/Class/Accessor/Grouped.pm';

requires 'Carp';
requires 'Scalar::Util';
requires 'MRO::Compat';
requires 'Class::Inspector';

if (can_cc or $Module::Install::AUTHOR) {
  requires 'Sub::Name' => '0.05';

  requires 'Class::XSAccessor' => '1.11'
    if $] > '5.008';
}

test_requires 'Test::More' => '0.94';
test_requires 'Test::Exception' => '0.31';

clean_files "Class-Accessor-Grouped-* t/var";

if (-e 'MANIFEST.SKIP') {
    system('pod2text lib/Class/Accessor/Grouped.pm > README');
    realclean_files 'README';
}

auto_install;

resources repository =>
'http://dev.catalyst.perl.org/repos/bast/Class-Accessor-Grouped/trunk';

WriteAll;
use Data::Dumper;
if ($Module::Install::AUTHOR) {
  @{Meta->{values}{requires}} = grep
    { $_->[0] !~ /^ (?: Class\:\:XSAccessor | Sub\:\:Name ) $/x }
    @{Meta->{values}{requires}}
  ;
  print "Regenerating META with XS requires excluded\n";
  Meta->write;
}

