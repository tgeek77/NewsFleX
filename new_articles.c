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

/* 
 The new-article list is used to keep track of incoming articles.
 It is cleared before getting any data from the server.
 During the session, any article retrieved (header or body) is stored in
 this list.
 Then immediatly after the connection closes, the filter routines will
 operate on all article headers (and -bodies) in this list.
 This saves online time, and prevents from reareading the whole data base,
 which could take a long time.
*/


#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;


struct new_article
	{
	char *name;/* this is the group and article id long decimal */
	struct new_article *nxtentr;
	struct new_article *prventr;
	}; 
struct new_article *new_articletab[2];
/* first element points to first entry, second element to last entry */


struct new_article *lookup_new_article(char *name)
{
struct new_article *pa;

/*pa points to next entry*/
for(pa = new_articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_new_article */


struct new_article *install_new_article_at_end_of_list(char *name)
{
struct new_article *plast, *pnew;
struct new_article *lookup_new_article();

if(debug_flag) fprintf(stdout, "install_new_article(): arg name=%s\n", name);

pnew = lookup_new_article(name);
if(pnew)
	{
	/* free previous definition */
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct new_article *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = new_articletab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !new_articletab[0] ) new_articletab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
new_articletab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_new_article_at_end_of_list */


int clear_new_articles_list()/* delete all entries from table */
{
struct new_article *pa;

if(debug_flag) 
	{
	fprintf(stdout, "clear_new_articles_list() arg none\n");
	}

while(1)
	{	
	pa = new_articletab[0];
	if(! pa) break;
	new_articletab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */
new_articletab[1] = 0;
return(1);
}/* end function clear_new_articles_list */


int add_to_new_list(char *group, long article_id)
{
char temp[612];
struct new_article *pa, *install_new_article_at_end_of_list();

if(debug_flag)
	{
	fprintf(stdout,\
	"add_to_new_list(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* Note: name must be unique */
sprintf(temp, "%s %ld", group, article_id);

/*
 if retrieving the body, and the header was in this list already (threads),
 the entry is overwritten.
*/
pa = install_new_article_at_end_of_list(temp);
if(! pa) return(0);
return(1);
}/* end function add_to_new_list */


int set_all_filter_flags_for_new_articles()
{
struct new_article *pa;
char group[TEMP_SIZE];
long article_id;

if(debug_flag)
	{
	fprintf(stdout, "set_all_filter_flags_for_new_articles(): arg none\n");
	}
	
fl_set_object_label(fdui -> command_status_display,\
"Filtering all new articles");
XSync(fl_get_display(), 0);

for(pa = new_articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	sscanf(pa -> name, "%s %ld", group, &article_id);
	
	apply_filters_to_article(group, article_id);

	}/* end for new articles */

fl_set_object_label(fdui -> command_status_display, "");
/*XSync(fl_get_display(), 0);*/

set_new_data_flag(0);

return(1);
}/* end function set_all_filter_flags_in_group */


int remove_from_new_list(char *group, long article_id)
{
char temp[612];
struct new_article *pa, *pdel, *pnext, *pprev;

if(debug_flag)
	{
	fprintf(stdout,\
	"remove_from_new_list(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* Note: name must be unique */
sprintf(temp, "%s %ld", group, article_id);

pa = new_articletab[0];
while(1)
	{
	/* if end list, return not found */
	if(! pa) return(0);

	/* test for match in name */
	if(strcmp(temp, pa -> name) != 0) /* no match */
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
	/* if first one, modify articletab[0] */
	if(pprev == 0) new_articletab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify articletab[1] */
	if(pnext == 0) new_articletab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */

return(1);
}/* end function remove_from_new_list */

