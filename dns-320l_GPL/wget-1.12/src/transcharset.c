#include <stdio.h>
#include <iconv.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "transcharset.h"

char *transcharset(char *instr, char *fromcode, char *tocode)
{
	size_t insize, outsize, conv_res;
	char *inptr, *outbuf, *outptr;
	iconv_t conv_d;

	printf("trans codeset from %s to %s\n", fromcode, tocode);

	conv_d = iconv_open(tocode, fromcode); // convert from fromcode to UTF-8
	if(conv_d == (iconv_t)-1)
	{
		printf("can't  allocate a conversion descriptor\n");
		printf("errno = %d, %s\n", errno , strerror(errno));
		return NULL;
	}

	insize = strlen(instr);
	inptr = instr;
	outsize = insize*4; // 1 byte can be extended at most 4 bytes.
	outbuf = (char*)calloc(outsize, 1);
	outptr = outbuf;

	conv_res = iconv(conv_d, (const char**)&inptr, &insize, &outptr, &outsize);
	if(conv_res == -1)
	{
		iconv_close(conv_d);
		return NULL;
	}

	iconv_close(conv_d);
	return outbuf;
}
