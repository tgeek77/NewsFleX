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


char *a_newsserver;
char *a_database;
char *a_group;
char *last_group;
char *last_newsserver;
char *last_database;
char *last_group_shown;

struct article
	{
	char *name;/* this is the article id long decimal */
	char *subject;
	char *from;
	char *content_type;
	char *references;
	char *message_id;
	char *attachment_name;
	char *date;
	int retrieval_flag;	
	int have_body_flag;
	int follow_flag;
	int browser_line;/* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
	int read_flag;
	int lock_flag;
	int encoding;
	long lines;
	int not_availeble_flag;
	int summary_flag;
	int filter_flags;
	struct article *nxtentr;
	struct article *prventr;
	}; 
struct article *articletab[2]; /* first element points to first entry,
									second element to last entry */

int reset_articles()
{
free(a_newsserver);
a_newsserver = 0;
free(a_database);
a_database = 0;
free(a_group);
a_group = 0;
free(last_group);
last_group = 0;
free(last_newsserver);
last_newsserver = 0;
free(last_database);
last_database = 0;
free(last_group_shown);
last_group_shown = 0;

/* clear the article list, nothing selected yet */
fl_clear_browser(fdui -> article_list_browser);

delete_all_articles();

/* clear any visible text in the article body */
fl_set_input(fdui -> article_body_input_field, "");

return(1);
}/* end function fresh_articles_start */


struct article *lookup_article(char *name)
{
struct article *pa;

for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}

return(0);/*not found*/
}/* end function lookup_article */


struct article *install_article_at_end_of_list(char *name)
{
struct article *plast, *pnew;
struct article *lookup_article();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_article_at_end_of_list(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pnew = lookup_article(name);
if(pnew)
	{
	/* free previous definition */
	free(pnew -> subject);
	free(pnew -> from);
	free(pnew -> content_type);
	free(pnew -> references);
	free(pnew -> message_id);
	free(pnew -> attachment_name);
	free(pnew -> date);
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct article *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = articletab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previous structure pointers */
if( !articletab[0] ) articletab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
articletab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_article_at_end_of_list */


struct article *install_article_in_position(\
char *name, char *subject, char *references, int *follow)
{
int i;
int no_match;
struct article *pa, *pprev, *plast, *plastmatch, *pnew, *pnext;
struct article *lookup_article();
char *test_string;
char *ptr1, *ptr2;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_article_in_position():\nname=%s\nsubject=%s\nreferences=%s\n",\
	name, subject, references);
	}
	
/* argument check */
if(! name) return(0);
if(! subject) return(0);
if(! references) return(0);

*follow = 0;
plastmatch = 0;/* for gcc -Wall */

/* ********  ********* ******* ***** *** ** *
I have tried to optimize this routine for speed, now only 3 searches left.
It consumes a lot of time on a fast 486 DX 2 66 compared to the 14k4
about 2.8 Kb /second download.
*/

/* search 1 */
/*
Test if article number already exists, if so, overwrite :(
Note that at this point the old header file will have been overwritten anyway.
*/
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0)
		{
		/* alert user? maybe on wrong news server */
		/* commented out, this will hang an auto connect */
/*
		fl_show_alert(\
		group, name, "already exists and overwritten, wrong news server?",\
		0);
*/
		*follow = pa -> follow_flag;
		return(pa); /* already there */
		}
	}

/* search 2 */
/* follow up to something */
if(strstr(subject, "Re:") == subject)
	{
	/* 
	Test for thread: 
	For a match, the reference line must be present in the new one,
	and the message_id must be the same as the last field in the new
	reference line.
	example:
	new reference line <flip> <flap> <flop>			references
	old reference line <flip> <flap>				pa -> references
	old message id <flop>							pa -> message_id
	add old msg id to old ref and do a strcmp().
	*/
	plastmatch = 0;/* no match yet */
	for(pa = articletab[1]; pa != 0; pa = pa -> prventr)
		{
		/* speed we need */
		/*
		testing first few characters prevents us from a lot of mallocs
		and sprintf and compares.
		*/
		no_match = 0;
		/* 10 is just a guess, to long and we lose because we do 2 compares */
		for(i = 0; i < 10; i++)
			{
			if(references[i] != (pa -> references[i]) )
				{
				no_match = 1;
				break;
				}
			if(!references[i]) break;/* end of string, both 0 */
			}			
		if(no_match) continue;

		test_string =\
		(char *)malloc(strlen(pa -> references) +\
		strlen(" ") +\
		strlen(pa -> message_id) + 1);
		sprintf(test_string, "%s %s", pa -> references, pa -> message_id);

		if(strcmp(test_string, references) == 0)/* match */
			{
			free(test_string);
			*follow = pa -> follow_flag;
			plastmatch = pa;
			break;
			}

		free(test_string);
		}/* end for all articles */
	}/* end if a followup */
	
if(debug_flag)
	{
	if(plastmatch)
		{
		fprintf(stdout,\
		"reference found=%s *follow=%d\n", pa -> subject, *follow);
		}
	}

/* search 3 */
/*
If we did not find a references match, try to put it with same subject.
*/
if(! plastmatch)
	{
	/* the original may come later */
	/* strip any "Re: " from subject */
	if(strstr(subject, "Re: ") == subject)
		{
		ptr1 = subject + 4;
		}
	else ptr1 = subject;
		
	/* look from end to start for the first article with this subject */
	for(pa = articletab[1]; pa != 0; pa = pa -> prventr)
		{
		/* strip any "Re: " */
		ptr2 = pa -> subject;
		if(pa -> subject[0] == 'R')
			{
			if(pa -> subject[1] == 'e')
				{
				if(pa -> subject[2] == ':')  ptr2 = pa -> subject + 4;
				}
			}

		if(strcmp(ptr1, ptr2) == 0)
			{
			*follow = pa -> follow_flag;
			plastmatch = pa;
			if(debug_flag)
				{
				fprintf(stdout,\
				"subject found=%s *follow=%d\n", pa -> subject, *follow);
				}
			break;
			}
		}/* end for all articles */
	}/* end if ! plastmatch */

/* plastmatch now points to insert point, or zero if not found, */

/* create new structure */
pnew = (struct article *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* add to end list if no reference match and no subject match */
if(! plastmatch)
	{
	*follow = 0;
	/* get previous structure */
	plast = articletab[1]; /* end list */

	/* set new structure pointers */
	pnew -> nxtentr = 0; /* new points top zero (is end) */
	pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

	/* set previous structure pointers */
	if( !articletab[0] ) articletab[0] = pnew; /* first element in list */
	else plast -> nxtentr = pnew;

	/* set array end pointer */
	articletab[1] = pnew;

	/* return pointer to new structure */
	return(pnew);
	}/* end if ! plastmatch */

/* plastmatch now holds insert position, insert article */
/* insert the structure AFTER plastmatch */
		
/* get previous and next structure */
pnext = plastmatch -> nxtentr;
pprev = plastmatch;

/* set pointers for new struture */
pnew -> nxtentr = pnext;
pnew -> prventr = pprev;

/* set pointers for next structure */
if(pnext == 0) articletab[1] = pnew;
else pnext -> prventr = pnew;		

/* set pointer for previous structure */
if(pprev == 0) articletab[0] = pnew;
else pprev -> nxtentr = pnew;
		
/* return pointer to new structure */
return(pnew);
}/* end function install_article_in_position */


int delete_article(char *name)/* delete entry from double linked list */
{
struct article *pa, *pprev, *pdel, *pnext;

if(debug_flag) fprintf(stdout, "delete_article(): arg name=%s\n", name);

/* parameter check */
if(! name) return(0);

pa = articletab[0];
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
	/* if first one, modify articletab[0] */
	if(pprev == 0) articletab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify articletab[1] */
	if(pnext == 0) articletab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel -> subject);
	free(pdel -> from);
	free(pdel -> content_type);
	free(pdel -> references);
	free(pdel -> message_id);		
	free(pdel -> attachment_name);
	free(pdel -> date);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_article */


int delete_all_articles()/* delete all entries from table */
{
struct article *pa;

if(debug_flag) fprintf(stdout, "delete_all_articles() arg none\n");

while(1)
	{	
	pa = articletab[0];
	if(! pa) break;
	articletab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa -> subject);
	free(pa -> from);
	free(pa -> content_type);
	free(pa -> references);
	free(pa -> message_id);
	free(pa -> attachment_name);			
	free(pa -> date);
	free(pa);/* free structure */
	}/* end while all structures */
articletab[1] = 0;
if(debug_flag) fprintf(stdout, "returning from delete_all_articles\n");
return(1);
}/* end function delete_all_articles */


int load_articles(char *group)
{
int a;
FILE *articles_dat_file;
char temp[READSIZE];
char pathfilename[TEMP_SIZE];
char name[TEMP_SIZE];
int retrieval_flag;
int have_body_flag;
int follow_flag;
int read_flag;
int lock_flag;
int encoding;
long lines;
int not_availeble_flag;
int summary_flag;
int filter_flags;
int browser_line;
struct article *pa;
char *subject;
char *from;
char *references;
char *content_type;
char *message_id;
char *attachment_name;
char *date;
int first_article_flag;

if(debug_flag)
	{
	fprintf(stdout,\
	"load_articles() arg group=%s using\n\
	last_newsserver=%s\n\
	last_database=%s\n\
	last_group=%s\n",\
	group, last_newsserver, last_database, last_group);
	}
	
/* argument check */
if(! group) return(0);

if(last_newsserver && last_database && last_group)
	{
	/* test if this group was already loaded, (structure may be modified) */
	if	(
		(strcmp(group, last_group) == 0) &&\
		(strcmp(news_server_name, last_newsserver) == 0) &&\
		(strcmp(database_name, last_database) == 0)
 		)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"load_articles(): already loaded, returning\n");
				}
			return(1);
			}
	}/* end if valid last_ */

/*
test if any group was loaded, if so save it,
structure may have been modified
*/
if(last_group) if(last_group[0] != 0) save_articles();

free(last_group);
last_group = strsave(group);
if(! last_group) return(0);
free(last_newsserver);
last_newsserver = strsave(news_server_name);
if(! last_newsserver) return(0);
free(last_database);
last_database = strsave(database_name);
if(! last_database) return(0);

delete_all_articles();

/*
group is now empty, do not write to directory on next occasion, if this
load fails for any reason.
*/
free(a_group);
a_group = strsave("");
if(! a_group) return(0);
/*
ok, so now it will go to ~/.NewsFleX/news_server_name/,
but save articles will prevent that.
*/
free(a_newsserver);
a_newsserver = strsave("");
if(! a_newsserver) return(0);

free(a_database);
a_database = strsave("");
if(! a_database) return(0);

sprintf(pathfilename, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
home_dir, news_server_name, database_name, group);
articles_dat_file = fopen(pathfilename, "r");
if(! articles_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not load file %s\n", pathfilename);
		}
	return(0);
	} 
first_article_flag = 1;
while(1)
	{
	/* 
	This should make it possible to read old formats, that do not have a
	filter_flags.
	*/
	filter_flags = 0;

	a = readline(articles_dat_file, temp);
	if(a == EOF)
		{
		free(a_group);
		a_group = strsave(group);
		if(! a_group) break;

		free(a_newsserver);
		a_newsserver = strsave(news_server_name);
		if(! a_newsserver) break;

		free(a_database);
		a_database = strsave(database_name);
		if(! a_database) break;

		if(debug_flag)
			{
			fprintf(stdout, "load_articles(): returning EOF\n\
			setting a_group=%s\n\
			a_newsserver=%s\n\
			a_database=%s\n",\
			a_group, a_newsserver, a_database);
			}

		return(1);
		}
	sscanf(temp, "%s %d %d %d %d %d %d %ld %d %d %d %d",\
	name, &retrieval_flag, &have_body_flag, &follow_flag, &read_flag,\
	&lock_flag, &encoding, &lines, &browser_line, &not_availeble_flag,\
	&summary_flag, &filter_flags);

	if(first_article_flag)
		{
		selected_article = atol(name);
		first_article_flag = 0;
		}

	/* Subject: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	subject = strsave(temp);
	if(! subject) break;

	/* From: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	from = strsave(temp);
	if(! from) break;;
	 
	/* Content-Type: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	content_type = strsave(temp);
	if(! content_type) break;

	/* References: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	references = strsave(temp);
	if(! references) break;

	/* Mesage-ID: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	message_id = strsave(temp);
	if(! message_id) break;

	/* Attachment_name: */
	a = readline(articles_dat_file, temp);
	if(a == EOF) break;/* file format error */
	attachment_name = strsave(temp);
	if(! attachment_name) break;

	/* Date: */
	a = readline(articles_dat_file, temp);
	date = strsave(temp);
	if(! date) break;

	/* create a structure entry */
	pa = install_article_at_end_of_list(name);
	if(! pa) break;
	
	/* data to structure */
	pa -> subject = subject;
	pa -> from = from;
	pa -> content_type = content_type;
	pa -> references = references;
	pa -> message_id = message_id;
	pa -> attachment_name = attachment_name;
	pa -> date = date;
	pa -> retrieval_flag = retrieval_flag;
	pa -> have_body_flag = have_body_flag;
	pa -> follow_flag = follow_flag;
	pa -> read_flag = read_flag;
	pa -> lock_flag = lock_flag;
	pa -> encoding = encoding;
	pa -> lines = lines;
	pa -> browser_line = browser_line;
	pa -> not_availeble_flag = not_availeble_flag;
	pa -> summary_flag = summary_flag;
	pa -> filter_flags = filter_flags;
	}/* end while all lines in articles.dat */
	
if(a == EOF)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_articles(): file format error group=%s article=%s\n",
		group, name);
		}
	}
else
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_articles(): memory allocation error group=%s article=%s\n",
		group, name);
		}
	}	
return(0);
}/* end function load articles */


int save_articles()
{
FILE *articles_dat_file;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct article *pa;
char temp[TEMP_SIZE];
DIR *dirptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"save_articles() arg none\n\
using a_newsserver=%s a_database=%s a_group=%s\n",\
	a_newsserver, a_database, a_group);
	}

/* argument check */
if(! a_group)
	{
	/*
	Note:
	This is the case at startup.
	*/
	if(debug_flag)
		{
		fprintf(stdout, "save_articles(): a_group is NULL cancel\n");
		}
	return(0);
	}
if(strlen(a_group) == 0)
	{
/*
	fl_show_alert(\
	"save_articles(): a_group is EMPTY STRING", "", "cancel", 0);
*/
	fl_set_object_label(fdui -> command_status_display,\
	"save_articles(): a_group is EMPTY string cancel");

	return(0);
	}

if(! a_newsserver)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_articles(): a_newsserver is NULL cancel\n");
		}
	return(0);
	}
if(strlen(a_newsserver) == 0)
	{
	fl_show_alert(\
	"save_articles(): a_newsserver is EMPTY STRING", "", "cancel",\
	0);
	return(0);
	}

if(! a_database)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_articles(): a_database is NULL cancel\n");
		}
	return(0);
	}
if(strlen(a_database) == 0)
	{
	fl_show_alert(\
	"save_articles(): a_database is EMPTY STRING", "", "cancel",\
	0);
	return(0);
	}

/* test if group was deleted, while in this group */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s",\
home_dir, a_newsserver, a_database, a_group);
dirptr = opendir(temp);
if(! dirptr)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_articles(): directory %s does not exist, cancel\n", temp);
		return(0);
		}
	}
closedir(dirptr);

/* save file as .tmp */
sprintf(pathfilename, "%s/.NewsFleX/%s/%s/%s/articles.tmp",\
home_dir, a_newsserver, a_database, a_group);
articles_dat_file = fopen(pathfilename, "w");
if(! articles_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_articles(): Cannot open %s for write, last_deleted_group=%s\n",\
		pathfilename, last_deleted_group);
		}

	/* this happens after a delete group, if the group was loaded */
	if(strcmp(pathfilename, last_deleted_group) == 0) return(0);/* ignore */
	
	fl_show_alert("Cannot open for write", pathfilename, "", 0);
	return(0);
	} 

for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(articles_dat_file, "%s %d %d %d %d %d %d %ld %d %d %d %d\n",\
	pa -> name, pa -> retrieval_flag, pa -> have_body_flag,\
	pa -> follow_flag, pa -> read_flag, pa -> lock_flag,\
	pa -> encoding, pa -> lines, pa -> browser_line,\
	pa -> not_availeble_flag, pa -> summary_flag, pa -> filter_flags);
	fprintf(articles_dat_file, "%s\n", pa -> subject);
	fprintf(articles_dat_file, "%s\n", pa -> from);
	fprintf(articles_dat_file, "%s\n", pa -> content_type);
	fprintf(articles_dat_file, "%s\n", pa -> references);
	fprintf(articles_dat_file, "%s\n", pa -> message_id);
	fprintf(articles_dat_file, "%s\n", pa -> attachment_name);
	fprintf(articles_dat_file, "%s\n", pa -> date);
	}/* end for all structures in the chain starting from articletab[0] */

fclose(articles_dat_file);

/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
home_dir, a_newsserver, a_database, a_group);
sprintf(pathfilename2, "%s/.NewsFleX/%s/%s/%s/articles.dat~",\
home_dir, a_newsserver, a_database, a_group);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_articles(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/%s/%s/%s/articles.tmp",\
home_dir, a_newsserver, a_database, a_group
);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_articles(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);/* ok */
}/* end function save_articles */


int add_article_header(char *group, long id, int *delete_flag)
{
int c, i;
struct article *install_article_in_position(), *pa;
char name[TEMP_SIZE];
FILE *header_file;
char path_filename[TEMP_SIZE];
char temp[READSIZE];
char *subject;
char *from;
char *content_type;
char *references;
char *message_id;
char *date;
long lines;
int summary_flag;
int follow;
int lf_flag;
int first_char;
int first_char_flag;
int folded_flag;
int message_id_present_flag;
long new_articles;

if(debug_flag)
	{
	fprintf(stdout, "add_article_header(): arg group=%s id=%ld\n",\
	group, id);
	}

/* argument check */
if(! group) return(0);
if(id < 0) return(0);

first_char = 0;/* only for clean compilation with -Wall */

/* fill the structure for this group */
load_articles(group);

sprintf(path_filename, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
home_dir, news_server_name, database_name, group, id);
header_file = fopen(path_filename, "r");
if(! header_file) return(0);
from = strsave("");
subject = strsave("");
content_type = strsave("");
references = strsave("");
message_id = strsave("");
date = strsave("");
lines = 0;
follow = 0;
summary_flag = 0;

/* flag used to indicate that we already have this message id */
*delete_flag = 0;

/* scan the headerfile */
temp[0] = 0;/* mark empty */
first_char_flag = 0;
while(1)/* all entries (unfolded header lines) in header file */
	{
	folded_flag = 0;	/* the function of this variable is to find out if
							the line was folded */
	i = 0;
	lf_flag = 0;
	while(1)/* all characters in a line */
		{
		if(first_char_flag)
			{
			temp[i] = first_char;
			i++;
			first_char_flag = 0;
			}

		while(1)/* error re read */
			{
			c = getc(header_file);
			if(! ferror(header_file) ) break;
			perror("add_article_header(): read failed ");
			}/* end while error re read */	

		if(feof(header_file) || (c == EOF) )
			{
			fclose(header_file);
			c = EOF;
			break;
			}
		
		if(lf_flag)
			{
			if( (c != ' ') && (c != 9) ) /* TAB = 9, no LWS */
				{
				/*
				line ends, save this character, it is the first one in the
				next line.
				*/
				first_char = c;
				first_char_flag = 1;
				break;
				}
			/* line continous */
			lf_flag = 0;
			folded_flag = 1;
			/* store char in line */
			}
			
		if(c == '\n')
			{
			lf_flag = 1;		
			continue;/* get next char, test for linear white space */
			}
			
		temp[i] = c;
		i++;
		if(i >= READSIZE - 1) break; /* prevent overflow */
		}/* end while all characters in a line */
	temp[i] = 0;/* string termination */

	/* all '\n' now removed!!!!, so one long line */
	if(strstr(temp, "From:") == temp)
		{
		free(from);
		from = strsave(strstr(temp, ":") + 2);
		}
	if(strstr(temp, "Subject:") == temp)
		{
		if(folded_flag)/* subject line was multiple line */
			{
			/* 
			use 1 as a flag for the display routine that it should
			read from the header file, instead of pa -> subject
			used by display_articles() and display_article_body()
			Note: cannot set flag in position 0, need to check for Re:
			later.
			*/
			strcat(temp, "\001");
			free(subject);
			subject = strsave(strstr(temp, ":") + 2);
			}
		else
			{
			free(subject);
			subject = strsave(strstr(temp, ":") + 2);
			}
		}
	if(strstr(temp, "Content-Type:") == temp)
		{
		free(content_type);
		content_type = strsave(strstr(temp, ":") + 2);
		}
	if(strstr(temp, "References:") == temp)
		{
		free(references);
		references = strsave(strstr(temp, ":") + 2);
		}
	if(strstr(temp, "Message-ID:") == temp)
		{
		free(message_id);
		message_id = strsave(strstr(temp, ":") + 2);
		}
	if(strstr(temp, "Date:") == temp)
		{
		free(date);
		date = strsave(strstr(temp, ":") + 2);
		}
	if(strstr(temp, "Lines:") == temp)
		{
		lines = atol( (strstr(temp, ":") + 2) );
		}
	if(strstr(temp, "Summary:") == temp)
		{
		summary_flag = 1;
		}
		
	if(c == EOF) break;
	if(feof(header_file) ) break;
	}/* end while all entries (unfolded header lines) in header file */

if(show_article_once_flag)
	{
	/* check if we already received this message_id (cross post?) */
	if(! add_to_message_id_list(message_id, &message_id_present_flag) )
		{
		if(debug_flag)
			{
			fprintf(stdout, "add_article_header(): failing on message_id\n");
			}
		/* error */
		return(0);
		}
	}
else
	{
	message_id_present_flag = 0;
	}
if(message_id_present_flag)/* already there */
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"add_article_header(): existing msg_id %s found, deleting %s %ld\n",\
		message_id, group, id);
		}

	/* if already there, erase header */
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
	home_dir, news_server_name, database_name, group, id);
	unlink(temp);
	
	/* remove from new articles list (used by filters) */
	remove_from_new_list(group, id);

	/*
	update subscribed_groups.dat for the correct number of new articles.
	*/
	get_new_articles(group, &new_articles);
	if(new_articles > 0) new_articles--;
	set_new_articles(group, new_articles);

	/*
	In case insert_headers_offline_flag, signal to
	add_new_headers_from_list() to delete new_headers structure entry.
	*/
	*delete_flag = 1;

	/* return OK */
	return(1);
	}/* end if message_id_present_flag */

/* install article header in structure */
sprintf(name, "%ld", id);
pa = install_article_in_position(name, subject, references, &follow);
if(! pa) return(0);

if(debug_flag)
	{
	fprintf(stdout, "add_article_header(): past install in position\n");
	}

/* set the follow_flag if set in the preceeding article */
if(follow)
	{
	pa -> follow_flag = follow;
	pa -> retrieval_flag = 1;
	
	/*
	this is a global, used by process_command_queue(),
	and if set will add articles marked for retrieval to the command
	queue, so these bodies will always be retrievd.
	*/
	new_thread_bodies_flag = 1;
	sprintf(temp, "SETTING new_thread_bodies_flag, pa->follow_flag=%d",
	pa -> follow_flag);
	fl_set_object_label(fdui -> command_status_display, temp);
	if(debug_flag) fprintf(stdout, "%s\n", temp);
	}
else
	{
	pa -> follow_flag = 0;
	pa -> retrieval_flag = 0;
	}

pa -> subject = subject;
pa -> from = from;
pa -> content_type = content_type;
pa -> references = references;
pa -> message_id = message_id;
pa -> date = date;
pa -> lines = lines;
pa -> summary_flag = summary_flag;

pa -> attachment_name = strsave("");/* not known yet at this time */
pa -> not_availeble_flag = 0; /* not availeble, not here, maybe not needed */ 
pa -> have_body_flag = 0;
pa -> read_flag = 0;
pa -> lock_flag = 0;
pa -> encoding = 0;
pa -> browser_line = 0;
pa -> filter_flags = FILTER_OFF;/* filter not applied yet */

/*
If save online, save structure every time, else only after the whole group
is done, and for the last group always.
*/
if(! insert_headers_offline_flag)
	{
	save_articles();
	}
	
if(debug_flag) fprintf(stdout, "RETURNING from add_article_header\n");

return(1);
}/* end function add_article_header */


int add_article_body(char *group, long article_id)
{
struct article *lookup_article(), *pa;
char name[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "add_article_body(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(name, "%ld", article_id);
pa = lookup_article(name);
if(! pa) return(0);/* no header for this body */

pa -> have_body_flag = 1;
pa -> retrieval_flag = 0;
pa -> not_availeble_flag = 0;

return(1);
}/* end function add_article_body */


int regenerate_mark_article(char *group, long article)
{
struct article *lookup_article(), *pa;
char name[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout,\
	"regenerate_mark_article(): arg group=%s article=%ld\n",\
	group, article);
	}

/* argument check */
if(! group) return(0);
if(article < 0) return(0);

sprintf(name, "%ld", article);
pa = lookup_article(name);
if(! pa) return(0);/* no entry for this body */

pa -> have_body_flag = 1;
pa -> retrieval_flag = 0;
pa -> not_availeble_flag = 0;
pa -> lock_flag = 1;
pa -> read_flag = 1;

return(1);
}/* end function regenerate_mark_article */


int show_articles(char *group)
{
int i;
char temp[READSIZE];
struct article *pa;
char formatstr[1024];
char infostr[1024];
int browser_line;
int browser_topline;
int browser_maxline;
int browser_last_selected_line;
char *ptr;
char *cptr;
int first_new_or_unread_article_line;

if(debug_flag)
	{
	fprintf(stdout, "show_articles(): arg group=%s\n", group);
	}

/* argument check */
if(! group) return(0);
if(strlen(group) == 0) return(0);

/* remember the last line selected */
browser_last_selected_line = fl_get_browser(fdui -> article_list_browser);

/* remember vertical scrollbar position */
browser_topline = fl_get_browser_topline(fdui -> article_list_browser);

/* first clear display in order not to confuse reader with old data */
fl_clear_browser(fdui -> article_list_browser);

load_articles(group);

/* if load failed (no articles.dat file yet), continue anyway */	
first_new_or_unread_article_line = 0;
browser_line = 1;
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	/* test if diplay allowed */
	if(filters_enabled_flag)
		{
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
		}
		
	/* want to show the first new or unread article at top of display */
	if(! first_new_or_unread_article_line)
		{
		if(! pa -> have_body_flag)
			{
			first_new_or_unread_article_line = browser_line;
			}
		}

	strcpy(formatstr, "@f");
	/* there are 8 possibilities if not read */
	if(pa -> read_flag)
		{
		strcat(formatstr, "@C0");/* black */
		}
	else if(!pa->retrieval_flag && !pa->have_body_flag && !pa->not_availeble_flag)
		{
		strcat(formatstr, "@C1");/* red is new unread header */
		}
	else if(!pa->retrieval_flag && !pa->have_body_flag && pa->not_availeble_flag)
		{
		strcat(formatstr, "@C1@b"); /* red bold */
		}
	else if(!pa->retrieval_flag && pa->have_body_flag && !pa->not_availeble_flag)
		{
		strcat(formatstr, "@C4"); /* blue */
		}
	else if(!pa->retrieval_flag && pa->have_body_flag && pa->not_availeble_flag)
		{
		strcat(formatstr, "@C4@b"); /* blue bold */
		}
	else if(pa->retrieval_flag && !pa->have_body_flag && !pa->not_availeble_flag)
		{
		strcat(formatstr, "@C5"); /* magenta */
		}
	else if(pa->retrieval_flag && !pa->have_body_flag && pa->not_availeble_flag)
		{
		strcat(formatstr, "@C18@b"); /* dark green bold */
		}
	else if(pa->retrieval_flag && pa->have_body_flag && !pa->not_availeble_flag)
		{
		strcat(formatstr, "@C21"); /* dark cyan */
		}
	else if(pa->retrieval_flag && pa->have_body_flag && pa->not_availeble_flag)
		{
		strcat(formatstr, "@C21@b"); /* dark cyan bold */
		}

	if(highlight_articles(group, pa) )
		{
		strcat(formatstr, "@_");/* for underscore */
		}

	if(pa -> follow_flag & FOLLOW_THIS_SUBJECT)
		{
		if(pa -> lock_flag) strcpy(infostr, "*LOCK=");
		else strcpy(infostr, "-----=");
		}
	else if(pa -> follow_flag & FOLLOW_THIS_THREAD)
		{
		if(pa -> lock_flag) strcpy(infostr, "*LOCK<");
		else strcpy(infostr, "-----<");
		}
	else
		{
		if(pa -> lock_flag) strcpy(infostr, "*LOCK*");
		else strcpy(infostr, "      ");
		}
	if(pa -> not_availeble_flag)
		{
		/*strcat(formatstr, "@C18");*/ /* dark green */
		/*strcat(formatstr, "@C19");*/ /* brown (gold brown) */
/*
		if(pa -> lock_flag) strcpy(infostr, "*N.A.*");
		else strcpy(infostr, "-N.A.-");
*/
		}
	/* test if followup to something */
	if(strstr(pa -> subject, "Re:") == pa -> subject)
		{
		strcat(infostr, "     ");/* identinate */
		/*
		in case of a thread, add as many spaces as there are entries
		in references.
		entries are embedded in '<xxx>', so counting '<'.
		*/
		for(cptr = pa -> references; *cptr != 0; cptr++)
			{
			if(*cptr == '<') strcat(infostr, " ");
			}
		}

	/*
	test if we have to get the subject from disk (in case formatted)
	or just use pa -> subject (not formatted, but faster).
	Note: add_article_header() sets pa -> subject to "use header\001" 
	if it is formatted (unfolded, LF removed).
	*/
	if(!strstr(pa -> subject, "\001" ) )
		{
		if(lines_first_flag)
			{
			sprintf(temp, "%s %s %ld %s %s %s (%s)",\
			formatstr, infostr, pa -> lines, pa -> subject, pa -> from,\
			pa -> date, pa -> name);
			}
		else
			{
			sprintf(temp, "%s %s %s %s %ld %s (%s)",\
			formatstr, infostr, pa -> subject, pa -> from, pa -> lines,\
			pa -> date, pa -> name);
			}

		fl_add_browser_line(fdui -> article_list_browser, temp);
		pa -> browser_line = browser_line;/* first one is 1 */
		browser_line++;
		}
	else /* a formatted subject, read from header file */
		{
		pa -> browser_line = browser_line;
		ptr = (char *) get_formatted_header_data(\
		group, atol(pa -> name), "Subject:");
		if(ptr)
			{
 			strcat(infostr, " ");
 			cptr = ptr;
			while(1)/* for all lines in the summary field */
				{
				sprintf(temp, "%s %s", formatstr, infostr);
				i = strlen(temp);
				while(1)/* extract a line */
					{
					if(*cptr == '\n') break;
					temp[i] = *cptr;
					if(! *cptr) break;
					i++;
					cptr++;
					}/* end while all characters in a line */
				temp[i] = 0;
				fl_add_browser_line(fdui -> article_list_browser, temp);
				browser_line ++;

				if(! *cptr) break;
				cptr++;
				}/* end while for all lines in summary */
			free(ptr);
			}/* end if subject read OK */

		if(lines_first_flag)
			{
			sprintf(temp, "%s %s %ld %s %s %s (%s)",\
			formatstr, infostr, pa -> lines, "", pa -> from,\
			pa -> date, pa  -> name);
			}
		else
			{
			sprintf(temp, "%s %s %s %s %ld %s (%s)",\
			formatstr, infostr, "", pa -> from, pa -> lines,\
			pa -> date, pa  -> name);
			}
		
		fl_add_browser_line(fdui -> article_list_browser, temp);
		browser_line++;

		}/* end else formatted subject */

	if(pa -> summary_flag)
		{
		if(debug_flag)
			{
			fprintf(stdout, "show_articles(): Processing summary\n");
			}
		/* if a summary field is present, show it */	
		ptr = (char *) get_formatted_header_data(\
		group, atol(pa -> name), "Summary:");
		if(ptr)
			{
			strcat(infostr, "  ");

			sprintf(temp, "%s %sSummary:", formatstr, infostr);
			fl_add_browser_line(fdui -> article_list_browser, temp);
			browser_line++;

 			cptr = ptr;
			while(1)/* for all lines in the summary field */
				{
				sprintf(temp, "%s %s", formatstr, infostr);
				i = strlen(temp);
				while(1)/* extract a line */
					{
					if(*cptr == '\n') break;
					temp[i] = *cptr;
					if(! *cptr) break;
					i++;
					cptr++;
					}/* end while all characters in a line */
				temp[i] = 0;
				fl_add_browser_line(fdui -> article_list_browser, temp);
				browser_line++;

				if(! *cptr) break;
				cptr++;
				}/* end while for all lines in summary */
			free(ptr);
			}/* end if summary read OK */
		}/* end if pa -> summary_flag */
	}/* end for all structures in the chain starting from articletab[0] */

/* select the line that was last double_clicked again */
if(last_group_shown)
	{
	if(strcmp(selected_group, last_group_shown) == 0)
		{
		fl_select_browser_line(fdui -> article_list_browser,\
		browser_last_selected_line);
		}
	else
		{
		free(last_group_shown);
		}
	}
last_group_shown = strsave(selected_group);

/* show first new or unread article at top display */
if(unread_article_at_top_flag)
	{
	if(first_new_or_unread_article_line)
		{
		fl_set_browser_topline(fdui -> article_list_browser,\
		first_new_or_unread_article_line);
		}
	else
		{
		/* show last article (bottom display) */
		browser_maxline = fl_get_browser_maxline(fdui -> article_list_browser);
		fl_set_browser_topline(fdui -> article_list_browser, browser_maxline);
	
/*
		fl_set_browser_topline(fdui -> article_list_browser,\
		first_new_or_unread_article_line);
*/
		}
	
	unread_article_at_top_flag = 0;
	}
else
	{
	/*
	re adjust browser for same position of top line (vertical scrollbar)
	*/	
	fl_set_browser_topline(fdui -> article_list_browser, browser_topline);
	}

/* structure was modified WHY, WHERE? */
/* save_articles(); */

return(1);
}/* end function show_articles */


int show_article_body(char *group, long article_id, int decoded)
{
int c;
char temp[READSIZE];	
FILE *load_filefd;
struct article *pa;
char *space, *spaceptr;
struct stat *statptr;
char *expanded_space;
int add_to_history_flag;

if(debug_flag)
	{
	fprintf(stdout,\
	"show_article_body(): arg group=%s article_id=%ld decoded=%d\n",\
	group, article_id, decoded);
	}

/* argument check */
if(! group) return(0);
if(strlen(group) == 0) return(0);
if(! article_id) return(0);

fl_set_input(fdui -> article_body_input_field, "");

load_articles(group);

/* show subject and sender in description display */
sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

sprintf(temp, "%s %s (%s)", pa -> subject, pa -> from, pa -> name);
fl_set_object_label(fdui -> description_display, temp);

/* test if allowed */
if(filters_enabled_flag)
	{
	if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY)
		{
		fl_set_input(fdui -> article_body_input_field,\
		"Filter: Permission denied");
		return(1);
		}
	}

/* article present, load it */
if(decoded)
	{
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}
else
	{
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}
if(debug_flag) fprintf(stdout, "Opening file%s\n", temp);

load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_article_body(): cannot open %s for read\n", temp);
		}
	fl_show_alert(\
	"show_article_body():", "cannot open file for read", temp, 0);
	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(load_filefd), statptr);

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_article_body(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(load_filefd);
		if(! ferror(load_filefd) ) break;
		perror("show_article_body(): read failed ");
		}/* end while error re read */	

	if(feof(load_filefd) )
		{
		fclose(load_filefd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

/*
Make a choice here to start http_browser(filename, &add_to_history_flag, 1)
add_to_history_flag not used,
*/

/*Test for <!DOCTYPE HTML PUBLIC "-//W3C//DTD W3 HTML//EN">*/
/* test for Content-Type: text/htm in body */
/*if(strcasestr(space, "!DOCTYPE HTML") ) */
/* doing it manually, button right top next to mail display*/
if(html_decode_flag)
	{
	free(space);

	/* filename in temp */
	http_browser(temp, &add_to_history_flag, 1);
	}
else
	{
	expanded_space = (char *) expand_tabs(space, tab_size);
	free(space);

	fl_set_input(\
	fdui -> article_body_input_field, expanded_space);

	/* used by extract url (in article_body_input_field_cb) */
	free(article_body_copy);
	article_body_copy = strsave(expanded_space);
	free(expanded_space);

	/* set scollbar to top of article */
	fl_set_input_topline(fdui -> article_body_input_field, 1);
	article_body_topline = 1;
	}

pa -> read_flag = 1;
get_and_set_unread_articles(group);/* does a save articles */

return(1);
}/* end function show_article_body */


int set_retrieval_flag(char *group, long article_id, int state)
{
char temp[50];	
struct article *pa, *lookup_article();

if(debug_flag)
	{
	fprintf(stdout,\
	"set_retieval_flag(): arg group=%s article_id=%ld state=%d\n",\
	group, article_id, state);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);/* fatal error */
pa -> retrieval_flag = state;

/* done in calling save_articles();*/
return(1);
}/* end function set_retrieval_flag */


int set_all_read_flags(char *group, int state)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout, "set_all_read_flags(): arg group=%s state=%d\n",\
	group, state);
	}
	
/* argument check */
if(! group) return(0);

load_articles(group);

/*pa points to next entry*/
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	pa -> read_flag = state;
	}

save_articles();
return(1);
}/* end function set_all_read_flags */


int set_read_flag(char *group, long article_id, int state)
{
char temp[50];	
struct article *pa, *lookup_article();

if(debug_flag)
	{
	fprintf(stdout,\
	"set_read_flag(): arg group=%s article_id=%ld state=%d\n",\
	group, article_id, state);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);/* fatal error */
pa -> read_flag = state;

/* done in calling save_articles();*/
return(1);
}/* end function set_read_flag */


int exists_article_body(char *group, long article_id)
{
char temp[TEMP_SIZE];	
struct article *pa, *lookup_article();

load_articles(group);/* clears structure first */

/* test if we have the body of this article, else mark for retrieval */
sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);/* fatal error */

if(filters_enabled_flag)
	{
	if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY)
		{
		fl_set_object_label(fdui -> command_status_display,\
		"Filter: Permission denied");
		return(0);
		}
	}/* end if filters_enabled_flag */

if(! pa -> have_body_flag)
	{
	pa -> retrieval_flag = 1 - pa -> retrieval_flag;
	save_articles();
	return(0);/* no article body to show */
	}

return(1);/* exists */
}/* end function exists_article_body */


int line_to_article_id(char *group, int line, long *article_id)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout, "line_to_article_id(): arg group=%s line=%d\n",\
	group, line);
	}

/* argument check */
if(! group) return(0);
if(line < 0) return(0);

load_articles(group);

for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		*article_id = atol(pa -> name);
		return(1);
		}
	}
/* no save_articles, nothing was changed */
return(0);
}/* end function line_to_article_id */


int add_marked_article_bodies_in_group_to_command_queue(\
char *group, int *some_marked_article_body)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_marked_article_bodies_in_group_to_command_queue(): arg group=%s\n",\
	group);
	}

/* argument check */
if(! group) return(0);

load_articles(group);

for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> retrieval_flag)
		{
		*some_marked_article_body = 1;

		if(request_by_message_id_flag)
			{
			add_to_article_body_list(\
			pa -> message_id, group, atol(pa -> name) );
			add_to_article_body2_list(pa -> message_id);
			acquire_body_count++;
			}
		else
			{
			add_to_command_queue(group, atol(pa -> name), GET_ARTICLE_BODY);
			}
		}/* end if pa -> retrieval_flag */
	}/* end for */

/* no save_articles nothing has changed */
return(1);
}/* end function add_marked_article_bodies_in_group_to_command_queue */


char *get_article_attachment(char *group, long article_id)
{
struct article *pa, *lookup_article();
char temp[50];
extern char* strsave();

if(debug_flag)
	{
	fprintf(stdout,\
	"get_article_attachment(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* parameter_check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

/* no save, nothing was changed */

return(pa -> attachment_name);
}/* end function get_article_attachment */ 


int delete_article_entry_header_body(char *group, long article_id)
{
int a;
int permission;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
struct article *pa, *lookup_article();
char *ptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"delete_article_entry_header_body(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

if(pa -> lock_flag) return(1);
if(pa -> retrieval_flag) return(1);
if( (pa -> have_body_flag) && (! pa -> read_flag) ) return(1);	

sprintf(temp, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
home_dir, news_server_name, database_name, group, article_id);
unlink(temp);

sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
home_dir, news_server_name, database_name, group, article_id);
unlink(temp);

/* test if binary attachment was launched before */
/* if so erase it
*/
if(! get_article_encoding(group, article_id, &a, &permission) ) return(0);
if(a & ATTACHMENT_PRESENT)
	{
	ptr = (char *) get_article_attachment(group, article_id);
	if(! ptr) return(0);/* trouble */

	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
	home_dir, news_server_name, database_name, group, ptr);
	unlink(temp);
	
	/*
	Note: I am now deleting .desc just before calling munpack in attachments.c
	because it seems sometimes xxx.desc files are left, perhaps because
	picture.jpg generates something else then picture.desc.
	So the lines below could be deleted.
DISABLED IN  attachments.c for debugging printf added in unlink
	*/
/*TO BE DELETED */
	/*
	erase .desc file generated by muncpac (if any ),
	example: picture.jpg may have picture.desc
	*/
	strcpy(temp2, ptr);
	ptr = strstr(temp2, ".");
	if(ptr)
		{
		*ptr = 0;/* replace dot by string termination, always safe */
		strcat(temp2, ".desc");
		sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
		home_dir, news_server_name, database_name, group, temp2);
		if( unlink(temp) == -1)
			{
			fprintf(stdout,\
			"delete_article_entry_header_body(): cannot unlink %s\n",
			temp);
			}
		}

/* END TO BE DELETED */
	}/* end if attachment present */

/* erase any decoded.nn */ 
if(a & DECODED_PRESENT)
	{
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	unlink(temp);
	}/* end if decoded present */

sprintf(temp, "%ld", article_id);
delete_article(temp);

return(1);
}/* end function delete_article_body */


int set_lock_flag(char *group, long article_id, int lock)
{
struct article *pa, * lookup_article();
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "set_lock_flag(): arg group=%s article_id=%ld lock=%d\n",\
	group, article_id, lock);
	}

if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);
if(pa -> lock_flag != 4) pa -> lock_flag = lock;/* NewsFleX.help articles are
													4 and cannot be erased */
return(1);
}/* end funtion set_lock_flag */


char *lookup_content_type(char *group, long article_id)
{
struct article *pa, *lookup_article();
char temp[50];

if(debug_flag)
	{
	fprintf(stdout, "lookup_content_type(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(!pa) return(0);/* no such article_id */

return(pa -> content_type);/* a char * */
}/* end function lookup_content_type */


int set_article_encoding(char *group, long article_id, int encoding, int on)
{
struct article *pa, *lookup_article();
char temp[50];

if(debug_flag)
	{
	fprintf(stdout,\
	"set_article_encoding(): arg group=%s article_id=%ld encoding=%d on=%d\n",\
	group, article_id, encoding, on);
	}

if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

if(on) pa -> encoding |= encoding;
else pa -> encoding &= 255 - encoding;

save_articles();

return(1);
}/* end function set_article_encoding */


int get_article_encoding(\
char *group, long article_id, int *encoding, int *view_permission)
{
struct article *pa, *lookup_article();
char temp[50];

if(debug_flag)
	{
	fprintf(stdout,\
	"get_article_encoding(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

/* test if allowed */
*view_permission = 1;
if(filters_enabled_flag)
	{
	if(pa -> filter_flags & FILTER_DO_NOT_SHOW_ATTACHMENT)
		{
		*view_permission = 0;
		}
	}/* end if filters_enabled_flag */
		
*encoding = pa -> encoding;

/* no save, nothing was changed */
return(1);
}/* end function get_article_encoding */


int set_article_attachment(\
char *group, long article_id, char *attachment_name)
{
struct article *pa, *lookup_article();
char temp[50];
extern char* strsave();

if(debug_flag)
	{
	fprintf(stdout,\
"set_article_attachment(): arg group=%s article_id=%ld attachment_name=%s\n",\
	group, article_id, attachment_name);
	}

/* parameter_check */
if(! group) return(0);
if(article_id < 0) return(0);
if(! attachment_name) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(0);

free(pa -> attachment_name);
pa -> attachment_name = (char *) strsave(attachment_name);

save_articles();

return(1);
}/* end function set_article_attachment */ 


int set_article_no_longer_availeble_flag(char *group, long article_id)
{
struct article *lookup_article(), *pa;
char temp[50];

if(debug_flag)
	{
	fprintf(stdout,\
	"set_article_no_longer_availeble_flag(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
pa = lookup_article(temp);
if(! pa) return(1);

pa -> not_availeble_flag = 1;
pa -> retrieval_flag = 0;/* do not try again */

save_articles();
return(1);
}/* end function set_article_no_longer_availeble_flag */


int decode_articles_with_content_type_id(char *group, char *content_type_id)
{
int c, i, j;
struct article *pa;
char *ptr;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "decode_articles_with_content_type_id(): arg id=%s\n",\
	content_type_id);
	}

load_articles(group);

/* for all articles */
pa = articletab[0];
while(1)
	{	
	if(! pa) return(1);/* end chain */
	i = 0;
	/* extract the id= part */
	while(1)/* for all parts of content_type */
		{
		/* get article from partial_articles */
		j = 0;
		while(1)/* for all character in a field */
			{
			c = pa -> content_type[i];
			if( (c == ' ') || (c == 10) || (c == 13) )
				{
				i++;
				continue;
				}
			temp[j] = c;
			if(c == 0) break;/* end of content_type */
			if(c == ';')/* end of field in content_type */
				{
				temp[j] = 0;/* string termination */
				break;
				}
			i++;
			j++;
			}/* end while field in content_type */
		/* check if this field holds "id=" */
		ptr = strstr(temp, "id=");
		if(ptr) /* found id */
			{
			/* extract id */
			if(debug_flag)
				{
				fprintf(stdout,\
				"decode_articles_with_content_type_id(): found id=%s\n",\
				temp);
				}
			/* test if same content_type_id */
			if(strcmp(content_type_id, ptr + 3) == 0)
				{
				decode_article(group, atol(pa -> name), pa -> content_type);
				}
			}/* end found id */
		if(c == 0) break;
		i++;/* scip the ';' */
		}/* end while all parts of content_type */
	pa = pa -> nxtentr;
	}/* end while all articles (structures) */
}/* end function decode_articles_with_content_type_id */


int get_and_set_unread_articles(char *group)
{
struct article *pa;
long unread_articles;

if(debug_flag)
	{
	fprintf(stdout, "get_and_set_unread_articles(): arg group=%s\n", group);
	}
/* argument check */
if(! group) return(0);

load_articles(group);

unread_articles = 0;
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(! pa -> read_flag) unread_articles++;	
	}
set_new_articles(group, unread_articles); 

save_articles();
return(1);
}/* end function get_and_set_unread_articles */


int delete_all_not_locked_articles(char *group)
{
int view_permission;
int delete, encoded;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
struct article *pa, *pprev, *pdel, *pnext;
long article_id;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_not_locked_articles(): arg group=%s\n",\
	group);
	}
	
sprintf(temp, "Deleting all not locked articles in %s", group);
fl_set_object_label(fdui -> command_status_display, temp);
XSync(fl_get_display(), 0);

/* argument check */
if(! group) return(0);

load_articles(group);
pa = articletab[0];
while(1)
	{
	if(! pa) break;
	delete = 1;
	if(pa -> lock_flag) delete = 0;
	if(pa -> retrieval_flag) delete = 0;
	if( (pa -> have_body_flag) && (! pa -> read_flag) ) delete = 0;	
	if(! delete)
		{
		pa = pa -> nxtentr;
		continue;
		}

	/* delete body */
	article_id = atol(pa -> name);
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	unlink(temp);

	/* delete header */
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	unlink(temp);

	/* test if binary attachment was launched before */
	/* if so erase it */
	article_id = atol(pa -> name);
	if(! get_article_encoding(\
	group, article_id, &encoded, &view_permission) ) return(0);
	if(debug_flag)
		{
		fprintf(stdout, "after get_article_encoding encoded=%d\n", encoded);
		}

	/* delete any attachment */
	if(encoded & ATTACHMENT_PRESENT)
		{
		ptr = (char *) get_article_attachment(group, article_id);
		if(! ptr) return(0);/* more trouble */

		/* delete attachment */
		sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
		home_dir, news_server_name, database_name, group, ptr);
		unlink(temp);
		/*
		erase .desc file generated by muncpac (if any ),
		example: picture.jpg may have picture.desc
		*/
		strcpy(temp2, ptr);
		ptr = strstr(temp2, ".");
		if(ptr)
			{
			*ptr = 0;/* replace dot by string termination, always safe */
			strcat(temp2, ".desc");
			sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
			home_dir, news_server_name, database_name, group, temp2);
			unlink(temp);
			}
		}/* end if attachment present */

	/* delete any decoded.nnn */
	if(encoded & DECODED_PRESENT)
		{
		sprintf(temp,\
		"%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
		home_dir, news_server_name, database_name, group, article_id);
		unlink(temp);
		}/* end decoded present */

	/* remove structure from linked list */
	/* get the address of the to be deleted structure */
	pdel = pa;

	/* get next and previous structure */
	pnext = pa -> nxtentr;
	pprev = pa -> prventr;
	
	/* set pointers for previous structure */
	/* if first one, modify articletab[0] */
	if(pprev == 0) articletab[0] = pnext;
	else pprev -> nxtentr = pnext;
	
	/* set pointers for next structure */
	/* if last one, modify articletab[1] */
	if(pnext == 0) articletab[1] = pprev;
	else pnext -> prventr = pprev;

	/* point to next one */
	pa = pa -> nxtentr;

	/* delete structure data */	
	free(pdel -> name);/* free name */
	free(pdel -> subject);
	free(pdel -> from);
	free(pdel -> content_type);
	free(pdel -> references);
	free(pdel -> message_id);		
	free(pdel -> attachment_name);
	free(pdel -> date);
	free(pdel);/* free structure */
	}/* end for all structures */

get_and_set_unread_articles(group); /* does a save articles */

/* clear message in command status display */
fl_set_object_label(fdui -> command_status_display, "");
return(1);
}/* end function delete_all_not_locked_articles */


int display_next_unread_article()
{
struct article *pa, *lookup_article();
char temp[80];
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "display_next_unread_article():\n\
	arg using selected_group=%s selected_article=%ld\n",\
	selected_group, selected_article);	
	}

/* argument check */
if(! selected_group) return(0);
if(selected_article < 0) return(0);

/* remember where we are */
strcpy(start_unread_group, selected_group);
start_unread_article = selected_article;

load_articles(selected_group);

/* move to present position */
sprintf(temp, "%ld", selected_article);
pa = lookup_article(temp);
/* if no article selected error */
if(! pa) return(0);

/* inform user what is happening */
fl_set_object_label(fdui -> command_status_display,\
"Searching next unread article");
/* force display */
XSync(fl_get_display(), 0);

while(1)/* for all groups */
	{
	/*
	search for next unread that we have body from from this point
	onwards
	*/
	while(1)
		{
		if(pa -> have_body_flag)
			{
			if(! pa -> read_flag)
				{
				selected_article = atol(pa -> name);

/*
				if(pa -> encoding)
					{
					launch_binary_attachment(\
					selected_group, selected_article);
					return(1);
					}
				else
*/	
					{
					show_browser(ARTICLE_BODY_BROWSER);
					refresh_screen(selected_group);
					fl_set_object_label(\
					fdui -> command_status_display, selected_group);
					return(1);
					}/* end not encoded */
				}/* end read */
			}/* end have body */

		/* move to next article */
		pa = pa -> nxtentr;
		
		if(! pa)/* end of article list in this group */
			{
			/* search for next not empty group */
			while(1)
				{
				/* move to next group */
				ptr = (char *) get_next_subscribed_group(selected_group);
				if(! ptr)
					{
					fl_set_object_label(\
					fdui -> command_status_display, selected_group);
	
					return(0);/* possibly no subscribed groups */
					}
				if(selected_group) free(selected_group);
				selected_group = strsave(ptr);
				load_articles(selected_group);

				/* first article in next group */
				pa = articletab[0];
				/* if group not empty (valid pa) test article */
				if(pa) break;
				}/* end while search for not empty group */
			}/* end if end of article list */
				
		/* test for round */
		if(strcmp(selected_group, start_unread_group) == 0)
			{
			if(atol(pa -> name) == start_unread_article)
				{
				fl_show_alert("All articles have been read", "", "", 0);
				fl_set_object_label(fdui -> command_status_display,\
				selected_group);
				return(0);
				}
			}/* end if round */
		}/* end while all articles in this group */
	}/* end while all articles */

fl_set_object_label(fdui -> command_status_display, selected_group);
return(1);
}/* end function display_next_unread_article */


int display_next_unread_article_in_subject()
{
struct article *pa, *lookup_article(), *pselected;
char *pssubject, *pasubject;
char temp[TEMP_SIZE];
int match;

/*
I have a problem here,
this function is impossible.
Threads may split, that would require a decision by NewsFleX which branch to
follow.
A choice could be presented to the user, but making a choice would leave
out all other branches in the original starting point.
To make a usefull choice you would have to read the articles first :).
So now implementing as next unread in subject.
*/

if(debug_flag)
	{
	fprintf(stdout, "display_next_unread_article_in_subject() arg none\n");
	}
	
/* argument check */
if(! selected_group) return(0);
if(selected_article < 0) return(0);

sprintf(temp, "%ld", selected_article);
pselected = lookup_article(temp);
if(! pselected) return(0);

/* test if reference line present */
/*Thread if(! pselected -> references) return(0);*/

/*
Note: start_article and start_group set to selected_article and
selected_group, by search_accept_button_cb.
*/

/* inform user what is happening */
fl_set_object_label(fdui -> command_status_display,\
"Searching next unread article in subject");
/* force display */
XSync(fl_get_display(), 0);

match = 0;
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if( (pa -> have_body_flag) && (! pa -> read_flag) )
		{

		/* test if reference line present, or exeption, if not there! */
/*		Thread if(! pa -> references) continue;*/
		
			/*
			If the selected_article is a Re:
			For an article to be a followup to the selected_article,
			the references line of the selected_article must be present in
			the reference line of the article.
			*/

			/*
			else
			The reference line of the article must be the same as the 
			mesage_id of the selected_article.
			*/

			/*
			Easing the requirement, to message_id of the selected_article
			must be present in the reference line of the article.
			Now if missing articles in thread, 
			then later ones are also shown.
			*/

			/*
			Gave up on this function as 'next thread',
			see comment at top of function.
			*/

		/* The 'next unread in subject' implementation */
		
		/* strip any "Re: " in selected_article subject */
		if(strstr(pselected -> subject, "Re: ") == pselected -> subject)
			{
			pssubject = pselected -> subject + 4;
			}
		else pssubject = pselected -> subject;
		
		/* strip any "Re: " in article subject */
		if(strstr(pa -> subject, "Re: ") == pa -> subject)
			{
			pasubject = pa -> subject + 4;
			}
		else pasubject = pa -> subject;
		
		/* now we can compare subjects */ 
		if(strcmp(pssubject, pasubject) == 0) match = 1;
				
		if(match)
			{
			selected_article = atol(pa -> name);

			/*
			if(pa -> encoding)
				{
				launch_binary_attachment(\
				selected_group, selected_article);
				fl_set_object_label(fdui -> command_status_display,\
				selected_group);
				return(1);		
				}
			else
			*/

				{
				show_browser(ARTICLE_BODY_BROWSER);
				refresh_screen(selected_group);/* always in this group */
				fl_set_object_label(fdui -> command_status_display,\
				selected_group);
 				return(1);
				}/* end not encoded */

			}/* end if match */
		}/* end not read and have body */
	}/* end for all structures in chain */

fl_show_alert("No more unread articles on this subject", "", "", 0);
fl_set_object_label(fdui -> command_status_display, selected_group);
return(1);
} /* end function display_next_unread_article_in_subject */


int set_follow_flag(char *group, long article_id, int value)
{
struct article *pa, *ps, *lookup_article();
char temp[80];
char *ptr1, *ptr2;

if(debug_flag)
	{
	fprintf(stdout,\
	"set_follow_flag(): arg group=%s article_id=%ld value=%d\n",\
	group, article_id, value);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(temp, "%ld", article_id);
ps = lookup_article(temp);
if(! ps) return(0);

if( (value == FOLLOW_THREAD) || (value == FOLLOW_SUBJECT) )
	{
	/*
	set the lock flag, so if later read, not flushed, need this for reference.
	*/
	ps -> lock_flag = 1;
	/* note: we never unlock, decision left to the user */
	}

if(value == FOLLOW_THREAD)
	{
	ps -> follow_flag |= FOLLOW_THIS_THREAD;

	if(! ps -> have_body_flag) ps -> retrieval_flag = 1;	
	return(1);
	}

if(value == DO_NOT_FOLLOW_THREAD)
	{
	ps -> follow_flag &= (255 - FOLLOW_THIS_THREAD);

	ps -> retrieval_flag = 0;
	return(1);
	}

if( (value == FOLLOW_THREAD) || (value == DO_NOT_FOLLOW_THREAD) )
	{
	/* to be done, see below? */
	}
	
if( (value == FOLLOW_SUBJECT) || (value == DO_NOT_FOLLOW_SUBJECT) )
	{
	/* strip possible "Re: " */
	if(strstr(ps -> subject, "Re: ") == ps -> subject)
		{
		ptr2 = ps -> subject + 4;
		}
	else ptr2 = ps -> subject;

	/* modify the follow flag in all articles with this subject */
	for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
		{
		/* strip the possible "Re: ". */
		if(strstr(pa -> subject, "Re: ") == pa -> subject)
			{
			ptr1 = pa -> subject + 4;
			}
		else ptr1 = pa -> subject;

		/* test if subject match */
		if(strcmp(ptr1, ptr2) == 0)
			{
			if(value == FOLLOW_SUBJECT)
				{
				pa -> follow_flag |= FOLLOW_THIS_SUBJECT;

				/* set the retrieval flag if we do not have article body */
				if(! pa -> have_body_flag) pa -> retrieval_flag = 1;
				}
			if(value == DO_NOT_FOLLOW_SUBJECT)
				{
				pa -> follow_flag &= (255 - FOLLOW_THIS_SUBJECT);

				/* reset the retrieval flag if set */
				pa -> retrieval_flag = 0;
				}
			}/* end match */
		}/* end for all articles */

	return(1);
	}/* end if value is follow subject or value is do not follow subject */

/* if here illegal value */
return(0);
}/* end function set_follow_flag */


char *first_group_in_newsserver_and_database;

int search_next_article(char *search_groups_keywords)
{
struct article *pa, *lookup_article();
char temp[TEMP_SIZE];
char *ptr;
int group_match;

if(debug_flag)
	{
	fprintf(stdout,\
	"search_next_article(): arg search_groups_keywords=%s\n\
selected_group=%s selected_article=%ld\n",\
	search_groups_keywords, selected_group, selected_article);	
	fprintf(stdout,\
	"using start_article=%ld start_group=%s", start_article, start_group);
	fprintf(stdout,\
	"using start_newsserver_and_database=%s\n",\
	start_newsserver_and_database);
	fprintf(stdout,\
	"using first_matching_group=%s first_matching_article=%ld\n",\
	first_matching_group, first_matching_article);
	fprintf(stdout, "first_matching_newsserver_and_database=%s\n",\
	first_matching_newsserver_and_database);
	}

/* argument check */
if(! search_groups_keywords) return(0);

/* in case called from idle callback, prevent looping */
go_search_next_article_from_idle_cb_flag = 0;

search_last_search = SEARCH_ARTICLES;

load_articles(selected_group);

/* move to present position */
sprintf(temp, "%ld", selected_article);
pa = lookup_article(temp);
/* if not found, start at first artilcel in list */
if(! pa) pa = articletab[0];
/* if found, select next article */
else pa = pa -> nxtentr; /* select next position */
/* and if pa == 0, for() will end, and next group is selected */

/* for all newsservers and databases */
while(1)
	{
	if(first_group_in_newsserver_and_database)\
	free(first_group_in_newsserver_and_database);
	first_group_in_newsserver_and_database = strsave(selected_group);
	if(! first_group_in_newsserver_and_database) return(0);

	/* for all subscribed groups */
	while(1)
		{
		/*
		if no subscribed groups, or all groups empty, selected_article = -1 
		*/
		if(debug_flag)
			{
			fprintf(stdout,\
			"start_newsserver_and_database=%s\n",\
			start_newsserver_and_database);
			fprintf(stdout,\
			"start_group=%s start_article=%ld\n",\
			start_group, start_article);

			fprintf(stdout,\
			"news_server=%s database=%s\n",\
			news_server_name, database_name); 
			fprintf(stdout, \
			"selected_group=%s selected_article=%ld\n",\
			selected_group, selected_article);

			fprintf(stdout,\
			"first_matching_group=%s first_matching_article=%ld\n",
			first_matching_group, first_matching_article);
			fprintf(stdout, "first_matching_newsserver_and_database=%s\n",\
			first_matching_newsserver_and_database);
			}

		if(strlen(search_groups_keywords) == 0) group_match = 1;
		else
			{
			/* test if match search_groups_keywords for this group */
			group_match = search_in_for(\
			selected_group, search_groups_keywords,\
			search_case_sensitive_flag);
			}

		/* for all articles in group */
		for(pa = pa; pa != 0; pa = pa -> nxtentr)
			{
			/* test for permission to show header */
			if(filters_enabled_flag)
				{
				if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
				}/* end if filters_enabled_flag */
		
			/*
			a special case is when there is only one matching article,
			and it is the selected one.
			If we do not stop here, it will be found over and over again.
			*/
			/* make sure no NULL pointers, else segfault in strcmp */
			if(first_matching_newsserver_and_database && first_matching_group)
				{
				sprintf(temp, "%s %s", news_server_name, database_name);
				if( (atol(pa -> name) == first_matching_article) &&\
				(strcmp(selected_group, first_matching_group) == 0) &&\
				(strcmp(temp, first_matching_newsserver_and_database) == 0) )
					{
					fl_show_alert(\
					"All has been searched", "", "", 0);
					/* this will hide the question form from news_cb.c */
					ask_question_flag = 0;
					return(0); /* round */
					}
				}/* end if first_matching_newsserver_and_database */
			if(group_match)
				{
				if(search_all(\
				selected_group, pa -> name, pa -> encoding,\
				pa -> attachment_name, pa -> have_body_flag, SEARCH_ARTICLES) )
					{
					selected_article = atol(pa -> name);
					show_browser(ARTICLE_BODY_BROWSER);
					refresh_screen(selected_group);
					if(first_matching_article == -1)
						{
						first_matching_article = atol(pa -> name);

						if(first_matching_group) free(first_matching_group);
						first_matching_group = strsave(selected_group);
						if(! first_matching_group) return(0);

						sprintf(temp, "%s %s", news_server_name, database_name);
						if(first_matching_newsserver_and_database)\
						free(first_matching_newsserver_and_database);
						first_matching_newsserver_and_database =\
						strsave(temp);
						if(! first_matching_newsserver_and_database) return(0);
						}
					return(1); /* found */
					}/* end found */
				}/* end if group match */
			}/* end for all articles */

		/* no more in this group */
		ptr = (char *) get_next_subscribed_group(selected_group);
		if(! ptr) break;/* possibly no subscribed groups,
						   try next server database */
		if(debug_flag)
			{
			fprintf(stdout,\
			"search_next_article(): next subscribed group=%s\n", ptr);
			}

		if(selected_group) free(selected_group);
		selected_group = strsave(ptr);
		load_articles(selected_group);
		pa = articletab[0];

		if(\
		(strcmp(first_group_in_newsserver_and_database, selected_group)\
		== 0) ||\
		(strcmp(selected_group, "no.subscribed_groups") == 0) )
			{
			break;/* from while groups */
			}/* end if same group */

		}/* end while all subscribed groups */

	/* move to next newsserver and database */
	while(1)
		{
		/* move to next newsserver and database */
		sprintf(temp, "%s %s", news_server_name, database_name);
		ptr = find_next_newsserver_and_period(temp);
		if(! ptr) return(0);
		if(debug_flag)
			{
			fprintf(stdout,\
			"search_next_article(): next_newsserver_and_period=%s\n", ptr);
			}
		select_newsserver(ptr);
		if(strcmp(start_newsserver_and_database, ptr) == 0)
			{
			go_search_next_article_from_idle_cb_flag = 0;
			fl_show_alert("All has been searched", "", "", 0);

			/* this will close question form from news_cb.c */
			ask_question_flag = 0;
			return(0); /* round */
			}
 
		/*
		If no subscribed groups, or all groups empty, selected_article = -1 
		in that case try next newsserver database.
		*/
		if(selected_article != -1) break;
		}/* end while next newsserver_and_database */

	pa = articletab[0];
	}/* end while all newsservers and databases */

/*fprintf(stdout, "RETURNING END OF FUNCTION\n");*/
return(1);
}/* end function search_next_article */


int set_all_filter_flags_in_group(char *group)
{
struct article *pa;
/*char temp[TEMP_SIZE];*/

if(debug_flag)
	{
	fprintf(stdout, "set_all_filter_flags_in_group(): arg group=%s\n",\
	group);
	}
	
/* argument check */
if(! group) return(0);

load_articles(group);

for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* tell the user what is happening NO this would reveil groups */
	/*
	sprintf(temp, "Setting filter flags for group %s article %s",\
	group, pa -> name);
	fl_set_object_label(fdui -> command_status_display, temp);
	XSync(fl_get_display(), 0);
	*/
		
	/* get the filter result */
	pa -> filter_flags = 0;/* the default */

	get_filter_flags(\
	group, pa -> name, pa -> encoding, pa -> attachment_name,\
	pa -> have_body_flag, SEARCH_ARTICLES, &(pa -> filter_flags) );
	
	}/* end for all structures */

/* clear the command status display */
/*
fl_set_object_label(fdui -> command_status_display, "");
XSync(fl_get_display(), 0);
*/		
save_articles();
return(1);
}/* end function set_all_filter_flags_in_group */


int apply_filters_to_article(char *group, long article_id)
{
struct article *pa, *lookup_article();
char article_name[50];

if(debug_flag)
	{
	fprintf(stdout,\
	"apply_filters_to_article_header(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

load_articles(group);

sprintf(article_name, "%ld", article_id);
pa = lookup_article(article_name);
if(! pa) return(0);

get_filter_flags(\
group, article_name, pa -> encoding, pa -> attachment_name,\
pa -> have_body_flag, SEARCH_ARTICLES, &(pa -> filter_flags) );

save_articles();
return(1);
}/* end function apply_filters_to_article_header */


int set_all_retrieval_flags_in_new_articles(char *group, int state)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"set_all_retrieval_flags_in_new_articles(): arg group=%s state=%d\n",\
	group, state);
	}
	
/* argument check */
if(! group) return(0);

load_articles(group);

/*pa points to next entry*/
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(! pa -> have_body_flag)
		{
		pa -> retrieval_flag = state;
		}
	}

save_articles();
return(1);
}/* end function set_all_retrieval_flags_in_new_articles */


int highlight_articles(char *group, struct article *pa)
{
int c, i;
int found;
int first_char_flag, folded_flag, lf_flag;
int first_char;
char *database;
char temp[READSIZE];
FILE *header_fd;

if(debug_flag)
	{
	fprintf(stdout,\
	"highlight_articles(): arg group=%s pa=%lu\n", group, (long)pa);
	}

/* argument check */
if(! group) return(0);
if(! pa) return(0);

first_char = 0;/* just to keep gcc -Wall happy */

if(! highlight_articles_flag) return(0);

/* headers */
if(highlight_headers_flag)
	{
	database = 0; /* keep gcc -Wall happy */

	/* subject */
	if(highlight_headers_subject_flag)
		{
		if(strstr(pa -> subject, "\001") )
			{
			database = (char *) get_formatted_header_data(\
			group, atol(pa -> name), "Subject:");
			if(! database) return(0);
			}
		else
			{
			database = strsave(pa -> subject);
			}
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_headers_flag */

	/* summary */
	/* FLAG DOES NOT EXIST, use others */
	/*
	if(highlight_headers_summary_flag)
		{
		if(pa -> summary_flag)
			{
			database = (char *) get_formatted_header_data(\
			group, atol(pa -> name), "Summary");
			if(! database) return(0);
			found = search_in_for_or(\
			database, highlight_keywords, highlight_case_sensitive_flag);
			free(database);
			if(found) return(1);
			}
		}
	*/
	
	/* from */
	if(highlight_headers_from_flag)
		{
		database = strsave(pa -> from);
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_from_flag */
	
	/* references */
	if(highlight_headers_references_flag)
		{
		database = strsave(pa -> references);
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_references_flag */

	/* to (newsgroups) */
	if(highlight_headers_to_flag)
		{
		database = (char *) get_formatted_header_data(\
		group, atol(pa -> name), "Newsgroups:");
		if(! database) return(0);

		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_to_flag */

	/* others */
	if(highlight_headers_others_flag)
		{
		sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
		home_dir, news_server_name, database_name, group, atol(pa -> name) );
		header_fd = fopen(temp, "r");
		if(!header_fd)
			{
			fl_show_alert("Cannot open", temp, "for read", 0);
			return(0);
			}		
		else
			{
			/* scan the headerfile */
			temp[0] = 0;/* mark empty */
			first_char_flag = 0;
			while(1) /* all entries (unfolded header lines)
						in header file */
				{
				folded_flag = 0; /* the function of this variable is to
									find out if the line was folded */
				i = 0;
				lf_flag = 0;
				while(1)/* all characters in a line */
					{
					if(first_char_flag)
						{
						temp[i] = first_char;
						i++;
						first_char_flag = 0;
						}

					while(1)/* error re read */
						{
						c = getc(header_fd);
						if(! ferror(header_fd) ) break;
						perror("highlight(): read failed ");
						}/* end while error re read */	

					if(feof(header_fd) || (c == EOF) )
						{
						fclose(header_fd);
						c = EOF;
						break;
						}
		
					if(lf_flag)
						{
						if( (c != ' ') && (c != 9) )/* TAB = 9, no LWS */
							{
							/*
							line ends, save this character,
							it is the first one in the next line.
							*/
							first_char = c;
							first_char_flag = 1;
							break;
							}
						/* line continous */
						lf_flag = 0;
						folded_flag = 1;
						/* store char in line */
						}
			
					if(c == '\n')
						{
						lf_flag = 1;		
						continue;/* get next char,
									test for linear white space */
						}
			
					temp[i] = c;
					i++;
					if(i >= READSIZE - 1) break; /* prevent overflow */
					}/* end while all characters in a line */
				temp[i] = 0;/* string termination */
					
				if(strstr(temp, "From:") == 0)
					{
					if(strstr(temp, "Newsgroups:") ==  0)
						{
						if(strstr(temp, "Subject:") ==  0)
							{		
							if(strstr(temp, "References:") ==  0)
								{
								found = search_in_for_or(\
								temp, highlight_keywords,\
								highlight_case_sensitive_flag);
								if(found)
									{
									if(c != EOF) fclose(header_fd);
									return(1);
									}
								}/* end no references */
							}/*	end no subject */
						}/* end no to */
					}/* end no from */

				if(c == EOF) break;
					
				}/* end while all lines in file */
			}/* end header_fd != 0 */
		}/* end highlight_headers_others_flag */
	}/* end if highlight_headers_flag */

return(0);
}/* end function highlight_articles */


int cancel_article(char *group, long article)
{
char temp[TEMP_SIZE];
struct article *lookup_artcle(), *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"cancel_article(): arg group=%s article=%ld\n",\
	group, article);
	}

/* argument check */
if (! group) return(0);
if(article < 0) return(0);

/* get the message_id */
load_articles(group);
sprintf(temp, "%ld", article);
pa = lookup_article(temp);
if(! pa) return(0);

/* fake this */
posting_source = POSTING_MENU;

/* true */
posting_menu_selection = NEW_ARTICLE;

/* fill in the input fields for the posting editor */
if(! fill_in_input_fields(group, 99999999, posting_menu_selection) )
	{
	return(0);
	}

/* show the editor */
/*
fl_show_form(fd_NewsFleX -> posting_editor,\
FL_PLACE_CENTER,  FL_UP_FRAME, "");
*/

/* set the subject */
sprintf(temp, "cmsg %s", pa -> message_id);
fl_set_input(fdui -> posting_subject_input_field, temp);

/* set the  custom header */
sprintf(temp, "Control: cancel %s\n", pa -> message_id);
fl_set_input(fdui -> local_custom_headers_editor_input_field, temp);

/* tell save_posting about the custom header */
custom_headers_state |= LOCAL_HEADERS_MODIFIED;
custom_headers_state |= LOCAL_HEADERS_ENABLED;

/* force a save in save_posting */
posting_modified_flag = 1;

save_posting(group, article, SEND_LATER);/* sets global body_posting */

if(debug_flag)
	{
	fprintf(stdout,\
	"cancel_article(): group=%s article=%ld pa=%ld pa->msg_id=%s\n",
	group, article, (long)pa, pa -> message_id);
	}

/* mark posting for transmit */
set_send_status(body_posting, TO_BE_SEND);

return(1);
}/* end function cancel_article */


int export_article_plus_header(char *group, long article, int header_flag)
{
char *ptr;
char temp[READSIZE];
FILE *execfptr;
FILE *fptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"export_article_plus_header(): arg group=%s article=%ld header=%d\n",\
	group, article, header_flag);
	}

/* argument check */
if(! group) return(0);
if(article < 0) return(0);
if(strlen(group) == 0)
	{
fprintf(stdout,\
"export_article_plus_header(): bogus request, strlen group=0\n");
	return(0);
	}

ptr = (char *) fl_show_input("Path filename?", "");
if(! ptr) return(0);

/* test if destination file can be created */
sprintf(temp, "%s", ptr);
fptr = fopen(ptr, "w");
if (! fptr)
	{
	fl_show_alert("Cannot create file", ptr, "", 0);
	return 0;
	}
fclose(fptr);

if(header_flag)
	{
	sprintf(temp,\
	"cat %s/.NewsFleX/%s/%s/%s/head.%ld %s/.NewsFleX/%s/%s/%s/body.%ld > %s",\
	home_dir, news_server_name, database_name, group, article,\
	home_dir, news_server_name, database_name, group, article,\
	ptr);
	}
else
	{
	sprintf(temp,\
	"cp %s/.NewsFleX/%s/%s/%s/body.%ld %s",\
	home_dir, news_server_name, database_name, group, article, ptr);
	}
execfptr = popen(temp, "r");
if(! execfptr)
	{
	fl_show_alert("Cannot execute command", temp, "", 0);
	return(0);
	}
pclose(execfptr);
return(1);
}/* end function export_article_plus_header */


int get_first_article_in_group(char *group, long *article)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_first_article_in_group(): arg group=%s\n",\
	group);
	}

/* argument check */
if(! group) return(0);
if(strlen(group) == 0) return(0);
if(strstr(group, "/") != 0) return(0);
if(strstr(group, " ") != 0) return(0);

*article = -1;
if(! load_articles(group) ) return(0);

/* get first entry */
pa = articletab[0];

/* if no entry return */
if(!pa) return(1);

/* return article number */
*article = atol(pa -> name);
return(1);
}/* end function get_first_article_in_group */


int mark_all_articles_for_retrieval_in_group(\
char *group, int *found_some_flag)
{
struct article *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"mark_all_articles_for_retrieval_in_group(): arg group = %s\n", group);
	}

load_articles(group);

*found_some_flag = 0;
for(pa = articletab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(! pa -> read_flag)
		{
		if(! pa -> have_body_flag)
			{
			/* will be here again, prevent loop (process_command_queue() ) */
			if(! pa -> retrieval_flag)
				{
				pa -> retrieval_flag = 1;
				*found_some_flag = 1;
				}
			}
		}
	}

/* structure has changed */
save_articles();

return(1);
}/* end function mark_all_articles_for_retrieval_in_group */


