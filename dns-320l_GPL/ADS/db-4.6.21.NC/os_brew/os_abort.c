/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2005,2007 Oracle.  All rights reserved.
 *
 * $Id: os_abort.c,v 1.1.1.1 2009/04/20 07:15:26 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"

/*
 * __os_abort --
 */
void
__os_abort()
{
	AEEApplet *app;

	app = (AEEApplet *)GETAPPINSTANCE();
	ISHELL_CloseApplet(app->m_pIShell, FALSE);

	/* NOTREACHED */
}
