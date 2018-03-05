#ifndef DNS340L_RAIDINFO_H
#define DNS340L_RAIDINFO_H

typedef struct _DNS340L_VOLUME_TABLE_
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
	struct _DNS340L_VOLUME_TABLE_ 	*next;
}dns340lvolumeTable, *ID_dns340lvolumeTable;
#define DNS340LVOLUME_TABLE_SIZE	(sizeof(struct _DNS340L_VOLUME_TABLE_))

#define DNS340L_VOLUME_NUM			1
#define DNS340L_VOLUME_NAME			2
#define DNS340L_VOLUME_FS_TYPE		3
#define DNS340L_VOLUME_RAID_LEVEL	4
#define DNS340L_VOLUME_SIZE			5
#define DNS340L_VOLUME_FREE_SPACE	6


void initialize_table_dns340lVolumeTable(void);
Netsnmp_First_Data_Point dns340lVolumeTable_get_first_data_point;
Netsnmp_Next_Data_Point dns340lVolumeTable_get_next_data_point;
Netsnmp_Node_Handler dns340lVolumeTable_handler;
ID_dns340lvolumeTable dns340lVolumeTable_createEntry(long volume_num);
void dns340lVolumeTable_Initialize(void);


#endif
