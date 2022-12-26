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
 The new_header list is used to keep track of incoming headers.
 It is cleared before getting any data from the server.
 During the session, any header retrieved is stored in this list.
 The header is also written to disk.
 Then immediatly after the connection closes, the headers are stored
 in the correct postition in the articles structure for their group.
 This saves online time, since the search for the correct insert position
 for thread and subject could take a long time if there are many articles
 in a group.
 The list is cleared if all headers have been inserted.
*/


#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

/* using a double linked list, so one element can be easily deleted */
struct new_header
	{
	char *name;/* this is the group and article id long decimal */
	int added_flag;/* 
					used if mark all (groups menu), then add_headers_from_list("*")
					is called again later, since the msg id would already be there,
					the article header would be erased if 'show article once' from
					setup selected.
					*/
	struct new_header *nxtentr;
	struct new_header *prventr;
	}; 
struct new_header *new_headertab[2];
/* first element points to first entry, second element to last entry */


struct new_header *lookup_new_header(char *name)
{
struct new_header *pa;

/*pa points to next entry*/
for(pa = new_headertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_new_header */


struct new_header *install_new_header_at_end_of_list(char *name)
{
struct new_header *plast, *pnew;
struct new_header *lookup_new_header();

if(debug_flag) fprintf(stdout, "install_new_header(): arg name=%s\n", name);

pnew = lookup_new_header(name);
if(pnew)
	{
	/* free previous definition */
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct new_header *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = new_headertab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !new_headertab[0] ) new_headertab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
new_headertab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_new_header_at_end_of_list */


int clear_new_headers_list()/* delete all entries from table */
{
struct new_header *pa;

if(debug_flag) 
	{
	fprintf(stdout, "clear_new_headers_list() arg none\n");
	}

while(1)
	{	
	pa = new_headertab[0];
	if(! pa) break;
	new_headertab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */
new_headertab[1] = 0;
return(1);
}/* end function clear_new_headers_list */


int add_to_new_header_list(char *group, long article_id)
{
char temp[612];
struct new_header *pa, *install_new_header_at_end_of_list();

if(debug_flag)
	{
	fprintf(stdout,\
	"add_to_new_header_list(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* Note: name must be unique */
sprintf(temp, "%s %ld", group, article_id);

pa = install_new_header_at_end_of_list(temp);
if(! pa) return(0);
pa -> added_flag = 0;

return(1);
}/* end function add_to_new_header_list */


int remove_from_new_header_list(char *group, long article_id)
{
char temp[612];
struct new_header *pa, *pdel, *pnext, *pprev;

if(debug_flag)
	{
	fprintf(stdout,\
	"remove_from_new_header_list(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* Note: name must be unique */
sprintf(temp, "%s %ld", group, article_id);

pa = new_headertab[0];
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
	if(pprev == 0) new_headertab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify articletab[1] */
	if(pnext == 0) new_headertab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */

return(1);
}/* end function remove_from_new_header_list */


int add_new_headers_from_list(char *spec_group)
{
struct new_header *pa, *pdel, *pprev, *pnext;
long article_id;
char group[512];
char temp[512];
/*long new_articles;*/
int delete_flag;
char *last_group;

if(debug_flag)
	{
	fprintf(stdout, "add_new_headers_from_list(): arg spec_group=%s\n",\
	spec_group);
	}

fl_set_object_label(fdui -> command_status_display,\
"Adding new article headers");
XSync(fl_get_display(), 0);

last_group = strsave("");
pa = new_headertab[0];
while(1)
	{
	if(! pa) break;
	
	/*
	If the routine was called twice or more, because of auto mark all in
	group from the groups menu.
	If we do an add_article_header twice, the header will be erased, since the
	msg id is already present. This should prevent that.
	*/
	if(pa -> added_flag)
		{
		pa = pa -> nxtentr;
		continue;
		}
	
	sscanf(pa -> name, "%s %ld", group, &article_id);
	
	/* this is used by auto mark all in a group (groups menu) */
	if(strcmp(spec_group, "*") != 0)
		{
		if(strcmp(spec_group, group) != 0)
			{
			pa = pa -> nxtentr;
			continue;
			}/* end group match */
		}/* end if specific group */
	
	free(last_group);
	last_group = strsave(group);
	
	sprintf(temp, "adding %s %ld", group, article_id);
	fl_set_object_label(fdui -> command_status_display, temp);
	XSync(fl_get_display(), 0);

	/* delete this structure entry if message_id already existed */
	delete_flag = 0;
	if(! add_article_header(group, article_id, &delete_flag) ) return(0);
	if(delete_flag)	
		{
		/* delete header from list */
		
		pdel = pa;
		
		/* get previous and next structure */
		pnext = pa -> nxtentr;
		pprev = pa -> prventr;

		/* set pointers for previous structure */
		/* if first one, modify articletab[0] */
		if(pprev == 0) new_headertab[0] = pnext;
		else pprev -> nxtentr = pnext;

		/* set pointers for next structure */
		/* if last one, modify articletab[1] */
		if(pnext == 0) new_headertab[1] = pprev;
		else pnext -> prventr = pprev;
	
		/* delete structure */	
		free(pdel -> name);
		free(pdel); /* free structure */

		pa = pnext;

		/*
		update subscribed_groups.dat for the correct number of new articles.
		*/
/*
		get_new_articles(group, &new_articles);
		if(new_articles > 0) new_articles--;
		set_new_articles(group, new_articles);
*/

		}/* end delete entry */
	else
		{
		pa -> added_flag = 1;
		pa = pa -> nxtentr;
		}
	}/* end while all structures */

/*
Make sure that the last structure is also saved to disk,
save_articles() is disabled in add_article_header() if
insert_headers_offline_flag is set.
*/
save_articles();

/*
all is done, do NOT do this twice, it will remove any selections,
if more in commands queue.
*/
if(strcmp(spec_group, "*") == 0) clear_new_headers_list();

fl_set_object_label(fdui -> command_status_display, "");
/*XSync(fl_get_display(), 0);*/

return(1);
}/* end function add_new_headers_from_list */

