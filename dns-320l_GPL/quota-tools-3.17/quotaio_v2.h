/*
 *
 *	Header file for disk format of new quotafile format
 *
 */

#ifndef _QUOTAIO_V2_H
#define _QUOTAIO_V2_H

#include <sys/types.h>
#include "quota.h"

#define V2_DQINFOOFF	sizeof(struct v2_disk_dqheader)	/* Offset of info header in file */
#define V2_DQBLKSIZE_BITS	10
#define V2_DQBLKSIZE	(1 << V2_DQBLKSIZE_BITS)	/* Size of block with quota structures */
#define V2_DQTREEOFF	1	/* Offset of tree in file in blocks */
#define V2_DQTREEDEPTH	4	/* Depth of quota tree */
#define V2_DQSTRINBLK	((V2_DQBLKSIZE - sizeof(struct v2_disk_dqdbheader)) / sizeof(struct v2_disk_dqblk))	/* Number of entries in one blocks */
#define V2_GETIDINDEX(id, depth) (((id) >> ((V2_DQTREEDEPTH-(depth)-1)*8)) & 0xff)
#define V2_GETENTRIES(buf) ((struct v2_disk_dqblk *)(((char *)(buf)) + sizeof(struct v2_disk_dqdbheader)))
#define INIT_V2_VERSIONS { 0, 0}

struct v2_disk_dqheader {
	u_int32_t dqh_magic;	/* Magic number identifying file */
	u_int32_t dqh_version;	/* File version */
} __attribute__ ((packed));

/* Flags for version specific files */
#define V2_DQF_MASK  0x0000	/* Mask for all valid ondisk flags */

/* Header with type and version specific information */
struct v2_disk_dqinfo {
	u_int32_t dqi_bgrace;	/* Time before block soft limit becomes hard limit */
	u_int32_t dqi_igrace;	/* Time before inode soft limit becomes hard limit */
	u_int32_t dqi_flags;	/* Flags for quotafile (DQF_*) */
	u_int32_t dqi_blocks;	/* Number of blocks in file */
	u_int32_t dqi_free_blk;	/* Number of first free block in the list */
	u_int32_t dqi_free_entry;	/* Number of block with at least one free entry */
} __attribute__ ((packed));

/*
 *  Structure of header of block with quota structures. It is padded to 16 bytes so
 *  there will be space for exactly 18 quota-entries in a block
 */
struct v2_disk_dqdbheader {
	u_int32_t dqdh_next_free;	/* Number of next block with free entry */
	u_int32_t dqdh_prev_free;	/* Number of previous block with free entry */
	u_int16_t dqdh_entries;	/* Number of valid entries in block */
	u_int16_t dqdh_pad1;
	u_int32_t dqdh_pad2;
} __attribute__ ((packed));

/* Structure of quota for one user on disk */
struct v2_disk_dqblk {
	u_int32_t dqb_id;	/* id this quota applies to */
	u_int32_t dqb_ihardlimit;	/* absolute limit on allocated inodes */
	u_int32_t dqb_isoftlimit;	/* preferred inode limit */
	u_int32_t dqb_curinodes;	/* current # allocated inodes */
	u_int32_t dqb_bhardlimit;	/* absolute limit on disk space (in QUOTABLOCK_SIZE) */
	u_int32_t dqb_bsoftlimit;	/* preferred limit on disk space (in QUOTABLOCK_SIZE) */
	u_int64_t dqb_curspace;	/* current space occupied (in bytes) */
	u_int64_t dqb_btime;	/* time limit for excessive disk use */
	u_int64_t dqb_itime;	/* time limit for excessive inode use */
} __attribute__ ((packed));

/* Structure of quota for communication with kernel */
struct v2_kern_dqblk {
	unsigned int dqb_ihardlimit;
	unsigned int dqb_isoftlimit;
	unsigned int dqb_curinodes;
	unsigned int dqb_bhardlimit;
	unsigned int dqb_bsoftlimit;
	qsize_t dqb_curspace;
	time_t dqb_btime;
	time_t dqb_itime;
};

/* Structure of quotafile info for communication with kernel */
struct v2_kern_dqinfo {
	unsigned int dqi_bgrace;
	unsigned int dqi_igrace;
	unsigned int dqi_flags;
	unsigned int dqi_blocks;
	unsigned int dqi_free_blk;
	unsigned int dqi_free_entry;
};

/* Structure with gathered statistics from kernel */
struct v2_dqstats {
	u_int32_t lookups;
	u_int32_t drops;
	u_int32_t reads;
	u_int32_t writes;
	u_int32_t cache_hits;
	u_int32_t allocated_dquots;
	u_int32_t free_dquots;
	u_int32_t syncs;
	u_int32_t version;
};

#endif
