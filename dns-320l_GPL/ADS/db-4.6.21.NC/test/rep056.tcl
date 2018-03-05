# See the file LICENSE for redistribution information.
#
# Copyright (c) 2005,2007 Oracle.  All rights reserved.
#
# $Id: rep056.tcl,v 1.1.1.1 2009/04/20 07:15:19 jack Exp $
#
# TEST  rep056
# TEST	Replication test with in-memory named databases.
# TEST
# TEST	Rep056 is just a driver to run rep001 with in-memory
# TEST	named databases.

proc rep056 { method args } {
	source ./include.tcl

	# Valid for all access methods.
	if { $checking_valid_methods } {
		return "ALL"
	}

	eval { rep001 $method 1000 "056" } $args
}
