#ifndef DNS345_UPSINFO_H
#define DNS345_UPSINFO_H

typedef struct _DNS345_UPS_TABLE_
{
	char	ups_mode[64];
	char	ups_manufacturer[64];
	char	ups_product[64];
	char	ups_batterycharge[64];
	char	ups_status[64];
	long	ups_num;

	//Illustrate using a simple linked list
	int             			valid;
	struct _DNS345_UPS_TABLE_ 	*next;
}dns345UPSTable, *ID_dns345UPSTable;
#define DNS345_UPS_TABLE_SIZE	(sizeof(struct _DNS345_VOLUME_TABLE_))

#define DNS345_UPS_NUM		1
#define DNS345_UPS_MODE				2
#define DNS345_UPS_MANUFACTURER			3
#define DNS345_UPS_PRODUCT			4
#define DNS345_UPS_BATTERYCHARGE			5
#define DNS345_UPS_STATUS		6



void initialize_table_dns345UPSTable(void);
void dns345UPSTable_Initialize(void);
Netsnmp_First_Data_Point dns345UPSTable_get_first_data_point;
Netsnmp_Next_Data_Point dns345UPSTable_get_next_data_point;
Netsnmp_Node_Handler dns345UPSTable_handler;
ID_dns345UPSTable dns345UPSTable_createEntry(long ups_num);

#endif
