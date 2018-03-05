#ifndef DNS340L_MIB_H
#define DNS340L_MIB_H


config_require(dns-340l-mib/dns340l_sysinfo)
config_require(dns-340l-mib/dns340l_hdinfo)
config_require(dns-340l-mib/dns340l_raidinfo)
config_require(dns-340l-mib/dns340l_upsinfo)
config_add_mib(DNS-340L-MIB)

#endif
