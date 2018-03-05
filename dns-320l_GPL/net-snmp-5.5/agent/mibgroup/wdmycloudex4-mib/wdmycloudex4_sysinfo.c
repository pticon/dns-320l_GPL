
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
#include "wdmycloudex4_sysinfo.h"
#include "platform.h" //for snmp oid



ID_wdmycloudex4nasAgent		p_nasAgent;

/*-----------------------------------------------------------------
* ROUTINE NAME - init_wdmycloudex4_sysinfo
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
void init_wdmycloudex4_sysinfo(void)
{
	static oid wdmycloudex4AgentVer_oid[]			   = { NAS_COMMA_OID, 1, 1 };
	static oid wdmycloudex4SoftwareVersion_oid[] 	= { NAS_COMMA_OID, 1, 2 };
	static oid wdmycloudex4HostName_oid[]			   = { NAS_COMMA_OID, 1, 3 };
	static oid wdmycloudex4DHCPServer_oid[]		   = { NAS_COMMA_OID, 1, 4 };
	static oid wdmycloudex4FTPServer_oid[]		      = { NAS_COMMA_OID, 1, 5 };
	static oid wdmycloudex4NetType_oid[]			   = { NAS_COMMA_OID, 1, 6 };
	static oid wdmycloudex4Temperature_oid[]		   = { NAS_COMMA_OID, 1, 7 };
	static oid wdmycloudex4FanStatus_oid[]		      = { NAS_COMMA_OID, 1, 8 };

	//malloc structure memory
	p_nasAgent = malloc(WDMYCLOUDEX4NASAGENT_SIZE);
	memset(p_nasAgent, 0, WDMYCLOUDEX4NASAGENT_SIZE);
	DEBUGMSGTL(("wdmycloudex4_sysinfo", "Initializing\n"));

	//wdmycloudex4AgentVer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4AgentVer", handle_wdmycloudex4AgentVer,
							wdmycloudex4AgentVer_oid,
							OID_LENGTH(wdmycloudex4AgentVer_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4SoftwareVersion
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4SoftwareVersion", handle_wdmycloudex4SoftwareVersion,
							wdmycloudex4SoftwareVersion_oid,
							OID_LENGTH(wdmycloudex4SoftwareVersion_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4HostName
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4HostName", handle_wdmycloudex4HostName,
							wdmycloudex4HostName_oid,
							OID_LENGTH(wdmycloudex4HostName_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4DHCPServer
//	netsnmp_register_scalar(netsnmp_create_handler_registration
//							("wdmycloudex4DHCPServer", handle_wdmycloudex4DHCPServer,
//							wdmycloudex4DHCPServer_oid,
//							OID_LENGTH(wdmycloudex4DHCPServer_oid),
//							HANDLER_CAN_RONLY));

	//wdmycloudex4FTPServer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4FTPServer", handle_wdmycloudex4FTPServer,
							wdmycloudex4FTPServer_oid,
							OID_LENGTH(wdmycloudex4FTPServer_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4NetType
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4NetType", handle_wdmycloudex4NetType,
							wdmycloudex4NetType_oid,
							OID_LENGTH(wdmycloudex4NetType_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4Temperature
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4Temperature", handle_wdmycloudex4Temperature,
							wdmycloudex4Temperature_oid,
							OID_LENGTH(wdmycloudex4Temperature_oid),
							HANDLER_CAN_RONLY));

	//wdmycloudex4FanStatus
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("wdmycloudex4FanStatus", handle_wdmycloudex4FanStatus,
							wdmycloudex4FanStatus_oid,
							OID_LENGTH(wdmycloudex4FanStatus_oid),
							HANDLER_CAN_RONLY));

}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4AgentVer
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4AgentVer(netsnmp_mib_handler *handler,
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
		snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4AgentVer\n", reqinfo->mode);
		return SNMP_ERR_GENERR;

	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4SoftwareVersion
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4SoftwareVersion(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4SoftwareVersion\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4HostName
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4HostName(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4HostName\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4DHCPServer
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
//int handle_wdmycloudex4DHCPServer(netsnmp_mib_handler *handler,
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
//			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4DHCPServer\n", reqinfo->mode);
//			return SNMP_ERR_GENERR;
//	}
//	return SNMP_ERR_NOERROR;
//}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4FTPServer
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4FTPServer(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4FTPServer\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4NetType
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4NetType(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4NetType\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4Temperature
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4Temperature(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4Temperature\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_wdmycloudex4FanStatus
*------------------------------------------------------------------
* FUNCTION:
*
* INPUT:
* OUTPUT:
* RETURN:
*
* NOTE:
*----------------------------------------------------------------*/
int handle_wdmycloudex4FanStatus(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_wdmycloudex4FanStatus\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}





