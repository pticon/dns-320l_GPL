#ifndef WDMYCLOUDEX4_RAIDINFO_H
#define WDMYCLOUDEX4_RAIDINFO_H

typedef struct _WDMYCLOUDEX4_VOLUME_TABLE_
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
	struct _WDMYCLOUDEX4_VOLUME_TABLE_ 	*next;
}wdmycloudex4volumeTable, *ID_wdmycloudex4volumeTable;
#define WDMYCLOUDEX4VOLUME_TABLE_SIZE	(sizeof(struct _WDMYCLOUDEX4_VOLUME_TABLE_))

#define WDMYCLOUDEX4_VOLUME_NUM			1
#define WDMYCLOUDEX4_VOLUME_NAME			2
#define WDMYCLOUDEX4_VOLUME_FS_TYPE		3
#define WDMYCLOUDEX4_VOLUME_RAID_LEVEL	4
#define WDMYCLOUDEX4_VOLUME_SIZE			5
#define WDMYCLOUDEX4_VOLUME_FREE_SPACE	6


void initialize_table_wdmycloudex4VolumeTable(void);
Netsnmp_First_Data_Point wdmycloudex4VolumeTable_get_first_data_point;
Netsnmp_Next_Data_Point wdmycloudex4VolumeTable_get_next_data_point;
Netsnmp_Node_Handler wdmycloudex4VolumeTable_handler;
ID_wdmycloudex4volumeTable wdmycloudex4VolumeTable_createEntry(long volume_num);
void wdmycloudex4VolumeTable_Initialize(void);


#endif
