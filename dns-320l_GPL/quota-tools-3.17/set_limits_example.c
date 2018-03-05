#include <sys/types.h>
#include <sys/quota.h>
#include <errno.h>
#include <stdio.h>

#include "pot.h"

int copy_user_quota_limits(const char *block_device, uid_t from, uid_t to)
{
	struct dqblk dq;

	if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), block_device, from, (caddr_t) & dq) == 0) {
		if (quotactl(QCMD(Q_SETQLIM, USRQUOTA), block_device, to, (caddr_t) & dq) == 0) {
			return (0);
		}
		else {
			errstr(
				_("copy_user_quota_limits: Failed to set userquota for uid %ld : %s\n"),
				to, strerror(errno));
			return (1);
		}
	}
	else {
		errstr(
			_("copy_user_quota_limits: Failed to get userquota for uid %ld : %s\n"),
			from, strerror(errno));
		return (1);
	}
}

int copy_group_quota_limits(const char *block_device, gid_t from, gid_t to)
{
	struct dqblk dq;

	if (quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), block_device, from, (caddr_t) & dq) == 0) {
		if (quotactl(QCMD(Q_SETQLIM, GRPQUOTA), block_device, to, (caddr_t) & dq) == 0) {
			return (0);
		}
		else {
			errstr(
				_("copy_group_quota_limits: Failed to set groupquota for uid %ld : %s\n"),
				to, strerror(errno));
			return (1);
		}
	}
	else {
		errstr(
			_("copy_group_quota_limits: Failed to get groupquota for uid %ld : %s\n"),
			from, strerror(errno));
		return (1);
	}
}

main(int argc, char **argv)
{
	gettexton();
	copy_user_quota_limits("/dev/hda8", 152, 151);
}
