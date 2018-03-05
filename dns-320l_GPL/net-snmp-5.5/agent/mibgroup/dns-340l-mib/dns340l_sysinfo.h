#ifndef DNS340L_SYSINFO_H
#define DNS340L_SYSINFO_H

#define NAS_AGENT_VERSION		"1.00b01"

typedef struct _DNS340LNASAGENT_
{
	char	sw_version[64];
	char	host_name[64];
	char	dhcp_enable[16];
	char	ftp_enable[16];
	char	net_type[32];			//0:Workgroup	1:Active Directory
	char	temperature[64];
	char	fan_status[64];
}dns340lnasAgent, *ID_dns340lnasAgent;
#define DNS340LNASAGENT_SIZE	(sizeof(struct _DNS340LNASAGENT_))

void init_dns340l_sysinfo(void);
Netsnmp_Node_Handler handle_dns340lAgentVer;
Netsnmp_Node_Handler handle_dns340lSoftwareVersion;
Netsnmp_Node_Handler handle_dns340lHostName;
Netsnmp_Node_Handler handle_dns340lDHCPServer;
Netsnmp_Node_Handler handle_dns340lFTPServer;
Netsnmp_Node_Handler handle_dns340lNetType;
Netsnmp_Node_Handler handle_dns30l5Temperature;
Netsnmp_Node_Handler handle_dns340lFanStatus;

#endif
