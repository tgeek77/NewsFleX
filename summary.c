/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997 <Jan Mourer>
email: pante@pi.net
snail mail: PO BOX 61  SINT ANNA  9076ZP HOLLAND

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;


char *fold_text(char *text)
{
int c;
char *ptr, *optr;
char *folded_text;

if(debug_flag)
	{
	fprintf(stdout, "fold_text(): arg text=%s\n", text);
	}

/* argument check */
if(! text) return(0);

/*if(text[0] == 0) return(0);*/  /* empty string */

/* create space */
folded_text = (char *) malloc(2 * (strlen(text) + 1) + 1);
/* 2 * in case all LF, one extra in case forgotten LF at end of text */
if(! folded_text) return(0);

ptr = text;
optr = folded_text;
while(1)
	{
	c = *ptr;
	*optr = c;
	if(c == 0) break;/* end of text */
	ptr++;
	optr++;
	if(c == '\n')
		{
		/* remove empty lines */
		if(*ptr == '\n') /* emty line */
			{
			optr--;
			continue;
			}

		/* text ended with LF, remove LF, LF  will be added later */
		if(*ptr == 0)
			{
			*(optr - 1) = 0;
			break;
			}

		/* if no space and no tab (no LWS), add space */
		if( (*ptr != ' ') && (*ptr != 9) )
			{
			*optr = ' ';/* insert space */			
			optr++;
			}

		}
	}/* end while all chars in text */

return(folded_text);
}/* end function fold_text */


