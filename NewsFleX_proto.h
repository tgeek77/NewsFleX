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
redefined by me.
from arpa/inet.h (char *)inet_ntoa((struct in_addr) in) );
Note that in is actually unsigned long.
*/
unsigned long inet_addr(const char *);
extern char *inet_ntoa(unsigned long sa);

extern char *expand_posting_source_macros(\
	char *text, char *group, long article_id);
extern char *add_source_info_line(char *text, char *group, long article_id);
extern void posting_color_box_cb(FL_OBJECT *ob, long data);

extern void setup_form_posting_source_button_cb(\
    FL_OBJECT *ob, long data);
extern void  setup_form_posting_source_input_field_cb(\
   FL_OBJECT *ob, long data);
extern void setup_form_posting_wrap_chars_input_field_cb(\
	FL_OBJECT *ob, long data);
extern int cut_off_sig(char *text);
extern void setup_form_beep_on_mail_button_cb(\
	FL_OBJECT *ob, long data);
extern void setup_form_cut_off_sig_in_reply_button_cb(\
	FL_OBJECT *ob, long data);
extern char *trim_references_line(char *text);
extern char *fold_header_line(char *text);
extern int delete_url_file(char *url);
extern int delete_url_file_and_all_url_files_referenced_by_it(char *url);

extern int replace_in(\
char *instr, char *namestr, char *defstr, char **resstr);
extern char *expand_posting_macros (char *text);

extern void url_form_show_marked_only_button_cb(FL_OBJECT *ob, long data);

extern int set_url_update_time(char *url_in_progress, time_t now);
extern int get_first_allowed_posting(long *first);
extern char *find_next_posting_period(char *current_posting_period);
extern struct posting *find_next_allowed_posting(char *posting);

extern char *find_next_newsserver_and_period(\
char *current_newsserver_and_period);

extern host_allowed(char *url, int *allowed, int flags);
extern int load_custom_colors();
extern int find_anchor_line_number(char *space, char *anchor, int *line);
extern int extract_url_from_input_html_file_and_add_to_url_list(\
FL_OBJECT *obj, char *copy);

extern void html_decode_button_cb(FL_OBJECT *ob, long data);

extern char *get_content_type(char *url);
extern int start_helper(char *url, char *content_type);

extern int http_browser_reload();
extern int set_browser_buttons();
extern int set_url_history_current_topline(int topline);
extern char *get_previous_url(int *topline);
extern char *get_next_url(int *topline);
extern add_to_url_history(char *url, int topline);
extern struct displayed_url *lookup_displayed_url(char *name);
extern struct displayed_url *install_displayed_url_at_end_of_list(char *name);
extern int delete_displayed_url(char *name);

/*
This is a GNU extention, it should have been prototyped in
strings.h, or maybe strings.h, but it is not.
*/
extern int strncasecmp(const char *S1, const char *S2, size_t N);

extern int connected();

extern int scip_tag(char *position, char **new_pos);
extern char *do_img(char *position, char **new_pos);
extern char *do_a(char *position, char **new_pos);
extern char *do_quoted_char(char *position, char **new_pos);
extern char *dequote(char *text);
extern char *do_pre_on(char *position, char **new_pos);
extern char *do_pre_off(char *position, char **new_pos);
extern char *do_center_on(char *position, char **new_pos);
extern char *do_center_off(char *position, char **new_pos);
extern char *do_title_on(char *position, char **new_pos);
extern char *do_title_off(char *position, char **new_pos);
extern char *do_table_end(char *position, char **new_pos);
extern char *do_tr(char *position, char **new_pos);
extern char *do_th(char *position, char **new_pos);
extern char *do_td(char *position, char **new_pos);
extern char *do_table_start(char *position, char **new_pos, int center);
extern char *do_p(char *position, char **new_pos);
extern char *do_b(char *position, char **new_pos);
extern char *do_br(char *position, char **new_pos);
extern char *do_hr(char *position, char **new_pos);
extern char *do_hx(char *position, char **new_pos);
extern int http_browser(char *url, int *add_to_history, int mode);

extern struct html_table_entry *install_html_table_entry_at_end_of_list(\
char *name);
extern int delete_all_html_table_entries();
extern int add_html_table_entry(char *text, int row, int column, int type);
extern char *print_formatted_html_table(int center);

extern int http_modify_url(char *url, FILE *outfileptr, int image_flag, char *host);
extern int modify_comment(char *position, char **new_pos, FILE *outfileptr);
extern int modify_base(char *position, char **new_pos, FILE *outfileptr, char *host);
extern int modify_body(char *position, char **new_pos, FILE *outfileptr, char *host);
extern int modify_link(char *position, char **new_pos, FILE *outfileptr, char *host);
extern int modify_a(char *position, char **new_pos, FILE *outfileptr, char *host);
extern int modify_img(char *position, char **new_pos, FILE *outfileptr, char *host);
extern int write_modified_html_file(char *filename, int flags);

extern char *scip_comment(char *position, char **new_pos);
extern char *find_base(char *position, char **new_pos);
extern char *find_body(char *position, char **new_pos);
extern char *find_link(char *position, char **new_pos);
extern char *find_a(char *position, char **new_pos);
extern char *find_img(char *position, char **new_pos);

extern char *strip_http_www(char *url);

extern struct html_file *lookup_html_file(char *name);
extern struct html_file *install_html_file_at_end_of_list(char *name);
extern int delete_html_file_entry(char *name);
extern int delete_all_html_files_entries();
extern int add_html_file(char *filename, int flags);
extern int modify_all_html_files();

extern struct moved_url *lookup_moved_url(char *name);
extern struct moved_url *install_moved_url_at_end_of_list(char *name);
extern int delete_moved_url(char *name);
extern int delete_all_moved_urls();
extern char *lookup_new_location(char *url);
extern int add_new_location(char *url, char *new_location);

extern char *strcasestr(char *str1, char *str2);

extern struct inurl *lookup_inurl(char *name);
extern struct inurl *install_inurl_at_end_of_list(char *name);
extern int delete_all_inurls();
extern int add_inurl(char *url, int *present_flag);

extern char *reformat_text(char *text);
extern int news_io_cb_error_return(char *text);
extern int http_extract_url_and_add_to_command_queue(\
	char *dir, char *field, int flags);
extern int http_parse_file(char *filename, float version, int mode);

extern void url_form_depth_input_field_cb(FL_OBJECT *ob, long data);
extern void url_form_get_daily_button_cb(FL_OBJECT *ob, long data);
extern void url_form_get_pictures_button_cb(FL_OBJECT *ob, long data);
extern void url_form_host_only_button_cb(FL_OBJECT *ob, long data);
extern int set_offline_indicator();

extern void url_form_browse_online_button_cb(FL_OBJECT *ob, long data);
extern int set_url_status(char *url, int status);
extern int get_url_status(char *url, int *status);
extern int add_marked_urls_to_command_queue();

extern int http_post(char *source, char *url);
extern int http_put(char *source, char *url);
extern int url_get(char *url, int options);

extern int connect_to_http_server(char *www_server, int port);
extern int send_to_http_server(char *text);
extern void http_io_read_cb(int socket, long data);
extern int http_io_cb_error_return(char *error_message);

extern int connect_to_ftp_server(char *www_server, int port, int *socketfd);
extern int send_to_ftp_server(int socketfd, char *text);
extern void ftp_control_io_read_cb(int socket, long data);
extern int ftp_control_io_cb_error_return(char *error_message);
extern void ftp_data_io_read_cb(int socket, long data);
extern int ftp_data_io_cb_error_return(char *error_message);

extern int extract_url_from_input(\
FL_OBJECT *ob, char * copy, char *group, long article_id);

extern int mark_all_articles_for_retrieval_in_group(\
char *group, int *found_some_flag);
extern int mark_all_bodies_in_get_always_all_bodies_groups();
extern int set_mark_all_flag(char *group, int state);

extern update_mail_display(int messages);
extern int check_mail_local();

extern int show_groups_in_tree();
extern int show_groups_tree();
extern int show_groups_hash();

extern int create_new_newsserver_databases(char *oldperiod);
extern int delete_all_workinglists();
extern struct workinglist *install_workinglist_at_end_of_list(char *name);
extern struct workinglist *lookup_workinglist(char *name);

extern int _calloc__();
extern void server_username_input_field_cb(FL_OBJECT *ob, long data);
extern void server_password_input_field_cb(FL_OBJECT *ob, long data);

extern char *increment_space(char *pa);
extern char *substract_space(char *pbig, char *psmall);
extern char *divide_space(char *pbig, char *psmall);
extern char *add_space(char *pa, char *pb);
extern char *multiply_space(char *space, char *cd);
extern char *code(char *text, char *key1);
extern char *decode(char *text, char *key1);

extern int _malloc__(char *a_code);
extern int get_maximum_headers(char *group, int *m_headers);
extern int set_maximum_headers(char *group, int m_headers);
extern int to_error_log(char *text);
extern int set_title();
extern int get_first_article_in_group(char *group, long *article);
extern char *detect_new_period_and_return_old_period(int *new);
extern int move_current_database_and_create_new_current(\
char *newsserver, char *newname);

extern struct posting_period *lookup_posting_period(char *name);
extern struct posting_period *install_posting_period_at_end_of_list(\
char *name);
extern int delete_posting_period(char *name);
extern int delete_all_posting_periods();
extern int load_posting_periods();
extern int save_posting_periods();
extern int delete_a_posting_period(char *period);
extern int show_posting_period_form();
extern int add_posting_period(char *period);
extern int select_posting_period(char *period);
extern int show_posting_periods(int position);
extern char *line_to_posting_period(int line);
extern int posting_period_to_line(char *name, int *line);
extern int move_current_postings_database_and_create_new_current(\
char *destperiod);

extern void posting_period_form_input_field_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_browser_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_browser_double_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_delete_button_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_add_button_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_fontsize_up_button_cb(\
FL_OBJECT *ob, long data);
extern void posting_period_form_fontsize_down_button_cb(\
FL_OBJECT *ob, long data);
extern void posting_period_form_cancel_button_cb(FL_OBJECT *ob, long data);
extern void posting_period_form_accept_button_cb(FL_OBJECT *ob, long data);

extern int reset_groups();
extern int reset_articles();
extern int reset_postings();

extern int load_filter_settings();
extern int save_filter_settings();
extern int load_search_settings();
extern int save_search_settings();
extern int load_highlight_settings();
extern int save_highlight_settings();
extern int load_newsserver_setting(char *newsserver);
extern int save_newsserver_setting(char *newsserver);
extern int load_general_settings();
extern int save_general_settings();

extern int make_space();
extern int get_space();
extern int allocate_space();
extern int no_space();
extern int show_space();
extern int show_warning_space();

extern struct newsserver *lookup_newsserver(char *name);
extern struct newsserver *install_newsserver_at_end_of_list(char *name);
extern int delete_newsserver(char *name);
extern int load_newsservers();
extern int show_newsserver_form();
extern int show_newsservers(int postition);
extern int select_newsserver(char *newsserver);
extern int delete_a_newsserver(char *name);
extern int add_newsserver(char *newsserver);
extern char *line_to_newsserver(int line);
extern int newsserver_to_line(char *newsserver, int *line);

extern void newsserver_form_browser_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_browser_double_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_input_field_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_delete_button_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_add_button_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_cancel_button_cb(FL_OBJECT *ob, long data);
extern void newsserver_form_accept_button_cb(FL_OBJECT *ob, long data);

extern int regenerate_subscribed_groups(char *newsserver, char *period);
extern int regenerate_articles_dat();
extern int regenerate_postings_dat();
extern int regenerate_mark_article(char *group, long article);
extern int regenerate_mark_posting(long posting);
extern int regenerate_posting(\
long posting, int body_flag, int local_custom_headers_flag,\
int attachment_flag);

extern struct mail *lookup_mail(char *name);
extern struct mail *install_mail_at_end_of_list(char *name);
extern int delete_all_mails();
extern int load_incoming_mail(int *messages);
extern int show_incoming_mail_form();
extern int show_incoming_mail();
extern int line_to_mail_id(int line, long *mail_id);
extern int reply_to_incoming_mail(long mail_id);
extern int mail_fill_in_input_fields(long mail_id);
extern int show_incoming_mail_form();

extern void incoming_mail_form_browser_cb(FL_OBJECT *ob, long data);
extern void incoming_mail_form_browser_double_cb(FL_OBJECT *ob, long data);
extern void incoming_mail_form_mark_read_button_cb(\
FL_OBJECT *ob, long data);
extern void incoming_mail_form_fontsize_up_button_cb(\
FL_OBJECT *ob, long data);
extern void incoming_mail_form_fontsize_down_button_cb(\
FL_OBJECT *ob, long data);
extern void incoming_mail_form_cancel_button_cb(FL_OBJECT *ob, long data);
extern void incoming_mail_form_accept_button_cb(FL_OBJECT *ob, long data);

extern int mail_io_cb_error_return(char *error_message);
extern int post_email(long posting_id);
extern int post_email_via_sendmail(long posting_id);
extern int connect_to_mail_server(char *mail_server, int port);
extern void mail_io_read_cb(int socket, long data);
extern int send_to_mail_server(char *text);

extern int export_article_plus_header(\
char *group, long article, int header_flag);

extern int verify_postings_point_dat();
extern int verify_articles_point_dat(char *group);

extern int search_for_keyword(FL_OBJECT *obj);
extern int make_visible_line_of_text_with_keyword_in_input_field(\
FL_OBJECT *obj, char *keyword, int first);

extern int test_for_ipcp_up(int we_started_pppd_flag, int *up_flag);
extern int calculate_and_display_tasks();

extern struct article_body2 *lookup_article_body2(char *name);
extern struct article_body2 *install_article_body2_at_end_of_list(\
char *name);
extern int clear_article_body2_list();
extern int add_to_article_body2_list(char *message_id);
extern int send_all_message_ids_to_server2();

extern struct article_body *lookup_article_body(char *name);
extern struct article_body *install_article_body(char *name);
extern int clear_article_body_list();
extern int add_to_article_body_list(\
char *message_id, char *group, long article);
extern int send_all_message_ids_to_server();
extern char *get_article_bodies_group(char *message_id, long *article);

extern int backup();
extern int restore();

extern struct helpers *lookup_helpers(char *name, int sequence);
extern struct helpers *install_helpers(char *name, char *def);
extern struct helpers *reverse_lookup_helpers(char *name, int sequence);
extern int set_helpers(char *name, char *def);
extern int get_helpers(char *name, char *def, int sequence);
extern int delete_helpers();
extern int show_helpers();
extern int save_helpers();
extern int load_helpers();
extern int show_helpers_form();
extern void helpers_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void helpers_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void helpers_form_input_cb(FL_OBJECT *ob, long data);
extern void helpers_form_accept_button_cb(FL_OBJECT *ob, long data);
extern void helpers_form_cancel_button_cb(FL_OBJECT *ob, long data);

extern struct new_header *lookup_new_header(char *name);
extern struct new_header *install_new_header_at_end_of_list(char *name);
extern int clear_new_headers_list();
extern int add_to_new_header_list(char *group, long article_id);
extern int remove_from_new_header_list(char *group, long article_id);
extern int add_new_headers_from_list(char *spec_group);

extern int show_error_log_form();
extern int erase_error_log();

extern void error_log_form_input_cb(FL_OBJECT *ob, long data);
extern void error_log_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void error_log_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void error_log_form_clear_button_cb(FL_OBJECT *ob, long data);
extern void error_log_form_accept_button_cb(FL_OBJECT *ob, long data);
extern void error_log_form_cancel_button_cb(FL_OBJECT *ob, long data);

extern int execute_get_mail_script();
extern int auto_connect_to_net();

extern struct message_id *lookup_message_id(char *name);
extern struct message_id *install_message_id(char *name);
extern int delete_all_message_ids();
extern int load_message_ids();
extern int save_message_ids();
extern int add_to_message_id_list(char *message_id, int *present_flag);

extern char *extract_keyword_groups(char *keywords);

extern int show_highlight_form();
extern void highlight_postings_button_cb(FL_OBJECT *ob, long data);
extern void highlight_articles_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_from_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_to_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_subject_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_references_button_cb(FL_OBJECT *ob, long data);
extern void highlight_headers_others_button_cb(FL_OBJECT *ob, long data);
extern void highlight_bodies_button_cb(FL_OBJECT *ob, long data);
extern void highlight_attachments_button_cb(FL_OBJECT *ob, long data);
extern void highlight_urls_button_cb(FL_OBJECT *ob, long data);
extern void highlight_urls_url_button_cb(FL_OBJECT *ob, long data);
extern void highlight_urls_description_button_cb(FL_OBJECT *ob, long data);
extern void highlight_case_sensitive_button_cb(FL_OBJECT *ob, long data);
extern void highlight_keywords_input_field_cb(FL_OBJECT *ob, long data);
extern void highlight_cancel_button_cb(FL_OBJECT *ob, long data);
extern void highlight_accept_button_cb(FL_OBJECT *ob, long data);

extern void url_form_browser_command_input_field_cb(\
FL_OBJECT *ob, long data);
extern void url_form_external_browser_button_cb(FL_OBJECT *ob, long data);
extern void url_form_browser_cb(FL_OBJECT *ob, long data);
extern void url_form_browser_double_cb(FL_OBJECT *ob, long data);
extern void url_form_input_field_cb(FL_OBJECT *ob, long data);
extern void url_form_delete_button_cb(FL_OBJECT *ob, long data);
extern void url_form_clear_button_cb(FL_OBJECT *ob, long data);
extern void url_form_add_button_cb(FL_OBJECT *ob, long data);
extern void url_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void url_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void url_form_cancel_button_cb(FL_OBJECT *ob, long data);
extern void url_form_accept_button_cb(FL_OBJECT *ob, long data);
extern void url_form_search_input_field_cb(FL_OBJECT *ob, long data);
extern void url_form_search_button_cb(FL_OBJECT *ob, long data);
extern void url_form_search_case_button_cb(FL_OBJECT *ob, long data);
extern int search_and_select_next_url();
extern int url_form_reset_last_found_line();
extern int set_url_input(char *url);
extern int show_article_that_url_came_from(char *url);
extern void replace(char *text, char *string, char *replace, char *result);
extern int show_url_form();
extern int add_url(char *text, char *group, long article_id);
extern delete_url(char *url);
extern int delete_all_urls();
extern load_urls();
extern save_urls();
extern start_browser(char *url);
extern show_urls(int position);
extern char *line_to_url(int line);
extern int url_to_line(char *url, int *line);

extern int cancel_article(char *group, long article);
extern char *get_message_id(char *group, long article);
extern int do_alt_editor();

extern int pgp_clear_sign_posting();
extern int pgp_encrypt_posting();
extern int pgp_append_public_key_to_posting();
extern int otp_encrypt_posting();
extern int otp_create_random_key_file();
extern int show_unencrypted_posting(long posting_id);

extern int pgp_check_signature_decode_and_show_article(\
char *group, long article_id);
extern int pgp_decode_and_show_article(char *group, long article_id);
extern int delete_decoded_article(char *group, long article_id);
extern int otp_decode_and_show_article(char *group, long article_id);

extern void coding_form_input_field_cb(FL_OBJECT *ob, long data);
extern void coding_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void coding_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void coding_form_cancel_button_cb(FL_OBJECT *ob, long data);

extern int save_unencoded_body(char *u_data, long posting_id);

extern int set_posting_encoding(long posting_id, int state);
extern int get_posting_encoding(long posting_id, int *state);

extern int delete_posting_entry_header_body_attachment_unencoded(\
long posting_id);
extern int show_posting_header(long posting_id);
extern void posting_header_form_input_field_cb(FL_OBJECT *ob, long data);
extern void posting_header_form_fontsize_up_button_cb(\
FL_OBJECT *ob, long data);
extern void posting_header_form_fontsize_down_button_cb(\
FL_OBJECT *ob, long data);
extern void posting_header_form_cancel_button_cb(FL_OBJECT *ob, long data);

extern void view_article_header_button_cb(FL_OBJECT *ob, long data);
extern void article_header_form_input_field_cb(FL_OBJECT *ob, long data);
extern void article_header_form_fontsize_up_button_cb(\
FL_OBJECT *ob, long data);
extern void article_header_form_fontsize_down_button_cb(\
FL_OBJECT *ob, long data);
extern void article_header_form_cancel_button_cb(FL_OBJECT *ob, long data);
extern int show_article_header(char *group, long article_id);
extern void tab_size_input_field_cb(FL_OBJECT *ob, long data);
extern void setup_form_lines_first_button_cb(FL_OBJECT *ob, long data);
extern void setup_form_check_local_mail_button_cb(FL_OBJECT *ob, long data);
extern void setup_form_local_mail_check_interval_input_field_cb(\
FL_OBJECT *ob, long data);
extern char *expand_tabs(char *text, int tabsize);
extern int set_tabs(int tabsize);
extern char *fold_text(char *text);
extern void summary_editor_insert_file_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_write_file_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_cancel_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_accept_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void summary_editor_input_field_cb(FL_OBJECT *ob, long data);
extern void summary_editor_clear_button_cb(FL_OBJECT *ob, long data);
extern int set_all_retrieval_flags_in_new_articles(char *group, int state);
extern void posting_coding_menu_cb(FL_OBJECT *ob, long data);
extern void posting_editor_signature_button_cb(FL_OBJECT *ob, long data);
extern void posting_editor_clear_button_cb(FL_OBJECT *ob, long data);
extern void posting_write_file_button_cb(FL_OBJECT *ob, long data);
extern void posting_insert_file_button_cb(FL_OBJECT *ob, long data);
extern void posting_cross_post_button_cb(FL_OBJECT *ob, long data);

extern struct article *lookup_article(char *name);
extern struct article *install_article_at_end_of_list(char *name);
extern struct article *install_article_in_position(\
 char *name, char *subject, char *references, int *thread);
extern int delete_article(char *name);/* delete entry from double linked list */
extern int delete_all_articles();
extern int load_articles(char *group);
extern int save_articles();
extern int add_article_header(char *group, long id, int *delete_flag);
extern int add_article_body(char *group, long article_id);
extern int show_articles(char *group);
extern int show_article_body(char *group, long article_id, int decoded);
extern int set_retrieval_flag(char *group, long article_id, int state);
extern int set_all_read_flags(char *group, int state);
extern int set_read_flag(char *group, long article_id, int state);
extern int exists_article_body(char *group, long article_id);
extern int line_to_article_id(char *group, int line, long *article_id);
extern int add_marked_article_bodies_in_group_to_command_queue(\
char *group, int *some_marked_article_body);
extern char *get_article_attachment(char *group, long article_id);
extern int delete_article_entry_header_body(char *group, long article_id);
extern int set_lock_flag(char *group, long article_id, int lock);
extern char *lookup_content_type(char *group, long article_id);
extern int set_article_encoding(\
char *group, long article_id, int encoding, int on);
extern int get_article_encoding(\
 char *group, long article_id, int *encoding, int *view_permission);
extern int set_article_attachment(\
 char *group, long article_id, char *attachment_name);
extern int set_article_no_longer_availeble_flag(char *group, long article_id);
extern int decode_articles_with_content_type_id(\
 char *group, char *content_type_id);
extern int get_and_set_unread_articles(char *group);
extern int delete_all_not_locked_articles(char *group);
extern int display_next_unread_article();
extern int display_next_unread_article_in_subject();
extern int set_follow_flag(char *group, long article_id, int value);
extern int search_next_article(char *search_groups_keywords);
extern int select_first_group_and_article();
extern char *get_first_group();
extern int get_first_article(char *group, long *article);
extern int set_all_filter_flags_in_group(char *group);
extern int apply_filters_to_article(char *group, long article_id);
extern int pop_up_desc_info(char *group, char *attachment_name);
extern int launch_binary_attachment(char *group, long article_id);
extern int decode_article(char *group, long article_id, char *content_type);
extern int show_info_form();
extern struct command *lookup_command(char *name);
extern struct command *install_command_at_end_of_list(char *name);
extern int delete_command(char *name);
extern int delete_all_commands();
extern int add_to_command_queue(char *group, long article, int command);
extern int process_command_queue();
extern int get_command_queue_size();
extern int connect_to_news_server(char *server, int port);
extern int send_to_news_server(char *text);
extern int auto_disconnect_from_net();
extern int to_command_status_display(char *text);
extern void global_custom_headers_enable_button_cb(FL_OBJECT *ob, long data);
extern void global_custom_headers_editor_input_field_cb(\
 FL_OBJECT *ob, long data);
extern void local_custom_headers_enable_button_cb(FL_OBJECT *ob, long data);
extern void local_custom_headers_editor_input_field_cb(\
 FL_OBJECT *ob, long data);
extern void custom_headers_editor_fontsize_up_button_cb(\
 FL_OBJECT *ob, long data);
extern void custom_headers_editor_fontsize_down_button_cb(\
 FL_OBJECT *ob, long data);
extern void posting_editor_fontsize_up_button_cb(\
 FL_OBJECT *ob, long data);
extern void posting_editor_fontsize_down_button_cb(\
 FL_OBJECT *ob, long data);
extern void custom_headers_editor_accept_button_cb(FL_OBJECT *ob, long data);
extern void custom_headers_editor_cancel_button_cb(FL_OBJECT *ob, long data);
extern void filter_password_input_field_cb(FL_OBJECT *ob, long data);
extern void filter_postings_button_cb(FL_OBJECT *ob, long data);
extern void filter_articles_button_cb(FL_OBJECT *ob, long data);
extern void filter_groups_input_field_cb(FL_OBJECT *ob, long data);
extern void filter_headers_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_from_button_cb(FL_OBJECT *ob, long data);
extern void filter_enable_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_to_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_subject_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_newsgroups_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_others_button_cb(FL_OBJECT *ob, long data);
extern void filter_bodies_button_cb(FL_OBJECT *ob, long data);
extern void filter_groups_button_cb(FL_OBJECT *ob, long data);
extern void filter_list_browser_cb(FL_OBJECT *ob, long data);
extern void filter_list_browser_double_cb(FL_OBJECT *ob, long data);
extern void filter_list_browser_delete_button_cb(FL_OBJECT *ob, long data);
extern void filter_list_browser_new_button_cb(FL_OBJECT *ob, long data);
extern void filter_attachments_button_cb(FL_OBJECT *ob, long data);
extern void filter_case_sensitive_button_cb(FL_OBJECT *ob, long data);
extern void filter_keywords_input_field_cb(FL_OBJECT *ob, long data);
extern void filter_keywords_and_button_cb(FL_OBJECT *ob, long data);
extern void filter_keywords_not_button_cb(FL_OBJECT *ob, long data);
extern void filter_keywords_only_button_cb(FL_OBJECT *ob, long data);
extern void filter_allow_button_cb(FL_OBJECT *ob, long data);
extern void filter_groups_show_button_cb(FL_OBJECT *ob, long data);
extern void filter_headers_show_button_cb(FL_OBJECT *ob, long data);
extern void filter_bodies_show_button_cb(FL_OBJECT *ob, long data);
extern void filter_attachments_show_button_cb(FL_OBJECT *ob, long data);
extern void filter_form_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void filter_form_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void filter_cancel_button_cb(FL_OBJECT *ob, long data);
extern void filter_accept_button_cb(FL_OBJECT *ob, long data);
extern struct filter *lookup_filter(char *name);
extern struct filter *install_filter_at_end_of_list(char *name);
extern int delete_filter(long filter_id);
extern int delete_all_filters();
extern int load_filters();
extern int save_filters();
extern int show_filter_form();
extern int show_filters();
extern int line_to_filter_id(int line, long *filter_id);
extern int set_filter(long filter_id);
extern int get_new_filter_id(long *filter_id);
extern int get_filter(long filter_id);
extern int get_first_filter(long *filter_id);
extern int add_filter(long filter_id);
extern int filter_id_to_line(long filter_id, int *line);

extern int get_filter_flags(char *group, char *article, int encoding,\
 char *attachment, int have_body_flag, int in_what, int *filter_flags);
extern int search_in_for_or(char *database, char *data, int case_sensitive);
extern int filter_search_in_for(char *database, char *data, int mode);
extern int strcmp_case_insensitive(char *s, char *t);
extern int set_new_data_flag(int val);
extern int get_new_data_flag(int *val);
extern int hash(char *s);

extern struct group *lookup_group(char *name);
extern struct group *install_group(char *name);
extern int delete_all_groups();
/*
extern struct newsgroup *lookup_group(char *name, struct newsgroup *pa);
extern struct newsgroup *install_group(char *name, struct newsgroup **group);
extern int delete_all_groups(struct newsgroup *pa);
*/
extern int get_article_ptr(char *group, long *article_ptr);
extern int set_article_ptr(char *group, long article_ptr);
extern int set_new_articles(char *group, long new_articles);
extern int get_new_articles(char *group, long *new_articles);
extern int mark_all_groups_cancelled();
extern int load_groups();
extern int load_subscribed_groups(int mode);
extern int save_groups();
extern int save_subscribed_groups();
extern int subscribe_group(char *group, int subscribe);
extern int add_group(char *data);
extern int show_groups();
extern int add_new_headers_in_subscribed_groups_to_command_queue();
extern int add_marked_article_bodies_in_subscribed_groups_to_command_queue();
extern int subscribed_to_group(char *group);
extern int delete_all_not_locked_articles_in_subscribed_groups();
extern char *get_next_subscribed_group(char *group);
extern int completely_delete_group(char *group);
extern int set_all_filter_flags_in_articles();
extern int apply_filters_to_group(char *group);
extern int set_all_filter_flags_in_groups();
extern struct new_article *lookup_new_article(char *name);
extern struct new_article *install_new_article_at_end_of_list(char *name);
extern int clear_new_articles_list();
extern int add_to_new_list(char *group, long article_id);
extern int remove_from_new_list(char *group, long article_id);
extern int set_all_filter_flags_for_new_articles();
extern struct posting *lookup_posting(char *name);
extern struct posting *install_posting_at_end_of_list(char *name);
extern int delete_posting(char *name);
extern int delete_all_postings();
extern int show_structure();
extern int load_postings();
extern int save_postings();
extern int show_postings();
extern int show_posting_body(long posting_id);
extern int line_to_posting_id(int line, long *posting_id);
extern int fill_in_input_fields(char *group, long article_id, int type);
extern int save_posting(char *group, long article_id, int mode);
extern int set_send_status(long posting_id, int status);
extern int get_send_status();
extern int save_posting_header(long posting_id);
extern int save_posting_body(long posting_id);
extern int load_and_ident_article(char *group, long article_id);
extern int get_new_posting_id(long *posting_id);
extern char *get_universal_time_as_string();
extern char *get_header_data(char *group, long article_id, char *field);
extern char *get_formatted_header_data(char *group, long article_id, char *field);
extern int post_email(long posting_id);
extern int get_posting_type(long posting_id);
extern int test_if_posting_has_attachment(long posting_article, int *present_flag);
extern int set_lock_posting(long posting_id, int status);
extern int get_posting_status(long posting_id, int *send, int *lock);
extern int add_marked_posting_bodies_to_command_queue();
extern int search_next_posting();
extern int get_custom_headers_flag(long posting_article, int *present_flag);
extern int set_all_filter_flags_in_postings();
extern int posting_show_permission(long posting_id);
extern char *strstr_case_insensitive(char *s, char *t);
extern int show_search_form();
extern int search_all(char *group, char *article, int encoding,\
 char *attachment, int have_body_flag, int search_what);
extern int search_in_for(char *database, char *data, int case_sensitive);
extern int search_display_progress(char *text, char *subject);
extern void search_postings_button_cb(FL_OBJECT *ob, long data);
extern void search_articles_button_cb(FL_OBJECT *ob, long data);
extern void search_groups_input_field_cb(FL_OBJECT *ob, long data);
extern void search_headers_button_cb(FL_OBJECT *ob, long data);
extern void search_headers_from_button_cb(FL_OBJECT *ob, long data);
extern void search_headers_to_button_cb(FL_OBJECT *ob, long data);
extern void search_headers_subject_button_cb(FL_OBJECT *ob, long data);
extern void search_headers_references_button_cb(FL_OBJECT *ob, long data);
extern void search_headers_others_button_cb(FL_OBJECT *ob, long data);
extern void search_bodies_button_cb(FL_OBJECT *ob, long data);
extern void search_attachments_button_cb(FL_OBJECT *ob, long data);
extern void search_case_sensitive_button_cb(FL_OBJECT *ob, long data);
extern void search_keywords_input_field_cb(FL_OBJECT *ob, long data);
extern void search_cancel_button_cb(FL_OBJECT *ob, long data);
extern void search_accept_button_cb(FL_OBJECT *ob, long data);
extern void question_no_button_cb(FL_OBJECT *ob, long data);
extern void question_yes_button_cb(FL_OBJECT *ob, long data);
extern int show_secret_form();
extern void secret_password_input_field_cb(FL_OBJECT *ob, long data);
extern void secret_cancel_button_cb(FL_OBJECT *ob, long data);
extern void secret_accept_button_cb(FL_OBJECT *ob, long data);
extern int readline(FILE *file, char *contents);
extern int save_setting(char *type, char *string);
extern char *load_setting(char *type);
extern int save_fontsizes();
extern int load_fontsizes();
extern int load_preferences();
extern int save_preferences();
extern int show_setup_form();
extern int set_stop_button_color();
extern void get_new_headers_enable_button_cb(FL_OBJECT *ob, long data);
extern void get_new_headers_time_input_field_cb(FL_OBJECT *ob, long data);
extern void pid_test_input_field_cb(FL_OBJECT *ob, long data);
extern void desc_button_cb(FL_OBJECT *ob, long data);
extern void width_button_cb(FL_OBJECT *ob, long data);
extern void auto_connect_to_net_button_cb(FL_OBJECT *ob, long data);
extern void auto_connect_to_net_input_field_cb(FL_OBJECT *ob, long data);
extern void auto_disconnect_from_net_button_cb(FL_OBJECT *ob, long data);
extern void auto_disconnect_from_net_input_field_cb(FL_OBJECT *ob, long data);
extern void auto_execute_application_button_cb(FL_OBJECT *ob, long data);
extern void auto_execute_application_input_field_cb(FL_OBJECT *ob, long data);
extern void news_server_input_field_cb(FL_OBJECT *ob, long data);
extern void mail_server_input_field_cb(FL_OBJECT *ob, long data);
extern void news_server_port_input_field_cb(FL_OBJECT *ob, long data);
extern void mail_server_port_input_field_cb(FL_OBJECT *ob, long data);
extern void real_name_input_field_cb(FL_OBJECT *ob, long data);
extern void user_email_input_field_cb(FL_OBJECT *ob, long data);
extern void maximum_headers_per_group_input_field_cb(\
FL_OBJECT *ob, long data);
extern void connect_to_news_server_timeout_input_field_cb(\
FL_OBJECT *ob, long data);
extern void connect_to_mail_server_timeout_input_field_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_insert_headers_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_get_by_message_id_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_show_only_once_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_do_not_use_sendmail_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_fontsize_up_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_form_fontsize_down_button_cb(\
FL_OBJECT *ob, long data);
extern void setup_accept_button_cb(FL_OBJECT *ob, long data);
extern void setup_cancel_button_cb(FL_OBJECT *ob, long data);
/*extern FD_NewsFleX *create_form_NewsFleX(void);*/
extern char *strsave(char *s);
extern int idle_cb(XEvent *ev, void *data);
extern void news_io_read_cb(int socket, long data);
extern void desc_input_cb(FL_OBJECT *ob, long data);
extern void desc_cancel_button_cb(FL_OBJECT *ob, long data);
extern void desc_accept_button_cb(FL_OBJECT *ob, long data);
extern void groups_menu_cb(FL_OBJECT *ob, long data);
extern void article_menu_cb(FL_OBJECT *ob, long data);
extern void posting_menu_cb(FL_OBJECT *ob, long data);
extern void options_menu_cb(FL_OBJECT *ob, long data);
extern void online_menu_cb(FL_OBJECT *ob, long data);
extern void file_menu_cb(FL_OBJECT *ob, long data);
extern void window_menu_cb(FL_OBJECT *ob, long data);
extern void group_list_browser_cb(FL_OBJECT *ob, long data);
extern void group_list_browser_double_cb(FL_OBJECT *ob, long data);
extern void article_list_browser_cb(FL_OBJECT *ob, long data);
extern void article_list_browser_double_cb(FL_OBJECT *ob, long data);
extern void article_body_input_field_cb(FL_OBJECT *ob, long data);
extern void posting_list_browser_cb(FL_OBJECT *ob, long data);
extern void posting_list_browser_double_cb(FL_OBJECT *ob, long data);
extern void posting_to_input_cb(FL_OBJECT *ob, long data);
extern void posting_subject_input_cb(FL_OBJECT *ob, long data);
extern void posting_attachment_input_cb(FL_OBJECT *ob, long data);
extern void posting_body_editor_cb(FL_OBJECT *ob, long data);
extern void posting_send_now_button_cb(FL_OBJECT *ob, long data);
extern void posting_send_later_button_cb(FL_OBJECT *ob, long data);
extern void posting_attach_file_button_cb(FL_OBJECT *ob, long data);
extern void posting_cancel_button_cb(FL_OBJECT *ob, long data);
extern void posting_summary_button_cb(FL_OBJECT *ob, long data);
extern void posting_add_url_button_cb(FL_OBJECT *ob, long data);
extern void posting_alt_editor_button_cb(FL_OBJECT *ob, long data);
extern void posting_alt_editor_command_input_field_cb(\
FL_OBJECT *ob, long data);
extern void posting_custom_headers_button_cb(FL_OBJECT *ob, long data);
extern void group_list_button_cb(FL_OBJECT *ob, long data);
extern void article_list_button_cb(FL_OBJECT *ob, long data);
extern void article_body_button_cb(FL_OBJECT *ob, long data);
extern int posting_list_button_cb();
extern void next_thread_button_cb(FL_OBJECT *ob, long data);
extern void next_unread_button_cb(FL_OBJECT *ob, long data);
extern void browser_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void browser_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void desc_input_fontsize_up_button_cb(FL_OBJECT *ob, long data);
extern void desc_input_fontsize_down_button_cb(FL_OBJECT *ob, long data);
extern void launch_button_cb(FL_OBJECT *ob, long data);
extern void stop_button_cb(FL_OBJECT *ob, long data);
extern int integer_to_fontsize(int fontsize);
extern int redraw_browser();
extern int show_browser(int browser);
extern int my_command_status_display(char *text);
extern int my_line_status_display(char *text);
extern int my_description_display(char *text);
extern int my_show_alert(char *text1, char *text2, char *text3);
extern int refresh_screen(char *group);
extern int ogettime(char *weekday, char *month, int *monthday, int *hour,\
 int *minute, int *second,  int *year);
extern struct url *lookup_url(char *name);
extern struct url *install_url_at_end_of_list(char *name);

extern int main(int argc, char *argv[]);

/* structure references */
extern struct newsgroup *install_group_in_tree(\
char *name, struct newsgroup **newsgroup);

extern int walk_grouplist_and_display(struct newsgroup *pa);

extern int delete_all_groups_in_tree(struct newsgroup *pa);

extern int highlight_articles(char *group, struct article *pa);
extern int highlight_postings(struct posting *pa);
extern int highlight_urls(struct url *pa);
extern int apply_filter_pa_to(struct filter *pa, char *group, char *article,\
 int encoding, char *attachment, int have_body_flag, int filter_what,\
 int *results);
extern struct urldata *parse_url(char *url);
extern int free_urldata(struct urldata *pa);

/* end structure references */

