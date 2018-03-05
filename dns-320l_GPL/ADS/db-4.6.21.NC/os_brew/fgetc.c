/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2006,2007 Oracle.  All rights reserved.
 *
 * $Id: fgetc.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * fgetc --
 *
 * PUBLIC: #ifndef HAVE_FGETC
 * PUBLIC: int fgetc __P((FILE *));
 * PUBLIC: #endif
 */
int
fgetc(fp)
	FILE *fp;
{
	char b[1];

	if (IFILE_Read(fp, b, 1))
		return ((int)b[0]);

	__os_set_errno(EIO);
	return (EOF);
}
