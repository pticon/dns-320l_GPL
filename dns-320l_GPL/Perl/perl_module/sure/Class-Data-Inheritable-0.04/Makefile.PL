use ExtUtils::MakeMaker;

WriteMakefile(
	NAME         => 'Class::Data::Inheritable',
	VERSION_FROM => "lib/Class/Data/Inheritable.pm",
	PREREQ_PM    => {},
	dist         => {
		COMPRESS     => 'gzip -9',
		SUFFIX       => '.gz',
		DIST_DEFAULT => 'all tardist',
	},
);
