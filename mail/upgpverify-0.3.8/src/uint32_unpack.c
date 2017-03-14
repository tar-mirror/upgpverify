/* Reimplementation of Daniel J. Bernsteins uint library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.uint32_unpack.c 1.3 01/04/29 08:00:27+00:00 uwe@fjoras.ohse.de $ */
#include "uint32.h"

void 
uint32_unpack(const char s[4],uint32 *u)
{
	const unsigned char *t=s;
	*u=(t[3] << 24) + (t[2] << 16) + (t[1] << 8) + t[0];
}

void 
uint32_unpack_big(const char s[4],uint32 *u)
{
	const unsigned char *t=s;
	*u=(t[0] << 24) + (t[1] << 16) + (t[2] << 8) + t[3];
}
