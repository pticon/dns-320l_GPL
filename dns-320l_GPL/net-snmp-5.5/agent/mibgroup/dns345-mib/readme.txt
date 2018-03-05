
net-snmp

===================================
export MIBS=ALL
./configure --host=arm-none-linux-gnueabi --enable-shared --enable-static --with-endianness=little --enable-ipv6=no --enable-ucd-snmp-compatibility --with-default-snmp-version="2" --with-sys-contact="admin@net-snmp.org" --with-sys-location="Unknow" --with-logfile="none" --with-persistent-directory="/var/net-snmp"  LDFLAGS="-lpthread" --disable-embedded-perl --disable-as-needed --with-mib-modules="dns345-mib"



/home/roy/tmp9/net-snmp-5.5/_install
make prefix=/home/roy/cvs_home/Gandolf5/module/net-snmp-5.5/_install exec_prefix=/home/roy/cvs_home/Gandolf5/module/net-snmp-5.5/_install install


===================================
cp files

cp ./snmplib/.libs/libnetsnmp.so.20 /home/tmp/roy/345/snmp/libnetsnmp.so.20;
cp ./agent/.libs/snmpd /home/tmp/roy/345/snmp/snmpd;
cp ./agent/.libs/libnetsnmpagent.so.20 /home/tmp/roy/345/snmp/libnetsnmpagent.so.20;
cp ./agent/.libs/libnetsnmpmibs.so.20 /home/tmp/roy/345/snmp/libnetsnmpmibs.so.20;
cp agent/mibgroup/dns345-mib/DNS345-MIB.txt /home/tmp/roy/345/snmp/snmp/mibs
===================================

cp agent/mibgroup/dns345-mib/DNS345-MIB.txt mibs

===================================

make link

mkdir /usr/local/share
ln -s /mnt/tmp3/roy/345/snmp/snmp /usr/local/share/snmp
ln -s /mnt/tmp3/roy/345/snmp/libnetsnmpagent.so.20 /lib
ln -s /mnt/tmp3/roy/345/snmp/libnetsnmphelpers.so.20 /lib
ln -s /mnt/tmp3/roy/345/snmp/libnetsnmpmibs.so.20 /lib
ln -s /mnt/tmp3/roy/345/snmp/libnetsnmp.so.20 /lib

===================================

run & test

./snmpd -c /etc/snmp/snmpd.conf -Dmib_init

./snmpwalk -O s -c public localhost DNS345-MIB::nasAgent
./snmpget -c public localhost DNS345-MIB::Dns345VolumeEntry.0


./snmpget -c public localhost DNS345-MIB::dns345AgentVer.0
./snmpwalk -c public localhost DNS345-MIB::dns345VolumeTable
./snmpwalk -c public localhost DNS345-MIB::dns345DiskTable


snmpget -c public localhost ifInOctets.1 
mkdir /etc/snmp
echo 'rocommunity public' > /etc/snmp/snmpd.conf
echo 'view    systemview    included   .1.3.6.1.4.1.171.50.10.2.1' >> /etc/snmp/snmpd.conf

//for swtest 1
snmpwalk -v 2c -c public 2.66.69.43 DNS345-MIB::nasAgent
snmpwalk -v 2c -c public 2.66.69.43 DNS345-MIB::dns345VolumeTable
snmpwalk -v 2c -c public 2.66.69.43 DNS345-MIB::dns345DiskTable
snmpget -v 2c  -c public 2.66.69.43 DNS345-MIB::dns345DiskEntry.dns345DiskCapacity

===================================




.1.3.6.1.4.1.9 -->.iso.org.dod.internet.private.enterprises

./snmpget -c public localhost ifDescr.1 -v 1
./snmpget -c public localhost hrSystemUptime.0 -v 1


===================================
reload mib
kill -HUP `pidof snmpd`
===================================


./snmpwalk -O s -v 1 -c public localhost 1.3.6.1.2.1
sysDescr.0 = STRING: Linux DNS-323D1 2.6.22.18 #110 Wed Feb 24 11:35:05 CST 2010 armv5tejl
sysObjectID.0 = OID: org
sysUpTimeInstance = Timeticks: (1295) 0:00:12.95
sysContact.0 = STRING: admin@net-snmp.org
sysName.0 = STRING: DNS-323D1
sysLocation.0 = STRING: arm-none-linux
sysORLastChange.0 = Timeticks: (9) 0:00:00.09
.
.
.
.

# 