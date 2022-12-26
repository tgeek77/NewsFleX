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

/*
The purpose of this list is to be able to send message id's of marked
articles to the server, where all articles in a group are send at the time.
Doing it from the article_body list would send them in a sequence depending
on the hash value.
This results in constant saving and reloading of the article structure
if the group changes, while receiving the article bodies.
This slows down the download of the marked article bodies considerably.
Now keeping 2 lists, with the same data, but differently stored.
One (article_body) is used to quickly look up the group and article number
belonging to a received message id, and this one (article_body2) is used to
quickly send all the message id's in each group, one group at the time, in
the sequence they were stored (non hashing).
*/

struct article_body2
	{
	char *name;/* this is the message_id */
	struct article_body2 *nxtentr;
	struct article_body2 *prventr;
	};
struct article_body2 *article_body2tab[2]; /* first element points to first entry,
			second element to last entry */


struct article_body2 *lookup_article_body2(char *name)
{
struct article_body2 *pa;

/*pa points to next entry*/
for(pa = article_body2tab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_article_body2 */


struct article_body2 *install_article_body2_at_end_of_list(char *name)
{
struct article_body2 *plast, *pnew;
struct article_body2 *lookup_article_body2();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_article_body2_at_end_off_list(): arg name=%s\n", name);
	}

pnew = lookup_article_body2(name);
if(pnew)
	{
	/* free previous definition */
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct article_body2 *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = article_body2tab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !article_body2tab[0] ) article_body2tab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
article_body2tab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_article_body2 */


int clear_article_body2_list()/* delete all entries from table */
{
struct article_body2 *pa;

if(debug_flag)
	{
	fprintf(stdout, "clear_article_body2_list() arg none\n");
	}

while(1)
	{	
	pa = article_body2tab[0];
	if(! pa) break;
	article_body2tab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */

article_body2tab[1] = 0;
return(1);
}/* end function clear_article_body2_list() */


int add_to_article_body2_list(char *message_id)
{
struct article_body2 *pa, *install_article_body2_at_end_of_list();

if(debug_flag)
	{
	fprintf(stdout, "add_to_article_body2_list(): arg message_id=%s\n",\
	message_id);
	}

pa = install_article_body2_at_end_of_list(message_id);
if(! pa) return(0);

return(1);
}/* end function add_to_article_body2_list */


int send_all_message_ids_to_server2()
{
struct article_body2 *pa;
char temp[READSIZE];

if(debug_flag)
	{
	fprintf(stdout, "send_all_message_ids_to_server2(): arg none\n");
	}

for(pa = article_body2tab[0]; pa != 0; pa = pa -> nxtentr)
	{
	sprintf(temp, "BODY %s\n", pa -> name);
	send_to_news_server(temp);
	}
return(1);
}/* end function send_all_message_ids_to_server2 */

