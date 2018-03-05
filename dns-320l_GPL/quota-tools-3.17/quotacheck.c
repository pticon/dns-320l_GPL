/*
 *
 *	Utility to check disk quotas
 *
 *	Some parts of this utility are copied from old quotacheck by
 *	Marco van Wieringen <mvw@planets.elm.net> and Edvard Tuinder <ed@elm.net>
 * 
 *	New quota format implementation - Jan Kara <jack@suse.cz> - Sponsored by SuSE CR
 */

#ident "$Id: quotacheck.c,v 1.57 2008/12/17 12:40:07 jkar8572 Exp $"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/statfs.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/utsname.h>

#if defined(HAVE_EXT2_INCLUDE)
#include <linux/types.h>
#include <ext2fs/ext2fs.h>
#endif

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quotasys.h"
#include "mntopt.h"
#include "bylabel.h"
#include "quotacheck.h"
#include "quotaops.h"

#ifndef HAVE_EXT2_INO_T
typedef ino_t ext2_ino_t;
#endif

#define LINKSHASHSIZE 16384	/* Size of hashtable for hardlinked inodes */
#define DQUOTHASHSIZE 32768	/* Size of hashtable for dquots from file */

struct dlinks {
	ino_t i_num;
	struct dlinks *next;
};

struct dirs {
	char *dir_name;
	struct dirs *next;
};

#define BITS_SIZE 4		/* sizeof(bits) == 5 */
#define BLIT_RATIO 10		/* Blit in just 1/10 of blit() calls */

dev_t cur_dev;			/* Device we are working on */
int files_done, dirs_done;
int flags, fmt = -1, cfmt;	/* Options from command line; Quota format to use spec. by user; Actual format to check */
int uwant, gwant, ucheck, gcheck;	/* Does user want to check user/group quota; Do we check user/group quota? */
char *mntpoint;			/* Mountpoint to check */
char *progname;
struct util_dqinfo old_info[MAXQUOTAS];	/* Loaded infos */

char extensions[MAXQUOTAS + 2][20] = INITQFNAMES;	/* Extensions depending on quota type */
char *basenames[] = INITQFBASENAMES;	/* Names of quota files */

#ifdef DEBUG_MALLOC
size_t malloc_mem = 0;
size_t free_mem = 0;
#endif

struct dquot *dquot_hash[MAXQUOTAS][DQUOTHASHSIZE];
struct dlinks *links_hash[MAXQUOTAS][DQUOTHASHSIZE];

/*
 * Ok check each memory allocation.
 */
void *xmalloc(size_t size)
{
	void *ptr;

#ifdef DEBUG_MALLOC
	malloc_mem += size;
#endif
	ptr = malloc(size);
	if (!ptr)
		die(3, _("Not enough memory.\n"));
	memset(ptr, 0, size);
	return (ptr);
}

void debug(int df, char *fmtstr, ...)
{
	va_list args;

	if (!(flags & df))
		return;

	fprintf(stderr, "%s: ", progname);
	va_start(args, fmtstr);
	vfprintf(stderr, fmtstr, args);
	va_end(args);
}

/* Compute hashvalue for given inode number */
static inline uint hash_ino(uint i_num)
{
	return ((i_num ^ (i_num << 16)) * 997) & (LINKSHASHSIZE - 1);
}

/*
 * Store a hardlinked inode as we don't want to count it more then once.
 */
static int store_dlinks(int type, ino_t i_num)
{
	struct dlinks *lptr;
	uint hash = hash_ino(i_num);

	debug(FL_DEBUG, _("Adding hardlink for ino %llu\n"), (unsigned long long)i_num);

	for (lptr = links_hash[type][hash]; lptr; lptr = lptr->next)
		if (lptr->i_num == i_num)
			return 1;

	lptr = (struct dlinks *)xmalloc(sizeof(struct dlinks));

	lptr->i_num = i_num;
	lptr->next = links_hash[type][hash];
	links_hash[type][hash] = lptr;
	return 0;
}

/* Hash given id */
static inline uint hash_dquot(uint id)
{
	return ((id ^ (id << 16)) * 997) & (DQUOTHASHSIZE - 1);
}

/*
 * Do a lookup of a type of quota for a specific id. Use short cut with
 * most recently used dquot struct pointer.
 */
struct dquot *lookup_dquot(qid_t id, int type)
{
	struct dquot *lptr;
	uint hash = hash_dquot(id);

	for (lptr = dquot_hash[type][hash]; lptr != NODQUOT; lptr = lptr->dq_next)
		if (lptr->dq_id == id)
			return lptr;
	return NODQUOT;
}

/*
 * Add a new dquot for a new id to the list.
 */
struct dquot *add_dquot(qid_t id, int type)
{
	struct dquot *lptr;
	uint hash = hash_dquot(id);

	debug(FL_DEBUG, _("Adding dquot structure type %s for %d\n"), type2name(type), (int)id);

	lptr = (struct dquot *)xmalloc(sizeof(struct dquot));

	lptr->dq_id = id;
	lptr->dq_next = dquot_hash[type][hash];
	dquot_hash[type][hash] = lptr;
	lptr->dq_dqb.dqb_btime = lptr->dq_dqb.dqb_itime = (time_t) 0;

	return lptr;
}

/*
 * Add a number of blocks and inodes to a quota.
 */
static void add_to_quota(int type, ino_t i_num, uid_t i_uid, gid_t i_gid, mode_t i_mode,
			 nlink_t i_nlink, loff_t i_space, int need_remember)
{
	qid_t wanted;
	struct dquot *lptr;

	if (type == USRQUOTA)
		wanted = i_uid;
	else
		wanted = i_gid;

	if ((lptr = lookup_dquot(wanted, type)) == NODQUOT)
		lptr = add_dquot(wanted, type);

	if (i_nlink != 1 && need_remember)
		if (store_dlinks(type, i_num))	/* Did we already count this inode? */
			return;
	lptr->dq_dqb.dqb_curinodes++;
	lptr->dq_dqb.dqb_curspace += i_space;;
}

/*
 * Clean up all list from a previous run.
 */
static void remove_list(void)
{
	int cnt;
	uint i;
	struct dquot *dquot, *dquot_free;
	struct dlinks *dlink, *dlink_free;

	for (cnt = 0; cnt < MAXQUOTAS; cnt++) {
		for (i = 0; i < DQUOTHASHSIZE; i++) {
			dquot = dquot_hash[cnt][i];
			while (dquot != NODQUOT) {
				dquot_free = dquot;
				dquot = dquot->dq_next;
#ifdef DEBUG_MALLOC
				free_mem += sizeof(struct dquot);
#endif
				free(dquot_free);
			}
			dquot_hash[cnt][i] = NODQUOT;
		}
		for (i = 0; i < LINKSHASHSIZE; i++) {
			dlink = links_hash[cnt][i];
			while (dlink) {
				dlink_free = dlink;
				dlink = dlink->next;
#ifdef DEBUG_MALLOC
				free_mem += sizeof(struct dlinks);
#endif
				free(dlink_free);
			}
			links_hash[cnt][i] = NULL;
		}
	}
}

/* Get size used by file */
static loff_t getqsize(char *fname, struct stat *st)
{
	static char ioctl_fail_warn;
	int fd;
	loff_t size;

	if (S_ISLNK(st->st_mode))	/* There's no way to do ioctl() on links... */
		return st->st_blocks << 9;
	if (!S_ISDIR(st->st_mode) && !S_ISREG(st->st_mode))
		return st->st_blocks << 9;
	if ((fd = open(fname, O_RDONLY)) == -1)
		die(2, _("Cannot open file %s: %s\n"), fname, strerror(errno));
	if (ioctl(fd, FIOQSIZE, &size) == -1) {
		size = st->st_blocks << 9;
		if (!ioctl_fail_warn) {
			ioctl_fail_warn = 1;
			fputs(_("Cannot get exact used space... Results might be inaccurate.\n"), stderr);
		}
	}
	close(fd);
	return size;
}

/*
 * Show a blitting cursor as means of visual progress indicator.
 */
static inline void blit(char *msg)
{
	static int bitc = 0;
	static const char bits[] = "|/-\\";
	static int slow_down;

	if (flags & FL_VERYVERBOSE && msg) {
		int len = strlen(msg);
		
		putchar('\r');
		printf("%.70s", msg);
		if (len > 70)
			fputs("...", stdout);
		else
			printf("%*s",73-len, "");
	}
	if (flags & FL_VERYVERBOSE || ++slow_down >= BLIT_RATIO) {
		putchar(bits[bitc]);
		putchar('\b');
		fflush(stdout);
		bitc++;
		bitc %= BITS_SIZE;
		slow_down = 0;
	}
}

static void usage(void)
{
	printf(_("Utility for checking and repairing quota files.\n%s [-gucbfinvdmMR] [-F <quota-format>] filesystem|-a\n\n\
-u, --user                check user files\n\
-g, --group               check group files\n\
-c, --create-files        create new quota files\n\
-b, --backup              create backups of old quota files\n\
-f, --force               force check even if quotas are enabled\n\
-i, --interactive         interactive mode\n\
-n, --use-first-dquot     use the first copy of duplicated structure\n\
-v, --verbose             print more information\n\
-d, --debug               print even more messages\n\
-m, --no-remount          do not remount filesystem read-only\n\
-M, --try-remount         try remounting filesystem read-only,\n\
                          continue even if it fails\n\
-R, --exclude-root        exclude root when checking all filesystems\n\
-F, --format=formatname   check quota files of specific format\n\
-a, --all                 check all filesystems\n\
-h, --help                display this message and exit\n\
-V, --version             display version information and exit\n\n"), progname);
	printf(_("Bugs to %s\n"), MY_EMAIL);
	exit(1);
}

static void parse_options(int argcnt, char **argstr)
{
	int ret;
	struct option long_opts[] = {
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ "backup", 0, NULL, 'b' },
		{ "create-files", 0, NULL, 'c' },
		{ "verbose", 0, NULL, 'v' },
		{ "debug", 0, NULL, 'd' },
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "interactive", 0, NULL, 'i' },
		{ "use-first-dquot", 0, NULL, 'n' },
		{ "force", 0, NULL, 'f' },
		{ "format", 1, NULL, 'F' },
		{ "no-remount", 0, NULL, 'm' },
		{ "try-remount", 0, NULL, 'M' },
		{ "exclude-root", 0, NULL, 'R' },
		{ "all", 0, NULL, 'a' },
		{ NULL, 0, NULL, 0 }
	};

	while ((ret = getopt_long(argcnt, argstr, "VhbcvugidnfF:mMRa", long_opts, NULL)) != -1) {
  	        switch (ret) {
		  case 'b':
  		          flags |= FL_BACKUPS;
			  break;
		  case 'g':
			  gwant = 1;
			  break;
		  case 'u':
			  uwant = 1;
			  break;
		  case 'd':
			  flags |= FL_DEBUG;
			  setlinebuf(stderr);
			  break;
		  case 'v':
			  if (flags & FL_VERBOSE)
				flags |= FL_VERYVERBOSE;
			  else
				flags |= FL_VERBOSE;
			  break;
		  case 'f':
			  flags |= FL_FORCE;
			  break;
		  case 'i':
			  flags |= FL_INTERACTIVE;
			  break;
		  case 'n':
			  flags |= FL_GUESSDQ;
			  break;
		  case 'c':
			  flags |= FL_NEWFILE;
			  break;
		  case 'V':
			  version();
			  exit(0);
		  case 'M':
			  flags |= FL_FORCEREMOUNT;
			  break;
		  case 'm':
			  flags |= FL_NOREMOUNT;
			  break;
		  case 'a':
			  flags |= FL_ALL;
			  break;
		  case 'R':
			  flags |= FL_NOROOT;
			  break;
		  case 'F':
			  if ((fmt = name2fmt(optarg)) == QF_ERROR)
				  exit(1);
			  break;
		  default:
			usage();
		}
	}
	if (!(uwant | gwant))
		uwant = 1;
	if ((argcnt == optind && !(flags & FL_ALL)) || (argcnt > optind && flags & FL_ALL)) {
		fputs(_("Bad number of arguments.\n"), stderr);
		usage();
	}
	if (fmt == QF_XFS) {
		fputs(_("XFS quota format needs no checking.\n"), stderr);
		exit(0);
	}
	if (flags & FL_VERBOSE && flags & FL_DEBUG)
		flags &= ~FL_VERBOSE;
	if (!(flags & FL_ALL))
		mntpoint = argstr[optind];
	else
		mntpoint = NULL;
}

#if defined(EXT2_DIRECT)
static int ext2_direct_scan(char *device)
{
	ext2_ino_t i_num;
	ext2_filsys fs;
	errcode_t error;
	ext2_inode_scan scan;
	struct ext2_inode inode;
	int inode_buffer_blocks = 0;
	ext2fs_inode_bitmap inode_used_map;
	ext2fs_inode_bitmap inode_dir_map;
	uid_t uid;
	gid_t gid;

	if ((error = ext2fs_open(device, 0, 0, 0, unix_io_manager, &fs))) {
		errstr(_("error (%d) while opening %s\n"), (int)error, device);
		return -1;
	}

	if ((error = ext2fs_allocate_inode_bitmap(fs, "in-use inode map", &inode_used_map))) {
		errstr(_("error (%d) while allocating inode file bitmap\n"), (int)error);
		return -1;
	}

	if ((error = ext2fs_allocate_inode_bitmap(fs, "directory inode map", &inode_dir_map))) {
		errstr(_("errstr (%d) while allocating inode directory bitmap\n"), (int)error);
		return -1;
	}

	if ((error = ext2fs_open_inode_scan(fs, inode_buffer_blocks, &scan))) {
		errstr(_("error (%d) while opening inode scan\n"), (int)error);
		return -1;
	}

	if ((error = ext2fs_get_next_inode(scan, &i_num, &inode))) {
		errstr(_("error (%d) while starting inode scan\n"), (int)error);
		return -1;
	}

	while ((long)i_num) {
		if (inode.i_links_count) {
			debug(FL_DEBUG, _("Found i_num %ld, blocks %ld\n"), (long)i_num, (long)inode.i_blocks);
			if (flags & FL_VERBOSE)
				blit(NULL);
			uid = inode.i_uid | (inode.i_uid_high << 16);
			gid = inode.i_gid | (inode.i_gid_high << 16);
			if (inode.i_uid_high | inode.i_gid_high)
				debug(FL_DEBUG, _("High uid detected.\n"));
			if (ucheck)
				add_to_quota(USRQUOTA, i_num, uid, gid,
					     inode.i_mode, inode.i_links_count,
					     ((loff_t)inode.i_blocks) << 9, 0);
			if (gcheck)
				add_to_quota(GRPQUOTA, i_num, uid, gid,
					     inode.i_mode, inode.i_links_count,
					     ((loff_t)inode.i_blocks) << 9, 0);
			if (S_ISDIR(inode.i_mode))
				dirs_done++;
			else
				files_done++;
		}

		if ((error = ext2fs_get_next_inode(scan, &i_num, &inode))) {
			errstr(_("Something weird happened while scanning. Error %d\n"), (int)error);
			return -1;
		}
	}
	return 0;
}
#endif

/*
 * Scan a directory with the readdir systemcall. Stat the files and add the sizes
 * of the files to the appropriate quotas. When we find a dir we recursivly call
 * ourself to scan that dir.
 */
static int scan_dir(char *pathname)
{
	struct dirs *dir_stack = NULL;
	struct dirs *new_dir;
	struct dirent *de;
	struct stat st;
	loff_t qspace;
	DIR *dp;
	int ret;

	if (lstat(pathname, &st) == -1) {
		errstr(_("Cannot stat directory %s: %s\n"), pathname, strerror(errno));
		goto out;
	}
	qspace = getqsize(pathname, &st);
	if (ucheck)
		add_to_quota(USRQUOTA, st.st_ino, st.st_uid, st.st_gid, st.st_mode,
			     st.st_nlink, qspace, 0);
	if (gcheck)
		add_to_quota(GRPQUOTA, st.st_ino, st.st_uid, st.st_gid, st.st_mode,
			     st.st_nlink, qspace, 0);

	if ((dp = opendir(pathname)) == (DIR *) NULL)
		die(2, _("\nCan open directory %s: %s\n"), pathname, strerror(errno));

	chdir(pathname);
	if (flags & FL_VERYVERBOSE)
		blit(pathname);
	while ((de = readdir(dp)) != (struct dirent *)NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		if (flags & FL_VERBOSE)
			blit(NULL);

		if ((lstat(de->d_name, &st)) == -1) {
			errstr(_("lstat Cannot stat `%s/%s': %s\nGuess you'd better run fsck first !\nexiting...\n"),
				pathname, de->d_name, strerror(errno));
			goto out;
		}

		if (S_ISDIR(st.st_mode)) {
			if (st.st_dev != cur_dev)
				continue;
			/*
			 * Add this to the directory stack and check this later on.
			 */
			debug(FL_DEBUG, _("pushd %s/%s\n"), pathname, de->d_name);
			new_dir = xmalloc(sizeof(struct dirs));

			new_dir->dir_name = xmalloc(strlen(pathname) + strlen(de->d_name) + 2);
			sprintf(new_dir->dir_name, "%s/%s", pathname, de->d_name);
			new_dir->next = dir_stack;
			dir_stack = new_dir;
		}
		else {
			qspace = getqsize(de->d_name, &st);
			if (ucheck)
				add_to_quota(USRQUOTA, st.st_ino, st.st_uid, st.st_gid, st.st_mode,
					     st.st_nlink, qspace, 1);
			if (gcheck)
				add_to_quota(GRPQUOTA, st.st_ino, st.st_uid, st.st_gid, st.st_mode,
					     st.st_nlink, qspace, 1);
			debug(FL_DEBUG, _("\tAdding %s size %lld ino %d links %d uid %u gid %u\n"), de->d_name,
			      (long long)st.st_size, (int)st.st_ino, (int)st.st_nlink, (int)st.st_uid, (int)st.st_gid);
			files_done++;
		}
	}
	closedir(dp);

	/*
	 * Traverse the directory stack, and check it.
	 */
	debug(FL_DEBUG, _("Scanning stored directories from directory stack\n"));
	while (dir_stack != (struct dirs *)NULL) {
		new_dir = dir_stack;
		dir_stack = dir_stack->next;
		debug(FL_DEBUG, _("popd %s\nEntering directory %s\n"), new_dir->dir_name,
		      new_dir->dir_name);
		ret = scan_dir(new_dir->dir_name);
		dirs_done++;
#ifdef DEBUG_MALLOC
		free_mem += sizeof(struct dirs) + strlen(new_dir->dir_name) + 1;
#endif
		free(new_dir->dir_name);
		free(new_dir);
		if (ret < 0)	/* Error while scanning? */
			goto out;
	}
	debug(FL_DEBUG, _("Leaving %s\n"), pathname);
	return 0;
      out:
	for (new_dir = dir_stack; new_dir; new_dir = dir_stack) {
		dir_stack = dir_stack->next;
#ifdef DEBUG_MALLOC
		free_mem += sizeof(struct dirs) + strlen(new_dir->dir_name) + 1;
#endif
		free(new_dir->dir_name);
		free(new_dir);
	}
	return -1;
}

/* Ask user y/n question */
int ask_yn(char *q, int def)
{
	char a[10];		/* Users answer */

	printf("%s [%c]: ", q, def ? 'y' : 'n');
	fflush(stdout);
	while (1) {
		fgets(a, sizeof(a)-1, stdin);
		if (a[0] == '\n')
			return def;
		if (!strcasecmp(a, "y\n"))
			return 1;
		if (!strcasecmp(a, "n\n"))
			return 0;
		printf("Illegal answer. Please answer y/n: ");
		fflush(stdout);
	}
}

/* Do checks and buffer quota file into memory */
static int process_file(struct mntent *mnt, int type)
{
	char *qfname = NULL;
	int fd = -1, ret;

	debug(FL_DEBUG, _("Going to check %s quota file of %s\n"), type2name(type),
	      mnt->mnt_dir);

	if (kern_quota_on(mnt->mnt_fsname, type, (1 << cfmt)) > 0) {	/* Is quota enabled? */
		if (!(flags & FL_FORCE)) {
			if (flags & FL_INTERACTIVE) {
				printf(_("Quota for %ss is enabled on mountpoint %s so quotacheck might damage the file.\n"), type2name(type), mnt->mnt_dir);
				if (!ask_yn(_("Should I continue"), 0)) {
					printf(_("As you wish... Canceling check of this file.\n"));
					return -1;
				}
			}
			else
				die(6, _("Quota for %ss is enabled on mountpoint %s so quotacheck might damage the file.\n\
Please turn quotas off or use -f to force checking.\n"),
				    type2name(type), mnt->mnt_dir);
		}
		/* At least sync quotas so damage will be smaller */
		if (quotactl(QCMD((kernel_iface == IFACE_GENERIC)? Q_SYNC : Q_6_5_SYNC, type),
			     mnt->mnt_fsname, 0, NULL) < 0)
			die(4, _("Error while syncing quotas on %s: %s\n"), mnt->mnt_fsname, strerror(errno));
	}

	if (!(flags & FL_NEWFILE)) {	/* Need to buffer file? */
		if (get_qf_name(mnt, type, (1 << cfmt), 0, &qfname) < 0) {
			errstr(_("Cannot get quotafile name for %s\n"), mnt->mnt_fsname);
			return -1;
		}
		if ((fd = open(qfname, O_RDONLY)) < 0) {
			if (errno != ENOENT) {
				errstr(_("Cannot open quotafile %s: %s\n"),
					qfname, strerror(errno));
				free(qfname);
				return -1;
			}
			/* When file was not found, just skip it */
			flags |= FL_NEWFILE;
			free(qfname);
			qfname = NULL;
		}
	}

	ret = 0;
	memset(old_info + type, 0, sizeof(old_info[type]));
	switch (cfmt) {
		case QF_TOONEW:
			errstr(_("Too new quotafile format on %s\n"), mnt->mnt_fsname);
			ret = -1;
			break;
		case QF_VFSOLD:
			ret = v1_buffer_file(qfname, fd, type);
			break;
		case QF_VFSV0:
			ret = v2_buffer_file(qfname, fd, type);
			break;
	}
	if (!(flags & FL_NEWFILE)) {
		free(qfname);
		close(fd);
	}
	return ret;
}

/* Backup old quotafile and rename new one to right name */
static int rename_files(struct mntent *mnt, int type)
{
	char *filename, newfilename[PATH_MAX];
	struct stat st;
	mode_t mode = S_IRUSR | S_IWUSR;
#ifdef HAVE_EXT2_INCLUDE
	long ext2_flags = -1;
	int fd;
#endif

	debug(FL_DEBUG, _("Renaming new files to proper names.\n"));
	if (get_qf_name(mnt, type, (1 << cfmt), 0, &filename) < 0)
		die(2, _("Cannot get name of old quotafile on %s.\n"), mnt->mnt_dir);
	if (stat(filename, &st) < 0) {	/* File doesn't exist? */
		if (errno == ENOENT) {
			debug(FL_DEBUG | FL_VERBOSE, _("Old file not found.\n"));
			goto rename_new;
		}
		errstr(_("Error while searching for old quota file %s: %s\n"),
			filename, strerror(errno));
		free(filename);
		return -1;
	}
	mode = st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
#ifdef HAVE_EXT2_INCLUDE
	if ((fd = open(filename, O_RDONLY)) < 0) {
		if (errno == ENOENT) {
			debug(FL_DEBUG | FL_VERBOSE, _("Old file found removed during check!\n"));
			goto rename_new;
		}
		errstr(_("Error while opening old quota file %s: %s\n"),
			filename, strerror(errno));
		free(filename);
		return -1;
	}
	if (ioctl(fd, EXT2_IOC_GETFLAGS, &ext2_flags) < 0)
		debug(FL_DEBUG, _("EXT2_IOC_GETFLAGS failed: %s\n"), strerror(errno));
	/* IMMUTABLE flag set probably because system crashed and quota was not properly
	 * turned off */
	if (ext2_flags & EXT2_IMMUTABLE_FL) {
		debug(FL_DEBUG | FL_VERBOSE, _("Quota file %s has IMMUTABLE flag set. Clearing.\n"), filename);
		ext2_flags &= ~EXT2_IMMUTABLE_FL;
		if (ioctl(fd, EXT2_IOC_SETFLAGS, &ext2_flags) < 0) {
			errstr(_("Failed to remove IMMUTABLE flag from quota file %s: %s\n"), filename, strerror(errno));
			free(filename);
			close(fd);
			return -1;
		}
	}
	close(fd);
#endif
	if (flags & FL_BACKUPS) {
		debug(FL_DEBUG, _("Renaming old quotafile to %s~\n"), filename);
		/* Backup old file */
		strcpy(newfilename, filename);
		/* Make backingup safe */
		sstrncat(newfilename, "~", PATH_MAX);
		if (newfilename[strlen(newfilename) - 1] != '~')
			die(8, _("Name of quota file too long. Contact %s.\n"), MY_EMAIL);
		if (rename(filename, newfilename) < 0) {
			errstr(_("Cannot rename old quotafile %s to %s: %s\n"),
				filename, newfilename, strerror(errno));
			free(filename);
			return -1;
		}
	}
	debug(FL_DEBUG, _("Renaming new quotafile\n"));
rename_new:
	/* Rename new file to right name */
	strcpy(newfilename, filename);
	sstrncat(newfilename, ".new", PATH_MAX);
	if (rename(newfilename, filename) < 0) {
		errstr(_("Cannot rename new quotafile %s to name %s: %s\n"),
			newfilename, filename, strerror(errno));
		free(filename);
		return -1;
	}
	if (chmod(filename, mode) < 0) {
		errstr(_("Cannot change permission of %s: %s\n"), filename, strerror(errno));
		free(filename);
		return -1;
	}
#ifdef HAVE_EXT2_INCLUDE
	if (ext2_flags != -1) {
		if ((fd = open(filename, O_RDONLY)) < 0) {
			errstr(_("Cannot open new quota file %s: %s\n"), filename, strerror(errno));
			free(filename);
			return -1;
		}
		if (ioctl(fd, EXT2_IOC_SETFLAGS, &ext2_flags) < 0)
			errstr(_("Warning: Cannot set EXT2 flags on %s: %s\n"), filename, strerror(errno));
		close(fd);
	}
#endif
	free(filename);
	return 0;
}

/*
 * Dump the quota info that we have in memory now to the appropriate
 * quota file. As quotafiles doesn't account to quotas we don't have to
 * bother about accounting new blocks for quota file
 */
static int dump_to_file(struct mntent *mnt, int type)
{
	struct dquot *dquot;
	uint i;
	struct quota_handle *h;

	debug(FL_DEBUG, _("Dumping gathered data for %ss.\n"), type2name(type));
	if (!(h = new_io(mnt, type, cfmt))) {
		errstr(_("Cannot initialize IO on new quotafile: %s\n"),
			strerror(errno));
		return -1;
	}
	if (!(flags & FL_NEWFILE)) {
		h->qh_info.dqi_bgrace = old_info[type].dqi_bgrace;
		h->qh_info.dqi_igrace = old_info[type].dqi_igrace;
		if (cfmt == QF_VFSV0)
			v2_merge_info(&h->qh_info, old_info + type);
		mark_quotafile_info_dirty(h);
	}
	for (i = 0; i < DQUOTHASHSIZE; i++)
		for (dquot = dquot_hash[type][i]; dquot; dquot = dquot->dq_next) {
			dquot->dq_h = h;
			update_grace_times(dquot);
			h->qh_ops->commit_dquot(dquot, COMMIT_ALL);
		}
	if (end_io(h) < 0) {
		errstr(_("Cannot finish IO on new quotafile: %s\n"), strerror(errno));
		return -1;
	}
	debug(FL_DEBUG, _("Data dumped.\n"));
	if (cfmt == kern_quota_on(mnt->mnt_fsname, type, 1 << cfmt)) {	/* Quota turned on? */
		char *filename;

		if (get_qf_name(mnt, type, 1 << cfmt, NF_FORMAT, &filename) < 0)
			errstr(_("Cannot find checked quota file for %ss on %s!\n"), type2name(type), mnt->mnt_fsname);
		else {
			if (quotactl(QCMD((kernel_iface == IFACE_GENERIC) ? Q_QUOTAOFF : Q_6_5_QUOTAOFF, type),
				     mnt->mnt_fsname, 0, NULL) < 0)
				errstr(_("Cannot turn %s quotas off on %s: %s\nKernel won't know about changes quotacheck did.\n"),
					type2name(type), mnt->mnt_fsname, strerror(errno));
			else {
				int ret;

				/* Rename files - if it fails we cannot do anything better then just turn on quotas again */
				rename_files(mnt, type);

				if (kernel_iface == IFACE_GENERIC)
					ret = quotactl(QCMD(Q_QUOTAON, type), mnt->mnt_fsname, util2kernfmt(cfmt), filename);
				else
					ret = quotactl(QCMD(Q_6_5_QUOTAON, type), mnt->mnt_fsname, 0, filename);
				if (ret < 0)
					errstr(_("Cannot turn %s quotas on on %s: %s\nKernel won't know about changes quotacheck did.\n"),
						type2name(type), mnt->mnt_fsname, strerror(errno));
			}
			free(filename);
		}
	}
	else
		if (rename_files(mnt, type) < 0)
		        return -1;
	return 0;
}

/* Substract space used by old quota file from usage */
static void sub_quota_file(struct mntent *mnt, int qtype, int ftype)
{
	char *filename;
	struct stat st;
	loff_t qspace;
	struct dquot *d;
	qid_t id;

	debug(FL_DEBUG, _("Substracting space used by old %s quota file.\n"), type2name(ftype));
	if (get_qf_name(mnt, ftype, 1 << cfmt, 0, &filename) < 0) {
		debug(FL_VERBOSE, _("Old %s file not found. Usage will not be substracted.\n"), type2name(ftype));
		return;
	}

	if (stat(filename, &st) < 0) {
		debug(FL_VERBOSE, _("Cannot stat old %s quota file: %s\n"), type2name(ftype), strerror(errno));
		free(filename);
		return;
	}
	qspace = getqsize(filename, &st);
	free(filename);
	
	if (qtype == USRQUOTA)
		id = st.st_uid;
	else
		id = st.st_gid;
	if ((d = lookup_dquot(id, qtype)) == NODQUOT) {
		errstr(_("Quota structure for %s owning quota file not present! Something is really wrong...\n"), type2name(qtype));
		return;
	}
	d->dq_dqb.dqb_curinodes--;
	d->dq_dqb.dqb_curspace -= qspace;
	debug(FL_DEBUG, _("Substracted %lu bytes.\n"), (unsigned long)qspace);
}

/* Buffer quotafile, run filesystem scan, dump quotafiles */
static void check_dir(struct mntent *mnt)
{
	struct stat st;
	int remounted = 0;

	if (lstat(mnt->mnt_dir, &st) < 0)
		die(2, _("Cannot stat mountpoint %s: %s\n"), mnt->mnt_dir, strerror(errno));
	if (!S_ISDIR(st.st_mode))
		die(2, _("Mountpoint %s is not a directory?!\n"), mnt->mnt_dir);
	cur_dev = st.st_dev;
	files_done = dirs_done = 0;
	if (ucheck)
		if (process_file(mnt, USRQUOTA) < 0)
			ucheck = 0;
	if (gcheck)
		if (process_file(mnt, GRPQUOTA) < 0)
			gcheck = 0;
	if (!ucheck && !gcheck)	/* Nothing to check? */
		return;
	if (!(flags & FL_NOREMOUNT)) {
		/* Now we try to remount fs read-only to prevent races when scanning filesystem */
		if (mount
		    (NULL, mnt->mnt_dir, mnt->mnt_type, MS_MGC_VAL | MS_REMOUNT | MS_RDONLY,
		     NULL) < 0 && !(flags & FL_FORCEREMOUNT)) {
			if (flags & FL_INTERACTIVE) {
				printf(_("Cannot remount filesystem mounted on %s read-only. Counted values might not be right.\n"), mnt->mnt_dir);
				if (!ask_yn(_("Should I continue"), 0)) {
					printf(_("As you wish... Canceling check of this file.\n"));
					goto out;
				}
			}
			else {
				errstr(_("Cannot remount filesystem mounted on %s read-only so counted values might not be right.\n\
Please stop all programs writing to filesystem or use -m flag to force checking.\n"), mnt->mnt_dir);
				goto out;
			}
		}
		else
			remounted = 1;
		debug(FL_DEBUG, _("Filesystem remounted read-only\n"));
	}
	debug(FL_VERBOSE, _("Scanning %s [%s] "), mnt->mnt_fsname, mnt->mnt_dir);
#if defined(EXT2_DIRECT)
	if (!strcmp(mnt->mnt_type, MNTTYPE_EXT2) || !strcmp(mnt->mnt_type, MNTTYPE_EXT3)) {
		if (ext2_direct_scan(mnt->mnt_fsname) < 0)
			goto out;
	}
	else {
#else
	if (mnt->mnt_dir) {
#endif
		if (flags & FL_VERYVERBOSE)
			putchar('\n');
		if (scan_dir(mnt->mnt_dir) < 0)
			goto out;
	}
	dirs_done++;
	if (flags & FL_VERBOSE || flags & FL_VERYVERBOSE)
		fputs(_("done\n"), stdout);
	if (ucheck) {
		sub_quota_file(mnt, USRQUOTA, USRQUOTA);
		sub_quota_file(mnt, USRQUOTA, GRPQUOTA);
	}
	if (gcheck) {
		sub_quota_file(mnt, GRPQUOTA, USRQUOTA);
		sub_quota_file(mnt, GRPQUOTA, GRPQUOTA);
	}
	debug(FL_DEBUG | FL_VERBOSE, _("Checked %d directories and %d files\n"), dirs_done,
	      files_done);
	if (remounted) {
		if (mount(NULL, mnt->mnt_dir, mnt->mnt_type, MS_MGC_VAL | MS_REMOUNT, NULL) < 0)
			die(4, _("Cannot remount filesystem %s read-write. cannot write new quota files.\n"), mnt->mnt_dir);
		debug(FL_DEBUG, _("Filesystem remounted RW.\n"));
	}
	if (ucheck)
		dump_to_file(mnt, USRQUOTA);
	if (gcheck)
		dump_to_file(mnt, GRPQUOTA);
out:
	remove_list();
}

/* Detect quota format from filename of present files */
static int detect_filename_format(struct mntent *mnt, int type)
{
	char *option;
	struct stat statbuf;
	char namebuf[PATH_MAX];
	int journal = 0;

	if (type == USRQUOTA) {
		if ((option = hasmntopt(mnt, MNTOPT_USRQUOTA)))
			option += strlen(MNTOPT_USRQUOTA);
		else if (hasmntopt(mnt, MNTOPT_USRJQUOTA)) {
			journal = 1;
			option += strlen(MNTOPT_USRJQUOTA);
		}
		else if ((option = hasmntopt(mnt, MNTOPT_QUOTA)))
			option += strlen(MNTOPT_QUOTA);
	}
	else {
		if ((option = hasmntopt(mnt, MNTOPT_GRPQUOTA)))
			option += strlen(MNTOPT_GRPQUOTA);
		else if (hasmntopt(mnt, MNTOPT_GRPJQUOTA)) {
			journal = 1;
			option += strlen(MNTOPT_GRPJQUOTA);
		}
	}
	if (!option)
		die(2, _("Cannot find quota option on filesystem %s with quotas!\n"), mnt->mnt_dir);
	if (journal) {
		int fmt;
		char fmtbuf[64], *space;
		
		if (!(option = hasmntopt(mnt, MNTOPT_JQFMT))) {
jquota_err:
			errstr(_("Cannot detect quota format for journalled quota on %s\n"), mnt->mnt_dir);
			return -1;
		}
		option += strlen(MNTOPT_JQFMT);
		if (*option != '=')
			goto jquota_err;
		space = strchr(option, ',');
		if (!space)
			space = option + strlen(option);
		if (space-option > sizeof(fmtbuf))
			goto jquota_err;
		sstrncpy(fmtbuf, option+1, space-option);
		if ((fmt = name2fmt(fmtbuf)) == QF_ERROR)
			goto jquota_err;
		return fmt;
	}
	else if (*option == '=')	/* If the file name is specified we can't detect quota format from it... */
		return -1;
	snprintf(namebuf, PATH_MAX, "%s/%s.%s", mnt->mnt_dir, basenames[QF_VFSV0], extensions[type]);
	if (!stat(namebuf, &statbuf))
		return QF_VFSV0;
	if (errno != ENOENT)
		return -1;
	snprintf(namebuf, PATH_MAX, "%s/%s.%s", mnt->mnt_dir, basenames[QF_VFSOLD], extensions[type]);
	if (!stat(namebuf, &statbuf))
		return QF_VFSOLD;
	/* Old quota files don't exist, just create newest quotafile available */
	if (kernel_formats & (1 << QF_VFSV0))
		return QF_VFSV0;
	if (kernel_formats & (1 << QF_VFSOLD))
		return QF_VFSOLD;
	return -1;
}

static void check_all(void)
{
	struct mntent *mnt;
	int checked = 0;
	static int warned;

	if (init_mounts_scan((flags & FL_ALL) ? 0 : 1, &mntpoint, 0) < 0)
		die(2, _("Cannot initialize mountpoint scan.\n"));
	while ((mnt = get_next_mount())) {
		if (flags & FL_ALL && flags & FL_NOROOT && !strcmp(mnt->mnt_dir, "/"))
			continue;
		if (!strcmp(mnt->mnt_type, MNTTYPE_XFS) || nfs_fstype(mnt->mnt_type) ||
		    meta_qf_fstype(mnt->mnt_type)) {
			debug(FL_DEBUG | FL_VERBOSE, _("Skipping %s [%s]\n"), mnt->mnt_fsname, mnt->mnt_dir);
			continue;
		}
		cfmt = fmt;
		if (uwant && hasquota(mnt, USRQUOTA, 0))
			ucheck = 1;
		else
			ucheck = 0;
		if (gwant && hasquota(mnt, GRPQUOTA, 0))
			gcheck = 1;
		else
			gcheck = 0;
		if (!ucheck && !gcheck)
			continue;
		if (cfmt == -1) {
			if ((cfmt = detect_filename_format(mnt, ucheck ? USRQUOTA : GRPQUOTA)) == -1) {
				errstr(_("Cannot guess format from filename on %s. Please specify format on commandline.\n"),
					mnt->mnt_fsname);
				continue;
			}
			debug(FL_DEBUG, _("Detected quota format %s\n"), fmt2name(cfmt));
		}

		if (flags & FL_VERBOSE && !hasmntopt(mnt, MNTOPT_USRJQUOTA) &&
		    !hasmntopt(mnt, MNTOPT_GRPJQUOTA) && !warned &&
		    (!strcmp(mnt->mnt_type, MNTTYPE_EXT3) ||
		     !strcmp(mnt->mnt_type, MNTTYPE_EXT4) ||
		     !strcmp(mnt->mnt_type, MNTTYPE_EXT4DEV) ||
		     !strcmp(mnt->mnt_type, MNTTYPE_REISER))) {
			struct utsname stats;

			/* Parse Linux kernel version and issue warning if not using
			 * journaled quotas. */
			warned = 1;
			if (uname(&stats) < 0)
				errstr(_("Cannot get system info: %s\n"),
					strerror(errno));
			else if (!strcmp(stats.sysname, "Linux")) {
				int v;
				char *errch;

				v = strtol(stats.release, &errch, 10);
				if (*errch == '.' && v >= 2) {
					v = strtol(errch + 1, &errch, 10);
					if (*errch == '.' && v >= 6) {
						v = strtol(errch + 1, &errch, 10);
						if (v >= 11)
							errstr(_("Your kernel probably supports journaled quota but you are not using it. Consider switching to journaled quota to avoid running quotacheck after an unclean shutdown.\n"));
					}
				}
			}
		}

		checked++;
		check_dir(mnt);
	}
	end_mounts_scan();
	if (!checked && (!(flags & FL_ALL) || flags & (FL_VERBOSE | FL_DEBUG)))
		errstr(_("Cannot find filesystem to check or filesystem not mounted with quota option.\n"));
}

int main(int argc, char **argv)
{
	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();

	check_all();
#ifdef DEBUG_MALLOC
	errstr(_("Allocated %d bytes memory\nFree'd %d bytes\nLost %d bytes\n"),
		malloc_mem, free_mem, malloc_mem - free_mem);
#endif
	return 0;
}
