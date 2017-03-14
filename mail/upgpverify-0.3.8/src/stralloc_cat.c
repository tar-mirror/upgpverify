/* Reimplementation of Daniel J. Bernsteins stralloc library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.stralloc_cat.c 1.4 01/05/01 08:01:18+00:00 uwe@fjoras.ohse.de $ */

#include "stralloc.h"

int stralloc_cat(stralloc *to,stralloc *from)
{
	return stralloc_catb(to,from->s,from->len);
}
