/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1999,2007 Oracle.  All rights reserved.
 *
 * $Id: os_root.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * __os_isroot --
 *	Return if user has special permissions.
 *
 * PUBLIC: int __os_isroot __P((void));
 */
int
__os_isroot()
{
#ifdef HAVE_GETUID
	return (getuid() == 0);
#else
	return (0);
#endif
}
