# $Id: commit.awk,v 1.1.1.1 2009/04/20 07:15:34 jack Exp $
#
# Output tid of committed transactions.

/txn_regop/ {
	print $5
}
