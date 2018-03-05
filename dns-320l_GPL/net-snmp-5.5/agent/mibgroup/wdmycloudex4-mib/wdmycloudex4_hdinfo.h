#ifndef WDMYCLOUDEX4_HDINFO_H
#define WDMYCLOUDEX4_HDINFO_H

typedef struct _WDMYCLOUDEX4_DISK_TABLE_
{
	long	disk_num;
	char	disk_vendor[64];
	char	disk_model[64];
	char	disk_serial[64];
	char	disk_temperature[64];
	char	disk_capacity[64];

	//Illustrate using a simple linked list
	int             			valid;
	struct _WDMYCLOUDEX4_DISK_TABLE_ 	*next;
}wdmycloudex4diskTable, *ID_wdmycloudex4diskTable;
#define WDMYCLOUDEX4_DISK_TABLE_SIZE	(sizeof(struct _WDMYCLOUDEX4_VOLUME_TABLE_))

#define WDMYCLOUDEX4_DISK_NUM				1
#define WDMYCLOUDEX4_DISK_VENDOR			2
#define WDMYCLOUDEX4_DISK_MODEL			3
#define WDMYCLOUDEX4_DISK_SERIAL			4
#define WDMYCLOUDEX4_DISK_TEMPERATURE		5
#define WDMYCLOUDEX4_DISK_CAPACITY		6


void initialize_table_wdmycloudex4DiskTable(void);
void wdmycloudex4DiskTable_Initialize(void);
Netsnmp_First_Data_Point wdmycloudex4DiskTable_get_first_data_point;
Netsnmp_Next_Data_Point wdmycloudex4DiskTable_get_next_data_point;
Netsnmp_Node_Handler wdmycloudex4DiskTable_handler;
ID_wdmycloudex4diskTable wdmycloudex4DiskTable_createEntry(long disk_num);

#endif
