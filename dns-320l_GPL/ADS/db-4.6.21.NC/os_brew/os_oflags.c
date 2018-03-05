/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997,2007 Oracle.  All rights reserved.
 *
 * $Id: os_oflags.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * __db_omode --
 *	Convert a permission string to the correct open(2) flags.
 */
int
__db_omode(perm)
	const char *perm;
{
	COMPQUIET(perm, NULL);

	/*
	 * BREW doesn't have permission modes, we're kind of done.
	 */
	return (0);
}
