/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.taia_frac.c 1.3 01/05/03 06:21:05+00:00 uwe@fjoras.ohse.de $ */
#include "taia.h"

#define N (1000000000.0)

double
taia_frac (struct taia *t)
{
	return (t->atto / N + t->nano) / N;
}
