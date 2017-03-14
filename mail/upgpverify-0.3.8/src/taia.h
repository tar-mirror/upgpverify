/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.taia.h 1.3 01/05/03 06:21:04+00:00 uwe@fjoras.ohse.de $ */
#ifndef TAIA_H
#define TAIA_H

#include "tai.h"

struct taia {
  struct tai sec;
  unsigned long nano; /* 0...999999999 */
  unsigned long atto; /* 0...999999999 */
} ;

extern void taia_now(struct taia *);
extern void taia_uint(struct taia *,unsigned int);
extern void taia_add(struct taia *,struct taia *,struct taia *);
extern void taia_sub(struct taia *,struct taia *,struct taia *);
extern void taia_half(struct taia *to,struct taia *src);

extern void taia_tai(struct taia *from,struct tai *to); /* taia to tai */
extern int taia_less(struct taia *,struct taia *);
extern double taia_approx(struct taia *); 
extern double taia_frac(struct taia *);

#define TAIA_FMTFRAC 19
extern unsigned int taia_fmtfrac(char *to,struct taia *src);

#define TAIA_PACK 16
extern void taia_pack(char *to,struct taia *src);
extern void taia_unpack(const char *src,struct taia *to);

#endif
