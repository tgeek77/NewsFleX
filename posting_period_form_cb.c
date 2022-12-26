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

int posting_period_form_modified_flag;
int fontsizes_modified_flag;

void posting_period_form_input_field_cb(FL_OBJECT *ob, long data)
{
posting_period_form_modified_flag = 1;
}/* end function posting_period_form_input_field_cb */


void posting_period_form_browser_cb(FL_OBJECT *ob, long data)
{
int selected_line;

if(debug_flag)
	{
	fprintf(stdout, "posting_period_form_browser_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

/* maintain visible indication that line is selected with single click  */
fl_select_browser_line(fdui -> posting_period_form_browser, abs(selected_line) );

/*posting_period_form_modified_flag = 1;*/
}/* end function  posting_period_form_browser_cb */


void posting_period_form_browser_double_cb(FL_OBJECT *ob, long data)
{
int selected_line;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "posting_period_form_browser_double_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

ptr = line_to_posting_period(abs(selected_line) );
if(!ptr) return;

free(postings_database_name);
postings_database_name = strsave(ptr);

select_posting_period(ptr);

if(debug_flag)
	{
	fprintf(stdout,\
	"posting_period_form_browser_double_cb():\n\
	postings_database_name=%s\n",\
	postings_database_name);
	}

show_posting_periods(SHOW_SAME_POSITION);

fl_select_browser_line(fdui -> posting_period_form_browser,\
abs(selected_line) );
posting_period_form_modified_flag = 1;
}/* end function  posting_period_form_browser_double_cb */


void posting_period_form_delete_button_cb(FL_OBJECT *ob, long data)
{
int total_lines;
int s_line;
char *name;

total_lines = fl_get_browser_maxline(fdui -> posting_period_form_browser);

for(s_line = 1; s_line <= total_lines; s_line++)
	{
	if(fl_isselected_browser_line(fdui -> posting_period_form_browser, s_line) )
		{
		posting_period_form_modified_flag = 1;
		name = (char *) line_to_posting_period(s_line);
		if(name) delete_a_posting_period(name);
		}/* end selected line */
	}/* end for all lines */

show_posting_periods(SHOW_SAME_POSITION);

posting_period_form_modified_flag = 1;
}/* end function posting_period_form_delete_button_cb */


void posting_period_form_add_button_cb(FL_OBJECT *ob, long data)
{
int s_line;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "posting_period_form_add_button_cb(): arg none\n");
	}

ptr = (char *) fl_get_input(fdui -> posting_period_form_input_field);
if(! ptr) return;

if(! add_posting_period(ptr) ) return;

/* clear the input field */
/*fl_set_input(fdui -> posting_period_form_input_field, "");*/

show_posting_periods(SHOW_BOTTOM);

/* mark new entry in browser */
posting_period_to_line(ptr, &s_line);
fl_select_browser_line(fdui -> posting_period_form_browser, s_line);

posting_period_form_modified_flag = 1;
}/* end function posting_period_form_add_button_cb */


void posting_period_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(posting_period_form_fontsize < 5) posting_period_form_fontsize++;
size = integer_to_fontsize(posting_period_form_fontsize);

fl_set_browser_fontsize(fdui -> posting_period_form_browser, size);
/*fl_set_object_lsize(fdui -> posting_period_form_input_field, size);*/

fontsizes_modified_flag = 1;
}/* end function posting_period_form_fontsize_up_button_cb */


void posting_period_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(posting_period_form_fontsize > 0) posting_period_form_fontsize--;
size = integer_to_fontsize(posting_period_form_fontsize);

fl_set_browser_fontsize(fdui -> posting_period_form_browser, size);
/*fl_set_object_lsize(fdui -> posting_period_form_input_field, size);*/

fontsizes_modified_flag = 1;
}/* end function posting_period_form_fontsize_down_button_cb */


void posting_period_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
if(posting_period_form_modified_flag)
	{
	/* restore from disc */
/*	load_posting_periods();*/
	fl_set_input(fdui -> posting_period_form_input_field, "");
	posting_period_form_modified_flag = 0;
 	}

fl_hide_form(fd_NewsFleX -> posting_period_form);

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}
}/* end function posting_period_form_cancel_button_cb */


void posting_period_form_accept_button_cb(FL_OBJECT *ob, long data)
{
if(posting_period_form_modified_flag)
	{
	refresh_screen(selected_group);
	posting_period_form_modified_flag = 0;
	}

fl_hide_form(fd_NewsFleX -> posting_period_form);

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function posting_period_form_accept_button_cb */


