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


void filter_password_input_field_cb(FL_OBJECT *ob, long data)
{
/*filter_modified_flag = 1;*/
filter_enable_modified_flag = 1;
}/* end function filter_password_input_field_cb */


void filter_postings_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_articles_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_groups_input_field_cb(FL_OBJECT *ob, long data)
{
int line;

filter_modified_flag = 1;
set_filter(selected_filter);
show_filters();
filter_id_to_line(selected_filter, &line);
fl_select_browser_line(fdui -> filter_list_browser, line);
}/* end function filter_groups_input_field_cb */


void filter_headers_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_headers_from_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_enable_button_cb(FL_OBJECT *ob, long data)
{
/*filter_modified_flag = 1;*/
filter_enable_modified_flag = 1;
set_filter(selected_filter);
}


void filter_headers_to_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_headers_subject_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_headers_newsgroups_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_headers_others_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_bodies_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_groups_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_list_browser_cb(FL_OBJECT *ob, long data)
{
}


void filter_list_browser_double_cb(FL_OBJECT *ob, long data)
{
int selected_line;

if(debug_flag)
	{
	fprintf(stdout, "filter_list_browser_double_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

line_to_filter_id(abs(selected_line), &selected_filter);
get_filter(selected_filter);

show_filters();
fl_select_browser_line(fdui -> filter_list_browser, abs(selected_line) );
}/* end function  filter_list_browser_double_cb */


void filter_list_browser_delete_button_cb(FL_OBJECT *ob, long data)
{
int total_lines;
int s_line;
long filter_id;

total_lines = fl_get_browser_maxline(fdui -> filter_list_browser);
for(s_line = 1; s_line <= total_lines; s_line++)
	{
	if(fl_isselected_browser_line(fdui -> filter_list_browser, s_line) )
		{
		filter_modified_flag = 1;
		line_to_filter_id(s_line, &filter_id);
		delete_filter(filter_id);
		}/* end selected line */
	}/* end for all lines */
show_filters();
}/* end function filter_list_browser_delete_button_cb */


void filter_list_browser_new_button_cb(FL_OBJECT *ob, long data)
{
int s_line;

if(debug_flag)
	{
	fprintf(stdout, "filter_list_browser_new_button_cb(): arg none\n");
	}

get_new_filter_id(&selected_filter);

if(! add_filter(selected_filter) ) return;
set_filter(selected_filter);
show_filters();
	
/* mark new entry in browser */
filter_id_to_line(selected_filter, &s_line);
fl_select_browser_line(fdui -> filter_list_browser, s_line);
}/* end function filter_list_browser_new_button_cb */


void filter_attachments_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_case_sensitive_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}


void filter_keywords_input_field_cb(FL_OBJECT *ob, long data)
{
int line;

filter_modified_flag = 1;
set_filter(selected_filter);
show_filters();
filter_id_to_line(selected_filter, &line);
fl_select_browser_line(fdui -> filter_list_browser, line);
}/* end function filter_keywords_input_field_cb */


void filter_keywords_and_button_cb(FL_OBJECT *ob, long data)
{
/* button interlocking mechanism */
if(fl_get_button(ob) )
	{
	fl_set_button(fdui ->  filter_keywords_not_button, 0);
	fl_set_button(fdui ->  filter_keywords_only_button, 0);
	}

filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_keywords_and_button_cb */


void filter_keywords_not_button_cb(FL_OBJECT *ob, long data)
{
/* button interlocking mechanism */
if(fl_get_button(ob) )
	{
	fl_set_button(fdui ->  filter_keywords_and_button, 0);
	fl_set_button(fdui ->  filter_keywords_only_button, 0);
	}

filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_keywords_not_button_cb */


void filter_keywords_only_button_cb(FL_OBJECT *ob, long data)
{
/* button interlocking mechanism */
if(fl_get_button(ob) )
	{
	fl_set_button(fdui ->  filter_keywords_not_button, 0);
	fl_set_button(fdui ->  filter_keywords_and_button, 0);
	}

filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_keywords_only_button_cb */


void filter_allow_button_cb(FL_OBJECT *ob, long data)
{
/* button interlocking mechanism */
if(fl_get_button(ob) )
	{
	fl_set_object_label(ob, "ALLOW");
	fl_redraw_object(ob);
	}
else
	{
	fl_set_object_label(ob, "DENY");
	fl_redraw_object(ob);
	}
filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_keywords_button_cb */


void filter_groups_show_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_groups_show_button_cb */


void filter_headers_show_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_headers_show_button_cb */


void filter_bodies_show_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_bodies_show_button_cb */


void filter_attachments_show_button_cb(FL_OBJECT *ob, long data)
{
filter_modified_flag = 1;
set_filter(selected_filter);
}/* end function filter_attachments_show_button_cb */


void filter_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(filter_form_fontsize < 5) filter_form_fontsize++;
size = integer_to_fontsize(filter_form_fontsize);

fl_set_browser_fontsize(fdui -> filter_list_browser, size);

fontsizes_modified_flag = 1;
}/* end function filter_form_fontsize_up_button_cb */


void filter_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(filter_form_fontsize > 0) filter_form_fontsize--;
size = integer_to_fontsize(filter_form_fontsize);

fl_set_browser_fontsize(fdui -> filter_list_browser, size);

fontsizes_modified_flag = 1;
}/* end function filter_form_fontsize_down_button_cb */


void filter_cancel_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;

load_filters();

fl_hide_form(fd_NewsFleX -> filter);

/* clear the password */
fl_set_input(fdui -> filter_password_input_field, "");

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

ptr = (char *) load_setting(".fe");
if(ptr)
	{
	filters_enabled_flag = atoi(ptr); 
	free(ptr);
	}
else filters_enabled_flag = 1;
}/* end function filter_cancel_button_cb */


void filter_accept_button_cb(FL_OBJECT *ob, long data)
{
int newdata;
char temp[80];

fl_hide_form(fd_NewsFleX -> filter);

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

get_new_data_flag(&newdata);

if(!filter_modified_flag && !filter_enable_modified_flag) return;

filters_enabled_flag = fl_get_button(fdui -> filter_enable_button);
sprintf(temp, "%d", filters_enabled_flag);
save_setting(".fe", temp);

refresh_screen(selected_group);

/* only for password */
save_general_settings();
load_general_settings();

if(! newdata) if(! filter_modified_flag) return;
	
save_filters();

fl_set_object_label(fdui -> command_status_display,\
"FILTERS: UPDATING DATABASE, THIS MAY TAKE A WHILE");
XSync(fl_get_display(), 0);

set_all_filter_flags_in_articles();
set_all_filter_flags_in_postings();
set_all_filter_flags_in_groups();

set_new_data_flag(0);

refresh_screen(selected_group);

fl_set_object_label(fdui -> command_status_display,\
"READY, DATABASE UPDATED");

}/* end function filter_accept_button_cb */


