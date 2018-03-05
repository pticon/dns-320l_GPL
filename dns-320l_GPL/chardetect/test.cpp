#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "chardetect.h"

static char *progname;

using namespace std;

void usage()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s detect_string\n", progname);
//	fprintf(stderr, "  %s filename\n", progname);
//	fprintf(stderr, "or\n");
//	fprintf(stderr, "  echo \"xxxxx\" | %s -\n", progname);
}

int main(int argc, char *argv[])
{
	chardet_t det = NULL;
	char encoding[CHARDET_MAX_ENCODING_NAME];
	int res = 0;
	FILE *fp;
	char buf[4096];
	int len;

	progname = argv[0];

	memset(buf, 0, sizeof(buf));

	if (argc < 2) {
		cerr << "ERROR: need one argument" << endl;
		usage();
		exit(1);
	}

//	if (!strcmp(argv[1], "-")) {
//		printf("Input from STDIN\n");
//		fp = stdin;
//	} else {
//		fp = fopen(argv[1], "rb");
//		if (!fp) {
//			fprintf(stderr, "ERROR: can't open %s\n", argv[1]);
//			usage();
//			exit(1);
//		}
//	}

	chardet_create(&det);

	res = chardet_handle_data(det, argv[1], strlen(argv[1]));

//	do {
//		len = fread(buf, 1, sizeof(buf), fp);
//		res = chardet_handle_data(det, buf, len);
//	} while ((res == CHARDET_RESULT_OK) && !feof(fp));
	
	chardet_data_end(det);

	chardet_get_charset(det, encoding, CHARDET_MAX_ENCODING_NAME);
	if (encoding[0] == '\0')
		printf("ASCII");
	else
		printf("%s", encoding);

	chardet_destroy(det);

	return 0;
}
