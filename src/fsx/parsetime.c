/*
 * Copyright (c) 1998-2009 Apple Inc. All rights reserved.
 * Copyright (c) 2026 Kirk Russell. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "parsetime.h"

static long long
clamp(long long v)
{
	if (v > INT_MAX)
		return INT_MAX;
	if (v < INT_MIN)
		return INT_MIN;
	return v;
}

int
parsetime(const char *t)
{
	static const struct {
		char c;
		int mult;
	} units[] = {
		{ 's', 1 },
		{ 'm', 60 },
		{ 'h', 3600 },
		{ 'd', 86400 },
		{ 'w', 604800 },
		{ 'y', 31536000 },
	};
	const char *t_orig = t;
	const char *num = t; /* start of the current number */
	long long secs = 0; /* clamped to INT_MIN..INT_MAX, so never overflows */
	long long val;

	for (; *t; t++) {
		size_t u;
		for (u = 0; u < sizeof(units) / sizeof(units[0]); u++)
			if (units[u].c == *t) {
				/* like atoi(): leading sign, stop at "." etc. */
				val = clamp(strtol(num, NULL, 10));
				secs = clamp(secs + val * units[u].mult);
				num = t + 1;
				break;
			}
	}
	if (secs)
		return (int)secs;
	/* keep the "bare number" fallback */
	return (int)clamp(strtol(t_orig, NULL, 10));
}
