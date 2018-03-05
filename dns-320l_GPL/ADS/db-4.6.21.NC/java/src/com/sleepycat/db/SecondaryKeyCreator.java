/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2007 Oracle.  All rights reserved.
 *
 * $Id: SecondaryKeyCreator.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */

package com.sleepycat.db;

public interface SecondaryKeyCreator {
    boolean createSecondaryKey(SecondaryDatabase secondary,
                                      DatabaseEntry key,
                                      DatabaseEntry data,
                                      DatabaseEntry result)
        throws DatabaseException;
}
