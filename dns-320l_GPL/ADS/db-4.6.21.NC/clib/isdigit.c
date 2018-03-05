/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2005,2007 Oracle.  All rights reserved.
 *
 * $Id: isdigit.c,v 1.1.1.1 2009/04/20 07:15:34 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * isdigit --
 *
 * PUBLIC: #ifndef HAVE_ISDIGIT
 * PUBLIC: int isdigit __P((int));
 * PUBLIC: #endif
 */
int
isdigit(c)
	int c;
{
	/*
	 * Depends on ASCII-like character ordering.
	 */
	return (c >= '0' && c <= '9' ? 1 : 0);
}
