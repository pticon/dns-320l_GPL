/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997,2007 Oracle.  All rights reserved.
 *
 * $Id: raise.c,v 1.1.1.1 2009/04/20 07:15:33 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * raise --
 *	Send a signal to the current process.
 *
 * PUBLIC: #ifndef HAVE_RAISE
 * PUBLIC: int raise __P((int));
 * PUBLIC: #endif
 */
int
raise(s)
	int s;
{
	return (kill(getpid(), s));
}
