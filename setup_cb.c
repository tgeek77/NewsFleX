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

int fontsizes_modified_flag;


void setup_form_check_local_mail_button_cb(FL_OBJECT *ob, long data)
{
check_incoming_mail_periodically_flag = fl_get_button(ob);
if( check_incoming_mail_periodically_flag)
	{
	fl_set_object_label(ob, "YES");
	}		
else
	{
	fl_set_object_label(ob, "NO");
	}

/*save_general_settings(); see accept and cancel */
}/* end function setup_form_check_local_mail_button_cb */


void setup_form_local_mail_check_interval_input_field_cb(\
FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"setup_form_local_mail_check_interval_input_field arg none\n");
	}
}/* end function setup_form_local_mail_check_interval_input_field */


void setup_form_posting_wrap_chars_input_field_cb(FL_OBJECT *ob, long data)
{
global_posting_wrap_chars = atoi(fl_get_input(ob) );

/*save_general_settings(); see accept and cancel */

}/* end function setup_form_posting_wrap_chars_input_field_cb */


void setup_form_lines_first_button_cb(FL_OBJECT *ob, long data)
{
lines_first_flag = fl_get_button(ob);
if(lines_first_flag)
	{
	fl_set_object_label(ob, "YES");
	}		
else
	{
	fl_set_object_label(ob, "NO");
	}

/*save_general_settings(); see accept and cancel */

}/* end function setup_form_lines_first_button_cb */


void setup_form_cut_off_sig_in_reply_button_cb(FL_OBJECT *ob, long data)
{
global_cut_of_sig_in_reply_flag = fl_get_button(ob);
if(global_cut_of_sig_in_reply_flag)
	{
	fl_set_object_label(ob, "YES");
	}
else
	{
	fl_set_object_label(ob, "NO");
	}

}/* end function setup_form_cut_off_sig_in_reply_button_cb */


void setup_form_beep_on_mail_button_cb(FL_OBJECT *ob, long data)
{
global_beep_on_mail_flag = fl_get_button(ob);
if(global_beep_on_mail_flag)
	{
	fl_set_object_label(ob, "YES");
	}
else
	{
	fl_set_object_label(ob, "NO");
	}

}/* end function setup_form_beep_on_mail_button_cb */


void get_new_headers_enable_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "get_new_headers_enable_button_cb(): arg none\n");
	} 

if(expired_flag)
	{
	auto_get_new_headers_flag = 0;
	fl_set_button(ob, auto_get_new_headers_flag);
	fl_set_object_label(fdui -> get_new_headers_enable_button, "NO");

	show_space();
	}/* end if expired_flag */

auto_get_new_headers_flag = fl_get_button(ob);
if(auto_get_new_headers_flag)
	{
	fl_set_object_label(fdui -> get_new_headers_enable_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> get_new_headers_enable_button, "NO");
	}
}/* end function get_new_headers_enable_button_cb */


void get_new_headers_time_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "get_new_headers_time_input_field_cb(): arg none\n");
	}
}/* end function get_new_headers_time_input_field_cb */


void pid_test_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "pid_test_input_field_cb(): arg none\n");
	}

/* test if pid file exists  not very usefull here */
}/* end function pid_test_input_field_cb */


void server_username_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "server_username_input_field_cb(): arg none\n");
	}

}/* end function server_username_input_field_cb */


void server_password_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "server_password_input_field_cb(): arg none\n");
	}

}/* end function server_password_input_field_cb */


void desc_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "desc_button_cb(): arg none\n");
	}

enable_description_popup_flag = fl_get_button(ob);
if(enable_description_popup_flag)
	{
	fl_set_object_label(fdui -> desc_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> desc_button, "NO");
	}
}/* end function desc_button_cb */


void width_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "width_button_cb(): arg none\n");
	}

variable_width_flag = fl_get_button(ob);
if(variable_width_flag)
	{
	fl_set_object_label(fdui -> width_button, "VARIABLE");
	}
else
	{
	fl_set_object_label(fdui -> width_button, "FIXED");
	}
}/* end function width_button_cb */


void setup_form_insert_headers_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "setup_form_insert_headers_button_cb(): arg none\n");
	}
insert_headers_offline_flag = fl_get_button(ob);
if(insert_headers_offline_flag)
	{
	fl_set_object_label(fdui -> setup_form_insert_headers_button,\
	"OFFLINE");
	}
else
	{
	fl_set_object_label(fdui -> setup_form_insert_headers_button,\
	"ONLINE");
	}
}/* end function setup_form_insert_headers_button_cb */


void setup_form_get_by_message_id_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "setup_form_get_by_message_id_button_cb(): arg none\n");
	}
request_by_message_id_flag = fl_get_button(ob);
if(request_by_message_id_flag)
	{
	fl_show_alert("This function may not work on your server", "", "", 0);
	fl_set_object_label(fdui -> setup_form_get_by_message_id_button,\
	"MSG  ID");
	}
else
	{
	fl_set_object_label(fdui -> setup_form_get_by_message_id_button,\
	"NUMBER");
	}
}/* end function setup_form_get_by_message_id_button_cb */


void setup_form_show_only_once_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "setup_form_show_only_once_button_cb(): arg none\n");
	}
show_article_once_flag = fl_get_button(ob);
if(show_article_once_flag)
	{
	fl_set_object_label(fdui -> setup_form_show_only_once_button,\
	"ONCE");
	}
else
	{
	fl_set_object_label(fdui -> setup_form_show_only_once_button,\
	"ALWAYS");
	}
}/* end function setup_form_show_only_once_button_cb */


void setup_form_do_not_use_sendmail_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "setup_form_do_not_use_sendmail_button_cb(): arg none\n");
	}
do_not_use_sendmail_flag = fl_get_button(ob);
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
}/* end function setup_form_do_not_use_sendmail_button_cb */


void auto_connect_to_net_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "auto_connect_net_button_cb(): arg none\n");
	} 
auto_connect_to_net_flag = fl_get_button(ob);
if(auto_connect_to_net_flag)
	{
	fl_set_object_label(fdui -> auto_connect_to_net_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_connect_to_net_button, "NO");
	}
}/* end function auto_connect_to_net_button_cb */


void auto_connect_to_net_input_field_cb(FL_OBJECT *ob, long data)
{
FILE *autoconnect_filefd;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "auto_connect_to_net_input_field_cb(): arg none\n");
	}
/* test if auto connect file exists */
if(fl_get_button(fdui -> auto_connect_to_net_button) )
	{
	ptr = (char *) fl_get_input(fdui -> auto_connect_to_net_input_field);
	autoconnect_filefd = fopen(ptr, "r");
	if(! autoconnect_filefd)
		{
		fl_show_alert(\
		"Cannot open connect to net file", ptr, "file not found", 0); 
		}
	else fclose(autoconnect_filefd);
	}
}/* end function auto_connect_to_net_input_field_cb */


void auto_disconnect_from_net_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "auto_disconnect_button_cb(): arg none\n");
	}
auto_disconnect_from_net_flag = fl_get_button(ob);
if(auto_disconnect_from_net_flag)
	{
	fl_set_object_label(fdui -> auto_disconnect_from_net_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_disconnect_from_net_button, "NO");
	}
}/* end function auto_disconnect_from_net_button_cb */


void auto_disconnect_from_net_input_field_cb(FL_OBJECT *ob, long data)
{
FILE *autodisconnect_filefd;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"auto_disconnect_from_net_input_field_cb(): arg none\n");
	}
/* test if auto disconnect file exists */
if(fl_get_button(fdui -> auto_disconnect_from_net_button) )
	{
	ptr = (char *) fl_get_input(fdui -> auto_disconnect_from_net_input_field);
	autodisconnect_filefd = fopen(ptr, "r");
	if(! autodisconnect_filefd)
		{
		fl_show_alert("Cannot open disconnect from net file", ptr,\
		"file not found", 0); 
		}
	else fclose(autodisconnect_filefd);
	}
}/* end function auto_disconnect_from_net_input_field_cb */


void auto_execute_application_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "auto_execute_application_button_cb(): arg none\n");
	}
auto_execute_program_flag = fl_get_button(ob);
if(auto_execute_program_flag)
	{
	fl_set_object_label(fdui -> auto_execute_application_button, "YES");
	}		
else
	{
	fl_set_object_label(fdui -> auto_execute_application_button, "NO");
	}
}/* end function auto_execute_application_button_cb */


void auto_execute_application_input_field_cb(FL_OBJECT *ob, long data)
{
FILE *auto_execute_filefd;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "auto_execute_application_input_field_cb(): arg none\n");
	}
if(fl_get_button(fdui -> auto_execute_application_button) )
	{
	/* test if auto disconnect file exists */
	ptr =\
	(char *) fl_get_input(fdui -> auto_execute_application_input_field);
	auto_execute_filefd = fopen(ptr, "r");
	if(! auto_execute_filefd)
		{
		fl_show_alert("Cannot find application", ptr,\
		"file not found", 0); 
		}
	else fclose(auto_execute_filefd);
	}
}/* end function auto_execute_application_input_field_cb */


void news_server_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "news_server_input_field_cb(): arg none\n");
	}
fl_show_alert(\
"This field cannot be changed",\
"Select NEWSSERVER LIST from the WINDOW\npuldown menu",\
"to change news servers", 0);

/* this field cannot be changed by user */
fl_set_input(fdui -> news_server_input_field, news_server_name);
}/* end function news_server_input_field_cb */


void news_server_port_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "news_server_port_input_field_cb(): arg none\n");
	}
}/* end function news_server_port_input_field_cb */


void mail_server_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "mail_server_input_field_cb(): arg none\n");
	}
}/* end function mail_server_input_field_cb */


void mail_server_port_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "mail_server_port_input_field_cb(): arg none\n");
	}
}/* end function mail_server_port_input_field_cb */


void real_name_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "real_name_input_field_cb(): arg none\n");
	}
}/* end function real_name_input_field_cb */


void user_email_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "user_email_input_field_cb(): arg none\n");
	}
}/* end function user_email_input_field_cb */


void maximum_headers_per_group_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"maximum_headers_per_group_input_field_cb(): arg none\n");
	}
}/* end function maximum_headers_per_group_input_field_cb */


void connect_to_news_server_timeout_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"connect_to_news_server_timeout_input_filed_cb(): arg none\n");
	}
}/* end function connect_to_news_server_timeout_input_field_cb */


void connect_to_mail_server_timeout_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"connect_to_mail_server_timeout_input_filed_cb(): arg none\n");
	}
}/* end function connect_to_mail_server_timeout_input_field_cb */


void tab_size_input_field_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "tab_size_input_field_cb(): arg none\n");
	}
}/* end function tab_size_input_field_cb */


void setup_form_posting_source_button_cb(\
FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"setup_form_posting_source_button_cb(): arg none\n");
	}
global_show_posting_source_flag = fl_get_button(ob);
if( global_show_posting_source_flag)
	{
	fl_set_object_label(ob, "YES");
	}		
else
	{
	fl_set_object_label(ob, "NO");
	}

/*save_general_settings(); see accept and cancel */
}/* end function setup_form_posting_source_button_cb */


void setup_form_posting_source_input_field_cb(\
FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"setup_form_posting_source_input_field_cb(): arg none\n");
	}

/*global_posting_source_text */
}/* end function setup_form_posting_source_input_field_cb */


void setup_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(setup_form_fontsize < 5) setup_form_fontsize++;
size = integer_to_fontsize(setup_form_fontsize);

fontsizes_modified_flag = 1;
}/* end function setup_form_fontsize_up_button_cb */


void setup_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(setup_form_fontsize > 0) setup_form_fontsize--;
size = integer_to_fontsize(setup_form_fontsize);

fontsizes_modified_flag = 1;
}/* end function setup_form_fontsize_down_button_cb */


void setup_accept_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
FILE *autoconnect_filefd;
FILE *autodisconnect_filefd;
FILE *autoexecute_filefd;
char temp[TEMP_SIZE];

/* test if auto connect file exists */
if( fl_get_button(fdui -> auto_connect_to_net_button) )
	{
	ptr = (char *) fl_get_input(fdui -> auto_connect_to_net_input_field);
	autoconnect_filefd = fopen(ptr, "r");
	if(! autoconnect_filefd)
		{
		fl_show_alert(\
		"Cannot open connect to net file", ptr, "file not found", 0); 
		return;/* abort */
		}
	else fclose(autoconnect_filefd);
	}

/* test if auto disconnect file exists */
if(fl_get_button(fdui -> auto_disconnect_from_net_button) )
	{
	ptr =\
	(char *) fl_get_input(fdui -> auto_disconnect_from_net_input_field);
	autodisconnect_filefd = fopen(ptr, "r");
	if(! autodisconnect_filefd)
		{
		fl_show_alert("Cannot open disconnect from net file", ptr,\
		"file not found", 0); 
		return;/* abort */
		}
	else fclose(autodisconnect_filefd);
	}

/* test if auto execute file exists */
if(fl_get_button(fdui -> auto_execute_application_button) )
	{
	ptr =\
	(char *) fl_get_input(fdui -> auto_execute_application_input_field);
	autoexecute_filefd = fopen(ptr, "r");
	if(! autoexecute_filefd)
		{
		fl_show_alert("Cannot find application", ptr,\
		"file not found", 0); 
		return;/* abort */
		}
	else fclose(autoexecute_filefd);
	}

sprintf(temp, "%s %s", news_server_name, database_name);
if(! save_newsserver_setting(temp) ) return;

fl_hide_form(fd_NewsFleX -> setup);

/* for tabs */
refresh_screen(selected_group);

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

save_general_settings();/* global setting */
}/* end function setup_accept_button_cb */


void setup_cancel_button_cb(FL_OBJECT *ob, long data)
{
char temp[TEMP_SIZE];

sprintf(temp, "%s %s", news_server_name, database_name);

/* restore settings */
load_newsserver_setting(temp);

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

load_general_settings();/* global settings */

fl_hide_form(fd_NewsFleX -> setup);
}/* end function setup_cancel_button_cb */


