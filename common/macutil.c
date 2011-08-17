/*
 $Id$
 Copyright 1995, 2003, 2004, 2005 Eric L. Smith <eric@brouhaha.com>
 
 Nonpareil is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.  Note that I am not
 granting permission to redistribute or modify Nonpareil under the
 terms of any later version of the General Public License.
 
 Nonpareil is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program (in the file "COPYING"); if not, write to the
 Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 MA 02111, USA.
 */

//
// Changes for Mac OS X and iOS by Maciej Bartosiak
// Copyright 2005-2010 Maciej Bartosiak
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>

#include "macutil.h"

void *alloc (size_t size)
{
	void *p;
	
	//p = calloc (1, size);
    p = malloc (size);
    memset(p,0,size);
	if (! p)
		fatal (2, "Memory allocation failed\n");
	return (p);
}

void trim_trailing_whitespace (char *s)
{
	int i;
	char c;
	
	i = strlen (s);
	while (--i >= 0)
    {
		c = s [i];
		if ((c == '\n') || (c == '\r') || (c == ' ') || (c == '\t'))
			s [i] = '\0';
		else
			break;
    }
}

size_t fread_bytes  (FILE *stream,
					 void *ptr,
					 size_t byte_count,
					 bool *eof,
					 bool *error)
{
	size_t total = 0;
	
	*eof = false;
	*error = false;
	
	while (byte_count)
    {
		size_t count;
		
		count = fread (ptr, 1, byte_count, stream);
		ptr += count;
		total += count;
		byte_count -= count;
		if (byte_count)
		{
			if (ferror (stream))
			{
				*error = true;
				return total;
			}
			if (feof (stream))
			{
				*eof = true;
				return total;
			}
		}
    }
	return total;
}

void fatal (int ret, char *format, ...)
{
	va_list ap;
	
	if (format)
    {
		fprintf (stderr, "fatal error: ");
		va_start (ap, format);
		vfprintf (stderr, format, ap);
		va_end (ap);
    }
	//if (ret == 1)
	//	usage (stderr);
	exit (ret);
}

/*void warning (char *format, ...)
{
	va_list ap;
	
	fprintf (stderr, "warning: ");
	va_start (ap, format);
	vfprintf (stderr, format, ap);
	va_end (ap);
}*/

