#ifndef WDMYCLOUDEX4_SYSINFO_H
#define WDMYCLOUDEX4_SYSINFO_H

#define NAS_AGENT_VERSION		"1.00"

typedef struct _WDMYCLOUDEX4NASAGENT_
{
	char	sw_version[64];
	char	host_name[64];
	char	dhcp_enable[8];
	char	ftp_enable[8];
	char	net_type[16];			//0:Workgroup	1:Active Directory
	char	temperature[64];
	char	fan_status[64];
}wdmycloudex4nasAgent, *ID_wdmycloudex4nasAgent;
#define WDMYCLOUDEX4NASAGENT_SIZE	(sizeof(struct _WDMYCLOUDEX4NASAGENT_))

void init_wdmycloudex4_sysinfo(void);
Netsnmp_Node_Handler handle_wdmycloudex4AgentVer;
Netsnmp_Node_Handler handle_wdmycloudex4SoftwareVersion;
Netsnmp_Node_Handler handle_wdmycloudex4HostName;
Netsnmp_Node_Handler handle_wdmycloudex4DHCPServer;
Netsnmp_Node_Handler handle_wdmycloudex4FTPServer;
Netsnmp_Node_Handler handle_wdmycloudex4NetType;
Netsnmp_Node_Handler handle_wdmycloudex4Temperature;
Netsnmp_Node_Handler handle_wdmycloudex4FanStatus;

#endif
