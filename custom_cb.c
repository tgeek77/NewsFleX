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


void global_custom_headers_enable_button_cb(FL_OBJECT *ob, long data)
{
if(fl_get_button(fdui ->  global_custom_headers_enable_button) )
	{
	custom_headers_state |= GLOBAL_HEADERS_ENABLED;
	}
else
	{
	custom_headers_state &= 255 - GLOBAL_HEADERS_ENABLED;
	}
headers_modified_flag = 1;
}/* end function global_custom_headers_enable_button_cb */


void global_custom_headers_editor_input_field_cb(FL_OBJECT *ob, long data)
{
custom_headers_state |= GLOBAL_HEADERS_MODIFIED;
headers_modified_flag = 1;
}/* end function global_custom_headers_input_field_cb */


void local_custom_headers_enable_button_cb(FL_OBJECT *ob, long data)
{
if(fl_get_button(fdui ->  local_custom_headers_enable_button) )
	{
	custom_headers_state |= LOCAL_HEADERS_ENABLED;
	}
else
	{
	custom_headers_state &= 255 - LOCAL_HEADERS_ENABLED;
	}
 
headers_modified_flag = 1;
}/* end function local_custom_headers_enable_button_cb */


void local_custom_headers_editor_input_field_cb(FL_OBJECT *ob, long data)
{
custom_headers_state |= LOCAL_HEADERS_MODIFIED;
headers_modified_flag = 1;
}/* end function local_custom_headers_input_field_cb */


void custom_headers_editor_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(custom_headers_editor_fontsize < 5) custom_headers_editor_fontsize++;
size = integer_to_fontsize(custom_headers_editor_fontsize);

fl_set_object_lsize(fdui -> global_custom_headers_editor_input_field, size);
fl_set_object_lsize(fdui -> local_custom_headers_editor_input_field, size);
save_fontsizes();
}/* end function custom_headers_editor_fontsize_up_button_cb */


void custom_headers_editor_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(custom_headers_editor_fontsize > 0) custom_headers_editor_fontsize--;
size = integer_to_fontsize(custom_headers_editor_fontsize);

fl_set_object_lsize(fdui -> global_custom_headers_editor_input_field, size);
fl_set_object_lsize(fdui -> local_custom_headers_editor_input_field, size);
save_fontsizes();
}/* end function custom_headers_fontsize_down_button_cb */


void custom_headers_editor_accept_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
char pathfilename[TEMP_SIZE];
FILE *custom_headers_filefd;

if(! headers_modified_flag) 
	{
	fl_hide_form(fd_NewsFleX -> custom_headers_editor);
	return;
	}
	
if(custom_headers_state & GLOBAL_HEADERS_MODIFIED)
	{
	ptr =\
	(char *) fl_get_input(fdui -> global_custom_headers_editor_input_field);
	if(! ptr) return;
	/* save the custom headers_file */
	sprintf(pathfilename, "%s/.NewsFleX/global_custom_head",\
	home_dir);
	custom_headers_filefd = fopen(pathfilename, "w");
	if(! custom_headers_filefd)
		{
		fl_show_alert("Cannot open file", pathfilename, "for write", 0);
		return;
		}
	fprintf(custom_headers_filefd, "%s", ptr);
	/* if the user did not provide the terminating cr */
	if(*(ptr + strlen(ptr) - 1) != '\n')
		{
		fprintf(custom_headers_filefd, "\n");
		}
	fclose(custom_headers_filefd);
	}/* end local headers modified */

if( (custom_headers_state & GLOBAL_HEADERS_ENABLED) ||
(custom_headers_state & LOCAL_HEADERS_ENABLED) )
	{
	fl_set_object_color(fdui -> posting_custom_headers_button,\
	FL_DARKORANGE, FL_DARKORANGE);
	}
else
	{
	fl_set_object_color(\
	fdui -> posting_custom_headers_button, FL_COL1, FL_COL1);
	}

fl_hide_form(fd_NewsFleX -> custom_headers_editor);
posting_modified_flag = 1;
}/* end function custom_headers_editor_accept_button_cb */


void custom_headers_editor_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> custom_headers_editor);
}/* end function custom_headers_editor_cancel_button_cb */


