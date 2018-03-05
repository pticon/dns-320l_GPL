
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
#include "dns340l_sysinfo.h"


ID_dns340lnasAgent		p_nasAgent;

/*-----------------------------------------------------------------
* ROUTINE NAME - init_dns340l_sysinfo         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void init_dns340l_sysinfo(void)
{
	static oid dns340lAgentVer_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 1 };
	static oid dns340lSoftwareVersion_oid[] 	= { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 2 };
	static oid dns340lHostName_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 3 };
	static oid dns340lFTPServer_oid[]		      = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 5 };
	static oid dns340lNetType_oid[]			   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 6 };
	static oid dns340lTemperature_oid[]		   = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 7 };
	static oid dns340lFanStatus_oid[]		      = { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 8 };

	//malloc structure memory
	p_nasAgent = malloc(DNS340LNASAGENT_SIZE);
	memset(p_nasAgent, 0, DNS340LNASAGENT_SIZE);
	DEBUGMSGTL(("dns340l_sysinfo", "Initializing\n"));
	
	//dns340lAgentVer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LAgentVer", handle_dns340lAgentVer,
							dns340lAgentVer_oid,
							OID_LENGTH(dns340lAgentVer_oid),
							HANDLER_CAN_RONLY));
     
	//dns340lSoftwareVersion
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LSoftwareVersion", handle_dns340lSoftwareVersion,
							dns340lSoftwareVersion_oid,
							OID_LENGTH(dns340lSoftwareVersion_oid),
							HANDLER_CAN_RONLY));

	//dns340LHostName 
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LHostName", handle_dns340lHostName,
							dns340lHostName_oid,
							OID_LENGTH(dns340lHostName_oid),
							HANDLER_CAN_RONLY));

	//dns340LFTPServer
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LFTPServer", handle_dns340lFTPServer,
							dns340lFTPServer_oid,
							OID_LENGTH(dns340lFTPServer_oid),
							HANDLER_CAN_RONLY));
	
	//dns340LNetType
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LNetType", handle_dns340lNetType,
							dns340lNetType_oid,
							OID_LENGTH(dns340lNetType_oid),
							HANDLER_CAN_RONLY));
							
	//dns340LTemperature
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LTemperature", handle_dns30l5Temperature,
							dns340lTemperature_oid,
							OID_LENGTH(dns340lTemperature_oid),
							HANDLER_CAN_RONLY));
							
	//dns340LFanStatus
	netsnmp_register_scalar(netsnmp_create_handler_registration
							("dns340LFanStatus", handle_dns340lFanStatus,
							dns340lFanStatus_oid,
							OID_LENGTH(dns340lFanStatus_oid),
							HANDLER_CAN_RONLY));
	
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lAgentVer         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lAgentVer(netsnmp_mib_handler *handler,
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
		snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lAgentVer\n", reqinfo->mode);
		return SNMP_ERR_GENERR;
	         
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lSoftwareVersion         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lSoftwareVersion(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lSoftwareVersion\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lHostName         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lHostName(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lHostName\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lFTPServer         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lFTPServer(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_config_xml(XML_FTP_STATE, p_nasAgent->ftp_enable,sizeof(p_nasAgent->ftp_enable)) == 0)
				return SNMP_ERR_GENERR;
				
			if(strcmp(p_nasAgent->ftp_enable, "0") == 0)
				strcpy(p_nasAgent->ftp_enable, "Disable");
			else
				strcpy(p_nasAgent->ftp_enable, "Enable");
			
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->ftp_enable,
									strlen(p_nasAgent->ftp_enable));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lFTPServer\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lNetType         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lNetType(netsnmp_mib_handler *handler,
							netsnmp_handler_registration *reginfo,
							netsnmp_agent_request_info *reqinfo,
							netsnmp_request_info *requests)
{
	switch(reqinfo->mode)
	{
		case MODE_GET:
			if(get_config_xml(XML_SAB_ADS_ENABLE, p_nasAgent->net_type,sizeof(p_nasAgent->net_type)) == 0)
				return SNMP_ERR_GENERR;

			if(strcmp(p_nasAgent->net_type, "0") == 0)
				strcpy(p_nasAgent->net_type, "Workgroup");
			else
				strcpy(p_nasAgent->net_type, "Active Directory");
				
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
									(u_char *)p_nasAgent->net_type,
									strlen(p_nasAgent->net_type));
			break;
		
		default:
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lNetType\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns30l5Temperature         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns30l5Temperature(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns30l5Temperature\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - handle_dns340lFanStatus         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int handle_dns340lFanStatus(netsnmp_mib_handler *handler,
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
			snmp_log(LOG_ERR, "unknown mode (%d) in handle_dns340lFanStatus\n", reqinfo->mode);
			return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}





