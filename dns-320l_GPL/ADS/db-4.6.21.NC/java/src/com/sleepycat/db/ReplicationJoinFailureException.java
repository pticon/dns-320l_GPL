/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997,2007 Oracle.  All rights reserved.
 *
 * $Id: ReplicationJoinFailureException.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */
package com.sleepycat.db;

import com.sleepycat.db.internal.DbEnv;

public class ReplicationJoinFailureException extends DatabaseException {
    /* package */ ReplicationJoinFailureException(final String s,
                                   final int errno,
                                   final DbEnv dbenv) {
        super(s, errno, dbenv);
    }
}
