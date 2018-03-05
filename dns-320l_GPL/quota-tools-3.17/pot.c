#include "pot.h"
#include <locale.h>

/*************************************************************************
 * if you want to turn off gettext without changing sources edit pot.h 
 *************************************************************************/

void gettexton(void)
{
#ifdef __GETTEXT__
	setlocale(LC_ALL, "");
	bindtextdomain("quota", "/usr/share/locale");
	textdomain("quota");
#endif
}
