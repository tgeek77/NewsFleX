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

#define ARTICLE_BODY_TIMEOUT	30


char *strsave(char *s) /* save char array s somewhere */
{
char *p;

p = malloc(strlen(s) +  1);
if(p) strcpy(p, s);
return(p);
}/* end function strsave */


void desc_input_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "desc_input_cb(): arg none\n");
	}

extract_url_from_input(ob, desc_input_copy, selected_group, selected_article);
}/* end function desc_input_cb */


void desc_cancel_button_cb(FL_OBJECT *ob, long data)
{
char temp[TEMP_SIZE];
FILE *fptr;

fl_hide_form(fd_NewsFleX -> desc);

if(show_space_flag)
	{
	show_space_flag = 0;
	fl_hide_object(fdui -> desc_accept_button);

	sprintf(temp, "%s/.NewsFleX/setup/.NewsFleXf", home_dir);
	unlink(temp);
	fptr = fopen(temp, "w");
	if(! fptr) exit(1);
	fprintf(fptr, "z");
	fclose(fptr);
	exit(1);
	}
}/* end function desc_cancel_button_cb */


void desc_accept_button_cb(FL_OBJECT *ob, long data)
{
fl_hide_form(fd_NewsFleX -> desc);

show_space_flag = 0;
fl_hide_object(fdui -> desc_accept_button);
}/* end function desc_accept_button_cb */


void groups_menu_cb(FL_OBJECT *ob, long data)
{
int a, i;
char temp[TEMP_SIZE], _dummy[TEMP_SIZE];
FILE *exec_filefd;
int total_lines;
char s_group[TEMP_SIZE];
char *ptr;
int something_selected_flag;
int max_headers;
char s_server[TEMP_SIZE];
char s_period[TEMP_SIZE];
int copy_server_flag;
FILE *copy_fptr;
int permission_denied;

copy_fptr = 0;/* keep gcc -Wall happy */

groups_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(groups_menu_selection <= 0) return;

if(groups_menu_selection == SORTED_DISPLAY)
	{
	sort_flag = 1 - sort_flag;
	if(sort_flag)
	 	{
		fl_set_menu_item_mode(\
		fdui -> groups_menu, SORTED_DISPLAY, FL_PUP_CHECK); 	
	 	}
	 else
	 	{
	 	fl_set_menu_item_mode(\
	 	fdui -> groups_menu, SORTED_DISPLAY, FL_PUP_BOX);
		}
	save_general_settings();
	}

if(groups_menu_selection == ALWAYS_GET_ALL_ARTICLES_IN_GROUP)
	{
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		if(groups_show_selection != SHOW_ONLY_SUBSCRIBED_GROUPS)
			{
			fl_show_alert(\
			"Select SHOW ONLY SUBSCRIBED GROUPS first", "", "", 0);
			return;
			}
		}	

	if(visible_browser != GROUP_LIST_BROWSER)
		{
		sprintf(s_group, "%s", selected_group);
		if(! set_mark_all_flag(s_group, 0) ) return;
		}
	else
		{
		something_selected_flag = 0;
		total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
		for(i = 1; i <= total_lines; i++)
			{
			if (fl_isselected_browser_line(fdui -> group_list_browser, i))
				{
				something_selected_flag = 1;
				strcpy(temp,\
				fl_get_browser_line(fdui -> group_list_browser, i) );
				sscanf(temp, "%s %s", _dummy, s_group);

				if(! set_mark_all_flag(s_group, 1) ) return;
				}/* end if line selected */
			}/* end for all lines */

		if(! something_selected_flag)
			{
			ptr = (char *) fl_show_input("Group?", selected_group);
			if(! ptr) return;/* cancel pressed */
			strcpy(s_group, ptr);
			if(! set_mark_all_flag(s_group, 1) )
				{
				fl_show_alert(\
				"No such subscribed group", s_group, "command cancelled", 0);
				return;
				}
			}

		}/* end else group list browser selected */

	save_subscribed_groups();
	refresh_screen(selected_group);
	return;
	}/* end set maximum headers */

if(groups_menu_selection == ONLY_GET_MARKED_ARTICLES_IN_GROUP)
	{
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		if(groups_show_selection != SHOW_ONLY_SUBSCRIBED_GROUPS)
			{
			fl_show_alert(\
			"Select SHOW ONLY SUBSCRIBED GROUPS first", "", "", 0);
			return;
			}
		}	

	if(visible_browser != GROUP_LIST_BROWSER)
		{
		sprintf(s_group, "%s", selected_group);
		if(! set_mark_all_flag(s_group, 0) ) return;
		}
	else
		{
		something_selected_flag = 0;
		total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
		for(i = 1; i <= total_lines; i++)
			{
			if (fl_isselected_browser_line(fdui -> group_list_browser, i))
				{
				something_selected_flag = 1;
				strcpy(temp,\
				fl_get_browser_line(fdui -> group_list_browser, i) );
				sscanf(temp, "%s %s", _dummy, s_group);

				if(! set_mark_all_flag(s_group, 0) ) return;
				}/* end if line selected */
			}/* end for all lines */

		if(! something_selected_flag)
			{
			ptr = (char *) fl_show_input("Group?", selected_group);
			if(! ptr) return;/* cancel pressed */
			strcpy(s_group, ptr);
			if(! set_mark_all_flag(s_group, 0) )
				{
				fl_show_alert(\
				"No such subscribed group", s_group, "command cancelled", 0);
				return;
				}
			}

		}/* end else group list browser selected */

	save_subscribed_groups();
	refresh_screen(selected_group);
	return;
	}/* end set maximum headers */

if(groups_menu_selection == SET_MAXIMUM_HEADERS)
	{
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		if(groups_show_selection != SHOW_ONLY_SUBSCRIBED_GROUPS)
			{
			fl_show_alert(\
			"Select SHOW ONLY SUBSCRIBED GROUPS first", "", "", 0);
			return;
			}
		}	

	ptr = (char *) fl_show_input(\
	"maximum headers?", "");
	if(! ptr) return;/* cancel pressed */
	max_headers = atoi(ptr);
	if(max_headers < 0) max_headers = 0;
	
	if(visible_browser != GROUP_LIST_BROWSER)
		{
		sprintf(s_group, "%s", selected_group);
		if(! set_maximum_headers(s_group, max_headers) ) return;
		}
	else
		{
		something_selected_flag = 0;
		total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
		for(i = 1; i <= total_lines; i++)
			{
			if (fl_isselected_browser_line(fdui -> group_list_browser, i))
				{
				something_selected_flag = 1;
				strcpy(temp,\
				fl_get_browser_line(fdui -> group_list_browser, i) );
				sscanf(temp, "%s %s", _dummy, s_group);

				if(! set_maximum_headers(s_group, max_headers) ) return;
				}/* end if line selected */
			}/* end for all lines */

		if(! something_selected_flag)
			{
/*
			fl_show_alert("No group(s) selected", "command cancelled", "", 0);
			return;
*/
			ptr = (char *) fl_show_input("Group?", selected_group);
			if(! ptr) return;/* cancel pressed */
			strcpy(s_group, ptr);
			if(! set_maximum_headers(s_group, max_headers) )
				{
				fl_show_alert(\
				"No such subscribed group", s_group, "command cancelled", 0);
				return;
				}
			}

		}/* end else group list browser selected */

	save_subscribed_groups();
	refresh_screen(selected_group);
	return;
	}/* end set maximum headers */

if(groups_menu_selection == SET_DISPLAY_FILTER)
	{
	ptr = (char *) fl_show_input(\
	"Filter? (clear selects all)", groups_display_filter);
	if(! ptr) return;/* cancel pressed */
	free(groups_display_filter);
	groups_display_filter = strsave(ptr);
	}
if(groups_menu_selection == SHOW_ALL_GROUPS)
	{
	load_groups();
	if(filters_enabled_flag)
		{
		set_all_filter_flags_in_groups();
		}
	fl_set_menu_item_mode(ob, SHOW_ALL_GROUPS, FL_PUP_CHECK);
	fl_set_menu_item_mode(ob, SHOW_ONLY_SUBSCRIBED_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(ob, SHOW_ONLY_NEW_GROUPS, FL_PUP_BOX);
	fl_set_object_label(fdui -> description_display, "ALL GROUPS");
	groups_show_selection = SHOW_ALL_GROUPS;
	}
if(groups_menu_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
	{
	fl_set_menu_item_mode(ob, SHOW_ALL_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(ob, SHOW_ONLY_SUBSCRIBED_GROUPS, FL_PUP_CHECK);
	fl_set_menu_item_mode(ob, SHOW_ONLY_NEW_GROUPS, FL_PUP_BOX);
	fl_set_object_label(fdui -> description_display, "SUBSCRIBED GROUPS");
	groups_show_selection = SHOW_ONLY_SUBSCRIBED_GROUPS;
	}
if(groups_menu_selection == SHOW_ONLY_NEW_GROUPS)
	{
	load_groups();
	if(filters_enabled_flag)
		{
		set_all_filter_flags_in_groups();
		}
	fl_set_menu_item_mode(ob, SHOW_ALL_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(ob, SHOW_ONLY_SUBSCRIBED_GROUPS, FL_PUP_BOX);
	fl_set_menu_item_mode(ob, SHOW_ONLY_NEW_GROUPS, FL_PUP_CHECK);
	fl_set_object_label(fdui -> description_display, "NEW GROUPS");
	groups_show_selection = SHOW_ONLY_NEW_GROUPS;
	}
if(groups_menu_selection ==\
DELETE_ALL_NOT_LOCKED_ARTICLES_IN_SUBSCRIBED_GROUPS)
	{
	if(!fl_show_question(\
"This will delete all not locked articles in subscribed groups\n\
unread articles and articles marked for retrieval are not deleted\n\
Are you sure?",\
	0) )/* 0 = mouse on NO */
		{
		return;
		}
	delete_all_not_locked_articles_in_subscribed_groups();
	}
if(groups_menu_selection == SUBSCRIBE)
	{
	something_selected_flag = 0;
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		/*
		if show all groups, subscribe to marked groups, else ask for
		group.
		*/
		if(groups_show_selection == SHOW_ALL_GROUPS)
			{
			total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
			for(i = 1; i <= total_lines; i++)
				{
				if (fl_isselected_browser_line(fdui -> group_list_browser, i))
					{
					something_selected_flag = 1;
					strcpy(temp,\
					fl_get_browser_line(fdui -> group_list_browser, i) );
					sscanf(temp, "%s%s", _dummy, s_group);
					if(! subscribe_group(s_group, 1) ) return;
					sprintf(temp, "mkdir %s/.NewsFleX/%s/%s/%s",\
					home_dir, news_server_name, database_name, s_group);
					exec_filefd = popen(temp, "r");
					pclose(exec_filefd);

					/*
					test if articles.dat exists in this group
					previous subscribed?
					*/					
					sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
					home_dir, news_server_name, database_name, s_group);
					exec_filefd = fopen(temp, "r");
					if(exec_filefd)
						{
						fclose(exec_filefd);
						}/* end if articles dat existed */					
					else /* create empty articles.dat and articles.dat~ */
						{
						/* create empty articles.dat */
						sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
						home_dir, news_server_name, database_name, s_group);
						exec_filefd = fopen(temp, "w");
						if(! exec_filefd)
							{
							if(! subscribe_group(s_group, 0) ) return;
							fl_show_alert("Cannot create file", temp,\
							"command cancelled", 0);
							return;
							}
						fclose(exec_filefd);
	
						/* create empty articles.dat~ */
						sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat~",\
						home_dir, news_server_name, database_name, s_group);
						exec_filefd = fopen(temp, "w");
						if(! exec_filefd)
							{
							if(! subscribe_group(s_group, 0) ) return;
							fl_show_alert("Cannot create file", temp,\
							"command cancelled", 0);
							return;
							}
						fclose(exec_filefd);
						}/* end if articles.dat did not exist yet */
					}/* end if line selected */
				}/* end for all lines in group list browser */
			}/* end if show all groups */
		}/* end visible browser is group list browser */

	if(! something_selected_flag)
		{
		ptr = (char *) fl_show_input("News group?", "");
		if(!ptr) return;/* cancel pressed */

		if(filters_enabled_flag)
			{
			get_filter_flags(\
			ptr, "dummy" , 0, "dummy", 0, SEARCH_GROUPS,\
			&(permission_denied) );
			if(permission_denied)
				{
				fl_show_alert(ptr, "filter", "permission denied", 0);
				return;
				}
			}/* end if filters_enabled_flag */

		load_groups();
		if(strstr(ptr, "<") == ptr)
			{
			copy_server_flag = 1;
			sscanf(ptr, "<%s %s", s_server, s_period);
			}
		else
			{
			copy_server_flag = 0;
			strcpy(s_group, ptr);
			}

		if(copy_server_flag)
			{
			/*
			read s_group and period from subscribed_groups.dat in
			other server.
			*/
			sprintf(temp, "%s/.NewsFleX/%s/%s/subscribed_groups.dat",\
			home_dir, s_server, s_period);
			copy_fptr = fopen(temp, "r");
			if(! copy_fptr)
				{
				fl_show_alert(\
				"No such server period",\
				temp,\
				"command cancelled", 0);
				return;
				}
			}

		while(1)
			{
			if(copy_server_flag)
				{
				a = readline(copy_fptr, temp);
				if(a == EOF) break;
				sscanf(temp, "%s", s_group);
				}
			
			if(! subscribe_group(s_group, 1) )
				{
				if(copy_server_flag) continue;
				else return;
				}
			sprintf(temp, "mkdir %s/.NewsFleX/%s/%s/%s",\
			home_dir, news_server_name, database_name, s_group);
			exec_filefd = popen(temp, "r");
			pclose(exec_filefd);

			/*
			test if articles.dat exists in this group
			previous subscribed?
			*/					
			sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
			home_dir, news_server_name, database_name, s_group);
			exec_filefd = fopen(temp, "r");
			if(exec_filefd)/* check if dat and dat~ are same */
				{
				fclose(exec_filefd);
				}/* end if articles.dat existed */					
			else /* create empty articles.dat and articles.dat~ */
				{
				/* create empty articles.dat */
				sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
				home_dir, news_server_name, database_name, s_group);
				exec_filefd = fopen(temp, "w");
				if(! exec_filefd)
					{
					if(! subscribe_group(s_group, 0) ) return;
					fl_show_alert("Cannot create file", temp,\
					"command cancelled", 0);
					return;
					}
				fclose(exec_filefd);
	
				/* create empty articles.dat~ */
				sprintf(temp, "%s/.NewsFleX/%s/%s/%s/articles.dat~",\
				home_dir, news_server_name, database_name, s_group);
				exec_filefd = fopen(temp, "w");
				if(! exec_filefd)
					{
					if(! subscribe_group(s_group, 0) ) return;
					fl_show_alert("Cannot create file", temp,\
					"command cancelled", 0);
					return;
					}
				fclose(exec_filefd);
				}/* end if articles.dat did not exist yet */
			if(! copy_server_flag) break;	
			}/* end while all lines from subscribed_groups.dat in other server */
		}/* end if ! something_selected_flag */
	save_subscribed_groups();
	}/* end subscribe groups */
if(groups_menu_selection == UNSUBSCRIBE)
	{
	something_selected_flag = 0;
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
		for(i = 1; i <= total_lines; i++)
			{
			if (fl_isselected_browser_line(fdui -> group_list_browser, i))
				{
				something_selected_flag = 1;
				strcpy(temp,\
				fl_get_browser_line(fdui -> group_list_browser, i) );
				sscanf(temp, "%s %s", _dummy, s_group);
				if(! subscribe_group(s_group, 0) ) return;
				}/* end if line selected */
			}/* end for all lines in group list browser */
		}/* end visible browser is group list browser */

	if(! something_selected_flag)
		{
		ptr = (char *) fl_show_input("Unsubscribe group?", "");
		if(! ptr) return;/* cancel pressed */
		strcpy(s_group, ptr);
		if(! subscribe_group(s_group, 0) ) return;
		}
	save_subscribed_groups();

	/*
	To prevent hanging if now 'next unread' pressed,
	select something.
	*/
	select_first_group_and_article();
	}/* end unsubscribe groups */
if(groups_menu_selection == DELETE_GROUP)
	{
	something_selected_flag = 0;	
	if(visible_browser == GROUP_LIST_BROWSER)
		{
		total_lines = fl_get_browser_maxline(fdui -> group_list_browser);
		for(i = 1; i <= total_lines; i++)
			{
			if (fl_isselected_browser_line(fdui -> group_list_browser, i))
				{
				something_selected_flag = 1;
				strcpy(temp,\
				fl_get_browser_line(fdui -> group_list_browser, i) );
				sscanf(temp, "%s %s", _dummy, s_group);
					
				completely_delete_group(s_group);
				}/* end if line selected */
			}/* end for all lines in group list browser */
		}/* end visible browser is group list browser */
	
	if(! something_selected_flag)
		{
		ptr = (char *) fl_show_input("Delete group?", "");
		if(! ptr) return;/* cancel pressed */
		strcpy(s_group, ptr);

		completely_delete_group(s_group);
		}
	save_subscribed_groups();

	/*
	To prevent hanging if now 'next unread' pressed,
	select something.
	*/
	select_first_group_and_article();
	}/* end delete groups */
show_browser(GROUP_LIST_BROWSER);
refresh_screen(selected_group);
}/* end function groups_menu_cb */


void article_menu_cb(FL_OBJECT *ob, long data)
{
int a;
int total_lines;
int s_line;
long article_id;

if(! selected_group) return;

articles_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(articles_menu_selection <= 0) return;

switch(articles_menu_selection)
	{
	case SEARCH_FOR_KEYWORD:
	case PGP_DECRYPT:
	case PGP_CHECK_SIGNATURE_AND_DECRYPT:
	case OTP_DECRYPT:
	case EXPORT_ARTICLE_PLUS_HEADER:
	case EXPORT_ARTICLE_ONLY:
		if(visible_browser != ARTICLE_BODY_BROWSER)
			{
			fl_show_alert("Select article body first", "", "", 0);
			return;
			}
		break;
	}/* end case */
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	if(articles_menu_selection == LOCK_ARTICLE)
		{
		set_lock_flag(selected_group, selected_article, 1);
		}
	if(articles_menu_selection == UNLOCK_ARTICLE)
		{
		set_lock_flag(selected_group, selected_article, 0);
		}
	if(articles_menu_selection == FOLLOW_SUBJECT)
		{
		set_follow_flag(\
		selected_group, selected_article, FOLLOW_SUBJECT);
		}
	if(articles_menu_selection == DO_NOT_FOLLOW_SUBJECT)
		{
		set_follow_flag(\
		selected_group, selected_article, DO_NOT_FOLLOW_SUBJECT);
		}
	if(articles_menu_selection == FOLLOW_THREAD)
		{
		set_follow_flag(\
		selected_group, selected_article, FOLLOW_THREAD);
		}
	if(articles_menu_selection == DO_NOT_FOLLOW_THREAD)
		{
		set_follow_flag(\
		selected_group, selected_article, DO_NOT_FOLLOW_THREAD);
		}
	if(articles_menu_selection == SEARCH_FOR_KEYWORD)
		{
		search_for_keyword(fdui -> article_body_input_field);
		}
	if(articles_menu_selection == EXPORT_ARTICLE_PLUS_HEADER)
		{
		export_article_plus_header(selected_group, selected_article, 1);
		}
	if(articles_menu_selection == EXPORT_ARTICLE_ONLY)
		{
		export_article_plus_header(selected_group, selected_article, 0);
		}
	if(articles_menu_selection == PGP_DECRYPT)
		{
		pgp_decode_and_show_article(selected_group, selected_article);
		}
	if(articles_menu_selection == PGP_CHECK_SIGNATURE_AND_DECRYPT)
		{
		pgp_check_signature_decode_and_show_article(\
		selected_group, selected_article);
		}
	if(articles_menu_selection == OTP_DECRYPT)
		{
		otp_decode_and_show_article(selected_group, selected_article);
		}
	save_articles();
	return;
	}
if(visible_browser != ARTICLE_LIST_BROWSER)
	{
	fl_show_alert("Select article list first", "", "", 0);
	return;
	}
total_lines = fl_get_browser_maxline(fdui -> article_list_browser);
for(s_line = 1; s_line <= total_lines; s_line++)
	{
	if(fl_isselected_browser_line(fdui -> article_list_browser, s_line) )
		{
		/* 
		lines in summary do not have an article_id,
		if clicking on a summary, the first previous article_id is used.
		*/
		a = s_line;
		while(1)
			{
			if(line_to_article_id(selected_group, a, &article_id) ) break;
			a--;
			if(a < 1) break;/* first browser line is 1 */
			}

		if(articles_menu_selection == MARK_ARTICLE_FOR_RETRIEVAL)
			{
			set_retrieval_flag(selected_group, article_id, 1);
			}/* end mark article for retrieval */
		if(articles_menu_selection == UNMARK_ARTICLE_FOR_RETRIEVAL)
			{
			set_retrieval_flag(selected_group, article_id, 0);
			}/* end unmark article for retrieval */
		if(articles_menu_selection == DELETE_ARTICLE)
			{
			if(strcmp(selected_group, "NewsFleX.help") == 0)
				{
				fl_show_alert("articles in NewsFleX.help cannot be deleted",\
				"", "", 0);
				return;				
				}
			delete_article_entry_header_body(selected_group, article_id);
			}/* end delete article */
		if(articles_menu_selection == MARK_ARTICLE_READ)
			{
			set_read_flag(selected_group, article_id, 1);
			}/* end mark article read */
		if(articles_menu_selection == MARK_ARTICLE_UNREAD)
			{
			set_read_flag(selected_group, article_id, 0);
			}/* end mark article unread */
		if(articles_menu_selection == LOCK_ARTICLE)
			{
			set_lock_flag(selected_group, article_id, 1);
			}
		if(articles_menu_selection == UNLOCK_ARTICLE)
			{
			set_lock_flag(selected_group, article_id, 0);
			}
		if(articles_menu_selection == FOLLOW_SUBJECT)
			{
			set_follow_flag(\
			selected_group, article_id, FOLLOW_SUBJECT);
			}
		if(articles_menu_selection == DO_NOT_FOLLOW_SUBJECT)
			{
			set_follow_flag(\
			selected_group, article_id, DO_NOT_FOLLOW_SUBJECT);
			}
		if(articles_menu_selection == FOLLOW_THREAD)
			{
			set_follow_flag(\
			selected_group, article_id, FOLLOW_THREAD);
			}
		if(articles_menu_selection == DO_NOT_FOLLOW_THREAD)
			{
			set_follow_flag(\
			selected_group, article_id, DO_NOT_FOLLOW_THREAD);
			}
		if(articles_menu_selection == CANCEL_ARTICLE)
			{
			cancel_article(selected_group, article_id);
			}
		}/* end selected line */
	}/* end for all lines */
save_articles();
if(articles_menu_selection == MARK_ALL_NEW_ARTICLES_FOR_RETRIEVAL)
	{
	set_all_retrieval_flags_in_new_articles(selected_group, 1);
	}/* end mark all articles for retrieval */
if(articles_menu_selection == UNMARK_ALL_NEW_ARTICLES_FOR_RETRIEVAL)
	{
	set_all_retrieval_flags_in_new_articles(selected_group, 0);
	}/* end unmark all articles for retrieval */
if(articles_menu_selection == DELETE_ARTICLE)
	{
	get_and_set_unread_articles(selected_group);
	}/* end delete article */
if(articles_menu_selection == DELETE_ALL_NOT_LOCKED_ARTICLES)
	{
	delete_all_not_locked_articles(selected_group);
	get_and_set_unread_articles(selected_group);
	}/* end delete all not locked articles article */
if(articles_menu_selection == MARK_ALL_ARTICLES_READ)
	{
	set_all_read_flags(selected_group, 1);
	get_and_set_unread_articles(selected_group);
	}/* end mark all articles read */
if(articles_menu_selection == MARK_ALL_ARTICLES_UNREAD)
	{
	set_all_read_flags(selected_group, 0);
	get_and_set_unread_articles(selected_group);
	}/* end mark all articles unread */
refresh_screen(selected_group);
}/* end function article_menu_cb */


void posting_menu_cb(FL_OBJECT *ob, long data)
{
int a;
int s_line;
int total_lines;
long posting_id;
int send_status, lock;

posting_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(posting_menu_selection <= 0) return;

posting_source = POSTING_MENU;
if(posting_menu_selection == NEW_ARTICLE)
	{
	if( (visible_browser != ARTICLE_LIST_BROWSER) &&\
	(visible_browser != ARTICLE_BODY_BROWSER) )
		{
		fl_show_alert("Select a news group first", "", "", 0);
		return;
		}
	/* fill in the input fields for the posting editor */
	if(! fill_in_input_fields(\
	selected_group, 99999999, posting_menu_selection) ) return;
	/* show the editor */
	fl_show_form(fd_NewsFleX -> posting_editor,\
	FL_PLACE_CENTER,  FL_UP_FRAME, "");
	return;
	}
if(posting_menu_selection == FOLLOW_UP_ARTICLE)
	{
	if(visible_browser != ARTICLE_BODY_BROWSER)
		{
		fl_show_alert("Select an article first", "", "", 0);
		return;
		}
	/* fill in the input fields for the posting editor */
	if(! fill_in_input_fields(\
	selected_group, selected_article, posting_menu_selection) ) return;

	/* show the editor */
	fl_show_form(fd_NewsFleX -> posting_editor,\
	FL_PLACE_CENTER,  FL_UP_FRAME, "");
	return;
	}
if(posting_menu_selection == REPLY_VIA_EMAIL)
	{
	if(visible_browser != ARTICLE_BODY_BROWSER)
		{
		fl_show_alert("Select an article first", "", "", 0);
		return;
		}
	/* fill in the input fields for the posting editor */
	if(! fill_in_input_fields(\
	selected_group, selected_article, posting_menu_selection) ) return;

	/* show the editor */
	fl_show_form(fd_NewsFleX -> posting_editor,\
	FL_PLACE_CENTER,  FL_UP_FRAME, "");
	return;
	}

/* the following commands all need the posting list */
if (visible_browser != POSTING_LIST_BROWSER)
	{
	fl_show_alert("Select posting list first",\
	"select window then posting list", "", 0);
	return;
	}

total_lines = fl_get_browser_maxline(fdui -> posting_list_browser);
for(s_line = 1; s_line <= total_lines; s_line++)
	{
/* XXXXXXXXXXXXXXXXXXX problems if multiline entries in posting list */
	if (fl_isselected_browser_line(fdui -> posting_list_browser, s_line))
		{
		/* 
		lines in multi line postings do not have posting_id,
		if clicking on a posting, the first previous posting_id is used.
		*/
		a = s_line;
		while(1)
			{
			if(line_to_posting_id(a, &posting_id) ) break;
			a--;
			if(a < 1) break;/* first browser line is 1 */
			}
/*		line_to_posting_id(i, &posting_id);*/
		if(posting_menu_selection == MARK_FOR_TRANSMIT)
			{
			set_send_status(posting_id, TO_BE_SEND);
			}
		if(posting_menu_selection == UNMARK_FOR_TRANSMIT)
			{
			set_send_status(posting_id, SEND_LATER);
			}
		if(posting_menu_selection == LOCK_POSTING)
			{
			set_lock_posting(posting_id, 1);
			}
		if(posting_menu_selection == UNLOCK_POSTING)
			{
			set_lock_posting(posting_id, 0);
			}
		if(posting_menu_selection == MARK_SEND)
			{
			set_send_status(posting_id, SEND_OK);
			}
		if(posting_menu_selection == UNMARK_SEND)
			{
			set_send_status(posting_id, SEND_LATER);
			}
		if(posting_menu_selection == DELETE_SELECTED_POSTINGS)
			{
			get_posting_status(posting_id, &send_status, &lock);
			if(lock) continue;
			if(send_status == SEND_OK) continue;
			if(send_status == TO_BE_SEND) continue;
			delete_posting_entry_header_body_attachment_unencoded(posting_id);
			}
		}/* end if line selected */
	}/* end for all lines */
save_postings();
refresh_screen(selected_group);
}/* end function posting_menu_cb*/


void options_menu_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "options_menu_selection=%d\n", options_menu_selection);
	}

options_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(options_menu_selection <= 0) return;

if(options_menu_selection == DEBUG_MODE)	
	{
	debug_flag = 1 - debug_flag;		
	if(debug_flag)
		{
		fl_set_menu_item_mode(ob, DEBUG_MODE, FL_PUP_CHECK);
		}
	else
		{
		fl_set_menu_item_mode(ob, DEBUG_MODE, FL_PUP_BOX);
		}
	}
if(options_menu_selection == SETUP)
	{
	show_setup_form();
	}
if(options_menu_selection == HELPERS)
	{
	show_helpers_form();
	}
if(options_menu_selection == LOCAL_SEARCH)
	{
	show_search_form();
	}
if(options_menu_selection == HIGHLIGHT)
	{
	show_highlight_form();
	}
if(options_menu_selection == FILTER)
	{
	show_filter_form();
	}
/*
if(options_menu_selection == RESTORE_PREVIOUS_SESSION)
	{
	restore();
	}
*/
if(options_menu_selection == REGENERATE_SUBSCRIBED_GROUPS_DAT)
	{
	regenerate_subscribed_groups(news_server_name, database_name);
	}
if(options_menu_selection == REGENERATE_ARTICLES_DAT)
	{
	regenerate_articles_dat();
	}
if(options_menu_selection == REGENERATE_POSTINGS_DAT)
	{
	regenerate_postings_dat();
	}
/*
if(options_menu_selection == SECRET_AGENT)
	{
	show_secret_form();
	}
*/
if(options_menu_selection == XAGENT_INFO)
	{
	show_info_form();
	}
}/* end function options_menu_cb */


void online_menu_cb(FL_OBJECT *ob, long data)
{
int i;
char s_group[TEMP_SIZE];
int total_lines;
char temp[TEMP_SIZE], _dummy[TEMP_SIZE];
long posting_id;
int type;

online_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(online_menu_selection <= 0) return;

process_command_queue_error_flag = 0;

if(expired_flag)
	{
	show_space();
	return;
	}

if(strcmp(database_name, "current") != 0)
	{
	fl_show_alert(\
	"online operations are not possible on",\
	database_name,\
	"select current database from WINDOW NEWSSERVER LIST",\
	0); 
	return;
	}/* end if not a current database selected */

/*
If the artile list is visible, it would be refreshed every time a new
header is received, this may slow down the online process in a bad way.
Select the group list browser, user can over ride this by selecting article
list again.
Only do this in case get new headers or get marked article bodies.
*/
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	switch(online_menu_selection)
		{
		case GET_NEW_HEADERS_IN_SELECTED_GROUPS:
		case GET_NEW_HEADERS_IN_SUBSCRIBED_GROUPS:
		case GET_MARKED_ARTICLE_BODIES:
			show_browser(GROUP_LIST_BROWSER);
			refresh_screen(selected_group);
			break;
		default:
			break;
		}/* end switch online_menu_selection */
	}/* end if visible browser == article list browser */

/* report per session */
error_log_present_flag = 0;

/* always send any postings marked for transmit */
posting_source = ONLINE_MENU;
add_marked_posting_bodies_to_command_queue();

/* always get any marked article bodies */
/* no, user decides, may be wrong time of day, high phone rates, etc. */
/*add_marked_article_bodies_in_subscribed_groups_to_command_queue();*/

if(online_menu_selection == GET_NEW_HEADERS_IN_SELECTED_GROUPS)
	{
	total_lines  = fl_get_browser_maxline(fdui -> group_list_browser);
	for(i = 1; i <= total_lines; i++)
		{
		if (fl_isselected_browser_line(fdui -> group_list_browser, i))
			{
			strcpy(temp,\
			fl_get_browser_line(fdui -> group_list_browser, i) );
			sscanf(temp, "%s %s", _dummy, s_group);
		
			add_to_command_queue(s_group, 0, GET_NEW_HEADERS_IN_GROUP);
			}
		}
	}
if(online_menu_selection == GET_NEW_HEADERS_IN_SUBSCRIBED_GROUPS)
	{
	add_new_headers_in_subscribed_groups_to_command_queue();/* in groups.c */
	}
if(online_menu_selection == GET_MARKED_ARTICLE_BODIES)
	{
	add_marked_article_bodies_in_subscribed_groups_to_command_queue();
	}
if(online_menu_selection == GET_MARKED_URLS)
	{
	add_marked_urls_to_command_queue();
	}
if(online_menu_selection == REFRESH_GROUP_LIST)
	{
	add_to_command_queue("dummy", 0, LIST);
	}
if(online_menu_selection == POST_SELECTED_POSTINGS)
	{
	if(visible_browser != POSTING_LIST_BROWSER)
		{
		fl_show_alert(\
		"Select posting list first", "(select window, then posting list)",\
		"", 0);
		return;
		}
	total_lines = fl_get_browser_maxline(fdui -> posting_list_browser);
	for(i = 1; i <= total_lines; i++)
		{
		if (fl_isselected_browser_line(fdui -> posting_list_browser, i))
			{
			line_to_posting_id(i, &posting_id);
			body_posting = posting_id;
			type = get_posting_type(body_posting);
			posting_source = ONLINE_MENU;

			/* fill in the input fields (not really showing them) */
			if(! show_posting_body(body_posting) ) /* filter: permission
													denied 
													*/
				{
				continue;
				}
			if(! save_posting("dummy", 99999999, SEND_NOW) ) return;
			if( (type == NEW_ARTICLE) || (type == FOLLOW_UP_ARTICLE) )
				{
				add_to_command_queue("dummy", body_posting , POST_ARTICLE);
				}
			if(type == REPLY_VIA_EMAIL)
				{
				add_to_command_queue("dummy", body_posting , EMAIL_ARTICLE);
				}
			}/* end if line selected */
		}/* end for all lines */
	}/* end online_menu_selection == post selected postings */

}/* end function online_menu_cb */


void file_menu_cb(FL_OBJECT *ob, long data)
{
file_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(file_menu_selection <= 0) return;

if(file_menu_selection == 1) 
	{
/*	save_message_ids(); */ /* really takes to long, list becomes very long */
	if(error_log_present_flag)
		{
		fl_show_alert(\
		"There were online errors", "check error log", "", 0);
		}

	if( connected() )
		{
		fl_show_alert("You are still connected to a server!", "", "", 0);
		return; 
		}

	if(fl_show_question("Quit NewsFleX?", 0) )/* mouse on NO */
		{
		save_general_settings();
		exit(1);
		}
	}/* end if file menu selection == 1 */
}/* end function file_menu_cb */


void window_menu_cb(FL_OBJECT *ob, long data)
{
int topline;

window_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(window_menu_selection <= 0) return;

if(window_menu_selection == GROUP_LIST_WINDOW)
	{
	group_list_button_cb(fdui -> group_list_button, 0);
	}
if(window_menu_selection == ARTICLE_LIST_WINDOW)
	{
	article_list_button_cb(fdui -> article_list_button, 0);
	}
if(window_menu_selection == ARTICLE_BODY_WINDOW)
	{
	article_body_button_cb(fdui -> article_body_button, 0);
	}
if(window_menu_selection == POSTING_LIST_WINDOW)
	{
	posting_list_button_cb();/* button does not exist, just a function */

	/* make the status of the last article visible */
	topline = fl_get_browser_maxline(fdui -> posting_list_browser);
	fl_set_browser_topline(fdui -> posting_list_browser, topline);
	}
if(window_menu_selection == URL_LIST_WINDOW)
	{
	show_url_form();
	}
if(window_menu_selection == INCOMING_MAIL_WINDOW)
	{
	show_incoming_mail_form();
	}
if(window_menu_selection == NEWSSERVER_WINDOW)
	{
	show_newsserver_form();
	}
if(window_menu_selection == POSTING_PERIOD_WINDOW)
	{
	show_posting_period_form();
	}
if(window_menu_selection == ERROR_LOG_WINDOW)
	{
	show_error_log_form();
	}
}/* end function window_menu_cb */


void group_list_browser_cb(FL_OBJECT *ob, long data)
{
}/* end function group_list_browser_cb */


void group_list_browser_double_cb(FL_OBJECT *ob, long data)
{
int a;
char temp[TEMP_SIZE], _dummy[TEMP_SIZE];
char group[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "group_list_browser_double_cb(): arg none\n");
	}

a = fl_get_browser(ob);
if(! a) return;
		
/* sometimes a is negative why? deselect */
strcpy(temp, fl_get_browser_line(ob, abs(a) ) );

sscanf(temp, "%s %s", _dummy, group);/* loose format string @C1 */
if(selected_group) free(selected_group);
selected_group = strsave(group);

/* 
select the line, as reference when switching between group list
and article list 
*/ 
fl_select_browser_line(fdui -> group_list_browser, abs(a) );

if(! subscribed_to_group(selected_group) ) return;

fl_set_object_label(fdui -> description_display, selected_group);

/*
This flag causes first  article to appear at top of article list
then flag is reset by show_articles()
*/
unread_article_at_top_flag = 1;

show_browser(ARTICLE_LIST_BROWSER);
refresh_screen(selected_group);
}/* end function group_list_browser_double_cb */


void article_list_browser_cb(FL_OBJECT *ob, long data)
{
}/* end function article_list_browser_cb */


void article_list_browser_double_cb(FL_OBJECT *ob, long data)
{
int a, m, selected_line;
int encoding;
long article_id;
int view_permission;
 
if(debug_flag)
	{
	fprintf(stdout, "article_list_browser_double_cb() arg none\n");
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
	if(line_to_article_id(selected_group, a, &article_id) ) break;
	a--;
	if(a < 1) break;/* first browser line is 1 */
	}

selected_article = article_id;

if(debug_flag)
	{
	fprintf(stdout, "SELECTED a=%d article_id=%ld\n", a, article_id);
	}


m = fl_mouse_button();
if(m == FL_RIGHT_MOUSE)
	{
	if(selected_article < 0) return;

	show_article_header(selected_group, selected_article);

	fl_show_form(fd_NewsFleX -> article_header_form,\
	FL_PLACE_CENTER, FL_NOBORDER, "");
	return;
	}

/* 
select the line, as reference when switching between article list
and article body 
*/ 
fl_select_browser_line(fdui -> article_list_browser, abs(a) );

/* 
NOTE: exists toggles pa -> retrieval_flag,
if no permission to view body and filters enabled, it also returns 0.
*/
if( exists_article_body(selected_group, selected_article) ) 
	{
	/* 
	test if binary attachment was launched before,
	if so do it again 
	*/
	if(! get_article_encoding(\
	selected_group, selected_article, &encoding, &view_permission) )
		{
		return;
		}

	if(! view_permission)
		{
		fl_set_object_label(fdui -> command_status_display,\
		"Filter: Permission denied");
		return;
		}
		
	if(encoding & ATTACHMENT_PRESENT)
		{
		launch_binary_attachment(selected_group, selected_article);
		return;
		}
	else
		{
		show_browser(ARTICLE_BODY_BROWSER);
		}
	}/* end article exists */

refresh_screen(selected_group);
}/* end function article_list_browser_double__cb */


void article_body_input_field_cb(FL_OBJECT *ob, long data)
{
extract_url_from_input(\
ob, article_body_copy, selected_group, selected_article);
}/* end function article_body_input_field_cb */


void posting_list_browser_cb(FL_OBJECT *ob, long data)
{
}/* end function posting_list_browser_cb */


void posting_list_browser_double_cb(FL_OBJECT *ob, long data)
{
int a, m;
int selected_line;

selected_line = fl_get_browser(ob);
	
/* 
lines in summary do not have an posting_id,
if clicking on a summary, the previous posting_id is used.
*/
a = abs(selected_line);
if(! a) return;
while(1)
	{
	if(line_to_posting_id(a, &selected_posting) ) break;
	a--;
	if(a < 1) break;/* first browser line is 1 */
	}

m = fl_mouse_button();
if(m == FL_RIGHT_MOUSE)
	{
	if(selected_posting < 0) return;

	show_posting_header(selected_posting);

	fl_show_form(fd_NewsFleX -> posting_header_form,\
	FL_PLACE_CENTER, FL_NOBORDER, "");
	return;
	}

/*
select the line, as reference when switching between posting list
and body
*/ 
fl_select_browser_line(fdui -> posting_list_browser, a);

posting_modified_flag = 0;
posting_source = POSTING_LIST;
body_posting = selected_posting;/* in case send now button pressed */
if(debug_flag)fprintf(stdout, "selected_posting=%ld\n", selected_posting);

if(! posting_show_permission(selected_posting) )
	{
	fl_set_object_label(fdui -> command_status_display,\
	"Filter: Permission denied");
	return;
	}

/* show the editor */
show_posting_body(selected_posting);

/* fill in alt_editor_command_input_field */
load_general_settings();

fl_show_form(fd_NewsFleX -> posting_editor,\
FL_PLACE_CENTER,  FL_UP_FRAME, "");
}/* end function posting_list_browser_double_cb */


void group_list_button_cb(FL_OBJECT *ob, long data)
{
char temp[TEMP_SIZE];

if(visible_browser == GROUP_LIST_BROWSER) return;

if(groups_show_selection == SHOW_ONLY_NEW_GROUPS)
	{
	sprintf(temp, "NEW GROUPS");
	}
if(groups_show_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
	{
	sprintf(temp, "SUBSCRIBED GROUPS");
	}
if(groups_show_selection == SHOW_ALL_GROUPS) sprintf(temp, "ALL GROUPS");

fl_set_object_label(fdui -> description_display, temp);

show_browser(GROUP_LIST_BROWSER);
refresh_screen(selected_group);
}/* end function group_list_button_cb */


void article_list_button_cb(FL_OBJECT *ob, long data)
{
char temp[TEMP_SIZE];

if(! selected_group) return;

if(visible_browser == ARTICLE_LIST_BROWSER) return;

sprintf(temp, "%s", selected_group);
fl_set_object_label(fdui -> description_display, temp);

/*
This flag causes first  article to appear at top of article list
then flag is reset by show_articles()
*/
/*unread_article_at_top_flag = 1;*/

show_browser(ARTICLE_LIST_BROWSER);
refresh_screen(selected_group);
}/* end function article_list_button_cb */


void article_body_button_cb(FL_OBJECT *ob, long data)
{
/* check for http browser reload */
if(html_file_shown)
	{
	http_browser_reload();
	return;
	}

if(selected_article < 0) return;

if(visible_browser == ARTICLE_BODY_BROWSER) return;

show_browser(ARTICLE_BODY_BROWSER);
refresh_screen(selected_group);
}/* end function article_body_button_cb */


void view_article_header_button_cb(FL_OBJECT *ob, long data)
{
/* check for http browser reload */
if(html_file_shown)
	{
	http_browser_reload();
	return;
	}

if(selected_article < 0) return;

show_article_header(selected_group, selected_article);

fl_show_form(fd_NewsFleX -> article_header_form,\
FL_PLACE_CENTER, FL_NOBORDER, "");
}/* end function view_article_header_button_cb */


int posting_list_button_cb()
/* this button does not exist!! called by window_menu_cb */
{
if(visible_browser == POSTING_LIST_BROWSER) return(0);

fl_set_object_label(fdui -> description_display, "POSTING LIST");

show_browser(POSTING_LIST_BROWSER);
refresh_screen("dummy");/* 
						selected_group = NULL on startup, 
						so if first thing selected is WINDOW then
						POSTING LIST no show, because return 0 
						*/
return(1);
}/* end function posting_list_button_cb */


void next_thread_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
int topline;
int add_to_history;

/*
char temp[80];
sprintf(temp, "%ld", selected_posting);
ptr = find_next_allowed_posting(temp);
if(! ptr)
	{
	selected_posting = -1;
	}
else selected_posting = atol(ptr);
fprintf(stdout, "SELETED POSTING=%s\n", ptr);
return;
*/

if(html_file_shown)
	{
	ptr = get_next_url(&topline);
	if(! ptr) return;
	http_browser(ptr, &add_to_history, 0);
	fl_set_input_topline(fdui -> article_body_input_field, topline);
	}
else
	{
	display_next_unread_article_in_subject();
	}
}/* end function next_thread_button_cb */


void next_unread_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
int topline;
int add_to_history;

if(html_file_shown)
	{
	ptr = get_previous_url(&topline);
	if(! ptr) return;
	http_browser(ptr, &add_to_history, 0);
	fl_set_input_topline(fdui -> article_body_input_field, topline);
	}
else
	{
	display_next_unread_article();
	}
}/* end function next_unread_button_cb */


void html_decode_button_cb(FL_OBJECT *ob, long data)
{
if(! html_file_shown)
	{
	/* this will signal to show_article_body to use http_browser() */
	html_decode_flag = 1 - html_decode_flag;
	if(html_decode_flag)
		{
		fl_set_object_color(ob, FL_YELLOW, FL_YELLOW);
		fl_set_object_label(ob, "H");
		}
	else
		{
		fl_set_object_color(ob, FL_COL1, FL_YELLOW);
		fl_set_object_label(ob, "N");
		}
	
	/* redraw if article body shown */
	if(visible_browser == ARTICLE_BODY_BROWSER)
		{
		refresh_screen(selected_group);
		}
	}/* end if ! html_file_shown */
}/* end function html_decode_button_cb */


void browser_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;
int fontsize;

fontsize = 0;/* only for clean compilation with -Wall */

if(visible_browser == GROUP_LIST_BROWSER)
	{
	fontsize = group_list_browser_fontsize;
	}
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	fontsize = article_list_browser_fontsize;
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	fontsize = article_body_browser_fontsize;
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	fontsize = posting_list_browser_fontsize;
	}

if(fontsize < 5) fontsize++;
size = integer_to_fontsize(fontsize);

if(visible_browser == GROUP_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> group_list_browser, size);
	group_list_browser_fontsize = fontsize;
	}
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> article_list_browser, size);
	article_list_browser_fontsize = fontsize;
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	fl_set_object_lsize(fdui -> article_body_input_field, size);
	article_body_browser_fontsize = fontsize;
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> posting_list_browser, size);
	posting_list_browser_fontsize = fontsize;
	}

save_fontsizes();
}/* end function size_up_button_cb */


void browser_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;
int fontsize;

fontsize = 0;/* only for clean compilation with -Wall */

if(visible_browser == GROUP_LIST_BROWSER)
	{
	fontsize = group_list_browser_fontsize;
	}
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	fontsize = article_list_browser_fontsize;
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	fontsize = article_body_browser_fontsize;
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	fontsize = posting_list_browser_fontsize;
	}

if(fontsize > 0) fontsize--;
size = integer_to_fontsize(fontsize);

if(visible_browser == GROUP_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> group_list_browser, size);
	group_list_browser_fontsize = fontsize;
	}
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> article_list_browser, size);
	article_list_browser_fontsize = fontsize;
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	fl_set_object_lsize(fdui -> article_body_input_field, size);
	article_body_browser_fontsize = fontsize;
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	fl_set_browser_fontsize(fdui -> posting_list_browser, size);
	posting_list_browser_fontsize = fontsize;
	}

save_fontsizes();
}/* end function size_down_button_cb */


void desc_input_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(desc_input_fontsize < 5) desc_input_fontsize++;
size = integer_to_fontsize(desc_input_fontsize);

fl_set_object_lsize(fdui -> desc_input, size);
save_fontsizes();
}/* end function desc_input_fontsize_up_button_cb */


void desc_input_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(desc_input_fontsize > 0) desc_input_fontsize--;
size = integer_to_fontsize(desc_input_fontsize);

fl_set_object_lsize(fdui -> desc_input, size);
save_fontsizes();
}/* end function desc_input_fontsize_down_button_cb */


void launch_button_cb(FL_OBJECT *ob, long data)
{
if(visible_browser != ARTICLE_BODY_BROWSER) return;

if(html_file_shown)
	{
	extract_url_from_input_html_file_and_add_to_url_list(\
	fdui -> article_body_input_field, article_body_copy);
	}
else
	{
	launch_binary_attachment(selected_group, selected_article);
	}
return;
}/* end function launch_button_cb */


void stop_button_cb(FL_OBJECT *ob, long data)
{
/* alarm quitering */
audio_alarm_flag = 0;

/*
???
if(process_command_queue_error_flag) return;
*/

if(news_server_status == CONNECTED)
	{
	news_io_cb_error_return("");
	}
if(mail_server_status == CONNECTED)
	{
	mail_io_cb_error_return("");
	}
if(http_server_status == CONNECTED)
	{
	http_io_cb_error_return("");
	}
if(ftp_control_server_status == CONNECTED)
	{
	ftp_control_io_cb_error_return("");
	}
if(ftp_data_server_status == CONNECTED)
	{
	ftp_data_io_cb_error_return("");
	}

process_command_queue_error_flag = 1;
delete_all_commands();
command_in_progress = 0;

if(auto_disconnect_from_net_flag) auto_disconnect_from_net();

/*
replace any net references with local file references, including moved url's.
*/
modify_all_html_files();

/*
clear the list that prevents going in a loop if in recursive mode,
and downloaded .html files refer to each other.
*/
delete_all_inurls();

/* no more need for these */
delete_all_moved_urls();

if(insert_headers_offline_flag)
	{
	add_new_headers_from_list("*");/* this will clear the list */
	}

set_new_data_flag(1);

if(filters_enabled_flag) set_all_filter_flags_for_new_articles();

/* prevent looping within this minute (fast connection, little data ) */
if(time_match_flag) get_new_headers_time -= 60;
time_match_flag = 0;
}/* end function stop_button_cb */


int integer_to_fontsize(int fontsize)
{
int size;

size = 0;/* only for clean compilation with -Wall */

/* tiny snall normal medium large huge */
if(fontsize == 0) size = FL_TINY_SIZE;
if(fontsize == 1) size = FL_SMALL_SIZE;
if(fontsize == 2) size = FL_NORMAL_SIZE;
if(fontsize == 3) size = FL_MEDIUM_SIZE;
if(fontsize == 4) size = FL_LARGE_SIZE;
if(fontsize == 5) size = FL_HUGE_SIZE;

return(size);
}/* end function integer to fontsize */


int redraw_browser()
{
/* argument check */
if(! selected_group) return(0);
if(! acquire_group) return(0);

/* ifacquire group is shown, redraw it (lines will turn blue from magenta) */
if(strcmp(acquire_group, selected_group) == 0)
	{
	refresh_screen(selected_group);
	}
return(1);
}


int show_browser(int browser)
{
if(debug_flag)
	{
	fprintf(stdout, "show_browser(): browser=%d visible_browser=%d\n",\
	browser, visible_browser);
	}

/* test if already selected */
if(browser == visible_browser) return(1);

/* hide the visible one */
if(visible_browser == GROUP_LIST_BROWSER)
	{
	fl_hide_object(fdui -> group_list_browser);
	}
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	fl_hide_object(fdui -> article_list_browser);
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	fl_hide_object(fdui -> article_body_input_field);
	fl_hide_object(fdui -> view_article_header_button);
	fl_show_object(fdui -> article_body_button);
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	fl_hide_object(fdui -> posting_list_browser);
	}

/* show the selected one */
if(browser == GROUP_LIST_BROWSER)
	{
	fl_show_object(fdui -> group_list_browser);
	if(groups_show_selection == SHOW_ALL_GROUPS)
		{
		fl_set_object_label(fdui -> description_display, "ALL GROUPS");
		}
	if(groups_show_selection == SHOW_ONLY_SUBSCRIBED_GROUPS)
		{
		fl_set_object_label(fdui -> description_display, "SUBSCRIBED GROUPS");
		}	
	if(groups_show_selection == SHOW_ONLY_NEW_GROUPS)
		{
		fl_set_object_label(fdui -> description_display, "NEW GROUPS");
		}
	}
if(browser == ARTICLE_LIST_BROWSER)
	{
	fl_show_object(fdui -> article_list_browser);
	}
if(browser == ARTICLE_BODY_BROWSER)
	{
	fl_show_object(fdui -> article_body_input_field);
	fl_hide_object(fdui -> article_body_button);
	fl_show_object(fdui -> view_article_header_button);
	}
if(browser == POSTING_LIST_BROWSER)
	{
	fl_show_object(fdui -> posting_list_browser);
	}

visible_browser = browser;
/* 
clear the command status display (from "helper program xxx started")
this is a compromise, but put it in every button if you want,
and then offline AND online?
*/
my_command_status_display("");

/* let the mouse know: cut and paste, not get clicked URLs */
html_file_shown = 0;

set_browser_buttons();
return(1);
}/* end function show_browser */


int my_command_status_display(char *text)
{
sprintf(command_status_string, "%s", text);
command_status_string_flag = 1;
return(1);
}/* end function my_command_status_display */


int my_line_status_display(char *text)
{
sprintf(line_status_string, "%s", text);
line_status_string_flag = 1;
return(1);
}/* end function my_line_status_display */


int my_show_alert(char *text1, char *text2, char *text3)
{
char temp[TEMP_SIZE];

if(! auto_disconnect_from_net_flag)
	{
	fl_show_alert(text1, text2, text3, 0);
	}

/* combine text */
sprintf(temp, "%s %s %s",\
text1, text2, text3);

/* write to error log */
to_error_log(temp);

return(1);
}/* end function my_show_alert */
 

int refresh_screen(char *group)
{
if(debug_flag)
	{
	fprintf(stdout,\
	"refresh_screen(): arg group=%s using selected_group=%s\
 visible_browser=%d\n",\
	group, selected_group, visible_browser);
	}

/* argument check */
if(! group) return(0);

/* warn user that diplaying about 28219 lines (groups) takes time */
if(visible_browser == GROUP_LIST_BROWSER)
	{
	if(groups_loaded_flag)
		{
		if( (groups_menu_selection == SHOW_ALL_GROUPS) ||\
		(groups_menu_selection == SHOW_ONLY_NEW_GROUPS) ||\
		(groups_menu_selection == SUBSCRIBE) ||\
		(groups_menu_selection == UNSUBSCRIBE) ||\
		(groups_menu_selection == DELETE_GROUP) )
			{
			to_command_status_display(\
			"building display, this may take a while");
			}
		}/* end if all groups loaded */
	}/* end warn user */

/* prevent flashing */
fl_freeze_form(fdui -> NewsFleX);

if(visible_browser == GROUP_LIST_BROWSER) show_groups();
if(visible_browser == ARTICLE_LIST_BROWSER)
	{
	/* 
	The following strcmp is EXTREMELY important!!!!
	if omitted, show_articles() will load selected_group into the
	structure articles, EVEN if the group being processed is a different
	one, like for example select article list, and then from the groups
	menu, delete all not locked articles in al groups.
	That would mess up ALL articles.dat files.
	The same goes if online called from within io_callback if data is
	read for a different group.
	The calling routine will not know the structure has been replaced
	with that of an other group, and will write to the wrong group
	(i.e. selected group).
	This was a bug in NewsFleX.05 and before, and HARD to find.
	I added the group argument to refresh_screen()
	*/		
	if(strcmp(group, selected_group) == 0) show_articles(selected_group);
	}
if(visible_browser == ARTICLE_BODY_BROWSER)
	{
	/* 
	The following strcmp is EXTREMELY important!!!!
	if omitted, show_article_body() will load selected_group into the
	structure articles, EVEN if the group being processed is a different
	one, like for example select article list, and then from the groups
	menu, delete all not locked articles in al groups.
	That would mess up ALL articles.dat files.
	The same goes if online called from within io_callback if data is
	read for a different group.
	The calling routine will not know the structure has been replaced
	with that of an other group, and will write to the wrong group
	(i.e. selected group).
	This was a bug in NewsFleX.05 and before, and HARD to find.
	I added the group argument to refresh_screen()
	*/		
	if(strcmp(group, selected_group) == 0)
		{
		show_article_body(selected_group, selected_article, 0);
		}
	}
if(visible_browser == POSTING_LIST_BROWSER)
	{
	show_postings();
	}

fl_unfreeze_form(fdui -> NewsFleX);

return(1);
}/* end function refresh screen */


int ogettime(
char *weekday,
char *month,
int *monthday,
int *hour,
int *minute,
int *second,
int *year
)
{
unsigned long int I;
char *datestring;

time(&I); /* sets unsigned long int I to seconds since start 1970 */
datestring = ctime(&I);
					/* char * converts I to character string date and time */

sscanf(datestring,"%s %s %d %d:%d:%d %d",\
weekday, month, monthday, hour, minute, second, year);
												/* no &, already pointers */

return(1);
}/* end function ogettime */


int to_error_log(char *text)
{
char *universal_time;
char temp[TEMP_SIZE];

/* argument check */
if(! text) return(0);

/* create a new error log */
sprintf(temp, "%s/.NewsFleX/error_log.txt", home_dir);
error_log_fileptr = fopen(temp, "a");
if(! error_log_fileptr)
	{
	fprintf(stdout, "Cannot open file %s for write\n", temp);
	return(0);
/*	exit(1);*/
	}

universal_time = get_universal_time_as_string();
/*if(! universal_time) return(0);*/

/* write to error log */
fprintf(error_log_fileptr, "%s %s\n", text, universal_time);
free(universal_time);

fclose(error_log_fileptr);

error_log_present_flag = 1;

return(1);
}/* end function to_error_log */

