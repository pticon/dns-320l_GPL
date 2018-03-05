# $Id: count.awk,v 1.1.1.1 2009/04/20 07:15:34 jack Exp $
#
# Print out the number of log records for transactions that we
# encountered.

/^\[/{
	if ($5 != 0)
		print $5
}
