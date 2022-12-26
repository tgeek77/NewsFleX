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

FL_APPEVENT_CB my_idle_cb;

struct passwd *userinfo;

struct in_addr local_ip_address;

int queue_size;

struct command
	{
	char *name;
	int type;
	int send_flag;
	char *group; /* used as URL in url_get() */
	long article; /* used as options in url_get() */
	long sequence;
	struct command *nxtentr;
	struct command *prventr;
	};
struct command *commandtab[2];


struct command *lookup_command(char *name)
{
struct command *pa;

if(debug_flag) 
	{
	fprintf(stdout, "lookup_command(): arg name=%s\n", name);
	}

for(pa = commandtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0); /* not found */
}


struct command *install_command_at_end_of_list(char *name)
{
struct command *plast, *pnew;
struct command *lookup_command();

if(debug_flag)
	{
	fprintf(stdout, "install_command_at_end_of_list(): arg name=%s\n", name);
	}

pnew = lookup_command(name);
if(pnew)
	{
	/* else already there */
	if(debug_flag)
		{
		fprintf(stdout,\
		"install_command_at_end_of_list(): SERIOUS ERRROR, ALREADY THERE\n");
		}
	return(0);
	}

/* create new structure */
pnew = (struct command *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);
pnew -> group = strsave("");
if(! pnew -> group) return(0);

/* get previous structure */
plast = commandtab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !commandtab[0] ) commandtab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
commandtab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_command_at_end_of_list */


int delete_command(char *name)/* delete entry from double linked list */
{
struct command *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_command(): arg name=%s\n", name);
	}

/* parameter check */
if(! name) return(0);

pa = commandtab[0];
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
	/* if first one, modify commandtab[0] */
	if(pprev == 0) commandtab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify commandtab[1] */
	if(pnext == 0) commandtab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	/* delete structure data */	
	free(pdel -> name);/* free name */
	free(pdel -> group);/* free group */
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_command */


int delete_all_commands()/* delete all entries from table */
{
struct command *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_commands(): arg none\n");
	}

while(1)
	{
	pa = commandtab[0];
	if(! pa) break;
	commandtab[0] = pa -> nxtentr;/* commandtab entry points to next one,
												            this could be 0 */
	free(pa -> name);/* free name */
	free(pa -> group);
	free(pa);/* free structure */
	}/* end for all structures */
commandtab[1] = 0;
return(0);/* not found */
}/* end function delete_all_commands */


int add_to_command_queue(char *group, long article, int command)
{
struct command *pa;
extern char *strsave();
static long sequence;
char temp[50];

if(debug_flag)
	{
	fprintf(stdout,\
	"add_to_command_queue(): arg group=%s article=%ld command=%d\n",\
	group, article, command);
	}

/* argument check */
if(! group) return(0);
if(! command) return(0);
if(article < 0) return(0);

if(command == POST_ARTICLE)
	{
	if(news_server_mode != POSTING_ALLOWED)
		{
		my_show_alert(\
		"No posting allowed on this server", "posting cancelled", "");
		return(0);
		}
	}
/* test if command already in queue */
for(pa = commandtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(pa -> article == article)
		{
		if(pa -> type == command)
			{
			if(strcmp(pa -> group, group) == 0) return(1);/* already there */
			}
		}	
	}/* end for all structures */

sequence++;/* avoid the zero for a start */

sprintf(temp, "%ld", sequence);
pa = install_command_at_end_of_list(temp);
if(!pa) return(0);

pa -> type = command;

free(pa -> group);
pa -> group = strsave(group);
if(! pa -> group) return(0);

pa -> article = article;
pa -> send_flag = 0;
return(1);
}/* end function add_to_command_queue */


int process_command_queue()
/*
called from idle callback
connect to server,
send a command from the structure,
remove that entry from the structure,
return and wait for a server reply (in io_read_cb)
if no more entries in structure close server connection.
*/
{
struct command *pa;
char temp[TEMP_SIZE];
char info[TEMP_SIZE];
extern int idle_cb();

if(debug_flag)
	{
	fprintf(stdout, "process_command_queue(): arg none\n");
	}

for(pa = commandtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* if not on the net for certain */
	if( (net_status == DISCONNECTED) || (net_status == UNKNOWN) )
		{
		/* connect to the internet if auto_connect_to_net flag set */
		if(auto_connect_to_net_flag)
			{
			if(! auto_connect_to_net() )
				{
				/* clear command status display */
				to_command_status_display("");
				
				/* connection to net failed */
				return(0);
				}

			/* now are connected, try to get the mail */
			if(auto_execute_program_flag)
				{
				if(! execute_get_mail_script() )
					{
					/* no abort, do nothing, try the news server */
					}
				else check_mail_local();
				
				}/* end if auto_execute_program_flag */
			}/* end if auto_connect_to_net_flag */
		}/* end net_status disconnected or unknown */ 

	/* connect to server, if not already there (not for email) */
	if( (news_server_status == DISCONNECTED) &&\
		(pa -> type != EMAIL_ARTICLE) &&\
		(pa -> type != URL_GET) )
		{
		/* possible backup here if online */
		
		if(! connect_to_news_server(news_server_name, news_server_port) )
			{
			/* clear command status display */
			to_command_status_display("");

			if(auto_disconnect_from_net_flag) auto_disconnect_from_net();
			return(0);
			}
		
		/* start the timer */
		online_timer = time(0);
		bytes_received = 0;
		bytes_send = 0;
			
		fl_add_io_callback(socketfd,\
		FL_READ, (FL_IO_CALLBACK) news_io_read_cb, 0);
		
		if(last_acquire_group) free(last_acquire_group);
		last_acquire_group = 0;/* no group */
			
		clear_new_articles_list();
		}
		
	/* report the tasks remaining */
	queue_size = get_command_queue_size();
	sprintf(temp, "Tasks %d", queue_size);
	fl_set_object_label(fdui -> line_status_display, temp);

	/* wait for the server to say 200 or 201 */
	if(! news_server_mode)
		{
		expect_numeric_flag = 1;
		command_in_progress = WAIT_RESPONSE;
		return(1);
		}

	/* NewsFleX_cb.c needs to know what we are doing */
	free(acquire_group);
	acquire_group = strsave(pa -> group);
	acquire_article = pa -> article;/* long */

	/* send the command */
	if(pa -> type == LIST)
		{
		sprintf(info, "Refreshing group list");
		sprintf(temp, "LIST\n");
		command_in_progress = LIST;
		}
	if(pa -> type == GET_NEW_HEADERS_IN_GROUP)
		{
		sprintf(info, "Getting new headers in group %s", pa -> group);
		sprintf(temp, "GROUP %s\n", pa -> group);
		}	 	
	if(pa -> type == GET_ARTICLE_BODY)
		{
		sprintf(info, "Requesting article body");

		if(request_by_message_id_flag)
			{
			calculate_and_display_tasks();

/*			send_all_message_ids_to_server();*/
			/* this one uses a second list and sends group by group */
			send_all_message_ids_to_server2();

			pa -> send_flag = atol(pa -> name);
			command_in_progress = pa -> type;
			expect_numeric_flag = 1;

			delete_command(pa -> name);
			/* 
			pa NO LONGER DEFINED/
			now waiting for a response from the server,
			after that is processed the command_in_progress is reset
			*/
			/* inform user what is happening */
			to_command_status_display(info);	

			return(1);/* the next time here called from idle_cb */
			}

		/* 
		like get new headers, but command_in_progress is
		checked for in io_callback, and then ONE body is requested
		for this group. (we already have the header)
		
		if not in this group select group,
		this will also retrieve the article body.
		*/
		if( (! last_acquire_group) ||\
		(strcmp(pa -> group, last_acquire_group) != 0) )
			{
			sprintf(temp, "GROUP %s\n", pa -> group);
			}
		else /* get only body */
			{
			sprintf(temp, "BODY %ld\n", pa -> article);
			}
		}
	if(pa -> type == POST_ARTICLE)
		{
		sprintf(info, "posting article %ld", pa -> article);
		posting_article = pa -> article;
		sprintf(temp, "POST\n");
		}
	if(pa -> type == EMAIL_ARTICLE)
		{
		sprintf(info, "mailing article %ld", pa -> article);
   		to_command_status_display(info);

		command_in_progress = pa -> type;
		if(! do_not_use_sendmail_flag)
			{
			post_email_via_sendmail(pa -> article);
			}
		else
			{
			post_email(pa -> article);
			}

		delete_command(pa -> name);

   	    refresh_screen("");/* using dummy for group */

        return(1);/* the next time here called from idle_cb */
		}
	if(pa -> type == URL_GET)
		{
		sprintf(info, "requesting URL %s", pa -> group);
   		to_command_status_display(info);

		command_in_progress = pa -> type;

		/*
		If the next one has the same hostname send KEEP_ALIVE_URL
		and http should not reconnect either.
		*/
		
		/*
		pa -> group holds URL,
		pa -> article holds 65536 * url_depth + option flags.
		*/
		url_get(pa -> group, (int)pa -> article);

		delete_command(pa -> name);
   	    
   	    refresh_screen("");/* using dummy for group */

        return(1);/* the next time here called from idle_cb */
		}

	send_to_news_server(temp);	

	pa -> send_flag = atol(pa -> name);
	command_in_progress = pa -> type;
	expect_numeric_flag = 1;

	delete_command(pa -> name);
	/* 
	pa NO LONGER DEFINED/
	now waiting for a response from the server,
	after that is processed the command_in_progress is reset
	*/
	/* inform user what is happening */
	to_command_status_display(info);	
	return(1);/* the next time here called from idle_cb */
	}/* end for all structures */

/*
Save the last articles structure, not done anymore in add_article_body(),
now only save structure for each group, not each article body, to improve
speed.
I am aware this reduces data security, as you may loose all new bodies in a
group if sig KILL happens during the xfer.
*/
save_articles();

/* 
command queue now empty 
There may be new headers that are marked by the thread routines in
articles.c.
*/

/*
Test if there are any groups that require all articles to be retrieved
(groups menu option),  if any, mark all articles in these groups,
*/
if(mark_all_bodies_in_get_always_all_bodies_groups() )
	/*
	above will add headers for those groups, and return 1 if any unmarked
	found.
	*/
	{
	/* force a get marked article bodies */
	if(debug_flag)
		{
		fprintf(stdout, "process_command_queue(): MARKING ALL IN GROUPS\n");
		}	
	add_marked_article_bodies_in_subscribed_groups_to_command_queue();
	return(1);/* re enter the command queue */
	}

if(new_thread_bodies_flag)
	{
	if(debug_flag)
		{
		fprintf(stdout, "process_command_queue(): MARKING FOR THREAD\n");
		}

	add_marked_article_bodies_in_subscribed_groups_to_command_queue();
 	/* and reset this flag (so no loop). */
	new_thread_bodies_flag = 0;

	if(debug_flag)
		{
		fprintf(stdout, "process_command_queue(): LOOPING FOR THREAD\n");
		}
	return(1);
	}

if(news_server_status != DISCONNECTED)
	{
	send_to_news_server("QUIT\n");

	fl_remove_io_callback(socketfd,\
	FL_READ, (FL_IO_CALLBACK) news_io_read_cb);

	close(socketfd);
	news_server_status = DISCONNECTED;

	save_subscribed_groups();

	if(auto_disconnect_from_net_flag) auto_disconnect_from_net();

	if(insert_headers_offline_flag)
		{
		add_new_headers_from_list("*");  /* this will clear the list */
		}

	set_new_data_flag(1);
	
	if(filters_enabled_flag) set_all_filter_flags_for_new_articles();
	
	if(error_log_present_flag)
		{
		to_command_status_display(\
		"There were online errors, check error log");
		}
	else to_command_status_display("");

	}/* end server status != disconnected */

/* prevent disconnecting twice */
if(net_status != DISCONNECTED)
	{
	if(auto_disconnect_from_net_flag)
		{
		auto_disconnect_from_net();
		}
	else
		{
		/* 
		force a new test for ppp link up, the next time online,
		else if a new online command is given, and the net connection was closed,
		for example by user, then connect to net scrip would not be invoked.
		*/
		net_status = UNKNOWN;
		}
	}/* end if net_status != conencted */

/*
replace any net references with local file references, including moved url's.
*/
modify_all_html_files();

/*
clear the list that prevents going in a loop if in recursive mode,
and downloaded .html files refer to each other.
*/
delete_all_inurls();

/* no more need for these */
delete_all_moved_urls();

return(0);
}/* end function process_command_queue */


int get_command_queue_size()
{
struct command *pa;
int queue_size;

if(debug_flag)fprintf(stdout, "get_command_queue_size() arg none\n");

queue_size = 0;
for(pa = commandtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	queue_size++;
	}/* end for all structures */
return(queue_size);
}/* end function get_command_queue_size */


int auto_disconnect_from_net()
{
extern int idle_cb();
char temp[TEMP_SIZE];
FILE *disconnect_filefd;

if(debug_flag)
	{
	fprintf(stdout, "auto_disconnect_from_net(): arg none\n");
	}

/* parameter check :) */
if(net_status == DISCONNECTED) return(1);/* nothing to do */

if( connected() ) return(1);

to_command_status_display("Starting auto disconnect from net script");

disconnect_filefd = popen(disconnect_from_net_program_name, "r");
if(! disconnect_filefd)
	{
	/* 
	let the user know something is wrong, and he should manually
	disconnect from the ISP, or pay the phone.
	*/
	audio_alarm_flag = 1;/* pushing STOP will stop alarm */
	if(! auto_disconnect_from_net_flag)
		{
		my_idle_cb = fl_set_idle_callback(0, 0);
		fl_show_alert(\
		"Cannot execute file:", disconnect_from_net_program_name,\
		"Cannot disconnect from net", 0);
		fl_set_idle_callback(idle_cb, my_idle_cb);
		}
	else
		{
/*
		fl_show_alert(\
		"Cannot execute file:", disconnect_from_net_program_name,\
		"Cannot disconnect from net", 0);
*/
		sprintf(temp, "Cannot execute file %s cannot disconnect from net",\
		disconnect_from_net_program_name);
		to_command_status_display(temp);
		}
	return(0);
	}/* end if error */
pclose(disconnect_filefd);

to_command_status_display("");

net_status = DISCONNECTED;
return(1);
}/* end function auto_disconnect_from_net */


int to_command_status_display(char *text)
{
if(debug_flag)
	{
	fprintf(stdout, "to_command_status_display(): arg text=%s\n", text);
	}

/* argument check */
if(! text) return(0);
	
fl_set_object_label(fdui -> command_status_display, text);
/* This is a trick to force command status display to be shown */
XSync(fl_get_display(), 0);

return(1);
}/* end function to_command_status_display */


int auto_connect_to_net()
{
char temp[TEMP_SIZE];
extern int idle_cb();
FILE *exec_filefd;
FILE *test_filefd;
int test_seconds;
int we_started_pppd_flag;

if(debug_flag)
	{
	fprintf(stdout, "auto_connect_to_net(): arg none\n");
	}

sprintf(temp, "Auto connect to net: Testing for %s",\
pid_test_program_name);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}

we_started_pppd_flag = 0;
test_seconds = 0;
while(1)
	{
	/*
	If pppd was succesful, it executes the script /etc/ip-up, this script
	must set a semaphore (create a file), that we check for to see if the
	link is up.
	*/
	if(debug_flag)
		{
		fprintf(stdout, "testing for %s seconds=%d timeout=120\n",\
		pid_test_program_name, test_seconds);
		}
	test_filefd = fopen(pid_test_program_name, "r");
	if(! test_filefd)
		{
		if(test_seconds == 0)
			{
			sprintf(temp, "starting connect to net script %s",\
			connect_to_net_program_name);
			if(debug_flag)
				{
				fprintf(stdout, "%s\n", temp);
				}
			to_command_status_display(temp);
			exec_filefd = popen(connect_to_net_program_name, "r");
			if(! exec_filefd)
 				{
				my_idle_cb = fl_set_idle_callback(0, 0);
				fl_show_alert("Cannot execute file:",\
				connect_to_net_program_name, "", 0);
				fl_set_idle_callback(idle_cb, my_idle_cb);
				net_status = UNKNOWN;
				return(0);
				}
			pclose(exec_filefd);
			we_started_pppd_flag = 1;
			}/* end test_seconds = 0 */
		if(test_seconds > 120)
			{
			my_idle_cb = fl_set_idle_callback(0, 0);
			fl_show_alert("Cannot connect to net",\
			"pppd not running", "", 0);
			fl_set_idle_callback(idle_cb, my_idle_cb);
			net_status = DISCONNECTED;
			return(0);
			}
		}
	else 
		{
		fclose(test_filefd);		
		break;
		}
	sleep(1);/* take it easy */
	test_seconds++;
	}/* end while check for pppd up flag file */

net_status = CONNECTED;
if(debug_flag) fprintf(stdout, "connected\n");

return(1);
}/* end function auto_connect_to_net */


int execute_get_mail_script()
{
char temp[TEMP_SIZE];
FILE *exec_filefd;

/* get mail every time we connect to the net */
sprintf(temp, "starting mail script %s",\
auto_execute_program_name);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

exec_filefd = popen(auto_execute_program_name, "r");
if(! exec_filefd)
	{
/*	my_idle_cb = fl_set_idle_callback(0, 0);*/
	my_show_alert(\
	"Cannot execute file:", auto_execute_program_name, "");
/*	fl_set_idle_callback(idle_cb, my_idle_cb);*/
	return(0);
	}
pclose(exec_filefd);

return(1);
}/* end function  execute_get_mail_script */


int calculate_and_display_tasks()
{
/* This function is only used if request_by_message_id_flag is set */
int tasks;
char temp[80];

if(debug_flag)
	{
	fprintf(stdout, "calculate_and_diplay_tasks(): arg none\n");
	}
/*
int acquire_body_count;
int request_by_message_id_flag;
int queue_size;
*/

/*queue_size = get_command_queue_size();*/

/*
Ad the article bodies requested to the tasks, substract the task
of requesting marked article bodies.
*/
tasks = queue_size + acquire_body_count - 1;
sprintf(temp, "Tasks %d", tasks);
fl_set_object_label(fdui -> line_status_display, temp);
/*XSync(fl_get_display(), 0);*/

return(1);
}/* end function calculate_and_display_tasks */


	