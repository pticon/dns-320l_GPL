// kate: replace-tabs off; space-indent off;
// MODIFY History:
// Date-Author          Revision        Description
//-----------------------------------------------------------------------------
//Alpha.Vincent12012008	v1.01.20081201	fixed HDD can't sleep issue.


#include "../config.h"
#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
//#include <assert.h>
#include <regex.h>
#include <sys/utsname.h>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <dirent.h>
#include <mntent.h>
#include <libxmldbc.h>
#include "media_type.h"
#include "filter.h"

#define SHARE_FOLDER_MAX_LEN	512
#define MAX_FOLDER_NUM			10
char		F_UpnpavFolder[MAX_FOLDER_NUM][SHARE_FOLDER_MAX_LEN];
int			F_Folder_num = 0;
int			run_cmd_flag = 1;
int			main_loop_flag = 1;
int			sleep_flag = 1;

#define EXIT_OK 		0
#define EXIT_ERROR 		1
#define EXIT_TIMEOUT 	2


//#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

#define UPDATE_ADD				1
#define UPDATE_DELETE			2
#define UPDATE_DELETE_DIR		3
#define UPDATE_RENAME			4

//#define DBG						1
#define INO_UPNP_VERSION		"v1.07.20091026"
#define ITUNE_FOLDER_FILE		"/etc/mt-daapd.conf"
#define UPNPAV_FOLDER_FILE		"/etc/upnpav.conf"
#define RUN_FILE_NAME			"UpdateDB"

//#define STR_A2_NAS_PROG			"/mnt/HD/HD_a2/Nas_Prog"
//#define STR_B2_NAS_PROG			"/mnt/HD/HD_b2/Nas_Prog"

#define MAX_PATH_LEN			1024

#define CMD_LENGTH				64
typedef struct _I_Node_
{
	char 			command[CMD_LENGTH];
	char 			*path;
	int				len;
	struct _I_Node_	*next;
}*I_NODE_ID;

typedef struct _I_LIST_
{
	int			num;
	I_NODE_ID	head;
	I_NODE_ID	tail;
}I_LIST, *I_LIST_ID;


I_LIST		i_list;
I_LIST_ID	p_i_list = &i_list;
pthread_mutex_t CNT_LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SCAN_LOCK = PTHREAD_MUTEX_INITIALIZER;
int			debug_flag = 0;

//royc 20090330
//#define		TIME_LIMITED		120		//2 mins
#define		INPUT_LIMITED		2048		//64 input
#define		DB_REBUILD_LIMITED	100
//int			input_count=0;
int			update_type=0;		//0: for old,	1:call prescan
//time_t		old_time;
//time_t		now_time;

/*--------------------------------------------------------------
* ROUTINE NAME - check_ps
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
int check_ps(char *process)
{
	static DIR *dir;
	struct dirent *entry;
	int pid;
	char *name;
	FILE *fp;
	char buf[1024];
	char status[64];
	char ps_name[128];
	int find;
	
	if(strcmp(process, "dont_check") == 0)
		return 1;
	
	if (!dir)
	{
		dir = opendir("/proc");
		if(!dir)
			printf("Can't open /proc");
	}
	
	find=0;
	
	while(1)
	{
		//get /proc files
		if((entry = readdir(dir)) == NULL)
		{
			closedir(dir);
			dir = 0;
			break;
		}
		
		//get name
		name = entry->d_name;
		if (!(*name >= '0' && *name <= '9'))
			continue;
		
		pid = atoi(name);
		
		//open file to read status
		sprintf(status, "/proc/%d/status", pid);
		if((fp = fopen(status, "r")) == NULL)
			continue;
		name = fgets(buf, sizeof(buf), fp);
		fclose(fp);
		if(name == NULL)
			continue;
		
		sscanf(buf, "Name: %s", ps_name);

		//printf("pid-->[%s] %d\n", ps_name, pid);
		
		if(strcmp(process, ps_name) == 0)
		{
			find++;
		}
	}
	
	return find;
}
//royc end

/*--------------------------------------------------------------
* ROUTINE NAME - get_path_len
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
int get_path_len(int len)
{
	int		tmp = len;
	
	//error handle
	if(len == 0)
		return 0;
	
	if((tmp&(0x0003)) == 0)
	{
		//when len = 4*n, we return 4*n+4
		tmp += 4;
	}
	else
	{
		//when len = 4*n-1, 4*n-2, 4*n-3,we return 4*n
		//tmp = (tmp+3) & (-4);
		tmp = (tmp+0x3) & (0xfffc);
	}
//#ifdef DBG
	if(debug_flag)
	printf("node path = %d\n", tmp);
//#endif
	
	return tmp;
}

/*--------------------------------------------------------------
* ROUTINE NAME - Get_New_Node
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
static I_NODE_ID Get_New_Node(int len)
{
	I_NODE_ID		p;
	
	p = (I_NODE_ID)malloc(sizeof(struct _I_Node_));
	
	if( p == NULL )
	{
		return NULL;
	}
	else
	{
		memset(p->command, 0, CMD_LENGTH);
		p->path = (char *)malloc(get_path_len(len));
		if(p->path == NULL)
		{
			//not enough memeory
			free(p);
			return NULL;
		}
		
		p->len = len;
		return p;
	}
}

/*--------------------------------------------------------------
* ROUTINE NAME - Is_Empty
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
inline static int Is_Empty(void)
{
	return (p_i_list->num == 0) ? 1 : 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - Add_Node_Tail
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
static void Add_Node_Tail(char *command, char *path)
{
	I_NODE_ID		p;
	int 			len;
	
	len = strlen(path);
	p = Get_New_Node(len);
	if( p == NULL )
	{
		printf("ERROR: not enough memory!!\n");
		return;
	}

	pthread_mutex_lock(&CNT_LOCK);	
	if( Is_Empty() )
	{
		//list is empty, just add a node is ok
		p_i_list->num ++;
		p_i_list->head = p_i_list->tail = p;
	}
	else
	{
		//list not empty, we add at tail
		p_i_list->num ++;
		p_i_list->tail->next = p;
		p_i_list->tail = p;
	}
	
	//copy value to node
	strncpy(p->command, command, CMD_LENGTH);
	strncpy(p->path, path, len);
	
	p->next = NULL;
	
	pthread_mutex_unlock(&CNT_LOCK);
}

/*--------------------------------------------------------------
* ROUTINE NAME - Get_Node_Del_Head
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
static int Get_Node_Del_Head(char *command, char *path)
{
	I_NODE_ID p;
	
	if( Is_Empty() )
	{
		return 0;
	}
	
	pthread_mutex_lock(&CNT_LOCK);	
	p = p_i_list->head;
	if(p!=NULL)
	{
		//copy data
		strncpy(command, p->command, CMD_LENGTH);
		strncpy(path, p->path, p->len);
		
		//remove head node in list
		p_i_list->num --;
		p_i_list->head = p->next;
		
		if(p)
		{
			if(p->path)
				free(p->path);
			p->path=0;

			free(p);
			p=0;
		}
		pthread_mutex_unlock(&CNT_LOCK);
		return 1;
	}
	else
	{
		pthread_mutex_unlock(&CNT_LOCK);
		return 0;
	}
}

/*--------------------------------------------------------------
* ROUTINE NAME - Free_All_List
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
static void Free_All_List(void)
{
	I_NODE_ID		p;
	
	pthread_mutex_lock(&CNT_LOCK);
	
	while( !Is_Empty() )
	{
		p = p_i_list->head;
		
		if( p_i_list->num == 1 )
		{
			p_i_list->num = 0;
			p_i_list->head  = NULL;
			p_i_list->tail  = NULL;
			if(p)
			{
				if(p->path)
					free(p->path);
				p->path=0;
				
				free(p);
				p=0;
			}
		}
		else
		{
			p_i_list->num --;
			p_i_list->head = p->next;
			if(p)
			{
				if(p->path)
					free(p->path);
				p->path=0;
				
				free(p);
				p=0;
			}
		}
	}
	
	pthread_mutex_unlock(&CNT_LOCK);
}

/*-----------------------------------------------------------------
* ROUTINE NAME - remove_0D_0A         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
inline void remove_0D_0A(char *tmp)
{
	char 	*p;
	
	if(tmp == NULL || strlen((char*)tmp)==0)
	{
		return;
	}
	
	p = tmp + strlen((char*)tmp) - 1;
	if(*p==0x0a)
		*p=0;
	if(*(p-1)==0x0d)
		*(p-1) = 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - InitNASMediaFolder
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int InitNASMediaFolder(void)
{
	int		i;
	char	xml_name[128];
	int		enable;
	
	//royc 20100208
	//fill zero to buffer, (MAX_FOLDER_NUM = 10)
	for(i=0; i<MAX_FOLDER_NUM; i++)
	{
		memset(F_UpnpavFolder[i], 0, SHARE_FOLDER_MAX_LEN);
	}
	
	F_Folder_num = 0;
	for(i=0; i<MAX_FOLDER_NUM; i++)
	{
		sprintf(xml_name, "/app_mgr/upnpavserver/sharedfolders/item:%d/enable", i);
		enable = xml_get_int(xml_name);
		
		//this item not enabled, we skip it
		if(enable == 0)
			continue;
			
		sprintf(xml_name, "/app_mgr/upnpavserver/sharedfolders/item:%d/path", i);
		xml_get_str(xml_name, SHARE_FOLDER_MAX_LEN, F_UpnpavFolder[F_Folder_num]);
		
		//this item path is null, we skip it
		if(strlen(F_UpnpavFolder[F_Folder_num]) == 0)
			continue;
			
		//this path not exist, we skip it
		if(access(F_UpnpavFolder[F_Folder_num], F_OK) != 0)
		{
			memset(F_UpnpavFolder[F_Folder_num], 0, SHARE_FOLDER_MAX_LEN);
			continue;
		}
		
		//inc folder number
		F_Folder_num++;
	}

	//no path found in array, we return 0
	if(F_Folder_num == 0)
	{
		return 0;
	}
	
	//royc end

	return 1;
}

/*--------------------------------------------------------------
* ROUTINE NAME - GetFileExt 
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
char *GetFileExt(char *path)
{
	int len, i;
	
	len = (int)strlen(path);
	if(len <= 4)
	{
		return NULL;	
	}
	
	for( i=len-1; i>=0; i--)
	{
		if('.' == path[i])
		{
			return path+i;
		}
	}
	
	return NULL;
}

/*--------------------------------------------------------------
* ROUTINE NAME - itune_media_type
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
//int itune_media_type(char *str)
//{
//	if(strncasecmp(str, ".mp3", 4) == 0
//		|| strncasecmp(str, ".m4a", 4) == 0
//		|| strncasecmp(str, ".m4p", 4) == 0 )
//	{
//		return 1;
//	}
//	
//	return 0;
//}

/*--------------------------------------------------------------
* ROUTINE NAME - CheckFileType
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int CheckFileType(char *full_path, int updateFlag)
{
	int		i, j;
	char	   *file_ext;
	char     tmp_str[MAX_PATH_LEN] = {0};
	char     *volume_path_str[4] = {"/mnt/HD/HD_a2", "/mnt/HD/HD_b2", "/mnt/HD/HD_c2", "/mnt/HD/HD_d2"};
	
	//dont check hidden partition
	if(strstr(full_path, "/mnt/HD_a4")!=NULL
		|| strstr(full_path, "/mnt/HD_b4")!=NULL
		|| strstr(full_path, "/mnt/HD_c4")!=NULL
		|| strstr(full_path, "/mnt/HD_d4")!=NULL)
	{
//#ifdef DBG
		if(debug_flag)
		printf("-----------------IGNORE: %s\n", full_path);
//#endif
		return 0;
	}
	
//	//royc 20100401
//	//ignore a2, b2 Nas_Prog
//	if(memcmp(full_path, STR_A2_NAS_PROG, strlen(STR_A2_NAS_PROG)) == 0
//		|| memcmp(full_path, STR_B2_NAS_PROG, strlen(STR_B2_NAS_PROG)) == 0 )
//	{
//		return 0;
//	}
//	//royc end
	
	//20110613 check filter.h
	for(i=0; i<(sizeof(ft_table)/sizeof(ft_define)); i++)
	{
	   for(j=0; j<4; j++)
	   {
	      memset(tmp_str, 0, MAX_PATH_LEN);
	      sprintf(tmp_str, "%s/%s", volume_path_str[j], ft_table[i].ft_name);

	      if(memcmp(full_path, tmp_str, strlen(tmp_str)) == 0)
   	   {
   	      //found it
   	      return 0;
   	   }
	   }
	}
	//royc end
	
	//check dir
	if(isdir(full_path) || updateFlag == UPDATE_DELETE_DIR)
	{
		return 1;
	}
	
	//below is file
	file_ext = GetFileExt(full_path);
	if(file_ext == NULL)
	{
		//file that no file ext name
		return 0;
	}

//#ifdef DBG	
	if(debug_flag)
	   printf("-----------------ext: [%s]\n", file_ext);
//#endif
	
	for(i=0; CDS_FileType_Map[i].file_class != FC_UNKNOWN; i++)
	{
		//compare file ext name for upnpav
		//CDS_FileType_Map come from media_type.h
		if(strncasecmp(file_ext, CDS_FileType_Map[i].file_ext, strlen(CDS_FileType_Map[i].file_ext)) == 0)
		{
			//the file ext is media type
			return 1;
		}
	}
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - save_data
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
void save_data(int updateFlag, char *FilePath)
{
	char	i_command[CMD_LENGTH];
	char 	i_path[MAX_PATH_LEN];
	
	//royc 20090330
	if(update_type == 0)
	{
		if(p_i_list->num > INPUT_LIMITED)
		{
			//time(&now_time);
			//now_time = old_time;
			//input_count = 0;
			pthread_mutex_lock(&SCAN_LOCK);
			update_type = 1;
			pthread_mutex_unlock(&SCAN_LOCK);
			printf("**Enter time SCAN_LOCK mode\n");
			return;
		}
	}
//	else
//	{
//		input_count ++;
//	}
	//royc end
	
//#ifdef DBG
	if(debug_flag)
	{
		printf("---------------->Got EVENT(%d) [%s]!!!\n", updateFlag, FilePath);
	}
//#endif
	
	//check file ext type
	//check path length
	if(strlen(FilePath) > MAX_PATH_LEN)
	{
		printf("ERROR: File path too long !!!\n");
		return;
	}
	
	if(CheckFileType(FilePath, updateFlag) == 0)
	{
		//check file ext type not error
		return;
	}
	
	memset(i_command, 0, CMD_LENGTH);
	memset(i_path, 0, MAX_PATH_LEN);
	
	switch(updateFlag)
	{
		case UPDATE_ADD:
			sprintf(i_command, "%s -d UPnP -a add -p ", RUN_FILE_NAME);
			sprintf(i_path, "%s", FilePath);
			break;
			
		case UPDATE_DELETE:
			sprintf(i_command, "%s -d UPnP -a del -p ", RUN_FILE_NAME);
			sprintf(i_path, "%s", FilePath);
			break;
		
		case UPDATE_DELETE_DIR:
			sprintf(i_command, "%s -d UPnP -a delDir -p ", RUN_FILE_NAME);
			sprintf(i_path, "%s", FilePath);
			break;
			
		case UPDATE_RENAME:
			//do nothing
			break;
	}
	
//#ifdef DBG
	if(debug_flag)
	printf("===============> %s\n", i_path);
//#endif
	
	
	//insert data
	Add_Node_Tail(i_command, i_path);
	
}

/*--------------------------------------------------------------
* ROUTINE NAME - sig_kill
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
static void sig_kill(int signo)
{
	printf("!!! Inotify Terminated !!!\n");
	
	//stop
	main_loop_flag = 0;
	run_cmd_flag = 0;
	Free_All_List();
	
	exit(0);
	return;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - run_cmd_thread         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
static void sig_usr1(int signo)
{
	printf("open inotify_upnp debug mode!!\n");
	debug_flag = 1;
	
	return;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - run_cmd_thread         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
static void sig_usr2(int signo)
{
	printf("close inotify_upnp debug mode!!\n");
	debug_flag = 0;
	
	return;
}

/*--------------------------------------------------------------
* ROUTINE NAME - fix_path
*---------------------------------------------------------------
* FUNCTION: parse path, and add '\' before '$' char
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
void fix_path(char *path)
{
	char	fix_path[MAX_PATH_LEN];
	int		i, len;
	
	if(strchr(path, '$') == 0 && strchr(path, '`') == 0 && strchr(path, '#') == 0
		&& strchr(path, '%') == 0 && strchr(path, '^') == 0 && strchr(path, '&') == 0
		&& strchr(path, '(') == 0 && strchr(path, ')') == 0 && strchr(path, '+') == 0
		&& strchr(path, '{') == 0 && strchr(path, '}') == 0 && strchr(path, ';') == 0
		&& strchr(path, '[') == 0 && strchr(path, ']') == 0 && strchr(path, '\'') == 0
		&& strchr(path, '=') == 0 && strchr(path, ' ') == 0)
	{
		return;
	}
	
	memset(fix_path, 0, MAX_PATH_LEN);
	len = 0;
	for(i=0; i<strlen(path); i++)
	{
		if(path[i]=='$' || path[i]=='`' || path[i]=='#'
		|| path[i]=='%' || path[i]=='^' || path[i]=='&'
		|| path[i]=='(' || path[i]==')' || path[i]=='+'
		|| path[i]=='{' || path[i]=='}' || path[i]==';'
		|| path[i]=='[' || path[i]==']' || path[i]=='\''
		|| path[i]=='=' || path[i]==' ')
		{
			fix_path[len++] = '\\';
		}
		fix_path[len++] = path[i];
	}
	//printf("[%s]\n", fix_path);
	strcpy(path, fix_path);
	
#if 0
	char	fix_path[MAX_PATH_LEN];
	int		i, len;
	
	if(strchr(path, '$') == 0 && strchr(path, '`') == 0)
	{
		return;
	}
	
	memset(fix_path, 0, MAX_PATH_LEN);
	len = 0;
	
	for(i=0; i<strlen(path); i++)
	{
		if(path[i]=='$' || path[i]=='`')
		{
			fix_path[len++] = '\\';
		}
		fix_path[len++] = path[i];
	}
	
	printf("[%s]\n", fix_path);
	strcpy(path, fix_path);
#endif
}

/*-----------------------------------------------------------------
* ROUTINE NAME - run_cmd_thread         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void *run_cmd_thread(void *arg)
{
	int		i;
	char	run_cmd[MAX_PATH_LEN];
	char	g_command[CMD_LENGTH];
	char	g_path[MAX_PATH_LEN];
	int		rebuild_count;
	
	rebuild_count = 0;
	run_cmd_flag = 1;
	
	while(run_cmd_flag == 1)
	{
		//royc 20090330
		if(update_type == 1)
		{
			Free_All_List();
			printf("**call prescan !!\n");
			system("/usr/sbin/prescan -i");
			
			pthread_mutex_lock(&SCAN_LOCK);
			update_type = 0;
			pthread_mutex_unlock(&SCAN_LOCK);
			
			printf("**leave time limited mode\n");
			
//			if(now_time == old_time)
//			{
//				//call prescan
//				printf("**call prescan !!\n");
//				system("/usr/sbin/prescan -i");
//			}
//			time(&now_time);

//			if(now_time-old_time >= TIME_LIMITED)
//			{
//				time(&old_time);
//				
//				//check prescan finished or not
//				if(check_ps("prescan")==0)
//				{
//					if(input_count == 0)
//					{
//						update_type = 0;
//						printf("**leave time limited mode\n");
//					}
//					else
//					{
//						input_count = 0;
//						old_time = now_time;
//					}
//				}
//				continue;
//			}
			
			sleep(1);
			continue;
		}
		else
		{
			//royc 20100402
			if(p_i_list->num > INPUT_LIMITED)
			{
				pthread_mutex_lock(&SCAN_LOCK);
				update_type = 1;
				pthread_mutex_unlock(&SCAN_LOCK);
				continue;
			}	
			//royc end
		}
		//royc end
		
		//check any item in database
		if( Is_Empty() )
		{
//#ifdef DBG
	if(debug_flag)
		printf("###############data is empty !!!\n");
//#endif

			sleep_flag = 1;
        	
			//link list is empty, this thread is in-sleep
			while(1)
			{
				if(sleep_flag == 0)
				{
					break;
				}
				sleep(3);
			}
			continue;
		}
//#ifdef DBG
		if(debug_flag)	
		printf("number = %d\n", p_i_list->num);
//#endif

		memset(g_command, 0, CMD_LENGTH);
		memset(g_path, 0, MAX_PATH_LEN);
		Get_Node_Del_Head(g_command, g_path);
		fix_path(g_path);
		rebuild_count++;
		
		//check to force rebuild database index
		memset(run_cmd, 0, MAX_PATH_LEN);
		if(Is_Empty() || rebuild_count >= DB_REBUILD_LIMITED)
		{
			//reset count
			rebuild_count = 0;
			
			//add cmd with -i opt
			sprintf(run_cmd, "%s%s -i", g_command, g_path);
		}
		else
		{
			//run cmd here
			sprintf(run_cmd, "%s%s", g_command, g_path);
		}
			
//#ifdef DBG
		if(debug_flag)
		printf("###############should run [%s]\n", run_cmd);
//#endif

		//run UpdateDB cmd
		system(run_cmd);
	
	}
}

/*--------------------------------------------------------------
* ROUTINE NAME - main
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	int 		events = 0;
	//int 		recursive = 0;
	int 		i;
	struct		inotify_event *event;
	char		moved_from[MAX_PATH_LEN];
	int			moved_from_flag = 0;
	FileList 	list;
	char		*path[11];
	
	char		new_name[MAX_PATH_LEN];
	char		new_file[MAX_PATH_LEN];
	char		del_name[MAX_PATH_LEN];
	
	
	pthread_t		run_cmd;
	struct utsname 	name;
	
	printf("inotify upnp Version: [%s]\n", INO_UPNP_VERSION);

	//royc 20090330
	//init value
	//input_count = 0;
	update_type = 0;
	//royc end
	
	//catch sigterm
	signal(SIGTERM, sig_kill);
	signal(SIGSEGV, sig_kill);
	signal(SIGUSR1, sig_usr1);
	signal(SIGUSR2, sig_usr2);
	
	uname(&name);
	printf("kernel version: %s\n", name.release);

	//check /dev/inotify for old kernel version
	if(strstr(name.release, "2.6.12") != NULL)
	{
		if(access("/dev/inotify", F_OK) != 0)
		{
			printf("Create /dev/inotify !\n");
			system("mknod /dev/inotify c 10 63");
		}
	}

	//get default itune&upnp setting path
	if(InitNASMediaFolder() != 1)
	{
		printf("ERROR: Can't find iTune/Upnp setting Path !\n");
		exit(1);
	}

	//we set default parameter
	if ( !inotifytools_initialize() )
	{
		printf("ERROR: inotify init fail!\n");
		return EXIT_ERROR;
	}

	// Attempt to watch file
	// If events is still 0, make it all events.
	events = (IN_MOVED_FROM|IN_MOVED_TO|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVE_SELF|IN_CLOSE_WRITE);
	
	//royc 20100208
	for(i=0; i<F_Folder_num; i++)
	{
		path[i] = F_UpnpavFolder[i];
	}
	//royc end
	list = construct_path_list(F_Folder_num, path, NULL );

	if (0 == list.watch_files[0])
	{
		printf("No files specified to watch!\n");
		return EXIT_ERROR;
	}

	// now watch files
	for(i = 0; list.watch_files[i]; ++i )
	{
		char const *this_file = list.watch_files[i];
		
//#ifdef DBG
		printf("this_file = [%d: %s]\n", i, this_file);
//#endif

		if ( (!inotifytools_watch_recursively_with_exclude(this_file,events,list.exclude_files ))
			|| (!inotifytools_watch_file( this_file, events )) )
		{
			if ( inotifytools_error() == ENOSPC )
			{
				printf("Failed to watch %s; upper limit on inotify watches reached!\n", this_file);
			}
			else
			{
				printf("Couldn't watch %s: %s\n", this_file, strerror( inotifytools_error()));
			}
			return EXIT_ERROR;
		}
	}

	//start watch
	printf("OK: Inotify Watches Established.\n" );

	//create thread
	pthread_create(&run_cmd, NULL, &run_cmd_thread, NULL);
	
	// Now wait till we get event
	//debug_flag = 1;
	main_loop_flag = 1;
	while(main_loop_flag == 1)
	{
		//0 for non-blocking, negative the function will block until an event occurs
		event = inotifytools_next_event(0);
		if(errno == EINTR)
		{
			errno = 0;
			continue;
		}

		if ( !event )
		{
			if ( !inotifytools_error() )
			{
				printf("@@@@@@@@@@@@%s\n", strerror(inotifytools_error()));
				return EXIT_TIMEOUT;
			}
			else
			{
				printf("############%s\n", strerror(inotifytools_error()));
				return EXIT_ERROR;
			}
		}
		
		// if we last had MOVED_FROM and don't currently have MOVED_TO,
		// moved_from file must have been moved outside of tree - so unwatch it.
		if (moved_from_flag && !(event->mask & IN_MOVED_TO))
		{
			if (!inotifytools_remove_watch_by_filename(moved_from))
			{
				printf("Error removing watch on %s: %s\n", moved_from, strerror(inotifytools_error()));
			}
//			if(moved_from)
//				free(moved_from);
			moved_from_flag = 0;
		}

//#ifdef DBG
	if(debug_flag)
		printf("@@@@@@@@@@@@SYSTEM: event mask:[%x]\n", event->mask);
//#endif
		sleep_flag = 0;
		if ((event->mask & IN_CREATE) || (!moved_from_flag && (event->mask & IN_MOVED_TO)))
		{
			// New file - if it is a directory, watch it
//			static char * new_file;
//#ifdef DBG	
			if(debug_flag)		
			printf("@@@@@@@@@@@@SYSTEM: IN_CREATE\n");
//#endif		
			//nasprintf( &new_file, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
			sprintf( new_file, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
			if(moved_from_flag)
			{
				//printf("^^^^^^^^^^[%s][%s]\n", moved_from, new_file);
				inotifytools_replace_filename(moved_from, new_file);
//				if(moved_from)
//					free(moved_from);
				moved_from_flag = 0;
			}
			
			if (isdir(new_file) && !inotifytools_watch_recursively(new_file, events))
			{
				printf("Couldn't watch new directory %s: %s\n", new_file, strerror(inotifytools_error()));
			}
			
			//royc 20090608
			//save_data(UPDATE_ADD, new_file);
			if(isdir(new_file))
			{
				save_data(UPDATE_ADD, new_file);
			}
			//royc end
			
			//free(new_file);
		} // IN_CREATE
		else if(event->mask & IN_CLOSE_WRITE)
		{
			if(debug_flag)
				printf("@@@@@@@@@@@@SYSTEM: IN_CLOSE_WRITE\n");

			sprintf( new_file, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
			
			if(isdir(new_file))
			{
				continue;
			}
			
			if(debug_flag)
				printf("###[%s]\n", new_file);
			
			//check file exist or not
			usleep(100000);
			if(access(new_file, F_OK) == 0)
			{
				save_data(UPDATE_ADD, new_file);
			}
		}
		else if (event->mask & IN_MOVED_FROM)
		{
//#ifdef DBG
			if(debug_flag)
			printf("@@@@@@@@@@@@SYSTEM: IN_MOVED_FROM\n");
//#endif
			//move file/dir from watched dir to another
			if (event->mask & IN_ISDIR)
			{
				//nasprintf( &moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				sprintf(moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				moved_from_flag = 1;
				save_data(UPDATE_DELETE_DIR, moved_from);
				
//				if ( inotifytools_wd_from_filename(moved_from) == -1 )
//				{
//					//free(moved_from);
//					//moved_from = 0;
//				}
			}
			else
			{
				//here file had move to another place
				//nasprintf( &moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				sprintf(moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				moved_from_flag = 1;
				save_data(UPDATE_DELETE, moved_from);
			}
		} // IN_MOVED_FROM
		else if (event->mask & IN_MOVED_TO)
		{
//#ifdef DBG
			if(debug_flag)
			printf("@@@@@@@@@@@@SYSTEM: IN_MOVED_TO\n");
//#endif
			if (moved_from_flag)
			{
//				static char * new_name;
				
				if (event->mask & IN_ISDIR)
				{				
					//nasprintf(&new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
					sprintf(new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
				}
				else
				{
					//nasprintf(&new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
					sprintf(new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
				}
//#ifdef DBG		
				if(debug_flag)		
				printf("@@@@@@@@@@@@[%s][%s]\n", new_name, moved_from);
//#endif				
				save_data(UPDATE_ADD, new_name);
				inotifytools_replace_filename(moved_from, new_name);
				
				//free(new_name);
//				if(moved_from)
//					free(moved_from);
				moved_from_flag = 0;
			} // moved_from
		}
		else if (event->mask & IN_DELETE_SELF || event->mask & IN_DELETE)
		{
//			static char * del_name;
//#ifdef DBG
			if(debug_flag)
			printf("@@@@@@@@@@@@SYSTEM: IN_DELETE_SELF or IN_DELETE\n");
//#endif		
			if(event->len == 0)
			{
				//directory -> "/mnt/HD_a2/a/test" or "/mnt/HD_a2/a/test/"
				//nasprintf(&del_name, "%s", inotifytools_filename_from_wd(event->wd));
				sprintf(del_name, "%s", inotifytools_filename_from_wd(event->wd));
			}
			else
			{
				//file -> "/mnt/HD_a2/a/test/xxx.mp3"
				//nasprintf(&del_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
				sprintf(del_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
			}
			//printf("del_name:[%s]\n", del_name);
			
			if (event->mask & IN_ISDIR)
			{
				if (del_name[strlen(del_name)-1] == '/')
				{
					//TODO:
					//inotifytools_remove_watch_by_wd_delete(event->wd);
					inotifytools_remove_watch_by_wd(event->wd);
					if(del_name[strlen(del_name)-1] == '/')
					{
						del_name[strlen(del_name)-1] = '\0';
					}
					save_data(UPDATE_DELETE_DIR, del_name);
				}
			}
			else
			{
				save_data(UPDATE_DELETE, del_name);
			}
			
			//free( del_name );
		}
		else
		{
			printf("SYSTEM: Event NO case matched !!\n");
		}

		fflush( NULL );
	}

	// If we weren't trying to listen for this event...
	if ( (events & event->mask) == 0 )
	{
		return EXIT_ERROR;
	}

	return EXIT_OK;
}

