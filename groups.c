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
In order to improve performance I did the following:
there are 2 files, groups.dat and subscribed_groups.dat.
At load these are merged together, in such a way that:
pa -> subscribed_flag, pa -> new_articles, and pa -> article_ptr are taken
from subscribed_groups.dat.
Now anytime online either new_articles or article_ptr is changed, only
the structure is changed, and subscribed_groups.dat is written to disk.
Since the groups.dat is VERY long (in my case more then 850 KByte), and
subscribed_groups.dat is much shorter (normally :) ), this reduces online
time and saves money.
groups.dat is only saved after a LIST (online, then refresh group list).
This also prevents file corruption of groups.dat if anything goes wrong.
*/


#include "NewsFleX.h"

extern FD_NewsFleX *fdui;

#define GROUPS_HASH_SIZE 100

char stemp[TEMP_SIZE];
/*char sformatstr[20];*/

char *g_newsserver;
char *g_databse;


struct newsgroup
	{
	char *name;
	char posting_allowed_flag;
	int subscribed_flag;
/*	long article_ptr;*/
	int group_status;
	long new_articles;
/*	int filter_flags;*/
	int max_headers;
	int mark_all_flag;
	struct newsgroup *left;
	struct newsgroup *right;
	};
struct newsgroup *newsgroup = 0;


struct group
	{
	char *name;
	char posting_allowed_flag;
	int subscribed_flag;
	long article_ptr;
	int group_status;
	long new_articles;
	int filter_flags;
	int max_headers;
	int mark_all_flag;
	struct group *nxtentr;
	struct group *prventr;
	};
struct group *grouptab[GROUPS_HASH_SIZE];


struct newsgroup *install_group_in_tree(\
char *name, struct newsgroup **newsgroup)
{
int a;
/*
This routine recursively finds the place in the tree to hang the string name.
*/
static struct newsgroup *pp;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_group_in_tree(): arg name=%s newsgroup=%ld\n",\
	name, (long)newsgroup);
	}

if(*newsgroup == 0)
	{
/*
fprintf(stdout, "c n for %s\n", name);
*/
	*newsgroup = malloc(sizeof (struct newsgroup));
	if(! *newsgroup)
		{
		pp = 0;
		return(pp);
		}
	(*newsgroup) -> left = 0;
	(*newsgroup) -> right = 0;

/*
Note: name is already allocated as an element of the hash table of
structures group, just pointing to that.
*/

/*
	(*newsgroup) -> name = malloc(strlen(name) + 1);
	if(! (*newsgroup) -> name)
		{
		pp = 0;
		return(pp);
		}
	for(ptr = (*newsgroup) -> name; (*ptr++ = *name++););
*/

	(*newsgroup) -> name = name;

	pp = *newsgroup;
	return(pp);
	}
a = strcmp(name, (*newsgroup) -> name);
if(a < 0) a = -1;
if(a > 0) a = 1;
switch(a)
	{
	case 0:
		pp = *newsgroup;
		return(pp);
		break;
	case -1:
/*
fprintf(stdout,\
"m l %s in node=%s\n", name, (*newsgroup) -> name);
*/
		install_group_in_tree(name, &(*newsgroup) -> left);
		break;
	case 1:
/*
fprintf(stdout,\
"m r %s in node=%s\n", name, (*newsgroup) -> name);
*/
		install_group_in_tree(name, &(*newsgroup) -> right);
		break;
	}
return(pp);
}/* end function install_group_in_tree */


int delete_all_groups_in_tree(struct newsgroup *pa)
{
/*
if(debug_flag)
	{
	fprintf(stdout, "delete_all_groups_in_tree(): pa=%ld\n", (long)pa);
	}
*/

if(pa)
	{
	delete_all_groups_in_tree(pa -> left);
	pa -> left = 0;

/*
Note: name is allocated in hashs table of structures group, will be freeed
there.
*/
/*	free(pa -> name);*/

	delete_all_groups_in_tree(pa -> right);
	pa -> right = 0;

/*fprintf(stdout, "freeing %s\n", pa -> value);*/

	free(pa);
	}
return(1);
}/* end function delete_all_groups_in_tree */


int delete_groups_tree(struct newsgroup *pa)
{
delete_all_groups_in_tree(pa);

/* delete the pointer to the root */
newsgroup = 0;

return(1);
}/* end function delete_groups_tree */


int walk_grouplist_and_display(struct newsgroup *pa)
{
char sformatstr[4];
/*char *ptr;*/

/*
Note: filter has already been applied, only allowed groups are in this list.
*/

/*
This routine prints out the tree recursively: walk left node, print
value of this node, and walk right node
*/
if(pa)
	{
	walk_grouplist_and_display(pa -> left);

/*	ptr = malloc(strlen(pa -> name) + 69);*/
	if(groups_show_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
		{	
		if(pa -> subscribed_flag)
			{
			strcpy(sformatstr, "@C0");/* black */
			if(pa -> group_status == CANCELLED_GROUP)
				{
				strcpy(sformatstr, "@C2");/* green */
				}		

			sprintf(stemp, "%s %s <=%d %ld",\
			sformatstr, pa -> name, pa -> max_headers,\
			pa -> new_articles);
		
			if(pa -> mark_all_flag)
				{
				strcat(stemp, " GET ALL"); 
				}

			if(pa -> posting_allowed_flag == 'n')
				{
				strcat(stemp, " NO POSTING ALLOWED");
				}
			
			fl_add_browser_line(fdui -> group_list_browser, stemp);
			}
		}/* end subscribed groups */
	if(groups_show_selection == SHOW_ONLY_NEW_GROUPS)
		{
		if(pa -> group_status == NEW_GROUP)
			{
			strcpy(sformatstr, "@C1");/* red */
			
			if(pa -> subscribed_flag)
				{
				sprintf(stemp, "%s %s <=%d %ld",\
				sformatstr, pa -> name, pa -> max_headers,\
				pa -> new_articles);
				}
			else
				{
				sprintf(stemp, "%s %s %ld",\
				sformatstr, pa -> name, pa -> new_articles);
				}

			if(pa -> mark_all_flag)
				{
				strcat(stemp, " GET ALL"); 
				}

			if(pa -> posting_allowed_flag == 'n')
				{
				strcat(stemp, " NO POSTING ALLOWED");
				}
			
			fl_add_browser_line(fdui -> group_list_browser, stemp);
			}
		}/* end new groups */
	if(groups_show_selection == SHOW_ALL_GROUPS)
		{
		strcpy(sformatstr, "@C0");/* black */
		if(pa -> group_status == NEW_GROUP)
			{
			strcpy(sformatstr, "@C1");/* red */
			}
		if(pa -> group_status == ACTIVE_GROUP)
			{
			strcpy(sformatstr, "@C0");/* black */
			}
		if(pa -> group_status == CANCELLED_GROUP)
			{
			strcpy(sformatstr, "@C2");/* green */
			}		
		if(pa -> subscribed_flag) strcpy(sformatstr, "@C4");/* blue */
			
		if(pa -> subscribed_flag)
			{
			sprintf(stemp, "%s %s <=%d %ld",\
			sformatstr, pa -> name, pa -> max_headers,\
			pa -> new_articles);
			}
		else
			{
			sprintf(stemp, "%s %s %ld",\
			sformatstr, pa -> name, pa -> new_articles);
			}

		if(pa -> mark_all_flag)
			{
			strcat(stemp, " GET ALL"); 
			}

		if(pa -> posting_allowed_flag == 'n')
			{
			strcat(stemp, " NO POSTING ALLOWED");
			}
		fl_add_browser_line(fdui -> group_list_browser, stemp);
		}/* end show all groups */
/*	free(ptr);*/

	walk_grouplist_and_display(pa -> right);
	}/* end if pa */
return(1);
}/* end function walk_grouplist_and_display */


int show_groups_in_tree()
{
int browser_topline;
int browser_last_selected_line;

if(debug_flag)
	{
	fprintf(stdout, "show_groups_in_tree(): arg none\n");
	}

/* remember whch line was double clicked */
browser_last_selected_line = fl_get_browser(fdui -> group_list_browser);

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> group_list_browser);

/* clear browser */
fl_clear_browser(fdui -> group_list_browser);

walk_grouplist_and_display(newsgroup);

/* restore double_clicked_line */
fl_select_browser_line(fdui -> group_list_browser,\
browser_last_selected_line);

/* readjust the vertical slider position to the old situation */
fl_set_browser_topline(fdui -> group_list_browser, browser_topline);

to_command_status_display("");

/*unread_article_at_top_flag = 1*/

return(1);
}/* end function show_groups_in_tree */


int show_groups_tree()
{
struct group *pa;
struct newsgroup *pn;
int i;
char *ptra, *ptrb, *ptrc;
int match_failed_flag;
int last_string_flag;


if(debug_flag)
	{
	fprintf(stdout, "show_groups_tree(): arg none\n");
	}

delete_groups_tree(newsgroup);

/* mark root empty */
/*fprintf(stdout, "tree root=%ld\n", (long) newsgroup);*/
/*
fprintf(stdout, "tree left=%ld\n", (long) (newsgroup) -> left);
fprintf(stdout, "tree right=%ld\n", (long) (newsgroup) -> right);
*/
/*
*newsgroup -> left = 0;
*newsgroup -> right = 0;
*/
/*newsgroup = 0;*/

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(groups_show_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
			{	
			if(!pa -> subscribed_flag) continue;
			}/* end subscribed groups */

		if(groups_show_selection == SHOW_ONLY_NEW_GROUPS)
			{
			if(pa -> group_status != NEW_GROUP) continue;
			}/* end new groups */

		/* test if diplay allowed */
		if(filters_enabled_flag)
			{
			if(pa -> filter_flags & FILTER_DO_NOT_SHOW_GROUP) continue;
			}

		if(groups_display_filter[0])/* some filter */
			{
			/* test if filter name present in group name */
			/* for all entries in filter */
			ptra = groups_display_filter;
			ptrb = strsave(groups_display_filter);/* create space */
			if(! ptrb) return(0);
			
			match_failed_flag = 0;
			last_string_flag = 0;
			while(1)
				{
				ptrc = ptrb;/* point to start availeble space */
				while(1)/* string in filter to ptrb */
					{
					*ptrc = *ptra;
					if(! *ptra) break;/* end filter */
					if(*ptra == ' ')/* end of string in filter */
						{
						*ptrc = 0;
						ptra++;/* scip the space */
						break;
						}
					ptra++;
					ptrc++;
					}/* end while all characters in string */
				/* test if string present in name */
				if(! strstr(pa -> name, ptrb) )
					{
					match_failed_flag = 1;
					break;
					}
				if(! *ptra) break;/* end filter */
				}/* end while all strings in filter */	
			free(ptrb);/* release space */
			if(match_failed_flag) continue;
			}/* end if filter */
		pn = install_group_in_tree(pa -> name, &newsgroup);	
/*
fprintf(stdout, "show_groups_tree(): install(): pn=%ld name=%s\n",\
(long)pn, pa -> name);
*/
		if(! pn) return(0);

		pn -> posting_allowed_flag = pa -> posting_allowed_flag;
		pn -> subscribed_flag = pa -> subscribed_flag;
/*		pn -> article_ptr = pa -> article_ptr;*/
		pn -> group_status = pa -> group_status;
		pn -> new_articles = pa -> new_articles;
/*		pn -> filter_flags = pa -> filter_flags;*/
		pn -> max_headers = pa -> max_headers;
		pn -> mark_all_flag = pa -> mark_all_flag;
			
		}/* end for all structures in the chain */
	}/* end for all entries in grouptab */

show_groups_in_tree();
return(1);
}/* end function show_groups_tree */
 
 
int reset_groups()
{
/* clear the groups list, and load new data  */
fl_clear_browser(fdui -> group_list_browser);

/* allow for loading (the selected newsserver) groups again */
groups_loaded_flag = 0;

delete_all_groups();

load_subscribed_groups(USE_ALL_DATA);

return(1);
}/* end function reset_groups */


int hash(s)/* form hash value for string s */
char *s;
{
int hashval;

for(hashval = 0; *s != '\0';) hashval += *s++;
/* sum of ascii value of characters divided by tablesize */
return(hashval % GROUPS_HASH_SIZE);
}


struct group *lookup_group(char *name)
{
struct group *pa;

if(debug_flag)
	{
	fprintf(stdout, "lookup_group(): arg name=%s\n", name);
	}
	
/*
fprintf(stdout,\
"lookup_group(): looking up name=%s at grouptab=%d loc=%ld\n",\
name, hash(name), grouptab[hash(name)]);
*/

for(pa = grouptab[hash(name)]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);/* found sequence entry */
	}
return(0); /* not found */
}/* end function lookup_group */


struct group *install_group(char *name)
{
struct group *pa, *pnew, *pnext, *lookup_group();
int hashval;

if(debug_flag)
	{
	fprintf(stdout, "install_group(): arg name=%s\n", name);
	}
	
pa = lookup_group(name);

if(!pa) /* not found */
	{
	/* create new structure */
	pnew = (struct group *) calloc(1, sizeof(*pnew) );
	if(! pnew) return(0);
	pnew -> name = strsave(name);
	if(! pnew -> name) return(0);

	/* get next structure */
	hashval = hash(name);
	pnext = grouptab[hashval];/* may be zero, if there was nothing */

/*
fprintf(stdout, "install_group(): installing name=%s at grouptab=%d\n",\
name, hashval);
*/
	
	/* insert before next structure (if any, else at start) */
	grouptab[hashval] = pnew;

	/* set pointers for next structure */
	if(pnext) pnext -> prventr = pnew;
	
	/* set pointers for new structure */
	pnew -> nxtentr = pnext;
	pnew -> prventr = 0;/* always inserting at start of chain of structures */
	
	return(pnew);/* pointer to new structure */
	}/* end if not found */
return(pa);
}/* end function install_group */


int delete_all_groups()
{
struct group *pa;
int i;

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	while(1)
		{
		pa = grouptab[i];
		if(! pa) break;
		grouptab[i] = pa -> nxtentr;/* grouptab entry points to next one,
															this could be 0
															*/
		free(pa -> name);/* free name */
		free(pa);/* free structure */
		}/* end while all structures hashing to this value */ 
	}/* end for all entries in grouptab */
return(0);/* not found */
}/* end function delete_all_groups */


int get_article_ptr(char *group, long *article_ptr)
{
struct group *lookup_group(), *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_article_ptr(): arg group=%s article_ptr=%ld\n",\
	group, *article_ptr);
	}

pa = lookup_group(group);
if(! pa) return(0);

*article_ptr = pa -> article_ptr;
return(1);
}/* end function get_article_ptr */


int set_article_ptr(char *group, long article_ptr)
{
struct group *lookup_group(), *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_article_ptr(): arg group=%s article_ptr=%ld\n",\
	group, article_ptr);
	}
	
pa = lookup_group(group);
if(! pa) return(0);

pa -> article_ptr = article_ptr;
return(1);
}/* end function set_article_ptr */


int set_new_articles(char *group, long new_articles)
{
struct group *lookup_group(), *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_new_articles(): arg group=%s new_articles=%ld\n",\
	group, new_articles);
	}
	
/* argument_check */
if(! group) return(0);
if(new_articles < 0) return(0);

pa = lookup_group(group);
if(! pa) return(0);

if(pa -> new_articles == new_articles) return(1);/* no save, nothing changed */

pa -> new_articles = new_articles;

save_subscribed_groups();
return(1);
}/* end function set_new_articles */


int get_new_articles(char *group, long *new_articles)
{
struct group *lookup_group(), *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_new_articles(): arg group=%s out *new_articles\n",\
	group);
	}
	
/* argument_check */
if(! group) return(0);

*new_articles = -1;

pa = lookup_group(group);
if(! pa) return(0);

*new_articles = pa -> new_articles;
return(1);
}/* end function get_new_articles */


int mark_all_groups_cancelled()
{
int i;
struct group *pa;

if(debug_flag) fprintf(stdout, "mark_all_groups_cancelled(): arg none\n");

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		pa -> group_status = CANCELLED_GROUP;
		}/* end for all structures in the chain */
	}/* end for all entries in grouptab */
return(1);
}/* end function mark_all_groups_cancelled */


int load_groups()
{
int a;
FILE *allgroupsfile;
char temp[TEMP_SIZE];/* lines in groups dat are of know size shorter then this */
char pathfilename[TEMP_SIZE];
char name[TEMP_SIZE];
char posting_allowed_flag;
int subscribed_flag;
long article_ptr;
int group_status;
long new_articles;
int max_headers;
int filter_flags;
int hashval;
struct group *pa;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout,\
	"load_groups(): arg none\nusing news_server_name=%s\n",\
	news_server_name);
	}

if(groups_loaded_flag) return(1);

delete_all_groups();

free(g_newsserver);
g_newsserver = strsave("");
if(! g_newsserver) return(0);

to_command_status_display("Loading groups file, this may take a while");

sprintf(pathfilename, "%s/.NewsFleX/%s/groups.dat", home_dir, news_server_name);
allgroupsfile = fopen(pathfilename, "r");
if(! allgroupsfile)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not load file %s\n", pathfilename);
		}
	return(0);
	} 

while(1)
	{
	a = readline(allgroupsfile, temp);
	if(a == EOF)
		{
		/* merge subscribed groups here */
		load_subscribed_groups(MERGE_DATA);

		groups_loaded_flag = 1;
		to_command_status_display("");		
		return(1);
		}

	sscanf(temp,\
	"%s %c %d %ld  %d %ld %d %d",\
	name, &posting_allowed_flag, &subscribed_flag, &article_ptr,\
	&group_status, &new_articles, &filter_flags, &max_headers);
	
	pa = (struct group *) malloc( sizeof(*pa) );
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout, "load_groups: cannot install group %s\n", temp);
			}
		return(0);
		}

	pa -> name = strsave(name);
	if(! pa -> name) return(0);

	hashval = hash(name);
	pa -> nxtentr = grouptab[hashval];
	if(grouptab[hashval])
		{
		grouptab[hashval] -> prventr = pa;
		}
	pa -> prventr = 0;
	grouptab[hashval] = pa;

	pa -> posting_allowed_flag = posting_allowed_flag;
	pa -> subscribed_flag = subscribed_flag;
	pa -> article_ptr = article_ptr;
	pa -> group_status = group_status;
	pa -> new_articles = new_articles;
	pa -> filter_flags = filter_flags;
	pa -> max_headers = max_headers;
	pa -> mark_all_flag = 0;/* load_subscribed(merge) may change this */
	}/* end while all lines in file */
}/* end function load_groups */


int load_subscribed_groups(int mode)
{
int a;
FILE *subscribedgroupsfile;
char temp[TEMP_SIZE];/* lines in groups dat are of know size shorter then this */
char pathfilename[TEMP_SIZE];
char name[TEMP_SIZE];
char posting_allowed_flag;
int subscribed_flag;
long article_ptr;
int group_status;
long new_articles;
int max_headers;
int mark_all_flag;
int filter_flags;
 
struct group *pa;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "load_subscribed_groups(): arg mode=%d\n", mode);
	}

pa = 0;/* only for clean compilation with -Wall */

sprintf(pathfilename, "%s/.NewsFleX/%s/%s/subscribed_groups.dat",\
home_dir, news_server_name, database_name);
subscribedgroupsfile = fopen(pathfilename, "r");
if(! subscribedgroupsfile)
	{
	if(debug_flag) fprintf(stdout, "could not load file %s\n", pathfilename);
	return(0);
	} 
while(1)
	{
	a = readline(subscribedgroupsfile, temp);
	if(a == EOF)
		{
		free(g_newsserver);
		g_newsserver = strsave(news_server_name);
		if(! g_newsserver) break;

		free(g_databse);
		g_databse = strsave(database_name);
		if(! g_databse) break;

		return(1);
		}

	/* try to stay compatible if reading old format without mark all */
	mark_all_flag = 0;

	sscanf(temp,\
	"%s %c %d %ld  %d %ld %d %d  %d",\
	name, &posting_allowed_flag, &subscribed_flag, &article_ptr,\
	&group_status, &new_articles, &filter_flags, &max_headers,\
	&mark_all_flag);

	if(mode == MERGE_DATA) pa = lookup_group(name);
	if(mode == USE_ALL_DATA) pa = install_group(name);
	if(! pa)
		{
		if(mode == MERGE_DATA)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"load_subscribed_groups: cannot lookup group %s\n", temp);
				}
			continue;/* NewsFleX.help is not there likely */
			}
		if(mode == USE_ALL_DATA)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"load_subscribed_groups: cannot install group %s\n", temp);
				}
			return(0);/* problem memory alocation */
			}
		}
		
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_subscribed_groups(): pa=%lu article_ptr=%ld new_articles=%ld\n",\
		(long) pa, article_ptr, new_articles);
		}

	if(mode == MERGE_DATA)
		{
		pa -> article_ptr = article_ptr;
		pa -> new_articles = new_articles;
		pa -> subscribed_flag = 1;
		pa -> filter_flags = filter_flags;
		pa -> max_headers = max_headers;
		pa -> mark_all_flag = mark_all_flag;
		}/* end mode merge data */

	if(mode == USE_ALL_DATA)
		{
		pa -> posting_allowed_flag = posting_allowed_flag;
		pa -> group_status = group_status;

		pa -> article_ptr = article_ptr;
		pa -> new_articles = new_articles;
		pa -> subscribed_flag = subscribed_flag;
		pa -> filter_flags = filter_flags;
		pa -> max_headers = max_headers;
		pa -> mark_all_flag = mark_all_flag;
		}/* end mode use all data */
	}/* end while all lines in file */
return(0);/* only for gcc -Wall */
}/* end function load groups_subscribed_groups */


int save_groups()
{
int i;
FILE *allgroupsfile;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct group *pa, *pb;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "save_groups(): arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/%s/groups.tmp", home_dir, news_server_name);
allgroupsfile = fopen(pathfilename, "w");
if(! allgroupsfile)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	/* find end of chain of structures */
	pb = 0;
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)
		{
		pb = pa;
		}
	/* pb now points to last structure */
	pa = pb;
	while(1)
		{
		if(! pa) break;
		/* 
		Note: writing pa -> subscribed_flag = 0, do not want subscribed
		info in groups.dat, else on doing a show all groups you will be
		subscribed again to anything you had unsubscribed from within
		subscribed groups :)
		*/

		if(debug_flag)
			{
			fprintf(stdout, "save_groups(): pa->name=%s\n", pa -> name);
			}

		/*
		name, posting_allowed_flag,
		subscribed_flag, article_ptr, group_status,
		new_articles,  filter_flags, max_headers,
		*/
		fprintf(allgroupsfile,\
		"%s %c  %d %ld %d  %ld %d %d\n",
		pa -> name, pa -> posting_allowed_flag,\
		0, (long) 0, pa -> group_status,\
		(long) 0, 0, maximum_headers);

		pa = pa -> prventr;
		}/* end for all structures */
	}/* end for all entries in grouptab */

fclose(allgroupsfile);

/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/%s/groups.dat", home_dir, news_server_name);
sprintf(pathfilename2, "%s/.NewsFleX/%s/groups.dat~", home_dir, news_server_name);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_groups(): rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/%s/groups.tmp", home_dir, news_server_name);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_groups(): rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

save_subscribed_groups();

return(1);
}/* end function save_groups */


int save_subscribed_groups()
{
int i;
FILE *subscribedgroupsfile;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct group *pa, *pb;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout,\
	"save_subscribed_groups(): arg none\n\
	using g_newsserver=%s g_databse=%s\n",\
	g_newsserver, g_databse);
	}

/* argument check */
if(! g_newsserver)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_subscribed_groups(): g_newsserver is NULL cancel\n");
		}
	return(0);
	}
if(strlen(g_newsserver) == 0)
	{
	fl_show_alert(\
	"save_subscribed_groups(): g_newsserver is EMPTY STRING", "", "cancel",\
	0);
	return(0);
	}

if(! g_databse)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_subscribed_groups(): g_databse is NULL cancel\n");
		}
	return(0);
	}
if(strlen(g_databse) == 0)
	{
	fl_show_alert(\
	"save_subscribed_groups(): g_databse is EMPTY STRING", "", "cancel",\
	0);
	return(0);
	}

sprintf(pathfilename, "%s/.NewsFleX/%s/%s/subscribed_groups.tmp",\
home_dir, g_newsserver, g_databse);
subscribedgroupsfile = fopen(pathfilename, "w");
if(! subscribedgroupsfile)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	/* find end of chain of structures */
	pb = 0;
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)
		{
		pb = pa;
		}
	/* pb now points to last structure */
	pa = pb;
	while(1)
		{
		if(! pa) break;
		if(pa -> subscribed_flag)
			{
			fprintf(subscribedgroupsfile,\
			"%s %c  %d %ld %d  %ld %d %d  %d\n",
			pa -> name, pa -> posting_allowed_flag,\
			pa -> subscribed_flag, pa -> article_ptr, pa -> group_status,\
			pa -> new_articles,  pa -> filter_flags, pa -> max_headers,\
			pa -> mark_all_flag);
			}
		pa = pa -> prventr;
		}/* end for all structures */
	}/* end for all entries in grouptab */

fclose(subscribedgroupsfile);

/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/%s/%s/subscribed_groups.dat",\
home_dir, g_newsserver, g_databse);
sprintf(pathfilename2, "%s/.NewsFleX/%s/%s/subscribed_groups.dat~",\
home_dir, g_newsserver, g_databse);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_subscribed_groups(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/%s/%s/subscribed_groups.tmp",\
home_dir, g_newsserver, g_databse);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_subscribed_groups(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);
}/* end function save_subscribed_groups */


int subscribe_group(char *group, int subscribe)
{
struct group *pa, *lookup_group();
char *strsave();
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "subscribe_group(): arg group=%s\n subscribe=%d",\
	group, subscribe);
	}

/* argument check */
if(! group) return(0);
if(strlen(group) == 0)
	{
	fl_show_alert("NO GROUP SPECIFIED", "command cancelled", "", 0);
	return(0);
	}

pa = lookup_group(group);
if(!pa) /* not found */
	{
	if(subscribe == 1)
		{
		fl_show_alert(\
		"CANNOT SUBSCRIBE TO GROUP:", group, "NOT IN ACTIVE FILE", 0);
		if(debug_flag)
			{
			fprintf(stdout, "\nCannot subscribe to group %s\n", group);
			}
		return(0);
		}
	if(subscribe == 0)
		{
		fl_show_alert(\
		"CANNOT UNSUBSCRIBE GROUP:", group, "NO SUCH GROUP", 0);
		if(debug_flag)
			{
			fprintf(stdout, "\nCannot unsubscribe group %s\n", group);
			}
		return(0);
		}/* end subscribe == 0 */
	}/* end ! pa */
	
if(subscribe && (pa -> subscribed_flag) )
	{
	fl_show_alert(\
	"ALREADY SUBSCRIBED TO:", group, "", 0);
	return(1);
	}
	
if(pa -> group_status == CANCELLED_GROUP)
	{
	fl_show_alert(\
	"WARNING: This group is cancelled", "not in active file on this server",\
	"online operation will fail", 0);
	}
	
if(subscribe == 0)
	{
	if(strcmp(group, "NewsFleX.help") == 0)
		{
		fl_show_alert("Group NewsFleX.help cannot be unsubscribed", "", "", 0);
		return(1);
		}

	/* ask for confirmation */
	sprintf(temp, "This will unsubscribe group\n%s\nare you sure?\n", group);
	if(!fl_show_question(temp, 0) ) /* mouse on no */
		{
		return(1);
		}
	}/* end if subscribe == 0 */

pa -> subscribed_flag = subscribe;
/* maybe leave this as it is */
pa -> new_articles = 0;
pa -> article_ptr = 0;
pa -> filter_flags = 0;
pa -> max_headers = maximum_headers;/* global default */
pa -> mark_all_flag = 0;/* the default */
return(1);
}/* end function subscribe_group */


int add_group(char *data)
{
struct group *install_group(), *pa;
char group[TEMP_SIZE];
char first_article[TEMP_SIZE];
char last_article[TEMP_SIZE];
char posting_allowed_flag[TEMP_SIZE];
long f_article;
long l_article;

if(debug_flag) fprintf(stdout, "add_group(): arg data=%s\n", data);

sscanf(data, "%s %s %s %s",\
group, last_article, first_article, posting_allowed_flag);

/*
NOTE: NewsFleX_cb.c has marked all groups CANCELLED_GROUP
in response to LIST
*/

/* test if this is an existing group */
pa = lookup_group(group);
if(pa) /* existing group */
	{
	pa -> posting_allowed_flag = posting_allowed_flag[0];
	pa -> group_status = ACTIVE_GROUP;
	/* last article and first article are not used */
	return(1);
	}

/* new group */
pa = install_group(group);
if(! pa)
	{
	if(debug_flag)fprintf(stdout, "add_group: cannot install group\n");
	return(0);
	}

f_article = atol(first_article);
l_article = atol(last_article);

pa -> posting_allowed_flag = posting_allowed_flag[0];
pa -> group_status = NEW_GROUP;
pa -> new_articles = l_article - f_article;
pa -> article_ptr = f_article;
pa -> subscribed_flag = 0;
pa -> filter_flags = 0;
pa -> max_headers = maximum_headers;/* global default */
pa -> mark_all_flag = 0;/* default */
/* So any group that was there, but not in this LIST, is now CANCELLED_GROUP */
return(1);
}/* end function add_group */


int show_groups_hash()
{
int i;
char temp[TEMP_SIZE];
struct group *pa;
extern char *strsave();
char formatstr[TEMP_SIZE];
int browser_topline;
int browser_last_selected_line;
char *ptra, *ptrb, *ptrc;
int match_failed_flag;
int last_string_flag;

if(debug_flag) fprintf(stdout, "show_groups_hash(): arg none\n");

/* remember whch line was double clicked */
browser_last_selected_line = fl_get_browser(fdui -> group_list_browser);

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> group_list_browser);

/* clear browser */
fl_clear_browser(fdui -> group_list_browser);

for(i = 0; i < GROUPS_HASH_SIZE; i++)
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		/* test if diplay allowed */
		if(filters_enabled_flag)
			{
			if(pa -> filter_flags & FILTER_DO_NOT_SHOW_GROUP) continue;
			}

		if(groups_display_filter[0])/* some filter */
			{
			/* test if filter name present in group name */
			/* for all entries in filter */
			ptra = groups_display_filter;
			ptrb = strsave(groups_display_filter);/* create space */
			match_failed_flag = 0;
			last_string_flag = 0;
			while(1)
				{
				ptrc = ptrb;/* point to start availeble space */
				while(1)/* string in filter to ptrb */
					{
					*ptrc = *ptra;
					if(! *ptra) break;/* end filter */
					if(*ptra == ' ')/* end of string in filter */
						{
						*ptrc = 0;
						ptra++;/* scip the space */
						break;
						}
					ptra++;
					ptrc++;
					}/* end while all characters in string */
				/* test if string present in name */
				if(! strstr(pa -> name, ptrb) )
					{
					match_failed_flag = 1;
					break;
					}
				if(! *ptra) break;/* end filter */
				}/* end while all strings in filter */	
			free(ptrb);/* release space */
			if(match_failed_flag) continue;
			}/* end if filter */
		if(groups_show_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
			{	
			if(pa -> subscribed_flag)
				{
				strcpy(formatstr, "@C0");/* black */
				if(pa -> group_status == CANCELLED_GROUP)
					{
					strcpy(formatstr, "@C2");/* green */
					}		

				sprintf(temp, "%s %s <=%d %ld",\
				formatstr, pa -> name, pa -> max_headers,\
				pa -> new_articles);
		
				if(pa -> mark_all_flag)
					{
					strcat(temp, " GET ALL"); 
					}

				if(pa -> posting_allowed_flag == 'n')
					{
					strcat(temp, " NO POSTING ALLOWED");
					}
				fl_add_browser_line(fdui -> group_list_browser, temp);
				}
			}/* end subscribed groups */
		if(groups_show_selection == SHOW_ONLY_NEW_GROUPS)
			{
			if(pa -> group_status == NEW_GROUP)
				{
				strcpy(formatstr, "@C1");/* red */
			
				if(pa -> subscribed_flag)
					{
					sprintf(temp, "%s %s <=%d %ld",\
					formatstr, pa -> name, pa -> max_headers,\
					pa -> new_articles);
					}
				else
					{
					sprintf(temp, "%s %s %ld",\
					formatstr, pa -> name, pa -> new_articles);
					}

				if(pa -> mark_all_flag)
					{
					strcat(temp, " GET ALL"); 
					}

				if(pa -> posting_allowed_flag == 'n')
					{
					strcat(temp, " NO POSTING ALLOWED");
					}
				fl_add_browser_line(fdui -> group_list_browser, temp);
				}
			}/* end new groups */
		if(groups_show_selection == SHOW_ALL_GROUPS)
			{
			strcpy(formatstr, "@C0");/* black */
			if(pa -> group_status == NEW_GROUP)
				{
				strcpy(formatstr, "@C1");/* red */
				}
			if(pa -> group_status == ACTIVE_GROUP)
				{
				strcpy(formatstr, "@C0");/* black */
				}
			if(pa -> group_status == CANCELLED_GROUP)
				{
				strcpy(formatstr, "@C2");/* green */
				}		
			if(pa -> subscribed_flag) strcpy(formatstr, "@C4");/* blue */
			
			if(pa -> subscribed_flag)
				{
				sprintf(temp, "%s %s <=%d %ld",\
				formatstr, pa -> name, pa -> max_headers,\
				pa -> new_articles);
				}
			else
				{
				sprintf(temp, "%s %s %ld",\
				formatstr, pa -> name, pa -> new_articles);
				}

			if(pa -> mark_all_flag)
				{
				strcat(temp, " GET ALL"); 
				}

			if(pa -> posting_allowed_flag == 'n')
				{
				strcat(temp, " NO POSTING ALLOWED");
				}
			fl_add_browser_line(fdui -> group_list_browser, temp);
			}/* end all groups */
		}/* end for all structures at this location in hash table */
	}/* end for all entries in hash table */

/* restore double_clicked_line */
fl_select_browser_line(fdui -> group_list_browser,\
browser_last_selected_line);

/* readjust the vertical slider position to the old situation */
fl_set_browser_topline(fdui -> group_list_browser, browser_topline);

to_command_status_display("");

/*unread_article_at_top_flag = 1*/

return(1);
}/* end function show_groups_hash */


int show_groups()
{
if(debug_flag)
	{
	fprintf(stdout, "show_groups(): arg none\n");
	}
if(! sort_flag) show_groups_hash();
else show_groups_tree();
return(1);
}/* end function show_groups */


int add_new_headers_in_subscribed_groups_to_command_queue()
{
struct group *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_new_headers_in_subscribed_groups_to_command_queue(): arg none\n");
	}

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(pa -> subscribed_flag)
			{
			add_to_command_queue(pa -> name, 0, GET_NEW_HEADERS_IN_GROUP);
			}
		}/* end for all structures */
	}/* end for all entries in grouptab */
return(1);
}/* end function add_new_headers_in_subscribed_groups_to_command_queue */


int add_marked_article_bodies_in_subscribed_groups_to_command_queue()
{
struct group *pa;
int i;
int some_marked_article_body;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_marked_article_bodies_in_subscribed_groups_to_command_queue():\
	arg none\n");
	}

if(request_by_message_id_flag)
	{
	clear_article_body_list();
	clear_article_body2_list();
	}

/* tell user what is happening */
fl_set_object_label(fdui -> command_status_display,\
"Searching for marked articles");
XSync(fl_get_display(), 0);

some_marked_article_body = 0;
/* for all subscribed groups */
for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(pa -> subscribed_flag)
			{
			add_marked_article_bodies_in_group_to_command_queue(\
			pa -> name, &some_marked_article_body);			
			}		
		}/* end for all structures */
	}/* end for all entries in grouptab */

if(request_by_message_id_flag)
	{
	if(some_marked_article_body)
		{
		add_to_command_queue("dummy", 0, GET_ARTICLE_BODY);
		}
	}

if(! some_marked_article_body)
	{
	/* tell user there are no marked article bodies */
	fl_set_object_label(fdui -> command_status_display,\
	"No marked article bodies found");
	}
else
	{
	/* clear command status display */
	fl_set_object_label(fdui -> command_status_display, "");
	}
XSync(fl_get_display(), 0);

return(1);
}/* end  add_marked_article_bodies_in_subscribed_groups_to_command_queue */


int subscribed_to_group(char *group)
{
struct group *pa, *lookup_group();

if(debug_flag)
	{
	fprintf(stdout, "subscribed_to_group(): arg group=%s\n", group);
	}

pa = lookup_group(group);
if(! pa) return(0);

if(! pa -> subscribed_flag) return(0);
return(1);
}


int delete_all_not_locked_articles_in_subscribed_groups()
{
struct group *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout,\
	"delete_all_not_locked_articles_in_subscribed_groups() arg none\n");
	}

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(! pa -> subscribed_flag) continue;

		delete_all_not_locked_articles(pa -> name);
		}/* end for all structures */
	}/* end for all entries in grouptab */

/*
we need to select a valid group and article, or next unread button will hang if pressed
immediatly after this function.
*/
select_first_group_and_article();

return(1);
}/* end function delete_all_not_locked_articles_in_subscribed_groups */


char *get_next_subscribed_group(char *group)
{
struct group *pa, *pstart, *lookup_group();
int hashval;
int i;

if(debug_flag)
	{
	fprintf(stdout, "get_next_subscribed_group(): arg group=%s\n", group);
	}

/* parameter check */
if(! group) return(0);

pa = lookup_group(group);
if(! pa) return(0); /* group does not exists */

hashval = hash(pa -> name);

/* set start search for subscribed groups */
pa = pa -> nxtentr;/* point to next group */
if(! pa) /* last one in the chain, or empty chain */
	{
	hashval++; /* point tot next postion in grouptab, or start if last */
	if(hashval >= GROUPS_HASH_SIZE) hashval = 0;
	pstart = grouptab[hashval];
/*fprintf(stdout, "!pa hashval=%d pstart=%ld\n", hashval, pstart);*/
	}
else
	{
/*fprintf(stdout, "pa hashval=%d pstart=%ld\n", hashval, pstart);*/
	pstart = pa;
	}
	
/* search from here to end of this chain */
for(pa = pstart; pa != 0; pa = pa -> nxtentr)
	{
	if(pa -> subscribed_flag)
		{
/*fprintf(stdout, "found in this chain name=%s\n", pa -> name);*/
		return(pa -> name);
		}
	}/* end for */
hashval ++;
if(hashval >= GROUPS_HASH_SIZE) hashval = 0;

/*fprintf(stdout, "moving to i = hashval=%d\n", hashval);*/

/* search from start next chain to end of chains */	
for(i = hashval; i < GROUPS_HASH_SIZE; i++)
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)
		{
		if(pa -> subscribed_flag)
			{
/*fprintf(stdout, "found in second half name=%s\n", pa -> name);*/
			return(pa -> name);
			}
		}/* end for */
	}/* end for all entries in grouptab */
return(0);/* not found */
}/* end function get_next_subscribed_group */


int completely_delete_group(char *group)
{
struct group *pa, *lookup_group();
char temp[TEMP_SIZE];
FILE *exec_filefd;

if(debug_flag)
	{
	fprintf(stdout, "completely_delete_group(): arg group=%s\n", group);
	}

/* argument check */
if(! group) return(0);

if(strcmp(group, "NewsFleX.help") == 0)
	{
	fl_show_alert("HELP CANNOT BE DELETED", "NOTHING DELETED", "", 0);
	return(0);
	}

if(strlen(group) == 0)
	{
	fl_show_alert("NO GROUP SPECIFIED", "NOTHING DELETED", "", 0);
	return(0);
	}

/* test if group exists */
pa = lookup_group(group);
if(! pa)
	{
	fl_show_alert("NO SUCH GROUP:", group, "NOTHING DELETED", 0);
	return(0);
	}

/* ask for confirmation */
sprintf(temp, "This will delete group\n%s\nare you sure?\n", group);
if(!fl_show_question(temp, 0) ) /* mouse on no */
	{
	return(1);
	}

/* unsubscribe */
pa -> subscribed_flag = 0;

/* I know about rm -rf, but just being carefull */

/* delete all files in group directory */
sprintf(temp, "rm %s/.NewsFleX/%s/%s/%s/*",\
home_dir, news_server_name, database_name, group);
/*
yes I deleted my setup file and all posts,
and the groups.dat (loosing all subscribed groups)
*/
if(strstr(temp, "//") != 0) return(0);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);
				
/* remove group directory */
sprintf(temp, "rmdir %s/.NewsFleX/%s/%s/%s",\
home_dir, news_server_name, database_name, group);
if(strstr(temp, "//") != 0) return(0);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/*
Signal to save_articles() that failure in writing back to this group is OK.
(Do not show error report to user).
*/

sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.tmp",\
home_dir, news_server_name, database_name, group);
free(last_deleted_group);
last_deleted_group = strsave(temp);

/* flush */
select_first_group_and_article();

return(1);
}/* end function completely_delete_group */


int set_all_filter_flags_in_articles()
{
struct group *pa;
int i;
/*char temp[TEMP_SIZE];*/

if(debug_flag)
	{
	fprintf(stdout,\
	"set_all_filter_flags_in_articless() arg none\n");
	}

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(! pa -> subscribed_flag) continue;
		
		/* tell the user what is happening */
/*
		sprintf(temp, "Setting filter flags for group %s", pa -> name);
		fl_set_object_label(fdui -> command_status_display, temp);
		XSync(fl_get_display(), 0);
*/		
		set_all_filter_flags_in_group(pa -> name);
		}/* end for all structures */
	}/* end for all entries in grouptab */

/* clear the command status display */
/*
fl_set_object_label(fdui -> command_status_display, "");
XSync(fl_get_display(), 0);
*/		
return(1);
}/* end function set_all_filter_flags_in_articles */


int apply_filters_to_group(char *group)
{
struct group *pa, *lookup_group();

if(debug_flag)
	{
	fprintf(stdout,\
	"apply_filters_to_group(): arg group=%s\n", group);
	}

/* argument check */
if(! group) return(0);

pa = lookup_group(group);
if(! pa) return(0);

get_filter_flags(\
group, "dummy", 0, "dummy", 0, SEARCH_GROUPS, &(pa -> filter_flags) );

return(1);
}/* end function apply_filters_to_group_header */


int set_all_filter_flags_in_groups()
{
struct group *pa;
int filter_flags;
int i;

if(debug_flag)
	{
	fprintf(stdout, "set_all_filter_flags_in_groups(): arg none\n");
	}
	
/* first test if we really HAVE to load all those groups..... */
for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)
		{
		/* get the filter result */
		filter_flags = 0;/* the default */
		get_filter_flags(\
		pa -> name, "dummy" , 0, "dummy", 0, SEARCH_GROUPS, &filter_flags);
		if(filter_flags) break;
		}/* end for all structures */
	}/* end for all entries in grouptab */
if(! filter_flags) return(1);

load_groups();

fl_set_object_label(fdui -> command_status_display,\
"FILTERS: UPDATING DATABASE, THIS MAY TAKE A WHILE");
XSync(fl_get_display(), 0);

for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)
		{
		/* get the filter result */
		pa -> filter_flags = 0;/* the default */

		get_filter_flags(\
		pa -> name, "dummy" , 0, "dummy", 0, SEARCH_GROUPS,\
		&(pa -> filter_flags) );
		}/* end for all structures */
	}/* end for all entries in grouptab */

save_groups();
return(1);
}/* end function set_all_filter_flags_in_groups */


int regenerate_subscribed_groups(char *newsserver, char *period)
{
char temp[TEMP_SIZE];
FILE *fileptr;
DIR *dirptr, *gdirptr;
struct dirent *eptr, *geptr;
char *ptr;
long article, last_article_present;

if(debug_flag)
	{
	fprintf(stdout,\
	"regenerate_subscribed_groups(): arg newsserver=%s period=%s\n",\
	newsserver, period);
	}

/* ask user confirmation */
/* mouse on no */
if(!fl_show_question("Overwrite your subscribed_groups.dat?", 0) )
	{
	return(0);
	}

/* user sanity test */
ptr = (char *) fl_show_input("Type: yes I want this", "No");
if(! ptr) return(0);
if(strcmp(ptr, "yes I want this") != 0)
	{
	fl_show_alert("Command cancelled", "", "", 0);
	return(0);
	}

/* argument check */
if(! newsserver) return(0);
if(strlen(newsserver) == 0) return(0);
if(strstr(newsserver, "/") != 0) return(0);
if(strstr(newsserver, " ") != 0) return(0);
if(! period) return(0);
if(strlen(period) == 0) return(0);
if(strstr(period, "/") != 0) return(0);
if(strstr(period, " ") != 0) return(0);

/*
struct group
	{
	char *name;
	char posting_allowed_flag;
	int subscribed_flag;
	long article_ptr;
	int group_status;
	long new_articles;
	int filter_flags;
	struct group *nxtentr;
	struct group *prventr;
	};
struct group *grouptab[GROUPS_HASH_SIZE];
*/

/* move any subscribed groups.dat to subscribed_groups.dat.org */
sprintf(temp,\
"mv %s/.NewsFleX/%s/%s/subscribed_groups.dat \
%s/.NewsFleX/%s/%s/subscribed_groups.dat.org",\
home_dir, newsserver, period,\
home_dir, newsserver, period);
fileptr = popen(temp, "r");
pclose(fileptr);

/* create a new empty subscribed_groups.dat */
sprintf(temp,\
"%s/.NewsFleX/%s/%s/subscribed_groups.dat",\
home_dir, newsserver, period);
fileptr = fopen(temp, "w");
fclose(fileptr);

/*
load the group list (active list),
this will clear the structure first and load the empty subscribed_groups.dat.
*/
/* force reload */
groups_loaded_flag = 0;
load_groups();

/* read directory entries for this newsserver and period */
sprintf(temp,\
"%s/.NewsFleX/%s/%s/",\
home_dir, newsserver, period);
dirptr = opendir(temp);
if(! dirptr)
	{
	fl_show_alert(\
	"No such group directory", temp, "command cancelled", 0);
	return(0);
	}
 
to_command_status_display(\
"regenerating subscribed_groups.dat, this may take a while");

while(1)
	{
	eptr = readdir(dirptr);
	if(!eptr) break;
	if(debug_flag)
		{
		fprintf(stdout,\
		"regenerate_subscribed_groups():\n\
		period directory scan %s\n",\
		eptr -> d_name);
		}

	/* some things we do not want to create an entry for */
	if(strcmp(eptr -> d_name, ".") == 0) continue;
	if(strcmp(eptr -> d_name, "..") == 0) continue;
	if(strcmp(eptr -> d_name, "subscribed_groups.dat") == 0) continue;
	if(strcmp(eptr -> d_name, "subscribed_groups.dat~") == 0) continue;
	if(strcmp(eptr -> d_name, "subscribed_groups.dat.org") == 0) continue;
	
	/* now it must be a group directory */
	if(! subscribe_group(eptr -> d_name, 1) )
		{
		fl_show_alert(\
		"regenerate_subscribed_groups()",\
		eptr -> d_name,\
		"cannot subscribe, trying next", 0);		
		continue;
		}
	
	/*
	set the article pointer to point to the last article present in this
	group.
	*/
	/* enter the group directory */
	/* read directory entries for this newsserver, period, group*/
	sprintf(temp,\
	"%s/.NewsFleX/%s/%s/%s",\
	home_dir, newsserver, period, eptr -> d_name);
	gdirptr = opendir(temp);
	if(! gdirptr) continue; /* subscribe_group did set article ptr to 0 */
	last_article_present = 0;
	while(1)
		{
		geptr = readdir(gdirptr);
		if(! geptr) break;
		if(debug_flag)
			{
			fprintf(stdout, "regenerate_subscribed_groups():\n\
			newsgroup directory scan %s\n",\
			geptr -> d_name);
			}
		/* test for head.xxxx */
		if(strstr(geptr -> d_name, "head.") == geptr -> d_name)
			{
			sscanf(geptr -> d_name, "head.%ld", &article);
			if(article > last_article_present)
				{
				last_article_present = article;
				}
			}/* end if entry is head.xxx */
		}/* end while all directory entries in news group */ 
	closedir(gdirptr);
	
	set_article_ptr(eptr -> d_name, last_article_present + 1);
	}/* end while all directory entries */
closedir(dirptr);

/* structure was modified */
save_subscribed_groups();

to_command_status_display("");

select_first_group_and_article();

groups_menu_selection = SHOW_ONLY_SUBSCRIBED_GROUPS;

/* force the gorup_list_button_cb to do something */
visible_browser = ARTICLE_LIST_BROWSER;
 
group_list_button_cb(fdui -> group_list_button, 0);

return(1);
}/* end function regenerate_subscribed_groups */


int select_first_group_and_article(int *article)
{
int i;
struct group *pa;
long first_article;
char *first_group;
int first_group_flag;

if(debug_flag)
	{
	fprintf(stdout, "select_first_group_and_article(): arg none\n");
	}

first_group = 0;/* keep gcc -Wall happy */
first_group_flag = 1;
/* for all structures at this position */
for(i = 0; i < GROUPS_HASH_SIZE; i++)
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(pa -> subscribed_flag)
			{
			if(first_group_flag)
				{
				first_group = pa -> name;
				first_group_flag = 0;

				if(debug_flag)
					{
					fprintf(stdout,\
					"select_first_group_and_article(): first_group=%s\n",\
					first_group);
					}

				}
			/* test if any article in this group */
			if(get_first_article_in_group(pa -> name, &first_article) )
				{
				if(first_article >= 0)
					{
					free(selected_group);
					selected_group = strsave(pa -> name);
					selected_article = first_article;

					if(debug_flag)
						{
						fprintf(stdout,\
						"select_first_group_and_article(): full match\n\
						selected_group=%s selected_article=%ld\n",\
						selected_group, selected_article);
						}

					return(1);
					}/* end if an article in this group */
				}/* end if get_first_article_in_group() returns OK */
			}/* end for all structures */
		}/* end for all structures in the chain */
	}/* end for all entries in grouptab */

/* all groups were empty */
if(! first_group_flag)
	{
	free(selected_group);
	selected_group = strsave(first_group);
	if(! selected_group) return(0);
	selected_article = -1;

	if(debug_flag)
		{
		fprintf(stdout,\
		"select_first_group_and_article(): only empty groups\n\
		selected_group=%s selected_article=%ld\n",\
		selected_group, selected_article);
		}

	return(1);
	}

/* no subscribed group found */
free(selected_group);
selected_group = strsave("no.subscribed.groups");
selected_article = -1;

if(debug_flag)
	{
	fprintf(stdout,\
	"select_first_group_and_article(): no subscribed groups found\n\
	selected_group=%s selected_article=%ld\n",\
	selected_group, selected_article);
	}

return(1);
}/* end function select_first_group_and_article */


int get_maximum_headers(char *group, int *m_headers)
{
struct group *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_maximum_headers(): arg group=%s\n", group);
	}
	
/* argument check */
if(! group) return(0);

pa = lookup_group(group);
if(! pa) return(0);

*m_headers = pa -> max_headers;

return(1);
}/* end function get_maximum_headers */


int set_maximum_headers(char *group, int m_headers)
{
struct group *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_maximum_headers(): arg group=%s\n", group);
	}

pa = lookup_group(group);
if(! pa) return(0);

if(! pa -> subscribed_flag) return(0);

pa -> max_headers = m_headers;

save_subscribed_groups();

return(1);
}/* end function set_maximum_headers */


int set_mark_all_flag(char *group, int state)
{
struct group *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_mark_all_flag(): arg group=%s state=%d\n",\
	group, state);
	}

pa = lookup_group(group);
if(! pa) return(0);

pa -> mark_all_flag = state;

save_subscribed_groups();

return(1);
}/* end function set_mark_all_flag */


int mark_all_bodies_in_get_always_all_bodies_groups()
{
int i;
struct group *pa;
int some_group_modified_flag;

if(debug_flag)
	{
	fprintf(stdout,\
	"mark_all_bodies_in_get_always_all_bodies_groups(): arg none\n");
	}

some_group_modified_flag = 0;
for(i = 0; i < GROUPS_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = grouptab[i]; pa != 0; pa = pa -> nxtentr)	
		{
		if(pa -> subscribed_flag)
			{
			if(pa -> group_status != CANCELLED_GROUP)
				{
				if(pa -> mark_all_flag)
					{
					add_new_headers_from_list(pa -> name);
					mark_all_articles_for_retrieval_in_group(\
					pa -> name, &some_group_modified_flag);
					}/* end if mark_all_flag */
				}/* end if not cancelled */
			}/* end if subscribed */		
		}/* end for all structures */
	}/* end for */

return(some_group_modified_flag);
}/* end function mark_all_bodies_in_get_always_all_bodies_groups */

