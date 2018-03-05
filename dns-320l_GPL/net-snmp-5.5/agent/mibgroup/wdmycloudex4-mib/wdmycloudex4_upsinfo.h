#ifndef WDMYCLOUDEX4_UPSINFO_H
#define WDMYCLOUDEX4_UPSINFO_H

typedef struct _WDMYCLOUDEX4_UPS_TABLE_
{
	char	ups_mode[64];
	char	ups_manufacturer[64];
	char	ups_product[64];
	char	ups_batterycharge[64];
	char	ups_status[64];
	long	ups_num;

	//Illustrate using a simple linked list
	int             			valid;
	struct _WDMYCLOUDEX4_UPS_TABLE_ 	*next;
}wdmycloudex4UPSTable, *ID_wdmycloudex4UPSTable;
#define WDMYCLOUDEX4_UPS_TABLE_SIZE	(sizeof(struct _WDMYCLOUDEX4_VOLUME_TABLE_))

#define WDMYCLOUDEX4_UPS_NUM		1
#define WDMYCLOUDEX4_UPS_MODE				2
#define WDMYCLOUDEX4_UPS_MANUFACTURER			3
#define WDMYCLOUDEX4_UPS_PRODUCT			4
#define WDMYCLOUDEX4_UPS_BATTERYCHARGE			5
#define WDMYCLOUDEX4_UPS_STATUS		6



void initialize_table_wdmycloudex4UPSTable(void);
void wdmycloudex4UPSTable_Initialize(void);
Netsnmp_First_Data_Point wdmycloudex4UPSTable_get_first_data_point;
Netsnmp_Next_Data_Point wdmycloudex4UPSTable_get_next_data_point;
Netsnmp_Node_Handler wdmycloudex4UPSTable_handler;
ID_wdmycloudex4UPSTable wdmycloudex4UPSTable_createEntry(long ups_num);

#endif
