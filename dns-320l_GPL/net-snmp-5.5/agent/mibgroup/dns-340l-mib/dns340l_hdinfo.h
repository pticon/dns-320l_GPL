#ifndef DNS345_HDINFO_H
#define DNS345_HDINFO_H

typedef struct _DNS340L_DISK_TABLE_
{
	long	disk_num;
	char	disk_vendor[64];
	char	disk_model[64];
	char	disk_serial[64];
	char	disk_temperature[64];
	char	disk_capacity[64];
	
	//Illustrate using a simple linked list 
	int             			valid;
	struct _DNS340L_DISK_TABLE_ 	*next;
}dns345diskTable, *ID_dns340ldiskTable;
#define DNS340L_DISK_TABLE_SIZE	(sizeof(struct _DNS340L_DISK_TABLE_))

#define DNS340L_DISK_NUM				1
#define DNS340L_DISK_VENDOR			2
#define DNS340L_DISK_MODEL			3
#define DNS340L_DISK_SERIAL			4
#define DNS340L_DISK_TEMPERATURE		5
#define DNS340L_DISK_CAPACITY		6


void initialize_table_dns340lDiskTable(void);
void dns340lDiskTable_Initialize(void);
Netsnmp_First_Data_Point dns340lDiskTable_get_first_data_point;
Netsnmp_Next_Data_Point dns340lDiskTable_get_next_data_point;
Netsnmp_Node_Handler dns340lDiskTable_handler;
ID_dns340ldiskTable dns340lDiskTable_createEntry(long disk_num);

#endif
