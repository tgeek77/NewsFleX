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


void article_header_form_input_field_cb(FL_OBJECT *ob, long data)
{
/* restore original text from cut and paste */
show_article_header(selected_group, selected_article);

/*
XXXXX
Here, if clicked on a references line, extract the message_id, add to a
list, once online get all message_id's in this list.
Unfortunately we do not know the group these message id's belong to,
so just store them in the selected group.
So group must also be in list.
*/

fl_show_alert(\
"This text cannot be modified",\
"for custom headers select a reply option from the posting pulldown menu",\
"then select custom headers", 0);

}/* end function header_editor_form_field_cb */


void article_header_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(article_header_form_fontsize < 5) article_header_form_fontsize++;
size = integer_to_fontsize(article_header_form_fontsize);

fl_set_object_lsize(fdui -> article_header_form_input_field, size);
save_fontsizes();
}/* end function article_header_form_fontsize_up_button_cb */


void article_header_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(article_header_form_fontsize > 0) article_header_form_fontsize--;
size = integer_to_fontsize(article_header_form_fontsize);

fl_set_object_lsize(fdui -> article_header_form_input_field, size);
save_fontsizes();
}/* end function article_header_form_fontsize_down_button_cb */


void article_header_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> article_header_form);
}/* end function article_header_form_cancel_button_cb */

