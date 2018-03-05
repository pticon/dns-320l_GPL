#!/usr/bin/perl


#use Getopt::Std;
 
#getopt('f:b:o:i:v:');

#  Make clean
system("make mrproper");
$BOARD="axp";
print "\n**** [Cleaning Make]\t*****\n\n";
#-------------------------------------------------------------
        $fail = chdir './tools/marvell';
         system("pwd"); 
         print " clean tools/marvell\n";
        $fail = system("make clean BOARD=$BOARD  ");
        if($fail){
        	print "\n *** Error: make clean\n\n";
        	exit 1;
        }
#-------------------------------------------------------------
	system("rm tools/marvell/doimage");
        system("pwd"); 

exit 0;
