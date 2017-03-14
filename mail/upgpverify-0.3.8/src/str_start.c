/* reimplementation of str_start by djb@pobox.com */
/* uwe@ohse.de, 20000521. GPL. */


#include "str.h"

int 
str_start(const char *s,const char *t)
{
	unsigned int i=0;
	for (;;) {
		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;
	}
}
