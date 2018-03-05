/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2001,2007 Oracle.  All rights reserved.
 *
 * $Id: BtreePrefixCalculator.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */

package com.sleepycat.db;

public interface BtreePrefixCalculator {
    int prefix(Database db, DatabaseEntry dbt1, DatabaseEntry dbt2);
}
