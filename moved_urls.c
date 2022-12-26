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


struct moved_url
	{
	char *name;/* this is the moved_url */
	char *new_location;
	struct moved_url *nxtentr;
	struct moved_url *prventr;
	};
struct moved_url *moved_urltab[2]; /* first element points to first entry,
			second element to last entry */


struct moved_url *lookup_moved_url(char *name)
{
struct moved_url *pa;

if(! name) return(0);

/* pa points to next entry */
for(pa = moved_urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(pa -> name) if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_moved_url */


struct moved_url *install_moved_url_at_end_of_list(char *name)
{
struct moved_url *plast, *pnew;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_moved_url_at_end_off_list(): arg name=%s\n", name);
	}

if(! name) return(0);

pnew = lookup_moved_url(name);
if(pnew)
	{
	/* free previous definition */
	free(pnew -> new_location);
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct moved_url *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = moved_urltab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !moved_urltab[0] ) moved_urltab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
moved_urltab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_moved_url */


int delete_moved_url(char *name)/* delete entry from double linked list */
{
struct moved_url *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_moved_url(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = moved_urltab[0];
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
	/* if first one, modify moved_urltab[0] */
	if(pprev == 0) moved_urltab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify moved_urltab[1] */
	if(pnext == 0) moved_urltab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel -> new_location);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_moved_url */


int delete_all_moved_urls()/* delete all entries from table */
{
struct moved_url *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_moved_urls() arg none\n");
	}

while(1)
	{	
	pa = moved_urltab[0];
	if(! pa) break;
	moved_urltab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa -> new_location);
	free(pa);/* free structure */
	}/* end while all structures */

moved_urltab[1] = 0;
return(1);
}/* end function delete_all_moved_urls */


char *lookup_new_location(char *url)
{
struct moved_url *pa;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "lookup_new_location(): arg\nurl=%s\n", url);
	}

/* argument check */
if(! url) return(0);

ptr = strip_http_www(url);
if(! ptr) return(0);

if(debug_flag)
	{
	fprintf(stdout, "lookup_new_location(): stripped ptr=%s\n", ptr);
	}

pa = lookup_moved_url(ptr);
if(! pa) return(0);

return(pa -> new_location);
}/* end function lookup_new_location */


int add_new_location(char *url, char *new_location)
{
struct moved_url *pa;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_new_location(): arg\nurl=%s\nnew_location=%s\n",\
	url, new_location);
	}

/* argument check */
if(! url) return(0);
if(! new_location) return(0);

ptr = strip_http_www(url);
if(! ptr) return(0);

if(debug_flag)
	{
	fprintf(stdout, "add_new_location(): stripped ptr=%s\n", ptr);
	}

pa = install_moved_url_at_end_of_list(ptr);
if(! pa) return(0);

pa -> new_location = strsave(new_location);
if(! pa -> new_location) return(0);

return(1);
}/* end function add_new_location */


char *strip_http_www(char *url)
{
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "strip_http_www(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

ptr = url;
/* strip any leading 'http://' */
if(strncasecmp(url, "http://", 7) == 0) ptr += 7;
if(strncasecmp(ptr, "www.", 4) == 0) ptr += 4;

if(debug_flag)
	{
	fprintf(stdout, "strip_http_www(): stripped ptr=%s\n", ptr);
	}

return(ptr);
}/* end function strip_http_www */

