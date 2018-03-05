1. Introduction

   This file will show you how to build the working f/w.

   The workstation we used to build the image is:
     OS Version: Ubuntu 14.04.5 (32-bit version)
     OS Kernel Version: Linux ubuntu 4.4.0-31-generic
     GCC Version: 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.3)
     (You may use other Linux distribution as your workstation 
     but no guarantee of a successful build.)

2. Procedure: (~# means command)

Step 1. Prepare a working environment

  1) Change the ownership from an ordinary user to the root user
    ~# su
    Password: ******** (Enter the root's password)

  2) Create a temporary directory:
    ~# cd /tmp
    ~# mkdir works
    ~# chmod a+rw /tmp/works

  3) Copy the GPL tar ball into this directory -- /tmp/works

  4) untar the archive file
    ~# cd /tmp/works
    ~# tar zxf DNS320L_Ax_GPL10906.tar.gz

Step 2. Building a working f/w

    ~# cd /tmp/works/DNS-320L_GPL/dns-320l_GPL
    ~# source build_fw
    Note : a. make sure uRamdisk, uImage, image.cfs,  and default.tar.gz are resided within the merge folder, the default name of generated F/W is : DNS-320L-firmware.
           b. upload the DNS-320L-firmware through device GUI

3. File list
	acl-2.2.47
	ADS
	afp_module
	aMule-2.2.6
	arm-mv5sft-linux-gnueabi
	attr-2.4.46
	audiostreamer
	avahi-0.6.30
	boost_1_44_0
	boost_1_55_0
	boost-jam-3.1.18
	busybox-1.20.2
	bzip2-1.0.6
	cdrkit-1.1.11
	cgic205
	chardetect
	coreutils-8.9
	cryptopp552
	cryptsetup-1.5.1
	curl-7.19.7
	db-4.7.25
	dbus-1.4.20
	DDNS_tools
	e2fsprogs-1.42.5
	e2fsprogs-libs-1.41.14
	exif-0.6.20
	expat-2.0.1
	flac-1.3.0
	fontconfig-2.8.0
	freetype-2.3.9
	fuse-2.8.4
	gd-2.0.35
	gdisk-0.6.13
	grive-0.3.0-pre
	gzip124
	hdparm-9.36
	i2c-tools-3.0.2
	imgconvert
	inotify-tools-3.13
	iproute2-3.4.0
	iTunes_server
	jhead-2.93
	jpeg-7
	json-c-0.11
	libcap-2.20
	libdaemon-0.14
	libedit-20121213-3.0
	libevent-2.0.19-stable
	libexif-0.6.20
	libgcrypt-1.5.0-beta1
	libgpg-error-1.10
	libiberty
	libiconv-1.9.2
	libid3tag-0.15.1b
	libmcrypt-2.5.8
	libpng-1.2.39
	libs3-2.0
	libtool-1.5.6
	libtorrent-rasterbar-0.15.4
	libusb-0.1.12
	libxml2-2.7.4
	lighttpd-1.4.28
	linux-2.6.31.8
	linux-3.2.40
	Linux-PAM-0.99.7.1
	LLTD-PortingKit
	logrotate-3.7.9
	LPRng-3.8.10
	mcrypt-2.6.8
	mdadm-2.5.6
	merge
	mhash-0.9.9.9
	module-init-tools-3.3-pre1
	msmtp-1.4.15
	mtd-utils-20080508
	mtp
	mutt-1.5.18
	mysql-5.1.56
	ncurses-5.5
	net-snmp-5.5
	net-tools-1.60_p20110409135728
	nfs
	ntfs-3g-2010.3.6
	ntp-4.2.2p4
	nut-2.6.4
	openssh-5.0p1
	openssl-1.0.2h
	pcre-7.6
	Perl
	php-5.2.17
	popt-1.16
	psmisc-22.19
	pure-ftpd-1.0.29
	quota-tools-3.17
	ramdisk
	readline-6.2
	rsync-3.0.7
	rsyslog-5.8.6
	samba-3.5.15_noads
	sg3_utils-1.28
	shadow-4.0.18.2
	smartmontools-5.38
	sqlite-3.6.14.1
	sysklogd-1.5
	taglib-1.7.2
	termcap-1.3.1
	tinylogin-1.4
	transmission-2.73
	u-boot-1.1.4
	u-boot-2011.12
	unzip60
	util-linux-2.22
	util-linux-ng-2.14.2
	wget-1.12
	wide-dhcpv6
	wordpress-3.5.2
	wxWidgets-2.8.9
	yajl-2.0.1-0
	zip30
	zlib-1.2.3
