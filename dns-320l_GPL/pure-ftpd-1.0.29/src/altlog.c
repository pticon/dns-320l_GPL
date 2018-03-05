
/*
 * Alternative logging formats for Pure-FTPd 
 */

#include <config.h>

#ifdef WITH_ALTLOG

# include "ftpd.h"
# include "ftpwho-update.h"
# include "globals.h"
# include "altlog.h"

# ifdef WITH_DMALLOC
#  include <dmalloc.h>
# endif

/*#define BUFFER_SIZE 51*512
#define MAX_ENTRIES 50

typedef struct
{
	char content[1024];
}LOG_CONTENT;

typedef struct FTP_LOG {
	LOG_CONTENT entries[MAX_ENTRIES+1];
	int num;
}Log, *pLog;
*/
static int altlog_write(const char *str)
{
    struct flock lock;
    ssize_t left;
    
    if (altlog_fd == -1 || str == NULL || 
        (left = (ssize_t) strlen(str)) <= (ssize_t) 0) {
        return -1;
    }
    lock.l_whence = SEEK_SET;
    lock.l_start = (off_t) 0;
    lock.l_len = (off_t) 0;
    lock.l_pid = getpid();
    lock.l_type = F_WRLCK;
    while (fcntl(altlog_fd, F_SETLKW, &lock) < 0 && errno == EINTR);
    if (lseek(altlog_fd, (off_t) 0, SEEK_END) < (off_t) 0
# ifdef ESPIPE
        && errno != ESPIPE
# endif
	) {
        return -1;
    }
    (void) safe_write(altlog_fd, str, (size_t) left);
    lock.l_type = F_UNLCK;
    while (fcntl(altlog_fd, F_SETLK, &lock) < 0 && errno == EINTR);    
    
    return 0;
}


/* Read and Write to another file */
/*
static int altlog_recycle(const char *str)
{
	char *buffer;
	int bytes_read=0, bytes_write=0, count=0, start=0, read_length=0, write_length=0, buffer_length=0;
	char *tok, *pos, *ptr=NULL, *p_close;
	off_t file_length=0;
	
	//Log _ftplog;
	pLog ftplog=malloc(sizeof(Log));
	
	memset(ftplog, 0x00, sizeof(Log));
	
	for(count=0;count<=MAX_ENTRIES;count++){
		memset(ftplog->entries[count].content, 0x00, sizeof(ftplog->entries[count].content));
	}
	
	buffer=malloc(BUFFER_SIZE);
	memset(buffer, 0x00, sizeof(buffer));

	struct flock lock;
	
	lock.l_whence = SEEK_SET;
	lock.l_start = (off_t)0;
	lock.l_len = (off_t)0;		// length, 0 = to EOF 
	lock.l_pid = getpid();
	lock.l_type = F_RDLCK;
	

	while (fcntl(altlog_fd, F_SETLKW, &lock) < 0 && errno == EINTR);
	
	if(lseek(altlog_fd, (off_t)0, SEEK_SET)<0)
		return -1;

	count=0;
	ftplog->num=0;
	
	while(bytes_read=read(altlog_fd, buffer, BUFFER_SIZE))
	{
		if((bytes_read==-1)&&errno!=EINTR) break;
		else if(bytes_read>0) {
			if(count==0) {
				buffer_length=bytes_read;
				ptr=buffer;
				while((pos=strchr(ptr, '\n'))!=NULL&&(ftplog->num<MAX_ENTRIES)) {
					strncpy(ftplog->entries[ftplog->num].content, ptr, (pos-ptr)+1);
					read_length+=(pos-ptr)+1;
					ptr=pos+1;
					ftplog->num++;
				}
			}
			count++;
		}
	}
	file_length=read_length;
	strncpy(ftplog->entries[ftplog->num].content, str, strlen(str));
	//strcpy(ftplog->entries[ftplog->num].content, str);
	ftplog->num++;
	
	//buffer_length=buffer_length+strlen(str);
	// Unlock the file
	lock.l_type = F_UNLCK;

	while (fcntl(altlog_fd, F_SETLK, &lock) < 0 && errno == EINTR);    
		
	
	
	lock.l_whence = SEEK_SET;
	lock.l_start = (off_t)0;
	lock.l_len = (off_t)0;		// length, 0 = to EOF 
	lock.l_pid = getpid();
	lock.l_type = F_WRLCK;

	while (fcntl(altlog_fd, F_SETLKW, &lock) < 0 && errno == EINTR);
	
	if(lseek(altlog_fd, (off_t)0, SEEK_SET)<0)
		return -1;
	
	
	if(ftplog->num>MAX_ENTRIES)
		start=1;
	else
		start=0;
	
	for(count=start;count<ftplog->num;count++) {
		bytes_write=write(altlog_fd, ftplog->entries[count].content, strlen(ftplog->entries[count].content));		//printf("%d\n", bytes_write);
		write_length+=strlen(ftplog->entries[count].content);

	}

	
	
	//* Write empty char in the end of entries to end of file. ++ 
	if(lseek(altlog_fd, (off_t)write_length, SEEK_SET)<0)
		return -1;
	
	if(read_length>write_length) {
		p_close=malloc(read_length-write_length);
		memset(p_close, 0x00, read_length-write_length);
		bytes_write=write(altlog_fd, p_close, read_length-write_length);

	}
	//* Write empty char in the end of entries to end of file. -- 
	
	// Patrick 20081119 ++
	if(write_length<file_length)
		ftruncate(altlog_fd, write_length);
	// Patrick 20081119 --
	
	lock.l_type = F_UNLCK;
	
	while (fcntl(altlog_fd, F_SETLK, &lock) < 0 && errno == EINTR);    
	
	free(buffer);
	free(ftplog);
	
	return 0;
}
*/


/* Verbose but compact log format for ftpStats */

static int altlog_writexfer_stats(const int upload,
                                  const char * const filename,
                                  const off_t size,
                                  const double duration)
{
    /*
     * <date> <start.pid> <user> <ip> <u/d> <size> <duration> <file>
     */

    const char *host_ = *host != 0 ? host : "-";
    const char *account_ = *account != 0 ? account : "-";
    char *alloca_line;
    size_t line_size;
    
    line_size = 16U /* now */ + 1U + 16U /* start */ + 1U /* . */ +
        16U /* pid */ + 1U + strlen(account_) + 1U + strlen(host_) + 1U +
        1U /* U/D */ + 1U + 20U /* size */ + 1U + 16U /* duration */ +
        strlen(filename) + 1U /* \n */ + 1U;
    if ((alloca_line = ALLOCA(line_size)) == NULL) {
        return -1;
    }
    if (!SNCHECK(snprintf(alloca_line, line_size,
                          "%llu %llx.%lx %s %s %c %llu %lu %s\n",
                          (unsigned long long) time(NULL),
                          (unsigned long long) session_start_time,
                          (unsigned long) getpid(),
                          account_, host_,
                          upload != 0 ? 'U' : 'D',
                          (unsigned long long) size,
                          (unsigned long) (duration + 0.5),
                          filename), line_size)) {
        altlog_write(alloca_line);        
    }
                          
    ALLOCA_FREE(alloca_line);
    
    return 0;
}

# define NO_URLENCODE(c) ( \
                               ((c) >= 'a' && (c) <= 'z') || \
                               ((c) >= 'A' && (c) <= 'Z') || \
                               ((c) >= '0' && (c) <= '9') || \
                               (c) == '.' || (c) == '/' || \
                               (c) == '_' || (c) == '-' \
                               )
# define HEXD(c) ((c) < 10 ? '0' + (c) : 'A' - 10 + (c))

static char *urlencode(const char *filename)
{
    const char *ptr = filename;
    char *quoted_filename;
    char *quoted_filename_ptr;
    size_t quoted_filename_size = (size_t) 1U;  
    int need_quote = 0;
    char c;

    while (*ptr != 0) {
        if (NO_URLENCODE(*ptr)) {
            quoted_filename_size++;
        } else {
            quoted_filename_size += (size_t) 3U;
            need_quote = 1;
        }
        ptr++;
    }
    if (need_quote == 0) {
        return (char *) filename;
    }
    if ((quoted_filename = malloc(quoted_filename_size)) == NULL) {
        return NULL;
    }
    quoted_filename_ptr = quoted_filename;
    ptr = filename;
    c = *ptr;
    do {
        if (NO_URLENCODE(c)) {
            if (quoted_filename_size <= (size_t) 1U) {
                return NULL;
            }
            quoted_filename_size--;         
            *quoted_filename_ptr++ = c;
        } else {
            if (quoted_filename_size <= (size_t) 3U) {
                return NULL;
            }
            quoted_filename_size -= (size_t) 3U;            
            *quoted_filename_ptr++ = '%';
            *quoted_filename_ptr++ = HEXD(((unsigned char) c) >> 4);
            *quoted_filename_ptr++ = HEXD(((unsigned char) c) & 0xf);
        }
        ptr++;
    } while ((c = *ptr) != 0);
    *quoted_filename_ptr = 0;
    
    return quoted_filename;
}

/* HTTPd-like common log format */

static int altlog_writexfer_clf(const int upload,
                                const char * const filename,
                                const off_t size)
{
    char date[sizeof "13/Apr/1975:12:34:56 +0100"];     
    struct tm *tm;
    char *alloca_line;
    const char *host_ = *host != 0 ? host : "-";
    const char *account_ = *account != 0 ? account : "-";
    char *quoted_filename;
    time_t now;
    long diff;
    int sign;
    size_t line_size;

    if ((now = time(NULL)) == (time_t) -1 ||
        (tm = localtime(&now)) == NULL ||
        tm->tm_mon > 11 || tm->tm_mon < 0) {
        return -1;
    }    
# ifdef HAVE_STRUCT_TM_TM_GMTOFF
    diff = -(tm->tm_gmtoff) / 60L;
# elif defined(HAVE_SCALAR_TIMEZONE)
    diff = -(timezone) / 60L;    
# else
    {
        struct tm gmt;
        struct tm *t;
        int days, hours, minutes;
        
        gmt = *gmtime(&now);    
        t = localtime(&now);
        days = t->tm_yday - gmt.tm_yday;
        hours = ((days < -1 ? 24 : 1 < days ? -24 : days * 24)
                 + t->tm_hour - gmt.tm_hour);
        minutes = hours * 60 + t->tm_min - gmt.tm_min;
        diff = -minutes;
    }
# endif
    if (diff > 0L) {
        sign = '+';
    } else {
        sign = '-';
        diff = -diff;
    }
    
    if (SNCHECK(snprintf(date, sizeof date, 
                         "%02d/%s/%d:%02d:%02d:%02d %c%02ld%02ld",
                         tm->tm_mday, months[tm->tm_mon], tm->tm_year + 1900,
                         tm->tm_hour, tm->tm_min, tm->tm_sec,
                         sign, diff / 60L, diff % 60L), sizeof date)) {
        return -1;
    }
    if ((quoted_filename = urlencode(filename)) == NULL) {
        return -1;
    }
    line_size = strlen(host_) + (sizeof " - " - 1U) + strlen(account_) +
        (sizeof " [" - 1U) + (sizeof date - 1U) + (sizeof "] \"" - 1U) +
        3U /* GET / PUT */ + (sizeof " " - 1U) + strlen(quoted_filename) +
        (sizeof "\" 200 18446744073709551616\n" - 1U) + 1U;
    if ((alloca_line = ALLOCA(line_size)) == NULL) {
        return -1;
    }
    if (!SNCHECK(snprintf(alloca_line, line_size, 
                          "%s - %s [%s] \"%s %s\" 200 %llu\n",
                          host_, account_, date, 
                          upload == 0 ? "GET" : "PUT", quoted_filename,
                          (unsigned long long) size), line_size)) {
        altlog_write(alloca_line);
    }
    if (quoted_filename != filename) {
        free(quoted_filename);
    }
    ALLOCA_FREE(alloca_line);
    
    return 0;
}



/* Simple log format for alphanetworks */

//static int altlog_writexfer_simple(const int upload,
//                                const char * const filename,
//                                const off_t size)
//{
//    struct tm *tm;
//    const char *account_ = *account != 0 ? account : "-";
//    time_t now;
//    char web_encode[1024];
//    char file_np[256];
//    char const *n_ptr=NULL;
//    // Patrick 20080930 ++
//		char access_date[sizeof "12/31/1975 12:22:34"];	// date format: month/day/year
//		char *pos;		// Here cost a lot of my time......:(
//    char *format_log, *no_path_filename, *new_filename;
//    size_t format_size;
//  
//    
//    
//    memset(web_encode, 0x00 ,sizeof(web_encode));
//    memset(file_np, 0x00, sizeof(file_np));
//    
//    // Patrick 20080930 --
//    if ((now = time(NULL)) == (time_t) -1 ||
//        (tm = localtime(&now)) == NULL ||
//        tm->tm_mon > 11 || tm->tm_mon < 0) {
//        return -1;
//    }    
//
//	// Patrick 20080930 ++
//		if(SNCHECK(snprintf(access_date, sizeof access_date, 
//			"%02d/%02d/%d %02d:%02d:%02d", 
//			tm->tm_mon+1, tm->tm_mday, tm->tm_year+1900, 
//			tm->tm_hour, tm->tm_min, tm->tm_sec),
//			sizeof access_date)) {		
//			return -1;
//		}
//
//#if 0
//    // Discard the file path ++
//    // patrick 20081107 thers is seems like the BUG is !!! +
//		if((pos = strrchr(filename, '/')) != NULL)
//			pos=pos+1;
//		else
//			return -1;
//			
//			
//		n_ptr = malloc(strlen(filename)-(pos-filename)+1);	
//		memset(n_ptr, 0, strlen(filename)-(pos-filename)+1);		
//		strncpy(n_ptr, pos, strlen(filename)-(pos-filename)+1);
//		// BUG--  can not use strrchr like below....!!
//			
//		strncpy(file_np	,pos, strlen(filename)-(pos-filename)+1);
//#endif			
//
//		//printf("1: %s\n", filename);
//		/* Upload (PUT) or download (GET) judgement */
//		if ((no_path_filename = urlencode(filename)) == NULL) {
//			return -1;
//		}
//
//	
//		//printf("%x, sizeof n_ptr: %d\n",n_ptr, strlen(n_ptr));
//		
//	// Discard the file path --
//    
//	format_size = strlen(access_date) + //(sizeof " " - 1U) + strlen(access_time) +
//		(sizeof " " - 1U) + strlen(account_) + (sizeof " \"" - 1U ) +
//        3U /* GET / PUT */ + (sizeof " \" " -1U ) +
//		strlen(no_path_filename) + (sizeof "\n" - 1U) + 1U;
//	
//	if ((format_log = ALLOCA(format_size)) == NULL) {
//	//if ((format_log = (char*)malloc(format_size)) == NULL) {
//		return -1;
//	}
//	if(!SNCHECK(snprintf(format_log, format_size,
//		"%s %s \"%s %s\"\n",
//		access_date, account_, 
//		upload == 0 ? "GET" : "PUT", no_path_filename), format_size)) {
//		//altlog_write(format_log);
//		altlog_recycle(format_log);
//	}
//
//	// Patrick 20080930 --
//    if (no_path_filename != filename) 
//        free(no_path_filename);
//    
//    ALLOCA_FREE(format_log);
//    
//    return 0;
//}

/* WuFTPd-like log format */

static int altlog_writexfer_xferlog(const int upload,
                    const char * const filename,
                    const off_t size,
                    const double duration)
{
    char date[sizeof "Mon Apr 13 12:34:56 1975"];
    struct tm *tm;
    char *alloca_line;
    const char *host_ = *host != 0 ? host : "-";
    const char *account_ = *account != 0 ? account : "-";
    char *quoted_filename;
    size_t filename_idx;
    time_t now;
    size_t line_size;
    size_t filename_size;
    char c;

    if ((now = time(NULL)) == (time_t) -1 ||
        (tm = localtime(&now)) == NULL ||
        tm->tm_mon > 11 || tm->tm_mon < 0 ||
        tm->tm_wday > 6 || tm->tm_wday < 0) {
        return -1;
    }    
    if (SNCHECK(snprintf(date, sizeof date, 
                         "%s %s %02d %02d:%02d:%02d %d",
                         week_days[tm->tm_wday], months[tm->tm_mon],
                         tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
                         tm->tm_year + 1900), sizeof date)) {
        return -1;
    }
    if ((filename_idx = strlen(filename)) <= (size_t) 0U) {
        return -1;
    }
    filename_size = filename_idx + (size_t) 1U;
    if ((quoted_filename = ALLOCA(filename_size)) == NULL) {
        return -1;
    }
    
    quoted_filename[filename_idx] = 0;
    do {
        filename_idx--;
        c = filename[filename_idx];
        if (isspace((unsigned char) c) || ISCTRLCODE(c)) {
            c = '_';
        }
        quoted_filename[filename_idx] = c;
    } while (filename_idx > (size_t) 0U);
    
    line_size = (sizeof date - 1U) + (sizeof " " - 1U) +
        (size_t) 16U /* duration */ + (sizeof " " - 1U) +
        strlen(host_) + (sizeof " " - 1U) + 
        (size_t) 20U /* size */ + (sizeof " " - 1U) +
        (filename_size - 1U) + (sizeof " " - 1U) +
        (size_t) 1U /* type */ + (sizeof " _ " - 1U) +
        (size_t) 1U /* direction */ + (sizeof " " - 1U) +
        (size_t) 1U /* anonymous */ + (sizeof " " - 1U) +
        strlen(account_) + (sizeof " ftp 1 * c\n" - 1U) + (size_t) 1U;
    if ((alloca_line = ALLOCA(line_size)) == NULL) {
        ALLOCA_FREE(quoted_filename);
        return -1;
    }
    if (!SNCHECK(snprintf(alloca_line, line_size, 
                          "%s %lu %s %llu %s %c _ %c %c %s ftp 1 * c\n",
                          date, (unsigned long) (duration + 0.5),
                          host_, (unsigned long long) size,
                          quoted_filename,
                          type == 1 ? 'a' : 'b',
                          upload != 0 ? 'i' : 'o',
                          loggedin != 0 ? 'r' : 'a',
                          account_), line_size)) {
        altlog_write(alloca_line);
    }
    ALLOCA_FREE(quoted_filename);
    ALLOCA_FREE(alloca_line);
    
    return 0;
}

static int altlog_writexfer_w3c(const int upload,
                                const char * const filename,
                                const off_t size,
                                const double duration)
{
    /*
     * <date> <time> <ip> "[]sent" <file> "226" <user>
     * date time c-ip cs-method cs-uri-stem sc-status cs-username
     */

    struct tm *tm;
    struct tm gmt;
    const char *host_ = *host != 0 ? host : "-";
    const char *account_ = *account != 0 ? account : "-";
    char *alloca_line;
    char *quoted_filename;
    time_t now;
    size_t line_size;

    (void) duration;
    if ((now = time(NULL)) == (time_t) -1 ||
        (tm = localtime(&now)) == NULL ||
        tm->tm_mon > 11 || tm->tm_mon < 0) {
        return -1;
    }
    gmt = *gmtime(&now);
    if ((quoted_filename = urlencode(filename)) == NULL) {
        return -1;
    }   
    line_size = (sizeof "13-04-1975 12:34:56 " - 1U) +
        strlen(host_) + 1U + (sizeof "[]created" - 1U) + 1U +
        strlen(quoted_filename) + 1U + (sizeof "226" - 1U) +
        strlen(account_) + 1U + 42U + 1U /* \n */ + 1U;

    if ((alloca_line = ALLOCA(line_size)) == NULL) {
        return -1;
    }
    if (!SNCHECK(snprintf(alloca_line, line_size,
                          "%d-%02d-%02d %02d:%02d:%02d %s []%s %s 226 %s %llu\n",
                          gmt.tm_year + 1900, gmt.tm_mon + 1, gmt.tm_mday,
                          gmt.tm_hour, gmt.tm_min, gmt.tm_sec,
                          host_, (upload != 0 ? "created" : "sent"),
                          quoted_filename,
                          account, (unsigned long long) size), line_size)) {
        altlog_write(alloca_line);
    }
    if (quoted_filename != filename) {
        free(quoted_filename);
    }
    ALLOCA_FREE(alloca_line);

    return 0;
}

int altlog_write_w3c_header(void)
{
    time_t now;
    struct tm *tm;
    struct tm gmt;
    char *alloca_line;
    size_t line_size;

    if ((now = time(NULL)) == (time_t) -1 ||
        (tm = localtime(&now)) == NULL ||
        tm->tm_mon > 11 || tm->tm_mon < 0) {
        return -1;
    }
    gmt = *gmtime(&now);
    line_size = sizeof "#Date: 001975-04-13 12:34:56\n";   /* be year-999999 compliant :) */
    if ((alloca_line = ALLOCA(line_size)) == NULL) {
        return -1;
    }

    altlog_write("#Software: Pure-FTPd " VERSION "\n");
    altlog_write("#Version: 1.0\n");

    if (!SNCHECK(snprintf(alloca_line, line_size,
                          "#Date: %04d-%02d-%02d %02d:%02d:%02d\n",
                          gmt.tm_year + 1900, gmt.tm_mon + 1, gmt.tm_mday,
                          gmt.tm_hour, gmt.tm_min, gmt.tm_sec),
                          line_size)) {
        altlog_write(alloca_line);
    }

    altlog_write("#Fields: date time c-ip cs-method cs-uri-stem sc-status cs-username sc-bytes\n");

    ALLOCA_FREE(alloca_line);

    return 0;
}

/* 
 * We should define a structure of function pointers, 
 * and associate a structure with each logging type in AltLogPrefixes.
 * But yet we only have *three* logging methods, and the code would be
 * complicated for nothing. So let's stick with simple tests for now. -j.
 */

int altlog_writexfer(const int upload,
                     const char * const filename,
                     const off_t size,
                     const double duration)
{
    switch (altlog_format) {
    case ALTLOG_NONE:
        return 0;
    case ALTLOG_CLF:
        return altlog_writexfer_clf(upload, filename, size);
    case ALTLOG_STATS:
        return altlog_writexfer_stats(upload, filename, size, duration);
    case ALTLOG_W3C:
        return altlog_writexfer_w3c(upload, filename, size, duration);
    case ALTLOG_XFERLOG:
        return altlog_writexfer_xferlog(upload, filename, size, duration);
//	case ALTLOG_SIMPLE:
//				return altlog_writexfer_simple(upload, filename, size);
    }    
    return -1;
}

#endif
