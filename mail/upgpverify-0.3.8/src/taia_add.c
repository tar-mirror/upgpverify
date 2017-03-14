/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.taia_add.c 1.3 01/05/03 06:21:04+00:00 uwe@fjoras.ohse.de $ */
#include "taia.h"

#define G 1000000000UL

void
taia_add (struct taia *to, struct taia *src1, struct taia *src2)
{
	tai_add(&to->sec,&src1->sec,&src2->sec);
	to->nano = src1->nano + src2->nano;
	to->atto = src1->atto + src2->atto;
	if (to->atto >= G) {
		to->atto -= G;
		to->nano++;
	}
	if (to->nano >= G) {
		to->nano -= G;
		to->sec.x++;
	}
}
