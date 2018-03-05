/*
 *
 *	Header file for disk format of new quotafile format
 *
 */

#ifndef _DQBLK_V2_H
#define _DQBLK_V2_H

#include <sys/types.h>

#define Q_V2_GETQUOTA	0x0D00	/* Get limits and usage */
#define Q_V2_SETQUOTA	0x0E00	/* Set limits and usage */
#define Q_V2_SETUSE	0x0F00	/* Set only usage */
#define Q_V2_SETQLIM	0x0700	/* Set only limits */
#define Q_V2_GETINFO	0x0900	/* Get information about quota */
#define Q_V2_SETINFO	0x0A00	/* Set information about quota */
#define Q_V2_SETGRACE	0x0B00	/* Set just grace times in quotafile information */
#define Q_V2_SETFLAGS	0x0C00	/* Set just flags in quotafile information */
#define Q_V2_GETSTATS	0x1100	/* get collected stats (before proc was used) */

/* Structure for format specific information */
struct v2_mem_dqinfo {
	uint dqi_flags;		/* Flags set in quotafile */
	uint dqi_blocks;	/* Number of blocks in file */
	uint dqi_free_blk;	/* Number of first free block in the list */
	uint dqi_free_entry;	/* Number of first block with free entry in the list */
	uint dqi_used_entries;	/* Number of entries in file - updated by scan_dquots */
	uint dqi_data_blocks;	/* Number of data blocks in file - updated by scan_dquots */
};

struct v2_mem_dqblk {
	loff_t dqb_off;		/* Offset of dquot in file */
};

struct quotafile_ops;		/* Will be defined later in quotaio.h */

/* Operations above this format */
extern struct quotafile_ops quotafile_ops_2;

#endif
