/* Keen Dreams Gold Edition: Keen Worlds native companion
 * Copyright (C) 2026 Anders Christensen
 * Borland C library calls the DOS source uses, for modern compilers.
 * GPL v2 or later (see LICENSE).
 */
#ifndef KDG_PORT_H
#define KDG_PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef EINVFMT
#define EINVFMT	EINVAL
#endif

static inline char *kdg_itoa (long value, char *buf, int radix)
{
	if (radix == 16) sprintf(buf, "%lx", (unsigned long)value & 0xffffffffUL);
	else sprintf(buf, "%ld", value);
	return buf;
}
static inline char *kdg_ultoa (unsigned long value, char *buf, int radix)
{
	if (radix == 16) sprintf(buf, "%lx", value);
	else sprintf(buf, "%lu", value);
	return buf;
}
#define itoa(v,b,r)	kdg_itoa((long)(int)(v),(b),(r))
#define ltoa(v,b,r)	kdg_itoa((long)(v),(b),(r))
#define utoa(v,b,r)	kdg_ultoa((unsigned long)(unsigned)(v),(b),(r))
#define ultoa(v,b,r)	kdg_ultoa((unsigned long)(v),(b),(r))

static inline char *kdg_strupr (char *s)
{
	for (char *p = s; *p; ++p)
		if (*p >= 'a' && *p <= 'z') *p -= 32;
	return s;
}
#define strupr kdg_strupr

#define _fmemcpy	memcpy
#define _fmemset	memset
#define _fstrcpy	strcpy
#define _fstrlen	strlen
#define _fmemcmp	memcmp

#define S_IREAD		0400
#define S_IWRITE	0200

#endif
