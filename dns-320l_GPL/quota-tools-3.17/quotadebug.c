/*
 *
 *	Utility for quota debugging
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>

#include <sys/types.h>

#include <asm/byteorder.h>

#include "quotaio_v2.h"

#define PROMPTSTR "> "
#define MAXCMDLEN 64

int fd;

void die(int ret, char *fmtstr, ...)
{
	va_list args;

	va_start(args, fmtstr);
	fprintf(stderr, "quotadebug: ");
	vfprintf(stderr, fmtstr, args);
	va_end(args);
	exit(ret);
}

void sstrncpy(char *d, const char *s, size_t len)
{
	strncpy(d, s, len);
	d[len - 1] = 0;
}

void parse_options(int argcnt, char **argstr)
{
	char *slash = strrchr(argstr[0], '/'), cmdname[PATH_MAX];

	if (!slash)
		slash = argstr[0];
	else
		slash++;
	sstrncpy(cmdname, slash, PATH_MAX);
	if (argcnt != 2) {
		fputs("Bad number of arguments.\n", stderr);
usage:
		printf("Utility for quota debugging.\nUsage:\n%s file\nBugs to /dev/null\n", cmdname);
		exit(1);
	}
	if (!strcmp(argstr[1], "-h"))
		goto usage;
}

void print_help(void)
{
	puts("Simple utility for debugging quota.");
	puts("Commands:\nh - help\nq - quit\nptb <blk> - print block as tree block\npdq <blk> - print block as quota block");
	puts("pi - print base info\nww <blk> <off> <val> - write word to given block on given offset\nwl <blk> <off> <val> - write long ...");
}


void print_tb(char *arg)
{
	int blk, ret, i;
	char *errch;
	__u32 data[V2_DQBLKSIZE/sizeof(__u32)];

	if (!arg) {
		puts("Bad number of arguments");
		return;
	}
	blk = strtol(arg, &errch, 0);
	if (blk < 0 || *errch) {
		printf("Illegal block number %s\n", arg);
		return;
	}
	lseek(fd, blk * V2_DQBLKSIZE, SEEK_SET);
	ret = read(fd, data, V2_DQBLKSIZE);
	if (ret != V2_DQBLKSIZE) {
		if (ret >= 0)
			errno = ENODATA;
		printf("Error occured while reading block %d: %s\n", blk, strerror(errno));
		return;
	}
	for (i = 0; i < V2_DQBLKSIZE / sizeof(__u32); i++)
		printf("%08x ", __le32_to_cpu(data[i]));
	putchar('\n');
}

void print_quota(char *arg)
{
	int ret, blk, i, entries;
	char *errch;
	char buf[V2_DQBLKSIZE];
	struct v2_disk_dqdbheader *head;
	struct v2_disk_dqblk *entry;

	if (!arg) {
		puts("Bad number of arguments.");
		return;
	}
	blk = strtol(arg, &errch, 0);
	if (blk < 0 || *errch) {
		printf("Illegal block number: %s\n", arg);
		return;
	}
	lseek(fd, blk * V2_DQBLKSIZE, SEEK_SET);
	ret = read(fd, buf, V2_DQBLKSIZE);
	if (ret < 0) {
		printf("Error occured while reading block %d: %s\n", blk, strerror(errno));
		return;
	}
	else if (ret != V2_DQBLKSIZE)
		memset(buf + ret, 0, V2_DQBLKSIZE - ret);
	head = (struct v2_disk_dqdbheader *)buf;
	entry = (struct v2_disk_dqblk *)(buf + sizeof(struct v2_disk_dqdbheader));
	printf("HEAD:\nEntries: %d Nextblk: %x Prevblk: %x\n", entries = __le16_to_cpu(head->dqdh_entries),
	  __le32_to_cpu(head->dqdh_next_free), __le32_to_cpu(head->dqdh_prev_free));
	printf("ENTRIES:\n");
	for (i = 0; i < V2_DQSTRINBLK; i++)
		printf("ID: %d B: %Lu/%u(%Lu)/%u I: %u/%u(%Lu)/%u\n", __le32_to_cpu(entry[i].dqb_id),
		  __le64_to_cpu(entry[i].dqb_curspace), __le32_to_cpu(entry[i].dqb_bsoftlimit), __le64_to_cpu(entry[i].dqb_btime), __le32_to_cpu(entry[i].dqb_bhardlimit),
		  __le32_to_cpu(entry[i].dqb_curinodes), __le32_to_cpu(entry[i].dqb_isoftlimit), __le64_to_cpu(entry[i].dqb_itime), __le32_to_cpu(entry[i].dqb_ihardlimit));
}

void print_info(void)
{
	struct v2_disk_dqinfo info;
	int ret;

	lseek(fd, V2_DQINFOOFF, SEEK_SET);
	ret = read(fd, &info, sizeof(struct v2_disk_dqinfo));
	if (ret != sizeof(struct v2_disk_dqinfo)) {
		if (ret >= 0)
			errno = ENODATA;
		printf("Can't read base info: %s\n", strerror(errno));
		return;
	}
	printf("Flags: %08x\nBlocks: %d = %x\nFree block: %x\nFree entry: %x\nBGrace: %Lu\nIGrace: %Lu\n",
	  __le32_to_cpu(info.dqi_flags), __le32_to_cpu(info.dqi_blocks), __le32_to_cpu(info.dqi_blocks),
	  __le32_to_cpu(info.dqi_free_blk), __le32_to_cpu(info.dqi_free_entry), __le64_to_cpu(info.dqi_bgrace), __le64_to_cpu(info.dqi_igrace));
}

void write_bytes(int len, char *args)
{
	char *blks, *offs, *vals;
	uint blk, off, val;
	char *errch;
	int wrt;

	blks = args;
	if (!blks || !(offs = strchr(blks, ' ')) || !(vals = strchr(offs+1, ' '))) {
		puts("Bad number of arguments.");
		return;
	}
	*(offs++) = *(vals++) = 0;
	blk = strtol(blks, &errch, 0);
	if (*errch) {
		printf("Bad block number: %s\n", blks);
		return;
	}
	off = strtol(offs, &errch, 0);
	if (*errch || off > V2_DQBLKSIZE) {
		printf("Bad offset: %s\n", offs);
		return;
	}
	val = __cpu_to_le32(strtol(vals, &errch, 0));
	if (*errch) {
		printf("Bad value: %s\n", vals);
		return;
	}
	lseek(fd, (blk << V2_DQBLKSIZE_BITS) + off, SEEK_SET);
	if ((wrt = write(fd, &val, len)) < 0)
		printf("Can't write value: %s\n", strerror(errno));
	else if (wrt != len)
		printf("Warning: Written only %d bytes.\n", wrt);
}

void run(void)
{
	char cmd[MAXCMDLEN];
	char *arg;

	puts("Ready to serve.");
	while (1) {
		printf(PROMPTSTR);
		fgets(cmd, MAXCMDLEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';	/* Discard '\n' */
		if ((arg = strchr(cmd, ' ')))
			*(arg++) = '\0';
		if (!strcmp(cmd, "h"))
			print_help();
		else if (!strcmp(cmd, "q"))
			break;
		else if (!strcmp(cmd, "ptb"))
			print_tb(arg);
		else if (!strcmp(cmd, "pdq"))
			print_quota(arg);
		else if (!strcmp(cmd, "pi"))
			print_info();
		else if (!strcmp(cmd, "ww"))
			write_bytes(2, arg);
		else if (!strcmp(cmd, "wl"))
			write_bytes(4, arg);
		else
			puts("Unknown command. Type h for help.");
	}
}

int main(int argcnt, char **argstr)
{
	parse_options(argcnt, argstr);
	if ((fd = open(argstr[1], O_RDWR)) < 0)
		die(1, "Can't open given file: %s\n", strerror(errno));
	run();
	close(fd);
	return 0;
}

