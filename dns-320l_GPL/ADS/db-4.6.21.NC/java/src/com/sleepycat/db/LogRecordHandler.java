/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2007 Oracle.  All rights reserved.
 *
 * $Id: LogRecordHandler.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */
package com.sleepycat.db;

public interface LogRecordHandler {
    int handleLogRecord(Environment dbenv,
                        DatabaseEntry logRecord,
                        LogSequenceNumber lsn,
                        RecoveryOperation operation);
}
