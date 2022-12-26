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

FD_NewsFleX *fd_setup;
FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

FL_IOPT fl_iopt;

/*#define SOURCE "0007760667423724"*/
#define SOURCE "0008063760287804871532216899451427988368823684672367684819503760664"


int main(int argc, char *argv[])
{
int i, c;
struct passwd *userinfo;
char *ptr;
char temp[TEMP_SIZE];
char *old_period;
int new;
/*sigset_t block_alarm;*/
FILE *fptr;
int illegal;
     
/*select con io 1 byte at the time*/
setbuf(stdout,NULL);
setbuf(stdin,NULL);

/* set defaults */
debug_flag = 0;
global_posting_wrap_chars = 78;

/* process command line arguments */
for(i = 1; i < argc; i++)
	{
	if(argv[i][0] == '-')
		{
/*fprintf(stdout, "argc=%d argv[%d]=%s\n", argc, i, argv[i]);*/
		if(strcmp(argv[i], "-debug") == 0) debug_flag = 1;
		if(strstr(argv[i], "-a") == argv[i])
			{
			access_code = strsave(argv[i]);
			}
		if(strstr(argv[i], "-so") == argv[i])
			{
			fprintf(stdout, "%s\n", SOURCE);
			exit(1);
			}
		}
	}/* end for all command line arguments */

/* get user info */
userinfo = getpwuid(getuid() );

/* get home directory */ 
home_dir = strsave(userinfo -> pw_dir);
user_name = strsave(userinfo -> pw_name);

socketfd = -1;/* no server connection yet */
command_in_progress = 0;/* no command send to server yet */

/*
prevent executing an empty command queue, then doing an auto disconnect
from net, if it is enabled.
That would kill other connection (such as ftp) that uses the net.
*/
process_command_queue_error_flag = 1;

fl_set_goodies_font(FL_BOLD_STYLE, FL_MEDIUM_SIZE);

/* this must be called before fl_initialize, but with home_dir set */
/* if this fails, default colors will be used */
load_custom_colors();

fl_initialize(&argc, argv, 0, 0, 0);
fd_NewsFleX = create_form_NewsFleX();

/* show the first form */
/*fl_show_form(fd_NewsFleX -> NewsFleX, FL_PLACE_CENTER, FL_FULLBORDER, VERSION);*/
fl_show_form(fd_NewsFleX -> NewsFleX, FL_PLACE_FREE, FL_FULLBORDER, VERSION);

groups_show_selection = SHOW_ONLY_SUBSCRIBED_GROUPS;
articles_menu_selection = SUBSCRIBE;
visible_browser = GROUP_LIST_BROWSER;
news_server_status = DISCONNECTED;
news_server_mode = POSTING_ALLOWED;
mail_server_status = DISCONNECTED;
http_server_status = DISCONNECTED;
ftp_control_server_status = DISCONNECTED;
ftp_data_server_status = DISCONNECTED;
/* these values are fixed */
connect_to_http_server_timeout = 30;
connect_to_ftp_server_timeout = 30; 


groups_display_filter = strsave("");
selected_group = strsave("");
net_status = UNKNOWN;

load_newsservers();
/* news_server_name and database_name set by load_newsservers */
sprintf(temp, "%s current", news_server_name);
select_newsserver(temp);/* also loads general_settings and fontsizes */

load_filters();

load_search_settings();

load_highlight_settings();

load_posting_periods();
select_posting_period("current");

load_subscribed_groups(USE_ALL_DATA);

select_first_group_and_article();

load_helpers();

load_urls();

refresh_screen(selected_group);

ask_question_flag = -1;

/*
going to check if someone tries to 'upgrade' by simply getting the
latest beta.
*/
illegal = 0;
sprintf(temp, "%s/.NewsFleX/setup/.NewsFleXf", home_dir);
fptr = fopen(temp, "r");
if(fptr)/* old copy present */
	{
	c = getc(fptr);
	fclose(fptr);
	if(c != 'z') illegal = 1;/* was accepted for use */
	}/* end if NewsFleXf exists */
/* Note this version will leave ~/.NewsFleX/setup/.NewsFleXff */
	
sprintf(temp, "%s/.gptr", home_dir);/* left by version beta <- 0.8.1 */
fptr = fopen(temp, "r");
if(fptr)
	{
	fclose(fptr);
	illegal = 1;
	}/* end if ~/.gptr exists */
/* Note: this version will leave ~.hptr */

if(illegal)
	{
	fprintf(stdout,\
	"\n\n*PROGRAM EXECUTION ABORTED*\n\n");
	fprintf(stdout,\
	"You have not registered, using new beta not possible.\n");
	fprintf(stdout,\
	"Read REGISTRATION for info on how to register,\n");
	fprintf(stdout,\
	"or email newsflex@panteltje.demon.nl for more info.\n");
	fprintf(stdout,\
	"Illegal software use is a crime.\n"); 
	fprintf(stdout,\
	"International legal rules and the ones for your country apply.\n");

	exit(1);
	}/* end if illegal (they tried) */

	
i = 0;
ptr = load_setting(".NewsFleXff");
if(! ptr) i = 1;
if(ptr) if(strcmp(ptr, "f") != 0) i = 1;
free(ptr);
if(i)	
	{
	save_setting(".NewsFleXff", "f");
	show_newsserver_form();
	fl_show_object(fdui -> desc_accept_button);
	show_space_flag = 1;
	show_info_form();
	}

/*delete_all_message_ids()*/
load_message_ids();

/*
test if the period has changed, and ask the user if he / she wants to
move the current database for this news server to one named with the
name of the previous period.
*/
old_period = (char *) detect_new_period_and_return_old_period(&new);
/* note old period cannot be NULL */
if(new)
	{
	if(debug_flag)
		{
		fprintf(stdout, "old_period=%s\n", old_period);
		}

	create_new_newsserver_databases(old_period);/* asks for confirmation
													for each database */

	/* move current postings to postings previous_month-year */
	sprintf(temp,\
	"Move database\n'postings current' to 'postings %s'?", old_period);
	if(fl_show_question(temp, 0) )/* 0 = mouse on NO */
		{
		if(debug_flag)
			{
			fprintf(stdout, "moving current postings to %s\n", old_period);		
			}
		move_current_postings_database_and_create_new_current(old_period);
		}/* end if user says yes */

	}/* end if new period */	

make_space();

check_mail_local();

fl_do_forms();

return(0);
}/* end function main */

