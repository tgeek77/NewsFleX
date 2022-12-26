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

int highlight_form_modified_flag;
int fontsizes_modified_flag;


void highlight_postings_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_postings_flag = fl_get_button(ob);
}


void highlight_articles_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_articles_flag = fl_get_button(ob);
}


void highlight_headers_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_flag = fl_get_button(ob);
}


void highlight_urls_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_urls_flag = fl_get_button(ob);
}


void highlight_headers_from_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_from_flag = fl_get_button(ob);
}


void highlight_headers_to_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_to_flag = fl_get_button(ob);
}


void highlight_headers_subject_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_subject_flag = fl_get_button(ob);
}


void highlight_headers_references_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_references_flag = fl_get_button(ob);
}


void highlight_headers_others_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_headers_others_flag = fl_get_button(ob);
}


void highlight_urls_url_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_urls_url_flag = fl_get_button(ob);
}


void highlight_urls_description_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_urls_description_flag = fl_get_button(ob);
}


void highlight_case_sensitive_button_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
highlight_case_sensitive_flag = fl_get_button(ob);
}


void highlight_keywords_input_field_cb(FL_OBJECT *ob, long data)
{
highlight_form_modified_flag = 1;
}


void highlight_cancel_button_cb(FL_OBJECT *ob, long data)
{
if(highlight_form_modified_flag)
	{
	load_highlight_settings();
	highlight_form_modified_flag = 0;
	}

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

fl_hide_form(fd_NewsFleX -> highlight);
}/* end function highlight_cancel_button_cb */


void highlight_accept_button_cb(FL_OBJECT *ob, long data)
{
if(highlight_form_modified_flag)
	{
	save_highlight_settings();
	load_highlight_settings();
	highlight_form_modified_flag = 0;
	}

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

if(! highlight_keywords)
	{
	fl_show_alert("No highlight keywords specified", "", "", 0);
	return;
	}
else /* a NULL pointer will crash */
	{
	if(strlen(highlight_keywords) == 0)
		{
		fl_show_alert("No highlight keywords specified", "", "", 0);
		return;
		}
	}
	
if(!highlight_postings_flag && !highlight_articles_flag)
	{
	fl_show_alert("No postings or articles button pressed",\
	"nothing to highlight for", "", 0);
	return;
	}

if(highlight_postings_flag || highlight_articles_flag)
	{
	if(!highlight_headers_flag)
		{
		fl_show_alert("Do not know what to highlight",\
		"Select one or more of these:",\
		"headers bodies attachments", 0);
		return;
		}
	}

if(highlight_headers_flag)
	{
	if(!highlight_headers_from_flag && !highlight_headers_to_flag &&\
	!highlight_headers_subject_flag && !highlight_headers_references_flag &&\
	!highlight_headers_others_flag)
		{
		fl_show_alert("Do not know what header lines to highlight", \
		"Select one or more header types", "", 0);
		return;
		}
	}

fl_hide_form(fd_NewsFleX -> highlight);

refresh_screen(selected_group);
}/* end function highlight_accept_button_cb */

