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

#ifndef FD_NewsFleX_h_
#define FD_NewsFleX_h_


/*
for some weard reason the crypt () function is not found in glibc in RedHat 6.1 WHY?
Undefine for normal use
*/
/*#define NO_CRYPT*/

#include <forms.h>

#include "NewsFleX_proto.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h> /* for crypt() */
#include <sys/stat.h> /* for stat */
#include <ctype.h> /* for isdigit */
/* DO NOT #include <arpa/inet.h>*/  /* for inet_ntoa gcc -Wall gets confused */
/* To avoid error msg I have proto typed inet_ntoa( unsigned long) (it is) */ 
#include <signal.h>
#include <sys/wait.h>
#include <errno.h> /* EAGAIN */
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <crypt.h>
#include <sys/time.h>

/*
#include <sys/un.h>
#include <strings.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <setjmp.h>
*/

#include "colorbox.h"

typedef struct {
	FL_FORM *NewsFleX;
	FL_FORM *setup;
	FL_FORM *desc;
	FL_FORM *error_log_form;
	FL_FORM *search;
	FL_FORM *filter;
/*	FL_FORM *secret;*/
	FL_FORM *question;
	FL_FORM *custom_headers_editor;
	FL_FORM *posting_editor;
	FL_FORM *summary_editor;
	FL_FORM *article_header_form;
	FL_FORM *posting_header_form;
	FL_FORM *coding_form;
	FL_FORM *url_form;
	FL_FORM *newsserver_form;
	FL_FORM *posting_period_form;
	FL_FORM *highlight;
	FL_FORM *helpers_form;
	FL_FORM *incoming_mail_form;
				
	FL_OBJECT *view_article_header_button;
	FL_OBJECT *groups_menu;
	FL_OBJECT *article_menu;
	FL_OBJECT *posting_menu;
	FL_OBJECT *options_menu;
	FL_OBJECT *online_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *window_menu;
	FL_OBJECT *group_list_button;
	FL_OBJECT *article_list_button;
	FL_OBJECT *article_body_button;
	FL_OBJECT *next_thread_button;
	FL_OBJECT *next_unread_button;
	FL_OBJECT *browser_fontsize_up_button;
	FL_OBJECT *browser_fontsize_down_button;
	FL_OBJECT *html_decode_button;
	FL_OBJECT *launche_button;
	FL_OBJECT *stop_button;
	FL_OBJECT *command_status_display;
	FL_OBJECT *line_status_display;
	FL_OBJECT *group_list_browser;
	FL_OBJECT *article_list_browser;
	FL_OBJECT *article_body_input_field;
	FL_OBJECT *posting_list_browser;
	FL_OBJECT *line_speed_display;
	FL_OBJECT *description_display;
	FL_OBJECT *mail_display;
	FL_OBJECT *width_button;
	FL_OBJECT *width_button_text;
	
	FL_OBJECT *setup_form_title;
	FL_OBJECT *setup_accept_button;
	FL_OBJECT *setup_cancel_button;
	FL_OBJECT *setup_form_fontsize_up_button;
	FL_OBJECT *setup_form_fontsize_down_button;
	FL_OBJECT *setup_form_insert_headers_button;
	FL_OBJECT *setup_form_insert_headers_text;
	FL_OBJECT *setup_form_get_by_message_id_button;
	FL_OBJECT *setup_form_get_by_message_id_button_text;
	FL_OBJECT *setup_form_show_only_once_button;
	FL_OBJECT *setup_form_show_only_once_button_text;
	FL_OBJECT *setup_form_check_local_mail_button;
	FL_OBJECT *setup_form_check_local_mail_button_text;
	FL_OBJECT *setup_form_local_mail_check_interval_input_field;
	FL_OBJECT *setup_form_local_mail_check_interval_input_field_text;
	FL_OBJECT *setup_form_do_not_use_sendmail_button;
	FL_OBJECT *setup_form_do_not_use_sendmail_button_text;
	FL_OBJECT *setup_form_posting_wrap_chars_input_field;
	FL_OBJECT *setup_form_posting_wrap_chars_input_text;
	FL_OBJECT *setup_form_lines_first_button;
	FL_OBJECT *setup_form_lines_first_button_text;
	FL_OBJECT *setup_form_cut_off_sig_in_reply_button;
	FL_OBJECT *setup_form_cut_off_sig_in_reply_button_text;
	FL_OBJECT *setup_form_beep_on_mail_button;
	FL_OBJECT *setup_form_beep_on_mail_button_text;
	FL_OBJECT *desc_button;
	FL_OBJECT *news_server_input_field;
	FL_OBJECT *mail_server_input_field;
	FL_OBJECT *news_server_port_input_field;
	FL_OBJECT *mail_server_port_input_field;
	FL_OBJECT *real_name_input_field;
	FL_OBJECT *user_email_input_field;
	FL_OBJECT *maximum_headers_per_group_input_field;
	FL_OBJECT *connect_to_news_server_timeout_input_field;
	FL_OBJECT *connect_to_mail_server_timeout_input_field;
	FL_OBJECT *tab_size_input_field;
	FL_OBJECT *auto_connect_to_net_button;
	FL_OBJECT *auto_connect_to_net_input_field;
	FL_OBJECT *auto_disconnect_from_net_button;
	FL_OBJECT *auto_disconnect_from_net_input_field;
	FL_OBJECT *auto_execute_application_button;
	FL_OBJECT *auto_execute_application_input_field;
	FL_OBJECT *pid_test_input_field;
	FL_OBJECT *server_username_input_field;
	FL_OBJECT *server_password_input_field;
	FL_OBJECT *get_new_headers_time_input_field;
	FL_OBJECT *get_new_headers_enable_button;
	FL_OBJECT *setup_form_posting_source_button;
	FL_OBJECT *setup_form_posting_source_input_field;

	FL_OBJECT *helpers_form_title;
	FL_OBJECT *helpers_form_input;
	FL_OBJECT *helpers_form_fontsize_up_button;
	FL_OBJECT *helpers_form_fontsize_down_button;
	FL_OBJECT *helpers_form_cancel_button;
	FL_OBJECT *helpers_form_accept_button;

	FL_OBJECT *desc_button_text;
	FL_OBJECT *desc_input;/*only used as output, can paste from here (urls)*/
	FL_OBJECT *desc_cancel_button;
	FL_OBJECT *desc_accept_button;
	FL_OBJECT *desc_input_fontsize_up_button;
	FL_OBJECT *desc_input_fontsize_down_button;
	FL_OBJECT *desc_form_title;

	FL_OBJECT *error_log_form_form_title;
	FL_OBJECT *error_log_form_input;/*only used as output */
	FL_OBJECT *error_log_form_clear_button;
	FL_OBJECT *error_log_form_cancel_button;
	FL_OBJECT *error_log_form_accept_button;
	FL_OBJECT *error_log_form_fontsize_up_button;
	FL_OBJECT *error_log_form_fontsize_down_button;

	FL_OBJECT *summary_editor_form_title;
	FL_OBJECT *summary_editor_input_field;
	FL_OBJECT *summary_editor_insert_file_button;
	FL_OBJECT *summary_editor_write_file_button;
	FL_OBJECT *summary_editor_clear_button;
	FL_OBJECT *summary_editor_fontsize_up_button;
	FL_OBJECT *summary_editor_fontsize_down_button;
	FL_OBJECT *summary_editor_cancel_button;
	FL_OBJECT *summary_editor_accept_button;

	FL_OBJECT *coding_form_title;
	FL_OBJECT *coding_form_input_field;
	FL_OBJECT *coding_form_fontsize_up_button;
	FL_OBJECT *coding_form_fontsize_down_button;
	FL_OBJECT *coding_form_cancel_button;
	
	FL_OBJECT *posting_header_form_title;
	FL_OBJECT *posting_header_form_input_field;
	FL_OBJECT *posting_header_form_fontsize_up_button;
	FL_OBJECT *posting_header_form_fontsize_down_button;
	FL_OBJECT *posting_header_form_cancel_button;
	
	FL_OBJECT *article_header_form_title;
	FL_OBJECT *article_header_form_input_field;
	FL_OBJECT *article_header_form_fontsize_up_button;
	FL_OBJECT *article_header_form_fontsize_down_button;
	FL_OBJECT *article_header_form_cancel_button;
	
	FL_OBJECT *posting_form_title;
	FL_OBJECT *posting_coding_menu;
	FL_OBJECT *posting_send_now_button;
	FL_OBJECT *posting_send_later_button;
	FL_OBJECT *posting_attach_file_button;
	FL_OBJECT *posting_cancel_button;
	FL_OBJECT *posting_insert_file_button;
	FL_OBJECT *posting_cross_post_button;
	FL_OBJECT *posting_write_file_button;
	FL_OBJECT *posting_editor_signature_button;
	FL_OBJECT *posting_editor_clear_button;
	FL_OBJECT *posting_summary_button;
	FL_OBJECT *posting_add_url_button;
	FL_OBJECT *posting_alt_editor_button;
	FL_OBJECT *posting_alt_editor_command_input_field;
	FL_OBJECT *posting_custom_headers_button;
	FL_OBJECT *posting_to_input_field;
	FL_OBJECT *posting_subject_input_field;
	FL_OBJECT *posting_attachment_input_field;
	FL_OBJECT *posting_body_editor;

	FL_OBJECT *posting_editor_color_box;

	FL_OBJECT *posting_editor_fontsize_up_button;
	FL_OBJECT *posting_editor_fontsize_down_button;

	FL_OBJECT *search_form_title;
	FL_OBJECT *search_postings_button;
	FL_OBJECT *search_articles_button;
	FL_OBJECT *search_groups_input_field;
	FL_OBJECT *search_headers_button;
	FL_OBJECT *search_headers_from_button;
	FL_OBJECT *search_headers_to_button;
	FL_OBJECT *search_headers_subject_button;
	FL_OBJECT *search_headers_references_button;
	FL_OBJECT *search_headers_others_button;
	FL_OBJECT *search_bodies_button;
	FL_OBJECT *search_attachments_button;
	FL_OBJECT *search_case_sensitive_button;
	FL_OBJECT *search_keywords_input_field;
	FL_OBJECT *search_accept_button;
	FL_OBJECT *search_cancel_button;
	FL_OBJECT *search_fontsize_up_button;
	FL_OBJECT *search_fontsize_down_button;
	
	FL_OBJECT *filter_form_title;
	FL_OBJECT *filter_password_input_field;
	FL_OBJECT *filter_enable_button;	
	FL_OBJECT *filter_postings_button;
	FL_OBJECT *filter_articles_button;
	FL_OBJECT *filter_groups_input_field;
	FL_OBJECT *filter_headers_button;
	FL_OBJECT *filter_headers_from_button;
	FL_OBJECT *filter_headers_to_button;
	FL_OBJECT *filter_headers_subject_button;
	FL_OBJECT *filter_headers_newsgroups_button;
	FL_OBJECT *filter_headers_others_button;
	FL_OBJECT *filter_bodies_button;
	FL_OBJECT *filter_groups_button;
	FL_OBJECT *filter_list_browser;
	FL_OBJECT *filter_list_browser_delete_button;
	FL_OBJECT *filter_list_browser_new_button;
	FL_OBJECT *filter_attachments_button;
	FL_OBJECT *filter_keywords_input_field;
	FL_OBJECT *filter_case_sensitive_button;
	FL_OBJECT *filter_keywords_and_button;
	FL_OBJECT *filter_keywords_not_button;
	FL_OBJECT *filter_keywords_only_button;
	FL_OBJECT *filter_allow_button;
	FL_OBJECT *filter_form_no_text;
	FL_OBJECT *filter_headers_show_button;
	FL_OBJECT *filter_bodies_show_button;
	FL_OBJECT *filter_attachments_show_button;
	FL_OBJECT *filter_groups_show_button;
	FL_OBJECT *filter_form_fontsize_up_button;
	FL_OBJECT *filter_form_fontsize_down_button;
	FL_OBJECT *filter_accept_button;
	FL_OBJECT *filter_cancel_button;

	FL_OBJECT *question_form_title;
	FL_OBJECT *question_text;
	FL_OBJECT *question_yes_button;
	FL_OBJECT *question_no_button;

	FL_OBJECT *custom_headers_form_title;
	FL_OBJECT *custom_headers_editor_accept_button;
	FL_OBJECT *custom_headers_editor_cancel_button;
	FL_OBJECT *custom_headers_editor_fontsize_up_button;
	FL_OBJECT *custom_headers_editor_fontsize_down_button;
	FL_OBJECT *global_custom_headers_editor_input_field;
	FL_OBJECT *local_custom_headers_editor_input_field;
	FL_OBJECT *global_custom_headers_enable_button;
	FL_OBJECT *local_custom_headers_enable_button;

/*
	FL_OBJECT *secret_form_title;
	FL_OBJECT *secret_password_input_field;
	FL_OBJECT *secret_accept_button;
	FL_OBJECT *secret_cancel_button;
*/	
	FL_OBJECT *url_form_title;
	FL_OBJECT *url_form_browser;
	FL_OBJECT *url_form_input_field;
	FL_OBJECT *url_form_delete_button;
	FL_OBJECT *url_form_clear_button;
	FL_OBJECT *url_form_add_button;
	FL_OBJECT *url_form_cancel_button;
	FL_OBJECT *url_form_accept_button;
	FL_OBJECT *url_form_fontsize_up_button;
	FL_OBJECT *url_form_fontsize_down_button;
	FL_OBJECT *url_form_browser_command_input_field;
	FL_OBJECT *url_form_browse_online_button;
	FL_OBJECT *url_form_get_daily_button;
	FL_OBJECT *url_form_get_pictures_button;
	FL_OBJECT *url_form_depth_input_field;
	FL_OBJECT *url_form_host_only_button;
	FL_OBJECT *url_form_external_browser_button;
	FL_OBJECT *url_form_search_input_field;
	FL_OBJECT *url_form_search_case_button;
	FL_OBJECT *url_form_search_button;
	FL_OBJECT *url_form_show_marked_only_button;
	
	FL_OBJECT *highlight_form_title;
	FL_OBJECT *highlight_postings_button;
	FL_OBJECT *highlight_articles_button;
	FL_OBJECT *highlight_headers_button;
	FL_OBJECT *highlight_headers_from_button;
	FL_OBJECT *highlight_headers_to_button;
	FL_OBJECT *highlight_headers_subject_button;
	FL_OBJECT *highlight_headers_references_button;
	FL_OBJECT *highlight_headers_others_button;
	FL_OBJECT *highlight_bodies_button;
	FL_OBJECT *highlight_attachments_button;
	FL_OBJECT *highlight_case_sensitive_button;
	FL_OBJECT *highlight_keywords_input_field;
	FL_OBJECT *highlight_accept_button;
	FL_OBJECT *highlight_cancel_button;
	FL_OBJECT *highlight_fontsize_up_button;
	FL_OBJECT *highlight_fontsize_down_button;
	FL_OBJECT *highlight_urls_button;
	FL_OBJECT *highlight_urls_url_button;
	FL_OBJECT *highlight_urls_description_button;

    FL_OBJECT *incoming_mail_form_title;
    FL_OBJECT *incoming_mail_form_browser;
    FL_OBJECT *incoming_mail_form_mark_read_button;
    FL_OBJECT *incoming_mail_form_cancel_button;
    FL_OBJECT *incoming_mail_form_accept_button;
    FL_OBJECT *incoming_mail_form_fontsize_down_button;
    FL_OBJECT *incoming_mail_form_fontsize_up_button;

	FL_OBJECT *newsserver_form_title;
	FL_OBJECT *newsserver_form_browser;
	FL_OBJECT *newsserver_form_input_field;
	FL_OBJECT *newsserver_form_delete_button;
	FL_OBJECT *newsserver_form_add_button;
	FL_OBJECT *newsserver_form_fontsize_up_button;
	FL_OBJECT *newsserver_form_fontsize_down_button;
    FL_OBJECT *newsserver_form_cancel_button;
    FL_OBJECT *newsserver_form_accept_button;

	FL_OBJECT *posting_period_form_title;
	FL_OBJECT *posting_period_form_browser;
	FL_OBJECT *posting_period_form_input_field;
	FL_OBJECT *posting_period_form_delete_button;
	FL_OBJECT *posting_period_form_add_button;
	FL_OBJECT *posting_period_form_fontsize_up_button;
	FL_OBJECT *posting_period_form_fontsize_down_button;
    FL_OBJECT *posting_period_form_cancel_button;
    FL_OBJECT *posting_period_form_accept_button;

	void *vdata;
	long ldata;
} FD_NewsFleX;

extern FD_NewsFleX *create_form_NewsFleX(void);
extern FD_NewsFleX *create_form_setup(void);
extern FD_NewsFleX *create_form_desc(void);
extern FD_NewsFleX *create_form_error_log_form(void);
extern FD_NewsFleX *create_form_search(void);
extern FD_NewsFleX *create_form_filter(void);
/*extern FD_NewsFleX *create_form_secret(void);*/
extern FD_NewsFleX *create_form_question(void);
extern FD_NewsFleX *create_form_custom_headers_editor(void);
extern FD_NewsFleX *create_form_posting_editor(void);
extern FD_NewsFleX *create_form_summary_editor(void);
extern FD_NewsFleX *create_form_article_header_form(void);
extern FD_NewsFleX *create_form_posting_header_form(void);
extern FD_NewsFleX *create_form_coding_form(void);
extern FD_NewsFleX *create_form_url_form(void);
extern FD_NewsFleX *create_form_highlight(void);
extern FD_NewsFleX *create_form_helpers_form(void);
extern FD_NewsFleX *create_form_incoming_mail_form(void);
extern FD_NewsFleX *create_form_newsserver_form(void);
extern FD_NewsFleX *create_form_posting_period_form(void);

/* for postings */
#define LINE_LENGTH 128

/* commands */
#define NONE						0
#define LIST						1
#define GET_NEW_HEADERS_IN_GROUP	2
#define GET_ARTICLE_BODY			3
#define POST_ARTICLE				4
#define EMAIL_ARTICLE				5
#define WAIT_RESPONSE				6
#define URL_GET						7 /* http, ftp */

/* posting source */
#define POSTING_MENU	1
#define POSTING_LIST	2
#define ONLINE_MENU		3

/* file menu */
#define EXIT						1

/* news server list status */
#define NEWSSERVER_NOT_SELECTED		0
#define NEWSSERVER_SELECTED			1
#define NEWSSERVER_HIDDEN			2

/* online menu */
#define GET_NEW_HEADERS_IN_SELECTED_GROUPS		1
#define GET_NEW_HEADERS_IN_SUBSCRIBED_GROUPS	2
#define GET_MARKED_ARTICLE_BODIES				3
#define GET_MARKED_URLS							4
#define REFRESH_GROUP_LIST						5
#define POST_SELECTED_POSTINGS					6
#define POST_MARKED_POSTINGS					7

/* groups menu */
#define SUBSCRIBE											1
#define UNSUBSCRIBE											2
#define SHOW_ALL_GROUPS										3
#define SHOW_ONLY_SUBSCRIBED_GROUPS							4
#define SHOW_ONLY_NEW_GROUPS								5
#define DELETE_GROUP										6
#define DELETE_ALL_NOT_LOCKED_ARTICLES_IN_SUBSCRIBED_GROUPS	7
#define MARK_ALL_GROUPS_READ								8
#define SET_DISPLAY_FILTER									9
#define SET_MAXIMUM_HEADERS									10
#define ALWAYS_GET_ALL_ARTICLES_IN_GROUP					11
#define ONLY_GET_MARKED_ARTICLES_IN_GROUP					12
#define SORTED_DISPLAY										13

/* group status */
#define CANCELLED_GROUP		1
#define NEW_GROUP			2
#define ACTIVE_GROUP		3

/* article menu */
#define MARK_ARTICLE_FOR_RETRIEVAL				1
#define UNMARK_ARTICLE_FOR_RETRIEVAL			2
#define FOLLOW_SUBJECT							3
#define DO_NOT_FOLLOW_SUBJECT					4
#define FOLLOW_THREAD							5
#define DO_NOT_FOLLOW_THREAD					6
#define LOCK_ARTICLE							7
#define UNLOCK_ARTICLE							8
#define DELETE_ARTICLE							9
#define DELETE_ALL_NOT_LOCKED_ARTICLES			10
#define MARK_ARTICLE_READ						11
#define MARK_ARTICLE_UNREAD						12
#define MARK_ALL_NEW_ARTICLES_FOR_RETRIEVAL		13
#define UNMARK_ALL_NEW_ARTICLES_FOR_RETRIEVAL	14
#define MARK_ALL_ARTICLES_READ					15
#define MARK_ALL_ARTICLES_UNREAD				16
#define SEARCH_FOR_KEYWORD						17
#define EXPORT_ARTICLE_PLUS_HEADER				18
#define EXPORT_ARTICLE_ONLY						19	
#define PGP_DECRYPT								20
#define PGP_CHECK_SIGNATURE_AND_DECRYPT			21
#define OTP_DECRYPT								22
#define CANCEL_ARTICLE							23

/* posting menu */
#define NEW_ARTICLE					1
#define FOLLOW_UP_ARTICLE			2
#define REPLY_VIA_EMAIL				3
#define MARK_FOR_TRANSMIT			4
#define UNMARK_FOR_TRANSMIT			5
#define LOCK_POSTING				6
#define UNLOCK_POSTING				7
#define MARK_SEND					8
#define UNMARK_SEND					9
#define DELETE_SELECTED_POSTINGS	10

/* options menu */
#define SETUP								1
#define HELPERS								2
#define LOCAL_SEARCH						3
#define HIGHLIGHT							4
#define	FILTER								5
/*#define SECRET_AGENT						6*/
#define REGENERATE_SUBSCRIBED_GROUPS_DAT	6
#define REGENERATE_ARTICLES_DAT				7
#define REGENERATE_POSTINGS_DAT				8
#define XAGENT_INFO							9
#define DEBUG_MODE							10

/* window menu */
#define GROUP_LIST_WINDOW			1
#define ARTICLE_LIST_WINDOW			2
#define ARTICLE_BODY_WINDOW			3
#define POSTING_LIST_WINDOW			4
#define URL_LIST_WINDOW				5
#define INCOMING_MAIL_WINDOW		6
#define NEWSSERVER_WINDOW			7
#define POSTING_PERIOD_WINDOW		8
#define ERROR_LOG_WINDOW			9
/*#define POSTING_BODY_WINDOW		*/

/* visible browser */
#define GROUP_LIST_BROWSER			1
#define ARTICLE_LIST_BROWSER		2
#define ARTICLE_BODY_BROWSER		3
#define POSTING_LIST_BROWSER		4
#define POSTING_BODY_BROWSER		5

/* helper types */
#define HELPER_TEXT					1
#define HELPER_MULTIPART			2
#define HELPER_APPLICATION			3
#define HELPER_MESSAGE				4
#define HELPER_IMAGE				5
#define HELPER_AUDIO				6
#define HELPER_VIDEO				7

/* posting status */
#define TO_BE_SEND		1
#define SEND_LATER		2
#define SEND_NOW		3
#define SEND_OK			5
#define SEND_FAILED		6

/* news, mail, http, ftp, ftp data  server status and net status */
#define DISCONNECTED		1
#define CONNECTED			2
#define READY				3
#define UNKNOWN				4

/*server mode */
#define POSTING_ALLOWED		1
#define NO_POSTING_ALLOWED	2
#define OUT_OF_ORDER		3

/* load_subscribe_groups mode */
#define USE_ALL_DATA		1
#define MERGE_DATA			2

/* for search */
#define SEARCH_GROUPS						1
#define SEARCH_POSTINGS						2
#define	SEARCH_ARTICLES						3
#define SEARCH_MATCH_IN_HEADER_FROM			4
#define SEARCH_MATCH_IN_HEADER_TO			5
#define SEARCH_MATCH_IN_HEADER_SUBJECT		6
#define SEARCH_MATCH_IN_HEADER_REFERENCES	7
#define SEARCH_MATCH_IN_HEADER_OTHERS		8
#define SEARCH_MATCH_IN_BODY				9
#define SEARCH_MATCH_IN_ATTACHMENT			10
#define SEARCH_MATCH_IN_GROUP				11

/* for custom headers */
#define NO_CUSTOM_HEADERS			0
#define GLOBAL_HEADERS_ENABLED		1
#define LOCAL_HEADERS_ENABLED		2
#define GLOBAL_HEADERS_MODIFIED		4
#define LOCAL_HEADERS_MODIFIED		8

/* for follow */
#define FOLLOW_THIS_SUBJECT		1
#define FOLLOW_THIS_THREAD		2

/* for filters */
#define FILTER_HEADER_LINE			1
#define FILTER_BODY_LINE			2
#define FILTER_ATTACHMENT_LINE		3
#define HEADER_FILE					4
#define BODY_FILE					5
#define ATTACHMENT_FILE				6

#define FILTER_MATCH_IN_NONE		0
#define FILTER_MATCH_IN_HEADER 		1
#define FILTER_MATCH_IN_BODY 		2
#define FILTER_MATCH_IN_ATTACHMENT	4
#define FILTER_MATCH_IN_GROUP		8

#define FILTER_OFF							0
#define FILTER_DO_NOT_SHOW_GROUP			1
#define FILTER_DO_NOT_SHOW_HEADER			2
#define FILTER_DO_NOT_SHOW_BODY				4
#define FILTER_DO_NOT_SHOW_ATTACHMENT		8

/* for article coding */
#define ATTACHMENT_PRESENT			1
#define DECODED_PRESENT				2

/* for posting coding */
#define PGP_ENCRYPT					1
#define PGP_CLEAR_SIGN				2
#define PGP_APPEND_PUBLIC_KEY		3
#define OTP_ENCRYPT					4
#define OTP_CREATE_RANDOM_KEY_FILE	5		
#define SHOW_UNENCRYPTED			6
#define SEARCH_KEYWORD				7
#define WORD_WRAP					8

/* for posting encoding */
#define NONE				0
#define PGP_ENCRYPTED		1
#define PGP_SIGNED			2
#define OTP_ENCRYPTED		4

/* for program */
#define MAX_POSTINGS	10000
#define MAX_FILTERS		1000
#define READSIZE		65535
#define TEMP_SIZE		65535

/* for urls */
/* valuses above 255 are depth, i.e. depth << 8) */
#define NEW_URL			0
#define ACCESSED_URL	1
#define FAILED_URL		2
#define RETRIEVE_URL	4
#define HAVE_URL		8
#define DAILY_URL		16
#define HOST_ONLY_URL	32
#define PICTURES_URL	64
#define KEEP_ALIVE_URL	128

/* for html parsing */
#define HTTP_ADD_LINKS_TO_COMMAND_QUEUE	1
#define HTTP_DISPLAY_FULL				2
#define HTTP_DISPLAY_TEXT				4

/* for position browser display */
#define SHOW_TOP			0
#define SHOW_SAME_POSITION	1
#define SHOW_UNREAD_AT_TOP	2
#define SHOW_BOTTOM			3

/* warning levels */
/* for posting attachments and inserting files in postings */
#define WARNING_FILESIZE		80000
#define WARNING_CROSS_POSTS		10

/* for http */
#define HTTP_OFF		0
#define HTTP_POST		1
#define HTTP_PUT		2
#define HTTP_GET		3

/* for ftp command */
#define FTP_OFF			0
#define FTP_SEND		2
#define FTP_RECEIVE		3

/* for ftp mode */
#define FTP_CONNECT		1
#define FTP_USER		2
#define FTP_PASS		3
#define FTP_TYPE		4
#define FTP_PASV		5
#define FTP_RETR		6
#define FTP_QUIT		7

/* for http browser */
#define TABLE_MAX_COLUMNS 1024
#define TABLE_HEADER		1
#define TABLE_DATA			2
#define HOR_CHARS			70

int visible_browser;

int file_menu_selection;
int groups_menu_selection;
int groups_show_selection;
int articles_menu_selection;
int online_menu_selection;
int posting_menu_selection;
int options_menu_selection;
int window_menu_selection;

char *selected_group;
long selected_article;
int group_double_clicked_flag;
char *acquire_group;
long acquire_article;
long acquire_lines;
char *last_acquire_group;

long selected_posting;
long posting_article;
long body_posting;
char *home_dir;
FILE *setupfile;

long selected_filter;

int process_command_queue_error_flag;

char *news_server_name;
char *mail_server_name;
char *user_email_address;
char *user_name;
char *real_name;
int news_server_port;
int mail_server_port;
int maximum_headers;

int group_list_browser_fontsize;
int article_list_browser_fontsize;
int article_body_browser_fontsize;
int posting_list_browser_fontsize;
int posting_body_editor_fontsize;
int setup_form_fontsize;
int helpers_form_fontsize;
int filter_form_fontsize;
int desc_input_fontsize;
int error_log_form_fontsize;
int summary_editor_fontsize;
int custom_headers_editor_fontsize;
int article_header_form_fontsize;
int posting_header_form_fontsize;
int coding_form_fontsize;
int url_form_fontsize;
int highlight_fontsize;
int search_fontsize;
int incoming_mail_form_fontsize;

int socketfd;
int command_in_progress;
int expect_numeric_flag;
int news_server_status;
int news_server_mode;
int posting_modified_flag;
int posting_source;
int connect_to_news_server_timeout;
int connect_to_mail_server_timeout;
int enable_description_popup_flag;
int auto_connect_to_net_flag;
int auto_disconnect_from_net_flag;
char *connect_to_net_program_name;
char *disconnect_from_net_program_name;
char *auto_execute_program_name;
int auto_execute_program_flag;
char *pid_test_program_name;
char *get_new_headers_time;
int auto_get_new_headers_flag;
int time_match_flag;
int new_thread_bodies_flag;

/*
 just trying to get around xforms bugs Sat Mar  1 15:06:27 GMT-0100 1997
 calling fl_set_object label and fl_show alert crashes the system on
 many occasions, now moving those calls to idle_cb.

 Sun Aug 31 21:58:08 GMT-0100 1997
 man the things I have to do to get xforms to work,
 Sometimes forms simply do not show, even if they have to.
 Moving fl_show_form() to idle callback shows it sooner or later.
*/

char command_status_string[512];
char line_status_string[512];
char description_string[512];
char alert_string1[512];
char alert_string2[512];
char alert_string3[512];
char question_string[512];

int command_status_string_flag;
int line_status_string_flag;
int description_string_flag;
int alert_string_flag;
int ask_question_flag;
char *groups_display_filter;

unsigned long bytes_received;
unsigned long bytes_send;

time_t online_timer;
time_t start_time;

int audio_alarm_flag;

int highlight_postings_flag;
int highlight_articles_flag;
int highlight_headers_flag;
int highlight_headers_from_flag;
int highlight_headers_to_flag;
int highlight_headers_subject_flag;
int highlight_headers_references_flag;
int highlight_headers_others_flag;
int highlight_case_sensitive_flag;
int highlight_urls_flag;
int highlight_urls_url_flag;
int highlight_urls_description_flag;
char *highlight_keywords;

int search_postings_flag;
int search_articles_flag;
int search_headers_flag;
int search_headers_from_flag;
int search_headers_to_flag;
int search_headers_subject_flag;
int search_headers_references_flag;
int search_headers_others_flag;
int search_bodies_flag;
int search_attachments_flag;
int search_case_sensitive_flag;
char *search_keywords;
char *search_groups_keywords;

char *filter_password;
int filter_modified_flag;
int filter_enable_modified_flag;

char start_group[512];
long start_article;
char start_unread_group[512];
long start_unread_article;
int expired_flag;
long start_posting;
int search_last_search;
int go_search_next_article_from_idle_cb_flag;
long first_matching_posting;
long first_matching_article;
char *first_matching_group;
char *first_matching_newsserver_and_database;
char *summary_input;
int headers_modified_flag;
int custom_headers_state;
int filters_enabled_flag;
int groups_loaded_flag;
int tab_size;
int posting_encoded_flag;
char *unencoded_data;
char *selected_url;
char *article_body_copy;
int article_body_topline;
char *browser_get_url_command;
int unread_article_at_top_flag;
int variable_width_flag;
char *alt_editor_command;
FILE *error_log_fileptr;
int error_log_present_flag;
int net_status;
int insert_headers_offline_flag;
int debug_flag;
int acquire_body_count;
int request_by_message_id_flag;
int show_article_once_flag;
int do_not_use_sendmail_flag;
long mail_posting;
int mail_server_status;
int mail_socketfd;
int reply_to_incoming_mail_flag;
char *database_name;
char *postings_database_name;
int newsserver_form_fontsize;
int posting_period_form_fontsize;
char *access_code;
char *server_username;
char *server_password;
int show_space_flag;
int sort_flag;
int lines_first_flag;
char *last_deleted_group;
int check_incoming_mail_periodically_flag;
int local_mail_check_interval;
char *posting_body_copy;
char *desc_input_copy;
int registered_flag;
char *url_in_progress;

int connect_to_http_server_timeout;
int http_server_status;
int http_socketfd;
int http_command;
FILE *http_read_fileptr;
int http_first_line_flag;
int http_first_byte_flag;
char *http_local_path_filename;
char *http_host_name;
long http_bytes_received;
int http_first_byte_flag;
long http_file_size;
float http_percentage_received;
float http_version;

int connect_to_ftp_server_timeout;
int ftp_control_server_status;
int ftp_data_server_status;

int ftp_control_socketfd;
int ftp_data_socketfd;
int ftp_command;
int ftp_mode;
FILE *ftp_read_fileptr;
/*FILE *ftp_write_fileptr;*/
char *ftp_local_path_filename;
char *ftp_remote_path_filename;
char *ftp_user;
char *ftp_password;
int ftp_control_port;
int ftp_data_port;
long ftp_bytes_received;
long ftp_file_size;
float ftp_percentage_received;
int ftp_failed_flag;
int external_browser_flag;
int browse_online_flag;
int posting_word_wrap_flag;

struct urldata
	{
	char *scheme;
	char *user;
	char *password;
	char *host;
	char *url_path;
	int port;
	char *localpathfilename;
	char *localpath;
	char *localfilename;
	};
	
int url_flags;
int html_file_shown;
char *reload_url;
int html_decode_flag;
char *url_current_host;
char *url_initial_host;

char *start_newsserver_and_database;
char *start_postings_database_name;
char *first_matching_postings_database_name;

int url_form_show_marked_only_flag;
int global_cut_of_sig_in_reply_flag;
int global_beep_on_mail_flag;
int global_posting_wrap_chars;
int global_show_posting_source_flag;
char *global_posting_source_text;

#define VERSION "NewsFleX-1.1.9.5"
#define USER_AGENT "NewsFleX/1.1.9.5 (egcs-2.91.66 Linux-2.2.12)"
#endif /* FD_NewsFleX_h_ */

