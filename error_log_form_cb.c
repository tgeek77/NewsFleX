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

int erase_error_log_flag;
int fontsizes_modified_flag;


void error_log_form_input_cb(FL_OBJECT *ob, long data)
{
}/* end function error_log_form_field_cb */


void error_log_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

fontsizes_modified_flag = 1;

if(error_log_form_fontsize < 5) error_log_form_fontsize++;
size = integer_to_fontsize(error_log_form_fontsize);

fl_set_object_lsize(fdui -> error_log_form_input, size);
}/* end function error_log_form_fontsize_up_button_cb */


void error_log_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

fontsizes_modified_flag = 1;

if(error_log_form_fontsize > 0) error_log_form_fontsize--;
size = integer_to_fontsize(error_log_form_fontsize);

fl_set_object_lsize(fdui -> error_log_form_input, size);
}/* end function error_log_form_fontsize_down_button_cb */


void error_log_form_clear_button_cb(FL_OBJECT *ob, long data)
{
erase_error_log_flag = 1;
fl_set_input(fdui -> error_log_form_input, "");

}/* end function error_log_form_clear_button_cb */


void error_log_form_accept_button_cb(FL_OBJECT *ob, long data)
{
if(erase_error_log_flag) erase_error_log();
erase_error_log_flag = 0;

fl_hide_form(fd_NewsFleX -> error_log_form);

/* clear the message about errors, and the flag */
if(error_log_present_flag)
	{
	error_log_present_flag = 0;
	my_command_status_display("");
	}

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function error_log_form_accept_button_cb */


void error_log_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> error_log_form);

/* clear the message about errors, and the flag */
if(error_log_present_flag)
	{
	error_log_present_flag = 0;
	my_command_status_display("");
	}

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function error_log_form_cancel_button_cb */


