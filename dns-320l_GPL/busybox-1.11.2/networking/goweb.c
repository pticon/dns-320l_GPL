#include <stdio.h>
#include <sys/time.h>
#include <sys/file.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "libbb.h"

//alpha.darren20081229 start
int goweb_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int goweb_main(int argc ATTRIBUTE_UNUSED, char **argv)
//alpha.darren20081229 end
{
	char my_ip[16];
	FILE *fp;
	char buf[80];
	
	if( argc != 3 )
	{
		bb_show_usage();
	}
	
	get_ip_info(argv[1], my_ip);
	printf("get IP = %s\n", my_ip);
	
	fp = fopen(argv[2], "w+");
	if ( fp == NULL )
	{
		return 0;
	}
	
	fputs("<script>\n", fp);
	sprintf(buf, "location.replace(\"http://%s/web/login.asp\")\n", my_ip);
	fputs(buf, fp);
	fputs("</script>", fp);
	
	fclose(fp);
	
	return 0;
}

