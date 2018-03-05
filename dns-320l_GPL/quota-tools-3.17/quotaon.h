/*
 *	Common types, macros, and routines for enabling/disabling
 *	quota for each of the various Linux quota formats.
 */

#include "pot.h"
#include "quota.h"
#include "quotasys.h"
#include "bylabel.h"
#include "common.h"
#include "quotaio.h"

#define STATEFLAG_ON		0x01
#define STATEFLAG_OFF		0x02
#define STATEFLAG_ALL		0x04
#define STATEFLAG_VERBOSE	0x08

typedef int (newstate_t) (struct mntent * mnt, int type, char *file, int flags);
extern int v1_newstate(struct mntent *mnt, int type, char *file, int flags);
extern int v2_newstate(struct mntent *mnt, int type, char *file, int flags);
extern int xfs_newstate(struct mntent *mnt, int type, char *file, int flags);
