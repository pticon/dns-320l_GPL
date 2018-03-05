#ifndef DNS340L_UPSINFO_H
#define DNS340L_UPSINFO_H

typedef struct _DNS340L_UPS_TABLE_
{
	char	ups_mode[64];
	char	ups_manufacturer[64];
	char	ups_product[64];
	char	ups_batterycharge[64];
	char	ups_status[64];
	long	ups_num;

	//Illustrate using a simple linked list
	int             			valid;
	struct _DNS340L_UPS_TABLE_ 	*next;
}dns340lUPSTable, *ID_dns340lUPSTable;
#define DNS340L_UPS_TABLE_SIZE	(sizeof(struct _DNS340L_UPS_TABLE_))

#define DNS340L_UPS_NUM		1
#define DNS340L_UPS_MODE				2
#define DNS340L_UPS_MANUFACTURER			3
#define DNS340L_UPS_PRODUCT			4
#define DNS340L_UPS_BATTERYCHARGE			5
#define DNS340L_UPS_STATUS		6



void initialize_table_dns340LUPSTable(void);
void dns340LUPSTable_Initialize(void);
Netsnmp_First_Data_Point dns340LUPSTable_get_first_data_point;
Netsnmp_Next_Data_Point dns340LUPSTable_get_next_data_point;
Netsnmp_Node_Handler dns340LUPSTable_handler;
ID_dns340lUPSTable dns340LUPSTable_createEntry(long ups_num);

#endif
