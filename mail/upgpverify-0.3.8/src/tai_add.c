/* Reimplementation of Daniel J. Bernsteins tai library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.tai_add.c 1.3 01/05/02 09:55:29+00:00 uwe@fjoras.ohse.de $ */
#include "tai.h"

void
tai_add (struct tai *target, struct tai *s1, struct tai *s2)
{
	target->x = s1->x + s2->x;
}
