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
 The body list is used for retrieval of the article bodies.
 In case of 'get marked article bodies', before connecting to the server, 
 all message id's from the marked articles are stored in this list, along
 with the group each article belongs to.
 Then once connected, the article bodies are requested by message id, not by
 number and group.
 Then for a received message id, the group is looked up, and the article
 body stored in that group.	 
 */


#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

#define ARTICLE_BODY_HASH_SIZE 100

struct article_body
	{
	char *name;/* this is the message_id of the header */
	char *group;
	long article;
	struct article_body *nxtentr;
	}; 
struct article_body *article_bodytab[ARTICLE_BODY_HASH_SIZE];


struct article_body *lookup_article_body(char *name)
{
struct article_body *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout, "lookup_article_body(): arg name=%s\n", name);
	}
	
for(pa = article_bodytab[hash(name)]; pa != 0; pa = pa -> nxtentr)
	{
	/* server sometimes return lower case if send in upper case */
	/* this does not work because  of hashing, maybe in the wrong chain */
/*	if(strcasecmp(pa -> name, name) == 0) return(pa); */
	if(strcmp(pa -> name, name) == 0) return(pa); /* found sequence entry */
	}

/*
now looking case independent for all entries
Seldom here, but sometimes server converts case.
*/
for(i = 0; i < ARTICLE_BODY_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = article_bodytab[i]; pa != 0; pa = pa -> nxtentr)
		{
		if(strcasecmp(pa -> name, name) == 0) return(pa);/* found */
		}
	}/* end for all entries in article_bodytab */

return(0); /* not found */
}/* end function lookup_article_body */


struct article_body *install_article_body(char *name)
{
struct article_body *pnew, *pnext, *lookup_article_body();
int hashval;

if(debug_flag)
	{
	fprintf(stdout, "install_article_body(): arg name=%s\n", name);
	}
	
/* create new structure */
pnew = (struct article_body *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get next structure */
hashval = hash(name);
pnext = article_bodytab[hashval];/* may be zero, if there was nothing */

/* insert before next structure (if any, else at start) */
article_bodytab[hashval] = pnew;

/* set pointers for new structure */
pnew -> nxtentr = pnext;

return(pnew);/* pointer to new structure */
}/* end function install_article_body */


int clear_article_body_list()
{
struct article_body *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout, "clear_article_body_list(): arg none\n");
	}

for(i = 0; i < ARTICLE_BODY_HASH_SIZE; i++)/* for all structures at this position */
	{
	while(1)
		{
		pa = article_bodytab[i];
		if(! pa) break;
		article_bodytab[i] = pa -> nxtentr;/* article_bodytab entry points to next one,
															this could be 0
															*/
		free(pa -> name);/* free name */
		free(pa -> group);
		free(pa);/* free structure */
		}/* end while all structures hashing to this value */ 
	}/* end for all entries in article_bodytab */

acquire_body_count = 0;

return(1);
}/* end function clear_article_body_list() */


int add_to_article_body_list(char *message_id, char *group, long article)
{
struct article_body *pa, *install_article_body();

if(debug_flag)
	{
	fprintf(stdout,\
	"add_to_article_body_list(): arg message_id=%s group=%s\n",\
	message_id, group);
	}

/* argument check */
if(! message_id) return(0);
if(! group) return(0);
if(article < 0) return(0);

/* test if already in list */
pa = lookup_article_body(message_id);
if(pa) return(1);/* already there */

pa = install_article_body(message_id);
if(! pa) return(0);

pa -> group = strsave(group);
if(! pa) return(0);

pa -> article = article;

return(1);
}/* end function add_to_article_body_list */


int send_all_message_ids_to_server()
{
struct article_body *pa;
int i;
char temp[READSIZE];

if(debug_flag)
	{
	fprintf(stdout, "send_all_message_ids_to_server(): arg none\n");
	}

for(i = 0; i < ARTICLE_BODY_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = article_bodytab[i]; pa != 0; pa = pa -> nxtentr)
		{
		sprintf(temp, "BODY %s\n", pa -> name);
		send_to_news_server(temp);
		}
	}/* end for all entries in article_bodytab */
return(1);
}/* end function send_all_message_ids_to_server */


char *get_article_bodies_group(char *message_id, long *article)
{
struct article_body *pa, *lookup_article_body();

if(debug_flag)
	{
	fprintf(stdout, "get_article_bodies_group(): arg message_id=%s\n",\
	message_id);
	}

/* argument check */
if(! message_id) return(0);

*article = -1;

pa = lookup_article_body(message_id);
if(! pa) return(0);

*article = pa -> article;
return(pa -> group);
}/* end function get_article_bodies_group */


