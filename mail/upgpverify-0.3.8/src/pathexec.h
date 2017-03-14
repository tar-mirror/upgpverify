/* Reimplementation of Daniel J. Bernsteins pathexec library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: s.pathexec.h 1.3 01/06/11 13:26:52+00:00 uwe@fjoras.ohse.de $ */
#ifndef PATHEXEC_H
#define PATHEXEC_H

void pathexec_run(const char *,char *const*,char *const*);
int pathexec_env(const char *varname,const char *value);
void pathexec(char *const *av);

#endif
