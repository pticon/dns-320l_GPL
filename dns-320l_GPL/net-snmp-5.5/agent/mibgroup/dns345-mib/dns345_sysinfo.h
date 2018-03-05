#ifndef DNS345_SYSINFO_H
#define DNS345_SYSINFO_H

#define NAS_AGENT_VERSION		"1.00b01"

typedef struct _DNS345NASAGENT_
{
	char	sw_version[64];
	char	host_name[64];
	char	dhcp_enable[8];
	char	ftp_enable[8];
	char	net_type[16];			//0:Workgroup	1:Active Directory
	char	temperature[64];
	char	fan_status[64];
}dns345nasAgent, *ID_dns345nasAgent;
#define DNS345NASAGENT_SIZE	(sizeof(struct _DNS345NASAGENT_))

void init_dns345_sysinfo(void);
Netsnmp_Node_Handler handle_dns345AgentVer;
Netsnmp_Node_Handler handle_dns345SoftwareVersion;
Netsnmp_Node_Handler handle_dns345HostName;
Netsnmp_Node_Handler handle_dns345DHCPServer;
Netsnmp_Node_Handler handle_dns345FTPServer;
Netsnmp_Node_Handler handle_dns345NetType;
Netsnmp_Node_Handler handle_dns345Temperature;
Netsnmp_Node_Handler handle_dns345FanStatus;

#endif
