/*
 *
 *	Generic IO operations on quotafiles
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <asm/byteorder.h>

#include "pot.h"
#include "bylabel.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio.h"

#include "dqblk_v1.h"
#include "dqblk_v2.h"
#include "dqblk_rpc.h"
#include "dqblk_xfs.h"

/* Header in all newer quotafiles */
struct disk_dqheader {
	u_int32_t dqh_magic;
	u_int32_t dqh_version;
} __attribute__ ((packed));

/*
 *	Detect quota format and initialize quota IO
 */
struct quota_handle *init_io(struct mntent *mnt, int type, int fmt, int flags)
{
	char *qfname = NULL;
	int fd = -1, kernfmt;
	struct quota_handle *h = smalloc(sizeof(struct quota_handle));
	const char *mnt_fsname = NULL;

	if (!hasquota(mnt, type, 0))
		goto out_handle;
	if (!(mnt_fsname = get_device_name(mnt->mnt_fsname)))
		goto out_handle;
	if (stat(mnt_fsname, &h->qh_stat) < 0)
		memset(&h->qh_stat, 0, sizeof(struct stat));
	h->qh_io_flags = 0;
	if (flags & IOI_READONLY)
		h->qh_io_flags |= IOFL_RO;
	if (flags & IOI_NFS_MIXED_PATHS)
		h->qh_io_flags |= IOFL_NFS_MIXED_PATHS;
	h->qh_type = type;
	sstrncpy(h->qh_quotadev, mnt_fsname, sizeof(h->qh_quotadev));
	free((char *)mnt_fsname);
	sstrncpy(h->qh_fstype, mnt->mnt_type, MAX_FSTYPE_LEN);
	if (nfs_fstype(mnt->mnt_type)) {	/* NFS filesystem? */
		if (fmt != -1 && fmt != QF_RPC) {	/* User wanted some other format? */
			errstr(_("Only RPC quota format is allowed on NFS filesystem.\n"));
			goto out_handle;
		}
#ifdef RPC
		h->qh_fd = -1;
		h->qh_fmt = QF_RPC;
		h->qh_ops = &quotafile_ops_rpc;
		return h;
#else
		errstr(_("RPC quota format not compiled.\n"));
		goto out_handle;
#endif
	}

	if (!strcmp(mnt->mnt_type, MNTTYPE_XFS)) {	/* XFS filesystem? */
		if (fmt != -1 && fmt != QF_XFS) {	/* User wanted some other format? */
			errstr(_("Only XFS quota format is allowed on XFS filesystem.\n"));
			goto out_handle;
		}
		h->qh_fd = -1;
		h->qh_fmt = QF_XFS;
		h->qh_ops = &quotafile_ops_xfs;
		memset(&h->qh_info, 0, sizeof(h->qh_info));
		h->qh_ops->init_io(h);
		return h;
	}
	else if (fmt == QF_XFS) {
		errstr(_("XFS quota allowed only on XFS filesystem.\n"));
		goto out_handle;
	}
	if (kernel_formats > 0 && (fmt == -1 || (1 << fmt) & kernel_formats)) {	/* Quota compiled and desired format available? */
		/* Quota turned on? */
		kernfmt = kern_quota_on(h->qh_quotadev, type, fmt == -1 ? kernel_formats : (1 << fmt));
		if (kernfmt >= 0) {
			h->qh_io_flags |= IOFL_QUOTAON;
			fmt = kernfmt;	/* Default is kernel used format */
		}
	}

	if (meta_qf_fstype(mnt->mnt_type)) {
		if (!QIO_ENABLED(h)) {
			errstr(_("Quota not supported by the filesystem.\n"));
			goto out_handle;
		}
		if (flags & IOI_OPENFILE) {
			errstr(_("Operation not supported for filesystems with hidden quota files!\n"));
			goto out_handle;
		}
		h->qh_fd = -1;
		h->qh_fmt = fmt;
		goto set_ops;
	}

	fmt = get_qf_name(mnt, type,
			  (fmt == -1) ? ((1 << QF_VFSOLD) | (1 << QF_VFSV0)) : (1 << fmt),
			  (!QIO_ENABLED(h) || flags & IOI_OPENFILE) ? NF_FORMAT : 0,
			  &qfname);
	if (fmt < 0) {
		errstr(_("Quota file not found or has wrong format.\n"));
		goto out_handle;
	}
	if (!QIO_ENABLED(h) || flags & IOI_OPENFILE) {	/* Need to open file? */
		/* We still need to open file for operations like 'repquota' */
		if ((fd = open(qfname, QIO_RO(h) ? O_RDONLY : O_RDWR)) < 0) {
			errstr(_("Cannot open quotafile %s: %s\n"),
				qfname, strerror(errno));
			goto out_handle;
		}
		flock(fd, QIO_RO(h) ? LOCK_SH : LOCK_EX);
		/* Init handle */
		h->qh_fd = fd;
		h->qh_fmt = fmt;
	}
	else {
		h->qh_fd = -1;
		h->qh_fmt = fmt;
	}
	free(qfname);	/* We don't need it anymore */
	qfname = NULL;

set_ops:
	if (h->qh_fmt == QF_VFSOLD)
		h->qh_ops = &quotafile_ops_1;
	else if (h->qh_fmt == QF_VFSV0)
		h->qh_ops = &quotafile_ops_2;
	else if (h->qh_fmt == QF_META)
		h->qh_ops = &quotafile_ops_meta;
	memset(&h->qh_info, 0, sizeof(h->qh_info));

	if (h->qh_ops->init_io && h->qh_ops->init_io(h) < 0) {
		errstr(_("Cannot initialize quota on %s: %s\n"), h->qh_quotadev, strerror(errno));
		goto out_lock;
	}
	return h;
out_lock:
	if (fd != -1)
		flock(fd, LOCK_UN);
out_handle:
	if (qfname)
		free(qfname);
	free(h);
	return NULL;
}

/*
 *	Create new quotafile of specified format on given filesystem
 */
struct quota_handle *new_io(struct mntent *mnt, int type, int fmt)
{
	char *qfname;
	int fd;
	struct quota_handle *h;
	const char *mnt_fsname;
	char namebuf[PATH_MAX];

	if (fmt == -1)
		fmt = QF_VFSV0;	/* Use the newest format */
	else if (fmt == QF_RPC || fmt == QF_XFS || meta_qf_fstype(mnt->mnt_type)) {
		errstr(_("Creation of %s quota format is not supported.\n"),
			fmt == QF_RPC ? "RPC" : "XFS");
		return NULL;
	}
	if (get_qf_name(mnt, type, (1 << fmt), 0, &qfname) < 0)
		return NULL;
	sstrncpy(namebuf, qfname, PATH_MAX);
	sstrncat(namebuf, ".new", PATH_MAX);
	free(qfname);
	if ((fd = open(namebuf, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) < 0) {
		errstr(_("Cannot create new quotafile %s: %s\n"),
			namebuf, strerror(errno));
		return NULL;
	}
	if (!(mnt_fsname = get_device_name(mnt->mnt_fsname)))
		goto out_fd;
	h = smalloc(sizeof(struct quota_handle));

	h->qh_fd = fd;
	h->qh_io_flags = 0;
	sstrncpy(h->qh_quotadev, mnt_fsname, sizeof(h->qh_quotadev));
	free((char *)mnt_fsname);
	h->qh_type = type;
	memset(&h->qh_info, 0, sizeof(h->qh_info));
	if (fmt == QF_VFSOLD)
		h->qh_ops = &quotafile_ops_1;
	else
		h->qh_ops = &quotafile_ops_2;

	flock(fd, LOCK_EX);
	if (h->qh_ops->new_io && h->qh_ops->new_io(h) < 0) {
		flock(fd, LOCK_UN);
		free(h);
		goto out_fd;
	}
	return h;
      out_fd:
	close(fd);
	return NULL;
}

/*
 *	Close quotafile and release handle
 */
int end_io(struct quota_handle *h)
{
	if (h->qh_io_flags & IOFL_INFODIRTY) {
		if (h->qh_ops->write_info && h->qh_ops->write_info(h) < 0)
			return -1;
		h->qh_io_flags &= ~IOFL_INFODIRTY;
	}
	if (h->qh_ops->end_io && h->qh_ops->end_io(h) < 0)
		return -1;
	if (h->qh_fd != -1) {
		flock(h->qh_fd, LOCK_UN);
		close(h->qh_fd);
	}
	free(h);
	return 0;
}

/*
 *	Create empty quota structure
 */
struct dquot *get_empty_dquot(void)
{
	struct dquot *dquot = smalloc(sizeof(struct dquot));

	memset(dquot, 0, sizeof(*dquot));
	dquot->dq_id = -1;
	return dquot;
}
