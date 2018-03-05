/* ============================================================================
 * Copyright (C) 1999-2000 Angus Mackay. All rights reserved; 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */

/*
 * cache_file.c
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#if HAVE_ERRNO_H
#  include <errno.h>
#endif

#include <cache_file.h>

#if HAVE_STRERROR
extern int errno;
#  define error_string strerror(errno)
#elif HAVE_SYS_ERRLIST
extern const char *const sys_errlist[];
extern int errno;
#  define error_string (sys_errlist[errno])
#else
#  define error_string "error message not found"
#endif

#ifdef DEBUG
#define dprintf(x) if( options & OPT_DEBUG ) \
{ \
  fprintf(stderr, "%s,%d: ", __FILE__, __LINE__); \
    fprintf x; \
}
#else
#  define dprintf(x)
#endif
#if HAVE_STRERROR
extern int errno;
#  define error_string strerror(errno)
#elif HAVE_SYS_ERRLIST
extern const char *const sys_errlist[];
extern int errno;
#  define error_string (sys_errlist[errno])
#else
#  define error_string "error message not found"
#endif

#define OPT_DEBUG       0x0001

extern int options;

#include <libxmldbc.h>		//+
#include <xml_tag.h>		//+
//#define XML_DDNS_IP 		"/network/ddns/ip"			//+
//#define XML_DDNS_TIMESTAMP	"/network/ddns/timestamp"	//+

int read_cache_file(char *file, time_t *date, char **ipaddr)
{
  FILE *fp = NULL;
  char buf[BUFSIZ+1];
  char *p;
  char *datestr;
  char *ipstr;
#if HAVE_STAT
  struct stat st;
#endif

  // safety first
  buf[BUFSIZ] = '\0';

  // indicate failure
  *date = 0;
  *ipaddr = NULL;

#if HAVE_STAT
  if(stat(file, &st) != 0)
  {
    if(errno == ENOENT)
    {
      return(0);
    }
    return(-1);
  }
#endif

  if((fp=fopen(file, "r")) == NULL)
  {
    return(-1);
  }

  if(fgets(buf, BUFSIZ, fp) != NULL)
  {

    /* chomp new line */
    p = buf;
    while(*p != '\0' && *p != '\r' && *p != '\n') { p++; }
    *p = '\0';

    /* find the first comma */
    p = buf;
    while(*p != '\0' && *p != ',') { p++; }
    if(*p == '\0')
    {
      fprintf(stderr, "malformed cache file\n");
      goto ERR;
    }

    // slap in a null
    *p++ = '\0';

    datestr = buf;
    ipstr = p;

    *date = strtoul(datestr, NULL, 10);
    *ipaddr = strdup(ipstr);
  }
  else
  {
    *date = 0;
    *ipaddr = NULL;
  }

  fclose(fp);

//	printf("time= %ld\n", *date);
//	printf("ipaddr= %s\n", *ipaddr);

  return 0;

ERR:

  if(fp) { fclose(fp); }
  return(-1);
}

int write_cache_file(char *file, time_t date, char *ipaddr)
{
  FILE *fp = NULL;

  if((fp=fopen(file, "w")) == NULL)
  {
    return(-1);
  }

  fprintf(fp, "%ld,%s\n", date, ipaddr);

  fclose(fp);

	save_cache_file(file);		//jeffrey2009/5/21
	
  return 0;
}

//jeffrey2009/5/21
//+ make cache file from xml config file
int make_cache_file_by_xml(char *file)
{
	char seconds[12];
	char ipaddr[32];
	
	seconds[0] = 0;
	ipaddr[0] = 0;
	
	xml_get_str(XML_DDNS_TIMESTAMP, sizeof(seconds), seconds);
	xml_get_str(XML_DDNS_IP, sizeof(ipaddr), ipaddr);
//	printf("time= %s\n", seconds);
//	printf("ipaddr= %s\n", ipaddr);
	
	if (seconds[0] && ipaddr[0])
	{
		FILE *fp = NULL;

		if((fp=fopen(file, "w")) == NULL)
		{
			perror("fopen ");
			return(-1);
		}

		fprintf(fp, "%ld,%s\n", atol(seconds), ipaddr);

		fclose(fp);
	}
	return 0;
}

int save_cache_file(char *file)		//jeffrey2009/5/21
{
	time_t seconds;
	char *ipaddr;
	char tmp[12];

	seconds = -1;
	ipaddr = NULL;
	read_cache_file(file, &seconds, &ipaddr);
	
//	printf("time= %ld\n", seconds);
//	printf("ipaddr= %s\n", ipaddr);

	if (seconds != -1 && ipaddr != NULL)
	{
		sprintf(tmp, "%ld", seconds);
		xml_set_str(XML_DDNS_TIMESTAMP, tmp);
		xml_set_str(XML_DDNS_IP, ipaddr);
	}

//	{
//	printf("-----\n");
//	char seconds[12];
//	char ipaddr[32];
//	xml_get_str(XML_DDNS_TIMESTAMP, sizeof(seconds), seconds);
//	xml_get_str(XML_DDNS_IP, sizeof(ipaddr), ipaddr);
//	printf("time= %s\n", seconds);
//	printf("ipaddr= %s\n", ipaddr);
//	}
	
	if (ipaddr)
		free(ipaddr);
	return 0;
}

int remove_cache_file(char *file)		// alpha.jeffrey20091229
{
	xml_del(XML_DDNS_TIMESTAMP);
	xml_del(XML_DDNS_IP);
	return 0;
}