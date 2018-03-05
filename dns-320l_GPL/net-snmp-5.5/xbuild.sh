
#####OID will also be defined in platform.h#####

fl_check_openssl=0

case $PROJECT_NAME in

	DNS-345)
	MIB_MODULES="dns345-mib"
	ENTERPRISE_OID="171"
	ENTERPRISE_SYSOID="1.3.6.1.4.1.171.50.1.5.1"
	ENTERPRISE_NOTIFICATION_OID="1.3.6.1.4.1.171.50.1.5.1.1.200"
	MIB_NAME="NAS-MIB.txt"
	SYS_CONTACT="customerservice@dlink.com"
	;;

	LIGHTNING-4A)
	MIB_MODULES="wdmycloudex4-mib"
	ENTERPRISE_OID="5127"
	ENTERPRISE_SYSOID="1.3.6.1.4.1.5127.1.1.1.1"
	ENTERPRISE_NOTIFICATION_OID="1.3.6.1.4.1.5127.1.1.1.1.1.200"
	MIB_NAME="WDMYCLOUDEX4-MIB.txt"
	SYS_CONTACT="support@wdc.com"
	;;

	DNS-340L)
	MIB_MODULES="dns-340l-mib"
	ENTERPRISE_OID="171"
	ENTERPRISE_SYSOID="1.3.6.1.4.1.171.50.1.10.1"
	ENTERPRISE_NOTIFICATION_OID="1.3.6.1.4.1.171.50.1.10.1.1.200"
	MIB_NAME="DNS-340L-MIB.txt"
	SYS_CONTACT="customerservice@dlink.com"
	fl_check_openssl=1
	;;

	KingsCanyon)
	MIB_MODULES="wdmycloudex2-mib"
	ENTERPRISE_OID="5127"
	ENTERPRISE_SYSOID="1.3.6.1.4.1.5127.1.1.1.2"
	ENTERPRISE_NOTIFICATION_OID="1.3.6.1.4.1.5127.1.1.1.2.1.200"
	MIB_NAME="WDMYCLOUDEX2-MIB.txt"
	SYS_CONTACT="support@wdc.com"
	;;


    *)
	echo $PROJECT_NAME dont supprot SNMP
	exit 0
	;;

esac

xcheck_support()
{
	snmp_support=`echo $CFLAGS | grep "DPROJECT_FEATURE_SNMP=1" > /dev/null 2>&1;echo $?`
	if [ "$snmp_support" != "0" ]; then
	   echo $PROJECT_NAME dont supprot SNMP
	   exit 0
	fi
}

xcheck_openssl()
{
	# We need openssl
	if [ ! -e $ROOT_FS/bin/openssl ]; then
		cat <<-EOF

		ERROR: "$ROOT_FS/bin/openssl" does not exist!
		Please build it first.

		$ cd ../openssl-1.0.1c
		$ ./xbuild build
		$ ./xbuild install

		EOF

		exit 0
	fi
}

source ../xcp.sh

xbuild()
{

[ $fl_check_openssl -eq 1 ] && xcheck_openssl


xcheck_support

./configure --host=${CC%-*} --enable-shared --enable-static --with-endianness=little --enable-ipv6=no --enable-ucd-snmp-compatibility --with-default-snmp-version="2" --with-sys-contact=${SYS_CONTACT} --with-sys-location="Unknow" --with-logfile="none" --with-persistent-directory="/var/net-snmp"  LDFLAGS="-lpthread -L${XLIB_DIR}/ -lxmldbc -lxml2 -lshare -lhdVerify -lsnmpgetinfo -ltemperature" --disable-embedded-perl --disable-as-needed --with-mib-modules=${MIB_MODULES} --with-enterprise-oid=${ENTERPRISE_OID} --with-enterprise-sysoid=${ENTERPRISE_SYSOID} --with-enterprise-notification-oid=${ENTERPRISE_NOTIFICATION_OID}  --without-perl-modules LIBS="-L${XLIB_DIR}/" CPPFLAGS="-I${XINC_DIR}/" --with-openssl="$ROOT_FS/bin/openssl"
make

}

xinstall()
{
xcheck_support

	${CC%-*}-strip agent/.libs/snmpd
	${CC%-*}-strip agent/.libs/libnetsnmpagent.so.20
	${CC%-*}-strip agent/.libs/libnetsnmpmibs.so.20
	${CC%-*}-strip snmplib/.libs/libnetsnmp.so.20
	${CC%-*}-strip agent/helpers/.libs/libnetsnmphelpers.so.20
	${CC%-*}-strip apps/.libs/snmpwalk
	${CC%-*}-strip apps/.libs/snmpget
	${CC%-*}-strip apps/.libs/snmptrap
	${CC%-*}-strip apps/.libs/snmptranslate


	case $1 in
		tmp)
			TMP_FS="/home/tmp/${USER}/${PROJECT_NAME}/snmpwd"
			mkdir -p ${TMP_FS}
			cp agent/.libs/snmpd  apps/.libs/snmpwalk apps/.libs/snmpget  apps/.libs/snmptrap apps/.libs/snmptranslate ${TMP_FS}
			cp agent/.libs/libnetsnmpagent.so.20 agent/.libs/libnetsnmpmibs.so.20 snmplib/.libs/libnetsnmp.so.20 agent/helpers/.libs/libnetsnmphelpers.so.20 ${TMP_FS}
			cp mibs/${MIB_NAME} ${TMP_FS}
		;;

		*)
			cp agent/.libs/snmpd  apps/.libs/snmpwalk apps/.libs/snmpget  apps/.libs/snmptrap apps/.libs/snmptranslate ${ROOT_FS}/bin
			cp agent/.libs/libnetsnmpagent.so.20 agent/.libs/libnetsnmpmibs.so.20 snmplib/.libs/libnetsnmp.so.20 agent/helpers/.libs/libnetsnmphelpers.so.20 ${ROOT_FS}/lib
			[ -e ${ROOT_FS}/files/snmp/mibs ] || { mkdir -p ${ROOT_FS}/files/snmp/mibs ;  }
			cp agent/mibgroup/${MIB_MODULES}/${MIB_NAME} mibs/
			cp mibs/${MIB_NAME} ${ROOT_FS}/files/snmp/mibs
		;;
	esac
}

xclean()
{
	make distclean
}


COMMAND=$1

case $COMMAND in

	build)
		xbuild
	;;

	install)
		xinstall
	;;

	installtmp)
		xinstall tmp
	;;

	clean)
		xclean
	;;

    *)
		echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
	;;

esac


#
#if [ "$1" = "build" ]; then
#   xbuild
#elif [ "$1" = "install" ]; then
#   xinstall
#elif [ "$1" = "clean" ]; then
#   xclean
#else
#   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
#fi
