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
#define INOTIFYBT_VERSION		"v1.00.20090923"
#define AMULE_CNF_FILE			"/mnt/HD/HD_a2/Nas_Prog/aMule/.aMule/amule.conf"
#define SHARE_FOLDER_MAX_LEN	512
#define MAX_FILE_NAME	256			// Patrick.alpha 20090106 ++
#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

#define EXIT_OK 		0
#define EXIT_ERROR 		1
#define EXIT_TIMEOUT 	2

#define UPDATE_ADD				1
#define UPDATE_DELETE			2
#define UPDATE_DELETE_DIR		3
#define UPDATE_RENAME			4


int read_xml_path(char* install_path) {
	FILE *fptr=NULL;
	char buf[64], *cptr=NULL ,*eptr=NULL;
	int idx=0, flag=0;
	
	fptr=fopen("/tmp/apkg.xml", "r");
	if(fptr==NULL)
		return -1;
	else {
		memset(buf, 0x00, sizeof(buf));
		while((fgets(buf, sizeof(buf), fptr))!=NULL)  {
			if((cptr=strchr(buf, '"')+1)!=NULL&&(eptr=strrchr(buf, '"'))!=NULL)
				idx=atoi(cptr);
			if(strstr(buf, "<name>aMule</name>")) 
				flag=1;
			if(idx|flag){
				if((cptr=strstr(buf, "<path>"))!=NULL) {
					cptr+=strlen("<path>");
					strncpy(install_path, cptr, 13);
					*(install_path+strlen(install_path)+1)='\0';
					flag=0;
				}
			}
		}
		fclose(fptr);
	}
	return 0;
}

int get_amule_monitor_dir(char *monitordir){
	FILE *fptr;
	char prefix[16], path[64], buf[256], *bptr=NULL;
	
	memset(prefix, 0x00, sizeof(prefix));
	if(read_xml_path(prefix)==0) {
		memset(path, 0x00, sizeof(path));
		snprintf(path, sizeof(path), "%s/Nas_Prog/aMule/.aMule/amule.conf", prefix);
		if(access(path, R_OK)==0) {
			if((fptr=fopen(path, "r"))!=NULL) {
				memset(buf, 0x00, sizeof(buf));
				while(fgets(buf, sizeof(buf), fptr)!=NULL) {
					if((bptr=strstr(buf, "IncomingDir"))!=NULL) {
						bptr+=strlen("IncomingDir=");
						strcpy(monitordir, bptr);
						if(strrchr(monitordir, '\n')) {
							*(monitordir+strlen(monitordir)-1)='/';	// Must add '/' to end of path, or it will not recongnize.
							*(monitordir+strlen(monitordir))='\0';
							printf("amule monitor folder: %s\n", monitordir);
						}
					}
				}
			}
			else {
				printf("Read error\n");
				return -1;
		}
		}
		else {
			printf("No this file\n");
			return -1;
	}
	}
	else {
		printf("read xml error\n");
		return -1;
	}
	
	return 0;
}


int filter_dir(const struct dirent *dir)
{
	if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0 )
		return 0;
	else
		return 1;
}



/*--------------------------------------------------------------
* ROUTINE NAME - change_mode
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   SerialNumber
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/
int change_mode(int updateFlag, char *FilePath1, char *FilePath2, char *amule_folder)
{
	char run_cmd[SHARE_FOLDER_MAX_LEN];
	
	memset(run_cmd, 0, SHARE_FOLDER_MAX_LEN);
	sprintf(run_cmd, "chmod 777 -R %s", amule_folder);
	printf("[%s]\n", run_cmd);
	system(run_cmd);//TODO
	
	return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - Email_notify
*---------------------------------------------------------------
* FUNCTION:
* INPUT:	None		
* OUTPUT:   
* RETURN:   
* NOTE:
*-------------------------------------------------------------*/

int Email_notify(int updateFlag, char *FilePath1, char *FilePath2, char* amule_file)
{
	char email_cmd[SHARE_FOLDER_MAX_LEN];
	
	memset(email_cmd, 0x00, SHARE_FOLDER_MAX_LEN);
	sprintf(email_cmd, "email -m 1024 -c \"%s\"", amule_file);
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
	char		amule_folder[256];
	char		amule_file[MAX_FILE_NAME];
	char		*p=NULL;
	
	printf("--->Version: %s\n", INOTIFYBT_VERSION);
	
	//check /dev/inotify
	if (access("/dev/inotify", F_OK) != 0)
	{
		printf("Create /dev/inotify !\n");
		system("mknod /dev/inotify c 10 63");
	}
	
	memset(amule_folder, 0, 256);
	get_amule_monitor_dir(amule_folder);
	if ( !inotifytools_initialize() )
	{
		printf("ERROR: inotify init fail!\n");
		return EXIT_ERROR;
	}

	// Attempt to watch file
	// If events is still 0, make it all events.
	events = (IN_MOVED_FROM|IN_MOVED_TO|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVE_SELF);
	
	path[0] = amule_folder;
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

			change_mode(UPDATE_ADD, new_file, '\0', amule_folder);
			
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
				change_mode(UPDATE_DELETE_DIR, moved_from, '\0', amule_folder);
				
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
				change_mode(UPDATE_DELETE, moved_from, '\0', amule_folder);
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
				change_mode(UPDATE_ADD, new_name, '\0', amule_folder);
				
				/* Patrick.alpha 20090105 ++ */
				if((strstr(moved_from, "TempDir")!=0) && (strstr(new_name, "IncomingDir")!=0)) {
					if((p=strrchr(moved_from, '/'))!=NULL) {
						memset(amule_file, 0x00, MAX_FILE_NAME);
						strcpy(amule_file, (p+1));
						#ifdef DBG
						printf("Complete file name: %s\n", amule_file);
						#endif
						Email_notify(UPDATE_ADD, new_name, '\0', amule_file);
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
					if(strcmp(amule_folder, del_name)==0)
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
					change_mode(UPDATE_DELETE_DIR, del_name, '\0', amule_folder);
				}
			}
			else
			{
				change_mode(UPDATE_DELETE, del_name, '\0', amule_folder);
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
