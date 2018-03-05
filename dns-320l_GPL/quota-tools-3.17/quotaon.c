/*
 * Copyright (c) 1980, 1990 Regents of the University of California. All
 * rights reserved.
 * 
 * This code is derived from software contributed to Berkeley by Robert Elz at
 * The University of Melbourne.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 3. All advertising
 * materials mentioning features or use of this software must display the
 * following acknowledgement: This product includes software developed by the
 * University of California, Berkeley and its contributors. 4. Neither the
 * name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ident "$Copyright: (c) 1980, 1990 Regents of the University of California $"
#ident "$Copyright: All rights reserved. $"
#ident "$Id: quotaon.c,v 1.26 2008/12/17 12:40:07 jkar8572 Exp $"

/*
 * Turn quota on/off for a filesystem.
 */
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "quotaon.h"
#include "quota.h"
#include "quotasys.h"

#define FL_USER 1
#define FL_GROUP 2
#define FL_VERBOSE 4
#define FL_ALL 8
#define FL_STAT 16
#define FL_OFF 32

int flags, fmt = -1;
char *progname;
char **mntpoints;
int mntcnt;
char *xarg = NULL;

static void usage(void)
{
	errstr(_("Usage:\n\t%s [-guvp] [-F quotaformat] [-x state] -a\n\
\t%s [-guvp] [-F quotaformat] [-x state] filesys ...\n\n\
-a, --all                turn quotas on for all filesystems\n\
-f, --off                turn quotas off\n\
-u, --user               operate on user quotas\n\
-g, --group              operate on group quotas\n\
-p, --print-state        print whether quotas are on or off\n\
-x, --xfs-command=cmd    perform XFS quota command\n\
-F, --format=formatname  operate on specific quota format\n\
-v, --verbose            print more messages\n\
-h, --help               display this help text and exit\n\
-V, --version            display version information and exit\n"), progname, progname);
	exit(1);
}

static void parse_options(int argcnt, char **argstr)
{
	int c;
	struct option long_opts[] = {
		{ "all", 0, NULL, 'a' },
		{ "off", 0, NULL, 'f' },
		{ "verbose", 0, NULL, 'v' },
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "print-state", 0, NULL, 'p' },
		{ "xfs-command", 1, NULL, 'x' },
		{ "format", 1, NULL, 'F' },
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	while ((c = getopt_long(argcnt, argstr, "afvugpx:VF:h", long_opts, NULL)) != -1) {
		switch (c) {
		  case 'a':
			  flags |= FL_ALL;
			  break;
		  case 'f':
			  flags |= FL_OFF;
			  break;
		  case 'g':
			  flags |= FL_GROUP;
			  break;
		  case 'u':
			  flags |= FL_USER;
			  break;
		  case 'v':
			  flags |= FL_VERBOSE;
			  break;
		  case 'x':
			  xarg = optarg;
			  break;
		  case 'p':
			  flags |= FL_STAT;
			  break;
		  case 'F':
			  if ((fmt = name2fmt(optarg)) == QF_ERROR)
				exit(1);
			  break;
		  case 'V':
			  version();
			  exit(0);
		  case 'h':
		  default:
			  usage();
		}
	}
	if ((flags & FL_ALL && optind != argcnt) || (!(flags & FL_ALL) && optind == argcnt)) {
		fputs(_("Bad number of arguments.\n"), stderr);
		usage();
	}
	if (fmt == QF_RPC) {
		fputs(_("Cannot turn on/off quotas via RPC.\n"), stderr);
		exit(1);
	}
	if (!(flags & (FL_USER | FL_GROUP)))
		flags |= FL_USER | FL_GROUP;
	if (!(flags & FL_ALL)) {
		mntpoints = argstr + optind;
		mntcnt = argcnt - optind;
	}
}

/*
 *	For both VFS quota formats, need to pass in the quota file;
 *	for XFS quota manager, pass on the -x command line option.
 */
static int newstate(struct mntent *mnt, int type, char *extra)
{
	int sflags, ret = 0, usefmt;
	newstate_t *statefunc;

	sflags = flags & FL_OFF ? STATEFLAG_OFF : STATEFLAG_ON;
	if (flags & FL_VERBOSE)
		sflags |= STATEFLAG_VERBOSE;
	if (flags & FL_ALL)
		sflags |= STATEFLAG_ALL;

	if (!strcmp(mnt->mnt_type, MNTTYPE_XFS)) {	/* XFS filesystem has special handling... */
		if (!(kernel_formats & (1 << QF_XFS))) {
			errstr(_("Cannot change state of XFS quota. It's not compiled in kernel.\n"));
			return 1;
		}
		if (kernel_formats & (1 << QF_XFS) &&
		    ((flags & FL_OFF && (kern_quota_on(mnt->mnt_fsname, USRQUOTA, 1 << QF_XFS)
		    || kern_quota_on(mnt->mnt_fsname, GRPQUOTA, 1 << QF_XFS)))
		    || (!(flags & FL_OFF) && kern_quota_on(mnt->mnt_fsname, type, 1 << QF_XFS))))
			ret = xfs_newstate(mnt, type, extra, sflags);
	}
	else if (meta_qf_fstype(mnt->mnt_type)) {
		if (!hasquota(mnt, type, 0))
			return 0;
		/* Must be non-empty because empty path is always invalid. */
		ret = v2_newstate(mnt, type, ".", sflags);
	}
	else {
		if (!hasquota(mnt, type, 0))
			return 0;
		usefmt = get_qf_name(mnt, type, fmt == -1 ? kernel_formats : (1 << fmt), NF_FORMAT, &extra);
		if (usefmt < 0) {
			errstr(_("Cannot find quota file on %s [%s] to turn quotas on/off.\n"), mnt->mnt_dir, mnt->mnt_fsname);
			return 1;
		}
		statefunc = (usefmt == QF_VFSV0) ? v2_newstate : v1_newstate;
		ret = statefunc(mnt, type, extra, sflags);
		free(extra);
	}
	return ret;
}

/* Print state of quota (on/off) */
static int print_state(struct mntent *mnt, int type)
{
	int on = 0;

	if (!strcmp(mnt->mnt_type, MNTTYPE_XFS)) {
		if (kernel_formats & (1 << QF_XFS))
			on = kern_quota_on(mnt->mnt_fsname, type, 1 << QF_XFS) != -1;
	}
	else if (kernel_iface == IFACE_GENERIC)
		/* PSz 28 Apr 04  Have V0 and OLD set, try both */
		on = kern_quota_on(mnt->mnt_fsname, type, kernel_formats) != -1;
	else if (kernel_formats & (1 << QF_VFSV0))
		on = kern_quota_on(mnt->mnt_fsname, type, 1 << QF_VFSV0) != -1;
	else if (kernel_formats & (1 << QF_VFSOLD))
		on = kern_quota_on(mnt->mnt_fsname, type, 1 << QF_VFSOLD) != -1;

	printf(_("%s quota on %s (%s) is %s\n"), type2name(type), mnt->mnt_dir, mnt->mnt_fsname,
	  on ? _("on") : _("off"));
	
	return on;
}

/*
 *	Enable/disable VFS quota on given filesystem
 */
static int quotaonoff(char *quotadev, char *quotadir, char *quotafile, int type, int fmt, int flags)
{
	int qcmd, kqf;

	if (flags & STATEFLAG_OFF) {
		if (kernel_iface == IFACE_GENERIC)
			qcmd = QCMD(Q_QUOTAOFF, type);
		else
			qcmd = QCMD(Q_6_5_QUOTAOFF, type);
		if (quotactl(qcmd, quotadev, 0, NULL) < 0) {
			errstr(_("quotactl on %s [%s]: %s\n"), quotadev, quotadir, strerror(errno));
			return 1;
		}
		if (flags & STATEFLAG_VERBOSE)
			printf(_("%s [%s]: %s quotas turned off\n"), quotadev, quotadir, type2name(type));
		return 0;
	}
	if (kernel_iface == IFACE_GENERIC) {
		qcmd = QCMD(Q_QUOTAON, type);
 		kqf = util2kernfmt(fmt);
	}
	else {
		qcmd = QCMD(Q_6_5_QUOTAON, type);
		kqf = 0;
	}
	if (quotactl(qcmd, quotadev, kqf, (void *)quotafile) < 0) {
		if (errno == ENOENT)
			errstr(_("cannot find %s on %s [%s]\n"), quotafile, quotadev, quotadir);
		else
			errstr(_("using %s on %s [%s]: %s\n"), quotafile, quotadev, quotadir, strerror(errno));
		if (errno == EINVAL)
			errstr(_("Maybe create new quota files with quotacheck(8)?\n"));
		else if (errno == ESRCH)
			errstr(_("Quota format not supported in kernel.\n"));
		return 1;
	}
	if (flags & STATEFLAG_VERBOSE)
		printf(_("%s [%s]: %s quotas turned on\n"), quotadev, quotadir, type2name(type));
	return 0;
}

/*
 *	Enable/disable rsquash on given filesystem
 */
static int quotarsquashonoff(const char *quotadev, int type, int flags)
{
#if defined(MNTOPT_RSQUASH)
	int ret;

	if (kernel_iface == IFACE_GENERIC) {
		int qcmd = QCMD(Q_SETINFO, type);
		struct if_dqinfo info;

		info.dqi_flags = V1_DQF_RSQUASH;
		info.dqi_valid = IIF_FLAGS;
		ret = quotactl(qcmd, quotadev, 0, (void *)&info);
	}
	else {
		int mode = (flags & STATEFLAG_OFF) ? 0 : 1;
		int qcmd = QCMD(Q_V1_RSQUASH, type);

		ret = quotactl(qcmd, quotadev, 0, (void *)&mode);
	}
	if (ret < 0) {
		errstr(_("set root_squash on %s: %s\n"), quotadev, strerror(errno));
		return 1;
	}
	if ((flags & STATEFLAG_VERBOSE) && (flags & STATEFLAG_OFF))
		printf(_("%s: %s root_squash turned off\n"), quotadev, type2name(type));
	else if ((flags & STATEFLAG_VERBOSE) && (flags & STATEFLAG_ON))
		printf(_("%s: %s root_squash turned on\n"), quotadev, type2name(type));
#endif
	return 0;
}

/*
 *	Enable/disable quota/rootsquash on given filesystem (version 1)
 */
int v1_newstate(struct mntent *mnt, int type, char *file, int flags)
{
	int errs = 0;
	const char *dev = get_device_name(mnt->mnt_fsname);

	if (!dev)
		return 1;
	if ((flags & STATEFLAG_OFF) && hasmntopt(mnt, MNTOPT_RSQUASH))
		errs += quotarsquashonoff(dev, type, flags);
	if (hasquota(mnt, type, 0))
		errs += quotaonoff((char *)dev, mnt->mnt_dir, file, type, QF_VFSOLD, flags);
	if ((flags & STATEFLAG_ON) && hasmntopt(mnt, MNTOPT_RSQUASH))
		errs += quotarsquashonoff(dev, type, flags);
	free((char *)dev);
	return errs;
}

/*
 *	Enable/disable quota on given filesystem (version 2 quota)
 */
int v2_newstate(struct mntent *mnt, int type, char *file, int flags)
{
	const char *dev = get_device_name(mnt->mnt_fsname);
	int errs = 0;

	if (!dev)
		return 1;

	if (hasquota(mnt, type, 0))
		errs = quotaonoff((char *)dev, mnt->mnt_dir, file, type, QF_VFSV0, flags);
	free((char *)dev);
	return errs;
}

int main(int argc, char **argv)
{
	struct mntent *mnt;
	int errs = 0;

	gettexton();

	progname = basename(argv[0]);
	if (strcmp(progname, "quotaoff") == 0)
		flags |= FL_OFF;
	else if (strcmp(progname, "quotaon") != 0)
		die(1, _("Name must be quotaon or quotaoff not %s\n"), progname);

	parse_options(argc, argv);

	init_kernel_interface();
	if (fmt != -1 && !(kernel_formats & (1 << fmt)))
		die(1, _("Required format %s not supported by kernel.\n"), fmt2name(fmt));
	else if (!kernel_formats)
		errstr(_("Warning: No quota format detected in the kernel.\n"));

	if (init_mounts_scan(mntcnt, mntpoints, MS_XFS_DISABLED | MS_LOCALONLY) < 0)
		return 1;
	while ((mnt = get_next_mount())) {
		if (nfs_fstype(mnt->mnt_type)) {
			if (!(flags & FL_ALL))
				fprintf(stderr, "%s: Quota cannot be turned on on NFS filesystem\n", mnt->mnt_fsname);
			continue;
		}

		if (!(flags & FL_STAT)) {
			if (flags & FL_GROUP)
				errs += newstate(mnt, GRPQUOTA, xarg);
			if (flags & FL_USER)
				errs += newstate(mnt, USRQUOTA, xarg);
		}
		else {
			if (flags & FL_GROUP)
				errs += print_state(mnt, GRPQUOTA);
			if (flags & FL_USER)
				errs += print_state(mnt, USRQUOTA);
		}
	}
	end_mounts_scan();

	return errs;
}

