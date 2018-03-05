
use strict;
use ExtUtils::MakeMaker;

my $name = 'Tie::LLHash';
my $file = ($name =~ /::(\w+)/)[0] . '.pm';

WriteMakefile(
	      'NAME'	     => $name,
	      'VERSION_FROM' => $file, # finds $VERSION
	      'PL_FILES' => {},
	      'dist' => { COMPRESS=>'gzip',
			  SUFFIX=>'gz', 
			},
	     );
