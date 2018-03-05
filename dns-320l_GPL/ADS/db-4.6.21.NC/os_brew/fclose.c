/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2006,2007 Oracle.  All rights reserved.
 *
 * $Id: fclose.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * fclose --
 *
 * PUBLIC: #ifndef HAVE_FCLOSE
 * PUBLIC: int fclose __P((FILE *));
 * PUBLIC: #endif
 */
int
fclose(fp)
	FILE *fp;
{
	/*
	 * Release (close) the file.
	 *
	 * Returns the updated reference count, which is of no use to us.
	 */
	(void)IFILE_Release(fp);

	return (0);
}
