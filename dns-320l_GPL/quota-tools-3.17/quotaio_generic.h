/*
 *
 *	Header file for communication with kernel generic interface
 *
 */

#ifndef _QUOTAIO_GENERIC_H
#define _QUOTAIO_GENERIC_H

#include "quotaio.h"

/* Get info from kernel to handle */
int vfs_get_info(struct quota_handle *h);

/* Set info in kernel from handle */
int vfs_set_info(struct quota_handle *h, int flags);

/* Get dquot from kernel */
int vfs_get_dquot(struct dquot *dquot);

/* Set dquot in kernel */
int vfs_set_dquot(struct dquot *dquot, int flags);

#endif
