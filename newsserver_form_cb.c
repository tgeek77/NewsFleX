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

int newsserver_form_modified_flag;
int fontsizes_modified_flag;
int new_newsserver_flag;

void newsserver_form_input_field_cb(FL_OBJECT *ob, long data)
{
newsserver_form_modified_flag = 1;
}/* end function newsserver_form_input_field_cb */


void newsserver_form_browser_cb(FL_OBJECT *ob, long data)
{
int selected_line;

if(debug_flag)
	{
	fprintf(stdout, "newsserver_form_browser_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

/* maintain visible indication that line is selected with single click  */
fl_select_browser_line(fdui -> newsserver_form_browser, abs(selected_line) );

/*newsserver_form_modified_flag = 1;*/
}/* end function  newsserver_form_browser_cb */


void newsserver_form_browser_double_cb(FL_OBJECT *ob, long data)
{
int selected_line;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "newsserver_form_browser_double_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

ptr = line_to_newsserver(abs(selected_line) );
if(!ptr) return;

select_newsserver(ptr);

if(debug_flag)
	{
	fprintf(stdout,\
	"newsserver_form_browser_double_cb():\n\
	news_server_name=%s database_name=%s\n",\
	news_server_name, database_name);
	}

show_newsservers(SHOW_SAME_POSITION);

fl_select_browser_line(fdui -> newsserver_form_browser, abs(selected_line) );
newsserver_form_modified_flag = 1;
}/* end function  newsserver_form_browser_double_cb */


void newsserver_form_delete_button_cb(FL_OBJECT *ob, long data)
{
int total_lines;
int s_line;
char *name;

total_lines = fl_get_browser_maxline(fdui -> newsserver_form_browser);
for(s_line = 1; s_line <= total_lines; s_line++)
	{
	if(fl_isselected_browser_line(fdui -> newsserver_form_browser, s_line) )
		{
		newsserver_form_modified_flag = 1;
		name = (char *) line_to_newsserver(s_line);
		if(name) delete_a_newsserver(name);
		}/* end selected line */
	}/* end for all lines */
show_newsservers(SHOW_SAME_POSITION);
newsserver_form_modified_flag = 1;
}/* end function newsserver_form_delete_button_cb */


void newsserver_form_add_button_cb(FL_OBJECT *ob, long data)
{
int s_line;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "newsserver_form_add_button_cb(): arg none\n");
	}

ptr = (char *) fl_get_input(fdui -> newsserver_form_input_field);
if(! ptr) return;

if(! add_newsserver(ptr) ) return;

/* clear the input field */
/*fl_set_input(fdui -> newsserver_form_input_field, "");*/

show_newsservers(SHOW_BOTTOM);

/* mark new entry in browser */
newsserver_to_line(ptr, &s_line);
fl_select_browser_line(fdui -> newsserver_form_browser, s_line);

new_newsserver_flag = 1;
newsserver_form_modified_flag = 1;
}/* end function newsserver_form_add_button_cb */


void newsserver_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(newsserver_form_fontsize < 5) newsserver_form_fontsize++;
size = integer_to_fontsize(newsserver_form_fontsize);

fl_set_browser_fontsize(fdui -> newsserver_form_browser, size);
/*fl_set_object_lsize(fdui -> newsserver_form_input_field, size);*/

fontsizes_modified_flag = 1;
}/* end function newsserver_form_fontsize_up_button_cb */


void newsserver_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(newsserver_form_fontsize > 0) newsserver_form_fontsize--;
size = integer_to_fontsize(newsserver_form_fontsize);

fl_set_browser_fontsize(fdui -> newsserver_form_browser, size);
/*fl_set_object_lsize(fdui -> newsserver_form_input_field, size);*/

fontsizes_modified_flag = 1;
}/* end function newsserver_form_fontsize_down_button_cb */


void newsserver_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
if(newsserver_form_modified_flag)
	{
	/* restore from disc */
/*	load_newsservers();*/
	fl_set_input(fdui -> newsserver_form_input_field, "");
	newsserver_form_modified_flag = 0;
 	}

fl_hide_form(fd_NewsFleX -> newsserver_form);

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}
}/* end function newsserver_form_cancel_button_cb */


void newsserver_form_accept_button_cb(FL_OBJECT *ob, long data)
{
char temp[TEMP_SIZE];
char *ptr;

if(newsserver_form_modified_flag)
	{
	refresh_screen(selected_group);
	newsserver_form_modified_flag = 0;
	}

fl_hide_form(fd_NewsFleX -> newsserver_form);

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

if(new_newsserver_flag)
	{
	ptr = (char *) fl_get_input(fdui -> newsserver_form_input_field);
	if(! ptr) return;

/*
	free(news_server_name);
	news_server_name = strsave(ptr);

	free(database_name);
	database_name = strsave("current");
*/

	/* news_server_name and database_name set by load_newsservers */
	sprintf(temp, "%s current", ptr);
	select_newsserver(temp);

	if(debug_flag)
		{
		fprintf(stdout,\
		"newsserver_form_accept_button_cb():\n\
		news_server_name=%s database_name=%s\n",\
		news_server_name, database_name);
		}

	/* this will fill in the fields in the setup form */
	load_newsservers();
	
	/* show setup form */
	show_setup_form();
	new_newsserver_flag = 0;
	}
}/* end function newsserver_form_accept_button_cb */


