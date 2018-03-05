#!/usr/bin/perl
# $File: //member/autrijus/Locale-Hebrew/Makefile.PL $ $Author: autrijus $
# $Revision: #4 $ $Change: 11167 $ $DateTime: 2004/09/17 21:19:15 $

use Config;
use ExtUtils::MakeMaker;

WriteMakefile(
    'NAME'	    => 'Locale::Hebrew',
    'VERSION_FROM'  => 'Hebrew.pm',
    'AUTHOR'        => 'Autrijus Tang <autrijus@autrijus.org>',
    'ABSTRACT'      => 'Bidirectional Hebrew support',
    'OBJECT'	    => join(' ', map "$_$Config{obj_ext}", qw(Hebrew bidi)),
    'SIGN'          => (ExtUtils::MakeMaker->VERSION >= 6.21),
);

1;
