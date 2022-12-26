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


int readline(FILE *file, char *contents)
{
int a, c, i;

for(i = 0; i < READSIZE - 1; i++)
	{
	while(1)
		{
		c = getc(file);
		a = ferror(file);
		if(a)
			{
			perror("readline():");
			continue;
			}
		break;
		}
	if(feof(file) )
		{
		fclose(file);
		contents[i] = 0;/* EOF marker */
		return(EOF);
		}
	if(c == '\n')
		{
		contents[i] = 0;/* string termination */
		return(1);/* end of line */
		} 
	contents[i] = c;
	}
/*
mmm since we are here, the line must be quite long, possibly something
is wrong.
Since I do not always check for a return 0 in the use uf this function,
just to be safe, gona force a string termination.
This prevents stack overflow, and variables getting overwritten.
*/
contents[i] = 0;/* force string termination */
/*fclose(file);*/
if(debug_flag)
	{
	fprintf(stdout,\
	"readline(): line to long, returning 0 contents=%s\n", contents);
	}
return(0);
}/* end function readline */


int save_setting(char *type, char *string)
{
char pathfilename[TEMP_SIZE];
FILE *setting_file;

/* combine with home directory */
sprintf(pathfilename, "%s/.NewsFleX/setup/%s", home_dir, type);
setting_file = fopen(pathfilename, "w");
if(! setting_file)
	{
	fl_show_alert("Cannot open file for write:", pathfilename, "", 0);
	if(debug_flag)
		{
		fprintf(stdout, "\nCannot open setting file %s for write\n",\
		pathfilename);
		}
	return(0);
	}

fprintf(setting_file, "%s", string);
fclose(setting_file);
return(1);	
}/* end function save_setting */


char *load_setting(char *type)
{
int a;
char temp[1024];
char pathfilename[TEMP_SIZE];
FILE *setting_file;

/* combine with home directory */
sprintf(pathfilename, "%s/.NewsFleX/setup/%s", home_dir, type);
setting_file = fopen(pathfilename, "r");
if(! setting_file)
	{
	fl_show_alert("Cannot open file for read:", pathfilename, "", 0);
	if(debug_flag)
		{
		fprintf(stdout, "\nCannot open setting file %s for read\n",\
		pathfilename);
		}
	return(0);
	}

a = readline(setting_file, temp);/* closes file if EOF */	
if(a != EOF) fclose(setting_file);
return(strsave(temp) );
}/* end function load_setting */


int save_fontsizes()
{
char temp[TEMP_SIZE];
sprintf(temp,\
"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",\
group_list_browser_fontsize,\
article_list_browser_fontsize,\
article_body_browser_fontsize,\
posting_list_browser_fontsize,\
posting_body_editor_fontsize,\
setup_form_fontsize,\
desc_input_fontsize,\
custom_headers_editor_fontsize,\
filter_form_fontsize,\
summary_editor_fontsize,\
article_header_form_fontsize,\
posting_header_form_fontsize,\
coding_form_fontsize,\
url_form_fontsize,\
search_fontsize,\
highlight_fontsize,\
error_log_form_fontsize,\
helpers_form_fontsize,\
incoming_mail_form_fontsize,\
newsserver_form_fontsize,\
posting_period_form_fontsize);
if(! save_setting("fontsizes", temp) ) return(0);

return(1);
}/* end function save_fontsizes */


int load_fontsizes()
{
char *data_read;
 
data_read = (char *) load_setting("fontsizes");
if(! data_read) return(0);
sscanf(data_read,\
"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",\
&group_list_browser_fontsize,\
&article_list_browser_fontsize,\
&article_body_browser_fontsize,\
&posting_list_browser_fontsize,\
&posting_body_editor_fontsize,\
&setup_form_fontsize,\
&desc_input_fontsize,\
&custom_headers_editor_fontsize,\
&filter_form_fontsize,\
&summary_editor_fontsize,\
&article_header_form_fontsize,\
&posting_header_form_fontsize,\
&coding_form_fontsize,\
&url_form_fontsize,\
&search_fontsize,\
&highlight_fontsize,\
&error_log_form_fontsize,\
&helpers_form_fontsize,\
&incoming_mail_form_fontsize,\
&newsserver_form_fontsize,\
&posting_period_form_fontsize);
free(data_read);

fl_set_browser_fontsize(fdui -> group_list_browser,\
integer_to_fontsize(group_list_browser_fontsize) );
fl_set_browser_fontsize(fdui -> article_list_browser,\
integer_to_fontsize(article_list_browser_fontsize) );
fl_set_object_lsize(fdui -> article_body_input_field,\
integer_to_fontsize(article_body_browser_fontsize) );
fl_set_browser_fontsize(fdui -> posting_list_browser,\
integer_to_fontsize(posting_list_browser_fontsize) );
fl_set_object_lsize(fdui -> posting_body_editor,\
integer_to_fontsize(posting_body_editor_fontsize) );

/*
Not implemented, no fonts to change
fl_set_object_lsize(fdui -> setup_xxxxx,\
integer_to_fontsize(setup_form_fontsize) );
*/

fl_set_object_lsize(fdui -> desc_input,\
integer_to_fontsize(desc_input_fontsize) );
fl_set_browser_fontsize(fdui -> filter_list_browser,\
integer_to_fontsize(filter_form_fontsize) );
fl_set_object_lsize(fdui -> summary_editor_input_field,\
integer_to_fontsize(summary_editor_fontsize) );
fl_set_object_lsize(fdui -> article_header_form_input_field,\
integer_to_fontsize(article_header_form_fontsize) );
fl_set_object_lsize(fdui -> posting_header_form_input_field,\
integer_to_fontsize(posting_header_form_fontsize) );
fl_set_object_lsize(fdui -> coding_form_input_field,\
integer_to_fontsize(coding_form_fontsize) );

/*
fl_set_object_lsize(fdui -> url_form_input_field,\
integer_to_fontsize(url_form_fontsize) );
*/

fl_set_browser_fontsize(fdui -> url_form_browser,\
integer_to_fontsize(url_form_fontsize) );
fl_set_object_lsize(fdui -> error_log_form_input,\
integer_to_fontsize(error_log_form_fontsize) );
fl_set_object_lsize(fdui -> helpers_form_input,\
integer_to_fontsize(helpers_form_fontsize) );
fl_set_browser_fontsize(fdui -> incoming_mail_form_browser,\
integer_to_fontsize(incoming_mail_form_fontsize) );
fl_set_browser_fontsize(fdui -> newsserver_form_browser,\
integer_to_fontsize(newsserver_form_fontsize) );
fl_set_browser_fontsize(fdui -> posting_period_form_browser,\
integer_to_fontsize(posting_period_form_fontsize) );

return(1);
}/* end function load_fontsizes */


int save_search_settings()
{
char temp[TEMP_SIZE];

/* save search_flags */
sprintf(temp, "%d %d %d %d %d %d %d %d %d %d %d",\
search_postings_flag,\
search_articles_flag,\
search_headers_flag,\
search_headers_from_flag,\
search_headers_to_flag,\
search_headers_subject_flag,\
search_headers_references_flag,\
search_headers_others_flag,\
search_bodies_flag,\
search_attachments_flag,\
search_case_sensitive_flag);
save_setting("search_flags", temp);

save_setting("search_groups_keywords",\
(char *) fl_get_input(fdui -> search_groups_input_field) );

save_setting("search_keywords",\
(char *) fl_get_input(fdui -> search_keywords_input_field) );

return(1);
}/* end function save_search_settings */


int load_search_settings()
{
char *ptr;

/* load search flags */
ptr = (char *) load_setting("search_flags");
sscanf(ptr, "%d %d %d %d %d %d %d %d %d %d %d ",\
&search_postings_flag,\
&search_articles_flag,\
&search_headers_flag,\
&search_headers_from_flag,\
&search_headers_to_flag,\
&search_headers_subject_flag,\
&search_headers_references_flag,\
&search_headers_others_flag,\
&search_bodies_flag,\
&search_attachments_flag,\
&search_case_sensitive_flag);
free(ptr);

fl_set_button(fdui -> search_articles_button, search_articles_flag);
fl_set_button(fdui -> search_headers_button, search_headers_flag);
fl_set_button(fdui -> search_headers_from_button, search_headers_from_flag);
fl_set_button(fdui -> search_headers_to_button, search_headers_to_flag);
fl_set_button(fdui -> search_headers_subject_button,\
search_headers_subject_flag);
fl_set_button(fdui -> search_headers_references_button,\
search_headers_references_flag);
fl_set_button(fdui -> search_headers_others_button,\
search_headers_others_flag);
fl_set_button(fdui -> search_bodies_button, search_bodies_flag);
fl_set_button(fdui -> search_attachments_button, search_attachments_flag);
fl_set_button(fdui -> search_postings_button, search_postings_flag);
fl_set_button(fdui -> search_case_sensitive_button,\
search_case_sensitive_flag);

ptr = (char *) load_setting("search_groups_keywords");
free(search_groups_keywords);
search_groups_keywords = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> search_groups_input_field,\
search_groups_keywords);

ptr = (char *) load_setting("search_keywords");
free(search_keywords);
search_keywords = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> search_keywords_input_field,\
search_keywords);

return(1);
}/* end function load_search_settings */


int save_highlight_settings()
{
char temp[TEMP_SIZE];

/* save highlight_flags */
sprintf(temp, "%d %d %d %d %d %d %d %d %d %d %d %d",\
highlight_postings_flag,\
highlight_articles_flag,\
highlight_headers_flag,\
highlight_headers_from_flag,\
highlight_headers_to_flag,\
highlight_headers_subject_flag,\
highlight_headers_references_flag,\
highlight_headers_others_flag,\
highlight_case_sensitive_flag,\
highlight_urls_flag,\
highlight_urls_url_flag,\
highlight_urls_description_flag);
save_setting("highlight_flags", temp);

save_setting("highlight_keywords",\
(char *) fl_get_input(fdui -> highlight_keywords_input_field) );

return(1);
}/* end function save_highlight_settings */


int load_highlight_settings()
{
char *ptr;

/* load highlight flags */
ptr = (char *) load_setting("highlight_flags");
sscanf(ptr, "%d %d %d %d %d %d %d %d %d %d %d %d ",\
&highlight_postings_flag,\
&highlight_articles_flag,\
&highlight_headers_flag,\
&highlight_headers_from_flag,\
&highlight_headers_to_flag,\
&highlight_headers_subject_flag,\
&highlight_headers_references_flag,\
&highlight_headers_others_flag,\
&highlight_case_sensitive_flag,\
&highlight_urls_flag,\
&highlight_urls_url_flag,\
&highlight_urls_description_flag);
free(ptr);

fl_set_button(fdui -> highlight_articles_button, highlight_articles_flag);
fl_set_button(fdui -> highlight_headers_button, highlight_headers_flag);
fl_set_button(fdui -> highlight_headers_from_button, highlight_headers_from_flag);
fl_set_button(fdui -> highlight_headers_to_button, highlight_headers_to_flag);
fl_set_button(fdui -> highlight_headers_subject_button,\
highlight_headers_subject_flag);
fl_set_button(fdui -> highlight_headers_references_button,\
highlight_headers_references_flag);
fl_set_button(fdui -> highlight_headers_others_button,\
highlight_headers_others_flag);
fl_set_button(fdui -> highlight_postings_button, highlight_postings_flag);
fl_set_button(fdui -> highlight_case_sensitive_button,\
highlight_case_sensitive_flag);
fl_set_button(fdui -> highlight_urls_button,\
highlight_urls_flag);
fl_set_button(fdui -> highlight_urls_url_button,\
highlight_urls_url_flag);
fl_set_button(fdui -> highlight_urls_description_button,\
highlight_urls_description_flag);

ptr = (char *) load_setting("highlight_keywords");
free(highlight_keywords);
highlight_keywords = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> highlight_keywords_input_field,\
highlight_keywords);

return(1);
}/* end function load_highlight_settings */


int save_general_settings()
{
char *ptr;
/*char *crypt();*/
char temp[TEMP_SIZE];

/*
ptr = (char *) fl_get_input(fdui -> secret_password_input_field);
if(strlen(ptr) != 0)
	{
	save_setting("secret_password", (char *) crypt(ptr, "P8") );
	}

*/
ptr = (char *) fl_get_input(fdui -> filter_password_input_field);
if(strlen(ptr) != 0)
	{
#ifndef NO_CRYPT
	save_setting("filter_password", (char *) crypt(ptr, "P8") );
#else
	save_setting("filter_password", ptr);
#endif
	}

save_setting("browser_get_url",\
(char *) fl_get_input(fdui -> url_form_browser_command_input_field));

save_setting("alt_editor_command",\
(char *) fl_get_input(fdui -> posting_alt_editor_command_input_field));

save_setting("posting_source_macro",\
(char *) fl_get_input(fdui -> setup_form_posting_source_input_field));

sprintf(temp, "%d %d %d %d %d %d %d",\
sort_flag, lines_first_flag, posting_word_wrap_flag,\
	global_cut_of_sig_in_reply_flag, global_beep_on_mail_flag,\
	global_posting_wrap_chars, global_show_posting_source_flag);

save_setting("global_flags", temp);

return(1);
}/* end function save_general_settings */


int load_general_settings()
{
char *ptr;
char temp[512];

/*
ptr = (char *) load_setting("secret_password");
free(secret_password);
secret_password = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> secret_password_input_field, "");
*/

ptr = (char *) load_setting("filter_password");
free(filter_password);
filter_password = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> filter_password_input_field, "");

ptr = (char *) load_setting("browser_get_url");
free(browser_get_url_command);
browser_get_url_command = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> url_form_browser_command_input_field,\
browser_get_url_command);

ptr = (char *) load_setting("alt_editor_command");
free(alt_editor_command);
alt_editor_command = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> posting_alt_editor_command_input_field,\
alt_editor_command);

ptr = (char *) load_setting("posting_source_macro");
free(global_posting_source_text);
global_posting_source_text = (char *) strsave(ptr);
free(ptr);
fl_set_input(fd_NewsFleX -> setup_form_posting_source_input_field,\
global_posting_source_text);

ptr = load_setting("global_flags");
if(! ptr) return(0);
sscanf(ptr, "%d %d %d %d %d %d %d",\
&sort_flag, &lines_first_flag, &posting_word_wrap_flag,\
&global_cut_of_sig_in_reply_flag, &global_beep_on_mail_flag,\
&global_posting_wrap_chars, &global_show_posting_source_flag);
free(ptr);

if(sort_flag)
	{
	fl_set_menu_item_mode(fdui -> groups_menu, SORTED_DISPLAY, FL_PUP_CHECK);
	}
else
	{
	fl_set_menu_item_mode(fdui -> groups_menu, SORTED_DISPLAY, FL_PUP_BOX);
	}

if(lines_first_flag)
	{
	fl_set_button(fdui -> setup_form_lines_first_button, 1);
	fl_set_object_label(fdui -> setup_form_lines_first_button, "YES");
	}
else
	{
	fl_set_button(fdui -> setup_form_lines_first_button, 0);
	fl_set_object_label(fdui -> setup_form_lines_first_button, "NO");
	}

if(posting_word_wrap_flag)
	{
	fl_set_menu_item_mode(fdui -> posting_coding_menu, WORD_WRAP,\
	FL_PUP_CHECK);
	}
else
	{
	fl_set_menu_item_mode(fdui -> posting_coding_menu, WORD_WRAP,\
	FL_PUP_BOX);
	}

if(global_cut_of_sig_in_reply_flag)
	{
	fl_set_button(\
	fdui -> setup_form_cut_off_sig_in_reply_button, 1);
	fl_set_object_label(\
		fdui -> setup_form_cut_off_sig_in_reply_button, "YES");
	}
else
	{
	fl_set_button(\
	fdui -> setup_form_cut_off_sig_in_reply_button, 0);
	fl_set_object_label(\
		fdui -> setup_form_cut_off_sig_in_reply_button, "NO");
	}

if(global_beep_on_mail_flag)
	{
	fl_set_button(\
	fdui -> setup_form_beep_on_mail_button, 1);
	fl_set_object_label(\
		fdui -> setup_form_beep_on_mail_button, "YES");
	}
else
	{
	fl_set_button(\
	fdui -> setup_form_beep_on_mail_button, 0);
	fl_set_object_label(\
		fdui -> setup_form_beep_on_mail_button, "NO");
	}

sprintf(temp, "%d", global_posting_wrap_chars);
fl_set_input(fdui -> setup_form_posting_wrap_chars_input_field, temp);

if(global_show_posting_source_flag)
	{
	fl_set_button(\
	fdui -> setup_form_posting_source_button, 1);
	fl_set_object_label(\
		fdui -> setup_form_posting_source_button, "YES");
	}
else
	{
	fl_set_button(\
	fdui -> setup_form_posting_source_button, 0);
	fl_set_object_label(\
		fdui -> setup_form_posting_source_button, "NO");
	}

return(1);
}/* end function load_general_settings */


int show_setup_form()
{
fl_show_form(fd_NewsFleX -> setup, FL_PLACE_CENTER, FL_NOBORDER, "setup");

return(1);
}/* end function show_setup */


int connected()
{
if(news_server_status != DISCONNECTED) return(1);
if(mail_server_status != DISCONNECTED) return(1);
if(http_server_status != DISCONNECTED) return(1);
if(ftp_control_server_status != DISCONNECTED) return(1);
if(ftp_data_server_status != DISCONNECTED) return(1);

return(0);
}/* end function online */


int set_offline_indicator()
{
/*
This will set the line status display text to 'Offline' if and only if
ALL (news, mail, http, ftp) server connections are closed.
*/

if( connected() ) return(1);

fl_set_object_label(fdui -> line_status_display, "Offline");

return(1);
}/* end function set_offline_indicator */


int set_stop_button_color()
{
/* if connected to a server, make it red */

if( connected() )
	{
	/* make the stop button red */
	fl_set_object_color(fdui -> stop_button, FL_RED, FL_COL1);
    fl_set_object_label(fdui -> stop_button, "STOP");
	fl_set_object_label(fdui -> posting_alt_editor_button, "N.A. ONLINE");
	return(1);
	}	
fl_set_object_label(fdui -> posting_alt_editor_button, "ALT EDITOR");

/* if on timer and not connected make it orange */
if(auto_get_new_headers_flag)
	{
	/* make the stop button orange */
	fl_set_object_color(fdui -> stop_button, FL_DARKORANGE, FL_COL1);
	fl_set_object_label(fdui -> stop_button, get_new_headers_time);
	return(1);
	}
/* else make it grey */
fl_set_object_color(fdui -> stop_button, FL_COL1, FL_COL1);
fl_set_object_label(fdui -> stop_button, "STOP");
return(1);
}/* end function set_stop_button_color */


int set_tabs(int tabsize)
{
int i;
char *space;

if(debug_flag)
	{
	fprintf(stdout, "set_tabs(): arg tabsize=%d\n", tabsize);
	}

/* argument check */
if(tabsize < 0) return(0);

/* set the global (used by expand_tabs ) */
tab_size = tabsize;

space = malloc(tabsize + 1);
if(! space) return(0);
for(i = 0; i < tabsize; i++)
	{
	space[i] = 'a';
	}
space[i] = 0;

fl_set_tabstop(space);
free(space);

return(1);
}/* end funcion set_tabs */


char *expand_tabs(char *text, int tabsize)
{
int c, i, j, k;
int tabs;
char *space;

if(debug_flag)
	{
	fprintf(stdout, "expand_tabs(): arg text=%s tabsize=%d\n",\
	text, tabsize);
	}

/* argument check */
if(! text) return(0);
if(tabsize < 0) return(0);

/* determine hom many tabs, so we can calculate the required space */
i = 0;
tabs = 0;
while(1)
	{
	c = text[i];
	if(c == 0) break;
	if(c == 9) tabs++;
	i++;
	}/* end while all characters in text */

space = malloc(strlen(text) + (tabs * tabsize) - tabs + 1 + 1);
if(! space) return(0);

/* copy and expand */
i = 0;
j = 0;
while(1)
	{
	c = text[i];
	if(c == 9)
		{
		for(k = 0; k < tabsize; k++)
			{
			space[j] = ' ';
			j++;
			}
		i++;
		continue;
		}
	else
		{
		space[j] = c;
		}
	if(c == 0) break;
	i++;
	j++;
	}
return(space);
}/* end function expand_tabs */


