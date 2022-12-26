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


char *expand_posting_macros (char *text)
{
char *ptr;
int current_macro;
char *macro_name;
char *macro_def;
char *in_space;
static char *macros[] = {"$DATE", "$NAME", NULL};

if(debug_flag)
	{
	fprintf (stdout, "expand_posting_macros(): arg text=%s\n", text);
	}

/* start with text */
in_space = strsave(text);
if(! in_space) return 0;

macro_def = 0;/* for -Wall */

current_macro = 0;
/* for all macros */
while(1)
	{
	/* point to current macro */
	macro_name = macros [current_macro]; 
	if (! macro_name) break;

	switch (current_macro)
		{
		case 0:/* time */
			macro_def = strsave(get_universal_time_as_string() );
			if(! macro_def) return 0;
			break;
		case 1:/* name */
			macro_def = strsave(real_name);
			if(! macro_def) return 0;
			break;
		}/* end switch current_macro */

	while(1)
		{
		if (! replace_in(in_space, macro_name, macro_def, &ptr) )
			{
			return 0;
			}
		if(! ptr) break;

		free (in_space);
		in_space = ptr;
		}
		
	/* next macro */
	free(macro_def);

	current_macro++;
	}/* end for al macros */

return in_space;
}/* end function expand_macros */


int replace_in(char *instr, char *namestr, char *defstr, char **resstr)
{
/*
replaces FIRST namestr by defstr in instr, return a new pointer
resstr is 0 if not found
return 1 for success, 0 for error.
*/
char *outstr;
char *inptr;
char *nameptr;
int c;

if(debug_flag)
	{
	fprintf(stdout,
	"replace_in(): arg instr=%s\nnamestr=%s defstr=%s\n",\
	instr, namestr, defstr);	
	}

*resstr = 0;

inptr = (char *) strstr (instr, namestr);
if(! inptr) return 1;

nameptr = namestr;

if (strncmp (inptr, namestr, strlen(namestr) ) != 0) return 0;

/* create new space with the correct size */
outstr = \
(char *)malloc ( strlen(instr) + strlen(defstr) - strlen(namestr) + 1);
if (! outstr) return 0;

c = *inptr;
*inptr = 0;
		
strcpy (outstr, instr);
strcat (outstr, defstr);
		
*inptr = c;
strcat (outstr, inptr + strlen (namestr) );

*resstr = outstr;

return 1;
} /* end function replace_in */


char *expand_posting_source_macros(char *text, char *group, long article_id)
{
int a;
char *ptr;
int current_macro;
char *macro_name;
char *macro_def;
char *in_space;
static char *macros[] = {"$DATE", "$NAME", "$SNAME", NULL};
char *posting_source_name;
char *posting_source_date;
struct tm *ptm;
time_t original_posting_time;
char temp[256];
char temp1[256], temp2[256], temp3[256], temp4[256], temp5[256], temp6[256];

if(debug_flag)
	{
	fprintf (stdout,\
	"expand_posting_source_macros(): arg text=%s group=%s article_id =%ld\n",\
	text, group, article_id);
	}

if(! text) return 0;
if(! group) return 0;
if (article_id < 0) return 0;

/* get the date the original was posted from its header */
posting_source_date =\
(char *) get_formatted_header_data(group, article_id, "Date:");
if(! posting_source_date)
	{
	/* date header missing */
	return 0;
	}

/*
get the name of the original poster from the header of the original post.
*/
posting_source_name =\
(char *) get_formatted_header_data(group, article_id, "From:");
if(! posting_source_name)
	{
	/* From header missing */
	return 0;
	}

/* start with text */
in_space = strsave(text);
if(! in_space) return 0;

macro_def = 0;/* for -Wall */

current_macro = 0;
/* for all macros */
while(1)
	{
	/* point to current macro */
	macro_name = macros [current_macro]; 
	if (macro_name == NULL) break;

	switch(current_macro)
		{
		case 0: /* $DATE date the original poster was send */
			macro_def = strsave(posting_source_date);
			if(! macro_def) return 0;
			break;
		case 1: /* $NAME name of the sender of the original post */
			macro_def = strsave(posting_source_name);
			if(! macro_def) return 0;
			break;
		case 2: /* $SNAME name sender original post without email address */
			/*
			we now have something like:
			jan@panteltje.demon.nl (Jan Panteltje)
			or:
			"Jan panteltje" <jan@panteltje.demon.nl>
			*/
			/*
			note this will fail if someone has more then 3 names, like:
			John Jean Sjon Doe, the last argument would be lost.
			*/
			a = sscanf(posting_source_name,\
				"%200s %200s %200s %200s",\
				temp1, temp2, temp3, temp4);
			if(a == 0) return 0;
			
			/* create space */
			macro_def = malloc(strlen(posting_source_name) + 1);
			if(! macro_def) return 0;

			/* copy only non email ('@') fields */
			strcpy(macro_def, "");
			if(a >= 1) if(strchr(temp1, '@') == 0) 
				{
				strcat(macro_def, temp1);
				}
			if(a >= 2) if(strchr(temp2, '@') == 0) 
				{
				strcat(macro_def, " ");
				strcat(macro_def, temp2);
				}
			if(a >= 3) if(strchr(temp3, '@') == 0) 
				{
				strcat(macro_def, " ");
				strcat(macro_def, temp3);
				}
			if(a >= 4) if(strchr(temp4, '@') == 0)
				{
				strcat(macro_def, " ");
				strcat(macro_def, temp4);
				}

			break;

		}/* end switch current_macro */

	while(1)
		{
		if (! replace_in(in_space, macro_name, macro_def, &ptr) )
			{
			return 0;
			}
		if(! ptr) break;

		free (in_space);
		in_space = ptr;
		}
		
	/* next macro */
	free(macro_def);

	current_macro++;
	}/* end for al macros */

/* free macro definitions */
free(posting_source_date);
free(posting_source_name);

return in_space;
}/* end function expand_posting_source_macros */


char *add_source_info_line(char *text, char *group, long article_id)
{
int c, i, j, k;
int tabs;
char *space;
char *ptr;
char *sptr;
char *dptr;
char *ptr2;
char *spaceptr;
char *lineptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_source_info_line(): arg text=%s group=%s article_id=%ld\n",\
	text, group, article_id);
	}

/* argument check */
if(! text) return(0);

/* 
get macro text from input field in setup,
always in global_posting_source_text
*/

/* expand the macros */
ptr =\
expand_posting_source_macros(global_posting_source_text, group, article_id);
if(! ptr) return 0;

/* wrap long lines */
if(strlen(ptr) > 78)
	{
	ptr2 = malloc(strlen(ptr) + 1 + (strlen(ptr) / 78) );
	i = 0;
	sptr = ptr;
	dptr = ptr2;
	lineptr = ptr2;
	spaceptr = lineptr;
	while(1)
		{
		/* copy chars */
		*dptr = *sptr;		

		/* test for string termination */
		if(! *sptr) break;

		/* keep a reference to teh last space encountered */
		if(*sptr == ' ') spaceptr = dptr;

		/* insert a LF if 78 chars reached */
		if(i == 78)
			{
			if(spaceptr != lineptr)/* break line at last space in line */
				{
				/* break line at last space */
				*spaceptr = '\n';				

				/* update the lineptr */
				lineptr = spaceptr;
				sptr++;
				dptr++;
				i = 0;
				continue;
				}
			else /* break line in word */
				{
				*dptr = '\n';
				dptr++;
				i = 0;
				continue;
				}
			}/* end if i == 78 */
		sptr++;
		dptr++;
		i++;
		}/* end while all chars in expanced macro text */
	free(ptr);
	ptr = ptr2;
	}/* end if strlen ptr > 78 */

/* assemble the text */
space = malloc(strlen(ptr) + strlen(text) + 3); /* 2 x slash n!! */
if(! space) return(0);

/* combine with the text */
sprintf(space, "%s\n\n%s", ptr, text);

/* free the expanded macro text */
free(ptr);

return(space);
}/* end function add_source_info_line */


