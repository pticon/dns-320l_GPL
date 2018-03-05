/*
 *
 *	Utility for reporting quotas
 *
 *	Based on old repquota.
 *	Jan Kara <jack@suse.cz> - Sponsored by SuSE CZ
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <getopt.h>

#include "pot.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio.h"

#define PRINTNAMELEN 9	/* Number of characters to be reserved for name on screen */
#define MAX_CACHE_DQUOTS 1024	/* Number of dquots in cache */

#define FL_USER 1
#define FL_GROUP 2
#define FL_VERBOSE 4
#define FL_ALL 8		/* Dump quota files on all filesystems */
#define FL_TRUNCNAMES 16	/* Truncate names to fit into the screen */
#define FL_SHORTNUMS 32	/* Try to print space in appropriate units */
#define FL_NONAME 64	/* Don't translate ids to names */
#define FL_NOCACHE 128	/* Don't cache dquots before resolving */
#define FL_NOAUTOFS 256	/* Ignore autofs mountpoints */
#define FL_RAWGRACE 512	/* Print grace times in seconds since epoch */
#define FL_SPECNAME 1024	/* Print specific name quota setting */
#define FL_USRFILE 2048	/* Specify a user name file*/
#define FL_GRPFILE 4096	/* Specify a group name file*/


int flags, fmt = -1;
char **mnt;
int mntcnt;
int cached_dquots;
struct dquot dquot_cache[MAX_CACHE_DQUOTS];
char *progname;
char spec_name[256]="";
char user_file[256]="/etc/passwd";
char group_file[256]="/etc/group";

static void usage(void)
{
	errstr(_("Utility for reporting quotas.\nUsage:\n%s [-vugsi] [-c|C] [-t|n] [-F quotaformat] (-a | mntpoint)\n\n\
-v, --verbose               display also users/groups without any usage\n\
-u, --user                  display information about users\n\
-g, --group                 display information about groups\n\
-s, --human-readable        show numbers in human friendly units (MB, GB, ...)\n\
-t, --truncate-names        truncate names to 8 characters\n\
-p, --raw-grace             print grace time in seconds since epoch\n\
-n, --no-names              do not translate uid/gid to name\n\
-i, --no-autofs             avoid autofs mountpoints\n\
-c, --batch-translation     translate big number of ids at once\n\
-C, --no-batch-translation  translate ids one by one\n\
-F, --format=formatname     report information for specific format\n\
-h, --help                  display this help message and exit\n\
-V, --version               display version information and exit\n\n"), progname);
	fprintf(stderr, _("Bugs to %s\n"), MY_EMAIL);
	exit(1);
}

static void parse_options(int argcnt, char **argstr)
{
	int ret;
	int cache_specified = 0;
	struct option long_opts[] = {
		{ "version", 0, NULL, 'V' },
		{ "all", 0, NULL, 'a' },
		{ "verbose", 0, NULL, 'v' },
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "help", 0, NULL, 'h' },
		{ "truncate-names", 0, NULL, 't' },
		{ "raw-grace", 0, NULL, 'p' },
		{ "human-readable", 0, NULL, 's' },
		{ "no-names", 0, NULL, 'n' },
		{ "cache", 0, NULL, 'c' },
		{ "no-cache", 0, NULL, 'C' },
		{ "no-autofs", 0, NULL, 'i' },
		{ "format", 1, NULL, 'F' },
		{ "uf", 1, NULL, 300 },
		{ "gf", 1, NULL, 301 },
		{ "specname", 1, NULL, 302 },
		{ NULL, 0, NULL, 0 }
	};

	while ((ret = getopt_long(argcnt, argstr, "VavughtspncCiF:", long_opts, NULL)) != -1) {
		switch (ret) {
			case '?':
			case 'h':
				usage();
			case 'V':
				version();
				exit(0);
			case 'u':
				flags |= FL_USER;
				break;
			case 'g':
				flags |= FL_GROUP;
				break;
			case 'v':
				flags |= FL_VERBOSE;
				break;
			case 'a':
				flags |= FL_ALL;
				break;
			case 't':
				flags |= FL_TRUNCNAMES;
				break;
			case 'p':
				flags |= FL_RAWGRACE;
				break;
			case 's':
				flags |= FL_SHORTNUMS;
				break;
			case 'C':
				flags |= FL_NOCACHE;
				cache_specified = 1;
				break;
			case 'c':
				cache_specified = 1;
				break;
			case 'i':
				flags |= FL_NOAUTOFS;
				break;
			case 'F':
				if ((fmt = name2fmt(optarg)) == QF_ERROR)
					exit(1);
				break;
			case 300:
				strcpy(user_file,optarg);
				flags |=FL_USRFILE;
				break;
			case 301:
				strcpy(group_file,optarg);
				flags |=FL_GRPFILE;
				break;
			case 302:
				strcpy(spec_name,optarg);
				flags |= FL_SPECNAME;
				break;
			case 'n':
				flags |= FL_NONAME;
				break;

		}
	}

	if ((flags & FL_ALL && optind != argcnt) || (!(flags & FL_ALL) && optind == argcnt)) {
		fputs(_("Bad number of arguments.\n"), stderr);
		usage();
	}
	if (fmt == QF_RPC) {
		fputs(_("Repquota cannot report through RPC calls.\n"), stderr);
		exit(1);
	}
	if (flags & FL_NONAME && flags & FL_TRUNCNAMES) {
		fputs(_("Specified both -n and -t but only one of them can be used.\n"), stderr);
		exit(1);
	}
	if (!(flags & (FL_USER | FL_GROUP)))
		flags |= FL_USER;
	if (!(flags & FL_ALL)) {
		mnt = argstr + optind;
		mntcnt = argcnt - optind;
	}
	if (!cache_specified && !(flags & FL_NONAME) && passwd_handling() == PASSWD_DB)
		flags |= FL_NOCACHE;
}

/* Are we over soft or hard limit? */
static char overlim(qsize_t usage, qsize_t softlim, qsize_t hardlim)
{
	if ((usage > softlim && softlim) || (usage > hardlim && hardlim))
		return '+';
	return '-';
}

/* Print one quota entry */
static void print(struct dquot *dquot, char *name)
{
	char pname[MAXNAMELEN];
	char time[MAXTIMELEN];
	char numbuf[3][MAXNUMLEN];
	
	struct util_dqblk *entry = &dquot->dq_dqb;

//jack
//	if (!entry->dqb_curspace && !entry->dqb_curinodes && !(flags & FL_VERBOSE))
//		return;
	
	if ( !(flags & FL_NONAME)&&(name[0] == '#') )
		return;
		
	if(flags & FL_SPECNAME)
	{
		if(strcmp(spec_name,name))
			return;
		else
		{
			space2str(toqb(entry->dqb_curspace), numbuf[0], flags & FL_SHORTNUMS);
			space2str(entry->dqb_bhardlimit, numbuf[2], flags & FL_SHORTNUMS);
			printf("%s %s\n",numbuf[0],numbuf[2]);
			return;
		}
	}
		 
	sstrncpy(pname, name, sizeof(pname));
	if (flags & FL_TRUNCNAMES)
		pname[PRINTNAMELEN] = 0;
	if (entry->dqb_bsoftlimit && toqb(entry->dqb_curspace) >= entry->dqb_bsoftlimit)
		if (flags & FL_RAWGRACE)
			sprintf(time, "%llu", (unsigned long long)entry->dqb_btime);
		else
			difftime2str(entry->dqb_btime, time);
	else
		if (flags & FL_RAWGRACE)
			strcpy(time, "0");
		else
			time[0] = 0;
	space2str(toqb(entry->dqb_curspace), numbuf[0], flags & FL_SHORTNUMS);
	space2str(entry->dqb_bsoftlimit, numbuf[1], flags & FL_SHORTNUMS);
	space2str(entry->dqb_bhardlimit, numbuf[2], flags & FL_SHORTNUMS);
	printf("%-*s %c%c %7s %7s %7s %6s", PRINTNAMELEN, pname,
	       overlim(qb2kb(toqb(entry->dqb_curspace)), qb2kb(entry->dqb_bsoftlimit), qb2kb(entry->dqb_bhardlimit)),
	       overlim(entry->dqb_curinodes, entry->dqb_isoftlimit, entry->dqb_ihardlimit),
	       numbuf[0], numbuf[1], numbuf[2], time);
	if (entry->dqb_isoftlimit && entry->dqb_curinodes >= entry->dqb_isoftlimit)
		if (flags & FL_RAWGRACE)
			sprintf(time, "%llu", (unsigned long long)entry->dqb_itime);
		else
			difftime2str(entry->dqb_itime, time);
	else
		if (flags & FL_RAWGRACE)
			strcpy(time, "0");
		else
			time[0] = 0;
	number2str(entry->dqb_curinodes, numbuf[0], flags & FL_SHORTNUMS);
	number2str(entry->dqb_isoftlimit, numbuf[1], flags & FL_SHORTNUMS);
	number2str(entry->dqb_ihardlimit, numbuf[2], flags & FL_SHORTNUMS);
	printf(" %7s %5s %5s %6s\n", numbuf[0], numbuf[1], numbuf[2], time);
}

/* Print all dquots in the cache */
static void dump_cached_dquots(int type)
{
	FILE *fp;
	int i;
	char namebuf[MAXNAMELEN];
//	struct stat buf;

	if (!cached_dquots)
		return;
	
	if (type == USRQUOTA) {
		struct passwd *pwent;

		if(!(flags & FL_USRFILE))
		{
			setpwent();
			while ((pwent = getpwent())) {
				for (i = 0; i < cached_dquots && pwent->pw_uid != dquot_cache[i].dq_id; i++);
				if (i < cached_dquots && !(dquot_cache[i].dq_flags & DQ_PRINTED)) {
					print(dquot_cache+i, pwent->pw_name);
					dquot_cache[i].dq_flags |= DQ_PRINTED;
				}
			}

			endpwent();
		}
		else
		{
//			stat(user_file,&buf);
//			(buf.st_mode)&SIFREG;
		fp=fopen(user_file,"r");
		if(fp == NULL)
		{
			cached_dquots = 0;
			return;
		}	
		
		while ((pwent = fgetpwent(fp))) {
			for (i = 0; i < cached_dquots && pwent->pw_uid != dquot_cache[i].dq_id; i++);
			if (i < cached_dquots && !(dquot_cache[i].dq_flags & DQ_PRINTED)) {
				print(dquot_cache+i, pwent->pw_name);
				dquot_cache[i].dq_flags |= DQ_PRINTED;
			}
		}
			
		fclose(fp);
		}
	}
	else {
		struct group *grent;

		if(!(flags & FL_GRPFILE))
		{
			setgrent();
			while ((grent = getgrent())) {
				for (i = 0; i < cached_dquots && grent->gr_gid != dquot_cache[i].dq_id; i++);
				if (i < cached_dquots && !(dquot_cache[i].dq_flags & DQ_PRINTED)) {
					print(dquot_cache+i, grent->gr_name);
					dquot_cache[i].dq_flags |= DQ_PRINTED;
				}
			}
			endgrent();
		
		}
		else
		{
		fp=fopen(group_file,"r");
		
		if(fp == NULL)
		{
			cached_dquots = 0;
			return;
		}
			
		while ((grent = fgetgrent(fp))) {
			for (i = 0; i < cached_dquots && grent->gr_gid != dquot_cache[i].dq_id; i++);
			if (i < cached_dquots && !(dquot_cache[i].dq_flags & DQ_PRINTED)) {
				print(dquot_cache+i, grent->gr_name);
				dquot_cache[i].dq_flags |= DQ_PRINTED;
			}
		}
		
		fclose(fp);
		}
		
	}
	for (i = 0; i < cached_dquots; i++)
		if (!(dquot_cache[i].dq_flags & DQ_PRINTED)) {
			sprintf(namebuf, "#%u", dquot_cache[i].dq_id);
			print(dquot_cache+i, namebuf);
		}
	cached_dquots = 0;
}

/* Callback routine called by scan_dquots on each dquot */
static int output(struct dquot *dquot, char *name)
{
	if (flags & FL_NONAME) {	/* We should translate names? */
		char namebuf[MAXNAMELEN];

		sprintf(namebuf, "#%u", dquot->dq_id);
		print(dquot, namebuf);
	}
	else if (name || flags & FL_NOCACHE) {	/* We shouldn't do batched id->name translations? */
		char namebuf[MAXNAMELEN];

		if (!name) {
			id2name(dquot->dq_id, dquot->dq_h->qh_type, namebuf);
			name = namebuf;
		}
		print(dquot, name);
	}
	else {	/* Lets cache the dquot for later printing */
		memcpy(dquot_cache+cached_dquots++, dquot, sizeof(struct dquot));
		if (cached_dquots >= MAX_CACHE_DQUOTS)
			dump_cached_dquots(dquot->dq_h->qh_type);
	}
	return 0;
}

/* Dump information stored in one quota file */
static void report_it(struct quota_handle *h, int type)
{
	char bgbuf[MAXTIMELEN], igbuf[MAXTIMELEN];
	
	if(flags & FL_SPECNAME)
	{
	}
	else
	{
		printf(_("*** Report for %s quotas on device %s\n"), type2name(type), h->qh_quotadev);
		time2str(h->qh_info.dqi_bgrace, bgbuf, TF_ROUND);
		time2str(h->qh_info.dqi_igrace, igbuf, TF_ROUND);
		printf(_("Block grace time: %s; Inode grace time: %s\n"), bgbuf, igbuf);
		printf(_("                        Block limits                File limits\n"));
		printf(_("%-9s       used    soft    hard  grace    used  soft  hard  grace\n"), (type == USRQUOTA)?_("User"):_("Group"));
		printf("----------------------------------------------------------------------\n");
	}
	if (h->qh_ops->scan_dquots(h, output) < 0)
		return;
	dump_cached_dquots(type);
	if (h->qh_ops->report) {
		if(flags & FL_SPECNAME)
		{
			h->qh_ops->report(h, flags & FL_VERBOSE);
		}
		else
		{
		putchar('\n');
		h->qh_ops->report(h, flags & FL_VERBOSE);
		putchar('\n');
	}
}
}

static void report(int type)
{
	struct quota_handle **handles;
	int i;

	if (flags & FL_ALL)
		handles = create_handle_list(0, NULL, type, fmt, IOI_READONLY | IOI_OPENFILE, MS_LOCALONLY | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0));
	else
		handles = create_handle_list(mntcnt, mnt, type, fmt, IOI_READONLY | IOI_OPENFILE, MS_LOCALONLY | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0));
	for (i = 0; handles[i]; i++)
		report_it(handles[i], type);
	dispose_handle_list(handles);
}

int main(int argc, char **argv)
{
	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();

	if (flags & FL_USER)
		report(USRQUOTA);

	if (flags & FL_GROUP)
		report(GRPQUOTA);

	return 0;
}
