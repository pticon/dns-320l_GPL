#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void auto_create_key(void)
{
    system("/usr/bin/ssh-keygen -t dsa -f /tmp/id_dsa -C \'\' -N \'\'");
}

int check_key_status(const char *fn)
{
	int ret ;
    struct stat st;
    ret = stat(fn, &st);
    if (ret != 0)
    	return -1;		
    return 0;
}

int main()
{
	int ret ;
		
	auto_create_key();
	ret = check_key_status("/tmp/id_dsa.pub");
	if (ret){
		printf("We have not generated any dsa key.\n");
		return -1;
	}
	ret = check_key_status("/home/admin/.ssh/");
	if (ret){
		system("mkdir -p /home/admin/.ssh");	
	}
	system ("mv /tmp/id_dsa.pub /tmp/id_dsa /home/admin/.ssh/");
		
	printf("Normal Exit Rsync_ckey.\n");
	return -1;		
}




