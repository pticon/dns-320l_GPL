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
//#include "../../media_type/media_type.h"
#include "filter.h"

#define SHARE_FOLDER_MAX_LEN	512
char		F_iTuneFolder[SHARE_FOLDER_MAX_LEN];
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
#define INO_ITUNE_VERSION		"inotify itune v1.06.20090602"
#define ITUNE_FOLDER_FILE		"/etc/mt-daapd.conf"
#define UPNPAV_FOLDER_FILE		"/etc/upnpav.conf"
#define RUN_FILE_NAME			"UpdateDB"

#define STR_A2_NAS_PROG			"/mnt/HD/HD_a2/Nas_Prog"
#define STR_B2_NAS_PROG			"/mnt/HD/HD_b2/Nas_Prog"

#define MAX_PATH_LEN			1024


//royc 20090330
int				input_count=0;

#define			EVENT_MAXNUM			12		//12 event
#define			EVENT_DELAY_TIME		10		//10 sec

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
	FILE *file;
	char temp[256];

	memset(F_iTuneFolder, 0, SHARE_FOLDER_MAX_LEN);
	memset(temp, 0, 256);

	//read itune folder info
	file = fopen(ITUNE_FOLDER_FILE,"r");
	if (file == NULL)
	{
		printf("ERROR: open itune file %s fails\n", ITUNE_FOLDER_FILE);
		return 0;
	}

	while (fgets(temp, 256, file) != NULL)
	{
		if(strstr(temp, "mp3_dir"))
		{
			//sscanf(temp, "mp3_dir %s", F_iTuneFolder);
			remove_0D_0A(temp);
			strcpy(F_iTuneFolder, temp+strlen("mp3_dir "));
			break;
		}
	}

	fclose(file);

	printf("iTuneFolder = [%s]\n",F_iTuneFolder);
	
	if(access(F_iTuneFolder, F_OK) != 0)
	{
		printf("ERROR: Folder %s not exist!\n", F_iTuneFolder);
		return 0;
	}

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
int itune_media_type(char *str)
{
	if(strncasecmp(str, ".mp3", 4) == 0
		|| strncasecmp(str, ".m4a", 4) == 0
		|| strncasecmp(str, ".m4p", 4) == 0 )
	{
		return 1;
	}
	
	return 0;
}

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
	
	//dont check
	if(strstr(full_path, "/mnt/HD_a4")!=NULL
		|| strstr(full_path, "/mnt/HD_b4")!=NULL
		|| strstr(full_path, "/mnt/HD_c4")!=NULL
		|| strstr(full_path, "/mnt/HD_d4")!=NULL)
	{
#ifdef DBG
		printf("-----------------IGNORE: %s\n", full_path);
#endif
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

#ifdef DBG	
	printf("-----------------ext: [%s]\n", file_ext);
#endif
	
	//compare file ext name for itune
	if(itune_media_type(file_ext) == 1)
	{
		return 1;
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
#ifdef DBG
	printf("---------------->Got EVENT(%d) [%s]!!!\n", updateFlag, FilePath);
#endif
	
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
	
	//count ++
	input_count ++;
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
	
	exit(0);
	return;
}

///*-----------------------------------------------------------------
//* ROUTINE NAME - run_cmd_thread         
//*------------------------------------------------------------------
//* FUNCTION: 
//*
//* INPUT:
//* OUTPUT:
//* RETURN:
//*                                 
//* NOTE:
//*----------------------------------------------------------------*/
//void restart_itune_server(void)
//{
//	FILE	*fp;
//	char	tmp[256];
//	char	line[256];
//	char	buf[256];
//	
//	system("kill -9 `pidof mt-daapd` 2>/dev/null");
//	
//	//get itune db directory form /etc/mt-daapd.conf
//	fp = fopen(ITUNE_FOLDER_FILE,"r");
//	
//	if(fp==NULL)
//		return;
//	
//	memset(line, 256, 0);
//	memset(tmp, 256, 0);
//	while((fgets(line,256,fp)) != NULL)
//	{
//		if (strstr(line,"tmp_dir")!=0 && strstr(line,"#")==0)
//		{
//			sscanf(line, "tmp_dir %s", tmp);
//			break;
//		}
//		memset(line, 256, 0);
//	}
//	fclose(fp);
//	
//	//when get null string, return
//	if(strlen(tmp)==0)
//		return;
//		
//	//remove file
//	memset(buf, 256, 0);
//	sprintf(buf,"rm %s/.scan_songs_done 2>/dev/null", tmp);
//	system(buf);
//	
//	memset(buf, 256, 0);
//	sprintf(buf,"rm %s/.mt-daapd/.songs.gdb 2>/dev/null",tmp);
//	system(buf);
//	
//	//start itune server
//	system("mt-daapd -i &");
//}

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
	time_t			old_time;
	time_t			now_time;
	int				wait_flag;
	
	int				input_tmp;
	
	run_cmd_flag = 1;
	wait_flag = 0;
	input_tmp = 0;
	
	while(run_cmd_flag == 1)
	{
		input_tmp = input_count;
		
		//check any item in database
		if( input_tmp == 0 )
		{
#ifdef DBG
		printf("###############inotify event is empty !!!\n");
#endif
			sleep_flag = 1;
        	
			//link list is empty
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
		
		//input_count != 0
		if(wait_flag == 1)
		{
			time(&now_time);
	
			if(now_time - old_time >= EVENT_DELAY_TIME || input_tmp >= EVENT_MAXNUM)
			{
#ifdef DBG
			printf("now_time-old_time >= EVENT_DELAY_TIME [%d][%d][%d]!!!\n", now_time, old_time, input_tmp);
#endif		
				wait_flag = 0;
				input_count -= input_tmp;
				input_tmp = 0;
				system("kill -HUP `pidof mt-daapd`");
				
				continue;
			}
		}
		
		if(input_tmp >= EVENT_MAXNUM)
		{
#ifdef DBG
			printf("input_tmp >= EVENT_MAXNUM !!!\n");
#endif
			wait_flag = 0;
			input_count -= input_tmp;
			input_tmp = 0;
			system("kill -HUP `pidof mt-daapd`");
		}
		else if(wait_flag != 1)
		{
#ifdef DBG
			printf("wait_flag [%d][%d][%d]!!!\n", now_time, old_time, input_tmp);
#endif	
			wait_flag = 1;
			time(&now_time);
			old_time = now_time;
		}
		
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
	int 		i;
	struct		inotify_event *event;
	char		moved_from[MAX_PATH_LEN];
	int			moved_from_flag = 0;
	FileList 	list;
	char		*path[2];
	
	char		new_name[MAX_PATH_LEN];
	char		new_file[MAX_PATH_LEN];
	char		del_name[MAX_PATH_LEN];
	
	
	pthread_t	run_cmd;
	struct utsname 	name;
	
	printf("Version: %s\n", INO_ITUNE_VERSION);

	//royc 20090330
	//init value
	input_count = 0;
	//royc end

	//catch sigterm
	signal(SIGTERM, sig_kill);
	signal(SIGSEGV, sig_kill);

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

	path[0] = F_iTuneFolder;
	list = construct_path_list( 1, path, NULL );

	if (0 == list.watch_files[0])
	{
		printf("No files specified to watch!\n");
		return EXIT_ERROR;
	}

	// now watch files
	for(i = 0; list.watch_files[i]; ++i )
	{
		char const *this_file = list.watch_files[i];
		
		printf("this_file = [%d: %s]\n", i, this_file);

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
	main_loop_flag = 1;
	while(main_loop_flag == 1)
	{
		//0 for non-blocking, negative the function will block until an event occurs
		event = inotifytools_next_event(0);
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

#ifdef DBG
		printf("@@@@@@@@@@@@SYSTEM: event mask:[%x]\n", event->mask);
#endif
		sleep_flag = 0;
		if ((event->mask & IN_CREATE) || (!moved_from_flag && (event->mask & IN_MOVED_TO)))
		{
			// New file - if it is a directory, watch it
//			static char * new_file;
#ifdef DBG			
			printf("@@@@@@@@@@@@SYSTEM: IN_CREATE\n");
#endif		
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
#ifdef DBG
			printf("@@@@@@@@@@@@SYSTEM: IN_CLOSE_WRITE\n");
#endif	
			sprintf( new_file, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
			
			if(isdir(new_file))
			{
				continue;
			}
			
#ifdef DBG
			printf("###[%s]\n", new_file);
#endif				
			//check file exist or not
			usleep(100000);
			if(access(new_file, F_OK) == 0)
			{
				save_data(UPDATE_ADD, new_file);
			}
		}
		else if (event->mask & IN_MOVED_FROM)
		{
#ifdef DBG
			printf("@@@@@@@@@@@@SYSTEM: IN_MOVED_FROM\n");
#endif
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
#ifdef DBG
			printf("@@@@@@@@@@@@SYSTEM: IN_MOVED_TO\n");
#endif	
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
#ifdef DBG				
				printf("@@@@@@@@@@@@[%s][%s]\n", new_name, moved_from);
#endif				
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
#ifdef DBG		
			printf("@@@@@@@@@@@@SYSTEM: IN_DELETE_SELF or IN_DELETE\n");
#endif		
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
					if(strcmp(F_iTuneFolder, del_name)==0)
					{
						//inotifytools_remove_watch_by_wd_delete(event->wd);
						inotifytools_remove_watch_by_wd(event->wd);
						printf("ERROR: the watched directory is not exist! Stop inotify Daemon.\n");
						
						//stop inotify daemon
						//free( del_name );
						main_loop_flag = 0;
						run_cmd_flag = 0;
						
						exit(0);
						continue;
					}
					
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

