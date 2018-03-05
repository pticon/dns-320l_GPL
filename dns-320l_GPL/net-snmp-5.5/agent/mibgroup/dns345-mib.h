#ifndef DNS345_MIB_H
#define DNS345_MIB_H

//config_require(dns345-mib/getinfo)
config_require(dns345-mib/dns345_sysinfo)
config_require(dns345-mib/dns345_hdinfo)
config_require(dns345-mib/dns345_raidinfo)
config_require(dns345-mib/dns345_upsinfo)
config_add_mib(NAS-MIB)

#endif
