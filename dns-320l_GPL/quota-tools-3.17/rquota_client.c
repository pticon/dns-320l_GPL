/*
 * QUOTA    An implementation of the diskquota system for the LINUX
 *          operating system. QUOTA is implemented using the BSD systemcall
 *          interface as the means of communication with the user level.
 *          Should work for all filesystems because of integration into the
 *          VFS layer of the operating system.
 *          This is based on the Melbourne quota system wich uses both user and
 *          group quota files.
 *
 *          This part does the rpc-communication with the rquotad.
 *
 * Version: $Id: rquota_client.c,v 1.11 2007/08/23 19:58:14 jkar8572 Exp $
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License
 *          as published by the Free Software Foundation; either version
 *          2 of the License, or (at your option) any later version.
 */
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "mntopt.h"
#include "rquota.h"
#include "common.h"
#include "quotaio.h"

#if defined(RPC)

/* Convert network format of quotas to utils one */
static inline void clinet2utildqblk(struct util_dqblk *u, struct rquota *n)
{
	time_t now;
	
	/* Copy the quota */
	u->dqb_bhardlimit = n->rq_bhardlimit;
	u->dqb_bsoftlimit = n->rq_bsoftlimit;
	u->dqb_ihardlimit = n->rq_fhardlimit;
	u->dqb_isoftlimit = n->rq_fsoftlimit;
	u->dqb_curinodes = n->rq_curfiles;
	u->dqb_curspace = ((qsize_t)n->rq_curblocks) * n->rq_bsize;
	time(&now);
	if (n->rq_btimeleft)
		u->dqb_btime = n->rq_btimeleft + now;
	else
		u->dqb_btime = 0;
	if (n->rq_ftimeleft)
		u->dqb_itime = n->rq_ftimeleft + now;
	else
		u->dqb_itime = 0;
	/* Convert from remote block size */
	if (n->rq_bsize != RPC_DQBLK_SIZE) {
		int conversion_unit;

		conversion_unit = n->rq_bsize >> RPC_DQBLK_SIZE_BITS;
		if (conversion_unit == 0) {
			conversion_unit = RPC_DQBLK_SIZE / n->rq_bsize;

			u->dqb_bhardlimit /= conversion_unit;
			u->dqb_bsoftlimit /= conversion_unit;
		}
		else {
			u->dqb_bhardlimit *= conversion_unit;
			u->dqb_bsoftlimit *= conversion_unit;
		}
	}
}

/* Convert utils format of quotas to network one */
static inline void cliutil2netdqblk(struct sq_dqblk *n, struct util_dqblk *u)
{
	time_t now;

	time(&now);
	n->rq_bhardlimit = u->dqb_bhardlimit;
	n->rq_bsoftlimit = u->dqb_bsoftlimit;
	n->rq_fhardlimit = u->dqb_ihardlimit;
	n->rq_fsoftlimit = u->dqb_isoftlimit;
	n->rq_curblocks = toqb(u->dqb_curspace);
	n->rq_curfiles = u->dqb_curinodes;
	if (u->dqb_btime)
		n->rq_btimeleft = u->dqb_btime - now;
	else
		n->rq_btimeleft = 0;
	if (u->dqb_itime)
		n->rq_ftimeleft = u->dqb_itime - now;
	else
		n->rq_ftimeleft = 0;
}

/* Write appropriate error message */
int rquota_err(int stat)
{
	switch (stat) {
		case -1:
			return -ECONNREFUSED;
		case 0:
			return -ENOSYS;
		case Q_NOQUOTA:
			return -ENOENT;
		case Q_OK:
			return 0;
		case Q_EPERM:
			return -EPERM;
		default:
			return -EINVAL;
	}
}

/*
 * Collect the requested quota information from a remote host.
 */
int rpc_rquota_get(struct dquot *dquot)
{
	CLIENT *clnt;
	getquota_rslt *result;
	union {
		getquota_args arg;
		ext_getquota_args ext_arg;
	} args;
	char *fsname_tmp, *host, *pathname;
	struct timeval timeout = { 2, 0 };

	/*
	 * Initialize with NULL.
	 */
	memset(&dquot->dq_dqb, 0, sizeof(dquot->dq_dqb));

	/*
	 * Convert host:pathname to seperate host and pathname.
	 */
	fsname_tmp = (char *)smalloc(strlen(dquot->dq_h->qh_quotadev) + 1);
	strcpy(fsname_tmp, dquot->dq_h->qh_quotadev);
	host = fsname_tmp;

	/*
	 * Strip off pathname on nfs mounted dir. Ignore entries of any
	 * automounter.
	 */
	if ((pathname = strchr(fsname_tmp, ':')) == (char *)0 || *(pathname + 1) == '(') {
		free(fsname_tmp);
		return -ENOENT;
	}

	*pathname++ = '\0';
	/* For NFSv4, we send the filesystem path without initial /. Server prepends proper
	 * NFS pseudoroot automatically and uses this for detection of NFSv4 mounts. */
	if ((dquot->dq_h->qh_io_flags & IOFL_NFS_MIXED_PATHS) &&
	    !strcmp(dquot->dq_h->qh_fstype, MNTTYPE_NFS4)) {
		while (*pathname == '/')
			pathname++;
	}

	/*
	 * First try EXT_RQUOTAPROG (Extended (LINUX) RPC quota program)
	 */
	args.ext_arg.gqa_pathp = pathname;
	args.ext_arg.gqa_id = dquot->dq_id;
	args.ext_arg.gqa_type = dquot->dq_h->qh_type;

	/*
	 * Create a RPC client.
	 */
	if ((clnt = clnt_create(host, RQUOTAPROG, EXT_RQUOTAVERS, "udp")) != NULL) {
		/*
		 * Initialize unix authentication
		 */
		clnt->cl_auth = authunix_create_default();

		/*
		 * Setup protocol timeout.
		 */
		clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

		/*
		 * Do RPC call and check result.
		 */
		result = rquotaproc_getquota_2(&args.ext_arg, clnt);
		if (result != NULL && result->status == Q_OK)
			clinet2utildqblk(&dquot->dq_dqb, &result->getquota_rslt_u.gqr_rquota);

		/*
		 * Destroy unix authentication and RPC client structure.
		 */
		auth_destroy(clnt->cl_auth);
		clnt_destroy(clnt);
	}
	else
		result = NULL;

	if (result == NULL || !result->status) {
		if (dquot->dq_h->qh_type == USRQUOTA) {
			/*
			 * Try RQUOTAPROG because server doesn't seem to understand EXT_RQUOTAPROG. (NON-LINUX servers.)
			 */
			args.arg.gqa_pathp = pathname;
			args.arg.gqa_uid = dquot->dq_id;

			/*
			 * Create a RPC client.
			 */
			if ((clnt = clnt_create(host, RQUOTAPROG, RQUOTAVERS, "udp")) != NULL) {
				/*
				 * Initialize unix authentication
				 */
				clnt->cl_auth = authunix_create_default();

				/*
				 * Setup protocol timeout.
				 */
				clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

				/*
				 * Do RPC call and check result.
				 */
				result = rquotaproc_getquota_1(&args.arg, clnt);
				if (result != NULL && result->status == Q_OK)
					clinet2utildqblk(&dquot->dq_dqb,
							 &result->getquota_rslt_u.gqr_rquota);

				/*
				 * Destroy unix authentication and RPC client structure.
				 */
				auth_destroy(clnt->cl_auth);
				clnt_destroy(clnt);
			}
		}
	}
	free(fsname_tmp);
	return rquota_err(result?result->status:-1);
}

/*
 * Set the requested quota information on a remote host.
 */
int rpc_rquota_set(int qcmd, struct dquot *dquot)
{
#if defined(RPC_SETQUOTA)
	CLIENT *clnt;
	setquota_rslt *result;
	union {
		setquota_args arg;
		ext_setquota_args ext_arg;
	} args;
	char *fsname_tmp, *host, *pathname;
	struct timeval timeout = { 2, 0 };

	/*
	 * Convert host:pathname to seperate host and pathname.
	 */
	fsname_tmp = (char *)smalloc(strlen(dquot->dq_h->qh_quotadev) + 1);
	strcpy(fsname_tmp, dquot->dq_h->qh_quotadev);
	host = fsname_tmp;

	/*
	 * Strip off pathname on nfs mounted dir. Ignore entries of any
	 * automounter.
	 */
	if ((pathname = strchr(fsname_tmp, ':')) == (char *)0 || *(pathname + 1) == '(')
		return -ENOENT;

	*pathname++ = '\0';
	/* For NFSv4, we send the filesystem path without initial /. Server prepends proper
	 * NFS pseudoroot automatically and uses this for detection of NFSv4 mounts. */
	if ((dquot->dq_h->qh_io_flags & IOFL_NFS_MIXED_PATHS) &&
	    !strcmp(dquot->dq_h->qh_fstype, MNTTYPE_NFS4)) {
		while (*pathname == '/')
			pathname++;
	}

	/*
	 * First try EXT_RQUOTAPROG (Extended (LINUX) RPC quota program)
	 */
	args.ext_arg.sqa_qcmd = qcmd;
	args.ext_arg.sqa_pathp = pathname;
	args.ext_arg.sqa_id = dquot->dq_id;
	args.ext_arg.sqa_type = dquot->dq_h->qh_type;
	cliutil2netdqblk(&args.ext_arg.sqa_dqblk, &dquot->dq_dqb);

	if ((clnt = clnt_create(host, RQUOTAPROG, EXT_RQUOTAVERS, "udp")) != NULL) {
		/*
		 * Initialize unix authentication
		 */
		clnt->cl_auth = authunix_create_default();

		/*
		 * Setup protocol timeout.
		 */
		clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

		/*
		 * Do RPC call and check result.
		 */
		result = rquotaproc_setquota_2(&args.ext_arg, clnt);
		if (result != NULL && result->status == Q_OK)
			clinet2utildqblk(&dquot->dq_dqb, &result->setquota_rslt_u.sqr_rquota);

		/*
		 * Destroy unix authentication and RPC client structure.
		 */
		auth_destroy(clnt->cl_auth);
		clnt_destroy(clnt);
	}
	else
		result = NULL;

	if (result == NULL || !result->status) {
		if (dquot->dq_h->qh_type == USRQUOTA) {
			/*
			 * Try RQUOTAPROG because server doesn't seem to understand EXT_RQUOTAPROG. (NON-LINUX servers.)
			 */
			args.arg.sqa_qcmd = qcmd;
			args.arg.sqa_pathp = pathname;
			args.arg.sqa_id = dquot->dq_id;
			cliutil2netdqblk(&args.arg.sqa_dqblk, &dquot->dq_dqb);

			/*
			 * Create a RPC client.
			 */
			if ((clnt = clnt_create(host, RQUOTAPROG, RQUOTAVERS, "udp")) != NULL) {
				/*
				 * Initialize unix authentication
				 */
				clnt->cl_auth = authunix_create_default();

				/*
				 * Setup protocol timeout.
				 */
				clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

				/*
				 * Do RPC call and check result.
				 */
				result = rquotaproc_setquota_1(&args.arg, clnt);
				if (result != NULL && result->status == Q_OK)
					clinet2utildqblk(&dquot->dq_dqb,
							 &result->setquota_rslt_u.sqr_rquota);

				/*
				 * Destroy unix authentication and RPC client structure.
				 */
				auth_destroy(clnt->cl_auth);
				clnt_destroy(clnt);
			}
		}
	}
	free(fsname_tmp);
	return rquota_err(result?result->status:-1);
#endif
	return -1;
}
#endif
