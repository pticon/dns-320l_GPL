#ifndef WDMYCLOUDEX2_HDINFO_H
#define WDMYCLOUDEX2_HDINFO_H

typedef struct _WDMYCLOUDEX2_DISK_TABLE_
{
	long	disk_num;
	char	disk_vendor[64];
	char	disk_model[64];
	char	disk_serial[64];
	char	disk_temperature[64];
	char	disk_capacity[64];

	//Illustrate using a simple linked list
	int             			valid;
	struct _WDMYCLOUDEX2_DISK_TABLE_ 	*next;
}wdmycloudex2diskTable, *ID_wdmycloudex2diskTable;
#define WDMYCLOUDEX2_DISK_TABLE_SIZE	(sizeof(struct _WDMYCLOUDEX2_VOLUME_TABLE_))

#define WDMYCLOUDEX2_DISK_NUM				1
#define WDMYCLOUDEX2_DISK_VENDOR			2
#define WDMYCLOUDEX2_DISK_MODEL			3
#define WDMYCLOUDEX2_DISK_SERIAL			4
#define WDMYCLOUDEX2_DISK_TEMPERATURE		5
#define WDMYCLOUDEX2_DISK_CAPACITY		6


void initialize_table_wdmycloudex2DiskTable(void);
void wdmycloudex2DiskTable_Initialize(void);
Netsnmp_First_Data_Point wdmycloudex2DiskTable_get_first_data_point;
Netsnmp_Next_Data_Point wdmycloudex2DiskTable_get_next_data_point;
Netsnmp_Node_Handler wdmycloudex2DiskTable_handler;
ID_wdmycloudex2diskTable wdmycloudex2DiskTable_createEntry(long disk_num);

#endif
