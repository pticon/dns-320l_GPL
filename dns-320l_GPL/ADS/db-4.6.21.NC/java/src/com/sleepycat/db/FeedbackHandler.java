/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997,2007 Oracle.  All rights reserved.
 *
 * $Id: FeedbackHandler.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */
package com.sleepycat.db;

public interface FeedbackHandler {
    void recoveryFeedback(Environment dbenv, int percent);
    void upgradeFeedback(Database db, int percent);
    void verifyFeedback(Database db, int percent);
}
