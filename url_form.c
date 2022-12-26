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


int last_found_line;

struct url
	{
	char *name;/* this is the url */
	char *description;
	char *group;
	long article;
	int status;
	int browser_line;	
	time_t last_updated_time;
	struct url *nxtentr;
	struct url *prventr;
	};
struct url *urltab[2]; /* first element points to first entry,
			second element to last entry */


struct url *lookup_url(char *name)
{
struct url *pa;

/*pa points to next entry*/
for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_url */


struct url *install_url_at_end_of_list(char *name)
{
struct url *plast, *pnew;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_url_at_end_off_list(): arg name=%s\n", name);
	}

pnew = lookup_url(name);
if(pnew)
	{
	/* free previous definition */
/*	free(pnew -> subject);*/
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct url *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = urltab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !urltab[0] ) urltab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
urltab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_url */


int delete_url(char *name)/* delete entry from double linked list */
{
struct url *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_url(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = urltab[0];
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
	/* if first one, modify urltab[0] */
	if(pprev == 0) urltab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify urltab[1] */
	if(pnext == 0) urltab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel -> description);
	free(pdel -> group);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_url */


int delete_all_urls()/* delete all entries from table */
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_urls() arg none\n");
	}

while(1)
	{	
	pa = urltab[0];
	if(! pa) break;
	urltab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa -> description);
	free(pa -> group);
	free(pa);/* free structure */
	}/* end while all structures */

urltab[1] = 0;
return(1);
}/* end function delete_all_urls */


int load_urls()
{
int a;
FILE *urls_dat_file;
char temp[READSIZE];
char pathfilename[TEMP_SIZE];
extern long atol();
struct url *pa;
extern char *strsave();
int first_url_flag;
char name[TEMP_SIZE];
char description[TEMP_SIZE];
char group[TEMP_SIZE];
long article;
int status;
int browser_line; /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
int status_line;
time_t last_updated_time;

if(debug_flag)
	{
	fprintf(stdout, "load_urls() arg none using selected_url=%s\n",\
	selected_url);
	}
	
delete_all_urls();

sprintf(pathfilename, "%s/.NewsFleX/urls.dat", home_dir);
urls_dat_file = fopen(pathfilename, "r");
if(! urls_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not load file %s\n", pathfilename);
		}
	return(0);
	} 
status_line = 1;
first_url_flag = 1;
while(1)
	{
	a = readline(urls_dat_file, temp);
	if(a == EOF) return(1);/* empty file or last line */
	/*
	Try to stay compatible with any old versions that do not have these
	flags. 
	*/
	if(status_line)
		{
		external_browser_flag = 0;
		browse_online_flag = 0;
		url_form_show_marked_only_flag = 0;
		sscanf(temp, "%d %d %d",\
		&external_browser_flag,\
		&browse_online_flag,\
		&url_form_show_marked_only_flag);

		fl_set_button(fdui -> url_form_external_browser_button,\
		external_browser_flag);
		if(external_browser_flag)
			{
			fl_set_object_label(fdui -> url_form_external_browser_button,\
			"EXT BROWSER");
			}
		else
			{
			fl_set_object_label(fdui -> url_form_external_browser_button,\
			"INT BROWSER");
			}

		fl_set_button(fdui -> url_form_browse_online_button,\
		browse_online_flag);
		if(browse_online_flag)
			{
			fl_set_object_label(fdui -> url_form_browse_online_button,\
			"BR ONLINE");
			}
		else
			{
			fl_set_object_label(fdui -> url_form_browse_online_button,\
			"BR OFFLINE");
			}

		fl_set_input(fdui -> url_form_depth_input_field, "sel.");

		fl_set_button(fdui -> url_form_show_marked_only_button,\
		url_form_show_marked_only_flag);
		if(url_form_show_marked_only_flag)
			{
			fl_set_object_label(fdui -> url_form_show_marked_only_button,\
			"MARKED ONLY");
			}
		else
			{
			fl_set_object_label(fdui -> url_form_show_marked_only_button,\
			"SHOW ALL");
			}	

		status_line = 0;
		continue;
		}/* end if status line */
	sscanf(temp,\
	"%s %d %d %ld %ld",\
	name,\
	&status,\
	&browser_line, /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
	&article,\
	&last_updated_time);

	a = readline(urls_dat_file, description);
	if(a == EOF) return(0);/* last line, or file format error */

	a = readline(urls_dat_file, group);
	if(a == EOF) return(0);/* file format error */
	
	/* create a structure entry */
	pa = install_url_at_end_of_list(name);
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"load_urls: cannot install url %s\n", temp);
			}
		return(0);
		}

	pa -> status = status;
	pa -> browser_line = browser_line;
	pa -> article = article;	

	pa -> description = strsave(description);
	if(! pa -> description) return(0);

	pa -> group = strsave(group);
	if(! pa -> group) return(0);
	
	pa -> last_updated_time = last_updated_time;

	if(first_url_flag)
		{
		selected_url = strsave(name);
		first_url_flag = 0;
		}
	}/* end while all lines in urls.dat */

return(1);
}/* end function load urls */


int save_urls()
{
FILE *urls_dat_file;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct url *pa;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "save_urls() arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/urls.tmp", home_dir);
urls_dat_file = fopen(pathfilename, "w");
if(! urls_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 
fprintf(urls_dat_file, "%d %d %d\n",\
external_browser_flag, browse_online_flag, url_form_show_marked_only_flag);
for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(urls_dat_file, "%s %d %d %ld %ld\n",\
	pa -> name,\
	pa -> status,\
	pa -> browser_line, /* used to lookup name (= id) if clicked in display,
							not saved on disk, assigned each time list
							is displayed */
	pa -> article,\
	pa -> last_updated_time);

	fprintf(urls_dat_file, "%s\n", pa -> description);
	fprintf(urls_dat_file, "%s\n", pa -> group);
	
	}/* end while all elements in list */

fclose(urls_dat_file);

if(debug_flag)
	{
	fprintf(stdout, "save_urls(): backing up urls.dat\n");
	}
/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/urls.dat", home_dir);
sprintf(pathfilename2, "%s/.NewsFleX/urls.dat~", home_dir);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_urls(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/urls.tmp", home_dir);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_urls(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);/* ok */
}/* end function save_urls */


int show_url_form()
{
int line;

if(debug_flag)
	{
	fprintf(stdout, "show_url_form(): arg none\n");
	}

load_urls();

/* in case no urls, avoid NULL pointers */
fl_set_input(fdui -> url_form_input_field, "");

show_urls(SHOW_UNREAD_AT_TOP);/* SHOW_BOTTOM */

url_to_line(selected_url, &line);
fl_select_browser_line(fdui -> url_form_browser, line);

fl_show_form(fd_NewsFleX -> url_form, FL_PLACE_CENTER, FL_UP_FRAME, "");
return(1);
}/* end function show_url_form */


int show_urls(int position)
{
char temp[TEMP_SIZE];
struct url *pa;
char formatstr[20];
int browser_line;
int browser_topline;
char *strsave();
int first_new_url_line;
int find_first_new_url_flag;
int maxline;
int a;

if(debug_flag)
	{
	fprintf(stdout, "show_urls(): arg position=%d\n", position);
	}

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> url_form_browser);

fl_freeze_form(fd_NewsFleX -> url_form);

fl_clear_browser(fdui -> url_form_browser);

first_new_url_line = -1;/* keep gcc -Wall from complaining */
find_first_new_url_flag = 1;
browser_line = 1;
for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(url_form_show_marked_only_flag)
		{
		if(! (pa -> status & RETRIEVE_URL) )
			{
			/* to prevent false finds in double click mark not existing */
			pa -> browser_line = 0;
			continue;
			}
		}

	if(position == SHOW_UNREAD_AT_TOP)
		{
		/* want to show the first unaccessed at top of display */
		if(find_first_new_url_flag)
			{
			if(pa -> status & NEW_URL)
				{
				first_new_url_line = browser_line;
				find_first_new_url_flag = 0;
				}
			}
		}
	
	strcpy(formatstr, "@f");
	/* red is default color */
	strcat(formatstr, "@C1"); /* red */
	if(pa -> status & ACCESSED_URL) strcat(formatstr, "@C0"); /* black */
	if(pa -> status & FAILED_URL) strcat(formatstr, "@C18"); /* dark green */
	if(pa -> status & HAVE_URL) strcat(formatstr, "@C4"); /* blue */
	if(pa -> status & RETRIEVE_URL) strcat(formatstr, "@C5"); /* magenta */
	
	if(highlight_urls(pa) ) strcat(formatstr, "@_");/* underscore */
	
	sprintf(temp, "%s %s (%s) %s %ld %s",\
	formatstr, pa -> name, pa -> description, pa -> group, pa -> article,\
	ctime(&pa -> last_updated_time) );
	/* Note ctime prints \n, do not want that */
	a = strlen(temp);	
	/* cut off \n */
	temp[a - 1] = 0; 
	
	fl_add_browser_line(fdui -> url_form_browser, temp);
	pa -> browser_line = browser_line;/* first one is 1 */
	browser_line++;
	}/* end for all elements in list */
	
/* re adjust browser for same position of top line (vertical slider) */	
if(position == SHOW_UNREAD_AT_TOP)
	{
	fl_set_browser_topline(fdui -> url_form_browser, first_new_url_line);
	}
if(position == SHOW_BOTTOM)
	{
	/* make the status of the last url visible */
	maxline = fl_get_browser_maxline(fdui -> url_form_browser);
	fl_set_browser_topline(fdui -> url_form_browser, maxline);
	}
/*
if(position == SHOW_TOP)
	{
	fl_set_browser_topline(fdui -> url_form_browser, 1);
	}
*/
if(position == SHOW_SAME_POSITION)
	{
	fl_set_browser_topline(fdui -> url_form_browser, browser_topline);
	}	

fl_unfreeze_form(fd_NewsFleX -> url_form);
return(1);
}/* end function show_urls */


char *line_to_url(int line)
{
struct url *pa;
extern long atol();

if(debug_flag)
	{
	fprintf(stdout, "line_to_url(): arg line=%d\n", line);
	}

/* argument check */
if(line < 0) return(0);

for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		return(pa -> name);
		}
	}

/* no save_urls, nothing was changed */
return(0);
}/* end function line_to_url_id */


int start_browser(char *url)
{
FILE *exec_fd;
char temp[TEMP_SIZE];
int a;
int topline;
int add_to_history;

if(debug_flag)
	{
	fprintf(stdout, "start_browser(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

if(! external_browser_flag)
	{
	/* start the local browser */
	a = http_browser(url, &add_to_history, 0);
	if(a)
		{
		/* do not add to history if image or some application */
		if(add_to_history)
			{
			/* for the BACK button, remember where we are */
			topline = fl_get_input_topline(fdui -> article_body_input_field);
			add_to_url_history(url, topline);
			}/* end if add_to_history */
		}/* end if http_browser ok */
	
	}/* end if internal browser */
else /* start some browser */
	{
	/* test if any external browser specified */
	if(! browser_get_url_command) return(0);

	/*netscape -remote "openURL(ftp://localhost/pub/)"*/

	replace(browser_get_url_command, "$url", url, temp);

	/*
	Show user that browser is activated by making the browser command input 
	field red
	*/
	fl_set_object_color(\
	fdui -> url_form_browser_command_input_field, FL_COL1, FL_RED);

	XSync(fl_get_display(), 0);

	/*fprintf(stdout, "popen temp=%s\n", temp);*/

	exec_fd = popen(temp, "r");
	if(! exec_fd)
		{
		fl_show_alert("Cannot execute file:", temp, "", 0);
		return(0);
		}
	pclose(exec_fd);

	/* set browser command input to normal color again */
	fl_set_object_color(\
	fdui -> url_form_browser_command_input_field, FL_COL1, FL_WHITE);
	}/* end if external browser */

return(1);
}/* end function start_browser */


int url_to_line(char *url, int *line)
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout, "url_to_line(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

pa = lookup_url(url);
if(! pa) return(0);

*line = pa -> browser_line;
return(1);
}/* end function url_to_line */


int add_url(char *text, char *group, long article_id)
{
char url[TEMP_SIZE];
struct url *pa;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_url(): arg text=%s group=%s article_id=%ld",\
	text, group, article_id);
	}

/* argument check */
if(! text) return(0);
if(! group) return(0);
if(article_id < 0) return(0);

if(sscanf(text, "%s", url) < 1) return(0);

pa = install_url_at_end_of_list(url);
if(! pa) return(0);

ptr = strstr(text, " ");
if(! ptr) /* no comment */
	{
	pa -> description = strsave("");
	}
else pa -> description = strsave(ptr + 1);
if(! pa -> description) return(0);

pa -> group = strsave(group);
if(! pa -> group) return(0);

pa -> article = article_id;

pa -> status = NEW_URL;/* this will reset retrieve, accessed, failed */

return(1);
}/* end function add_url */


void replace(char *text, char *string, char *replace, char *result)
{
while(1)
	{
	if( (char *)strstr(text, string) == text)
		{
		*result = 0;
		strcat(result, replace);
		text += strlen(string);
		result += strlen(replace);
		}
	*result = *text;
	if(*text == 0)break;
	result++;
	text++;
	}
}/* end function replace */


int show_article_that_url_came_from(char *url)
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout, "show_article_that_url_came_from(): arg url=%s", url);
	}

/* argument check */
if(! url) return(0);

pa = lookup_url(url);
if(! pa) return(0);

/*
if not an existing group, do not try, else empty a_group in save_articles()
*/
if(! pa -> group) return(0);
if(strlen(pa -> group) == 0) return(0);

free(selected_group);
selected_group = strsave(pa -> group);

selected_article = pa -> article;

show_browser(ARTICLE_BODY_BROWSER);
refresh_screen(selected_group);

return(1);
}/* end function show_article_that_url_came_from */


int set_url_input(char *url)
{
struct url *pa;
char temp[1024];

pa = lookup_url(url);
if(! pa) return(0);

sprintf(temp, "%s (%s) %s %ld",\
pa -> name, pa -> description, pa -> group, pa -> article);
fl_set_input(fdui -> url_form_input_field, temp);

return(1);
}/* end function set_url_input */


int highlight_urls(struct url *pa)
{
int found;

if(debug_flag)
	{
	fprintf(stdout, "highlight_urls(): arg pa=%ld\n", (long) pa);
	}

/* argument check */
if(! pa) return(0);

if(! highlight_urls_flag) return(0);

found = 0;
if(highlight_urls_url_flag)
	{
	found = search_in_for_or(\
	pa -> name, highlight_keywords, highlight_case_sensitive_flag);
	if(found) return(1);
	}
if(highlight_urls_description_flag)
	{
	found = search_in_for_or(\
	pa -> description, highlight_keywords, highlight_case_sensitive_flag);
	if(found) return(1);
	}

/* return not found */
return(0);
}/* end function highlight_urls */


int search_and_select_next_url()
{
struct url *pa;
char *keyword;
int case_sensitive_flag;
int found;
int depth;
char depthstr[20];

if(debug_flag)
	{
	fprintf(stdout, "search_and_select_next_url(): arg none\n");
	}

keyword = (char *)fl_get_input(fdui -> url_form_search_input_field);
if(! keyword) return(0);

case_sensitive_flag =\
fl_get_button(fdui -> url_form_search_case_button);

found = 0;
/* pa points to next entry */
for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* search from last position (set to zero if keyword changes */
	if(pa -> browser_line <= last_found_line) continue;
	
	if(search_in_for(pa -> name, keyword, case_sensitive_flag) )
		{
		found = 1;
		}

	if(search_in_for(pa -> description, keyword, case_sensitive_flag) )
		{
		found = 1;
		}

	if(found)
		{
		free(selected_url);
		selected_url = strsave(pa -> name);

		/*
		maintain visible indication that line is selected with single
		click
		*/
		fl_select_browser_line(fdui -> url_form_browser, pa -> browser_line);

		/* show the entry */
		fl_set_browser_topline(fdui -> url_form_browser, pa -> browser_line);

		/* select the entry */
		set_url_input(selected_url);

		/* remember where we are */
		last_found_line = pa -> browser_line;
		
		if(pa -> status & DAILY_URL)
			{
			fl_set_button(fdui -> url_form_get_daily_button, 1);
			fl_set_object_label(fdui -> url_form_get_daily_button, "DAILY");
			fl_set_object_color(fdui -> url_form_get_daily_button,\
			FL_COL1, FL_DARKORANGE);
			}
		else
			{
			fl_set_button(fdui -> url_form_get_daily_button, 0);
			fl_set_object_label(fdui -> url_form_get_daily_button, "ONCE");
			fl_set_object_color(fdui -> url_form_get_daily_button,\
			FL_COL1, FL_COL1);
			}
		if(pa -> status & HOST_ONLY_URL)
			{
			fl_set_button(fdui -> url_form_host_only_button, 1);
			fl_set_object_label(fdui -> url_form_host_only_button,\
			"HOST ONLY");
			fl_set_object_color(fdui -> url_form_host_only_button,\
			FL_COL1, FL_DARKORANGE);
			}
		else
			{
			fl_set_button(fdui -> url_form_host_only_button, 0);
			fl_set_object_label(fdui -> url_form_host_only_button, "ALL");
			fl_set_object_color(fdui -> url_form_host_only_button,\
			FL_COL1, FL_COL1);
			}	 
		if(pa -> status & PICTURES_URL)
			{	
			fl_set_button(fdui -> url_form_get_pictures_button, 1);
			fl_set_object_label(fdui -> url_form_get_pictures_button,\
			"PICTURES");
			fl_set_object_color(fdui -> url_form_get_pictures_button,\
			FL_COL1, FL_YELLOW);
			}
		else
			{
			fl_set_button(fdui -> url_form_get_pictures_button, 0);
			fl_set_object_label(fdui -> url_form_get_pictures_button,\
			"NO PICTURES");
			fl_set_object_color(fdui -> url_form_get_pictures_button,\
			FL_COL1, FL_COL1);
			}

		depth = (pa -> status & 0x00ff00) >> 8;
		sprintf(depthstr, "%4d", depth);
		fl_set_input(fdui -> url_form_depth_input_field, depthstr); 		

		/* return found */
		return(1);
		}/* end if found */
	}/* end for all url's */

if(last_found_line) fl_show_alert(keyword, "end of list", "", 0);
else fl_show_alert(keyword, "not found", "", 0);

last_found_line = 0;
/*url_form_reset_last_found_line();*/
/*fl_ringbell(1);*/

/* return not found */
return(0);
}/* end function search_and_select_url */


int url_form_reset_last_found_line()
{
last_found_line = 0;
return(1);
}/* end function url_form_reset_last_found_line */


int extract_url_from_input(\
FL_OBJECT *ob, char *copy, char *group, long article_id)
{
int a, c, i;
int cux, cuy;
int cupos;
char *ptr, *sptr, *nptr;
int start_selected_text;
int end_selected_text;
char *selected_text;
int len, nlen;
int topline;
int add_to_history;

if(debug_flag)
	{
	fprintf(stdout,\
	"extract_url_from_input(): arg ob=%ld copy=%s group=%s article_id=%ld\n",\
	(long) ob, copy, group, article_id);	
	}

/* argument check */
if(! ob) return(0);
if(! copy) return(0);
if(! group) return(0);
if(article_id < 0) return(0);

/*
Now you may wonder what is happening here.
Well the almost impossible is being done.
When using the left mouse button to set the cursor position in the 
article body input field, when pasting in a part of the input field
by pressing the middle mouse button, I substract the old and new lenght
and substract that from the present cursor position.
This results in the true cursor position that was pointed to.
Than if this is a space or control char, nothin was selected, return.
else move backwards until start of text, control char or space found.
This is the start of the selected text.
Then from the cursor position move forwards, until space or control char, or
end of text found.
This is the end of the selected text.
*/

/* get cursor position */
cupos =\
fl_get_input_cursorpos(ob, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	return(0);
	}

/* get the data */

/* use the unmodified data (by cut and paste with mouse button 2) */
ptr = copy;
len = strlen(ptr);

/* get the modified data, after the cut and paste action with the mouse */
nptr = (char *) fl_get_input(ob);
if(nptr) nlen = strlen(nptr);
else nlen = 0;

/*fprintf(stdout, "len=%d nlen=%d\n", len, nlen);*/

/*
The user clicked somewhere in a continous piece of text (no spaces),
find the start.
*/

/*
fprintf(stdout, "ptr=%lu cupos=%d char at cupos=%c (%d)\n",\
ptr, cupos, *(ptr + cupos), *(ptr + cupos) );
*/

/* find start selected text */
cupos -= nlen - len;

/*fprintf(stdout, "new calculated cupos=%d\n", cupos);*/

start_selected_text = cupos;
sptr = ptr + cupos;
if(*sptr == ' ') return(0);/* not on any text */
while(1)
	{
	if(sptr == ptr) break;
	/* 
	control char or space or ident char (anything not allowed in URL)
	there may be more
	*/
	if( (*sptr <= 32) || (*sptr == '>') )
		{
		start_selected_text++;/* point to last valid */
		break;
		}
	sptr--;
	start_selected_text--;
	}
	
/* find end selected text */
end_selected_text = cupos + 1;
sptr = ptr +  cupos;
while(1)
	{
	if(*sptr <= 32)
		{
		end_selected_text--;
		break; /* control character or space */
		}
	sptr++;
	end_selected_text++;
	}

/*
fprintf(stdout, "start=%d end=%d\n", start_selected_text, end_selected_text);
*/

/* copy selected continuous area to selected_text */
selected_text = malloc(end_selected_text - start_selected_text + 1);
for(i = 0; i < (end_selected_text - start_selected_text); i++)
	{
	c = ptr[start_selected_text + i];
	selected_text[i] = c;
	}
selected_text[i] = 0; /* string termination */	

if(debug_flag)
	{
	fprintf(stdout, "selected_text=%s\n", selected_text);
	fprintf(stdout, "html_file_shown=%d\n", html_file_shown);
	}

/* if displaying an html file, get this URL */
if(html_file_shown)
	{
	/* remember the current vertical position */
	topline = fl_get_input_topline(fdui -> article_body_input_field);
	set_url_history_current_topline(topline);
	
	a = 0;
	/*
	Strip file:// from the format:
	file:///root/.NewsFleX/http/localhost/newsflex/index.html
	becomes:
	/root/.NewsFleX/http/localhost/newsflex/index.html
	*/
	ptr = strstr(selected_text, "file://");
	if(ptr)
		{
		/* http_browser needs filename */
		a = http_browser(selected_text + 7, &add_to_history, 0);
		if(a)
			{
			/* do not add to history in case image or some applicaion */
			if(add_to_history)
				{
				/* for the BACK button, remember where we are */
				topline =\
				fl_get_input_topline(fdui -> article_body_input_field);
				add_to_url_history(selected_text + 7, topline);
				}/* end if add_to_history */
			}/* end if http_browser ok */
		}/* end if file:// */
	/*
	If the url was not shown restore the old text (was possibly
	modified by cut and paste
	*/
	if(! a)
		{
		fl_set_input(ob, article_body_copy);
		fl_set_input_topline(ob, article_body_topline);
		}
	
	free(selected_text);
	return(1);
	}/* end if html_file_shown */

ptr = (char *) fl_show_input("Comment on URL? (cancel aborts)", "");
if(! ptr)
	{
	free(selected_text);
	return(0);/* user cancel */
	}
		
sptr = malloc(strlen(selected_text) + strlen(" ") + strlen(ptr) + 1);
sprintf(sptr, "%s %s", selected_text, ptr);
add_url(sptr, group, article_id);
free(sptr);
free(selected_text);

save_urls();

show_urls(SHOW_SAME_POSITION);

/* restore the text, it was changed by cut and paste with the mouse */
fl_set_input(ob, copy);

return(1);
}/* end function extract url_from_input */


int set_url_status(char *url, int status)
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"set_url_status(): arg url=%s status=%d\n", url, status);
	}

/* argument check */
if(! url) return(0);

pa = lookup_url(url);
if(! pa) return(0);

pa -> status = status;

return(1);
}/* end function set_url_status */
 

int get_url_status(char *url, int *status)
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"get_url_status(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

pa = lookup_url(url);
if(! pa) return(0);

*status = pa -> status;

return(1);
}/* end function get_url_status */
 

int add_marked_urls_to_command_queue()
{
struct url *pa;
time_t now;
struct tm *present_time_s;
struct tm *last_updated_time_s;
int p_year, p_month, p_mday;
int l_year, l_month, l_mday;
int same_year, same_month, same_day;

if(debug_flag)
	{
	fprintf(stdout, "add_marked_urls_to_command_queue arg none\n");
	}

/* for gcc -Wall */
p_year = 0;
p_month = 0;
p_mday = 0;
l_year = 0;
l_month = 0;
l_mday = 0;

/* not in loop */
now = time(0);
for(pa = urltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(pa -> status & RETRIEVE_URL)/* overrules above */
		{
		if(pa -> status & DAILY_URL)
			{
			/* do not retrieve twice or more on times on the same day */
			present_time_s = gmtime(&now);		
			p_year = present_time_s -> tm_year;
			p_month = present_time_s -> tm_mon;
			p_mday = present_time_s -> tm_mday;

			last_updated_time_s = gmtime(&(pa -> last_updated_time) );
			l_year = last_updated_time_s -> tm_year;
			l_month = last_updated_time_s -> tm_mon;
			l_mday = last_updated_time_s -> tm_mday;
		
			same_year = 0;
			same_month = 0;
			same_day = 0;

			if(p_year == l_year) same_year = 1;
			if(p_month == l_month) same_month = 1;
			if(p_mday == l_mday) same_day = 1;

			if(debug_flag)
				{
				fprintf(stdout,\
				"add_marked_urls_to_command_queue():\n\
				p_year=%d p_month=%d p_mday=%d\n\
				l_year=%d l_month=%d l_mday=%d\n",\
				p_year, p_month, p_mday,\
				l_year, l_month, l_mday);
				}

			if(same_year && same_month && same_day) continue; /* same date */
			}/* end if daily */

		/*
		second argument later to become pa -> article, used as follows:
		pa -> status sets url_depth to 0 too,
		type = 65536 * url_depth + flags.
		*/
		add_to_command_queue(pa -> name, pa -> status, URL_GET);
		}/* end if retrieve */
	}/* end for all urls */

return(1);
}/* end function add_marked_urls_to_command_queue */


int extract_url_from_input_html_file_and_add_to_url_list(\
FL_OBJECT *ob, char *copy)
{
int c, i;
int cux, cuy;
int cupos;
char *selected_text;
char *ptr, *sptr, *nptr;
int start_selected_text;
int end_selected_text;
int len, nlen;
int topline;
char *comment;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout,\
	"extract_url_from_input_html_file_and_add_to_url_list(): arg ob=%ld copy=%s\n",\
	(long) ob, copy);	
	}

/* argument check */
if(! ob) return(0);
if(! copy) return(0);
if(! html_file_shown) return(0);

/* get cursor position */
cupos =\
fl_get_input_cursorpos(ob, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	return(0);
	}

/* get the data */

/* use the unmodified data (by cut and paste with mouse button 2) */
ptr = copy;
len = strlen(copy);

/* get the modified data, after the cut and paste action with the mouse */
nptr = (char *) fl_get_input(ob);
if(nptr) nlen = strlen(nptr);
else nlen = 0;

/*fprintf(stdout, "len=%d nlen=%d\n", len, nlen);*/

/*
The user clicked somewhere in a continous piece of text (no spaces),
find the start.
*/

/*
fprintf(stdout, "ptr=%lu cupos=%d char at cupos=%c (%d)\n",\
ptr, cupos, *(ptr + cupos), *(ptr + cupos) );
*/

/* find start selected text */
cupos -= nlen - len;

/*fprintf(stdout, "new calculated cupos=%d\n", cupos);*/

start_selected_text = cupos;
sptr = ptr + cupos;
if(*sptr == ' ') return(0);/* not on any text */
while(1)
	{
	if(sptr == ptr) break;
	/* 
	control char or space or ident char (anything not allowed in URL)
	there may be more
	*/
	if( (*sptr <= 32) || (*sptr == '>') )
		{
		start_selected_text++;/* point to last valid */
		break;
		}
	sptr--;
	start_selected_text--;
	}
	
/* find end selected text */
end_selected_text = cupos + 1;
sptr = ptr +  cupos;
while(1)
	{
	if(*sptr <= 32)
		{
		end_selected_text--;
		break; /* control character or space */
		}
	sptr++;
	end_selected_text++;
	}

/*
fprintf(stdout, "start=%d end=%d\n", start_selected_text, end_selected_text);
*/

/* copy selected continuous area to selected_text */
selected_text = malloc(end_selected_text - start_selected_text + 1);
for(i = 0; i < (end_selected_text - start_selected_text); i++)
	{
	c = ptr[start_selected_text + i];
	selected_text[i] = c;
	}
selected_text[i] = 0; /* string termination */	

/*fprintf(stdout, "selected_text=%s\n", selected_text);*/

/* remember the current vertical position */
topline = fl_get_input_topline(fdui -> article_body_input_field);
	
/*
Strip file:///root/.NewsFleX/http/  from the format:
file:///root/.NewsFleX/http/localhost/newsflex/index.html
becomes:
localhost/newsflex/index.html
*/

/* test if URL actually was converted to file format */
sprintf(temp, "file://%s/.NewsFleX/", home_dir);
if(strstr(selected_text, temp) )
	{
	/* test for http */
	ptr = strstr(selected_text, "http/");	
	if(ptr)
		{
		/* scipt the http/, and add prefix http:// */
		sprintf(temp, "http://%s", ptr + 5);
		}
	else
		{
		/* this does not seem to happen to often, not tested XXXX */
		ptr = strstr(selected_text, "ftp/");
		if(ptr)
			{
			/* scip the ftp/ and prefix ftp:// */
			sprintf(temp, "ftp://%s", ptr + 4);
			}
		else
			{
			/* some error */
			free(selected_text);
			return(0);
			}
		}
	}/* end if URL was converted to local file */
else
	{
	/* else use all of the selected text */
	sprintf(temp, "%s", selected_text);
	}

/* ask user for comment */
comment = (char *) fl_show_input("Comment on URL? (cancel aborts)", "");
if(! comment)
	{
	free(selected_text);
	return(0);/* user cancel */
	}	

sptr = malloc(strlen(temp) + strlen(" ") + strlen(comment) + 1);
sprintf(sptr, "%s %s", temp, comment);
add_url(sptr, "", 0);
free(sptr);
free(selected_text);

save_urls();

show_urls(SHOW_SAME_POSITION);

/* restore the text, it was changed by cut and paste with the mouse */
fl_set_input(ob, copy);

/* set the correct topline again */
fl_set_input_topline(fdui -> article_body_input_field, topline);

return(1);
}/* end function extract_url_from_input_html_file_and_add_to_url_list */


int set_url_update_time(char *url_in_progress, time_t now)
{
struct url *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"set_url_update_time(): arg url_in_progress=%s now=%ld\n",\
	url_in_progress, now);
	}

if(! url_in_progress) return(0);

pa = lookup_url(url_in_progress);
if(! pa) return(0);

pa -> last_updated_time = now;

return(1);
}/* end function set_url_update_time */


int delete_url_file(char *url)
{
char temp[TEMP_SIZE];
struct urldata *pa;

/*
url is in format:
http://server/[[[directory/]filename].ext]
ftp://server/[[[directory//]filename].ext]
Same for telnet, gopher, etc..
File is in format ~/.NewsFleX/http/.....
*/ 

if(debug_flag)
	{
	fprintf (stdout,\
	"delete_url_file(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

/* parse URL (from rfc1738.txt */
pa = parse_url(url);
if(! pa)
	{
	/* not a valid url */
	if(debug_flag)
		{
		fprintf (stdout,\
		"delete_url_file(): cannot parse url\n");
		}

	return(0);
	}

sprintf (temp, "rm %s", pa -> localpathfilename);
if( debug_flag)
	{
	fprintf(stdout,\
	"delete_url_file(): deleting=%s\n", temp);
	}

/*
fileptr = popen (temp, "r");
pclose (fileptr);
*/

return 1;
}/* end function delete_url_file */


int delete_url_file_and_all_url_files_referenced_by_it(char *url)
{
struct urldata *pa;

if(debug_flag)
	{
	fprintf (stdout,\
	"delete_url_file_and_all_url_files_referenced_by_it(): arg url=%s\n",\
	url);
	}

/* argument check */
if(! url) return(0);

/* parse URL (from rfc1738.txt */
pa = parse_url(url);
if(! pa)
	{
	/* not a valid url */
	if(debug_flag)
		{
		fprintf (stdout,\
		"delete_url_file_and_all_url_files_referenced_by_it():\n\t
cannot parse url\n");
		}

	return(0);
	}
if(debug_flag)
	{
	fprintf(stdout,\
	"delete_url_file_and_all_url_files_referenced_by_it():\n\t\
pa -> localpathfilename=%s\n",\
	pa -> localpathfilename);
	}

	
/* remove all links referenced from selected file */

/*unlink(pa -> localpathfilename);*/

return 1;
}/* end function delete_url_file_and_all_url_files_referenced_by_it */


