/*
 *	Implementation of new quotafile format
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/byteorder.h>

#include "pot.h"
#include "common.h"
#include "quotaio_v2.h"
#include "dqblk_v2.h"
#include "quotaio.h"
#include "quotasys.h"
#include "quotaio_generic.h"

typedef char *dqbuf_t;

static int v2_check_file(int fd, int type);
static int v2_init_io(struct quota_handle *h);
static int v2_new_io(struct quota_handle *h);
static int v2_write_info(struct quota_handle *h);
static struct dquot *v2_read_dquot(struct quota_handle *h, qid_t id);
static int v2_commit_dquot(struct dquot *dquot, int flags);
static int v2_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *dquot, char *dqname));
static int v2_report(struct quota_handle *h, int verbose);

struct quotafile_ops quotafile_ops_2 = {
check_file:	v2_check_file,
init_io:	v2_init_io,
new_io:		v2_new_io,
write_info:	v2_write_info,
read_dquot:	v2_read_dquot,
commit_dquot:	v2_commit_dquot,
scan_dquots:	v2_scan_dquots,
report:	v2_report
};

#define getdqbuf() smalloc(V2_DQBLKSIZE)
#define freedqbuf(buf) free(buf)

/*
 *	Copy dquot from disk to memory
 */
static inline void v2_disk2memdqblk(struct util_dqblk *m, struct v2_disk_dqblk *d)
{
	m->dqb_ihardlimit = __le32_to_cpu(d->dqb_ihardlimit);
	m->dqb_isoftlimit = __le32_to_cpu(d->dqb_isoftlimit);
	m->dqb_bhardlimit = __le32_to_cpu(d->dqb_bhardlimit);
	m->dqb_bsoftlimit = __le32_to_cpu(d->dqb_bsoftlimit);
	m->dqb_curinodes = __le32_to_cpu(d->dqb_curinodes);
	m->dqb_curspace = __le64_to_cpu(d->dqb_curspace);
	m->dqb_itime = __le64_to_cpu(d->dqb_itime);
	m->dqb_btime = __le64_to_cpu(d->dqb_btime);
}

/*
 *	Copy dquot from memory to disk
 */
static inline void v2_mem2diskdqblk(struct v2_disk_dqblk *d, struct util_dqblk *m)
{
	d->dqb_ihardlimit = __cpu_to_le32(m->dqb_ihardlimit);
	d->dqb_isoftlimit = __cpu_to_le32(m->dqb_isoftlimit);
	d->dqb_bhardlimit = __cpu_to_le32(m->dqb_bhardlimit);
	d->dqb_bsoftlimit = __cpu_to_le32(m->dqb_bsoftlimit);
	d->dqb_curinodes = __cpu_to_le32(m->dqb_curinodes);
	d->dqb_curspace = __cpu_to_le64(m->dqb_curspace);
	d->dqb_itime = __cpu_to_le64(m->dqb_itime);
	d->dqb_btime = __cpu_to_le64(m->dqb_btime);
}

/*
 *	Copy dqinfo from disk to memory
 */
static inline void v2_disk2memdqinfo(struct util_dqinfo *m, struct v2_disk_dqinfo *d)
{
	m->dqi_bgrace = __le32_to_cpu(d->dqi_bgrace);
	m->dqi_igrace = __le32_to_cpu(d->dqi_igrace);
	m->u.v2_mdqi.dqi_flags = __le32_to_cpu(d->dqi_flags) & V2_DQF_MASK;
	m->u.v2_mdqi.dqi_blocks = __le32_to_cpu(d->dqi_blocks);
	m->u.v2_mdqi.dqi_free_blk = __le32_to_cpu(d->dqi_free_blk);
	m->u.v2_mdqi.dqi_free_entry = __le32_to_cpu(d->dqi_free_entry);
}

/*
 *	Copy dqinfo from memory to disk
 */
static inline void v2_mem2diskdqinfo(struct v2_disk_dqinfo *d, struct util_dqinfo *m)
{
	d->dqi_bgrace = __cpu_to_le32(m->dqi_bgrace);
	d->dqi_igrace = __cpu_to_le32(m->dqi_igrace);
	d->dqi_flags = __cpu_to_le32(m->u.v2_mdqi.dqi_flags & V2_DQF_MASK);
	d->dqi_blocks = __cpu_to_le32(m->u.v2_mdqi.dqi_blocks);
	d->dqi_free_blk = __cpu_to_le32(m->u.v2_mdqi.dqi_free_blk);
	d->dqi_free_entry = __cpu_to_le32(m->u.v2_mdqi.dqi_free_entry);
}

/* Convert kernel quotablock format to utility one */
static inline void v2_kern2utildqblk(struct util_dqblk *u, struct v2_kern_dqblk *k)
{
	u->dqb_ihardlimit = k->dqb_ihardlimit;
	u->dqb_isoftlimit = k->dqb_isoftlimit;
	u->dqb_bhardlimit = k->dqb_bhardlimit;
	u->dqb_bsoftlimit = k->dqb_bsoftlimit;
	u->dqb_curinodes = k->dqb_curinodes;
	u->dqb_curspace = k->dqb_curspace;
	u->dqb_itime = k->dqb_itime;
	u->dqb_btime = k->dqb_btime;
}

/* Convert utility quotablock format to kernel one */
static inline void v2_util2kerndqblk(struct v2_kern_dqblk *k, struct util_dqblk *u)
{
	k->dqb_ihardlimit = u->dqb_ihardlimit;
	k->dqb_isoftlimit = u->dqb_isoftlimit;
	k->dqb_bhardlimit = u->dqb_bhardlimit;
	k->dqb_bsoftlimit = u->dqb_bsoftlimit;
	k->dqb_curinodes = u->dqb_curinodes;
	k->dqb_curspace = u->dqb_curspace;
	k->dqb_itime = u->dqb_itime;
	k->dqb_btime = u->dqb_btime;
}

/* Is given dquot empty? */
static int empty_dquot(struct v2_disk_dqblk *d)
{
	static struct v2_disk_dqblk fakedquot;

	return !memcmp(d, &fakedquot, sizeof(fakedquot));
}

/*
 *	Check whether given quota file is in our format
 */
static int v2_check_file(int fd, int type)
{
	struct v2_disk_dqheader h;
	int file_magics[] = INITQMAGICS;
	int known_versions[] = INIT_V2_VERSIONS;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &h, sizeof(h)) != sizeof(h))
		return 0;
	if (__le32_to_cpu(h.dqh_magic) != file_magics[type]) {
		if (__be32_to_cpu(h.dqh_magic) == file_magics[type])
			die(3, _("Your quota file is stored in wrong endianity. Please use convertquota(8) to convert it.\n"));
		return 0;
	}
	if (__le32_to_cpu(h.dqh_version) > known_versions[type])
		return 0;
	return 1;
}

/*
 *	Open quotafile
 */
static int v2_init_io(struct quota_handle *h)
{
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_info(h) < 0)
				return -1;
		}
		else {
			struct v2_kern_dqinfo kdqinfo;

			if (quotactl(QCMD(Q_V2_GETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0) {
				/* Temporary check just before fix gets to kernel */
				if (errno == EPERM)	/* Don't have permission to get information? */
					return 0;
				return -1;
			}
			h->qh_info.dqi_bgrace = kdqinfo.dqi_bgrace;
			h->qh_info.dqi_igrace = kdqinfo.dqi_igrace;
			h->qh_info.u.v2_mdqi.dqi_flags = kdqinfo.dqi_flags;
			h->qh_info.u.v2_mdqi.dqi_blocks = kdqinfo.dqi_blocks;
			h->qh_info.u.v2_mdqi.dqi_free_blk = kdqinfo.dqi_free_blk;
			h->qh_info.u.v2_mdqi.dqi_free_entry = kdqinfo.dqi_free_entry;
		}
	}
	else {
		struct v2_disk_dqinfo ddqinfo;

		lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
		if (read(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
			return -1;
		v2_disk2memdqinfo(&h->qh_info, &ddqinfo);
	}
	return 0;
}

/*
 *	Initialize new quotafile
 */
static int v2_new_io(struct quota_handle *h)
{
	int file_magics[] = INITQMAGICS;
	int known_versions[] = INIT_V2_VERSIONS;
	struct v2_disk_dqheader ddqheader;
	struct v2_disk_dqinfo ddqinfo;

	/* Write basic quota header */
	ddqheader.dqh_magic = __cpu_to_le32(file_magics[h->qh_type]);
	ddqheader.dqh_version = __cpu_to_le32(known_versions[h->qh_type]);
	lseek(h->qh_fd, 0, SEEK_SET);
	if (write(h->qh_fd, &ddqheader, sizeof(ddqheader)) != sizeof(ddqheader))
		return -1;
	/* Write information about quotafile */
	h->qh_info.dqi_bgrace = MAX_DQ_TIME;
	h->qh_info.dqi_igrace = MAX_IQ_TIME;
	h->qh_info.u.v2_mdqi.dqi_flags = 0;
	h->qh_info.u.v2_mdqi.dqi_blocks = V2_DQTREEOFF + 1;
	h->qh_info.u.v2_mdqi.dqi_free_blk = 0;
	h->qh_info.u.v2_mdqi.dqi_free_entry = 0;
	v2_mem2diskdqinfo(&ddqinfo, &h->qh_info);
	lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
	if (write(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
		return -1;
	return 0;
}

/*
 *	Write information (grace times to file)
 */
static int v2_write_info(struct quota_handle *h)
{
	if (QIO_RO(h)) {
		errstr(_("Trying to write info to readonly quotafile on %s\n"), h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_info(h, IIF_BGRACE | IIF_IGRACE))
				return -1;
		}
		else {
			struct v2_kern_dqinfo kdqinfo;

			kdqinfo.dqi_bgrace = h->qh_info.dqi_bgrace;
			kdqinfo.dqi_igrace = h->qh_info.dqi_igrace;
			kdqinfo.dqi_flags = h->qh_info.u.v2_mdqi.dqi_flags;
			kdqinfo.dqi_blocks = h->qh_info.u.v2_mdqi.dqi_blocks;
			kdqinfo.dqi_free_blk = h->qh_info.u.v2_mdqi.dqi_free_blk;
			kdqinfo.dqi_free_entry = h->qh_info.u.v2_mdqi.dqi_free_entry;
			if (quotactl(QCMD(Q_V2_SETGRACE, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0 ||
			    quotactl(QCMD(Q_V2_SETFLAGS, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0)
					return -1;
		}
	}
	else {
		struct v2_disk_dqinfo ddqinfo;

		v2_mem2diskdqinfo(&ddqinfo, &h->qh_info);
		lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
		if (write(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
			return -1;
	}
	return 0;
}

/* Read given block */
static void read_blk(struct quota_handle *h, uint blk, dqbuf_t buf)
{
	int err;

	lseek(h->qh_fd, blk << V2_DQBLKSIZE_BITS, SEEK_SET);
	err = read(h->qh_fd, buf, V2_DQBLKSIZE);
	if (err < 0)
		die(2, _("Cannot read block %u: %s\n"), blk, strerror(errno));
	else if (err != V2_DQBLKSIZE)
		memset(buf + err, 0, V2_DQBLKSIZE - err);
}

/* Write block */
static int write_blk(struct quota_handle *h, uint blk, dqbuf_t buf)
{
	int err;

	lseek(h->qh_fd, blk << V2_DQBLKSIZE_BITS, SEEK_SET);
	err = write(h->qh_fd, buf, V2_DQBLKSIZE);
	if (err < 0 && errno != ENOSPC)
		die(2, _("Cannot write block (%u): %s\n"), blk, strerror(errno));
	if (err != V2_DQBLKSIZE)
		return -ENOSPC;
	return 0;
}

/* Get free block in file (either from free list or create new one) */
static int get_free_dqblk(struct quota_handle *h)
{
	dqbuf_t buf = getdqbuf();
	struct v2_disk_dqdbheader *dh = (struct v2_disk_dqdbheader *)buf;
	struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;
	int blk;

	if (info->dqi_free_blk) {
		blk = info->dqi_free_blk;
		read_blk(h, blk, buf);
		info->dqi_free_blk = __le32_to_cpu(dh->dqdh_next_free);
	}
	else {
		memset(buf, 0, V2_DQBLKSIZE);
		if (write_blk(h, info->dqi_blocks, buf) < 0) {	/* Assure block allocation... */
			freedqbuf(buf);
			errstr(_("Cannot allocate new quota block (out of disk space).\n"));
			return -ENOSPC;
		}
		blk = info->dqi_blocks++;
	}
	mark_quotafile_info_dirty(h);
	freedqbuf(buf);
	return blk;
}

/* Put given block to free list */
static void put_free_dqblk(struct quota_handle *h, dqbuf_t buf, uint blk)
{
	struct v2_disk_dqdbheader *dh = (struct v2_disk_dqdbheader *)buf;
	struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;

	dh->dqdh_next_free = __cpu_to_le32(info->dqi_free_blk);
	dh->dqdh_prev_free = __cpu_to_le32(0);
	dh->dqdh_entries = __cpu_to_le16(0);
	info->dqi_free_blk = blk;
	mark_quotafile_info_dirty(h);
	write_blk(h, blk, buf);
}

/* Remove given block from the list of blocks with free entries */
static void remove_free_dqentry(struct quota_handle *h, dqbuf_t buf, uint blk)
{
	dqbuf_t tmpbuf = getdqbuf();
	struct v2_disk_dqdbheader *dh = (struct v2_disk_dqdbheader *)buf;
	uint nextblk = __le32_to_cpu(dh->dqdh_next_free), prevblk =

		__le32_to_cpu(dh->dqdh_prev_free);

	if (nextblk) {
		read_blk(h, nextblk, tmpbuf);
		((struct v2_disk_dqdbheader *)tmpbuf)->dqdh_prev_free = dh->dqdh_prev_free;
		write_blk(h, nextblk, tmpbuf);
	}
	if (prevblk) {
		read_blk(h, prevblk, tmpbuf);
		((struct v2_disk_dqdbheader *)tmpbuf)->dqdh_next_free = dh->dqdh_next_free;
		write_blk(h, prevblk, tmpbuf);
	}
	else {
		h->qh_info.u.v2_mdqi.dqi_free_entry = nextblk;
		mark_quotafile_info_dirty(h);
	}
	freedqbuf(tmpbuf);
	dh->dqdh_next_free = dh->dqdh_prev_free = __cpu_to_le32(0);
	write_blk(h, blk, buf);	/* No matter whether write succeeds block is out of list */
}

/* Insert given block to the beginning of list with free entries */
static void insert_free_dqentry(struct quota_handle *h, dqbuf_t buf, uint blk)
{
	dqbuf_t tmpbuf = getdqbuf();
	struct v2_disk_dqdbheader *dh = (struct v2_disk_dqdbheader *)buf;
	struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;

	dh->dqdh_next_free = __cpu_to_le32(info->dqi_free_entry);
	dh->dqdh_prev_free = __cpu_to_le32(0);
	write_blk(h, blk, buf);
	if (info->dqi_free_entry) {
		read_blk(h, info->dqi_free_entry, tmpbuf);
		((struct v2_disk_dqdbheader *)tmpbuf)->dqdh_prev_free = __cpu_to_le32(blk);
		write_blk(h, info->dqi_free_entry, tmpbuf);
	}
	freedqbuf(tmpbuf);
	info->dqi_free_entry = blk;
	mark_quotafile_info_dirty(h);
}

/* Find space for dquot */
static uint find_free_dqentry(struct quota_handle *h, struct dquot *dquot, int *err)
{
	int blk, i;
	struct v2_disk_dqdbheader *dh;
	struct v2_disk_dqblk *ddquot;
	struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;
	dqbuf_t buf;

	*err = 0;
	buf = getdqbuf();
	dh = (struct v2_disk_dqdbheader *)buf;
	ddquot = V2_GETENTRIES(buf);
	if (info->dqi_free_entry) {
		blk = info->dqi_free_entry;
		read_blk(h, blk, buf);
	}
	else {
		blk = get_free_dqblk(h);
		if (blk < 0) {
			freedqbuf(buf);
			*err = blk;
			return 0;
		}
		memset(buf, 0, V2_DQBLKSIZE);
		info->dqi_free_entry = blk;
		mark_quotafile_info_dirty(h);
	}
	if (__le16_to_cpu(dh->dqdh_entries) + 1 >= V2_DQSTRINBLK)	/* Block will be full? */
		remove_free_dqentry(h, buf, blk);
	dh->dqdh_entries = __cpu_to_le16(__le16_to_cpu(dh->dqdh_entries) + 1);
	/* Find free structure in block */
	for (i = 0; i < V2_DQSTRINBLK && !empty_dquot(ddquot + i); i++);
	if (i == V2_DQSTRINBLK)
		die(2, _("find_free_dqentry(): Data block full but it shouldn't.\n"));
	write_blk(h, blk, buf);
	dquot->dq_dqb.u.v2_mdqb.dqb_off =
		(blk << V2_DQBLKSIZE_BITS) + sizeof(struct v2_disk_dqdbheader) +

		i * sizeof(struct v2_disk_dqblk);
	freedqbuf(buf);
	return blk;
}

/* Insert reference to structure into the trie */
static int do_insert_tree(struct quota_handle *h, struct dquot *dquot, uint * treeblk, int depth)
{
	dqbuf_t buf;
	int newson = 0, newact = 0;
	u_int32_t *ref;
	uint newblk;
	int ret = 0;

	buf = getdqbuf();
	if (!*treeblk) {
		ret = get_free_dqblk(h);
		if (ret < 0)
			goto out_buf;
		*treeblk = ret;
		memset(buf, 0, V2_DQBLKSIZE);
		newact = 1;
	}
	else
		read_blk(h, *treeblk, buf);
	ref = (u_int32_t *) buf;
	newblk = __le32_to_cpu(ref[V2_GETIDINDEX(dquot->dq_id, depth)]);
	if (!newblk)
		newson = 1;
	if (depth == V2_DQTREEDEPTH - 1) {
		if (newblk)
			die(2, _("Inserting already present quota entry (block %u).\n"),
			    ref[V2_GETIDINDEX(dquot->dq_id, depth)]);
		newblk = find_free_dqentry(h, dquot, &ret);
	}
	else
		ret = do_insert_tree(h, dquot, &newblk, depth + 1);
	if (newson && ret >= 0) {
		ref[V2_GETIDINDEX(dquot->dq_id, depth)] = __cpu_to_le32(newblk);
		write_blk(h, *treeblk, buf);
	}
	else if (newact && ret < 0)
		put_free_dqblk(h, buf, *treeblk);
      out_buf:
	freedqbuf(buf);
	return ret;
}

/* Wrapper for inserting quota structure into tree */
static inline void dq_insert_tree(struct quota_handle *h, struct dquot *dquot)
{
	uint tmp = V2_DQTREEOFF;

	if (do_insert_tree(h, dquot, &tmp, 0) < 0)
		die(2, _("Cannot write quota (id %u): %s\n"), (uint) dquot->dq_id, strerror(errno));
}

/* Write dquot to file */
static void v2_write_dquot(struct dquot *dquot)
{
	ssize_t ret;
	struct v2_disk_dqblk ddquot;

	if (!dquot->dq_dqb.u.v2_mdqb.dqb_off)
		dq_insert_tree(dquot->dq_h, dquot);
	lseek(dquot->dq_h->qh_fd, dquot->dq_dqb.u.v2_mdqb.dqb_off, SEEK_SET);
	v2_mem2diskdqblk(&ddquot, &dquot->dq_dqb);
	ddquot.dqb_id = __cpu_to_le32(dquot->dq_id);
	ret = write(dquot->dq_h->qh_fd, (char *)&ddquot, sizeof(struct v2_disk_dqblk));
	if (ret != sizeof(struct v2_disk_dqblk)) {
		if (ret > 0)
			errno = ENOSPC;
		die(2, _("Quota write failed (id %u): %s\n"), (uint) dquot->dq_id, strerror(errno));
	}
}

/* Free dquot entry in data block */
static void free_dqentry(struct quota_handle *h, struct dquot *dquot, uint blk)
{
	struct v2_disk_dqdbheader *dh;
	dqbuf_t buf = getdqbuf();

	if (dquot->dq_dqb.u.v2_mdqb.dqb_off >> V2_DQBLKSIZE_BITS != blk)
		die(2, _("Quota structure has offset to other block (%u) than it should (%u).\n"), blk,
		    (uint) (dquot->dq_dqb.u.v2_mdqb.dqb_off >> V2_DQBLKSIZE_BITS));
	read_blk(h, blk, buf);
	dh = (struct v2_disk_dqdbheader *)buf;
	dh->dqdh_entries = __cpu_to_le16(__le16_to_cpu(dh->dqdh_entries) - 1);
	if (!__le16_to_cpu(dh->dqdh_entries)) {	/* Block got free? */
		remove_free_dqentry(h, buf, blk);
		put_free_dqblk(h, buf, blk);
	}
	else {
		memset(buf + (dquot->dq_dqb.u.v2_mdqb.dqb_off & ((1 << V2_DQBLKSIZE_BITS) - 1)), 0,
		       sizeof(struct v2_disk_dqblk));

		if (__le16_to_cpu(dh->dqdh_entries) == V2_DQSTRINBLK - 1)	/* First free entry? */
			insert_free_dqentry(h, buf, blk);	/* This will also write data block */
		else
			write_blk(h, blk, buf);
	}
	dquot->dq_dqb.u.v2_mdqb.dqb_off = 0;
	freedqbuf(buf);
}

/* Remove reference to dquot from tree */
static void remove_tree(struct quota_handle *h, struct dquot *dquot, uint * blk, int depth)
{
	dqbuf_t buf = getdqbuf();
	uint newblk;
	u_int32_t *ref = (u_int32_t *) buf;

	read_blk(h, *blk, buf);
	newblk = __le32_to_cpu(ref[V2_GETIDINDEX(dquot->dq_id, depth)]);
	if (depth == V2_DQTREEDEPTH - 1) {
		free_dqentry(h, dquot, newblk);
		newblk = 0;
	}
	else
		remove_tree(h, dquot, &newblk, depth + 1);
	if (!newblk) {
		int i;

		ref[V2_GETIDINDEX(dquot->dq_id, depth)] = __cpu_to_le32(0);
		for (i = 0; i < V2_DQBLKSIZE && !buf[i]; i++);	/* Block got empty? */
		/* Don't put the root block into the free block list */
		if (i == V2_DQBLKSIZE && *blk != V2_DQTREEOFF) {
			put_free_dqblk(h, buf, *blk);
			*blk = 0;
		}
		else
			write_blk(h, *blk, buf);
	}
	freedqbuf(buf);
}

/* Delete dquot from tree */
static void v2_delete_dquot(struct dquot *dquot)
{
	uint tmp = V2_DQTREEOFF;

	if (!dquot->dq_dqb.u.v2_mdqb.dqb_off)	/* Even not allocated? */
		return;
	remove_tree(dquot->dq_h, dquot, &tmp, 0);
}

/* Find entry in block */
static loff_t find_block_dqentry(struct quota_handle *h, struct dquot *dquot, uint blk)
{
	dqbuf_t buf = getdqbuf();
	int i;
	struct v2_disk_dqblk *ddquot = V2_GETENTRIES(buf);

	read_blk(h, blk, buf);
	if (dquot->dq_id)
		for (i = 0; i < V2_DQSTRINBLK && __le32_to_cpu(ddquot[i].dqb_id) != dquot->dq_id;
		     i++);
	else {			/* ID 0 as a bit more complicated searching... */
		for (i = 0; i < V2_DQSTRINBLK; i++)
			if (!__le32_to_cpu(ddquot[i].dqb_id) && !empty_dquot(ddquot + i))
				break;
	}
	if (i == V2_DQSTRINBLK)
		die(2, _("Quota for id %u referenced but not present.\n"), dquot->dq_id);
	freedqbuf(buf);
	return (blk << V2_DQBLKSIZE_BITS) + sizeof(struct v2_disk_dqdbheader) +

		i * sizeof(struct v2_disk_dqblk);
}

/* Find entry for given id in the tree */
static loff_t find_tree_dqentry(struct quota_handle *h, struct dquot *dquot, uint blk, int depth)
{
	dqbuf_t buf = getdqbuf();
	loff_t ret = 0;
	u_int32_t *ref = (u_int32_t *) buf;

	read_blk(h, blk, buf);
	ret = 0;
	blk = __le32_to_cpu(ref[V2_GETIDINDEX(dquot->dq_id, depth)]);
	if (!blk)		/* No reference? */
		goto out_buf;
	if (depth < V2_DQTREEDEPTH - 1)
		ret = find_tree_dqentry(h, dquot, blk, depth + 1);
	else
		ret = find_block_dqentry(h, dquot, blk);
      out_buf:
	freedqbuf(buf);
	return ret;
}

/* Find entry for given id in the tree - wrapper function */
static inline loff_t find_dqentry(struct quota_handle *h, struct dquot *dquot)
{
	return find_tree_dqentry(h, dquot, V2_DQTREEOFF, 0);
}

/*
 *  Read dquot (either from disk or from kernel)
 *  User can use errno to detect errstr when NULL is returned
 */
static struct dquot *v2_read_dquot(struct quota_handle *h, qid_t id)
{
	loff_t offset;
	ssize_t ret;
	struct v2_disk_dqblk ddquot;
	struct dquot *dquot = get_empty_dquot();

	dquot->dq_id = id;
	dquot->dq_h = h;
	dquot->dq_dqb.u.v2_mdqb.dqb_off = 0;
	memset(&dquot->dq_dqb, 0, sizeof(struct util_dqblk));

	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_dquot(dquot) < 0) {
				free(dquot);
				return NULL;
			}
		}
		else {
			struct v2_kern_dqblk kdqblk;

			if (quotactl(QCMD(Q_V2_GETQUOTA, h->qh_type), h->qh_quotadev, id, (void *)&kdqblk) < 0) {
				free(dquot);
				return NULL;
			}
			v2_kern2utildqblk(&dquot->dq_dqb, &kdqblk);
		}
		return dquot;
	}
	offset = find_dqentry(h, dquot);
	if (offset > 0) {
		dquot->dq_dqb.u.v2_mdqb.dqb_off = offset;
		lseek(h->qh_fd, offset, SEEK_SET);
		ret = read(h->qh_fd, (char *)&ddquot, sizeof(struct v2_disk_dqblk));
		if (ret != sizeof(struct v2_disk_dqblk)) {
			if (ret > 0)
				errno = EIO;
			die(2, _("Cannot read quota structure for id %u: %s\n"), dquot->dq_id,
			    strerror(errno));
		}
		v2_disk2memdqblk(&dquot->dq_dqb, &ddquot);
		/* Unescape all-zero structure (it can be on disk after a crash) */
		if (!dquot->dq_id && !dquot->dq_dqb.dqb_bhardlimit && !dquot->dq_dqb.dqb_bsoftlimit &&
		    !dquot->dq_dqb.dqb_curspace && !dquot->dq_dqb.dqb_ihardlimit && !dquot->dq_dqb.dqb_isoftlimit &&
		    !dquot->dq_dqb.dqb_curinodes && !dquot->dq_dqb.dqb_btime && dquot->dq_dqb.dqb_itime == 1)
			dquot->dq_dqb.dqb_itime = 0;
	}
	return dquot;
}

/* 
 *  Commit changes of dquot to disk - it might also mean deleting it when quota became fake one and user has no blocks...
 *  User can process use 'errno' to detect errstr
 */
static int v2_commit_dquot(struct dquot *dquot, int flags)
{
	struct util_dqblk *b = &dquot->dq_dqb;

	if (QIO_RO(dquot->dq_h)) {
		errstr(_("Trying to write quota to readonly quotafile on %s\n"), dquot->dq_h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(dquot->dq_h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_dquot(dquot, flags) < 0)
				return -1;
		}
		else {
			struct v2_kern_dqblk kdqblk;
			int cmd;

			if (flags == COMMIT_USAGE)
				cmd = Q_V2_SETUSE;
			else if (flags == COMMIT_LIMITS)
				cmd = Q_V2_SETQLIM;
			else if (flags & COMMIT_TIMES) {
				errno = EINVAL;
				return -1;
			}
			else
				cmd = Q_V2_SETQUOTA;
			v2_util2kerndqblk(&kdqblk, &dquot->dq_dqb);
			if (quotactl(QCMD(cmd, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev,
			     dquot->dq_id, (void *)&kdqblk) < 0)
				return -1;
		}
		return 0;
	}
	if (!b->dqb_curspace && !b->dqb_curinodes && !b->dqb_bsoftlimit && !b->dqb_isoftlimit
	    && !b->dqb_bhardlimit && !b->dqb_ihardlimit)
		v2_delete_dquot(dquot);
	else
		v2_write_dquot(dquot);
	return 0;
}

/*
 *	Scan all dquots in file and call callback on each
 */
#define set_bit(bmp, ind) ((bmp)[(ind) >> 3] |= (1 << ((ind) & 7)))
#define get_bit(bmp, ind) ((bmp)[(ind) >> 3] & (1 << ((ind) & 7)))

static int report_block(struct dquot *dquot, uint blk, char *bitmap,
			int (*process_dquot) (struct dquot *, char *))
{
	dqbuf_t buf = getdqbuf();
	struct v2_disk_dqdbheader *dh;
	struct v2_disk_dqblk *ddata;
	int entries, i;

	set_bit(bitmap, blk);
	read_blk(dquot->dq_h, blk, buf);
	dh = (struct v2_disk_dqdbheader *)buf;
	ddata = V2_GETENTRIES(buf);
	entries = __le16_to_cpu(dh->dqdh_entries);
	for (i = 0; i < V2_DQSTRINBLK; i++)
		if (!empty_dquot(ddata + i)) {
			v2_disk2memdqblk(&dquot->dq_dqb, ddata + i);
			dquot->dq_id = __le32_to_cpu(ddata[i].dqb_id);
			if (process_dquot(dquot, NULL) < 0)
				break;
		}
	freedqbuf(buf);
	return entries;
}

static void check_reference(struct quota_handle *h, uint blk)
{
	if (blk >= h->qh_info.u.v2_mdqi.dqi_blocks)
		die(2, _("Illegal reference in %s quota file on %s. Quota file is probably corrupted.\nPlease run quotacheck(8) and try again.\n"), type2name(h->qh_type), h->qh_quotadev);
}

static int report_tree(struct dquot *dquot, uint blk, int depth, char *bitmap,
		       int (*process_dquot) (struct dquot *, char *))
{
	int entries = 0, i;
	dqbuf_t buf = getdqbuf();
	u_int32_t *ref = (u_int32_t *) buf;

	read_blk(dquot->dq_h, blk, buf);
	if (depth == V2_DQTREEDEPTH - 1) {
		for (i = 0; i < V2_DQBLKSIZE >> 2; i++) {
			blk = __le32_to_cpu(ref[i]);
			check_reference(dquot->dq_h, blk);
			if (blk && !get_bit(bitmap, blk))
				entries += report_block(dquot, blk, bitmap, process_dquot);
		}
	}
	else {
		for (i = 0; i < V2_DQBLKSIZE >> 2; i++)
			if ((blk = __le32_to_cpu(ref[i]))) {
				check_reference(dquot->dq_h, blk);
				entries +=
					report_tree(dquot, blk, depth + 1, bitmap, process_dquot);
			}
	}
	freedqbuf(buf);
	return entries;
}

static uint find_set_bits(char *bmp, int blocks)
{
	uint i, used = 0;

	for (i = 0; i < blocks; i++)
		if (get_bit(bmp, i))
			used++;
	return used;
}

static int v2_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *, char *))
{
	char *bitmap;
	struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;
	struct v2_disk_dqinfo ddqinfo;
	struct dquot *dquot = get_empty_dquot();

	if (QIO_ENABLED(h))	/* Kernel uses same file? */
		if (quotactl(QCMD((kernel_iface == IFACE_GENERIC) ? Q_SYNC : Q_6_5_SYNC, h->qh_type),
			     h->qh_quotadev, 0, NULL) < 0)
			die(4, _("Cannot sync quotas on device %s: %s\n"), h->qh_quotadev,
			    strerror(errno));
	lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
	if (read(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo)) {
		free(dquot);
		return -1;
	}
	info->dqi_blocks = __le32_to_cpu(ddqinfo.dqi_blocks);
	dquot->dq_h = h;
	bitmap = smalloc((info->dqi_blocks + 7) >> 3);
	memset(bitmap, 0, (info->dqi_blocks + 7) >> 3);
	info->dqi_used_entries = report_tree(dquot, V2_DQTREEOFF, 0, bitmap, process_dquot);
	info->dqi_data_blocks = find_set_bits(bitmap, info->dqi_blocks);
	free(bitmap);
	free(dquot);
	return 0;
}

/* Report information about quotafile */
static int v2_report(struct quota_handle *h, int verbose)
{
	if (verbose) {
		struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;

		printf(_("Statistics:\nTotal blocks: %u\nData blocks: %u\nEntries: %u\nUsed average: %f\n"),
			 info->dqi_blocks, info->dqi_data_blocks, info->dqi_used_entries,
			 ((float)info->dqi_used_entries) / info->dqi_data_blocks);
	}
	return 0;
}
