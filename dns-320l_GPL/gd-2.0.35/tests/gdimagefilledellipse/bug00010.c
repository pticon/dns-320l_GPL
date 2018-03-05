/* $Id: bug00010.c,v 1.1.1.1 2009/10/09 02:34:28 jack Exp $ */
#include "gd.h"
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
 	int error = 0;
	char path[1024];

	im = gdImageCreateTrueColor(100,100);
	gdImageFilledEllipse(im, 50,50, 70, 90, 0x50FFFFFF);

	sprintf(path, "%s/gdimagefilledellipse/bug00010_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}

 	gdImageDestroy(im);
 	return error;
}
