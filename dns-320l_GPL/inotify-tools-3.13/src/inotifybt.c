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
#include <limits.h>
#include <dirent.h>
//#include <assert.h>
//#include <regex.h>

#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

//#define DBG						1
#define INOTIFYBT_VERSION		"v1.00.20080625"
#define BT_FOLDER_FILE			"/mnt/HD_a2/Nas_Prog/BT/btsettings.txt"
#define SHARE_FOLDER_MAX_LEN	512
#define TORRENT_NAME_LEN	256			// Patrick.alpha 20090106 ++
#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

#define EXIT_OK 		0
#define EXIT_ERROR 		1
#define EXIT_TIMEOUT 	2

#define UPDATE_ADD				1
#define UPDATE_DELETE			2
#define UPDATE_DELETE_DIR		3
#define UPDATE_RENAME			4

/*--------------------------------------------------------------
* ROUTINE NAME - get_BT_save_path
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int get_BT_save_path(char *path)
{
	FILE *file;
	char *p = NULL;
	char *q = NULL;
	char temp[SHARE_FOLDER_MAX_LEN] = "";
	char config_path[64];
	
	if(scan_bt_dir(config_path)==0) 
		printf("BT setting path: %s\n", config_path);
		
	if(access(config_path ,R_OK)==0) 
	//if(access(BT_FOLDER_FILE, R_OK) == 0)
	{
		file = fopen(config_path,"r");
		//file = fopen(BT_FOLDER_FILE,"r");
		if(file == NULL)
		{
			return -1;
		}
		
		while (fgets(temp, SHARE_FOLDER_MAX_LEN, file) != NULL)
		{
			if (strstr(temp, "dir_active"))
			{
				p = strchr(temp, '/');
				if(p)
				{
					strcpy(path, p);
					
					if ((q = strrchr(path, '/')) != NULL)
					{
						*(q+1) = '\0';
					}
					
					fclose(file);
					return 0;
				}
				else
				{
					fclose(file);
					return -1;
				}
			}
		}
		
		fclose(file);
		return -1;
	}
	else
	{
		return -1;
	}

}

/*--------------------------------------------------------------
* ROUTINE NAME - filter_dir
*---------------------------------------------------------------
* FUNCTION:
*
* INPUT:
*
* OUTPUT:   None
* RETURN:   None
* NOTE:
*-------------------------------------------------------------*/
int filter_dir(const struct dirent *dir)
{
        //filter "." and ".." directory
        if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0 )
                return 0;
        else
                return 1;
}

/*--------------------------------------------------------------
* ROUTINE NAME - scan_bt_dir
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int scan_bt_dir(char* bt_config_path)
{
	char config_path[64];
	char command[128];
	FILE *fptr=NULL;
	char *tmp_config = "/tmp/bt_conf_path";
	//struct dirent *dirptr;
	struct dirent **namelist;
	
	int count=0, i=0;
	memset(config_path, 0x00, sizeof(config_path));
	
	
	if(access(tmp_config,F_OK)!=0) {
		memset(command, 0x00, sizeof(command));
		sprintf(command, "find /mnt -type f -name \"btsettings.txt\" > %s", tmp_config);		// Here btsettings.dat can be replace to bt setting config file.
		system(command);
	}
	
	if(access(tmp_config,F_OK)==0) {
		fptr=fopen(tmp_config, "r");
		fscanf(fptr, "%[^\n]*%c", config_path);
		//fgets(config_path, sizeof(config_path), fptr);	// fgets will include '\n' in buffer... it will make access fail.
		printf("BT config path: %s\n", config_path);
		strcpy(bt_config_path, config_path);
		fclose(fptr);
		//unlink(tmp_config);	
	}
	
	
	
#if 0
	if((count=scandir("/mnt", &namelist, filter_dir, alphasort))<0) 
		perror("scandir");
	else {
		for(i=0;i<count;i++){
			printf("Name: %s\n", namelist[i]->d_name);
			if(strstr(namelist[i]->d_name, "btsettings.txt")!=0) {
				realpath(namelist[i]->d_name, config_path);
				printf("Resolved path: %s\n", config_path);
				strcpy(bt_config_path, config_path);
			}
			free(namelist[i]);
		}
		free(namelist);
	}

#endif

	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - hit_run
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int hit_run(int updateFlag, char *FilePath1, char *FilePath2, char *F_btTuneFolder)
{
	char		run_cmd[SHARE_FOLDER_MAX_LEN];
	
	memset(run_cmd, 0, SHARE_FOLDER_MAX_LEN);
	sprintf(run_cmd, "chmod 777 -R %s", F_btTuneFolder);
	
	printf("[%s]\n", run_cmd);
	
	//TODO
	system(run_cmd);
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - Email_notice
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/

int Email_notice(int updateFlag, char *FilePath1, char *FilePath2, char* torrent_name)
{
	char email_cmd[SHARE_FOLDER_MAX_LEN];
	
	memset(email_cmd, 0x00, SHARE_FOLDER_MAX_LEN);
	sprintf(email_cmd, "email -m 1024 -c \"%s\"", torrent_name);
	
	system(email_cmd);
	
	
	return 0;
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
	char		*moved_from = 0;
	FileList 	list;
	char		*path[1];
	char		F_btTuneFolder[SHARE_FOLDER_MAX_LEN];
	char		torrent_name[TORRENT_NAME_LEN];
	char		*p=NULL;
	
	printf("Version: %s\n", INOTIFYBT_VERSION);
	
	//check /dev/inotify
	if (access("/dev/inotify", F_OK) != 0)
	{
		printf("Create /dev/inotify !\n");
		system("mknod /dev/inotify c 10 63");
	}
	
	//get BT watched folder
	memset(F_btTuneFolder, 0, SHARE_FOLDER_MAX_LEN);
	//strcpy(F_btTuneFolder, "/mnt/HD_a2/BT/");
	get_BT_save_path(F_btTuneFolder);
	
	if ( !inotifytools_initialize() )
	{
		printf("ERROR: inotify init fail!\n");
		return EXIT_ERROR;
	}

	// Attempt to watch file
	// If events is still 0, make it all events.
	events = (IN_MOVED_FROM|IN_MOVED_TO|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVE_SELF);
	
	path[0] = F_btTuneFolder;
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
	
	//main loop
	while(1)
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

			hit_run(UPDATE_ADD, new_file, '\0', F_btTuneFolder);
			
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
				hit_run(UPDATE_DELETE_DIR, moved_from, '\0', F_btTuneFolder);
				
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
				hit_run(UPDATE_DELETE, moved_from, '\0', F_btTuneFolder);
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
				hit_run(UPDATE_ADD, new_name, '\0', F_btTuneFolder);
				
				/* Patrick.alpha 20090105 ++ */
				if((strstr(moved_from, "incomplete")!=0) && (strstr(new_name, "complete")!=0)) {
					if((p=strrchr(moved_from, '/'))!=NULL) {
						memset(torrent_name, 0x00, TORRENT_NAME_LEN);
						strcpy(torrent_name, (p+1));
						#ifdef DBG
						printf("Complete torrent name: %s\n", torrent_name);
						#endif
						Email_notice(UPDATE_ADD, new_name, '\0', torrent_name);
					}
					
				}
				/* Patrick.alpha 20090105 -- */
				
				
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
					if(strcmp(F_btTuneFolder, del_name)==0)
					{
						//inotifytools_remove_watch_by_wd_delete(event->wd);
						inotifytools_remove_watch_by_wd(event->wd);
						
						printf("ERROR: the watched directory is not exist! Stop inotify Daemon.\n");
						
						//stop inotify daemon
						free( del_name );
						
						exit(0);
						continue;
					}
					
					//TODO:
					//inotifytools_remove_watch_by_wd_delete(event->wd);
					inotifytools_remove_watch_by_wd(event->wd);
					hit_run(UPDATE_DELETE_DIR, del_name, '\0', F_btTuneFolder);
				}
			}
			else
			{
				hit_run(UPDATE_DELETE, del_name, '\0', F_btTuneFolder);
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
