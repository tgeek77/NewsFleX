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


int url_history_position;
char *url_history_selected_url;


struct displayed_url
	{
	char *name;
	int sequence;
	int top_line;
	struct displayed_url *nxtentr;
	struct displayed_url *prventr;
	};
struct displayed_url *displayed_urltab[2]; /* first element points to first entry,
			second element to last entry */


struct displayed_url *lookup_displayed_url(char *name)
{
struct displayed_url *pa;

/*pa points to next entry*/
for(pa = displayed_urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_displayed_url */


struct displayed_url *install_displayed_url_at_end_of_list(char *name)
{
struct displayed_url *plast, *pnew;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_displayed_url_at_end_off_list(): arg name=%s\n", name);
	}

/* create new structure */
pnew = (struct displayed_url *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = displayed_urltab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previous structure pointers */
if( !displayed_urltab[0] )
	{
	displayed_urltab[0] = pnew; /* first element in list */
	pnew -> sequence = 0;
	}
else
	{
	plast -> nxtentr = pnew;
	pnew -> sequence = plast -> sequence + 1;
	}

/* set array end pointer */
displayed_urltab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_displayed_url */


int delete_displayed_url(char *name)/* delete entry from double linked list */
{
struct displayed_url *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_displayed_url(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = displayed_urltab[0];
while(1)
	{
	/* if end list, return not found */
	if(! pa) return(0);

	/* test for match in name */
	if(strcmp(name, pa -> name) != 0) /* no match */
		{
		/* point to next element in list */
		pa = pa -> nxtentr;

		/* loop for next element in list */
		continue;
		}

	/* we now know which struture to delete */
	pdel = pa;

	/* get previous and next structure */
	pnext = pa -> nxtentr;
	pprev = pa -> prventr;

	/* set pointers for previous structure */
	/* if first one, modify displayed_urltab[0] */
	if(pprev == 0) displayed_urltab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify displayed_urltab[1] */
	if(pnext == 0) displayed_urltab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_displayed_url */


int delete_all_displayed_urls()/* delete all entries from table */
{
struct displayed_url *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_displayed_urls() arg none\n");
	}

while(1)
	{	
	pa = displayed_urltab[0];
	if(! pa) break;
	displayed_urltab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */

displayed_urltab[1] = 0;
return(1);
}/* end function delete_all_displayed_urls */


int add_to_url_history(char *url, int topline)
{
struct displayed_url *pa;

/* argumnet check */
if(! url) return(0);
if(topline < 1) return(0);

if(debug_flag)
	{
	fprintf(stdout, "add_to_url_histtory(): arg url=%s topline=%d\n",\
	url, topline);
	}

pa = install_displayed_url_at_end_of_list(url);
if(! pa) return(0);

pa -> top_line = topline;

/* this is where we are now */
url_history_position = pa -> sequence;

return(1);
}/* end function add_to_url_history */


char *get_previous_url(int *topline)
{
struct displayed_url *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_previous_url(): arg none\n");

	fprintf(stdout, "url_history_position=%d\n", url_history_position);
	}

for(pa = displayed_urltab[1]; pa != 0; pa = pa -> prventr)
	{
	if(pa -> sequence < url_history_position)
		{
		url_history_position = pa -> sequence;
		*topline = pa -> top_line;

		if(debug_flag)
			{
			fprintf(stdout,\
			"get_previous_url(): returning topline=%d\n", *topline);
			}

		if(url_history_selected_url) free(url_history_selected_url);
		url_history_selected_url = strsave(pa -> name);
		if(! url_history_selected_url) return(0);
				
		return(pa -> name);
		}
	
	}/* end for all structures */

return(0);
}/* end function get_previous_url */


char *get_next_url(int *topline)
{
struct displayed_url *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_next_url(): arg none\n");

	fprintf(stdout, "url_history_position=%d\n", url_history_position);
	}

for(pa = displayed_urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(pa -> sequence > url_history_position)
		{
		url_history_position = pa -> sequence;
		*topline = pa -> top_line;

		if(debug_flag)
			{
			fprintf(stdout, "get_next_url(): returning topline=%d\n", *topline);
			}

		if(url_history_selected_url) free(url_history_selected_url);
		url_history_selected_url = strsave(pa -> name);
		if(! url_history_selected_url) return(0);
		
		return(pa -> name);
		}

	}/* end for all structures */

return(0);
}/* end function_get_next_url */


int set_url_history_current_topline(int topline)
{
struct displayed_url *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_url_history_topline(): arg topline=%d\n",
	topline);
	}

/* argument check */
if(! reload_url) return(0);

for(pa = displayed_urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, reload_url) == 0)
		{
		pa -> top_line = topline;
		return(1);
		}	

	}/* end for all structures */

/* not found */
return(0);
}/* end function set_url_history_current_topline */


