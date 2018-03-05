/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2007 Oracle.  All rights reserved.
 *
 * $Id: RecordNumberAppender.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */
package com.sleepycat.db;

public interface RecordNumberAppender {
    void appendRecordNumber(Database db, DatabaseEntry data, int recno)
        throws DatabaseException;
}
