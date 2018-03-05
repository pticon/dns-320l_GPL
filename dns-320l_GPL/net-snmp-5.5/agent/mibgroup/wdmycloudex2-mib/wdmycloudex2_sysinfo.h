#ifndef WDMYCLOUDEX2_SYSINFO_H
#define WDMYCLOUDEX2_SYSINFO_H

#define NAS_AGENT_VERSION		"1.00"

typedef struct _WDMYCLOUDEX2NASAGENT_
{
	char	sw_version[64];
	char	host_name[64];
	char	dhcp_enable[8];
	char	ftp_enable[8];
	char	net_type[16];			//0:Workgroup	1:Active Directory
	char	temperature[64];
	char	fan_status[64];
}wdmycloudex2nasAgent, *ID_wdmycloudex2nasAgent;
#define WDMYCLOUDEX2NASAGENT_SIZE	(sizeof(struct _WDMYCLOUDEX2NASAGENT_))

void init_wdmycloudex2_sysinfo(void);
Netsnmp_Node_Handler handle_wdmycloudex2AgentVer;
Netsnmp_Node_Handler handle_wdmycloudex2SoftwareVersion;
Netsnmp_Node_Handler handle_wdmycloudex2HostName;
Netsnmp_Node_Handler handle_wdmycloudex2DHCPServer;
Netsnmp_Node_Handler handle_wdmycloudex2FTPServer;
Netsnmp_Node_Handler handle_wdmycloudex2NetType;
Netsnmp_Node_Handler handle_wdmycloudex2Temperature;
Netsnmp_Node_Handler handle_wdmycloudex2FanStatus;

#endif
