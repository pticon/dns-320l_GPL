
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#include <xml_tag.h>
#include <getinfo.h>
#include "dns345_sysinfo.h"


ID_dns345nasAgent		p_nasAgent;

/*-----------------------------------------------------------------
* ROUTINE NAME - init_dns345_sysinfo         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void init_dns345_sysinfo(void)
{
	static oid dns345AgentVer_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 1 };
	static oid dns345SoftwareVersion_oid[] 	= { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 2 };
	static oid dns345HostName_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 3 };
	static oid dns345DHCPServer_oid[]		   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 4 };
	static oid dns345FTPServer_oid[]		      = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 5 };
	static oid dns345NetType_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 6 };
	static oid dns345Temperature_oid[]		   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 7 };
	static oid dns345FanStatus_oid[]		      = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 5, 1, 1, 8 };

	//malloc structure memory
	p_nasAgent = malloc(DNS345NASAGENT_SIZE);
	memset(p_nasAgent, 0, DNS345NASAGENT_SIZE);
	DEBUGMSGTL(("dns345_sysinfo", "Initializing\n"));
	
	//dns345AgentVer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345AgentVer", handle_dns345AgentVer,
							dns345AgentVer_oid,
							OID_LENGTH(dns345AgentVer_oid),
							HANDLER_CAN_RONLY));
     
	//dns345SoftwareVersion
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345SoftwareVersion", handle_dns345SoftwareVersion,
							dns345SoftwareVersion_oid,
							OID_LENGTH(dns345SoftwareVersion_oid),
							HANDLER_CAN_RONLY));

	//dns345HostName 
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345HostName", handle_dns345HostName,
							dns345HostName_oid,
							OID_LENGTH(dns345HostName_oid),
							HANDLER_CAN_RONLY));
	
	//dns345DHCPServer
//	netsnmp_register_scalar(netsnmp_create_handler_registration
//							("dns345DHCPServer", handle_dns345DHCPServer,
//							dns345DHCPServer_oid,
//							OID_LENGTH(dns345DHCPServer_oid),
//							HANDLER_CAN_RONLY));
	
	//dns345FTPServer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345FTPServer", handle_dns345FTPServer,
							dns345FTPServer_oid,
							OID_LENGTH(dns345FTPServer_oid),
							HANDLER_CAN_RONLY));
	
	//dns345NetType
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345NetType", handle_dns345NetType,
							dns345NetType_oid,
							OID_LENGTH(dns345NetType_oid),
							HANDLER_CAN_RONLY));
							
	//dns345Temperature
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345Temperature", handle_dns345Temperature,
							dns345Temperature_oid,
							OID_LENGTH(dns345Temperature_oid),
							HANDLER_CAN_RONLY));
							
	//dns345FanStatus
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns345FanStatus", handle_dns345FanStatus,
							dns345FanStatus_oid,
							OID_LENGTH(dns345FanStatus_oid),
							HANDLER_CAN_RONLY));
	
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345AgentVer         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345AgentVer(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
    //We are never called for a GETNEXT if it's registered as a
    //"instance", as it's "magically" handled for us.  
    
    //a instance handler also only hands us one request at a time, so
    //we don't need to loop over a list of requests; we'll only get one. 
     
	switch(reqinfo->mode)
	{
		case MODE_GET:
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)NAS_AGENT_VERSION,
									strlen(NAS_AGENT_VERSION));
			break;
		
		default:
		//we should never get here, so this is a really bad error 
		snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345AgentVer\n", reqinfo->mode);
		return SNMP_ERR_GENERR;
	         
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345SoftwareVersion         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345SoftwareVersion(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_sw_version(p_nasAgent->sw_version) == 0)
				return SNMP_ERR_GENERR;
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->sw_version,
									strlen(p_nasAgent->sw_version));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345SoftwareVersion\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345HostName         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345HostName(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_hostname(p_nasAgent->host_name) == 0)
				return SNMP_ERR_GENERR;
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->host_name,
									strlen(p_nasAgent->host_name));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345HostName\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345DHCPServer         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
//int handle_dns345DHCPServer(netsnmp_mib_handler *handler,
//							netsnmp_handler_registration *reginfo,
//							netsnmp_agent_request_info *reqinfo,
//							netsnmp_request_info *requests)
//{
//	switch(reqinfo->mode)
//	{
//		case MODE_GET:
//			if(get_config_xml(XML_DHCPD_ENABLE, p_nasAgent->dhcp_enable,sizeof(p_nasAgent->dhcp_enable)) == 0)
//				return SNMP_ERR_GENERR;
//			
//			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
//									(u_char *)p_nasAgent->dhcp_enable,
//									strlen(p_nasAgent->dhcp_enable));
//			break;
//		
//		default:
//			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345DHCPServer\n", reqinfo->mode);
//			return SNMP_ERR_GENERR;
//	}
//	return SNMP_ERR_NOERROR;
//}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345FTPServer         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345FTPServer(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_config_xml(XML_FTP_STATE, p_nasAgent->ftp_enable,sizeof(p_nasAgent->ftp_enable)) == 0)
				return SNMP_ERR_GENERR;
				
//			if(StringInclude(p_nasAgent->ftp_enable, "stop"))
//				strcpy(p_nasAgent->ftp_enable, "0");
//			else
//				strcpy(p_nasAgent->ftp_enable, "1");
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->ftp_enable,
									strlen(p_nasAgent->ftp_enable));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345FTPServer\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345NetType         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345NetType(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_config_xml(XML_SAB_ADS_ENABLE, p_nasAgent->net_type,sizeof(p_nasAgent->net_type)) == 0)
				return SNMP_ERR_GENERR;
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->net_type,
									strlen(p_nasAgent->net_type));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345NetType\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345Temperature         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345Temperature(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_temperature(p_nasAgent->temperature) == 0)
				return SNMP_ERR_GENERR;
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->temperature,
									strlen(p_nasAgent->temperature));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345Temperature\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns345FanStatus         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns345FanStatus(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_fan_status(p_nasAgent->fan_status) == 0)
				return SNMP_ERR_GENERR;
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->fan_status,
									strlen(p_nasAgent->fan_status));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns345FanStatus\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}





