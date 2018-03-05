#ifndef DNS345_RAIDINFO_H
#define DNS345_RAIDINFO_H

typedef struct _DNS345_VOLUME_TABLE_
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
	struct _DNS345_VOLUME_TABLE_ 	*next;
}dns345volumeTable, *ID_dns345volumeTable;
#define DNS345VOLUME_TABLE_SIZE	(sizeof(struct _DNS345_VOLUME_TABLE_))

#define DNS345_VOLUME_NUM			1
#define DNS345_VOLUME_NAME			2
#define DNS345_VOLUME_FS_TYPE		3
#define DNS345_VOLUME_RAID_LEVEL	4
#define DNS345_VOLUME_SIZE			5
#define DNS345_VOLUME_FREE_SPACE	6


void initialize_table_dns345VolumeTable(void);
Netsnmp_First_Data_Point dns345VolumeTable_get_first_data_point;
Netsnmp_Next_Data_Point dns345VolumeTable_get_next_data_point;
Netsnmp_Node_Handler dns345VolumeTable_handler;
ID_dns345volumeTable dns345VolumeTable_createEntry(long volume_num);
void dns345VolumeTable_Initialize(void);


#endif
