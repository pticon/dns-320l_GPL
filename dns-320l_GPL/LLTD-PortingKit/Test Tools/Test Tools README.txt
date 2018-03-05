Test Tools for the LLTD protocol are included via the WDK (Windows Driver Kit) and are subject to all applicable WDK license(s) and terms of use.  See http://www.microsoft.com/whdc/devtools/wdk/default.mspx
The following can be used to extract the LLTD test tools from a DTM controller machine on which the WDK has been set up to verify a device LLTD implementation:
	: xcopy /Y "%RTLogTstBin%\NetTest\netxp\veritool\*" LLTD
	: xcopy /Y "%RTLogTstBin%\NetTest\netxp\homenet\lld2\tools\*" LLTD
