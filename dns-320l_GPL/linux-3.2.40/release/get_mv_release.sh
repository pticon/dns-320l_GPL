#!/usr/bin/perl

use File::Basename;

sub usage(){
 print "Usage: $0 <marvell-kernel DIR> <original linux kernel DIR>\n"
}

if ($#ARGV != 1)
{
    usage();
    exit(1);
    if (! -d $ARGV[1]) 
    {
        print "$ARGV1 directory doesn't exist";
        exit (1);
    }
    elsif ( ! -d $ARGV[2] )
    {
        print "$ARGV[2] directory doesn't exist";
        exit (1);
    }
    
}
$marvell_dir=$ARGV[0];
$linux_dir=$ARGV[1];

#print "marvell dir: $marvell_dir linux dir: $linux_dir\n";

system("diff -r --brief $marvell_dir $linux_dir > diff.txt");
$release_dir=${marvell_dir}."_release";
#print "Create marvell release dir:$release_dir\n";

open(INFO, "diff.txt");		# Open the file


foreach $line (<INFO>){
#    print " line is : $line";
    chomp($line);
    @words=split(/ /, $line);
    #print "$words[0], $words[1], $words[2], $words[3],\n";
    if ( $words[0] eq "Only" )
    {
        #print "$words[3] only in $words[2]\n";
        $dir=$words[2];
        if ( /$marvell_dir/ )
        {
            #print " file/dir $words[3] isn't in $marvell_dir !!!!\n";
            next;
        }
        else
        {
            @dirname_ = split(/:/,$dir); #get red of ":"
            #print "dirname  after: @dirname_[0]\n";
            $dirname=$dirname_[0];
            $basename=$words[3];
        }
    }
    elsif ( $words[0] eq "Files" )
    {
        #print "$words[1] $words[3] differs\n";
        $dirname=dirname($words[1]);
        $basename=basename($words[1]);
    }
    else
    {
        print "Bad input from diff: word1:$words[0]";
        exit;
    }
#    print "dirname $dirname basename $basename\n";
    $path="${release_dir}/${dirname}";
    system("mkdir -p $path");
#    print "cp -a $dirname/$basename $path/\n";
    system("cp -a $dirname/$basename $path/");    
}
close(INFO);
system("rm -fr diff.txt");
#print "Finished successfully\n";
 
