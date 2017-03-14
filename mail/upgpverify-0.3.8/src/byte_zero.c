
/* Reimplementation of Daniel J. Bernsteins uint library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.byte_zero.c 1.3 01/04/29 08:11:08+00:00 uwe@fjoras.ohse.de $ */
#include "byte.h"

void 
byte_zero(char *s,unsigned int n)
{
	for (;;) {
		if (!n) break; 
		s[--n]=0;

		if (!n) break; 
		s[--n]=0;

		if (!n) break; 
		s[--n]=0;

		if (!n) break; 
		s[--n]=0;
	}
}
