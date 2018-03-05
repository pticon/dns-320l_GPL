#ifndef _POT_H
#define _POT_H

#ifdef __GETTEXT__

#include <libintl.h>

#define _(x)	gettext((x))

#else

#define _(x) 	(x)

#endif

void gettexton(void);

#endif
