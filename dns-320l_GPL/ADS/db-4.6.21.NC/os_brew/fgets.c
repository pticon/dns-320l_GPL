/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2006,2007 Oracle.  All rights reserved.
 *
 * $Id: fgets.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * fgets --
 *
 * PUBLIC: #ifndef HAVE_FGETS
 * PUBLIC: char *fgets __P((char *, int, FILE *));
 * PUBLIC: #endif
 */
char *
fgets(s, n, fp)
	char *s;
	int n;
	FILE *fp;
{
	int c;
	char *cs;

	c = 0;
	cs = s;

	while (--n > 0 && (c = fgetc(fp)) != EOF) {
		*cs++ = c;
		if (c == '\n')
			break;
	}
	if (c == EOF && cs == s)
		return (NULL);

	*cs++ = '\0';
	return (s);
}
