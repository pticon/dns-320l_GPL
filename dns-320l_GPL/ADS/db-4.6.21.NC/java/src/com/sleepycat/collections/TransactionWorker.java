/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2007 Oracle.  All rights reserved.
 *
 * $Id: TransactionWorker.java,v 1.1.1.1 2009/04/20 07:15:16 jack Exp $
 */

package com.sleepycat.collections;

/**
 * The interface implemented to perform the work within a transaction.
 * To run a transaction, an instance of this interface is passed to the
 * {@link TransactionRunner#run} method.
 *
 * @author Mark Hayes
 */
public interface TransactionWorker {

    /**
     * Perform the work for a single transaction.
     *
     * @see TransactionRunner#run
     */
    void doWork()
        throws Exception;
}
