/* $Id: bug00033.c,v 1.1.1.1 2009/10/09 02:34:29 jack Exp $ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;
	char path[1024];

	sprintf(path, "%s/png/bug00033.png", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		printf("failed, cannot open file <%s>\n", path);
		return 1;
	}

	im = gdImageCreateFromPng(fp);
	fclose(fp);

	if (im) {
		gdImageDestroy(im);
		return 1;
	} else {
		return 0;
	}

}
