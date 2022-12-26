/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997, 1998, 1999, 2000 <Jan Mourer>
email: jan@panteltje.demon.nl
snail mail: Monnikebildtdijk 2 9078 VE Oude Bildtzijl Netherlands

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


struct workinglist
	{
	char *name;
	struct workinglist *nxtentr;
	struct workinglist *prventr;
	};
struct workinglist *workinglisttab[2]; /* first element points to first entry,
			second element to last entry */


struct newsserver
	{
	char *name;/* this is the newsserver space period */
	char *mail_server_name;
	char *user_email;
	char *real_name;
	char *auto_execute_script;
	char *connect_to_net_script;
	char *disconnect_from_net_script;
	char *net_device_pid;
	char *get_new_headers_time;
	char *username;
	char *password;	
	char *flags;
	int news_server_port;
	int mail_server_port;
	int connect_to_news_server_timeout;
	int connect_to_mail_server_timeout;
	int maximum_headers;
	int tab_size;
	int status;
	int browser_line;
	struct newsserver *nxtentr;
	struct newsserver *prventr;
	};
struct newsserver *newsservertab[2]; /* first element points to first entry,
			second element to last entry */


struct newsserver *lookup_newsserver(char *name)
{
struct newsserver *pa;

/* argument check */
if(! name) return(0);

/*pa points to next entry*/
for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_newsserver */


struct newsserver *install_newsserver_at_end_of_list(char *name)
{
struct newsserver *plast, *pnew;
struct newsserver *lookup_newsserver();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_newsserver_at_end_of_list(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pnew = lookup_newsserver(name);
if(pnew)
	{
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct newsserver *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = newsservertab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points to zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previous structure pointers */
if( !newsservertab[0] ) newsservertab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
newsservertab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_newsserver_at_end_of_list */


int delete_newsserver(char *name)/* delete entry from double linked list */
{
struct newsserver *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_newsserver(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = newsservertab[0];
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
	/* if first one, modify newsservertab[0] */
	if(pprev == 0) newsservertab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify newsservertab[1] */
	if(pnext == 0) newsservertab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel -> mail_server_name);
	free(pdel -> user_email);
	free(pdel -> real_name);
	free(pdel -> auto_execute_script);
	free(pdel -> connect_to_net_script);
	free(pdel -> disconnect_from_net_script);
	free(pdel -> net_device_pid);

	free(pdel -> get_new_headers_time);
	free(pdel -> username);
	free(pdel -> password);
	free(pdel -> flags);

	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_newsserver */


int delete_all_newsservers()/* delete all entries from table */
{
struct newsserver *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_newsservers() arg none\n");
	}

while(1)
	{	
	pa = newsservertab[0];
	if(! pa) break;
	newsservertab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa -> mail_server_name);
	free(pa -> user_email);
	free(pa -> real_name);
	free(pa -> auto_execute_script);
	free(pa -> connect_to_net_script);
	free(pa -> disconnect_from_net_script);
	free(pa -> net_device_pid);

	free(pa -> get_new_headers_time);
	free(pa -> username);
	free(pa -> password);
	free(pa -> flags);

	free(pa);/* free structure */
	}/* end while all structures */

newsservertab[1] = 0;
return(1);
}/* end function delete_all_newsservers */


int sort_newsservers()
/*
sorts the double linked list with as criterium that the lowest ASCII goes
on top.
doing some sort of bubble sort.
*/
{
struct newsserver *pa;
struct newsserver *pb;
int swap_flag;

while(1)/* go through list again and again */
	{
	if(debug_flag)
		{
		fprintf(stdout, "CHECKING LIST\n");
		}/* end if debug_flag */
		
	swap_flag = 0;
	for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)
		{
		if(debug_flag)
			{
			fprintf(stdout, "sort_newsservers(): sorting %s pa=%lu\n",\
			pa -> name, pa);		
			}	
	
		pb = pa -> prventr;
		if(debug_flag)
			{
			fprintf(stdout, "pb=pa->prventr=%lu\n", pb);
			}
		
		if(pb)
			{
			/* compare */
			if(strcmp(pa -> name, pb -> name) < 0)
				{
				swap_flag = swap_position(pa , pb);
				/* indicate position was swapped */
				if(debug_flag)
					{
					fprintf(stdout, "swap_flag=%d\n", swap_flag);
					fprintf(stdout,\
					"AFTER SWAP pa->prventr=%lu pa->nxtentr=%lu\n\
					pb->prventr=%lu pb-nxtentrr=%lu\n",\
					pa -> prventr, pa -> nxtentr,\
					pb -> prventr, pb -> nxtentr);		
					}										
				}/* end if strcmp < 0 */
			}/* end if pb */
		}/* end for all entries */

	/* if no more swapping took place, ready, list is sorted */
	if(! swap_flag) break;
	}/* end while go through list again and again */

return 1;
}/* end function sort_newsservers */


int load_newsservers()
{
int a;
FILE *newsservers_dat_file;
char temp[READSIZE];
char pathfilename[TEMP_SIZE];
extern long atol();
struct newsserver *pa;
char newsserver[TEMP_SIZE];
char period[TEMP_SIZE];
char name[TEMP_SIZE];
char mail_server_name[TEMP_SIZE];
char user_email[TEMP_SIZE];
char real_name[TEMP_SIZE];
char auto_execute_script[TEMP_SIZE];
char connect_to_net_script[TEMP_SIZE];
char disconnect_from_net_script[TEMP_SIZE];
char net_device_pid[TEMP_SIZE];
char get_new_headers_time[TEMP_SIZE];
char username[TEMP_SIZE];
char password[TEMP_SIZE];
char flags[TEMP_SIZE];
int news_server_port;
int mail_server_port;
int connect_to_news_server_timeout;
int connect_to_mail_server_timeout;
int maximum_headers;
int tab_size;
int status;
int browser_line; /* used to lookup name (= id) if clicked in display,
						not saved on disk, assigned each time list
						is displayed */
if(debug_flag)
	{
	fprintf(stdout, "load_newsservers() arg none\n");
	}
	
delete_all_newsservers();

sprintf(pathfilename, "%s/.NewsFleX/newsservers.dat", home_dir);
newsservers_dat_file = fopen(pathfilename, "r");
if(! newsservers_dat_file)
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
a = readline(newsservers_dat_file, temp);
if(a == EOF) return(0);/* file format error */

/* extract newsserver and period */
a = sscanf(temp, "%s %s", newsserver, period);

/* check for valid */
if(a != 2) return(0);
if(strlen(newsserver) == 0) return(0);
if(strlen(period) == 0) return(0);

free(news_server_name);
news_server_name = (char *) strsave(newsserver);

free(database_name);
database_name = strsave(period);

fl_set_input(fd_NewsFleX -> news_server_input_field, newsserver);

while(1)
	{
	a = readline(newsservers_dat_file, name);
	if(a == EOF) return(1);/* empty file or last line */
	
	a = readline(newsservers_dat_file, temp);
	if(a == EOF) break;/* last line, or file format error */
	sscanf(temp, "%d %d %d %d %d %d %d %d",\
	&news_server_port, &mail_server_port,\
	&connect_to_news_server_timeout, &connect_to_mail_server_timeout,\
	&maximum_headers, &tab_size,\
	&status, &browser_line);

	a = readline(newsservers_dat_file, mail_server_name);
	if(a == EOF) break;/* last line, or file format error */

	a = readline(newsservers_dat_file, user_email);
	if(a == EOF) break;/* last line, or file format error */

	a = readline(newsservers_dat_file, real_name);
	if(a == EOF) break;/* last line, or file format error */

	a = readline(newsservers_dat_file, auto_execute_script);
	if(a == EOF) break;/* last line, or file format error */

	a = readline(newsservers_dat_file, connect_to_net_script);
	if(a == EOF) break;/* last line, or file format error */

	a = readline(newsservers_dat_file, disconnect_from_net_script);
	if(a == EOF) break;/* file format error */

	a = readline(newsservers_dat_file, net_device_pid);
	if(a == EOF) break;/* file format error */

	a = readline(newsservers_dat_file, get_new_headers_time);
	if(a == EOF) break;/* file format error */

	a = readline(newsservers_dat_file, username);
	if(a == EOF) break; /* file format error */

	a = readline(newsservers_dat_file, password);
	if(a == EOF) break; /* file format error */

	a = readline(newsservers_dat_file, flags);
	if(a == EOF) break; /* file format error */

	/* create a structure entry */
	pa = install_newsserver_at_end_of_list(name);
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"load_newsservers: cannot install newsserver %s\n", temp);
			}
		break;
		}

	pa -> mail_server_name = strsave(mail_server_name);
	if(! pa -> mail_server_name) break;
	pa -> user_email = strsave(user_email);
	if(! pa -> user_email) break;
	pa -> real_name = strsave(real_name);
	if(! pa -> real_name) break;
	pa -> auto_execute_script = strsave(auto_execute_script);
	if(! pa -> auto_execute_script) break;
	pa -> connect_to_net_script = strsave(connect_to_net_script);
	if(! pa -> connect_to_net_script) break;
	pa -> disconnect_from_net_script = strsave(disconnect_from_net_script);
	if(! pa -> disconnect_from_net_script) break;
	pa -> net_device_pid = strsave(net_device_pid);
	if(! pa -> net_device_pid) break;
	pa -> get_new_headers_time = strsave(get_new_headers_time);
	if(! pa -> get_new_headers_time) break;
	pa -> username = strsave(username);
	if(! pa -> username) break;
	pa -> password = strsave(password);
	if(! pa -> password) break;
	pa -> flags = strsave(flags);
	if(! pa -> flags) break;

	pa -> news_server_port = news_server_port;
	pa -> mail_server_port = mail_server_port;
	pa -> connect_to_news_server_timeout = connect_to_news_server_timeout;
	pa -> connect_to_mail_server_timeout = connect_to_mail_server_timeout;
	pa -> maximum_headers = maximum_headers;
	pa -> tab_size = tab_size;
	pa -> status = status;
	
	pa -> browser_line = browser_line;

	}/* end while all lines in newsservers.dat */

if(a == EOF)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_newsservers(): file format error\n");
		}
	}
else
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_newsservers(): memory allocation error\n");
		}
	}	
return(0);
}/* end function load newsservers */


int save_newsservers()
{
FILE *newsservers_dat_file;
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
struct newsserver *pa;

if(debug_flag)
	{
	fprintf(stdout, "save_newsservers() arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/newsservers.tmp", home_dir);
newsservers_dat_file = fopen(pathfilename, "w");
if(! newsservers_dat_file)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"could not open file %s for write\n", pathfilename);
		}
	return(0);
	} 

fprintf(newsservers_dat_file, "%s %s\n", news_server_name, database_name);
for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(newsservers_dat_file, "%s\n", pa -> name);

	fprintf(newsservers_dat_file, "%d %d %d %d %d %d %d %d\n",\
	pa -> news_server_port, pa -> mail_server_port,\
	pa -> connect_to_news_server_timeout, pa -> connect_to_mail_server_timeout,\
	pa -> maximum_headers, pa -> tab_size,\
	pa -> status, pa -> browser_line);

	fprintf(newsservers_dat_file, "%s\n", pa -> mail_server_name);
	fprintf(newsservers_dat_file, "%s\n", pa -> user_email);
	fprintf(newsservers_dat_file, "%s\n", pa -> real_name);
	fprintf(newsservers_dat_file, "%s\n", pa -> auto_execute_script);
	fprintf(newsservers_dat_file, "%s\n", pa -> connect_to_net_script);
	fprintf(newsservers_dat_file, "%s\n", pa -> disconnect_from_net_script);
	fprintf(newsservers_dat_file, "%s\n", pa -> net_device_pid);
	fprintf(newsservers_dat_file, "%s\n", pa -> get_new_headers_time);
	fprintf(newsservers_dat_file, "%s\n", pa -> username);
	fprintf(newsservers_dat_file, "%s\n", pa -> password);
	fprintf(newsservers_dat_file, "%s\n", pa -> flags);
	}/* end while all elements in list */

fclose(newsservers_dat_file);

/* set some path file names */
sprintf(pathfilename, "%s/.NewsFleX/newsservers.dat", home_dir);
sprintf(pathfilename2, "%s/.NewsFleX/newsservers.dat~", home_dir);

/* unlink the old .dat~ */
unlink(pathfilename2);

/* rename .dat to .dat~ */
if( rename(pathfilename, pathfilename2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_newsservers(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(pathfilename2, "%s/.NewsFleX/newsservers.tmp", home_dir);
if( rename(pathfilename2, pathfilename) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_newsservers(); rename %s into %s failed\n",\
		pathfilename, pathfilename2);
		}
	return(0);
	}

return(1);/* ok */
}/* end function save_newsservers */


int delete_a_newsserver(char *name)
{
int a;
char temp[TEMP_SIZE];
FILE *exec_filefd;
char newsserver[TEMP_SIZE];
char period[TEMP_SIZE];
int format_error;
DIR *dirptr;
struct dirent *eptr;
int subdirs;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "delete_a_newsserver(): arg name=%s\n",\
	name);
	}

/* argument check */
if(!name) return(0);
if(strlen(name) == 0) return(0);
if(strstr(name, "/") != 0) return(0);/* illegal format, and dangerous */

format_error = 0;
strcpy(newsserver, "");
strcpy(period, "");
/* extract newsserver and period from name */
a = sscanf(name, "%s %s", newsserver, period);
if(a != 2) format_error = 1;

/* check again, do not want to erase other entries */
if(strlen(newsserver) == 0) format_error = 1;
if(strstr(newsserver, "/") != 0) format_error = 1;
if(strlen(period) == 0) format_error = 1;
if(strstr(period, "/") != 0) format_error = 1;
if(strcmp(newsserver, "setup") == 0) format_error = 1;
if(strcmp(newsserver, "postings") == 0) format_error = 1;

if(format_error)
	{
	fl_show_alert("delete_a_newsserver()",\
	name,\
	"format error, command cancelled", 0);
	return(0);
	}

/* this is the startup database */
if(strcmp(name, "localhost current") == 0)
	{
	fl_show_alert(\
	"This entry cannot be erased",\
	name,\
	"command cancelled", 0);
	return(0);
	}

/* it is possible that we want to delete a selected entry */
if( (strcmp(newsserver, news_server_name) == 0) &&\
(strcmp(period, database_name) == 0) )
	{
	fl_show_alert(\
	"This news server is selected",\
	"select an other one first",\
	"command cancelled", 0);
	return(0);
	}

/* last user confirmation */
sprintf(temp, "Delete all data for %s???", name);
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

/* delete all files in newsserver/period directory upward */
sprintf(temp, "rm -rf %s/.NewsFleX/%s/%s/*",\
home_dir, newsserver, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* delete newsserver/period directory */
sprintf(temp, "rmdir %s/.NewsFleX/%s/%s",\
home_dir, newsserver, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/*
Note:
the argument 'name' in this function, is actually the name in the structure,
(same memory), and after the next delete cannot be used anymore, since it
is freed.
*/
/* delete structure entry */
if(! delete_newsserver(name) )/* this tests for existence */
	{
	fl_show_alert(\
	"Cannot delete newsserver", name, "command cancelled", 0);
	return(0);
	}

/*'name' no longer allocated!!!!!!!! */

/* write modified structure to disk */
save_newsservers();

/*
Actually we should check here if directory 'newsserver' still has any
subdirectories 'period'.
If not, the directory 'newsserver' should be erased, with any files in it.
*/
subdirs = 0;
sprintf(temp, "%s/.NewsFleX/%s",\
home_dir, newsserver);
dirptr = opendir(temp);
if(! dirptr)
	{
	fl_show_alert(\
	"No such directory", temp, "command cancelled", 0);
	return(0);
	}
 
while(1)
	{
	eptr = readdir(dirptr);
	if(!eptr) break;
	if(debug_flag)
		{
		fprintf(stdout, "delete_a_newsserver():%s\n", eptr -> d_name);
		}

	/* test for '..' */
	if(strcmp(eptr -> d_name, "..") == 0) continue;
	
	/* test for '.' */
	if(strcmp(eptr -> d_name, ".") == 0) continue;
	
	/* test for groups.dat */
	if(strcmp(eptr -> d_name, "groups.dat") == 0) continue;
	
	/* anything else then groups.dat is a subdir */
	subdirs = 1;
	}/* end while all directory entries */
(void) closedir(dirptr);

if(! subdirs)
	{
	/* remove groups.dat */
	sprintf(temp, "rm %s/.NewsFleX/%s/groups.dat", home_dir, newsserver);
	exec_filefd = popen(temp, "r");
	pclose(exec_filefd);
	
	/*
	remove the newsserver directory (and this will fail if there were
	still any subdirs).
	*/
	sprintf(temp, "rmdir %s/.NewsFleX/%s", home_dir, newsserver);
	exec_filefd = popen(temp, "r");
	pclose(exec_filefd);
	}

return(1);
}/* end function delete_a_newsserver */


int show_newsserver_form()
{
show_newsservers(SHOW_UNREAD_AT_TOP);

if( (news_server_status == CONNECTED) || (mail_server_status == CONNECTED) )
	{
	fl_show_alert(\
	"You are still connected to a server",\
	"This function is only availeble offline", "", 0);
	return(1);
	}

fl_set_input(fdui -> newsserver_form_input_field, "");
fl_show_form(fd_NewsFleX -> newsserver_form, FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function show_newsserver_form */


int save_newsserver_setting(char *name)
{
struct newsserver *pa;
char temp[TEMP_SIZE];
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "save_newsserver_setting(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);
if(strlen(name) == 0) return(0);
if(strstr(name, "/") != 0) return(0);

/* test if exists */
pa = lookup_newsserver(name);
if(! pa)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_newsserver_setting(): newsserver %s does not exists\n",\
		name);
		}	
	return(0);
	}

ptr = (char *) fl_get_input(fdui -> mail_server_input_field);
free(mail_server_name);
mail_server_name = strsave(ptr);
if(! mail_server_name) return(0);
free(pa -> mail_server_name);
pa -> mail_server_name = strsave(ptr);
if(! pa -> mail_server_name) return(0);

ptr = (char *) fl_get_input(fdui -> user_email_input_field);
free(user_email_address);
user_email_address = strsave(ptr);
if(! user_email_address) return(0);
free(pa -> user_email);
pa -> user_email = strsave(ptr);
if(! pa -> user_email) return(0);

ptr = (char *) fl_get_input(fdui -> real_name_input_field);
free(real_name);
real_name = strsave(ptr);
if(! real_name) return(0);
free(pa -> real_name);
pa -> real_name = strsave(ptr);
if(! pa -> real_name) return(0);

ptr = (char *) fl_get_input(fdui -> auto_execute_application_input_field);
free(auto_execute_program_name);
auto_execute_program_name = strsave(ptr);
if(! auto_execute_program_name) return(0);
free(pa -> auto_execute_script);
pa -> auto_execute_script = strsave(ptr);
if(! pa -> auto_execute_script) return(0);

ptr = (char *) fl_get_input(fdui -> auto_connect_to_net_input_field);
free(connect_to_net_program_name);
connect_to_net_program_name = strsave(ptr);
if(! connect_to_net_program_name) return(0);
free(pa -> connect_to_net_script);
pa -> connect_to_net_script = strsave(ptr);
if(! pa -> connect_to_net_script) return(0);

ptr = (char *) fl_get_input(fdui -> auto_disconnect_from_net_input_field);
free(disconnect_from_net_program_name);
disconnect_from_net_program_name = strsave(ptr);
if(! disconnect_from_net_program_name) return(0);
free(pa -> disconnect_from_net_script);
pa -> disconnect_from_net_script = strsave(ptr);
if(! pa -> disconnect_from_net_script) return(0);

ptr = (char *) fl_get_input(fdui -> pid_test_input_field);
free(pid_test_program_name);
pid_test_program_name = strsave(ptr);
if(! pid_test_program_name) return(0);
free(pa -> net_device_pid);
pa -> net_device_pid = strsave(ptr);
if(! pa -> net_device_pid) return(0);

ptr = (char *) fl_get_input(fdui -> get_new_headers_time_input_field);
free(get_new_headers_time);
get_new_headers_time = strsave(ptr);
if(! get_new_headers_time) return(0);
free(pa -> get_new_headers_time);
pa -> get_new_headers_time = strsave(ptr);
if(! pa -> get_new_headers_time) return(0);

ptr = (char *) fl_get_input(fdui -> server_username_input_field);
free(server_username);
server_username = strsave(ptr);
if(! server_username) return(0);
free(pa -> username);
pa -> username = strsave(ptr);
if(! pa -> username) return(0);

ptr = (char *) fl_get_input(fdui -> server_password_input_field);
free(server_password);
server_password = strsave(ptr);
if(! server_password) return(0);
free(pa -> password);
pa -> password = strsave(ptr);
if(! pa -> password) return(0);

ptr = (char *) fl_get_input(fdui -> maximum_headers_per_group_input_field);
maximum_headers = atoi(ptr);
if(maximum_headers < 0) maximum_headers = 0;
pa -> maximum_headers = maximum_headers;

ptr = (char *) fl_get_input(fdui -> news_server_port_input_field);
news_server_port = atoi(ptr);
if(news_server_port < 0) news_server_port = 0;
pa -> news_server_port = news_server_port;

ptr = (char *) fl_get_input(fdui -> mail_server_port_input_field);
mail_server_port = atoi(ptr);
if(mail_server_port < 0) mail_server_port = 0;
pa -> mail_server_port = mail_server_port;

ptr = (char *) fl_get_input(fdui -> connect_to_news_server_timeout_input_field);
connect_to_news_server_timeout = atoi(ptr);
if(connect_to_news_server_timeout < 0) connect_to_news_server_timeout = 0;
pa -> connect_to_news_server_timeout = connect_to_news_server_timeout;

ptr = (char *) fl_get_input(fdui -> connect_to_mail_server_timeout_input_field);
connect_to_mail_server_timeout = atoi(ptr);
if(connect_to_mail_server_timeout < 0) connect_to_mail_server_timeout = 0;
pa -> connect_to_mail_server_timeout = connect_to_mail_server_timeout;

ptr = (char *) fl_get_input(fdui -> tab_size_input_field);
tab_size = atoi(ptr);
if(tab_size < 0) tab_size = 0;
pa -> tab_size = tab_size;

ptr = (char *) fl_get_input(\
fdui -> setup_form_local_mail_check_interval_input_field);
local_mail_check_interval = atoi(ptr);
if(local_mail_check_interval <= 10) local_mail_check_interval = 10;

pa -> status = 0;
pa -> browser_line = 0;
	
sprintf(temp, "%d %d %d %d %d %d %d %d %d %d %d %d %d",\
enable_description_popup_flag,\
auto_connect_to_net_flag,\
auto_disconnect_from_net_flag,\
auto_execute_program_flag,\
auto_get_new_headers_flag,\
filters_enabled_flag,\
variable_width_flag,\
insert_headers_offline_flag,\
request_by_message_id_flag,\
show_article_once_flag,\
do_not_use_sendmail_flag,\
check_incoming_mail_periodically_flag,\
local_mail_check_interval);

pa -> flags = strsave(temp);
if(! pa -> flags) return(0);

save_newsservers();
			 
return(1);
}/* end function save_newsserver_setting */


int load_newsserver_setting(char *name)
{
struct newsserver *pa;
char *ptr;
char temp[TEMP_SIZE];
int dummy_filters_enabled_flag;/* saved in setup/.fe */

if(debug_flag)
	{
	fprintf(stdout, "load_newsserver_setting(): arg name=%s\n",\
	name);
	}

/* argument check */
if(! name) return(0);
if(strlen(name) == 0) return(0);
if(strstr(name, "/") != 0) return(0);

pa = lookup_newsserver(name);
if(!pa)
	{
	if(debug_flag)
		{
		fprintf(stdout, "load_newsserver_setting(): no such entry %s\n",\
		name);
		}
	return(0);
	}

news_server_port = pa -> news_server_port;
sprintf(temp, "%d", pa -> news_server_port);
fl_set_input(fd_NewsFleX -> news_server_port_input_field, temp);

mail_server_port = pa -> mail_server_port;
sprintf(temp, "%d", pa -> mail_server_port);
fl_set_input(fd_NewsFleX -> mail_server_port_input_field, temp);

set_tabs(pa -> tab_size);
sprintf(temp, "%d",  pa -> tab_size);
fl_set_input(fd_NewsFleX -> tab_size_input_field, temp);

ptr = pa -> mail_server_name;
free(mail_server_name);
mail_server_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> mail_server_input_field, mail_server_name);

ptr = pa -> user_email;
free(user_email_address);
user_email_address = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> user_email_input_field, user_email_address);

ptr = pa -> real_name;
free(real_name);
real_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> real_name_input_field, real_name);

ptr = pa -> username;
free(server_username);
server_username = strsave(ptr);
fl_set_input(fd_NewsFleX -> server_username_input_field, server_username);

ptr = pa -> password;
free(server_password);
server_password = strsave(ptr);
fl_set_input(fd_NewsFleX -> server_password_input_field, server_password);

maximum_headers = pa -> maximum_headers;
sprintf(temp, "%d", pa -> maximum_headers);
fl_set_input(fd_NewsFleX -> maximum_headers_per_group_input_field, temp);
	
connect_to_news_server_timeout = pa -> connect_to_news_server_timeout;
sprintf(temp, "%d", pa -> connect_to_news_server_timeout);
fl_set_input(fd_NewsFleX -> connect_to_news_server_timeout_input_field, temp);

connect_to_mail_server_timeout = pa -> connect_to_mail_server_timeout;
sprintf(temp, "%d", pa -> connect_to_mail_server_timeout);
fl_set_input(fd_NewsFleX -> connect_to_mail_server_timeout_input_field, temp);

ptr = pa -> connect_to_net_script;
free(connect_to_net_program_name);
connect_to_net_program_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> auto_connect_to_net_input_field,\
connect_to_net_program_name);

ptr = pa -> disconnect_from_net_script;
free(disconnect_from_net_program_name);
disconnect_from_net_program_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> auto_disconnect_from_net_input_field,\
disconnect_from_net_program_name);

ptr = pa -> auto_execute_script;
free(auto_execute_program_name);
auto_execute_program_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> auto_execute_application_input_field,\
auto_execute_program_name);

ptr = pa -> net_device_pid;
free(pid_test_program_name);
pid_test_program_name = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> pid_test_input_field, pid_test_program_name);

ptr = pa -> get_new_headers_time;
free(get_new_headers_time);
get_new_headers_time = (char *) strsave(ptr);
fl_set_input(fd_NewsFleX -> get_new_headers_time_input_field,\
get_new_headers_time);

/* flags */
sscanf(pa -> flags, "%d %d %d %d %d %d %d %d %d %d %d %d %d",\
&enable_description_popup_flag,\
&auto_connect_to_net_flag,\
&auto_disconnect_from_net_flag,\
&auto_execute_program_flag,\
&auto_get_new_headers_flag,\
&dummy_filters_enabled_flag,\
&variable_width_flag,\
&insert_headers_offline_flag,\
&request_by_message_id_flag,\
&show_article_once_flag,\
&do_not_use_sendmail_flag,\
&check_incoming_mail_periodically_flag,\
&local_mail_check_interval);

fl_set_button(fdui -> setup_form_check_local_mail_button,\
check_incoming_mail_periodically_flag);
if(check_incoming_mail_periodically_flag)
	{
	fl_set_object_label(fdui -> setup_form_check_local_mail_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> setup_form_check_local_mail_button, "NO");
	}

sprintf(temp, "%d", local_mail_check_interval);
fl_set_input(fdui -> setup_form_local_mail_check_interval_input_field, temp);


fl_set_button(fdui -> width_button, variable_width_flag);
if(variable_width_flag)
	{
	fl_set_object_label(fdui -> width_button, "VARIABLE");
	fl_set_object_lstyle(\
	fdui -> summary_editor_input_field, FL_NORMAL_STYLE);
	fl_set_object_lstyle(\
	fdui -> posting_body_editor, FL_NORMAL_STYLE);
	fl_set_object_lstyle(\
	fdui -> article_body_input_field, FL_NORMAL_STYLE);
	}
else
	{
	fl_set_object_label(fdui -> width_button, "FIXED");
	fl_set_object_lstyle(\
	fdui -> summary_editor_input_field, FL_FIXED_STYLE);
	fl_set_object_lstyle(\
	fdui -> posting_body_editor, FL_FIXED_STYLE);
	fl_set_object_lstyle(\
	fdui -> article_body_input_field, FL_FIXED_STYLE);
	}

fl_set_button(fdui -> desc_button, enable_description_popup_flag);
if(enable_description_popup_flag)
	{
	fl_set_object_label(fdui -> desc_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> desc_button, "NO");
	}

fl_set_button(fdui -> auto_connect_to_net_button, auto_connect_to_net_flag);
if(auto_connect_to_net_flag)
	{
	fl_set_object_label(fdui -> auto_connect_to_net_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_connect_to_net_button, "NO");
	}

fl_set_button(fdui -> auto_disconnect_from_net_button,\
auto_disconnect_from_net_flag);
if(auto_disconnect_from_net_flag)
	{
	fl_set_object_label(fdui -> auto_disconnect_from_net_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_disconnect_from_net_button, "NO");
	}

fl_set_button(fdui -> auto_execute_application_button,\
auto_execute_program_flag);
if(auto_execute_program_flag)
	{
	fl_set_object_label(fdui -> auto_execute_application_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_execute_application_button, "NO");
	}

fl_set_button(fdui -> get_new_headers_enable_button,\
auto_get_new_headers_flag);
if(auto_get_new_headers_flag)
	{
	fl_set_object_label(fdui -> get_new_headers_enable_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> get_new_headers_enable_button, "NO");
	}

fl_set_button(fdui -> setup_form_insert_headers_button,\
insert_headers_offline_flag);
if(insert_headers_offline_flag)
	{
	fl_set_object_label(fdui -> setup_form_insert_headers_button, "OFFLINE");
	}		
else
	{
	fl_set_object_label(fdui -> setup_form_insert_headers_button, "ONLINE");
	}

fl_set_button(fdui -> setup_form_get_by_message_id_button,\
request_by_message_id_flag);
if(request_by_message_id_flag)
	{
	fl_set_object_label(fdui -> setup_form_get_by_message_id_button, "MSG ID");
	}		
else
	{
	fl_set_object_label(fdui -> setup_form_get_by_message_id_button, "NUMBER");
	}

fl_set_button(fdui -> setup_form_show_only_once_button,\
show_article_once_flag);
if(show_article_once_flag)
	{
	fl_set_object_label(fdui -> setup_form_show_only_once_button, "ONCE");
	}		
else
	{
	fl_set_object_label(fdui -> setup_form_show_only_once_button, "ALWAYS");
	}

fl_set_button(fdui -> setup_form_do_not_use_sendmail_button,\
do_not_use_sendmail_flag);
if(do_not_use_sendmail_flag)
	{
	fl_set_object_label(fdui -> setup_form_do_not_use_sendmail_button,\
	"NO");
	}
else
	{
    fl_set_object_label(fdui -> setup_form_do_not_use_sendmail_button,\
	"YES");
	}

return(1);
}/* end function load_newsserver_setting */


int add_newsserver(char *newsserver)
{
int illegal;
struct newsserver *pa;
char temp[TEMP_SIZE];
FILE *exec_filefd;
char period[TEMP_SIZE];
char name[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "add_newsserver(): arg name=%s", newsserver);
	}

/* argument check */
illegal = 0;
if(! newsserver) return(0);
if(strlen(newsserver) == 0) illegal = 1;
if(strstr(newsserver, "/") != 0) illegal = 1;
if(strstr(newsserver, " ") != 0) illegal = 1;
if(strcmp(newsserver, "setup") == 0) illegal = 1;
if(strcmp(newsserver, "postings") == 0) illegal = 1;
if(illegal)
	{
	fl_show_alert("Illegal name", newsserver, "command cancelled", 0);
	return(0);
	}

/* ALWAYS CURRENT! */
strcpy(period, "current");
sprintf(name, "%s %s", newsserver, period);

pa = lookup_newsserver(name);
if(pa)
	{
	fl_show_alert(\
	"News server exists", newsserver, "command cancelled", 0);

	if(debug_flag)
		{
		fprintf(stdout, "add_newsserver(): %s exists, returning\n",\
		name);
		}
	return(0);/* do not select */
	}

sprintf(temp,\
"News server %s does not exist\nCreate entry?",\
name);
if(! fl_show_question(temp, 1) )/* 1 = mouse on yes */
	{
	return(0);
	}

pa = install_newsserver_at_end_of_list(name);
if(! pa)
	{
	fl_show_alert(\
	"add_newsserver():",\
	"cannot install news server", \
	"command cancelled", 0);
	return(0);
	}

if(! save_newsserver_setting(name) )
	{
	fl_show_alert(\
	"add_newsserver():",\
	"call to save_newsserver_setting() failed",\
	"command cancelled", 0);
	return(0);
	}

/* create news server directory */
sprintf(temp, "mkdir %s/.NewsFleX/%s",\
home_dir, newsserver);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* create database directory */
sprintf(temp, "mkdir %s/.NewsFleX/%s/%s",\
home_dir, newsserver, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* create postings directory */
sprintf(temp, "mkdir %s/.NewsFleX/postings/%s",\
home_dir, period);
exec_filefd = popen(temp, "r");
pclose(exec_filefd);

/* test if groups.dat exists */
sprintf(temp, "%s/.NewsFleX/%s/groups.dat",\
home_dir, newsserver);
exec_filefd = fopen(temp, "r");
/* if groups.dat does not exist, create an empty one */
if(! exec_filefd)
	{
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
		"add_newsserver(): file %s exists, not creating it\n", temp);
		}
	fclose(exec_filefd);
	}

/* test if subscribed_groups.dat exists */
sprintf(temp, "%s/.NewsFleX/%s/%s/subscribed_groups.dat",\
home_dir, newsserver, period);
exec_filefd = fopen(temp, "r");
/* if subscribed_groups.dat does not exist, create an empty one */
if(! exec_filefd)
	{
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
		"add_newsserver(): file %s exists, not creating it\n", temp);
		}
	fclose(exec_filefd);
	}

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
		"add_newsserver(): file %s exists, not creating it\n", temp);
		}
	fclose(exec_filefd);
	}

return(1);
}/* end function add_newsserver */


int select_newsserver(char *name)
{
int a;
char newsserver[TEMP_SIZE];
char database[TEMP_SIZE];
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "select_newsserver(): arg name=%s\n",\
	name);
	}

/* argument check */
if(! name) return(0);
if(strlen(name) == 0) return(0);
if(strstr(name, "/") != 0) return(0);

a = sscanf(name, "%s %s", newsserver, database);
if(a != 2) return(0);

if(! load_newsserver_setting(name) )
	{
	if(debug_flag)
		{
		fprintf(stdout, "select_newsserver(): no such entry %s\n",\
		name);
		}
	return(0);
	}

/* set the global (selected news server) */

free(news_server_name);
news_server_name = strsave(newsserver);

free(database_name);
database_name = strsave(database);

/* newsservers.dat modified (first line, newsserver) */
save_newsservers();

/* set the title for the main form */
set_title();

reset_groups();
reset_articles();

/* set some defaults */
groups_show_selection = SHOW_ONLY_SUBSCRIBED_GROUPS;
articles_menu_selection = SUBSCRIBE;
/* this should NOT be modified, else show_browser() gets confused */
/*visible_browser = ARTICLE_LIST_BROWSER;*/
news_server_status = DISCONNECTED;
news_server_mode = POSTING_ALLOWED;
mail_server_status = DISCONNECTED;
groups_display_filter = strsave("");
selected_group = strsave("");
selected_article = 0;
/*net_status = UNKNOWN;*/

select_first_group_and_article();

ptr = (char *) load_setting(".fe");
if(ptr)
	{
	filters_enabled_flag = atoi(ptr); 
	free(ptr);
	}
else filters_enabled_flag = 1;
fl_set_button(fdui -> filter_enable_button, filters_enabled_flag);

show_browser(GROUP_LIST_BROWSER);

load_general_settings();

load_fontsizes();

/* redraw the display */
refresh_screen(selected_group);

group_list_button_cb(fdui -> group_list_button, 0);
ask_question_flag = -1;

return(1);
}/* end function select_newsserver */


int show_newsservers(int position)
{
char temp[TEMP_SIZE];
struct newsserver *pa;
char formatstr[20];
int browser_line;
int browser_topline;
int first_new_newsserver_line;
int find_first_new_newsserver_flag;

int maxline;

if(debug_flag)
	{
	fprintf(stdout, "show_newsservers(): arg position=%d\n", position);
	}

load_newsservers();
sort_newsservers();

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> newsserver_form_browser);

fl_freeze_form(fd_NewsFleX -> newsserver_form);

fl_clear_browser(fdui -> newsserver_form_browser);

first_new_newsserver_line = -1;/* keep gcc -Wall from complaining */
find_first_new_newsserver_flag = 1;
browser_line = 1;
for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(position == SHOW_UNREAD_AT_TOP)
		{
		/* want to show the first unaccessed at top of display */
		if(find_first_new_newsserver_flag)
			{
			if(pa -> status == 1)
				{
				first_new_newsserver_line = browser_line;
				find_first_new_newsserver_flag = 0;
				}
			}
		}
	
	strcpy(formatstr, "@f");
	if(pa -> status == 0) strcat(formatstr, "@C0");/* black */
	
	sprintf(temp, "%s %s",\
	formatstr, pa -> name);

	fl_add_browser_line(fdui -> newsserver_form_browser, temp);
	pa -> browser_line = browser_line;/* first one is 1 */
	browser_line++;
	}/* end for all elements in list */
	
/* re adjust browser for same position of top line (vertical slider) */	
if(position == SHOW_UNREAD_AT_TOP)
	{
	fl_set_browser_topline(fdui -> newsserver_form_browser, first_new_newsserver_line);
	}
if(position == SHOW_BOTTOM)
	{
	/* make the status of the last newsserver visible */
	maxline = fl_get_browser_maxline(fdui -> newsserver_form_browser);
	fl_set_browser_topline(fdui -> newsserver_form_browser, maxline);
	}
/*
if(position == SHOW_TOP)
	{
	fl_set_browser_topline(fdui -> newsserver_form_browser, 1);
	}
*/
if(position == SHOW_SAME_POSITION)
	{
	fl_set_browser_topline(fdui -> newsserver_form_browser, browser_topline);
	}	

fl_unfreeze_form(fd_NewsFleX -> newsserver_form);

return(1);
}/* end function show_newsservers */


char *line_to_newsserver(int line)
{
struct newsserver *pa;
extern long atol();

if(debug_flag)
	{
	fprintf(stdout, "line_to_newsserver(): arg line=%d\n", line);
	}

/* argument check */
if(line < 0) return(0);

for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		return(pa -> name);
		}
	}

/* no save_newsservers, nothing was changed */
return(0);
}/* end function line_to_newsserver_id */


int newsserver_to_line(char *name, int *line)
{
struct newsserver *pa;

if(debug_flag)
	{
	fprintf(stdout, "newsserver_to_line(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = lookup_newsserver(name);
if(! pa) return(0);

*line = pa -> browser_line;
return(1);
}/* end function newsserver_to_line */


char *detect_new_period_and_return_old_period(int *new)
{
char present_period[TEMP_SIZE];
char weekday[TEMP_SIZE], month[TEMP_SIZE];
int monthday, hour, minute, second, year;
char *old_period;

if(debug_flag)
	{
	fprintf(stdout, "detect_new_period_and_return_old_period(): arg none\n");
	}

/* get present date and time */
ogettime(weekday, month, &monthday, &hour, &minute, &second, &year);

sprintf(present_period, "%s-%d", month, year); 

old_period = load_setting("current_period");
if(! old_period)
	{
	old_period = strsave("");
	/* first startup if no file create one */
	save_setting("current_period", present_period);
	}

if(strcmp(old_period, present_period) != 0)
	{
	save_setting("current_period", present_period);
	*new = 1;
	}
else
	{
	*new = 0;
	}

return(old_period);
}/* end function detect_new_period_and_return_old_period */


int move_current_database_and_create_new_current(\
char *newsserver, char *newname)
{
int a;
FILE *fileptr;
FILE *sg_fileptr;
char temp[TEMP_SIZE];
char group_name[TEMP_SIZE];
struct newsserver *pa;
FILE *finptr, *foutptr;
char name[TEMP_SIZE];
int retrieval_flag, have_body_flag, follow_flag;
FILE *execfd;

if(debug_flag)
	{
	fprintf(stdout,\
	"move_current_database_and_create_new_current():\n\
	arg newsserver=%s newname=%s\n",\
	newsserver, newname);
	}

/* argument check */
if(! newsserver) return(0);
if(strlen(newsserver) == 0) return(0);
if(strstr(newsserver, "/") != 0) return(0);
if(strstr(newsserver, " ") != 0) return(0);
if(! newname) return(0);
if(strlen(newname) == 0) return(0);
if(strstr(newname, "/") != 0) return(0);
if(strstr(newname, " ") != 0) return(0);

/* create a new entry in newsservers.dat */
sprintf(temp, "%s %s", newsserver, newname);
pa = lookup_newsserver(temp);
if(pa)
	{
	fl_show_alert(temp, "already exists", "command cancelled", 0);
	if(debug_flag)
		{
		fprintf(stdout,\
		"move_current_database_and_create_new_current():\n\
		%s already exists, returning error\n",\
		temp);
		}
	return(0);
	}

pa = install_newsserver_at_end_of_list(temp);
if(! pa)
	{
	fl_show_alert("cannot install", temp, "command cancelled", 0);
	if(debug_flag)
		{
		fprintf(stdout,\
		"move_current_database_and_create_new_current():\n\
		cannot install %s in newsserver structure, returning error\n",\
		temp);
		}
	return(0);
	}

if(! save_newsserver_setting(temp) ) /* does a save_newsservers() */
	{
	fl_show_alert(\
	"move_current_database_and_create_new_current():",\
	"call to save_newsserver_setting() failed",\
	"command cancelled", 0);
	if(debug_flag)
		{
		fprintf(stdout,\
		"move_current_database_and_create_new_current():\n\
		call to save newsserver_setting for %s failed, returning error\n",\
		temp);
		}
	return(0);
	}

/*save_newsservers();*/

/* rename current to newname */
sprintf(temp,\
"mv %s/.NewsFleX/%s/current %s/.NewsFleX/%s/%s",\
home_dir, newsserver, home_dir, newsserver, newname);
fileptr = popen(temp, "r");
pclose(fileptr);

/* create new database named current */
sprintf(temp,\
"mkdir %s/.NewsFleX/%s/current",\
home_dir, newsserver);
fileptr = popen(temp, "r");
pclose(fileptr);

/* copy subscribed_groups.dat */
sprintf(temp,\
"cp %s/.NewsFleX/%s/%s/subscribed_groups.dat %s/.NewsFleX/%s/current/",\
home_dir, newsserver, newname,\
home_dir, newsserver);
fileptr = popen(temp, "r");
pclose(fileptr);

/* create directories for all groups in renamed database */
sprintf(temp,\
"%s/.NewsFleX/%s/current/subscribed_groups.dat",\
home_dir, newsserver);
sg_fileptr = fopen(temp, "r");
if(! sg_fileptr)
	{
	fl_show_alert("Cannot open file for read", temp, "aborting", 0);
	/* show for a while */
	sleep(3);
	if(debug_flag)
		{
		fprintf(stdout,\
		"move_current_database_and_create_new_current():\n\
		Cannot open file %s for read, aborting\n",\
		temp);
		}
	exit(1);
	} 

to_command_status_display(\
"creating subscribed groups in new current, this may take a while");

/* for all entries in -newsgroup/current/subscribed_groups.dat */
while(1)
	{
	a = readline(sg_fileptr, temp);
	if(a == EOF) break;
	if(debug_flag)
		{
		fprintf(stdout,\
		"move_current_database_and_create_new_current():\n\
		reading from new subscribed_groups.dat: %s\n", temp);
		}

	/* extract group name (is directory name) */
	sscanf(temp, "%s", group_name);

	/* create directory for the group */
	sprintf(temp,\
	"mkdir %s/.NewsFleX/%s/current/%s",\
	home_dir, newsserver, group_name);
	fileptr = popen(temp, "r");
	pclose(fileptr);

	/*
	Get old articles.dat,
	copy the ones marked follow thread and follow subject to new articles.dat.
	Copy the article header and body (if present) as well.
	*/
	
	/* open old articles.dat for read */
	sprintf(temp,\
	"%s/.NewsFleX/%s/%s/%s/articles.dat",\
	home_dir, newsserver, newname, group_name);
	finptr = fopen(temp, "r");
	if(! finptr)
		{
		/* perhaps write an empty articles.dat  */
		
		if(debug_flag)
			{
			fprintf(stdout,\
			"move_current_database_and_create_new_current():\n\
			Cannot open file %s for read, aborting\n",\
			temp);
			}

		exit(1);
		}
	
	/* open new articles.dat for write */
	sprintf(temp,\
	"%s/.NewsFleX/%s/current/%s/articles.dat",\
	home_dir, newsserver, group_name);
	foutptr = fopen(temp, "w");
	if(! foutptr)
		{
		/* perhaps write an empty articles.dat  */

		if(debug_flag)
			{
			fprintf(stdout,\
			"move_current_database_and_create_new_current():\n\
			Cannot open file %s for write, aborting\n",\
			temp);
			}

		exit(1);
		}
	
	/* copy all lines in articles.dat */ 
	while(1)
		{
		a = readline(finptr, temp);
		if(a == EOF)
			{
			a = 0;/* signal no error */
			break;/* end of file readline closes finptr */
			}

		/* name and flags */
		sscanf(temp, "%s %d %d %d",\
		name, &retrieval_flag, &have_body_flag, &follow_flag);
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* Subject: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* From: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* Content-Type: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* References: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* Mesage-ID: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* Attachment_name: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* file format error */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		/* Date: */
		a = readline(finptr, temp);
		if(a == EOF) break;/* MODIFIED WAS COMMENTED OUT */
		if(follow_flag) fprintf(foutptr, "%s\n", temp); /* write to output file */ 
	
		if(follow_flag)
			{
			/* copy old header, body, attachment, whatever */
			sprintf(temp,\
			"cp %s/.NewsFleX/%s/%s/%s/*.%s %s/.NewsFleX/%s/current/%s/",\
			home_dir, newsserver, newname, group_name, name,\
			home_dir, newsserver, group_name);
			execfd = popen(temp, "r");
			pclose(execfd);
			}
				
		}/* end while all lines from old articles.dat */

	if(a == EOF)
		{
		/* file format error */

		if(debug_flag)
			{
			fprintf(stdout,\
			"move_current_database_and_create_new_current():\n\
			file format error early EOF last line read was %s\n",\
			temp);
			}

		exit(1);
		}

	/* close the new articles.dat */
	fclose(foutptr);
	}/* end all entries in subscribed_groups.dat */
/* sg_fileptr closed by readline() */
show_newsservers(SHOW_UNREAD_AT_TOP);

return(1);
}/* move_current_database_and_create_new_current */


int set_title()
{
char temp[TEMP_SIZE];

sprintf(temp, "%s:  %s %s  postings %s",\
VERSION, news_server_name, database_name, postings_database_name);
fl_set_form_title(fdui -> NewsFleX, temp);

return(1);
}/* end function set_title */


struct workinglist *lookup_workinglist(char *name)
{
struct workinglist *pa;

/* argument check */
if(! name) return(0);

for(pa = workinglisttab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_workinglist */


struct workinglist *install_workinglist_at_end_of_list(char *name)
{
struct workinglist *plast, *pnew;
struct workinglist *lookup_workinglist();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_workinglist_at_end_off_list(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pnew = lookup_workinglist(name);
if(pnew)
	{
	/* free previous definition */
/*	free(pnew -> subject);*/
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct workinglist *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = workinglisttab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points to zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !workinglisttab[0] ) workinglisttab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
workinglisttab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_workinglist */


int delete_all_workinglists()/* delete all entries from table */
{
struct workinglist *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_workinglists() arg none\n");
	}

while(1)
	{	
	pa = workinglisttab[0];
	if(! pa) break;
	workinglisttab[0] = pa -> nxtentr;
	free(pa -> name);

	free(pa);/* free structure */
	}/* end while all structures */

workinglisttab[1] = 0;
return(1);
}/* end function delete_all_workinglists */


int create_new_newsserver_databases(char *oldperiod)
{
int a;
struct newsserver *pa;
struct workinglist *pb;
char n_server[TEMP_SIZE];
char period[TEMP_SIZE];
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "create_new_newsserver_databases(): arg oldperiod=%s\n",\
	oldperiod);
	}

/* argument check */
if(! oldperiod) return(0);

/* clear working list */
delete_all_workinglists();
	
/*
The reason the separate list is used, is that the news server list will be
modified by move_current_database_and_create_new_current().
*/

/* add all current to a working list */
for(pa = newsservertab[0]; pa != 0; pa = pa -> nxtentr)
	{
	a = sscanf(pa -> name, "%s %s", n_server, period);
	if(a != 2) continue;/* some format error, but try other servers */

	if(strcmp(period, "current") != 0) continue;
	
	if(debug_flag)
		{
		fprintf(stdout,\
		"create_new_newsserver_databases():\n\
		adding to working list pa->name=%s\n",\
		pa -> name);
		}

	install_workinglist_at_end_of_list(n_server);
	}/* end for */
		
/* process workinglist */
for(pb = workinglisttab[0]; pb != 0; pb = pb -> nxtentr)
	{
	sprintf(temp,\
	"Move database\n'%s current' to '%s %s'?\n",\
	pb -> name, pb -> name, oldperiod);	
	
	if(! fl_show_question(temp, 1) ) continue;/* 1 = mouse on YES */

	if(debug_flag)
		{
		fprintf(stdout,\
		"create_new_newsserver_databases():\n\
		Going to move %s current to %s %s(y/n)?\n",\
		pb -> name, pb -> name, oldperiod);
		}

	move_current_database_and_create_new_current(pb -> name, oldperiod);
	}/* end for all entries in workinglist */

return(1);
}/* end function create_new_newsserver_databases */


char *find_next_newsserver_and_period(char *current_newsserver_and_period)
{
struct newsserver *pa;

if(debug_flag)
	{
	fprintf(stdout,
	"find_next_newsserver_and_period(): arg current_newsserver_and_period=%s\n",\
	current_newsserver_and_period);
	}

/* argument check */
if(! current_newsserver_and_period) return(0);

pa = lookup_newsserver(current_newsserver_and_period);
if(! pa) return(0); /* newsserver_and_period does not exist in list*/

/* set start search for current_newsserver_and_period */
pa = pa -> nxtentr;/* point to next newsserver_and_period */
if(debug_flag)
	{
	fprintf(stdout, "pa -> nxtentr=%lu\n", (long)pa);
	}

/* test if last in list, if so go to start list */
if(! pa) pa = newsservertab[0];

/* test for empty list */
if(! pa) return(0);

if(debug_flag)
	{
	fprintf(stdout,\
	"find_next_newsserver_and_period(): next pa->name=%s\n",\
	pa -> name);
	}

return(pa -> name);
}/* end function find_next_newsserver_and_period */


int swap_position(struct newsserver *ptop, struct newsserver *pbottom)
{
struct newsserver *punder;
struct newsserver *pabove;

if(debug_flag)
	{
	fprintf(stdout,\
	"swap_position(): swapping top=%lu bottom=%lu\n", ptop, pbottom);
	}

/* argument check */
if(! ptop) return 0;
if(! pbottom) return 0;

/* get one below the bottom */
punder = pbottom -> prventr;/* could be zero if first entry */
if(debug_flag)
	{
	fprintf(stdout,\
	"swap_position(): punder=%lu\n", punder);
	}

/* get the one above the top */
pabove = ptop -> nxtentr;/* could be zero if last entry */
if(debug_flag)
	{
	fprintf(stdout,\
	"swap_position(): pabove=%lu\n", pabove);
	}

/* the next pointer in punder (or newsservertab[0]) must now point to ptop */
if(! punder)
	{
	newsservertab[0] = ptop; 
	}
else
	{
	punder -> nxtentr = ptop;
	}

/* the prev pointer in in ptop must now point to punder */
ptop -> prventr = punder;/* could be zero if first entry */

/* the next pointer in ptop must now point to pbottom */
ptop -> nxtentr = pbottom;

/* the next pointer in pbottom must now point to pabove */
pbottom -> nxtentr = pabove;

/* mark last one in newsservertab */
if(! pabove)
	{
	newsservertab[1] = pbottom;
	}
else
	{
	/* the prev pointer in pabove must now point to pbottom */
	pabove -> prventr = pbottom;
	}

/* the prev pointer in pbottom must now point to ptop */
pbottom -> prventr = ptop;

/* return swapped */
return 1;
}/* end function swap_position */


