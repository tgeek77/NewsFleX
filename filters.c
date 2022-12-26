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


struct filter
	{
	char *name;/* this is the filter id long decimal */
	char *keywords;
	char *groups_keywords;
	int postings_flag;
	int articles_flag;
	int header_flag;
	int header_from_flag;
	int header_to_flag;
	int header_subject_flag;
	int header_newsgroups_flag;
	int header_others_flag;
	int body_flag;
	int attachment_flag;
	int groups_flag;
	int case_flag;
	int mode;
	int allow_flag;
	int only_flag;
	int group_show_flag;
	int header_show_flag;
	int body_show_flag;
	int attachment_show_flag;
	int browser_line;/* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
	struct filter *nxtentr;
	struct filter *prventr;
	};
struct filter *filtertab[2]; /* first element points to first entry,
								second element to last entry */


struct filter *lookup_filter(char *name)
{
struct filter *pa;

for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/* not found */
}/* end function lookup_filter */


struct filter *install_filter_at_end_of_list(char *name)
{
struct filter *plast, *pnew;
struct filter *lookup_filter();

if(debug_flag)
	{
	fprintf(stdout, "install_filter(): arg name=%s\n", name);
	}

pnew = lookup_filter(name);
if(pnew)
	{
	/* free previous definition */
/*	free(pnew -> subject);*/
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct filter *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = filtertab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previous structure pointers */
if( !filtertab[0] ) filtertab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
filtertab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_filter */


int delete_filter(long filter_id)/* delete entry from double linked list */
{
struct filter *pa, *pprev, *pdel, *pnext;
char name[50];

if(debug_flag)
	{
	fprintf(stdout, "delete_filter(): arg filter_id=%ld\n", filter_id);
	}

sprintf(name, "%ld", filter_id);
pa = filtertab[0];
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
	/* if first one, modify filtertab[0] */
	if(pprev == 0) filtertab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify filtertab[1] */
	if(pnext == 0) filtertab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel -> keywords);
	free(pdel -> groups_keywords);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_filter */


int delete_all_filters()/* delete all entries from table */
{
struct filter *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_filters() arg none\n");
	}

while(1)
	{	
	pa = filtertab[0];
	if(! pa) break;
	filtertab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa -> keywords);
	free(pa -> groups_keywords);
	free(pa);/* free structure */
	}/* end while all structures */
filtertab[1] = 0;
if(debug_flag) fprintf(stdout, "returning from delete_all_filters\n");
return(1);
}/* end function delete_all_filters */


int load_filters()
{
int a;
FILE *filters_dat_file;
char temp[READSIZE];
char pathfilename[TEMP_SIZE];
extern long atol();
struct filter *pa;
extern char *strsave();
int first_filter_flag;
char name[50];
int postings_flag;
int articles_flag;
int header_flag;
int header_from_flag;
int header_to_flag;
int header_subject_flag;
int header_newsgroups_flag;
int header_others_flag;
int body_flag;
int groups_flag;
int attachment_flag;
int case_flag;
int mode;
int allow_flag;
int only_flag;
int group_show_flag;
int header_show_flag;
int body_show_flag;
int attachment_show_flag;
int browser_line; /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
char keywords[TEMP_SIZE];
char groups_keywords[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "load_filters() arg none using selected_filter=%ld\n",\
	selected_filter);
	}
	
delete_all_filters();

sprintf(pathfilename, "%s/.NewsFleX/filters.dat", home_dir);
filters_dat_file = fopen(pathfilename, "r");
if(! filters_dat_file)
	{
	if(debug_flag)fprintf(stdout, "could not load file %s\n", pathfilename);
	return(0);
	} 

first_filter_flag = 1;
while(1)
	{
	a = readline(filters_dat_file, temp);
	if(a == EOF) return(1);/* empty file or last line */
	
	sscanf(temp,\
	"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",\
	name,
	&postings_flag,
	&articles_flag,
	&header_flag,
	&header_from_flag,
	&header_to_flag,
	&header_subject_flag,
	&header_newsgroups_flag,
	&header_others_flag,
	&body_flag,
	&attachment_flag,
	&groups_flag,
	&case_flag,
	&mode,
	&allow_flag,
	&only_flag,
	&group_show_flag,
	&header_show_flag,
	&body_show_flag,
	&attachment_show_flag,
	&browser_line); /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */

	a = readline(filters_dat_file, keywords);
	if(a == EOF) break;/* file format error */

	a = readline(filters_dat_file, groups_keywords);
	if(a == EOF) break; /* file format error */

	/* create a structure entry */
	pa = install_filter_at_end_of_list(name);
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"load_filters: cannot install filter %s\n", temp);
			}
		return(0);
		}

	pa -> postings_flag = postings_flag;
	pa -> articles_flag = articles_flag;
	pa -> header_flag = header_flag;
	pa -> header_from_flag = header_from_flag;
	pa -> header_to_flag = header_to_flag;
	pa -> header_subject_flag = header_subject_flag;
	pa -> header_newsgroups_flag = header_newsgroups_flag;
	pa -> header_others_flag = header_others_flag;
	pa -> body_flag = body_flag;
	pa -> groups_flag = groups_flag;
	pa -> attachment_flag = attachment_flag;

	pa -> case_flag = case_flag;
	pa -> mode = mode;
	pa -> allow_flag = allow_flag;
	pa -> only_flag = only_flag;

	pa -> group_show_flag = group_show_flag;
	pa -> header_show_flag = header_show_flag;
	pa -> body_show_flag = body_show_flag;
	pa -> attachment_show_flag = attachment_show_flag;
	pa -> browser_line = browser_line;
	
	pa -> keywords = strsave(keywords);
	if(! pa -> keywords) return(0);

	pa -> groups_keywords = strsave(groups_keywords);
	if(! pa -> groups_keywords) return(0);

	if(first_filter_flag)
		{
		selected_filter = atol(name);
		first_filter_flag = 0;
		}
	}/* end while all lines in filters.dat */

if(debug_flag)
	{
	fprintf(stdout, "load_filters(): file format error\n");
	}

return(0);
}/* end function load filters */


int save_filters()
{
FILE *filters_dat_file;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct filter *pa;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "save_filters() arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/filters.tmp", home_dir);
filters_dat_file = fopen(pathfilename, "w");
if(! filters_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 
for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(filters_dat_file,\
	"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",\
	pa -> name,
	pa -> postings_flag,
	pa -> articles_flag,
	pa -> header_flag,
	pa -> header_from_flag,
	pa -> header_to_flag,
	pa -> header_subject_flag,
	pa -> header_newsgroups_flag,
	pa -> header_others_flag,
	pa -> body_flag,
	pa -> attachment_flag,
	pa -> groups_flag,

	pa -> case_flag,
	pa -> mode,
	pa -> allow_flag,
	pa -> only_flag,

	pa -> group_show_flag,
	pa -> header_show_flag,
	pa -> body_show_flag,
	pa -> attachment_show_flag,
	pa -> browser_line); /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
	fprintf(filters_dat_file, "%s\n", pa -> keywords);
	fprintf(filters_dat_file, "%s\n", pa -> groups_keywords);
	}/* end while all elements in list */
fclose(filters_dat_file);

if(debug_flag)
	{
	fprintf(stdout, "save_filters(): backing up filters.dat\n");
	}
/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/filters.dat", home_dir);
sprintf(pathfilename2, "%s/.NewsFleX/filters.dat~", home_dir);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_filters(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/filters.tmp", home_dir);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_filters(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);/* ok */
}/* end function save_filters */


int show_filter_form()
{
char *ptr;
int line;
char *crypt();
char *ptr2;

if(debug_flag)
	{
	fprintf(stdout, "show_filter_form(): arg none\n");
	}
	
ptr = (char *) fl_show_input("Password?", "");
if(! ptr) return(0); /* cancel pressed */

#ifndef NO_CRYPT
ptr2 = (char *) crypt(ptr, "P8");
#else
ptr2 = ptr;
#endif

if(strcmp(filter_password, ptr2) != 0) return(0);

/*load_filters();*/

/* in case no filters, avoid NULL pointers */
fl_set_input(fdui -> filter_keywords_input_field, "");
fl_set_input(fdui -> filter_groups_input_field, "");

get_first_filter(&selected_filter);
if(selected_filter != -1)
	{
	get_filter(selected_filter);
	}

show_filters();

filter_id_to_line(selected_filter, &line);
fl_select_browser_line(fdui -> filter_list_browser, line);

filter_modified_flag = 0;
filter_enable_modified_flag = 0;
fl_show_form(fd_NewsFleX -> filter, FL_PLACE_CENTER, FL_NOBORDER, "");
return(1);
}/* end function show_filter */


int show_filters()
{
char temp[TEMP_SIZE];
struct filter *pa;
char formatstr[20];
int browser_line;
int browser_topline;
char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "show_filters(): arg none\n");
	}

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> filter_list_browser);

fl_freeze_form(fd_NewsFleX -> filter);

fl_clear_browser(fdui -> filter_list_browser);

browser_line = 1;
for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	strcpy(formatstr, "@f");
	strcat(formatstr, "@C0");/* black */

	sprintf(temp, "%s %s (%s)",\
	formatstr, pa -> keywords, pa -> groups_keywords);

	fl_add_browser_line(fdui -> filter_list_browser, temp);
	pa -> browser_line = browser_line;/* first one is 1 */
	browser_line++;
	}/* end for all elements in list */
	
/* re adjust browser for same position of top line (vertical slider) */	
fl_set_browser_topline(fdui -> filter_list_browser, browser_topline);

fl_unfreeze_form(fd_NewsFleX -> filter);
return(1);
}/* end function show_filters */


int line_to_filter_id(int line, long *filter_id)
{
struct filter *pa;
extern long atol();

if(debug_flag)
	{
	fprintf(stdout, "line_to_filter_id(): arg line=%d\n", line);
	}

/* argument check */
if(line < 0) return(0);

for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		*filter_id = atol(pa -> name);
		return(1);
		}
	}

/* no save_filters, nothing was changed */
return(0);
}/* end function line_to_filter_id */


int set_filter(long filter_id)
{
struct filter *pa, *lookup_filter();
char *ptr;
char *strsave();
char name[50];
int and, not, only, case_sens;

if(debug_flag)
	{
	fprintf(stdout, "set_filter arg filter_id=%ld\n", filter_id);
	}

sprintf(name, "%ld", filter_id);
for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0)
		{
		pa -> postings_flag =\
		fl_get_button(fdui -> filter_postings_button);
		pa -> articles_flag =\
		fl_get_button(fdui -> filter_articles_button);
		pa -> header_flag =\
		fl_get_button(fdui -> filter_headers_button);
		pa -> header_from_flag =\
		fl_get_button(fdui -> filter_headers_from_button);
		pa -> header_to_flag =\
		fl_get_button(fdui -> filter_headers_to_button);
		pa -> header_subject_flag =\
		fl_get_button(fdui -> filter_headers_subject_button);
		pa -> header_newsgroups_flag =\
		fl_get_button(fdui -> filter_headers_newsgroups_button);
		pa -> header_others_flag =\
		fl_get_button(fdui -> filter_headers_others_button);
		pa -> body_flag =\
		fl_get_button(fdui -> filter_bodies_button);
		pa -> attachment_flag =\
		fl_get_button(fdui -> filter_attachments_button);
		pa -> groups_flag =\
		fl_get_button(fdui -> filter_groups_button);

		case_sens = fl_get_button(fdui -> filter_case_sensitive_button);
		and = fl_get_button(fdui -> filter_keywords_and_button);
		not = fl_get_button(fdui -> filter_keywords_not_button);
		only = fl_get_button(fdui -> filter_keywords_only_button);

		pa -> mode = (case_sens << 3) + (only << 2) + (not << 1) + and;

		pa -> allow_flag =\
		fl_get_button(fdui -> filter_allow_button);

		pa -> only_flag = 0;
		pa -> case_flag = 0;

		pa -> group_show_flag =\
		fl_get_button(fdui -> filter_groups_show_button);
		pa -> header_show_flag =\
		fl_get_button(fdui -> filter_headers_show_button);
		pa -> body_show_flag =\
		fl_get_button(fdui -> filter_bodies_show_button);
		pa -> attachment_show_flag =\
		fl_get_button(fdui -> filter_attachments_show_button);

		free(pa -> keywords);
		ptr = (char *) fl_get_input(fdui -> filter_keywords_input_field);
		if(ptr) pa -> keywords = strsave(ptr);
		else  pa -> keywords = strsave("");
		if(! pa -> keywords) return(0);

		free(pa -> groups_keywords);
		ptr = (char *) fl_get_input(\
		fdui -> filter_groups_input_field);
		if(ptr) pa -> groups_keywords = strsave(ptr);
		else  pa -> groups_keywords = strsave("");
		if(! pa -> keywords) return(0);

		return(1);
		}
	}/* end for all structures */
return(0);/* not found */
}/* end function set_fiter_flags */


int get_new_filter_id(long *filter_id)
{
struct filter *pa;
long li;
char temp[50];

if(debug_flag)
	{
	fprintf(stdout, "get_new_filter_id(): arg none\n");
	}

for(li = 0; li < MAX_FILTERS; li++)
	{
	sprintf(temp, "%ld", li);
	pa = lookup_filter(temp);/* place was free */
	if(! pa)	
		{
		*filter_id = li;
		return(1);
		}
	}

if(debug_flag)
	{
	fprintf(stdout, "get_new_filter_id(): arg *filter_id set to %ld\n",\
	*filter_id);
	}	

return(0);/* no free space */
}/* end function get_new_filter_id */


int get_filter(long filter_id)
{
struct filter *pa, *lookup_filter();
char name[50];

if(debug_flag)
	{
	fprintf(stdout, "get_filter(): arg filter_id=%ld\n", filter_id);
	}

sprintf(name, "%ld", filter_id);
for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0)
		{
		fl_set_button(\
		fdui ->  filter_postings_button, pa -> postings_flag);
		fl_set_button(\
		fdui ->  filter_articles_button, pa -> articles_flag);
		fl_set_button(\
		fdui ->  filter_headers_button, pa -> header_flag);
		fl_set_button(\
		fdui ->  filter_headers_from_button, pa -> header_from_flag);
		fl_set_button(\
		fdui ->  filter_headers_to_button, pa -> header_to_flag);
		fl_set_button(\
		fdui ->  filter_headers_subject_button, pa -> header_subject_flag);
		fl_set_button(\
		fdui ->  filter_headers_newsgroups_button,\
		pa -> header_newsgroups_flag);
		fl_set_button(\
		fdui ->  filter_headers_others_button, pa -> header_others_flag);
		fl_set_button(\
		fdui ->  filter_bodies_button, pa -> body_flag);
		fl_set_button(\
		fdui ->  filter_attachments_button, pa -> attachment_flag);
		fl_set_button(\
		fdui ->  filter_groups_button, pa -> groups_flag);

		fl_set_button(\
		fdui -> filter_keywords_and_button, (pa -> mode) & 1);
		fl_set_button(\
		fdui -> filter_keywords_not_button, ( (pa -> mode) & 2) >> 1);
		fl_set_button(\
		fdui -> filter_keywords_only_button, ( (pa -> mode) & 4) >> 2);
		fl_set_button(\
		fdui -> filter_case_sensitive_button, ( (pa -> mode) & 8) >> 3);

		fl_set_button(\
		fdui ->  filter_allow_button, pa -> allow_flag);
		if(pa -> allow_flag)
			{
			fl_set_object_label(\
			fdui ->  filter_allow_button, "ALLOW");
			}
		else
			{
			fl_set_object_label(\
			fdui ->  filter_allow_button, "DENY");
			}

		fl_set_button(\
		fdui ->  filter_groups_show_button, pa -> group_show_flag);
		fl_set_button(\
		fdui ->  filter_headers_show_button, pa -> header_show_flag);
		fl_set_button(\
		fdui ->  filter_bodies_show_button, pa -> body_show_flag);
		fl_set_button(\
		fdui ->  filter_attachments_show_button,\
		pa -> attachment_show_flag);

		fl_set_input(\
		fdui -> filter_keywords_input_field, pa -> keywords);

		fl_set_input(\
		fdui -> filter_groups_input_field, pa -> groups_keywords);

		return(1); 
		}
	}/* end for all structures */
return(0);/* not found */
}/* end function set_fiter_flags */


int get_first_filter(long *filter_id)
{
struct filter *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_first_filter(): arg *filter_id\n");
	}

pa = filtertab[0];
if(pa)
	{
	*filter_id = atol(pa -> name);
	return(1);
	}
*filter_id = -1;
return(1);
}/* end function get_first_filter */


int add_filter(long filter_id)
{
struct filter *pa, *install_filter_at_end_off_list();
char name[50];

if(debug_flag)
	{
	fprintf(stdout, "add_filter(): arg filter_id=%ld\n", filter_id);
	}

sprintf(name, "%ld", filter_id);
pa = install_filter_at_end_of_list(name);
if(!pa) return(0);
return(1);
}/* end function add_filter */


int filter_id_to_line(long filter_id, int *line)
{
struct filter *pa;

if(debug_flag)
	{
	fprintf(stdout, " filter_id_to_line(): arg filter_id=%ld *line\n",\
	filter_id);
	}

for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(atol(pa -> name) == filter_id)
		{
		*line = pa -> browser_line;
		return(1);
		}
	}


return(0);/* not found */
}/* end function filter_id_to_line */


int apply_filter_pa_to(\
struct filter *pa, char *group, char *article, int encoding,\
char *attachment, int have_body_flag, int filter_what, int *results)
{
int a, c, i;
char temp[READSIZE];
FILE *filter_filefd;
int iptr;
char path[TEMP_SIZE];
int crossposts_allowed, crossposts_cnt;

if(debug_flag)
	{
	fprintf(stdout,\
	"apply_filter_pa_to(): arg pa=%ld group=%s article=%s\n\
	encoding=%d attachment=%s have_body_flag=%d filter_what=%d\n",\
	(long) pa, group, article, encoding, attachment, have_body_flag,\
	filter_what);
	}

/* argument check */
if(! pa) return(0);
if(! group) return(0);
if(! article) return(0);	
if(strlen(group) == 0) return(0);
if(strlen(article) == 0) return(0);
if( (filter_what != SEARCH_POSTINGS) &&\
(filter_what != SEARCH_ARTICLES) &&\
(filter_what != SEARCH_GROUPS) )
	{
	return(0);
	}

/*
	char *keywords;
	char *groups_keywords;
	int postings_flag;
	int articles_flag;
	int header_flag;
	int header_from_flag;
	int header_to_flag;
	int header_subject_flag;
	int header_newsgroups_flag;
	int header_others_flag;
	int body_flag;
	int attachment_flag;
	int case_flag;
	int header_store_flag;
	int header_show_flag;
	int body_store_flag;
	int body_show_flag;
	int attachment_store_flag;
	int attachment_show_flag;
*/

/* 
Whatever we do, we read a file line by line, and check against
filter_keywords.
*/

/* no match yet */
*results = FILTER_MATCH_IN_NONE;

/*if(strlen(pa -> keywords == 0) return(1);*/

if(filter_what == SEARCH_GROUPS)
	{
	if(pa -> groups_flag)
		{
		if(filter_search_in_for(group, pa -> keywords, pa -> mode) )
			{
			*results |= FILTER_MATCH_IN_GROUP;
			return(1);
			}
		}
	return(1);
	}

if(filter_what == SEARCH_ARTICLES)
	{
	if(strlen(pa -> groups_keywords) != 0)
		{
		/* test if match search_groups_keywords for this group
		*/
		/* Note: search (not filter search) case insensitive
		*/
		if(! search_in_for(group, pa -> groups_keywords, 1) )
			{
			/* scip the group */
			if(pa -> allow_flag) *results |= FILTER_MATCH_IN_GROUP;;
								/* will be reversed to a 0 if allow flag */
			return(1);
			}
		}
	sprintf(path, "%s/.NewsFleX/%s/%s/%s/",\
	home_dir, news_server_name, database_name, group);
	}

if(filter_what == SEARCH_POSTINGS)
	{
	sprintf(path, "%s/.NewsFleX/postings/%s/",\
	home_dir, postings_database_name);
	}

if(pa -> header_flag)/* if test headers */
	{
	sprintf(temp, "%shead.%s", path, article);
	filter_filefd = fopen(temp, "r");
	if(!filter_filefd)
		{
		fl_show_alert("apply_filter_pa_to(): Cannot open", temp, "for read", 0);
		}		
	else
		{
		while(1)/* for all lines in the file */
			{
			a = readline(filter_filefd, temp);
			if(a == EOF) break;

			if(pa -> header_from_flag)
				{
				if(strstr(temp, "From:") == temp)
					{
					if(filter_search_in_for(\
					temp, pa -> keywords, pa -> mode) )
						{
						*results |= FILTER_MATCH_IN_HEADER;
						fclose(filter_filefd);
						break;
						}
					}
				}
			if(pa -> header_to_flag)
				{
				if(strstr(temp, "To:") == temp)
					{
					if(filter_search_in_for(\
					temp, pa -> keywords, pa -> mode) )
						{
						*results |= FILTER_MATCH_IN_HEADER;
						fclose(filter_filefd);
						break;
						}
					}
				}
			if(pa -> header_subject_flag)
				{
				if(strstr(temp, "Subject:") == temp)
					{
					if(filter_search_in_for(\
					temp, pa -> keywords, pa -> mode) )
						{
						*results |= FILTER_MATCH_IN_HEADER;
						fclose(filter_filefd);
						break;
						}
					}
				}
			if(pa -> header_newsgroups_flag)
				{
				if(strstr(temp, "Newsgroups:") == temp)
					{
					/* a special case for crosspost limit */
					if( *(pa -> keywords) == '$')
						{
						crossposts_allowed =\
						atoi(strstr(pa -> keywords, "$") + 1);
						crossposts_cnt = 0;
						i = 0;
						while(1)
							{
							c = temp[i];
							if(!c) break;
							if(c == ',') crossposts_cnt++;
							i++;
							}/* end while all characters */
						if(crossposts_cnt > crossposts_allowed)
							{
							*results |= FILTER_MATCH_IN_HEADER;
							fclose(filter_filefd);
							return(1);
							}
						}/* end if temp[0] == '$' */

					if(filter_search_in_for(\
					temp, pa -> keywords, pa -> mode) )
						{
						*results |= FILTER_MATCH_IN_HEADER;
						fclose(filter_filefd);
						break;
						}
					}
				}
			if(pa -> header_others_flag)
				{
				if(strstr(temp, "From:") == 0)
					{
					if(strstr(temp, "To:") ==  0)
						{
						if(strstr(temp, "Subject:") ==  0)
							{		
							if(strstr(temp, "Newsgroups:") ==  0)
								{
								if(filter_search_in_for(strstr(temp, ": "),\
								pa -> keywords, pa -> mode) )
									{
									*results |= FILTER_MATCH_IN_HEADER;
									fclose(filter_filefd);
									break;
									}
								}/* end no newsgroups */
							}/* end no subject */
						}/* end no to */
					}/* end no from */
				}/* end if filter_other_headers_flag */
			}/* end while all lines in file */
		}/* end if filter headers flag */
	}/* end else file opened ok */
/* what do you mean 'so many brackets' ? */

/* here for articles and postings */
if(pa -> body_flag)
	{
	if(have_body_flag)
		{
		sprintf(temp, "%sbody.%s", path, article);
		filter_filefd = fopen(temp, "r");
		if(! filter_filefd)
			{
			fl_show_alert("Cannot open", temp, "for read", 0);
			}		
		else
			{
			while(1)/* for all lines in the file */
				{
				a = readline(filter_filefd, temp);
				if(a == EOF) break;
				if(filter_search_in_for(temp, pa -> keywords, pa -> mode) )
					{
					*results |= FILTER_MATCH_IN_BODY;
					fclose(filter_filefd);
					/*continue;*/
					break;
					}
				}/* end while all lines in file */
			}/* end if file opened OK */
		}/* end if have_body_flag */
	}/* end if filter_bodies_flag */

if(pa -> attachment_flag && (strlen(attachment) != 0) )
	{
	/*
	This will allow a filter for the string(s) in data in the possibly
	binary.
	*/
	if(filter_what == SEARCH_ARTICLES)
		{
		/*
		the location in the .NewsFleX/news_server_name/database_name/group/
		*/
		sprintf(temp, "%s%s", path, attachment);
		}
	if(filter_what == SEARCH_POSTINGS)
		{
		/* the actual path/filename on this machine */
		sprintf(temp, "%s", attachment);
/* XXXXXX MAYBE THIS SHOULD BE IN THE POSTINGS DIR */
		}
			
	if(debug_flag) fprintf(stdout, "filtering attachment %s\n", temp);
	filter_filefd = fopen(temp, "r");
	if(! filter_filefd)
		{
		fl_show_alert("Cannot open", temp, "for read", 0);
		}		
	else
		{
		/* NOTE: file may be BINARY, so readline will fail */
		/*
		now gone split in small 0 terminated blocks, so I can use
		filter_search_in_for()
		*/
		while(1)/* for all characters in the file */
			{
			iptr = 0;
			errno = 0;
			while(1)/* read characters from stream */
				{
				c = getc(filter_filefd);

				a = ferror(filter_filefd);
				if(a)
					{
	 				if(debug_flag)
	 					{
	 					perror("\napply_filter_pa_to(): read failed");
						}
					continue; /* read failed */
					}

				if(feof(filter_filefd) ) 
					{
					fclose(filter_filefd);
					break;/* EOF */
					}

				temp[iptr] = c;
				if(c == 0) break; /* use this as string */
				iptr++;
				if(iptr >= READSIZE) break;
				}/* end while read a character */ 

			if(feof(filter_filefd) ) break; /* EOF */

			temp[iptr] = 0;/* add a string termination, 
								so filter_search_in_for() can work */
			if(filter_search_in_for(\
			temp, pa -> keywords, pa -> mode) )
				{
				*results |= FILTER_MATCH_IN_ATTACHMENT;
				break;
				}
			}/* end while all parts of file */
		}/* end if attachment file exists */
	}/* end if filter_attachments_flag */
return(1);
}/* end function apply_filter_pa_to */


int get_filter_flags(\
char *group, char *article, int encoding, char *attachment,\
int have_body_flag, int in_what, int *filter_flags)
{
struct filter *pa;
int results;

if(debug_flag)
	{
	fprintf(stdout,\
	"get_filter_flags(): arg group=%s article=%s encoding=%d \
	attachment=%s have_body_flag=%d in_what=%d\n",\
	group, article, encoding, attachment, have_body_flag, in_what);
	}

/* argument check */
if(! group) return(0);
if(! article) return(0);
if(! attachment) return(0);

*filter_flags = 0;
for(pa = filtertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* test if filter active for this type */
	if( (in_what == SEARCH_ARTICLES) && (!(pa -> articles_flag) ) ) continue;
	if( (in_what == SEARCH_POSTINGS) && (!(pa -> postings_flag) ) ) continue;
	if( (in_what == SEARCH_GROUPS) && (!(pa -> groups_flag) ) )continue;
	
	apply_filter_pa_to(\
	pa, group, article, encoding, attachment, have_body_flag, in_what,\
	&results);

	/* if allow, reverse result */
	if(pa -> allow_flag)
		{
		if(results) results = 0;
		else results = 1;
		}
		
	if(results)/* found some match somewhere */
		{
		if(pa -> group_show_flag)
			{
			*filter_flags |= FILTER_DO_NOT_SHOW_GROUP;
			}
		if(pa -> header_show_flag)
			{
			*filter_flags |= FILTER_DO_NOT_SHOW_HEADER;
 			}
 		if(pa -> body_show_flag)
 			{
			*filter_flags |= FILTER_DO_NOT_SHOW_BODY;
 			}	
 		if(pa -> attachment_show_flag)
 			{
			*filter_flags |= FILTER_DO_NOT_SHOW_ATTACHMENT;
 			}
		}/* end if results */
	}/* end for all filters in list */

if(debug_flag)
	{
	fprintf(stdout,\
	"get_filter_flags(): returning *filter_flags=%d\n",\
	*filter_flags);
	}

return(1);
}/* end function get_filter_flags */


int filter_search_in_for(char *database, char *data, int mode)
{
char *ptra, *ptrb, *ptrc;
int match_failed_flag;
char *strsave();
char *strstr_case_insensitve();
int case_sensitive, and, not, only;

if(debug_flag)
	{
	fprintf(stdout,\
	"filter_search_in_for(): arg database=%s data=%s mode=%d\n",\
	database, data, mode);
	}

/* argument check */
if(! database) return(0);
if(! data) return(0);

/*
Test if some data,
data may consist of one or more words separated by spaces
*/
if(! data[0]) return(0);/* no data */

and = mode & 1;
not = (mode & 2) >> 1;
only = (mode & 4) >> 2;
case_sensitive = (mode & 8) >> 3;

if(only)
	{
	/* test if data base and data are indentical */
	if(case_sensitive)
		{
		if(strcmp(database, data) == 0) return(1);
		}
	else
		{
		if(strcmp_case_insensitive(database, data) == 0) return(1);
		}/* end if not case sensitive */
	return(0);
	}/* end if only */
		
if(and)
	{
	/* test if all fields in data are present in database */
	/* for all entries in data */
	ptra = data;
	ptrb = strsave(data);/* create space */
	match_failed_flag = 0;
	while(1)
		{
		/* extract a field from data */	
		ptrc = ptrb;/* point to start availeble space */
		while(1)/* string in data to ptrb */
			{
			*ptrc = *ptra;
			if(! *ptra) break;/* end data */
			if(*ptra == ' ')/* end of string in data */
				{
				*ptrc = 0;
				ptra++;/* scip the space */
				break;
				}
			ptra++;
			ptrc++;
			}/* end while all characters in string */
		/* ok, we have an field from data */	
	
		/* test if string present in database */
		if(case_sensitive)
			{
			if(! strstr(database, ptrb) )
				{
				match_failed_flag = 1;
				break;
				}
			}
		else
			{
			if(! strstr_case_insensitive(database, ptrb) )
				{
				match_failed_flag = 1;
				break;
				}
			}/* end if not case sensitive */
		if(! *ptra) break;/* end filter */
		}/* end while all fields in data */
	free(ptrb);/* release space */
	if(match_failed_flag) return(0);
	return(1);
	}/* and if and */

if(not)
	{
	/* test if any field in data is present present in database */
	/* for all entries in data */
	ptra = data;
	ptrb = strsave(data);/* create space */
	while(1)
		{
		/* extract a field from data */	
		ptrc = ptrb;/* point to start availeble space */
		while(1)/* string in data to ptrb */
			{
			*ptrc = *ptra;
			if(! *ptra) break;/* end data */
			if(*ptra == ' ')/* end of string in data */
				{
				*ptrc = 0;
				ptra++;/* scip the space */
				break;
				}
			ptra++;
			ptrc++;
			}/* end while all characters in string */
		/* ok, we have an field from data */	
	
		/* test if string present in database */
		if(case_sensitive)
			{
			if(strstr(database, ptrb) )
				{
				free(ptrb);
				return(1);/* found */
				}
			}
		else
			{
			if(strstr_case_insensitive(database, ptrb) )
				{
				free(ptrb);
				return(1);/* found */
				}
			}/* end if not case sensitive */
		if(! *ptra) break;/* end filter */
		}/* end while all fields in data */
	free(ptrb);/* release space */
	return(0);/* not found */
	}/* end if not */
return(0);/* just for Wall */
}/* end function filter_search_in_for */


int strcmp_case_insensitive(char *s, char *t)
{
char *p1, *p2;

p1 = s;
p2 = t;
while(1)
	{
	if(toupper(*p1) != toupper(*p2) ) return(1);/* differ */
	if(! (*p1) ) break;
	p1++;
	p2++;
	}
return(0);/* equal */
}/* end function strcmp_case_insensitive */



int set_new_data_flag(int val)
{
char temp[TEMP_SIZE];
FILE *filefd;

if(debug_flag)
	{
	fprintf(stdout, "set_new_data_flag(): arg val=%d\n", val);
	}

sprintf(temp, "%s/.NewsFleX/setup/new_data", home_dir);
filefd = fopen(temp, "w");
if(! filefd)
	{
	fl_show_alert(\
	"set_new_data_flag(): cannot open file for write", temp, "", 0); 
	if(debug_flag)
		{
		fprintf(stdout,\
		"set_new_data_flag(): cannot open %s for write", temp);
		}
	return(0);
	}

fprintf(filefd, "%d", val);
fclose(filefd);
return(1);
}/* end function set_new_data_flag */


int get_new_data_flag(int *val)
{
char temp[TEMP_SIZE];
FILE *filefd;

if(debug_flag)
	{
	fprintf(stdout, "get_new_data_flag(): arg none\n");
	}

sprintf(temp, "%s/.NewsFleX/setup/new_data", home_dir);
filefd = fopen(temp, "r");
if(! filefd)
	{
	fl_show_alert(\
	"get_new_data_flag(): cannot open file for read", temp, "", 0); 
	if(debug_flag)
		{
		fprintf(stdout,\
		"get_new_data_flag(): cannot open %s for read", temp);
		}
	return(0);
	}

fscanf(filefd, "%d", val);/* val already a pointer */
fclose(filefd);
return(1);
}/* end function get_new_data_flag */


