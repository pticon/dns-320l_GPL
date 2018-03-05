/*
 *	Implementation of communication with kernel generic interface
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quota.h"
#include "quotasys.h"

/* Convert kernel quotablock format to utility one */
static inline void generic_kern2utildqblk(struct util_dqblk *u, struct if_dqblk *k)
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
static inline void generic_util2kerndqblk(struct if_dqblk *k, struct util_dqblk *u)
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

/* Get info from kernel to handle */
int vfs_get_info(struct quota_handle *h)
{
	struct if_dqinfo kinfo;

	if (quotactl(QCMD(Q_GETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kinfo) < 0) {
		errstr(_("Cannot get info for %s quota file from kernel on %s: %s\n"), type2name(h->qh_type), h->qh_quotadev, strerror(errno));
		return -1;
	}
	h->qh_info.dqi_bgrace = kinfo.dqi_bgrace;
	h->qh_info.dqi_igrace = kinfo.dqi_igrace;
	return 0;
}

/* Set info in kernel from handle */
int vfs_set_info(struct quota_handle *h, int flags)
{
	struct if_dqinfo kinfo;

	kinfo.dqi_bgrace = h->qh_info.dqi_bgrace;
	kinfo.dqi_igrace = h->qh_info.dqi_igrace;
	kinfo.dqi_valid = flags;

	if (quotactl(QCMD(Q_SETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kinfo) < 0) {
		errstr(_("Cannot set info for %s quota file from kernel on %s: %s\n"), type2name(h->qh_type), h->qh_quotadev, strerror(errno));
		return -1;
	}
	return 0;
}

/* Get dquot from kernel */
int vfs_get_dquot(struct dquot *dquot)
{
	struct if_dqblk kdqblk;

	if (quotactl(QCMD(Q_GETQUOTA, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev, dquot->dq_id, (void *)&kdqblk) < 0) {
		errstr(_("Cannot get quota for %s %d from kernel on %s: %s\n"), type2name(dquot->dq_h->qh_type), dquot->dq_id, dquot->dq_h->qh_quotadev, strerror(errno));
		return -1;
	}
	generic_kern2utildqblk(&dquot->dq_dqb, &kdqblk);
	return 0;
}

/* Set dquot in kernel */
int vfs_set_dquot(struct dquot *dquot, int flags)
{
	struct if_dqblk kdqblk;

	generic_util2kerndqblk(&kdqblk, &dquot->dq_dqb);
	kdqblk.dqb_valid = flags;
	if (quotactl(QCMD(Q_SETQUOTA, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev, dquot->dq_id, (void *)&kdqblk) < 0) {
		errstr(_("Cannot set quota for %s %d from kernel on %s: %s\n"), type2name(dquot->dq_h->qh_type), dquot->dq_id, dquot->dq_h->qh_quotadev, strerror(errno));
		return -1;
	}
	return 0;
}
