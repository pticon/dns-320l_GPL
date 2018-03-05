#ifndef WDMYCLOUDEX2_RAIDINFO_H
#define WDMYCLOUDEX2_RAIDINFO_H

typedef struct _WDMYCLOUDEX2_VOLUME_TABLE_
{
	int entry_num;
	long	volume_num;
	char	volume_name[64];
	char	volume_fs_type[64];
	char	volume_raid_level[64];
	char	volume_size[64];
	char	volume_free_space[64];

	//Illustrate using a simple linked list
	int             				valid;
	struct _WDMYCLOUDEX2_VOLUME_TABLE_ 	*next;
}wdmycloudex2volumeTable, *ID_wdmycloudex2volumeTable;
#define WDMYCLOUDEX2VOLUME_TABLE_SIZE	(sizeof(struct _WDMYCLOUDEX2_VOLUME_TABLE_))

#define WDMYCLOUDEX2_VOLUME_NUM			1
#define WDMYCLOUDEX2_VOLUME_NAME			2
#define WDMYCLOUDEX2_VOLUME_FS_TYPE		3
#define WDMYCLOUDEX2_VOLUME_RAID_LEVEL	4
#define WDMYCLOUDEX2_VOLUME_SIZE			5
#define WDMYCLOUDEX2_VOLUME_FREE_SPACE	6


void initialize_table_wdmycloudex2VolumeTable(void);
Netsnmp_First_Data_Point wdmycloudex2VolumeTable_get_first_data_point;
Netsnmp_Next_Data_Point wdmycloudex2VolumeTable_get_next_data_point;
Netsnmp_Node_Handler wdmycloudex2VolumeTable_handler;
ID_wdmycloudex2volumeTable wdmycloudex2VolumeTable_createEntry(long volume_num);
void wdmycloudex2VolumeTable_Initialize(void);


#endif
