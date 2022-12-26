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

/*
Maybe you wonder why so complicated?
Why are routines scattered around the source files?
Why all these globals?
Why this stuff in idle_cb?
The answere my friend is XFORMS
xforms does not *reliably* show data, forms, or anything for that
matter if called from withing a callback.
This is logical according to the xforms people.
But it forces you to leave a routine every time you want to interact
with the user!

I solved the impossible (almost) by using global flags and having
idle callback call the routines that interact with the user (like
ask_question, which pops up a form).
Also I wrote code like search_next_article(), that returns every time
a match is found, but remembers where it was, using globals.
That way, if a match is found, the user can for instance scroll in the
displayed article,
Doing it from within a loop freezes any xforms interaction.
However this way the program gets difficult to maintain, as all the 
functions in idle callback become critical, you cannot for instance
change their sequence.

The other rule (for simplicity) is that any routine using a structure
reference to postings, groups or articles, is is postings.c, groups.c or
articles.c etc..
*/ 
 
#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

int search_form_modified_flag;

void search_postings_button_cb(FL_OBJECT *ob, long data)
{
search_postings_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_articles_button_cb(FL_OBJECT *ob, long data)
{
search_articles_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_groups_input_field_cb(FL_OBJECT *ob, long data)
{
search_form_modified_flag = 1;
}


void search_headers_button_cb(FL_OBJECT *ob, long data)
{
search_headers_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_headers_from_button_cb(FL_OBJECT *ob, long data)
{
search_headers_from_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_headers_to_button_cb(FL_OBJECT *ob, long data)
{
search_headers_to_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_headers_subject_button_cb(FL_OBJECT *ob, long data)
{
search_headers_subject_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_headers_references_button_cb(FL_OBJECT *ob, long data)
{
search_headers_references_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_headers_others_button_cb(FL_OBJECT *ob, long data)
{
search_headers_others_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_bodies_button_cb(FL_OBJECT *ob, long data)
{
search_bodies_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_attachments_button_cb(FL_OBJECT *ob, long data)
{
search_attachments_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_case_sensitive_button_cb(FL_OBJECT *ob, long data)
{
search_case_sensitive_flag = fl_get_button(ob);
search_form_modified_flag = 1;
}


void search_keywords_input_field_cb(FL_OBJECT *ob, long data)
{
search_form_modified_flag = 1;
}


void search_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> search);

if(search_form_modified_flag)
	{
	/* restore from disc */
	load_search_settings();
	}

}/* end function search_cancel_button_cb */


void search_accept_button_cb(FL_OBJECT *ob, long data)
{
if(search_form_modified_flag)
	{
	save_search_settings();/* this will write the text fields to disk */
	load_search_settings();/* this will set the variables from disk */
	search_form_modified_flag = 0;
	}

if(! search_keywords)
	{
	fl_show_alert("No search keywords specified", "", "", 0);
	return;
	}
else /* a NULL pointer will crash */
	{
	if(strlen(search_keywords) == 0)
		{
		fl_show_alert("No search keywords specified", "", "", 0);
		return;
		}
	}
	
if(!search_postings_flag && !search_articles_flag)
	{
	fl_show_alert("No postings or articles button pressed",\
	"nothing to search for", "", 0);
	return;
	}

if(search_postings_flag || search_articles_flag)
	{
	if(!search_headers_flag && !search_bodies_flag &&\
	!search_attachments_flag)
		{
		fl_show_alert("Do not know what to search",\
		"Select one or more of these:",\
		"headers bodies attachments", 0);
		return;
		}
	}

if(search_headers_flag)
	{
	if(!search_headers_from_flag && !search_headers_to_flag &&\
	!search_headers_subject_flag && !search_headers_references_flag &&\
	!search_headers_others_flag)
		{
		fl_show_alert("Do not know what header lines to search", \
		"Select one or more header types", "", 0);
		return;
		}
	}

fl_hide_form(fd_NewsFleX -> search);

if(search_postings_flag)
	{
	if(start_postings_database_name) free(start_postings_database_name);
	start_postings_database_name = strsave(postings_database_name);
	if(! start_postings_database_name) return;

	first_matching_posting = -1;
/*	start_posting = selected_posting;*/
	start_posting = -1;

	if(first_matching_postings_database_name)\
	free(first_matching_postings_database_name);
	first_matching_postings_database_name = NULL;	

	/* this will search articles if all postings done */
	search_next_posting();/* search articles_flag not set, only postings */
	return;
	}/* end if search_posting_flag */
else if(search_articles_flag)
	{
	if(start_newsserver_and_database) free(start_newsserver_and_database);
	start_newsserver_and_database = malloc(\
	strlen(news_server_name) + strlen(" ") + strlen(database_name) + 1);
	if(! start_newsserver_and_database) return;
	strcpy(start_newsserver_and_database, news_server_name);
	strcat(start_newsserver_and_database, " ");
	strcat(start_newsserver_and_database, database_name);

	strcpy(start_group, selected_group);

	start_article = selected_article;

	first_matching_article = -1;

	if(first_matching_group) free(first_matching_group);
	first_matching_group = (char *) strsave("");/* use NULL ? */

	search_next_article(search_groups_keywords);
	return;
	}/* end else if search_articles_flag */
}/* end function search_accept_button_cb */


void question_no_button_cb(FL_OBJECT *ob, long data)
{
ask_question_flag = 0;
/*fl_hide_form(fd_NewsFleX -> question);*/
}/* end function question_no_button_cb */


void question_yes_button_cb(FL_OBJECT *ob, long data)
{
char *strsave();

ask_question_flag = 0;
if(search_last_search == SEARCH_POSTINGS)
	{
	search_next_posting();
	/*
	this will search articles when finished,
	if search_articles_flag set
	*/
	return;
	}
if(search_last_search == SEARCH_ARTICLES)
	{
	search_next_article(search_groups_keywords);
	return;
	}
/*fl_hide_form(fd_NewsFleX -> question);*/
}/* end function question_yes_button_cb */


