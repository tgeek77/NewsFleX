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


struct posting_period
	{
	char *name;/* this is the posting_period space period */
	int browser_line;
	struct posting_period *nxtentr;
	struct posting_period *prventr;
	};
/* first element points to first entry, second element to last entry */
struct posting_period *posting_periodtab[2];


struct posting_period *lookup_posting_period(char *name)
{
struct posting_period *pa;

/*pa points to next entry*/
for(pa = posting_periodtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_posting_period */


struct posting_period *install_posting_period_at_end_of_list(char *name)
{
struct posting_period *plast, *pnew;
struct posting_period *lookup_posting_period();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_posting_period_at_end_off_list(): arg name=%s\n", name);
	}

pnew = lookup_posting_period(name);
if(pnew)
	{
	/* free previous definition */
/*	free(pnew -> subject);*/
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct posting_period *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = posting_periodtab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !posting_periodtab[0] ) posting_periodtab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
posting_periodtab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_posting_period_at_end_of_list */


int delete_posting_period(char *name)/* delete entry from double linked list */
{
struct posting_period *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_posting_period(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = posting_periodtab[0];
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
	/* if first one, modify posting_periodtab[0] */
	if(pprev == 0) posting_periodtab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify posting_periodtab[1] */
	if(pnext == 0) posting_periodtab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_posting_period */


int delete_all_posting_periods()/* delete all entries from table */
{
struct posting_period *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_posting_periods() arg none\n");
	}

while(1)
	{	
	pa = posting_periodtab[0];
	if(! pa) break;
	posting_periodtab[0] = pa -> nxtentr;
	free(pa -> name);

	free(pa);/* free structure */
	}/* end while all structures */

posting_periodtab[1] = 0;
return(1);
}/* end function delete_all_posting_periods */


int load_posting_periods()
{
int a;
FILE *posting_periods_dat_file;
char temp[READSIZE];
char pathfilename[TEMP_SIZE];
char name[TEMP_SIZE];
int browser_line;
struct posting_period *pa;

if(debug_flag)
	{
	fprintf(stdout, "load_posting_periods() arg none\n");
	}
	
delete_all_posting_periods();

sprintf(pathfilename, "%s/.NewsFleX/posting_periods.dat", home_dir);
posting_periods_dat_file = fopen(pathfilename, "r");
if(! posting_periods_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout, "could not load file %s\n", pathfilename);
		}
	return(0);
	} 

/*
Set the global and the input field in the setup form to the selected server.
*/
a = readline(posting_periods_dat_file, temp);
if(a == EOF) return(0);/* file format error */

free(postings_database_name);
postings_database_name = strsave(temp);

/*fl_set_input(fd_NewsFleX -> news_server_input_field, posting_period);*/

while(1)
	{
	a = readline(posting_periods_dat_file, temp);
	if(a == EOF) return(1);/* empty file or last line */
	
	sscanf(temp, "%s %d", name, &browser_line);

	/* create a structure entry */
	pa = install_posting_period_at_end_of_list(name);
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"load_posting_periods: cannot install posting_period %s\n", temp);
			}
		break;
		}

/*
	pa -> name = strsave(name);
	if(! pa -> name) break;
*/
	pa -> browser_line = browser_line;
	}/* end while all lines in posting_periods.dat */

if(a == EOF)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_posting_periods(): file format error\n");
		}
	}
else
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_posting_periods(): memory allocation error\n");
		}
	}	
return(0);
}/* end function load posting_periods */


int save_posting_periods()
{
FILE *posting_periods_dat_file;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct posting_period *pa;
extern char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "save_posting_periods() arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/posting_periods.tmp", home_dir);
posting_periods_dat_file = fopen(pathfilename, "w");
if(! posting_periods_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 

fprintf(posting_periods_dat_file, "%s\n", postings_database_name);
for(pa = posting_periodtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(posting_periods_dat_file, "%s %d\n",\
	pa -> name, pa -> browser_line);
	}/* end while all elements in list */

fclose(posting_periods_dat_file);

/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/posting_periods.dat", home_dir);
sprintf(pathfilename2, "%s/.NewsFleX/posting_periods.dat~", home_dir);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_posting_periods(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/posting_periods.tmp", home_dir);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_posting_periods(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);/* ok */
}/* end function save_posting_periods */


int delete_a_posting_period(char *period)
{
char temp[TEMP_SIZE];
FILE *exec_filefd;
int format_error;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "delete_a_posting_period(): arg period=%s\n",\
	period);
	}

/* argument check */
if(! period) return(0);
format_error = 0;
if(strlen(period) == 0) format_error = 1;
if(strstr(period, "/") != 0) format_error = 1;
if(strstr(period, " ") != 0) format_error = 1;

if(format_error)
	{
	fl_show_alert("delete_a_posting_period()",\
	period,\
	"format error, command cancelled", 0);
	return(0);
	}

/* this is the startup database */
if(strcmp(period, "current") == 0)
	{
	fl_show_alert(\
	"This entry cannot be erased",\
	period,\
	"command cancelled", 0);
	return(0);
	}

/* it is possible that we want to delete a selected entry */
if(strcmp(period, postings_database_name) == 0)
	{
	fl_show_alert(\
	"This posting period is selected",\
	"select an other one first",\
	"command cancelled", 0);
	return(0);
	}

/* last user confirmation */
sprintf(temp, "Delete all data for %s???", period);
if(! fl_show_question(temp, 0) ) /* mouse on no */
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

/* delete all files in ~/.NewsFleX/postings/period directory upward */
sprintf(temp, "rm -rf %s/.NewsFleX/postings/%s/*",\
home_dir, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* delete ~/.NewsFleX/postings/period directory */
sprintf(temp, "rmdir %s/.NewsFleX/postings/%s",\
home_dir, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* delete structure entry */
if(! delete_posting_period(period) )/* this tests for existence */
	{
	fl_show_alert(\
	"Cannot delete posting_period", period, "command cancelled", 0);
	return(0);
	}

/*'name' no longer allocated!!!!!!!! */

/* write modified structure to disk */
save_posting_periods();

return(1);
}/* end function delete_a_posting_period */


int show_posting_period_form()
{
show_posting_periods(SHOW_UNREAD_AT_TOP);

if( (news_server_status == CONNECTED) || (mail_server_status == CONNECTED) )
	{
	fl_show_alert(\
	"You are still connected to a server",\
	"This function is only availeble offline", "", 0);
	return(1);
	}

fl_set_input(fdui -> posting_period_form_input_field, "");
fl_show_form(fd_NewsFleX -> posting_period_form,\
FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function show_posting_period_form */


int add_posting_period(char *period)
{
int illegal;
struct posting_period *pa;
char temp[TEMP_SIZE];
FILE *exec_filefd;

if(debug_flag)
	{
	fprintf(stdout, "add_posting_period(): arg period=%s", period);
	}

/* argument check */
illegal = 0;
if(! period) return(0);
if(strlen(period) == 0) illegal = 1;
if(strstr(period, "/") != 0) illegal = 1;
if(strstr(period, " ") != 0) illegal = 1;
if(strcmp(period, "current") == 0) illegal = 1;
if(illegal)
	{
	fl_show_alert("Illegal name", period, "command cancelled", 0);
	return(0);
	}

pa = lookup_posting_period(period);
if(pa)
	{
	fl_show_alert(\
	"posting period exists", period, "command cancelled", 0);
	if(debug_flag)
		{
		fprintf(stdout, "add_posting_period(): %s exists, returning\n",\
		period);
		}
	return(0);/* do not select */
	}

sprintf(temp,\
"Posting period %s does not exist\nCreate entry?", period);
if(! fl_show_question(temp, 1) )/* 1 = mouse on yes */
	{
	return(0);
	}

pa = install_posting_period_at_end_of_list(period);
if(! pa)
	{
	fl_show_alert(\
	"add_posting_period():",\
	"cannot install posting period",\
	"command cancelled", 0);
	return(0);
	}

/* create posting period directory */
sprintf(temp, "mkdir %s/.NewsFleX/postings/%s",\
home_dir, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* test if postings.dat exists */
sprintf(temp, "%s/.NewsFleX/postings/%s/postings.dat",\
home_dir, period);
exec_filefd = fopen(temp, "r");
/* if postings.dat does not exist, create an empty one */
if(! exec_filefd)
	{
	/* create empty groups.dat */
	exec_filefd = fopen(temp, "w");
	if(! exec_filefd)
		{
		fl_show_alert(\
		"Cannot create file", temp, "", 0);
		return(0);
		}
	fclose(exec_filefd);
	}
else
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"add_posting_period(): file %s exists, not creating it\n", temp);
		}
	fclose(exec_filefd);
	}

/* structure was modified */
save_posting_periods();

return(1);
}/* end function add_posting_period */


int select_posting_period(char *period)
{
if(debug_flag)
	{
	fprintf(stdout, "select_posting_period(): arg period=%s\n", period);
	}

/* argument check */
if(! period) return(0);
if(strlen(period) == 0) return(0);
if(strstr(period, "/") != 0) return(0);
if(strstr(period, " ") != 0) return(0);

free(postings_database_name);
postings_database_name = strsave(period);

/* set the global (postings_database_name) */
/* posting_periods.dat modified (first line, posting_period) */
save_posting_periods();

/* set NewsFleX title */
set_title();

/* reset for a new start */
reset_postings();

/* set some defaults */
load_postings();

/* redraw the display */
refresh_screen(selected_group);

return(1);
}/* end function select_posting_period */


int show_posting_periods(int position)
{
char temp[TEMP_SIZE];
struct posting_period *pa;
char formatstr[20];
int browser_line;
int browser_topline;
int first_new_posting_period_line;
int find_first_new_posting_period_flag;

int maxline;

if(debug_flag)
	{
	fprintf(stdout, "show_posting_periods(): arg position=%d\n", position);
	}

load_posting_periods();

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> posting_period_form_browser);

fl_freeze_form(fd_NewsFleX -> posting_period_form);

fl_clear_browser(fdui -> posting_period_form_browser);

first_new_posting_period_line = -1;/* keep gcc -Wall from complaining */
find_first_new_posting_period_flag = 1;
browser_line = 1;
for(pa = posting_periodtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(position == SHOW_UNREAD_AT_TOP)
		{
		/* want to show the first unaccessed at top of display */
		first_new_posting_period_line = browser_line;
		find_first_new_posting_period_flag = 0;
		}
	
	strcpy(formatstr, "@f");
	strcat(formatstr, "@C0");/* black */

	sprintf(temp, "%s %s",\
	formatstr, pa -> name);

	fl_add_browser_line(fdui -> posting_period_form_browser, temp);
	pa -> browser_line = browser_line;/* first one is 1 */
	browser_line++;
	}/* end for all elements in list */
	
/* re adjust browser for same position of top line (vertical slider) */	
if(position == SHOW_UNREAD_AT_TOP)
	{
	fl_set_browser_topline(fdui -> posting_period_form_browser, first_new_posting_period_line);
	}
if(position == SHOW_BOTTOM)
	{
	/* make the status of the last posting_period visible */
	maxline = fl_get_browser_maxline(fdui -> posting_period_form_browser);
	fl_set_browser_topline(fdui -> posting_period_form_browser, maxline);
	}
/*
if(position == SHOW_TOP)
	{
	fl_set_browser_topline(fdui -> posting_period_form_browser, 1);
	}
*/
if(position == SHOW_SAME_POSITION)
	{
	fl_set_browser_topline(fdui -> posting_period_form_browser, browser_topline);
	}	

fl_unfreeze_form(fd_NewsFleX -> posting_period_form);

return(1);
}/* end function show_posting_periods */


char *line_to_posting_period(int line)
{
struct posting_period *pa;
extern long atol();

if(debug_flag)
	{
	fprintf(stdout, "line_to_posting_period(): arg line=%d\n", line);
	}

/* argument check */
if(line < 0) return(0);

for(pa = posting_periodtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		return(pa -> name);
		}
	}

/* no save_posting_periods, nothing was changed */
return(0);
}/* end function line_to_posting_period_id */


int posting_period_to_line(char *name, int *line)
{
struct posting_period *pa;

if(debug_flag)
	{
	fprintf(stdout, "posting_period_to_line(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = lookup_posting_period(name);
if(! pa) return(0);

*line = pa -> browser_line;
return(1);
}/* end function posting_period_to_line */


int move_current_postings_database_and_create_new_current(char *destperiod)
{
FILE *fileptr;
char temp[TEMP_SIZE];
struct posting_period *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"move_current_postings_database_and_create_new_current():\n\
	arg destperiod=%s\n",\
	destperiod);
	}

/* argument check */
if(! destperiod) return(0);
if(strlen(destperiod) == 0) return(0);
if(strstr(destperiod, "/") != 0) return(0);
if(strstr(destperiod, " ") != 0) return(0);

/* test if new period already exists */
pa = lookup_posting_period(destperiod);
if(pa)
	{
	fl_show_alert(destperiod, "already exists", "command cancelled", 0);
	return(0);
	}

/* rename current to destperiod */
sprintf(temp,\
"mv %s/.NewsFleX/postings/current %s/.NewsFleX/postings/%s",\
home_dir, home_dir, destperiod);
fileptr = popen(temp, "r");
pclose(fileptr);

/* create a new entry in structure */
pa = install_posting_period_at_end_of_list(destperiod);
if(! pa)
	{
	fl_show_alert("cannot install", destperiod, "command cancelled", 0);
	return(0);
	}

/* structure was modified */
save_posting_periods();

/* create new current directory */
sprintf(temp,\
"mkdir %s/.NewsFleX/postings/current",\
home_dir);
fileptr = popen(temp, "r");
pclose(fileptr);

/* create empty postings.dat in new current directory */
/* test if perhaps exists */
sprintf(temp,\
"%s/.NewsFleX/postings/current/postings.dat",\
home_dir);
fileptr = fopen(temp, "r");
if(fileptr)
	{
	fl_show_alert("file exists", temp, "aborting", 0);
	fclose(fileptr);
	exit(1);
	}

/* make empty file */
fileptr = fopen(temp, "w");
fclose(fileptr);

/*
load the new (empty) postings.dat, this prevents the old one (not empty)
to be written to the new current directory.
*/
reset_postings();
load_postings();

show_posting_periods(SHOW_UNREAD_AT_TOP);

refresh_screen(selected_group);

return(1);
}/* move_current_postings_database_and_create_new_current */


char *find_next_posting_period(char *current_posting_period)
{
struct posting_period *pa;

if(debug_flag)
	{
	fprintf(stdout,
	"find_next_posting_period(): arg current_posting_period=%s\n",\
	current_posting_period);
	}

/* argument check */
if(! current_posting_period) return(0);

pa = lookup_posting_period(current_posting_period);
if(! pa) return(0); /* posting_period does not exist in list*/

/* set start search for current_posting_period */
pa = pa -> nxtentr;/* point to next posting_period */
if(debug_flag)
	{
	fprintf(stdout,\
	"find_next_posting_period(): pa -> nxtentr=%lu\n", (long)pa);
	}

/* test if last in list, if so go to start list */
if(! pa) pa = posting_periodtab[0];

/* test for empty list */
if(! pa) return(0);

if(debug_flag)
	{
	fprintf(stdout,\
	"find_next_posting_period(): next pa->name=%s\n",\
	pa -> name);
	}

return(pa -> name);
}/* end function find_next_posting_period */


