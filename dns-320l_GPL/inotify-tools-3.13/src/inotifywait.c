// kate: replace-tabs off; space-indent off;

int main(int argc, char ** argv)
{
	return 0;
}

#if 0
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
//#include <regex.h>

#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

//#include <getopt.h>

#include <sqlite3.h>
#include <signal.h>
#include <pthread.h>
//TODO: change to related path
#include "../../media_type/media_type.h"

sqlite3 	*db_ptr;
int			run_cmd_flag = 1;
int			main_loop_flag = 1;
int			sleep_flag = 1;
pthread_mutex_t SQL_LOCK = PTHREAD_MUTEX_INITIALIZER;

#define EXIT_OK 		0
#define EXIT_ERROR 		1
#define EXIT_TIMEOUT 	2

#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

#define UPDATE_ADD				1
#define UPDATE_DELETE			2
#define UPDATE_DELETE_DIR		3
#define UPDATE_RENAME			4

//#define DBG						1
#define INOTIFY_VERSION			"v1.01.20080917"
#define ITUNE_FOLDER_FILE		"/etc/mt-daapd.conf"
#define UPNPAV_FOLDER_FILE		"/etc/upnpav.conf"
#define RUN_FILE_NAME			"UpdateDB"
//#define SCHEDULER_FILE			"/mnt/HD_a4/.systemfile/ScanMmsSch"
#define DATABASE_PATH_A			"/mnt/HD_a4/.systemfile"
#define DATABASE_PATH_B			"/mnt/HD_b4/.systemfile"
#define DATABASE_PATH_C			"/mnt/HD_c4/.systemfile"
#define DATABASE_PATH_D			"/mnt/HD_d4/.systemfile"
#define DATABASE_PATH_A2		"/mnt/HD_a2/.systemfile"
#define DATABASE_PATH_DEFAULT	"/mnt/HD_a2"
#define DATABASE_FILE_A			"/mnt/HD_a4/.systemfile/inotify.db"
#define DATABASE_FILE_B			"/mnt/HD_b4/.systemfile/inotify.db"
#define DATABASE_FILE_C			"/mnt/HD_c4/.systemfile/inotify.db"
#define DATABASE_FILE_D			"/mnt/HD_d4/.systemfile/inotify.db"
#define DATABASE_FILE_A2		"/mnt/HD_a2/.systemfile/inotify.db"
#define DATABASE_FILE_DEFAULT	"/mnt/HD_a2/inotify.db"
#define SHARE_FOLDER_MAX_LEN	512

#define ITUNE_PATH				1
#define UPNPAV_PATH				2
#define BOTH_PATH				3

#define ITUNE_FILE_EXT			1
#define UPNPAV_FILE_EXT			2
#define BOTH_FILE_EXT			3

#define MAX_PATH_LEN			1024
#define MAX_SQL_LEN				2048

/*--------------------------------------------------------------
* ROUTINE NAME - InitNASMediaFolder
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int InitNASMediaFolder(char *F_iTuneFolder, char *F_UpnpavFolder)
{
	FILE *file;
	char temp[256] = "";
	int get_itune_folder = 0;
	char *p = NULL;
	char *q = NULL;

	memset(F_iTuneFolder, 0, SHARE_FOLDER_MAX_LEN);
	memset(F_UpnpavFolder, 0, SHARE_FOLDER_MAX_LEN);

	//read itune folder info
	if (access(ITUNE_FOLDER_FILE,R_OK) == 0)
	{
		file = fopen(ITUNE_FOLDER_FILE,"r");
		if (file == NULL)
		{
			printf("ERROR: open itune file %s fails\n",ITUNE_FOLDER_FILE);
			F_iTuneFolder[0] = '\0';
			return 0;
		}
	
		while (fgets(temp,256,file) != NULL)
		{
			if (strstr(temp, "mp3_dir"))
			{
				p = strchr(temp, '/');
				if (p)
				{
					strcpy(F_iTuneFolder, p);
					if ((q = strchr(F_iTuneFolder, '\n')) != NULL)
					{
						*q = '/';
						*(q+1) = '\0';
					}
					else
					{
						strcat(F_iTuneFolder, "/");
					}
					get_itune_folder = 1;
				}
				break;
			}
		}

		fclose(file);
		if (get_itune_folder == 0)
		{
			printf("ERROR: get itune shared folder fails\n");
			F_iTuneFolder[0] = '\0';
		}
	}
	else
	{
		F_iTuneFolder[0] = '\0';
		return 0;	
	}
	
	//read upnpav folder info
	if (access(UPNPAV_FOLDER_FILE, R_OK) == 0)
	{
		file = fopen(UPNPAV_FOLDER_FILE,"r");
		if (file == NULL)
		{
			printf("ERROR: open upnp file %s fails\n",UPNPAV_FOLDER_FILE);
			F_UpnpavFolder[0] = '\0';
			return 0;
		}

		if (fgets(temp,256,file) == NULL)
		{
			printf("ERROR: read upnp file %s fails\n",UPNPAV_FOLDER_FILE);
			F_UpnpavFolder[0] = '\0';
		}

		fclose(file);
		p = strchr(temp, '/');
		if (p)
		{
			strcpy(F_UpnpavFolder, p);
		 	if ((q = strchr(F_UpnpavFolder, '\n')) != NULL)
		 	{
				*q = '/';
				*(q+1) = '\0';
			}
			else
			{
				strcat(F_UpnpavFolder, "/");
			}
		}
		else
		{
			printf("ERROR: get upnpav shared folder fails\n");
			F_UpnpavFolder[0] = '\0';
		}
			
	}
	else
	{
		F_UpnpavFolder[0] = '\0';
		return 0;
	}

#ifdef DBG
	printf("UpnpavFolder = [%s]\n",F_UpnpavFolder);
	printf("iTuneFolder = [%s]\n",F_iTuneFolder);
#endif

	if(access(F_UpnpavFolder, F_OK) != 0)
	{
		printf("ERROR: Folder %s not exist!\n", F_UpnpavFolder);
		return 0;
	}
	
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
int CheckFileType(char *full_path, char *F_iTuneFolder, char *F_UpnpavFolder, int updateFlag)
{
	int		i;
	char	*file_ext;
	char	server_path;
	
	//check media type (itune or upnp)
	if(strstr(full_path, F_iTuneFolder)!=NULL && strstr(full_path, F_UpnpavFolder)!=NULL)
	{
		server_path = BOTH_PATH;
	}
	else if(strstr(full_path, F_iTuneFolder)!=NULL)
	{
		server_path = ITUNE_PATH;
	}
	else if(strstr(full_path, F_UpnpavFolder)!=NULL)
	{
		server_path = UPNPAV_PATH;
	}
	else
	{
		printf("ERROR: can't find server type !\n");
		return 0;
	}
	
#ifdef DBG
	printf("server_path [%d]\n", server_path);
#endif
	
	//check dir
	//if(updateFlag == UPDATE_DELETE_DIR || updateFlag == UPDATE_RENAME || updateFlag == UPDATE_ADD)
	if(isdir(full_path) || updateFlag == UPDATE_DELETE_DIR)
	{
		switch(server_path)
		{
			case UPNPAV_PATH:
				return UPNPAV_FILE_EXT;
				break;
				
			case ITUNE_PATH:
				return ITUNE_FILE_EXT;
				break;
				
			case BOTH_PATH:
				return BOTH_FILE_EXT;
				break;
		}
	}
	
	file_ext = GetFileExt(full_path);
	if(file_ext == NULL)
	{
		return 0;
	}

#ifdef DBG	
	printf("ext: [%s][%d]\n", file_ext, server_path);
#endif
	
	for(i=0; i<MIME_TYPE_NUMBER; i++)
	{
		//compare file ext name
		if(strncasecmp(file_ext, CDS_FileType_Map[i].file_ext, strlen(CDS_FileType_Map[i].file_ext)) == 0)
		{
			switch(server_path)
			{
				case UPNPAV_PATH:
					return UPNPAV_FILE_EXT;
					break;
					
				case ITUNE_PATH:
					if(itune_media_type(file_ext) == 1)
					{
						return ITUNE_FILE_EXT;
					}
					break;
					
				case BOTH_PATH:
					if(itune_media_type(file_ext) == 1)
					{
						return BOTH_FILE_EXT;
					}
					else
					{
						return 	UPNPAV_FILE_EXT;
					}
					break;
			}
		}
	}
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - write_wlock
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int write_wlock(int fd)
{
	struct flock	stFLock;
	
	stFLock.l_type = F_WRLCK;
	stFLock.l_start = 0;
	stFLock.l_whence = SEEK_END;
	stFLock.l_len = 0;

	if ( -1 == fcntl( fd, F_SETLKW, &stFLock)  )
	{
		printf("FILE:%s, LINE:%s   Lock File Error.\n", __FILE__, __LINE__ );
		return -1;
	}
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - clear_lock
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int clear_lock(int fd)
{
	struct flock	stFLock;
	
	stFLock.l_type = F_UNLCK;
	stFLock.l_start = 0;
	stFLock.l_whence = SEEK_SET;
	stFLock.l_len = 0;

	if( -1 == fcntl( fd, F_SETLKW, &stFLock ) )
	{
		printf("UNLock File Error!File:%s Line:%s\n",__FILE__,__LINE__);
		return -1;
	}
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - UpdateScheduler
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
//void UpdateScheduler(char *cmd_buf)
//{
//	int	fd;
//	
//	fd = open(SCHEDULER_FILE, O_CREAT|O_APPEND|O_RDWR);
//	if (fd == -1)
//	{
//		printf("ERROR: can't open %s file !\n", SCHEDULER_FILE);
//		return;
//	}
//	
//	write_wlock(fd);
//	write(fd, cmd_buf, strlen(cmd_buf));
//	clear_lock(fd);
//	
//	close(fd);
//}

/*--------------------------------------------------------------
* ROUTINE NAME - reset_database
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int reset_database(void)
{
	char 	*zErrMsg = 0;
	char	database_file[MAX_PATH_LEN];
	
	memset(database_file, 0, MAX_PATH_LEN);
	
	if(access(DATABASE_PATH_A, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_A);
	}
	else if(access(DATABASE_PATH_B, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_B);
	}
	else if(access(DATABASE_PATH_C, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_C);
	}
	else if(access(DATABASE_PATH_D, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_D);
	}
	else if(access(DATABASE_PATH_A2, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_A2);
	}
	else if(access(DATABASE_PATH_DEFAULT, F_OK) == 0)
	{
		strcpy(database_file, DATABASE_FILE_DEFAULT);
	}
	else
	{
		return 0;
	}
	
	//database
	remove(database_file);
	if(sqlite3_open(database_file, &db_ptr))
	{
		exit(1);
	}
	
	//create table
	sqlite3_exec(db_ptr, "create database inotify_db", 0, 0, &zErrMsg);
	sqlite3_exec(db_ptr, "use inotify_db", 0, 0, &zErrMsg);
	sqlite3_exec(db_ptr, "create table inotify_tb (run_cmd varchar(32), path varchar(1024));", 0, 0, &zErrMsg);
	
	return 1;
}

/*--------------------------------------------------------------
* ROUTINE NAME - save_database
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int save_database(int updateFlag, char *FilePath1, char *FilePath2, char *F_iTuneFolder, char *F_UpnpavFolder)
{
	char	para[32];
	char 	inotifycommand[MAX_PATH_LEN];
	char 	sql_cmd[MAX_SQL_LEN];
	
	char 	MediaServerComm[8];
	int		file_ext_type;
	
	int 	nrow = 0, ncolumn = 0;
	char 	**azResult;
	char 	*zErrMsg = 0;

#ifdef DBG
	printf("---------------->Got EVENT(%d) [%s][%s]!!!\n", updateFlag, FilePath1, FilePath2);
#endif
	
	//TODO: check file ext type
	if(updateFlag == UPDATE_RENAME)
	{
		//check path length
		if(strlen(FilePath2) > MAX_PATH_LEN)
		{
			printf("ERROR: File path too long !!!\n");
			return 0;
		}
		file_ext_type = CheckFileType(FilePath2, F_iTuneFolder, F_UpnpavFolder, updateFlag);
	}
	else
	{
		//check path length
		if(strlen(FilePath1) > MAX_PATH_LEN)
		{
			printf("ERROR: File path too long !!!\n");
			return 0;
		}
		file_ext_type = CheckFileType(FilePath1, F_iTuneFolder, F_UpnpavFolder, updateFlag);
	}

#ifdef DBG	
	printf("file_ext_type: %d\n", file_ext_type);
#endif
	if(file_ext_type == 0)
	{
		//check file ext type not error
		return 0;
	}
	
	memset(MediaServerComm, 0, 8);
	switch(file_ext_type)
	{
		case UPNPAV_FILE_EXT:
			strncpy(MediaServerComm, "-M ", 3);
			break;
		
		case ITUNE_FILE_EXT:
			strncpy(MediaServerComm, "-I ", 3);
			break;
			
		case BOTH_FILE_EXT:
			//do nothing
			break;
			
		default:
			break;
	}
	
	
	memset(inotifycommand, 0x00, MAX_PATH_LEN);
	memset(para, 0x00, 32);
	switch(updateFlag)
	{
		case UPDATE_ADD:
			sprintf(para, "%s -C %s", RUN_FILE_NAME, MediaServerComm);
			sprintf(inotifycommand, "%s", FilePath1);
			break;
			
		case UPDATE_DELETE:
			sprintf(para, "%s -D %s", RUN_FILE_NAME, MediaServerComm);
			sprintf(inotifycommand, "%s", FilePath1);
			break;
		
		case UPDATE_DELETE_DIR:
			sprintf(para, "%s -DD %s", RUN_FILE_NAME, MediaServerComm);
			sprintf(inotifycommand, "%s", FilePath1);
			break;
			
		case UPDATE_RENAME:
			//sprintf(inotifycommand, "%s -R %s:%s: :%s:", RUN_FILE_NAME, MediaServerComm, FilePath1, FilePath2);
			break;
	}
	
#ifdef DBG
	printf("===============> %s\n", inotifycommand);
#endif
	
	//write to file
	//UpdateScheduler(inotifycommand);
	
	//insert database
	memset(sql_cmd, 0, MAX_SQL_LEN);
	sprintf(sql_cmd, "insert into inotify_tb values (\"%s\", \"%s\");", para, inotifycommand);
#ifdef DBG
	printf("sql_cmd [%s]\n",sql_cmd);
#endif

	pthread_mutex_lock(&SQL_LOCK);
	sqlite3_exec(db_ptr, sql_cmd, 0, 0, &zErrMsg);
	pthread_mutex_unlock(&SQL_LOCK);
	
	if(zErrMsg != NULL)
	{
		printf("inser [%s]\n", zErrMsg);
		printf("##################################\nsql_cmd [%s]\n",sql_cmd);
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

		//for error contorol, reset database
		reset_database();
		pthread_mutex_lock(&SQL_LOCK);
		sqlite3_exec(db_ptr, sql_cmd, 0, 0, &zErrMsg);
		pthread_mutex_unlock(&SQL_LOCK);
	}
	
	return 1;
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
	sqlite3_close(db_ptr);
	
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
void *run_cmd_thread(void *arg)
{
	int 	nrow = 0, ncolumn = 0;
	char 	**azResult;
	char 	*zErrMsg = 0;
	int		i;
	char	para[32];
	char	inotifycommand[MAX_PATH_LEN];
	char	run_cmd[MAX_PATH_LEN];
	char 	sql_cmd[MAX_SQL_LEN];
	int		error_count;
	
	run_cmd_flag = 1;
	error_count = 0;
	
	while(run_cmd_flag == 1)
	{
		//check any item in database
		pthread_mutex_lock(&SQL_LOCK);
		sqlite3_get_table(db_ptr, "select run_cmd, path from inotify_tb limit 0,1;", &azResult, &nrow, &ncolumn, &zErrMsg);
		pthread_mutex_unlock(&SQL_LOCK);
		
		if(nrow == 0)
		{
#ifdef DBG
		printf("****************  database is empty !!!\n");
#endif

			sleep_flag = 1;

			//database is empty
			for(i=0; i<10 ; ++i)
			{
				if(sleep_flag == 0)
				{
					break;
				}
				sleep(1);
			}
			continue;
		}
		
//		memset(sql_cmd, 0, MAX_SQL_LEN);
//		if(azResult[ncolumn] == NULL)
//		{
//			//for database error control
//			sprintf(sql_cmd, "delete from inotify_tb where run_cmd=\"%s\";", azResult[ncolumn]);
//#ifdef DBG
//			printf("sql_cmd [%s]\n",sql_cmd);
//#endif
//			sqlite3_exec(db_ptr, sql_cmd, 0, 0, &zErrMsg);
//			error_count++;
//			
//			if(error_count >= 10)
//			{
//				//we clear database
//				printf("ERROR: delete all from database\n");
//				sqlite3_exec(db_ptr, "delete from inotify_tb;", 0, 0, &zErrMsg);
//				error_count = 0;
//			}
//			sleep(5);
//			continue;
//		}
		
		error_count = 0;
		memset(para, 0, 32);
		strncpy(para, azResult[ncolumn], strlen(azResult[ncolumn]));
		memset(inotifycommand, 0, MAX_PATH_LEN);
		strncpy(inotifycommand, azResult[ncolumn+1], strlen(azResult[ncolumn+1]));
		
		
		//delete data in database
		sprintf(sql_cmd, "delete from inotify_tb where path=\"%s\";", inotifycommand);
#ifdef DBG
		printf("sql_cmd [%s]\n",sql_cmd);
#endif

		pthread_mutex_lock(&SQL_LOCK);
		sqlite3_exec(db_ptr, sql_cmd, 0, 0, &zErrMsg);
		pthread_mutex_unlock(&SQL_LOCK);
		
		//run cmd here
		memset(run_cmd, 0, MAX_PATH_LEN);
		sprintf(run_cmd, "%s\"%s\"", para, inotifycommand);
		
		#ifdef DBG
		printf("##############################should run [%s][%s]\n", run_cmd, zErrMsg);
#endif
		
		system(run_cmd);
		//TODO: add sleep ?
		//sleep(1);
		
#ifdef DBG
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@Done !!!\n");
#endif
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
	char		*moved_from = 0;
	FileList 	list;
	char		*path[2];
	
	char		F_iTuneFolder[SHARE_FOLDER_MAX_LEN];
	char		F_UpnpavFolder[SHARE_FOLDER_MAX_LEN];
	pthread_t	run_cmd;
	
	printf("Version: %s\n", INOTIFY_VERSION);

	//database
	if(reset_database()==0)
	{
		printf("ERROR: Create database error !\n");
		exit(1);
	}
	
	//catch sigterm
	signal(SIGTERM, sig_kill);
	signal(SIGSEGV, sig_kill);
	
	//check /dev/inotify
	if (access("/dev/inotify", F_OK) != 0)
	{
		printf("Create /dev/inotify !\n");
		system("mknod /dev/inotify c 10 63");
	}
	
	//get default itune&upnp setting path
	if(InitNASMediaFolder(F_iTuneFolder, F_UpnpavFolder) != 1)
	{
		printf("ERROR: Can't find iTune/Upnp setting Path !\n");
		exit(1);
	}
	
	//we set default parameter
	//recursive = 1;

	if ( !inotifytools_initialize() )
	{
		printf("ERROR: inotify init fail!\n");
		return EXIT_ERROR;
	}

	// Attempt to watch file
	// If events is still 0, make it all events.
	events = (IN_MOVED_FROM|IN_MOVED_TO|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVE_SELF);
	
	//TODO:for test
//	strcpy(F_iTuneFolder, "/mnt/HD_a2/a/");
//	strcpy(F_UpnpavFolder, "/mnt/HD_a2/b/");
//	printf("-->UpnpavFolder = [%s]\n",F_UpnpavFolder);
//	printf("-->iTuneFolder = [%s]\n",F_iTuneFolder);
	//for test end
	
	path[0] = F_iTuneFolder;
	path[1] = F_UpnpavFolder;
	list = construct_path_list( 2, path, NULL );

	if (0 == list.watch_files[0])
	{
		printf("No files specified to watch!\n");
		return EXIT_ERROR;
	}

	// now watch files
	for(i = 0; list.watch_files[i]; ++i )
	{
		char const *this_file = list.watch_files[i];
		
#ifdef DBG
		printf("this_file = [%d: %s]\n", i, this_file);
#endif

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

#ifdef DBG
		printf("Watches established.\n" );
#endif

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
				return EXIT_TIMEOUT;
			}
			else
			{
				printf("%s\n", strerror(inotifytools_error()));
				return EXIT_ERROR;
			}
		}

		// if we last had MOVED_FROM and don't currently have MOVED_TO,
		// moved_from file must have been moved outside of tree - so unwatch it.
		if (moved_from && !(event->mask & IN_MOVED_TO))
		{
			if (!inotifytools_remove_watch_by_filename(moved_from))
			{
				printf("Error removing watch on %s: %s\n", moved_from, strerror(inotifytools_error()));
			}
			free(moved_from);
			moved_from = 0;
		}

#ifdef DBG
		printf("SYSTEM: event mask:[%x]\n", event->mask);
#endif
		sleep_flag = 0;
		if ((event->mask & IN_CREATE) || (!moved_from && (event->mask & IN_MOVED_TO)))
		{
			// New file - if it is a directory, watch it
			static char * new_file;
#ifdef DBG			
			printf("SYSTEM: IN_CREATE\n");
#endif		
			nasprintf( &new_file, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
			
			if(moved_from)
			{
				//printf("^^^^^^^^^^[%s][%s]\n", moved_from, new_file);
				inotifytools_replace_filename(moved_from, new_file);
				free(moved_from);
				moved_from = 0;
			}
			
			if (isdir(new_file) && !inotifytools_watch_recursively(new_file, events))
			{
				printf("Couldn't watch new directory %s: %s\n", new_file, strerror(inotifytools_error()));
			}

			save_database(UPDATE_ADD, new_file, '\0', F_iTuneFolder, F_UpnpavFolder);
			
			free(new_file);
		} // IN_CREATE
		else if (event->mask & IN_MOVED_FROM)
		{
#ifdef DBG
			printf("SYSTEM: IN_MOVED_FROM\n");
#endif
			//move file/dir from watched dir to another
			if (event->mask & IN_ISDIR)
			{
				nasprintf( &moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				save_database(UPDATE_DELETE_DIR, moved_from, '\0', F_iTuneFolder, F_UpnpavFolder);
				
				if ( inotifytools_wd_from_filename(moved_from) == -1 )
				{
					//free(moved_from);
					//moved_from = 0;
				}
			}
			else
			{
				//here file had move to another place
				nasprintf( &moved_from, "%s%s", inotifytools_filename_from_wd( event->wd ), event->name );
				save_database(UPDATE_DELETE, moved_from, '\0', F_iTuneFolder, F_UpnpavFolder);
			}
		} // IN_MOVED_FROM
		else if (event->mask & IN_MOVED_TO)
		{
#ifdef DBG
			printf("SYSTEM: IN_MOVED_TO\n");
#endif	
			if (moved_from)
			{
				static char * new_name;
				
				if (event->mask & IN_ISDIR)
				{				
					nasprintf(&new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
				}
				else
				{
					nasprintf(&new_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
				}
#ifdef DBG				
				printf("^^^^^^^^^^[%s][%s]\n", new_name, moved_from);
#endif				
				save_database(UPDATE_ADD, new_name, '\0', F_iTuneFolder, F_UpnpavFolder);
				inotifytools_replace_filename(moved_from, new_name);
				
				free(new_name);
				free(moved_from);
				moved_from = 0;
			} // moved_from
		}
		else if (event->mask & IN_DELETE_SELF || event->mask & IN_DELETE)
		{
			static char * del_name;
#ifdef DBG		
			printf("SYSTEM: IN_DELETE_SELF or IN_DELETE\n");
#endif		
			if(event->len == 0)
			{
				//directory -> "/mnt/HD_a2/a/test" or "/mnt/HD_a2/a/test/"
				nasprintf(&del_name, "%s", inotifytools_filename_from_wd(event->wd));
			}
			else
			{
				//file -> "/mnt/HD_a2/a/test/xxx.mp3"
				nasprintf(&del_name, "%s%s", inotifytools_filename_from_wd(event->wd), event->name);
			}
			//printf("del_name:[%s]\n", del_name);
			
			if (event->mask & IN_ISDIR)
			{
				if (del_name[strlen(del_name)-1] == '/')
				{
					if(strcmp(F_iTuneFolder, del_name)==0
						|| strcmp(F_UpnpavFolder, del_name)==0 )
					{
						//inotifytools_remove_watch_by_wd_delete(event->wd);
						inotifytools_remove_watch_by_wd(event->wd);
						
						printf("ERROR: the watched directory is not exist! Stop inotify Daemon.\n");
						
						//stop inotify daemon
						free( del_name );
						main_loop_flag = 0;
						run_cmd_flag = 0;
						sqlite3_close(db_ptr);
						
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
					save_database(UPDATE_DELETE_DIR, del_name, '\0', F_iTuneFolder, F_UpnpavFolder);
				}
			}
			else
			{
				save_database(UPDATE_DELETE, del_name, '\0', F_iTuneFolder, F_UpnpavFolder);
			}
			
			free( del_name );
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
#endif
