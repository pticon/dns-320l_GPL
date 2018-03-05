/*
 * $Id: pam_modutil_cleanup.c,v 1.1.1.1 2009/04/20 07:15:46 jack Exp $
 *
 * This function provides a common pam_set_data() friendly version of free().
 */

#include "pam_modutil_private.h"

#include <stdlib.h>

void
pam_modutil_cleanup (pam_handle_t *pamh UNUSED, void *data,
		     int error_status UNUSED)
{
    if (data) {
	/* junk it */
	(void) free(data);
    }
}
