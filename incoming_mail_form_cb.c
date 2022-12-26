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


void incoming_mail_form_browser_cb(FL_OBJECT *ob, long data)
{
int a;
long mail_id;
int selected_line;

if(debug_flag)
	{
	fprintf(stdout, "incoming_mail_form_browser_cb() arg none\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

/* sometimes a is negative why? deselect */

/* 
lines in summary do not have an article_id,
if clicking on a summary, the previous article_id is used.
*/
a = abs(selected_line);
while(1)
	{
	if(line_to_mail_id(a, &mail_id) ) break;
	a--;
	if(a < 1) break;/* first browser line is 1 */
	}

if(! reply_to_incoming_mail(mail_id) ) return;
}/* end function incoming_mail_form_browser_cb */


void incoming_mail_form_browser_double_cb(FL_OBJECT *ob, long data)
{
}/* end function incoming_mail_form_browser_double_cb */


void incoming_mail_form_mark_read_button_cb(FL_OBJECT *ob, long data)
{
}/* end function incoming_mail_form_mark_read_button_cb */


void incoming_mail_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(incoming_mail_form_fontsize < 5) incoming_mail_form_fontsize++;
size = integer_to_fontsize(incoming_mail_form_fontsize);

fl_set_browser_fontsize(fdui -> incoming_mail_form_browser, size);
save_fontsizes();
}/* end function incoming_mail_form_fontsize_up_button_cb */


void incoming_mail_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(incoming_mail_form_fontsize > 0) incoming_mail_form_fontsize--;
size = integer_to_fontsize(incoming_mail_form_fontsize);

fl_set_browser_fontsize(fdui -> incoming_mail_form_browser, size);
save_fontsizes();
}/* end function incoming_mail_form_fontsize_down_button_cb */


void incoming_mail_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> incoming_mail_form);
}/* end function incoming_mail_form_cancel_button_cb */


void incoming_mail_form_accept_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> incoming_mail_form);
}/* end function incoming_mail_form_accept_button_cb */


