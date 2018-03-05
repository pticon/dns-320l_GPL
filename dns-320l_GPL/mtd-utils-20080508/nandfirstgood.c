

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <getopt.h>

#include <asm/types.h>
#include "mtd/mtd-user.h"

//#define PROGRAM "nandfirstgood"
//#define VERSION "$Revision: 1.1.1.1 $"

#define MAX_PAGE_SIZE	2048
#define MAX_OOB_SIZE	64

/*
 * Buffer array used for writing data
 */

char	*mtd_device, *img;
int	mtdoffset = 0;
int	quiet = 1;
int	writeoob = 0;
int	markbad = 0;
int	autoplace = 0;
int	forcejffs2 = 0;
int	forceyaffs = 0;
int	forcelegacy = 0;
int	noecc = 0;
int	pad = 0;
int	blockalign = 1; /*default to using 16K block size */

/*
 * Main program
 */
int main(int argc, char **argv)
{

	int fd, baderaseblock, blockstart = -1;
	int cnt = 0;
	struct mtd_info_user meminfo;
	loff_t offs;
	int ret; //, readlen;

	mtd_device = argv[1];


	/* Open the device */
	if ((fd = open(mtd_device, O_RDWR)) == -1) {
		perror("open flash");
		exit(1);
	}

	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		perror("MEMGETINFO");
		close(fd);
		exit(1);
	}

	/* Set erasesize to specified number of blocks - to match jffs2
	 * (virtual) block size */
	meminfo.erasesize *= blockalign;

	/* Make sure device page sizes are valid */
	if (!(meminfo.oobsize == 16 && meminfo.writesize == 512) &&
			!(meminfo.oobsize == 8 && meminfo.writesize == 256) &&
			!(meminfo.oobsize == 64 && meminfo.writesize == 2048)) {
		fprintf(stderr, "Unknown flash (not normal NAND)\n");
		close(fd);
		exit(1);
	}


	
		while (blockstart != (mtdoffset & (~meminfo.erasesize + 1))) {
			blockstart = mtdoffset & (~meminfo.erasesize + 1);
			offs = blockstart;
			baderaseblock = 0;
			if (!quiet)
				fprintf (stdout, "Writing data to block %x\n", blockstart);

			/* Check all the blocks in an erase block for bad blocks */
			do {
				if ((ret = ioctl(fd, MEMGETBADBLOCK, &offs)) < 0) {
					perror("ioctl(MEMGETBADBLOCK)");
					goto closeall;
				}
				if (ret == 1) {
					baderaseblock = 1;
					cnt++;
					if (!quiet)
						fprintf (stderr, "Bad block at %x, %u block(s) "
								"from %x will be skipped\n",
								(int) offs, blockalign, blockstart);
				}

				if (baderaseblock) {
					mtdoffset = blockstart + meminfo.erasesize;
				}
				offs +=  meminfo.erasesize / blockalign ;
			} while ( offs < blockstart + meminfo.erasesize );
		}

closeall:
	close(fd);

	/* Return happy */
	printf("First Good Block : %d\n", cnt);
	return 0;
}
