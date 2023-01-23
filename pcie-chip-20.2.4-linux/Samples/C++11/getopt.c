/* Modified version of getopt

   Copyright (C) 1989-1994, 1996-1999, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#if ! defined(__unix__) && !defined(__APPLE__)

#include <string.h>
#include <stdio.h>
#include "getopt.h"

#ifndef NULL
# define NULL     0
#endif
#define EOF     (-1)
#define ERR(str, chr) (opterr ? fprintf(stderr, "%s%s%c\n", argv[0], str, chr) : 0)

int opterr = 1;
int optind = 1;
int optopt = 0;
char* optarg = 0;


int getopt(int argc, char **argv, char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1) {
		if(optind >= argc || (argv[optind][0] != '+' &&
			argv[optind][0] != '-') || argv[optind][1] == '\0')
			return EOF;
		else if(strcmp(argv[optind], "--") == 0) {
			optind++;
			return EOF;
		}
		/* '+' for config options, '+' should not be in the opts list */
		if (argv[optind][0] == '+') {
			optarg = argv[optind++] + 1;
			return '+';
		}
	}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return '\0';
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return '\0';
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return c;
}

#endif
