/* Reimplementation of Daniel J. Bernsteins uint library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.byte_diff.c 1.3 01/04/29 08:11:07+00:00 uwe@fjoras.ohse.de $ */
#include "byte.h"

int 
byte_diff(const char *s,unsigned int n,const char *t)
{
	for (;;) {
		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;
	}
	return ((int)(unsigned int)(unsigned char) *s)
	   - ((int)(unsigned int)(unsigned char) *t);
}
