/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2007 Oracle.  All rights reserved.
 *
 * $Id: KeyRangeException.java,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */

package com.sleepycat.util.keyrange;

/**
 * An exception thrown when a key is out of range.
 *
 * @author Mark Hayes
 */
public class KeyRangeException extends IllegalArgumentException {

    /**
     * Creates a key range exception.
     */
    public KeyRangeException(String msg) {

        super(msg);
    }
}
