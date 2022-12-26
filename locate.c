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

char *s_text;
char *s_text_start;

int search_for_keyword(FL_OBJECT *obj)
{
char *keyword;
int first_flag;

if(debug_flag)
	{
	fprintf(stdout, "search_for_keyword arg obj=%ld using s_text=%ld\n",\
	(long) obj, (long) s_text);
	}

/* argument check */
if(! obj) return(0);

keyword = (char*) fl_show_input("Keyword?", "");
if(! keyword) return(0);
if(strlen(keyword) == 0) return(0);

first_flag = 1;
while(1)
	{
	if(! make_visible_line_of_text_with_keyword_in_input_field(\
	obj, keyword, first_flag) )
		{
		return(0);
		}
	first_flag = 0;

	keyword = (char*) fl_show_input("Next keyword?", keyword);
	if(! keyword)
		{
		free(s_text_start);
		return(0);
		}
	}/* end while */
	
return(1);
}/* end function search_for_keyword */


int make_visible_line_of_text_with_keyword_in_input_field(\
FL_OBJECT *obj, char *keyword, int first)
{
int column;
char *ptr, *match;
static int s_line;

if(debug_flag)
	{
	fprintf(stdout,\
	"make_visible_line_of_text_with_keyword_in_input_field():\n\
arg obj=%ld keyword=%s first=%d using s_text=%ld s_line=%d\n", \
	(long) obj, keyword, first, (long) s_text, s_line);
	}

/* argument_check */
if(! keyword) return(0);

if(first)
	{
	s_text_start = strsave( (char *)fl_get_input(obj) );
	s_line = 0;
	s_text = s_text_start;
	}
if(! s_text_start) return(0);

match = strstr(s_text, keyword);
if(! match) return(0);

ptr = s_text;
column = 0;
while(1)
	{
	if(*ptr == 0) break;
	if(*ptr == 10)
		{
		column = 0;
		s_line++;
		}
	else column++;
	if(ptr >= match) break;
	ptr++;
	}

if(debug_flag)
	{
	fprintf(stdout, "column=%d s_line=%d\n", column, s_line);
	fprintf(stdout, "s_text=%ld match=%ld\n", (long) s_text, (long) match);
	}

fl_set_input_topline(obj, s_line);

fl_set_input_cursorpos(obj, column, s_line);

/* start here next time */
s_text = match + strlen(keyword);

return(1);
}/* end function make_visible_line_of_text_with_keyword_in_input_field */





