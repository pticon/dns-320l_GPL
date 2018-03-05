#!/bin/sh

case $1 in

static)
	make clean
	make distclean
	
	./configure --host=arm-linux --prefix=${PWD}/xinst --disable-testio-debug --disable-tls
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "Configure failed.Recheck Please ."
		echo ""
		exit 1
	fi
	
	make clean
	make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "Make failed.Recheck Please ."
		echo ""
		exit 1
	fi
	
	if [ -e ${PWD}/xinst ]; then
		rm -rf ${PWD}/xinst
	fi
	make install
	
	$STRIP ${PWD}/xinst/bin/*
	$STRIP ${PWD}/xinst/sbin/*
	$STRIP ${PWD}/xinst/lib/*
	
	echo -e ""
	echo -e "***************************"
	echo -e "*  Building Static Done.  *"
	echo -e "***************************"
	echo -e ""
	echo -e "** Installed Files are UNDER ${PWD}/xinst **"	
	echo -e ""
	;;

dynamic)
	make clean
	make distclean

	./configure --host=arm-linux --prefix=${PWD}/xinst --disable-testio-debug --disable-tls --enable-elf-shlibs CFLAGS="$CFLAGS -O2"
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "Configure failed.Recheck Please ."
		echo ""
		exit 1
	fi
	
	make clean
	make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "Make failed.Recheck Please ."
		echo ""
		exit 1
	fi

	if [ -e ${PWD}/xinst ]; then
		rm -rf ${PWD}/xinst
	fi
	make install
		
	$STRIP ${PWD}/xinst/bin/*
	$STRIP ${PWD}/xinst/sbin/*
	$STRIP ${PWD}/xinst/lib/*

	echo -e ""
	echo -e "****************************"
	echo -e "*  Building Dynamic Done.  *"
	echo -e "****************************"
	echo -e ""
	echo -e "** Installed Files are UNDER ${PWD}/xinst **"	
	echo -e ""

	;;

*)
	echo -e "Usage   :"
	echo -e "build.sh [static/dynamic]"
	echo -e "static  : build static binarys ."
	echo -e "dynamic : build dynamic binarys/libs ."
	;;

esac

exit 0
