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

int helpers_form_modified_flag;
int fontsizes_modified_flag;

void helpers_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(helpers_form_fontsize < 5) helpers_form_fontsize++;
size = integer_to_fontsize(helpers_form_fontsize);

fl_set_object_lsize(fdui -> helpers_form_input, size);
 fontsizes_modified_flag = 1;
}/* end function helpers_form_fontsize_up_button_cb */


void helpers_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(helpers_form_fontsize > 0) helpers_form_fontsize--;
size = integer_to_fontsize(helpers_form_fontsize);

fl_set_object_lsize(fdui -> helpers_form_input, size);
 fontsizes_modified_flag = 1;
}/* end function helpers_form_fontsize_down_button_cb */


void helpers_form_input_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "helpers_form_input_cb(): arg none\n");
	}
helpers_form_modified_flag = 1;
}/* end function helpers_form_input_cb */


void helpers_form_accept_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> helpers_form);

if(helpers_form_modified_flag)
	{
	/* save new one to disk */
	if(! save_helpers() )
		{
		fl_show_alert(\
		"Could not save helpers file", "helpers file not modified",\
		"", 0);
		return;/* abort */
		}
	helpers_form_modified_flag = 0;

	/* read back from disk into structure */	
	if(! load_helpers() )
		{
		fl_show_alert(\
		"helpers file was modified but:", "could not load it",\
		"please check for syntax errors", 0);
		return;/* abort */
		}

	helpers_form_modified_flag = 0;
	}/* end if helpers_form_modified_flag */

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

/* for tabs */
refresh_screen(selected_group);

}/* end function helpers_form_accept_button_cb */


void helpers_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> helpers_form);

/* restore */
if(helpers_form_modified_flag)
	{
	load_helpers();
	helpers_form_modified_flag = 0;
	}

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function helpers_form_cancel_button_cb */

