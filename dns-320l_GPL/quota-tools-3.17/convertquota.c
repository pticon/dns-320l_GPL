/*
 *
 *	Utility for converting quota file from old to new format
 *
 *	Sponsored by SuSE CR
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include <asm/byteorder.h>

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quotasys.h"
#include "quota.h"
#include "bylabel.h"
#include "quotaio_v2.h"
#include "dqblk_v2.h"

#define ACT_FORMAT 1		/* Convert format from old to new */
#define ACT_ENDIAN 2		/* Convert endianity */

char *mntpoint;
char *progname;
int ucv, gcv;
struct quota_handle *qn;	/* Handle of new file */
int action;			/* Action to be performed */

static void usage(void)
{
	errstr(_("Utility for converting quota files.\nUsage:\n\t%s [options] mountpoint\n\n\
-u, --user            convert user quota file\n\
-g, --group           convert group quota file\n\
-e, --convert-endian  convert quota file to correct endianity\n\
-f, --convert-format  convert from old to VFSv0 quota format\n\
-h, --help            show this help text and exit\n\
-V, --version         output version information and exit\n\n"), progname);
	errstr(_("Bugs to %s\n"), MY_EMAIL);
	exit(1);
}

static void parse_options(int argcnt, char **argstr)
{
	int ret;
	struct option long_opts[] = {
		{ "help", 0, NULL, 'h'},
		{ "version", 0, NULL, 'V'},
		{ "user", 0, NULL, 'u'},
		{ "group", 0, NULL, 'g'},
		{ "convert-endian", 0, NULL, 'e'},
		{ "convert-format", 0, NULL, 'f'},
		{ NULL, 0, NULL, 0}
	};

	action = ACT_FORMAT;
	while ((ret = getopt_long(argcnt, argstr, "Vugefh:", long_opts, NULL)) != -1) {
		switch (ret) {
			case '?':
			case 'h':
				usage();
			case 'V':
				version();
				exit(0);
			case 'u':
				ucv = 1;
				break;
			case 'g':
				gcv = 1;
				break;
			case 'e':
				action = ACT_ENDIAN;
				break;
			case 'f':
				action = ACT_FORMAT;
				break;
		}
	}

	if (optind + 1 != argcnt) {
		puts(_("Bad number of arguments."));
		usage();
	}

	if (!(ucv | gcv))
		ucv = 1;

	mntpoint = argstr[optind];
}

/*
 *	Implementation of endian conversion
 */

typedef char *dqbuf_t;

#define set_bit(bmp, ind) ((bmp)[(ind) >> 3] |= (1 << ((ind) & 7)))
#define get_bit(bmp, ind) ((bmp)[(ind) >> 3] & (1 << ((ind) & 7)))

#define getdqbuf() smalloc(V2_DQBLKSIZE)
#define freedqbuf(buf) free(buf)

static inline void endian_disk2memdqblk(struct util_dqblk *m, struct v2_disk_dqblk *d)
{
	m->dqb_ihardlimit = __be32_to_cpu(d->dqb_ihardlimit);
	m->dqb_isoftlimit = __be32_to_cpu(d->dqb_isoftlimit);
	m->dqb_bhardlimit = __be32_to_cpu(d->dqb_bhardlimit);
	m->dqb_bsoftlimit = __be32_to_cpu(d->dqb_bsoftlimit);
	m->dqb_curinodes = __be32_to_cpu(d->dqb_curinodes);
	m->dqb_curspace = __be64_to_cpu(d->dqb_curspace);
	m->dqb_itime = __be64_to_cpu(d->dqb_itime);
	m->dqb_btime = __be64_to_cpu(d->dqb_btime);
}

/* Is given dquot empty? */
static int endian_empty_dquot(struct v2_disk_dqblk *d)
{
	static struct v2_disk_dqblk fakedquot;

	return !memcmp(d, &fakedquot, sizeof(fakedquot));
}

/* Read given block */
static void read_blk(int fd, uint blk, dqbuf_t buf)
{
	int err;

	lseek(fd, blk << V2_DQBLKSIZE_BITS, SEEK_SET);
	err = read(fd, buf, V2_DQBLKSIZE);
	if (err < 0)
		die(2, _("Cannot read block %u: %s\n"), blk, strerror(errno));
	else if (err != V2_DQBLKSIZE)
		memset(buf + err, 0, V2_DQBLKSIZE - err);
}

static void endian_report_block(int fd, uint blk, char *bitmap)
{
	dqbuf_t buf = getdqbuf();
	struct v2_disk_dqdbheader *dh;
	struct v2_disk_dqblk *ddata;
	struct dquot dquot;
	int i;

	set_bit(bitmap, blk);
	read_blk(fd, blk, buf);
	dh = (struct v2_disk_dqdbheader *)buf;
	ddata = V2_GETENTRIES(buf);
	for (i = 0; i < V2_DQSTRINBLK; i++)
		if (!endian_empty_dquot(ddata + i)) {
			memset(&dquot, 0, sizeof(dquot));
			dquot.dq_h = qn;
			endian_disk2memdqblk(&dquot.dq_dqb, ddata + i);
			dquot.dq_id = __be32_to_cpu(ddata[i].dqb_id);
			if (qn->qh_ops->commit_dquot(&dquot, COMMIT_ALL) < 0)
				errstr(_("Cannot commit dquot for id %u: %s\n"),
					(uint)dquot.dq_id, strerror(errno));
		}
	freedqbuf(buf);
}

static void endian_report_tree(int fd, uint blk, int depth, char *bitmap)
{
	int i;
	dqbuf_t buf = getdqbuf();
	u_int32_t *ref = (u_int32_t *) buf;

	read_blk(fd, blk, buf);
	if (depth == V2_DQTREEDEPTH - 1) {
		for (i = 0; i < V2_DQBLKSIZE >> 2; i++) {
			blk = __be32_to_cpu(ref[i]);
			if (blk && !get_bit(bitmap, blk))
				endian_report_block(fd, blk, bitmap);
		}
	}
	else {
		for (i = 0; i < V2_DQBLKSIZE >> 2; i++)
			if ((blk = __be32_to_cpu(ref[i])))
				endian_report_tree(fd, blk, depth + 1, bitmap);
	}
	freedqbuf(buf);
}

static int endian_scan_structures(int fd, int type)
{
	char *bitmap;
	loff_t blocks = (lseek(fd, 0, SEEK_END) + V2_DQBLKSIZE - 1) >> V2_DQBLKSIZE_BITS;

	bitmap = smalloc((blocks + 7) >> 3);
	memset(bitmap, 0, (blocks + 7) >> 3);
	endian_report_tree(fd, V2_DQTREEOFF, 0, bitmap);
	free(bitmap);
	return 0;
}

static int endian_check_header(int fd, int type)
{
	struct v2_disk_dqheader head;
	u_int32_t file_magics[] = INITQMAGICS;
	u_int32_t known_versions[] = INIT_V2_VERSIONS;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &head, sizeof(head)) != sizeof(head)) {
		errstr(_("Cannot read header of old quotafile.\n"));
		return -1;
	}
	if (__be32_to_cpu(head.dqh_magic) != file_magics[type] || __be32_to_cpu(head.dqh_version) > known_versions[type]) {
		errstr(_("Bad file magic or version (probably not quotafile with bad endianity).\n"));
		return -1;
	}
	return 0;
}

static int endian_load_info(int fd, int type)
{
	struct v2_disk_dqinfo dinfo;

	if (read(fd, &dinfo, sizeof(dinfo)) != sizeof(dinfo)) {
		errstr(_("Cannot read information about old quotafile.\n"));
		return -1;
	}
	qn->qh_info.u.v2_mdqi.dqi_flags = __be32_to_cpu(dinfo.dqi_flags);
	qn->qh_info.dqi_bgrace = __be32_to_cpu(dinfo.dqi_bgrace);
	qn->qh_info.dqi_igrace = __be32_to_cpu(dinfo.dqi_igrace);
	return 0;
}

/*
 *	End of endian conversion
 */

static int convert_dquot(struct dquot *dquot, char *name)
{
	struct dquot newdquot;

	memset(&newdquot, 0, sizeof(newdquot));
	newdquot.dq_id = dquot->dq_id;
	newdquot.dq_h = qn;
	newdquot.dq_dqb.dqb_ihardlimit = dquot->dq_dqb.dqb_ihardlimit;
	newdquot.dq_dqb.dqb_isoftlimit = dquot->dq_dqb.dqb_isoftlimit;
	newdquot.dq_dqb.dqb_curinodes = dquot->dq_dqb.dqb_curinodes;
	newdquot.dq_dqb.dqb_bhardlimit = dquot->dq_dqb.dqb_bhardlimit;
	newdquot.dq_dqb.dqb_bsoftlimit = dquot->dq_dqb.dqb_bsoftlimit;
	newdquot.dq_dqb.dqb_curspace = dquot->dq_dqb.dqb_curspace;
	newdquot.dq_dqb.dqb_btime = dquot->dq_dqb.dqb_btime;
	newdquot.dq_dqb.dqb_itime = dquot->dq_dqb.dqb_itime;
	if (qn->qh_ops->commit_dquot(&newdquot, COMMIT_ALL) < 0) {
		errstr(_("Cannot commit dquot for id %u: %s\n"),
			(uint)dquot->dq_id, strerror(errno));
		return -1;
	}
	return 0;
}

static int rename_file(int type, struct mntent *mnt)
{
	char *qfname, namebuf[PATH_MAX];
	int ret = 0;

	if (get_qf_name(mnt, type, (1 << QF_VFSV0), 0, &qfname) < 0) {
		errstr(_("Cannot get name of new quotafile.\n"));
		return -1;
	}
	strcpy(namebuf, qfname);
	sstrncat(namebuf, ".new", sizeof(namebuf));
	if (rename(namebuf, qfname) < 0) {
		errstr(_("Cannot rename new quotafile %s to name %s: %s\n"),
			namebuf, qfname, strerror(errno));
		ret = -1;
	}
	free(qfname);
	return ret;
}

static int convert_format(int type, struct mntent *mnt)
{
	struct quota_handle *qo;
	int ret = 0;
	
	if (!(qo = init_io(mnt, type, QF_VFSOLD, IOI_OPENFILE))) {
		errstr(_("Cannot open old format file for %ss on %s\n"),
			type2name(type), mnt->mnt_dir);
		return -1;
	}
	if (!(qn = new_io(mnt, type, QF_VFSV0))) {
		errstr(_("Cannot create file for %ss for new format on %s: %s\n"),
			type2name(type), mnt->mnt_dir, strerror(errno));
		end_io(qo);
		return -1;
	}
	if (qo->qh_ops->scan_dquots(qo, convert_dquot) >= 0)	/* Conversion succeeded? */
		ret = rename_file(type, mnt);
	else
		ret = -1;
	end_io(qo);
	end_io(qn);
	return ret;
}

static int convert_endian(int type, struct mntent *mnt)
{
	int ret = 0;
	int ofd;
	char *qfname;

	if (get_qf_name(mnt, type, (1 << QF_VFSV0), NF_EXIST, &qfname) < 0)
		return -1;
	if ((ofd = open(qfname, O_RDONLY)) < 0) {
		errstr(_("Cannot open old quota file on %s: %s\n"), mnt->mnt_dir, strerror(errno));
		free(qfname);
		return -1;
	}
	free(qfname);
	if (endian_check_header(ofd, type) < 0) {
		close(ofd);
		return -1;
	}
	if (!(qn = new_io(mnt, type, QF_VFSV0))) {
		errstr(_("Cannot create file for %ss for new format on %s: %s\n"),
			type2name(type), mnt->mnt_dir, strerror(errno));
		close(ofd);
		return -1;
	}
	if (endian_load_info(ofd, type) < 0) {
		end_io(qn);
		close(ofd);
		return -1;
	}
	ret = endian_scan_structures(ofd, type);
	end_io(qn);
	if (ret < 0)
		return ret;
	
	return rename_file(type, mnt);
}

static int convert_file(int type, struct mntent *mnt)
{
	switch (action) {
		case ACT_FORMAT:
			return convert_format(type, mnt);
		case ACT_ENDIAN:
			return convert_endian(type, mnt);
	}
	errstr(_("Unknown action should be performed.\n"));
	return -1;
}

int main(int argc, char **argv)
{
	struct mntent *mnt;
	int ret = 0;
	
	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();
	if (init_mounts_scan(1, &mntpoint, 0) < 0)
		return 1;
	if (!(mnt = get_next_mount())) {
		end_mounts_scan();
		return 1;
	}
	if (ucv)
		ret |= convert_file(USRQUOTA, mnt);
	if (gcv)
		ret |= convert_file(GRPQUOTA, mnt);
	end_mounts_scan();

	if (ret)
		return 1;
	return 0;
}
