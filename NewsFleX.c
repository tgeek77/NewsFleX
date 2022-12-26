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

FD_NewsFleX *fdui;


FD_NewsFleX *create_form_NewsFleX(void)
{
extern int idle_cb();
FL_OBJECT *obj;
fdui = (FD_NewsFleX *) fl_calloc(1, sizeof(*fdui));

fdui -> posting_period_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> posting_period_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 175, 5, 350, 30,\
	"P O S T I N G  P E R I O D  B R O W S E R");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> posting_period_form_browser = obj =\
	fl_add_browser(FL_HOLD_BROWSER, 20, 100, 560, 300, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, posting_period_form_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, posting_period_form_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);

fdui -> posting_period_form_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 160, 400, 420, 30, "Period?");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, posting_period_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> posting_period_form_delete_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 290, 90, 30, "DELETE");
	fl_set_object_callback(obj, posting_period_form_delete_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> posting_period_form_add_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 400, 90, 30, "ADD");
	fl_set_object_callback(obj, posting_period_form_add_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

/*
fdui -> posting_period_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, posting_period_form_cancel_button_cb, 0);
*/

fdui -> posting_period_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "CLOSE");
	fl_set_object_callback(obj, posting_period_form_accept_button_cb, 0);
	
fdui -> posting_period_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, posting_period_form_fontsize_down_button_cb, 0);

fdui -> posting_period_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, posting_period_form_fontsize_up_button_cb, 0);

fl_end_form(); /* posting_period_form */


fdui -> newsserver_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> newsserver_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 175, 5, 350, 30,\
	"N E W S  S E R V E R  B R O W S E R");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> newsserver_form_browser = obj =\
	fl_add_browser(FL_HOLD_BROWSER, 20, 100, 560, 300, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, newsserver_form_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, newsserver_form_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);

fdui -> newsserver_form_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 160, 400, 420, 30, "News server");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, newsserver_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> newsserver_form_delete_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 290, 90, 30, "DELETE");
	fl_set_object_callback(obj, newsserver_form_delete_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> newsserver_form_add_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 400, 90, 30, "ADD");
	fl_set_object_callback(obj, newsserver_form_add_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

/*
fdui -> newsserver_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, newsserver_form_cancel_button_cb, 0);
*/

fdui -> newsserver_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "CLOSE");
	fl_set_object_callback(obj, newsserver_form_accept_button_cb, 0);
	
fdui -> newsserver_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, newsserver_form_fontsize_down_button_cb, 0);

fdui -> newsserver_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, newsserver_form_fontsize_up_button_cb, 0);

fl_end_form(); /* newsserver_form */


fdui -> incoming_mail_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> incoming_mail_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 200, 5, 300, 30,\
	"I N C O M I N G  M A I L");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> incoming_mail_form_browser = obj =\
	fl_add_browser(FL_SELECT_BROWSER, 20, 100, 560, 300, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, incoming_mail_form_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, incoming_mail_form_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);

/*
fdui -> incoming_mail_form_mark_read_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 370, 90, 30, "MARK READ");
	fl_set_object_callback(obj, incoming_mail_form_mark_read_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);
*/

fdui -> incoming_mail_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CLOSE");
	fl_set_object_callback(obj, incoming_mail_form_cancel_button_cb, 0);

/*
fdui -> incoming_mail_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, incoming_mail_form_accept_button_cb, 0);
*/

fdui -> incoming_mail_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, incoming_mail_form_fontsize_down_button_cb, 0);

fdui -> incoming_mail_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, incoming_mail_form_fontsize_up_button_cb, 0);

fl_end_form(); /* incoming_mail_form */


fdui -> helpers_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> helpers_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"H E L P E R S");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> helpers_form_input = obj =\
    /* only used as output */
	fl_add_input(FL_MULTILINE_INPUT, 20, 50, 660, 380, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, helpers_form_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> helpers_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, helpers_form_cancel_button_cb, 0);

fdui -> helpers_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, helpers_form_accept_button_cb, 0);

fdui -> helpers_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, helpers_form_fontsize_down_button_cb, 0);

fdui -> helpers_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, helpers_form_fontsize_up_button_cb, 0);

fl_end_form(); /* helpers_form */


fdui -> error_log_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> error_log_form_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"E R R O R  L O G");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> error_log_form_input = obj =\
    /* only used as output */
	fl_add_input(FL_MULTILINE_INPUT, 20, 50, 550, 350, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, error_log_form_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> error_log_form_clear_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 370, 90, 30, "CLEAR");
	fl_set_object_callback(obj, error_log_form_clear_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> error_log_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, error_log_form_cancel_button_cb, 0);

fdui -> error_log_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, error_log_form_accept_button_cb, 0);

fdui -> error_log_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, error_log_form_fontsize_down_button_cb, 0);

fdui -> error_log_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, error_log_form_fontsize_up_button_cb, 0);

fl_end_form(); /* error_log_form */


fdui -> highlight =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> highlight_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"K E Y W O R D  H I G H L I G H T I N G");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> highlight_postings_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 50, 90, 30, "POSTINGS");
	fl_set_object_callback(obj, highlight_postings_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_articles_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 100, 90, 30, "ARTICLES");
	fl_set_object_callback(obj, highlight_articles_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
	
fdui -> highlight_headers_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 150, 90, 30, "HEADERS");
	fl_set_object_callback(obj, highlight_headers_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_urls_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 200, 90, 30, "URL LIST");
	fl_set_object_callback(obj, highlight_urls_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_headers_from_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 150, 90, 30, "From");
	fl_set_object_callback(obj, highlight_headers_from_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_headers_to_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 150, 90, 30, "To");
	fl_set_object_callback(obj, highlight_headers_to_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_headers_subject_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 390, 150, 90, 30, "Subject");
	fl_set_object_callback(obj, highlight_headers_subject_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_headers_references_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 490, 150, 90, 30, "References");
	fl_set_object_callback(obj, highlight_headers_references_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_headers_others_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 150, 90, 30, "Others");
	fl_set_object_callback(obj, highlight_headers_others_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_urls_url_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 200, 90, 30, "URL");
	fl_set_object_callback(obj, highlight_urls_url_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_urls_description_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 200, 90, 30, "Description");
	fl_set_object_callback(obj, highlight_urls_description_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> highlight_case_sensitive_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 300, 90, 30, "CASE SENS");
	fl_set_object_callback(obj, highlight_case_sensitive_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> highlight_keywords_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 190, 300, 490, 30, "keywords");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, highlight_keywords_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> highlight_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, highlight_cancel_button_cb, 0);

fdui -> highlight_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, highlight_accept_button_cb, 0);

/*
fdui -> highlight_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, highlight_fontsize_down_button_cb, 0);

fdui -> highlight_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, highlight_fontsize_up_button_cb, 0);
*/

fl_end_form(); /* highlight */


fdui -> url_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> url_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"U R L  B R O W S E R");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> url_form_browser_command_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 160, 40, 400, 30, "Get URL command");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, url_form_browser_command_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> url_form_browser = obj =\
	fl_add_browser(FL_HOLD_BROWSER, 20, 120, 560, 270, "");/* was 100, 560 */
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, url_form_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, url_form_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);

fdui -> url_form_external_browser_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 40, 90, 30, "INT BROWSER");
	fl_set_object_callback(obj, url_form_external_browser_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> url_form_search_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 160, 80, 200, 30, "Search keyword");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, url_form_search_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> url_form_search_case_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 380, 80, 90, 30, "CASE SENS");
	fl_set_object_callback(obj, url_form_search_case_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> url_form_search_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 480, 80, 90, 30, "SEARCH");
	fl_set_object_callback(obj, url_form_search_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> url_form_show_marked_only_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 80, 90, 30, "SHOW ALL");
	fl_set_object_callback(obj, url_form_show_marked_only_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> url_form_browse_online_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 120, 90, 30, "BR OFFLINE");
	fl_set_object_callback(obj, url_form_browse_online_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> url_form_get_pictures_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 160, 90, 30, "NO PICTURES");
	fl_set_object_callback(obj, url_form_get_pictures_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> url_form_depth_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 635, 200, 45, 30, "Depth");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, url_form_depth_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> url_form_host_only_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 240, 90, 30, "HOST ONLY");
	fl_set_object_callback(obj, url_form_host_only_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> url_form_get_daily_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 280, 90, 30, "ONCE");
	fl_set_object_callback(obj, url_form_get_daily_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> url_form_delete_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 320, 90, 30, "DELETE");
	fl_set_object_callback(obj, url_form_delete_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> url_form_clear_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 360, 90, 30, "CLEAR");
	fl_set_object_callback(obj, url_form_clear_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> url_form_add_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 400, 90, 30, "ADD");
	fl_set_object_callback(obj, url_form_add_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> url_form_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 60, 400, 520, 30, "URL");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, url_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> url_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, url_form_cancel_button_cb, 0);

fdui -> url_form_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, url_form_accept_button_cb, 0);
	
fdui -> url_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, url_form_fontsize_down_button_cb, 0);

fdui -> url_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, url_form_fontsize_up_button_cb, 0);

fl_end_form(); /* url_form */


fdui -> coding_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> coding_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"C O D E C");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> coding_form_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 100, 660, 300, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	coding_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> coding_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	coding_form_fontsize_up_button_cb, 0);

fdui -> coding_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	coding_form_fontsize_down_button_cb, 0);

fdui -> coding_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, coding_form_cancel_button_cb, 0);

fl_end_form(); /* coding_form */


fdui -> posting_header_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> posting_header_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"P O S T I N G  H E A D E R ");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> posting_header_form_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 100, 660, 300, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	posting_header_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> posting_header_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	posting_header_form_fontsize_up_button_cb, 0);

fdui -> posting_header_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	posting_header_form_fontsize_down_button_cb, 0);

fdui -> posting_header_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, posting_header_form_cancel_button_cb, 0);

fl_end_form(); /* posting_header_form */


fdui -> article_header_form =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> article_header_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"A R T I C L E  H E A D E R ");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> article_header_form_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 100, 660, 300, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	article_header_form_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> article_header_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	article_header_form_fontsize_up_button_cb, 0);

fdui -> article_header_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	article_header_form_fontsize_down_button_cb, 0);

fdui -> article_header_form_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, article_header_form_cancel_button_cb, 0);

fl_end_form(); /* article_header_form */


fdui -> summary_editor =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> summary_editor_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"S U M M A R Y  E D I T O R");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> summary_editor_insert_file_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 130, 50, 90, 30, "INSERT FILE");
	fl_set_object_callback(obj, summary_editor_insert_file_button_cb, 0);

fdui -> summary_editor_write_file_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 240, 50, 90, 30, "WRITE FILE");
	fl_set_object_callback(obj, summary_editor_write_file_button_cb, 0);
	/* UNTIL fl_get_input_selected implemented in xforms */
	fl_hide_object(obj);

fdui -> summary_editor_clear_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 350, 50, 90, 30, "CLEAR");
	fl_set_object_callback(obj, summary_editor_clear_button_cb, 0);

fdui -> summary_editor_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 100, 660, 300, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	summary_editor_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_FIXED_STYLE);

fdui -> summary_editor_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	summary_editor_fontsize_up_button_cb, 0);

fdui -> summary_editor_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	summary_editor_fontsize_down_button_cb, 0);

fdui -> summary_editor_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, summary_editor_accept_button_cb, 0);

fdui -> summary_editor_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, summary_editor_cancel_button_cb, 0);

fl_end_form(); /* summary */


fdui -> posting_editor =\
fl_bgn_form(FL_UP_BOX, 790, 570);

fdui -> posting_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 295, 5, 200, 30,\
	"P O S T I N G  E D I T O R");
	fl_set_object_lalign(obj, FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> posting_coding_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 680, 10, 90, 20, "GOODIES");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, posting_coding_menu_cb, 0);
    fl_set_menu(obj,\
"PGP ENCRYPT|\
PGP CLEAR SIGN|\
PGP APPEND PUBLIC KEY|\
OTP ENCRYPT|\
OTP CREATE RANDOM KEY FILE|\
SHOW UNENCRYPTED|\
SEARCH KEYWORD|\
WORD WRAP");
	fl_set_menu_item_mode(obj, WORD_WRAP, FL_PUP_BOX);

fdui -> posting_attach_file_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 50, 90, 30, "ATTACH FILE");
	fl_set_object_callback(obj, posting_attach_file_button_cb, 0);

fdui -> posting_insert_file_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 130, 50, 90, 30, "INSERT FILE");
	fl_set_object_callback(obj, posting_insert_file_button_cb, 0);

fdui -> posting_cross_post_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 240, 50, 90, 30, "CROSS POST");
	fl_set_object_callback(obj, posting_cross_post_button_cb, 0);

fdui -> posting_write_file_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 240, 50, 90, 30, "WRITE FILE");
	fl_set_object_callback(obj, posting_write_file_button_cb, 0);
	/* UNTIL fl_get_input_selected implemented in xforms */
	fl_hide_object(obj);

fdui -> posting_editor_clear_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 350, 50, 90, 30, "CLEAR");
	fl_set_object_callback(obj, posting_editor_clear_button_cb, 0);

fdui -> posting_editor_signature_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 460, 50, 90, 30, "SIGNATURE");
	fl_set_object_callback(obj, posting_editor_signature_button_cb, 0);

fdui -> posting_summary_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 570, 50, 90, 30, "SUMMARY");
	fl_set_object_callback(obj, posting_summary_button_cb, 0);

fdui -> posting_add_url_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 485, 10, 65, 30, "ADD URL");
	fl_set_object_callback(obj, posting_add_url_button_cb, 0);

fdui -> posting_alt_editor_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 570, 10, 90, 30, "ALT EDITOR");
	fl_set_object_callback(obj, posting_alt_editor_button_cb, 0);

fdui -> posting_alt_editor_command_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 70, 10, 220, 20, "Alt editor");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, posting_alt_editor_command_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
            
fdui -> posting_custom_headers_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 680, 50, 90, 30, "HEADERS");
	fl_set_object_callback(obj, posting_custom_headers_button_cb, 0);

fdui -> posting_to_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 65, 100, 705, 20, "To");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, posting_to_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
            
fdui -> posting_subject_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 65, 130, 705, 40, "Subject");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, posting_subject_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
            
fdui -> posting_attachment_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 65, 180, 705, 20, "Attachment");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, posting_attachment_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
            
fdui -> posting_body_editor = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 210, 750, 310, "");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, posting_body_editor_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_lstyle(obj, FL_FIXED_STYLE);
/*	fl_set_input_maxchars(obj, 79);*/

fdui -> posting_editor_color_box = obj =\
	fl_add_colbox(FL_NORMAL_COLBOX, 20, 210, 750, 310, "");
	fl_set_object_color(obj, FL_YELLOW, FL_RED);
	fl_set_object_callback(obj, posting_color_box_cb, 0);
	fl_set_object_boxtype(obj, FL_DOWN_BOX);
	fl_set_object_lstyle(obj, FL_FIXED_STYLE);
	fl_hide_object(obj);

fdui -> posting_editor_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 570, 540, 20, 20, "-");
	fl_set_object_callback(obj, posting_editor_fontsize_down_button_cb, 0);

fdui -> posting_editor_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 600, 540, 20, 20, "+");
	fl_set_object_callback(obj, posting_editor_fontsize_up_button_cb, 0);

fdui -> posting_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 530, 90, 30, "CANCEL");
	fl_set_object_callback(obj, posting_cancel_button_cb, 0);

fdui -> posting_send_later_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 460, 530, 90, 30, "SEND LATER");
	fl_set_object_callback(obj, posting_send_later_button_cb, 0);

fdui -> posting_send_now_button = obj  =\
	fl_add_button(FL_NORMAL_BUTTON, 680, 530, 90, 30, "SEND NOW");
	fl_set_object_callback(obj, posting_send_now_button_cb, 0);

fl_end_form(); /* posting editor */


fdui -> custom_headers_editor =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> custom_headers_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 150, 5, 400, 30,\
	"C U S T O M  H E A D E R S  E D I T O R");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> global_custom_headers_enable_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 50, 90, 30,\
	"ENABLE GLOBAL CUSTOM HEADERS");
	fl_set_object_callback(obj,\
	global_custom_headers_enable_button_cb, 0);
	fl_set_object_lalign(obj, FL_ALIGN_RIGHT);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> global_custom_headers_editor_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 100, 660, 100, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	global_custom_headers_editor_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> local_custom_headers_enable_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 250, 90, 30,\
	"ENABLE LOCAL CUSTOM HEADERS");
	fl_set_object_callback(obj,\
	local_custom_headers_enable_button_cb, 0);
	fl_set_object_lalign(obj, FL_ALIGN_RIGHT);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> local_custom_headers_editor_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 300, 660, 100, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj,\
	local_custom_headers_editor_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> custom_headers_editor_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	custom_headers_editor_fontsize_up_button_cb, 0);

fdui -> custom_headers_editor_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	custom_headers_editor_fontsize_down_button_cb, 0);

fdui -> custom_headers_editor_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, custom_headers_editor_accept_button_cb, 0);

fdui -> custom_headers_editor_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON,  20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, custom_headers_editor_cancel_button_cb, 0);

fl_end_form(); /* custom headers */


fdui -> question =\
fl_bgn_form(FL_UP_BOX, 300, 210);

fdui -> question_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 100, 5, 100, 30,\
	"LOCAL SEARCH");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> question_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 20, 50, 260, 90, "");
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lcol(obj, FL_BLACK);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> question_no_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 160, 90, 30, "CANCEL");
	fl_set_object_callback(obj, question_no_button_cb, 0);

fdui -> question_yes_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 190, 160, 90, 30, "NEXT");
	fl_set_object_callback(obj, question_yes_button_cb, 0);

fl_end_form(); /* question */


/*
fdui -> secret =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> secret_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"S E C R E T  A G E N T");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> secret_password_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 480, 50, 200, 20, "Secret password");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, secret_password_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> secret_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, secret_cancel_button_cb, 0);

fdui -> secret_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, secret_accept_button_cb, 0);

fl_end_form();*/ /* secret */


fdui -> filter =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> filter_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 300, 5, 100, 30,\
	"F I L T E R");
	fl_set_object_lalign(obj, FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> filter_password_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 480, 50, 200, 20, "Filter password");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, filter_password_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> filter_postings_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 50, 90, 30, "POSTINGS");
	fl_set_object_callback(obj, filter_postings_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_articles_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 100, 90, 30, "ARTICLES");
	fl_set_object_callback(obj, filter_articles_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_groups_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 190, 100, 390, 30, "Groups");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, filter_groups_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);

fdui -> filter_headers_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 150, 90, 30, "HEADERS");
	fl_set_object_callback(obj, filter_headers_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_headers_from_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 150, 90, 30, "From");
	fl_set_object_callback(obj, filter_headers_from_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_enable_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 50, 90, 30, "ENABLE");
	fl_set_object_callback(obj, filter_enable_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> filter_headers_to_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 150, 90, 30, "To");
	fl_set_object_callback(obj, filter_headers_to_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_headers_subject_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 390, 150, 90, 30, "Subject");
	fl_set_object_callback(obj, filter_headers_subject_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_headers_newsgroups_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 490, 150, 90, 30, "Newsgroups");
	fl_set_object_callback(obj, filter_headers_newsgroups_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_headers_others_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 150, 90, 30, "Others");
	fl_set_object_callback(obj, filter_headers_others_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_bodies_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 200, 90, 30, "BODIES");
	fl_set_object_callback(obj, filter_bodies_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_attachments_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 250, 90, 30, "ATTACHMENTS");
	fl_set_object_callback(obj, filter_attachments_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_groups_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 300, 90, 30, "GROUPS");
	fl_set_object_callback(obj, filter_groups_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_list_browser = obj =\
	fl_add_browser(FL_MULTI_BROWSER, 190, 200, 390, 90, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, filter_list_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, filter_list_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);

fdui -> filter_list_browser_delete_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 200, 90, 30, "DELETE");
	fl_set_object_callback(obj, filter_list_browser_delete_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_RED);

fdui -> filter_list_browser_new_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 250, 90, 30, "NEW");
	fl_set_object_callback(obj, filter_list_browser_new_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> filter_keywords_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 190, 300, 390, 30, "keywords");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, filter_keywords_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);
	
fdui -> filter_case_sensitive_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 350, 90, 30, "CASE SENS");
	fl_set_object_callback(obj, filter_case_sensitive_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> filter_keywords_and_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 350, 90, 30, "AND");
	fl_set_object_callback(obj, filter_keywords_and_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> filter_keywords_not_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 390, 350, 90, 30, "NOT");
	fl_set_object_callback(obj, filter_keywords_not_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> filter_keywords_only_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 490, 350, 90, 30, "ONLY");
	fl_set_object_callback(obj, filter_keywords_only_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_GREEN);

fdui -> filter_allow_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 350, 90, 30, "ALLOW");
	fl_set_object_callback(obj, filter_allow_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> filter_form_no_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 130, 400, 60, 30, "Inhibit");
	fl_set_object_lalign(obj, FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);

fdui -> filter_groups_show_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 400, 90, 30, "SHOW GROUP");
	fl_set_object_callback(obj, filter_groups_show_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> filter_headers_show_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 400, 90, 30, "SHOW HEAD");
	fl_set_object_callback(obj, filter_headers_show_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> filter_bodies_show_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 390, 400, 90, 30, "SHOW BODY");
	fl_set_object_callback(obj, filter_bodies_show_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> filter_attachments_show_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 490, 400, 90, 30, "SHOW ATT");
	fl_set_object_callback(obj, filter_attachments_show_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> filter_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj,\
	filter_form_fontsize_down_button_cb, 0);

fdui -> filter_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj,\
	filter_form_fontsize_up_button_cb, 0);

fdui -> filter_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, filter_cancel_button_cb, 0);

fdui -> filter_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, filter_accept_button_cb, 0);

fl_end_form(); /* filter */


fdui -> search =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> search_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"L O C A L  S E A R C H");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> search_postings_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 50, 90, 30, "POSTINGS");
	fl_set_object_callback(obj, search_postings_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_articles_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 100, 90, 30, "ARTICLES");
	fl_set_object_callback(obj, search_articles_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_groups_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 190, 100, 490, 30, "Groups");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, search_groups_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> search_headers_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 150, 90, 30, "HEADERS");
	fl_set_object_callback(obj, search_headers_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_headers_from_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 190, 150, 90, 30, "From");
	fl_set_object_callback(obj, search_headers_from_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_headers_to_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 290, 150, 90, 30, "To");
	fl_set_object_callback(obj, search_headers_to_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_headers_subject_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 390, 150, 90, 30, "Subject");
	fl_set_object_callback(obj, search_headers_subject_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_headers_references_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 490, 150, 90, 30, "References");
	fl_set_object_callback(obj, search_headers_references_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_headers_others_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 590, 150, 90, 30, "Others");
	fl_set_object_callback(obj, search_headers_others_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_bodies_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 200, 90, 30, "BODIES");
	fl_set_object_callback(obj, search_bodies_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_attachments_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 250, 90, 30, "ATTACHMENTS");
	fl_set_object_callback(obj, search_attachments_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> search_case_sensitive_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 20, 300, 90, 30, "CASE SENS");
	fl_set_object_callback(obj, search_case_sensitive_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);

fdui -> search_keywords_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 190, 300, 490, 30, "keywords");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, search_keywords_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> search_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, search_cancel_button_cb, 0);

fdui -> search_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, search_accept_button_cb, 0);

/*
fdui -> search_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, search_fontsize_down_button_cb, 0);

fdui -> search_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, search_fontsize_up_button_cb, 0);
*/

fl_end_form(); /* search */


fdui -> desc =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> desc_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 250, 5, 200, 30,\
	"D E S C R I P T I O N");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> desc_input = obj =\
    /* only used as output, can paste from here with mouse (urls!) */
	fl_add_input(FL_MULTILINE_INPUT, 20, 50, 660, 350, "");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, desc_input_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);

fdui -> desc_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, desc_cancel_button_cb, 0);

fdui -> desc_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, desc_accept_button_cb, 0);
	fl_hide_object(obj);

fdui -> desc_input_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, desc_input_fontsize_down_button_cb, 0);

fdui -> desc_input_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, desc_input_fontsize_up_button_cb, 0);

fl_end_form(); /* descr */


fdui -> setup =\
fl_bgn_form(FL_UP_BOX, 700, 500);

fdui -> setup_form_title = obj =\
	fl_add_text(FL_NORMAL_TEXT, 300, 5, 100, 30,\
	"S E T U P");
	fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> pid_test_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 170, 200, 20, "pppd pid");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, pid_test_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> auto_connect_to_net_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 620, 200, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_BLUE);
    fl_set_object_callback(obj, auto_connect_to_net_button_cb, 0);

fdui -> auto_connect_to_net_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 200, 200, 20, "Connect script");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, auto_connect_to_net_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> auto_disconnect_from_net_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 620, 230, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_RED);
    fl_set_object_callback(obj, auto_disconnect_from_net_button_cb, 0);

fdui -> auto_disconnect_from_net_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 230, 200, 20, "Disconnect script");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, auto_disconnect_from_net_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> auto_execute_application_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 620, 260, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, auto_execute_application_button_cb, 0);

fdui -> auto_execute_application_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 260, 200, 20, "Get mail script");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, auto_execute_application_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> get_new_headers_enable_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 620, 290, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_DARKORANGE);
    fl_set_object_callback(obj, get_new_headers_enable_button_cb, 0);

fdui -> get_new_headers_time_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 290, 200, 20, "Go online at");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, get_new_headers_time_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> server_username_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 320, 200, 20, "username");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, server_username_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> server_password_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 350, 200, 20, "password");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, server_password_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> setup_form_posting_source_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 620, 380, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_posting_source_button_cb, 0);

fdui ->setup_form_posting_source_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 410, 380, 200, 20, "posting source");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, setup_form_posting_source_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> tab_size_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 200, 50, 20, "Tab size");
	fl_set_input_return(obj, FL_RETURN_END_CHANGED);
	fl_set_object_callback(obj, tab_size_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> setup_form_posting_wrap_chars_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 260, 30, 20, "Posting wrap");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
    fl_set_object_callback(\
		obj, setup_form_posting_wrap_chars_input_field_cb, 0);

fdui -> setup_form_lines_first_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 100, 290, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_lines_first_button_cb, 0);

fdui -> setup_form_lines_first_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 10, 290, 90, 20,\
	"Lines first");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_cut_off_sig_in_reply_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 100, 320, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(\
		obj, setup_form_cut_off_sig_in_reply_button_cb, 0);

fdui -> setup_form_cut_off_sig_in_reply_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 10, 320, 90, 20,\
	"Cut sig in reply");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_beep_on_mail_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 100, 350, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(\
		obj, setup_form_beep_on_mail_button_cb, 0);

fdui -> setup_form_beep_on_mail_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 10, 350, 90, 20,\
	"Beep on mail");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> desc_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 230, 60, 20, "NO");
	fl_set_object_color(obj, FL_COL1, FL_GREEN);
    fl_set_object_callback(obj, desc_button_cb, 0);

fdui -> desc_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 140, 230, 100, 20,\
	"Description popup");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_insert_headers_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 260, 60, 20, "ONLINE");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_insert_headers_button_cb, 0);

fdui -> setup_form_insert_headers_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 155, 260, 85, 20,\
	"Insert headers");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_get_by_message_id_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 290, 60, 20, "NUMBER");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_get_by_message_id_button_cb, 0);

fdui -> setup_form_get_by_message_id_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 162, 290, 78, 20,\
	"Get bodies by");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_show_only_once_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 320, 60, 20, "ONCE");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_show_only_once_button_cb, 0);

fdui -> setup_form_show_only_once_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 165, 320, 75, 20,\
	"Show articles");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_check_local_mail_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 100, 380, 60, 20, "ONCE");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_check_local_mail_button_cb, 0);

fdui -> setup_form_check_local_mail_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 10, 380, 90, 20,\
	"Check local mail");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> setup_form_local_mail_check_interval_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 240, 380, 60, 20, "interval");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj,\
	setup_form_local_mail_check_interval_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
/*	fl_set_object_lstyle(obj, FL_BOLD_STYLE);*/
/*	fl_set_object_lsize(obj, FL_MEDIUM_SIZE);*/

/*
fdui ->  setup_form_local_mail_check_interval_input_field_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 160, 380, 80, 20,\
	"check interval");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
*/

fdui -> setup_form_do_not_use_sendmail_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 350, 60, 20, "YES");
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);
    fl_set_object_callback(obj, setup_form_do_not_use_sendmail_button_cb, 0);

fdui -> setup_form_do_not_use_sendmail_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 165, 350, 75, 30,\
	"Use sendmail");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> width_button = obj =\
	fl_add_button(FL_PUSH_BUTTON, 240, 200, 60, 20, "FIXED");
	fl_set_object_color(obj, FL_COL1, FL_GREEN);
    fl_set_object_callback(obj, width_button_cb, 0);

fdui -> width_button_text = obj =\
	fl_add_text(FL_NORMAL_TEXT, 160, 195, 80, 30,\
	"Text spacing");
	fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);

fdui -> news_server_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 50, 200, 20, "News server");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, news_server_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
	
fdui -> mail_server_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 80, 200, 20, "Mail server");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, mail_server_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> news_server_port_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 335, 50, 50, 20, "Port");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, news_server_port_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> mail_server_port_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 335, 80, 50, 20, "Port");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, mail_server_port_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> connect_to_news_server_timeout_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 485, 50, 50, 20, "Connect timeout");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, connect_to_news_server_timeout_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> connect_to_mail_server_timeout_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 485 , 80, 50, 20, "Connect timeout");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, connect_to_mail_server_timeout_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> real_name_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 110, 200, 20, "User name");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, real_name_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> user_email_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 100, 140, 200, 20, "User email");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, user_email_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

fdui -> maximum_headers_per_group_input_field = obj =\
	fl_add_input(FL_NORMAL_INPUT, 250, 170, 50, 20,\
	"Maximum headers per group");
	fl_set_input_return(obj, FL_RETURN_CHANGED);
	fl_set_object_callback(obj, maximum_headers_per_group_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

/*
fdui -> setup_form_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 400, 450, 20, 20, "-");
	fl_set_object_callback(obj, setup_form_fontsize_down_button_cb, 0);

fdui -> setup_form_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 430, 450, 20, 20, "+");
	fl_set_object_callback(obj, setup_form_fontsize_up_button_cb, 0);
*/

fdui -> setup_cancel_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 450, 90, 30, "CANCEL");
	fl_set_object_callback(obj, setup_cancel_button_cb, 0);

fdui -> setup_accept_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 590, 450, 90, 30, "ACCEPT");
	fl_set_object_callback(obj, setup_accept_button_cb, 0);
	
fl_end_form(); /* setup */


fdui -> NewsFleX =\
	fl_bgn_form(FL_NO_BOX, 790, 570);
	fl_set_idle_callback(idle_cb, 0);
	fl_add_box(FL_UP_BOX, 0, 0, 790, 570, "");
	
fdui -> group_list_browser = obj =\
	fl_add_browser(FL_MULTI_BROWSER, 20, 110, 750, 420, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_show_object(obj);
	fl_set_object_callback(obj, group_list_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, group_list_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);
	
fdui -> article_list_browser = obj =\
	fl_add_browser(FL_MULTI_BROWSER, 20, 110, 750, 420, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, article_list_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj,\
	article_list_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);
	fl_hide_object(obj);

fdui -> posting_list_browser = obj =\
	fl_add_browser(FL_MULTI_BROWSER, 20, 110, 750, 420, "");
	fl_set_object_color(obj, FL_WHITE, 15);
	fl_set_object_callback(obj, posting_list_browser_cb, 0);
	fl_set_browser_dblclick_callback(obj, posting_list_browser_double_cb, 0);
	fl_set_object_dblbuffer(obj, 1);
	fl_hide_object(obj);

fdui -> article_body_input_field = obj =\
	fl_add_input(FL_MULTILINE_INPUT, 20, 110, 750, 420, "");
	fl_set_input_return(obj, FL_RETURN_ALWAYS);
	fl_set_object_callback(obj, article_body_input_field_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_WHITE);
	fl_set_object_dblbuffer(obj, 1);
	fl_set_object_lstyle(obj, FL_FIXED_STYLE);
	fl_hide_object(obj);
	
fdui -> groups_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 240, 10, 90, 20, "GROUPS");
    fl_set_object_boxtype(obj, FL_FRAME_BOX);
    fl_set_object_callback(obj, groups_menu_cb, 0);
    fl_set_menu(obj,\
"SUBSCRIBE|\
UNSUBSCRIBE|\
SHOW ALL GROUPS|\
SHOW ONLY SUBSCRIBED GROUPS|\
SHOW ONLY NEW GROUPS|\
DELETE GROUP|\
DELETE ALL NOT LOCKED ARTICLES IN SUBSCRIBED GROUPS|\
MARK ALL GROUPS READ|\
SET DISPLAY FILTER|\
SET MAXIMUM HEADERS|\
ALWAYS GET ALL ARTICLES IN GROUP|\
ONLY GET MARKED ARTICLES IN GROUP|\
SORTED DISPLAY (SLOW)\
");
	fl_set_menu_item_mode(obj, SHOW_ALL_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(obj, SHOW_ONLY_SUBSCRIBED_GROUPS, FL_PUP_CHECK);
	fl_set_menu_item_mode(obj, SHOW_ONLY_NEW_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(obj, SORTED_DISPLAY, FL_PUP_BOX);
/*
	fl_set_menu_item_mode(obj, ALWAYS_GET_ALL_ARTICLES_IN_GROUP, FL_PUP_BOX);
	fl_set_menu_item_mode(obj, ONLY_GET_MARKED_ARTICLES_IN_GROUP, FL_PUP_CHECK);
*/
             
fdui -> article_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 350, 10, 90, 20, "ARTICLES");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, article_menu_cb, 0);
    fl_set_menu(obj,\
    "MARK ARTICLE FOR RETRIEVAL|\
UNMARK ARTICLE FOR RETRIEVAL|\
FOLLOW SUBJECT|\
DO NOT FOLLOW SUBJECT|\
FOLLOW THREAD|\
DO NOT FOLLOW THREAD|\
LOCK_ARTICLE|\
UNLOCK_ARTICLE|\
DELETE ARTICLE|\
DELETE ALL NOT LOCKED ARTICLES|\
MARK ARTICLE READ|\
MARK ARTICLE UNREAD|\
MARK ALL NEW ARTICLES FOR RETRIEVAL|\
UNMARK ALL NEW ARTICLES FOR RETRIEVAL|\
MARK ALL ARTICLES READ|\
MARK ALL ARTICLES UNREAD|\
SEARCH FOR KEYWORD|\
EXPORT ARTICLE PLUS HEADER|\
EXPORT ARTICLE ONLY|\
PGP DECRYPT|\
PGP CHECK SIGNATURE AND DECRYPT|\
OTP DECRYPT|\
CANCEL ARTICLE");

fdui -> posting_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 460, 10, 90, 20, "POSTING");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, posting_menu_cb, 0);
    fl_set_menu(obj,\
    "NEW ARTICLE|\
FOLLOW UP ARTICLE|\
REPLY VIA EMAIL|\
MARK FOR TRANSMIT|\
UNMARK FOR TRANSMIT|\
LOCK POSTING|\
UNLOCK POSTING|\
MARK SENT|\
UNMARK SENT|\
DELETE SELECTED POSTINGS");

fdui -> options_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 570, 10, 90, 20, "OPTIONS");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, options_menu_cb, 0);
    fl_set_menu(obj,\
"SETUP|\
HELPERS|\
SEARCH|\
UNDERSCORE|\
FILTER|\
REGENERATE SUBSCRIBED_GROUPS.DAT|\
REGENERATE ARTICLES.DAT|\
REGENERATE POSTINGS.DAT|\
INFO|\
DEBUG");

fl_set_menu_item_mode(obj, DEBUG_MODE, FL_PUP_BOX);

fdui -> online_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 130, 10, 90, 20, "ONLINE");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, online_menu_cb, 0);
    fl_set_menu(obj,\
"GET NEW HEADERS IN SELECTED GROUPS|\
GET NEW HEADERS IN SUBSCRIBED GROUPS|\
GET MARKED ARTICLE BODIES|\
GET MARKED URLS|\
REFRESH GROUP LIST|\
POST SELECTED POSTINGS|\
POST MARKED POSTINGS");

fdui -> file_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 20, 10, 90, 20, "FILE");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, file_menu_cb, 0);
    fl_set_menu(obj,\
    "EXIT");

fdui -> window_menu = obj =\
	fl_add_menu(FL_PULLDOWN_MENU, 680, 10, 90, 20, "WINDOW");
	fl_set_object_boxtype(obj, FL_FRAME_BOX);
	fl_set_object_callback(obj, window_menu_cb, 0);
    fl_set_menu(obj,\
"GROUP LIST|\
ARTICLE LIST|\
ARTICLE BODY|\
POSTING LIST|\
URL LIST|\
INCOMING MAIL|\
NEWS SERVER LIST|\
POSTING BOXES|\
ERROR LOG");

fdui -> group_list_button = obj  =\
	fl_add_button(FL_NORMAL_BUTTON, 20, 40, 90, 30, "GROUPS LIST");
	fl_set_object_callback(obj, group_list_button_cb, 0);

fdui -> article_list_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 130, 40, 90, 30, "ARTICLE LIST");
	fl_set_object_callback(obj, article_list_button_cb, 0);

fdui -> article_body_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 240, 40, 90, 30, "ARTICLE BODY");
	fl_set_object_callback(obj, article_body_button_cb, 0);

fdui -> view_article_header_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 240, 40, 90, 30, "VIEW HEADER");
	fl_set_object_callback(obj, view_article_header_button_cb, 0);
	fl_hide_object(obj);
	
fdui -> next_thread_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 460, 40, 90, 30, "ON SUBJECT");
    fl_set_object_callback(obj, next_thread_button_cb, 0);

fdui -> next_unread_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 350, 40, 90, 30, "NEXT UNREAD");
	fl_set_object_callback(obj,next_unread_button_cb,0);

fdui -> launche_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 570, 40, 90, 30, "LAUNCH");
	fl_set_object_callback(obj, launch_button_cb, 0);

fdui -> stop_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 680, 40, 90, 30, "STOP");
	fl_set_object_callback(obj, stop_button_cb, 0);

fdui -> browser_fontsize_up_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 570, 540, 20, 20, "-");
	fl_set_object_callback(obj, browser_fontsize_down_button_cb, 0);

fdui -> browser_fontsize_down_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 600, 540, 20, 20, "+");
	fl_set_object_callback(obj, browser_fontsize_up_button_cb, 0);

fdui -> html_decode_button = obj =\
	fl_add_button(FL_NORMAL_BUTTON, 710, 80, 20, 20, "N");
	fl_set_object_callback(obj, html_decode_button_cb, 0);
	fl_set_object_color(obj, FL_COL1, FL_YELLOW);

fdui -> description_display = obj =\
	fl_add_text(FL_NORMAL_TEXT, 20, 80, 680, 20, "");/* was 710 */
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	fl_set_object_lcol(obj, FL_BLUE);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);
	fl_set_object_lsize(obj, FL_NORMAL_SIZE);
	fl_set_object_label(obj, "SUBSCRIBED GROUPS");

fdui -> mail_display = obj =\
	fl_add_text(FL_NORMAL_TEXT, 740, 80, 30, 20, "");
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	fl_set_object_lcol(obj, FL_BLUE);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);
	fl_set_object_lsize(obj, FL_NORMAL_SIZE);
	fl_set_object_label(obj, ""); 

fdui -> command_status_display = obj =\
	fl_add_text(FL_NORMAL_TEXT, 20, 540, 540, 20, "");
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	fl_set_object_lcol(obj, FL_BLUE);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);

fdui -> line_status_display = obj =\
	fl_add_text(FL_NORMAL_TEXT, 630, 540, 65, 20, "Offline");
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	fl_set_object_lcol(obj, FL_BLUE);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);

fdui -> line_speed_display = obj =\
	fl_add_text(FL_NORMAL_TEXT, 705, 540, 65, 20, "");
	fl_set_object_boxtype(obj,FL_FRAME_BOX);
	fl_set_object_lstyle(obj,FL_BOLD_STYLE);
	fl_set_object_lcol(obj, FL_BLUE);
	fl_set_object_color(obj, FL_WHITE, FL_COL1);

fl_end_form(); /* NewsFleX */

return fdui;
}/* end function create_form_NewsFleX */


