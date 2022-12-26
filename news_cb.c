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

#define MAIL_SERVER_CONNECT_RETRIES	5

FL_APPEVENT_CB an_idle_cb;


#define ARTICLE_BODY_TIMEOUT	30

FILE *headerfile;/* must be preserved between calls */
FILE *bodyfile;
FILE *posting_headerfile;
FILE *posting_custom_headers_file;
FILE *posting_bodyfile;
FILE *posting_attachmentfile;
char rx_header_path_filename[TEMP_SIZE];
char rx_body_path_filename[TEMP_SIZE];

int header_file_open_flag;
int body_file_open_flag;
time_t last_body_line_time;
time_t now;
int test_if_article_finished_flag;
int retry_cnt;
time_t last_mail_check_time;



void news_io_read_cb(int socket, long data)
{
char rxbuf[READSIZE];
int a, i;
long li;
char c;
char temp[READSIZE];/* beware of overflow rxbuf=512 + additional text */
char temp2[READSIZE];
char arg0[TEMP_SIZE], arg1[TEMP_SIZE], arg2[TEMP_SIZE], arg3[TEMP_SIZE], arg4[TEMP_SIZE];
static int num_response;/* preserve between calls !!!!! */
static long articles_in_group, first_article, last_article, article_ptr;
static char group[TEMP_SIZE];/* must be preserved between calls */
char path_filename[TEMP_SIZE];
char *ptr;
extern long atol();
int cr_flag;
int delete_flag;
int max_headers_in_group;
int encrypted;

if(debug_flag) fprintf(stdout, "news_io_read_cb(): socket=%d\n", socket);

cr_flag = 0;
/* read from socket */
li = 0;
while(1)
	{
	errno = 0;
	a = read(socket, &c, 1);
	if(a < 0)
		{
		if(errno == EAGAIN) continue;/* try again */
		
		if(debug_flag)
			{
			fprintf(stdout,\
			"\nread failed, connection to server lost errno=%d\n", errno);
			}

		perror("read failed: connection to server lost");

		news_io_cb_error_return(\
		"Read failed, connection to news server lost");

		return;
		}
	if(a == 0)
		{
		news_io_cb_error_return("Conection closed by news server");
		return;
		}

	/* valid byte received */

	bytes_received++;
	
	if(c == 10)/* LF */
		{
/*fprintf(stdout, "FOUND 10\n");*/
		if(cr_flag)
			{
			break;
			}
		continue;/* no store of LF */
		}
	
	if(c == 13)/* CR */
		{
/*fprintf(stdout, "FOUND 13\n");*/
		cr_flag = 1;
		continue;/* no store of CR */
		}
	else cr_flag = 0;	

	rxbuf[li] = c;
	li++;
	if(li >= READSIZE) break;
	}/* end while read all characters */
rxbuf[li] = 0;/* string terminator */
/* It is guaranteed that there are no 13 or 10 in the string */

if(debug_flag)
	{
	fprintf(stdout, "rxbuf:%s\n", rxbuf);
	}
/* process data */

/* 
first test the data stream for reply to the STAT command,
send by the client to figure out if an article with incorrect
termination line was actually finished
*/
if( (! expect_numeric_flag) && test_if_article_finished_flag &&\
(command_in_progress == GET_ARTICLE_BODY) && (body_file_open_flag) )
	{
	if(debug_flag)
		{
		fprintf(stdout, "TIMEOUT IN TEST\n");
		}
	arg0[0] = 0;
	arg1[0] = 0;
	arg2[0] = 0;
	arg3[0] = 0;
	arg4[0] = 0;
	sscanf(rxbuf, "%s %s %s %s %s", arg0, arg1, arg2, arg3, arg4);
	/* 
	this is wat a reply to stat looks like :
	223:  n &lt;a> article retrieved - request text separately 
	*/
	if(atoi(arg0) == 223)
		{
		if(atol(arg1) == acquire_article)
			{
		  	ptr =\
		  	get_header_data(acquire_group, acquire_article, "Message-ID");
			if(debug_flag) fprintf(stdout, "IN TEST ptr=%s\n", ptr);
            /* force close if to many retries */
            /* sometimes Message-ID does not seem to match */
            if(retry_cnt > 1)
            	{
            	free(ptr);
            	ptr = 0;
            	}
            /* if the test cannot be done, force a close bodyfile */
            if(! ptr)
            	{
				if(body_file_open_flag)
					{
					fclose(bodyfile);
					body_file_open_flag = 0;
					add_article_body(acquire_group, acquire_article);

					add_to_new_list(acquire_group, acquire_article);

					test_if_article_finished_flag = 0;

					if(request_by_message_id_flag)
						{
						acquire_body_count--;
						calculate_and_display_tasks();
						if(acquire_body_count == 0)
							{
							if(debug_flag)
								{
								fprintf(stdout,\
"rq by msg_id stat test failed aborting last body received group=%s article=%ld\n",\
								acquire_group, acquire_article);
								}
							command_in_progress = 0;
							expect_numeric_flag = 0;
							return;
							}
						else
							{
							if(debug_flag)
								{
								fprintf(stdout,\
				"rq by msg_id stat test failed looping for next body\n");
								}
							expect_numeric_flag = 1;
							return;
							}
						}/* end if request_by_message_id_flag */

					expect_numeric_flag = 1;
					command_in_progress = 0;
					if(debug_flag)
						{
						fprintf(stdout,\
						"IN TEST body file closed ptr zero, returning\n");
						}
					return;
					}
            	}
			if(strcmp(arg2, ptr) == 0) 
				{
				free(ptr);
				/* 
				we can be reasonable sure at this point the article
				was finished 
				*/
				if(debug_flag)
					{
					fprintf(stdout, "IN TEST match on Message-ID\n");					
					}
				if(body_file_open_flag)
					{
					fclose(bodyfile);
					body_file_open_flag = 0;
					add_article_body(acquire_group, acquire_article);
					
					add_to_new_list(acquire_group, acquire_article);

					test_if_article_finished_flag = 0;

					if(request_by_message_id_flag)
						{
						acquire_body_count--;
						calculate_and_display_tasks();

						if(acquire_body_count == 0)
							{
							if(debug_flag) 
								{
								fprintf(stdout,\
	"rq by msg_id stat test OK, last body received group=%s article=%ld\n",\
								acquire_group, acquire_article);
								}
							command_in_progress = 0;
							expect_numeric_flag = 0;
							return;
							}
						else
							{
							if(debug_flag)
								{
								fprintf(stdout,\
					"rq by msg_id stat test OK, looping for next body\n");
								}
							expect_numeric_flag = 1;
							return;
							}
						}/* end if request_by_message_id_flag */

					expect_numeric_flag = 1;
					command_in_progress = 0;
					if(debug_flag)
						{
						fprintf(stdout,\
						"IN TEST body file closed ptr ok, returning\n");
						}
					return;
					}
				}/* end correct Message-ID */
			else /* no Mesage-ID match */ 
				{
				free(ptr);
				retry_cnt++;
				}
			}/* end correct article number */
		}/* end reply to stat */
	test_if_article_finished_flag = 0;
	/* article has not finished yet (slow connection ?) */
	/* but we will try again */
	last_body_line_time = time(0);
	if(debug_flag)
		{
		fprintf(stdout, "TIMEOUT IN TEST test failed trying again later\n");
		}
	}/* end incorrect article termination processing */

/* check for end of data */
if( (rxbuf[0] == '.') && (rxbuf[1] == 0) )
	{
	if(debug_flag)fprintf(stdout, "end of data detected\n");

	if(command_in_progress == LIST)
		{
		save_groups();
		command_in_progress = 0;
		return;
		}
	else if(command_in_progress == GET_NEW_HEADERS_IN_GROUP)
		{

		if(debug_flag) fprintf(stdout, "CLOSING headerfile\n");

		fclose(headerfile);		
		header_file_open_flag = 0;
		
		/* clear user info */
		my_command_status_display("");
		        
		if(insert_headers_offline_flag)
			{
			/* add article header to the new article header list */
			add_to_new_header_list(group, article_ptr);
			}
		else
			{
			/*
			add article info to articles.dat file in directory group
			reads from headerfile, delete_flag not used here.
			*/
			add_article_header(group, article_ptr, &delete_flag);
			}

		/* add to new article list (used by filters) */
		add_to_new_list(group, article_ptr);

		article_ptr++;
		set_article_ptr(group, article_ptr);
		if(article_ptr > last_article)
			{
			save_subscribed_groups();
			command_in_progress = 0;
			return;
			}
/*
		sprintf(temp, "HEAD %ld\n", article_ptr);
		send_to_news_server(temp);
*/
		expect_numeric_flag = 1;
		if(debug_flag)
			{
			fprintf(stdout, "looping for next header %s \n", temp);
 			}
 		redraw_browser();
		return;/* get next header */
		}
	else if(command_in_progress == GET_ARTICLE_BODY)
 		{
 		/* close body file here */
		
		if(debug_flag)fprintf(stdout, "CLOSING body file\n"); 		

		fclose(bodyfile);
		body_file_open_flag = 0;
		
		/* clear user info */
		my_command_status_display("");
		       
        /* 
        set the have_body_flag in article_headers,dat in dir group,
		and reset the retrieval_flag 
		*/
		add_article_body(acquire_group, acquire_article);
		
		add_to_new_list(acquire_group, acquire_article);
					
 		redraw_browser();

		if(request_by_message_id_flag)
			{
			acquire_body_count--;
			calculate_and_display_tasks();

			if(acquire_body_count == 0)
				{
				if(debug_flag)
					{
					fprintf(stdout,\
			"rq by msg_id NORMAL last body received group=%s article=%ld\n",\
					acquire_group, acquire_article);
					}
				command_in_progress = 0;
				expect_numeric_flag = 0;
				return;
				}
			else
				{
				if(debug_flag)
					{
					fprintf(stdout,\
					"rq by msg_id NORMAL looping for next body\n");
					}
				expect_numeric_flag = 1;
				return;
				}
			}/* end if request_by_message_id_flag */

 		command_in_progress = 0;
 		return;
 		}
	
	}/* end end of data detected */

/* get the numeric response if any */
if(expect_numeric_flag)
	{
	arg0[0] = 0;
	arg1[0] = 0;
	arg2[0] = 0;
	arg3[0] = 0;
	arg4[0] = 0;
				
	sscanf(rxbuf, "%s %s %s %s %s", arg0, arg1, arg2, arg3, arg4);
	if( (! isdigit(arg0[0]) ) || (! isdigit(arg0[1]) ) ||\
	(! isdigit(arg0[2]) ) )
		{
		my_show_alert(\
		"An online error occurred", "expecting numeric but rxbuf is",\
		rxbuf);

		if(debug_flag)
			{
			fprintf(stdout,\
			"EN: An online error occured: expecting numeric but rxbuf=%s\n",\
			rxbuf);
			fprintf(stdout,\
			"EN: acquire_group=%s acquire_article=%ld\n",\
			acquire_group, acquire_article);
			fprintf(stdout,\
			"EN: last_acquire_group=%s\n", last_acquire_group);
			fprintf(stdout,\
			"EN: header_file_open_flag=%d body_file_open_flag=%d\n",\
			header_file_open_flag, body_file_open_flag);
			}
				
		/* 
		IGNORE??????? just waiting for numeric to show up,
		but that MAY be text!
		*/
		news_io_cb_error_return("No numeric");
		return;
		}
	num_response = atoi(arg0);

	/* process the different commands */
	switch(num_response)
		{
		case 100:/* help text follows */
			break;
		case 199: /* debug output */
			break;/* ignore */
		case 200:/* server ready - posting allowed*/
			news_server_mode = POSTING_ALLOWED;
			my_command_status_display("server ready - posting allowed");
			
			/* start authentication if password specified */
			/* see note at case  480 */
			if(strlen(server_username) != 0)
				{
				sprintf(temp, "authinfo user %s\n", server_username);
				send_to_news_server(temp);

				/* now wait for response 381 Waiting for password */ 
				return;
				}
			command_in_progress = 0;
			break;
		case 201:/* server ready - no posting allowed */
			news_server_mode = NO_POSTING_ALLOWED;
			my_command_status_display("No posting allowed on this server");

			/* start authentication if password specified */
			/* see note at case  480 */
			if(strlen(server_username) != 0)
				{
				sprintf(temp, "authinfo user %s\n", server_username);
				send_to_news_server(temp);

				/* now wait for response 381 Waiting for password */ 
				return;
				}
			command_in_progress = 0;
			break;
		case 202: /* slave status noted */
			break;/* ignore */
		case 205: /* closing connection - goodbye! */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 211: /* n f l s group selected */
			/* reply to GROUP */
			/* rxbuf:211 141 71282 71713 sci.astro */
			/*       0   1   2     3     4 */  

			/* Well, any writes MUST now go to this group
			*/
			if(acquire_group) free(acquire_group);
			acquire_group = strsave(arg4);

			/* 
			This is used in commands,c to prevent sending 'GROUP'
			for every body in the group
			*/
			if(last_acquire_group) free(last_acquire_group);
			last_acquire_group = strsave(arg4);
			
			if(command_in_progress == GET_NEW_HEADERS_IN_GROUP)
				{
				articles_in_group = atol(arg1);
				first_article = atol(arg2);
				last_article = atol(arg3);
				sprintf(group, "%s", arg4);
				get_article_ptr(group, &article_ptr);

				if(debug_flag)
					{
					fprintf(stdout,\
					"IN 211: group=%s art=%ld first=%ld last=%ld ptr=%ld\n",\
					group, articles_in_group, first_article, last_article,\
					article_ptr);
					}
				
				/* input parameter check */
				if(articles_in_group < 0) articles_in_group = 0;
				/* microsoft server does this sometimes */
				if(first_article > last_article)
					{
					save_subscribed_groups();
					command_in_progress = 0;

					if(debug_flag)
						{
						fprintf(stdout,\
						"ending 211 because first_article > last_article\n");
						}
					set_new_articles(group, 0);
					refresh_screen(selected_group);
					return;
					}

				/* test if any new articles */
				if(article_ptr > last_article)/* nothing to do */
					{
					save_subscribed_groups();
					command_in_progress = 0;

					if(debug_flag)
						{
						fprintf(stdout,\
						"ending 211 because article_ptr > last_article\n");				
						}
					set_new_articles(group, 0);
					refresh_screen(selected_group);
					return;
					}
				/* test for not been here for a long time */
				if(article_ptr < first_article)
					{
					article_ptr = first_article;
					set_new_articles(group, articles_in_group);
					}

				/* limit maximum headers */
				if(! get_maximum_headers(group, &max_headers_in_group) )
					{
					max_headers_in_group = maximum_headers;/* global */
					}
				/* test for zero maximum headers */
				if(max_headers_in_group == 0)/* nothing to do */
					{
					save_subscribed_groups();
					command_in_progress = 0;

					if(debug_flag)
						{
						fprintf(stdout,\
						"ending 211 because maximum headers is zero\n");
						}
					set_new_articles(group, 0);
					refresh_screen(selected_group);
					return;
					}
				if( (last_article - article_ptr) > max_headers_in_group + 1)
					{
					if(debug_flag)
						{
						fprintf(stdout,\
						"articles_in_group==%ld new articles set to %d\n",\
						articles_in_group, max_headers_in_group); 
						}
					article_ptr = last_article - max_headers_in_group + 1;
					/* and tell user */				
					set_new_articles(group, max_headers_in_group);
					}
				/* now in range */
				set_new_articles(group, last_article - article_ptr + 1);
				
				/* request first header */
/*
				sprintf(temp, "HEAD %ld\n", article_ptr);
*/
				/* 
				the ultimate secret how to speed things up,
				doing it one by one takes sometimes 4 seconds per
				article, for 10 groups and 200 articles per group
				that would take 2000 seconds = 33 minutes.
				Now try this!
				server must have a buffer for maxheaders per group lines.
				*/
				
				/* send the lot */
				for(li = article_ptr; li <= last_article; li++)
					{
					sprintf(temp, "HEAD %ld\n", li);
					send_to_news_server(temp);
					}

				/* expecting numeric */

				if(debug_flag)
					{
					fprintf(stdout,\
					"LEAVING 211 HEAD requested in GET_NEW_HEADERS_IN_GROUP\n");
					}
				refresh_screen(selected_group);
				return;
				break;
				}
			else if(command_in_progress == GET_ARTICLE_BODY)
				{
				/* request body */
				sprintf(temp, "BODY %ld\n", acquire_article);
				send_to_news_server(temp);
				expect_numeric_flag = 1;

				if(debug_flag)
					{
					fprintf(stdout,\
					"LEAVING 211 BODY requested in GET_ARTICLE_BODY\n");
					}
				return;
				break;
				}	
			else
				{
				sprintf(temp,\
				"Incorrect numeric reply (%d) to GROUP (211)", num_response);
				news_io_cb_error_return(temp);
				return;
				}
		case 215: /* list of newsgroups follows */
			if(command_in_progress != LIST)
				{
				sprintf(temp,\
				"Incorrect numeric reply (%d) to LIST (215)",\
				num_response);
				news_io_cb_error_return(temp);
				return;
				}
			/* reply to LIST */
			load_groups();
			mark_all_groups_cancelled();
			to_command_status_display(\
			"Transfering active list, this may take a long time");
			expect_numeric_flag = 0;
			break;
		case 220: /* n &lt;a> article retrieved - head and body follow */
			break;
		case 221: /* n &lt;a> article retrieved - head follows */
			/* reply to HEAD */
			/* example 221  1096 <flipflap> article retrieved - head follows */
			/*         arg0 arg1 arg2 */ 

			if(debug_flag) fprintf(stdout, "RECEIVED 221 head follows\n");			

			article_ptr = atol(arg1);
			sprintf(rx_header_path_filename, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
			home_dir, news_server_name, database_name, acquire_group,\
			article_ptr);

			if(debug_flag)
				{
				fprintf(stdout, "opening file %s\n",\
				rx_header_path_filename);
				}
			headerfile = fopen(rx_header_path_filename, "w");			
			if(! headerfile)
				{
				/* to be done */
				}
			else header_file_open_flag = 1;
				
			/* inform user what is happening */
			sprintf(temp, "Retrieving group %s header %ld",\
			acquire_group, article_ptr);
			my_command_status_display(temp);
			expect_numeric_flag = 0;
			break;
		case 222: /* n <a> article retrieved - body follows */	
			/*
			rxbuf=222 0 body <01bd2153$7ed607c0$0ee6fbcd@fs.roadrunner.nf.net>
			This seems to violate the rfc977.
			*/
			/* 
			reply to BODY
			there are 2 ways to get here, directly if BODY command
			was send by process_command_queue, or indirectly if GROUP
			command was send by process_command_queue and
			command_in_process was GET_ARTICLE_BODY.
			*/

			if(debug_flag) fprintf(stdout, "RECEIVED 222 body follows\n");			

			if(request_by_message_id_flag)
				{
				if(debug_flag)
					{
					fprintf(stdout, "rq by msg_id 222 rxbuf=%s\n", rxbuf);
					}
				/*
				extract the message id <xx> from the rxbuf, server not rfc977, see above.
				*/
				ptr = strstr(rxbuf, "<");
				for(i = 0; i < 500; i++)
					{
					temp[i] = *ptr;
					if(*ptr == 0) break;
					if(*ptr == '>')
						{
						i++;
						temp[i] = 0;
						break;
						}
					ptr++;
					}	
				if(debug_flag)
					{
					fprintf(stdout, "222 extracted msgid=%s\n", temp);
					}
				if(acquire_group) free(acquire_group);
				ptr = get_article_bodies_group(temp, &acquire_article);
				if(! ptr)/* not found, fatal error, abort */
					{
					if(debug_flag)
						{
						fprintf(stdout,\
						"req by msg_id 222 message_id not found, aborting\n");	
						}
					command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
					return;
					}
				acquire_group = strsave(ptr);
				if(debug_flag)
					{
					fprintf(stdout,\
"rq by msg_id 222 group and article found, acquire_group=%s acquire_article=%ld\n",\
					acquire_group, acquire_article);
					}
				}/* end if request_by_message_id_flag */
			
			/* acquire_article = atol(arg1); */
			sprintf(rx_body_path_filename, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
			home_dir, news_server_name, database_name, acquire_group,\
			acquire_article);

			if(debug_flag)
				{
				fprintf(stdout, "opening file %s\n", rx_body_path_filename);
				}
			bodyfile = fopen(rx_body_path_filename, "w");			
			if(! bodyfile)
				{
				/* to be done */
				}
			else
				{
				body_file_open_flag = 1;
				retry_cnt = 0;
				}
			/* inform user what is happening */
			sprintf(temp, "Retrieving group %s body %ld",\
			acquire_group, acquire_article);
			my_command_status_display(temp);
			
			expect_numeric_flag = 0;
			break;
		case 223: /* n &lt;a> article retrieved - request text separately */
			/* reply to STAT */
			break;
		case 230: /* list of new articles by message-id follows	*/
			break;
		case 231: /* list of new newsgroups follows	*/
			break;
		case 235: /* article transferred ok */
			/* reply tio IHAVE, not used */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 240: /* article posted ok */
			if(command_in_progress != POST_ARTICLE)
				{
				sprintf(temp,\
				"Incorrect numeric reply (%d) to command_in_progress %d",\
				num_response, command_in_progress);
				news_io_cb_error_return(temp);
				return;
				}
			set_send_status(posting_article, SEND_OK);
			refresh_screen(selected_group);

			sprintf(temp, "Article %ld posted OK", posting_article);
			my_command_status_display(temp);
			command_in_progress = 0;
 			return;
			break;
		case 281: /* 281 Authentication ok SEE NOTE AT 480 */ 
			/*
			proceed with what we were doing before asked for authentication.
			*/
			command_in_progress = 0;
			return;
			break;
		case 335: /* 
					send article to be transferred.
				 	End with CR-LF>.CR-LF>
					*/
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 340:	/* 
					send article to be posted.
					End with CR-LF>.CR-LF> 
					*/
			if(command_in_progress != POST_ARTICLE)
				{
				sprintf(temp,\
				"Incorrect numeric reply (%d) to command_in_progress %d",\
				num_response, command_in_progress);
				news_io_cb_error_return(temp);
				return;
				}
				
			/* post header */
			sprintf(path_filename, "%s/.NewsFleX/postings/%s/head.%ld",\
			home_dir, postings_database_name, posting_article);
			posting_headerfile = fopen(path_filename, "r");			
			if(! posting_headerfile)
				{
				sprintf(temp,  "Cannot open file %s for read",\
				path_filename);
				my_command_status_display(temp);
				if(debug_flag) fprintf(stdout, "%s", temp);

				my_show_alert(\
				"An online error occured", "cannot open file for read",\
				path_filename);

				command_in_progress = 0;
/* try next command */		
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}
			/* inform user what is happening */
			sprintf(temp, "Posting %s", path_filename);
			my_command_status_display(temp);
			while(1)
				{
				a = readline(posting_headerfile, temp);
				if(a == EOF) break;
				strcat(temp, "\n");
				send_to_news_server(temp);
				}
					
			/* test for global custom_headers enabled */
			if(! get_custom_headers_flag(posting_article, &a) )
				{
				my_command_status_display(\
				"An online error occured, cannot read custom_headers_flag");
				command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}
			if(a & GLOBAL_HEADERS_ENABLED)
				{
				sprintf(path_filename,\
				"%s/.NewsFleX/global_custom_head",\
				home_dir);
				posting_custom_headers_file= fopen(path_filename, "r");			
				if(! posting_custom_headers_file)
					{
					sprintf(temp, "Cannot open file %s for read",\
					path_filename);
					my_command_status_display(temp);

					my_show_alert(\
					"An online error occurred", "cannot open file for read",\
					path_filename);

					command_in_progress = 0;
/*
				news_io_cb_error_return(temp);
*/
					return;
					break;
					}
				/* inform user what is happening */
				sprintf(temp, "Posting %s", path_filename);
				my_command_status_display(temp);
				while(1)
					{
					a = readline(posting_custom_headers_file, temp);
					if(a == EOF) break;
					/* prevent end of header detection if empty line */
					if(strlen(temp) == 0) continue;
					strcat(temp, "\n");
					send_to_news_server(temp);
					}/* end while all characters in custom headers file */
				}/* end if a global custom headers enabled */

			/* test for local custom headers enabled */
			if(! get_custom_headers_flag(posting_article, &a) )
				{
				my_command_status_display(\
				"An online error occured, cannot read custom_headers_flag");
				command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}
			if(a & LOCAL_HEADERS_ENABLED)
				{
				sprintf(path_filename,\
				"%s/.NewsFleX/postings/%s/custom.%ld",\
				home_dir, postings_database_name, posting_article);
				posting_custom_headers_file = fopen(path_filename, "r");			
				if(! posting_custom_headers_file)
					{
					sprintf(temp, "Cannot open file %s for read",\
					path_filename);
					my_command_status_display(temp);
	
					my_show_alert(\
					"An online error occurred", "cannot open file for read",\
					path_filename);

					command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
					return;
					break;
					}
				/* inform user what is happening */
				sprintf(temp, "Posting %s", path_filename);
				my_command_status_display(temp);
				while(1)
					{
					a = readline(posting_custom_headers_file, temp);
					if(a == EOF) break;
					/* prevent end of header detection if empty line */
					if(strlen(temp) == 0) continue;
					strcat(temp, "\n");
					send_to_news_server(temp);
					}/* end while all characters in custom headers file */
				}/* end if local custom headers enabled */

			/* insert a blank line at the end of the header */
			send_to_news_server("\n");

			/* post body */
			sprintf(path_filename, "%s/.NewsFleX/postings/%s/body.%ld",\
			home_dir, postings_database_name, posting_article);
			posting_bodyfile = fopen(path_filename, "r");			
			if(! posting_bodyfile)
				{
				sprintf(temp, "Cannot open file %s for read",\
				path_filename);
				my_command_status_display(temp);

				my_show_alert(\
				"An online error occurred", "cannot open file for read",\
				path_filename);

				command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}

			/* test if encrypted */
			if(! get_posting_encoding (posting_article, &encrypted) )
				{
				my_show_alert(\
				"An online error occurred",\
				"cannot get posting encoding",\
				path_filename);

				command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}

			/* inform user what is happening */
			sprintf(temp, "Posting %s", path_filename);
			my_command_status_display(temp);
			while(1)
				{
				a = readline(posting_bodyfile, temp);
				if(a == EOF) break;

				/* if not encrypted expand macros */
				if(! encrypted)
					{
					ptr = expand_posting_macros(temp);
					if(! ptr)
						{
						/* fatal, malloc failed */
/*
					news_io_cb_error_return(temp);
*/
						command_in_progress = 0;
						return;
						}
					strcpy(temp, ptr);
					free(ptr);
					}
				
				/* 
				if the first character of a line is a period, quote it
				with another period 
				*/
				if(temp[0] == '.') sprintf(temp2, ".%s\n", temp);
				else sprintf(temp2, "%s\n", temp);
				send_to_news_server(temp2);
				}

			/* test for attachment */
			if(! test_if_posting_has_attachment(posting_article, &a) )
				{
				my_command_status_display(\
				"An online error occured, cannot read attachment state");
				command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
				return;
				break;
				}
			/* if attachment send attachment */
			if(a)
				{
				sprintf(path_filename,\
				"%s/.NewsFleX/postings/%s/attachment.%ld",\
				home_dir, postings_database_name, posting_article);
				posting_attachmentfile= fopen(path_filename, "r");			
				if(! posting_attachmentfile)
					{
					sprintf(temp, "Cannot open file %s for read",\
					path_filename);

					my_show_alert(\
					"An online error occurred", "cannot open file for read",\
					path_filename);

					my_command_status_display(temp);
					command_in_progress = 0;
/*
			news_io_cb_error_return(temp);
*/
					return;
					break;
					}
				/* inform user what is happening */
				sprintf(temp, "Posting %s", path_filename);
				my_command_status_display(temp);
				while(1)
					{
					a = readline(posting_attachmentfile, temp);
					if(a == EOF) break;
					/* 
					if the first character of a line is a period, quote it
					with another period 
					*/
					if(temp[0] == '.') sprintf(temp2, ".%s\n", temp);
					else sprintf(temp2, "%s\n", temp);
					send_to_news_server(temp2);
					}/* end while all characters in attachment */
				}/* end if an attachment present) */

			/* send end of text */
			
/*
			if(temp2[strlen(temp2) - 1] != '\n') send_to_news_server("\n");
			send_to_news_server("\n");
*/
			send_to_news_server(".\n");			
			expect_numeric_flag = 1;
			break;
		case 381:/* 381 Waiting for password SEE NOTE at 480 */

			sprintf(temp, "authinfo pass %s\n", server_password);
			send_to_news_server(temp);

			/* now wait for 281 Authentication ok */
			return;
			
			break;
		case 400: /* service discontinued */
			news_server_mode = OUT_OF_ORDER;
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 411: /* no such news group */
			/* 
			cancel command, but do not go offline,
			there may be more and valid ones in queue
			*/
			sprintf(temp,\
			"An online error occurred, group %s, server said: %s",\
			acquire_group, rxbuf);
			to_command_status_display(temp);

			if(debug_flag)
				{
				fprintf(stdout, "%s\n", temp);
				}

			/* the absence of group NewsFleX.help is not considered an error */
			if(strcmp(acquire_group, "NewsFleX.help") != 0)
				{
				/* write to error log */
				sprintf(temp, "An online error occurred, group %s %s",\
				acquire_group, rxbuf);
				to_error_log(temp);

				/*
				this would stop the online process, better get what is there
				*/
/*
				my_show_alert(\
				"An online error occurred, group", acquire_group, rxbuf);
*/
				}

/*			expect_numeric_flag = 0; */
			command_in_progress = 0;
			break;
		case 412: /* no newsgroup has been selected */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 420: /* no current article has been selected */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 421: /* no next article in this group */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 422: /* no previous article in this group */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 423: /* no such article number in this group */
			sprintf(temp, "no such article number in this group");
			if(debug_flag) fprintf(stdout, "%s\n", temp);
		case 430: /* no such article found */
				/* rxbuf:430 No article by message-id <wqqwe?>, sorry. */
			if(command_in_progress == GET_NEW_HEADERS_IN_GROUP)
				{
				/* there may be more articles, try the next one */
				if(debug_flag)
					{
					fprintf(stdout,\
"error: num_response %d: no article %ld in this group, trying next one\n",\
					num_response, article_ptr);
					}
/*
				if( article_ptr <= last_article) article_ptr++;
				else
*/
				if(article_ptr >= last_article)
					{
					article_ptr++;/* scip error one */
					set_article_ptr(group, article_ptr);
					command_in_progress = 0;
					return;
					}
/*
				sprintf(temp, "HEAD %ld\n", article_ptr);
				send_to_news_server(temp);
*/
				expect_numeric_flag = 1;

				if(debug_flag)
					{
					fprintf(stdout, "looping for next header %s \n", temp);
					}
				return;/* get next header */
				}
			else if(command_in_progress == GET_ARTICLE_BODY)
				{
				/* cancel this one */
				if(request_by_message_id_flag)
					{
					/*
					Unfortunatly, according to rfc977, 430 does not tell us WHICH msg id
					failed, but my server says see above.
					*/
					if(debug_flag)
						{
						fprintf(stdout,\
						"rq by msg_id error 430, no such article found\n");
						}
					/*
					extract the message id <xx> from the rxbuf, server not rfc977, see above.
					*/
					ptr = strstr(rxbuf, "<");
					if(! ptr)/* scip body */
						{
						if(debug_flag)
							{
							fprintf(stdout, "rq by msg_id no '<' found\n");
							}
						acquire_body_count--;
						calculate_and_display_tasks();
						if(acquire_body_count == 0)
							{
							if(debug_flag)
								{
								fprintf(stdout,\
					"rq by msg_id 430 no '<' last body, ending command\n");
								}
							command_in_progress = 0;
							return;
							}
						else
							{
							if(debug_flag)
								{
								fprintf(stdout,\
						"rq by msg_id 430 no '<' looping for next body\n");
								}
							expect_numeric_flag = 1;
							return;
							}
						}/* end if ! ptr (scip body because no '<' found) */
					for(i = 0; i < READSIZE - 1; i++)
						{
						temp[i] = *ptr;
						if(*ptr == 0) break;
						if(*ptr == '>')
							{
							i++;
							/*temp[i] = 0;*/
							break;
							}
						ptr++;
						}	
					temp[i] = 0;/* string termination */
					if(debug_flag)
						{
						fprintf(stdout,\
						"rq by msg_id 430 extracted msgid=%s\n", temp);
						}
					if(acquire_group) free(acquire_group);
					ptr = get_article_bodies_group(temp, &acquire_article);
					if(! ptr)/* not found, fatal error, abort */
						{
						if(debug_flag)
							{
							fprintf(stdout,\
							"rq by msg_id  430 message_id not found, aborting\n");	
							}
						command_in_progress = 0;
						return;
						}
					acquire_group = strsave(ptr);
					if(debug_flag)
						{
						fprintf(stdout,\
"rq by msg_id 430 group and article found, acquire_group=%s acquire_article=%ld\n",\
						acquire_group, acquire_article);
						}	
					set_article_no_longer_availeble_flag(acquire_group, acquire_article);
				
					redraw_browser();

					acquire_body_count--;
					calculate_and_display_tasks();
					if(acquire_body_count == 0)
						{
						if(debug_flag)
							{
							fprintf(stdout,\
							"rq by msg_id 430 last body, ending command\n");
							}
						command_in_progress = 0;
						return;
						}
					else
						{
						if(debug_flag)
							{
							fprintf(stdout,\
							"rq by msg_id 430 looping for next body\n");
							}
						expect_numeric_flag = 1;
						return;
						}
					}/* end if request_by_message_id_flag */

				set_article_no_longer_availeble_flag(\
				acquire_group, acquire_article);
				
				redraw_browser();

				command_in_progress = 0;
				return;
				}
			break;
		case 435: /* article not wanted - do not send it */
			set_send_status(posting_article, SEND_FAILED);
			sprintf(temp, "article not wanted - do not send it");

			news_io_cb_error_return(temp);
			return;
			break;
		case 436: /* transfer failed - try again later */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 437: /* article rejected - do not try again. */
			set_send_status(posting_article, SEND_FAILED);
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 440: /* posting not allowed */
			set_send_status(posting_article, SEND_FAILED);
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 441: /* posting failed */
			set_send_status(posting_article, SEND_FAILED);
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 480: /* 480 Logon Required */
			/*
			this is not in the rfc977 I have, experimentally from
			news.hetnet.nl, do a help, it will say:
			authinfo [user|pass|generic|transact] <data>
			This evalutes to:

			480 Logon Required 
			authinfo user username
			381 Waiting for password
			authinfo pass password
			281 Authentication ok
			*/
			
			sprintf(temp, "authinfo user %s\n", server_username);
			send_to_news_server(temp);
			
			/* now wait for response 381 Waiting for password */ 
			return;

			break;
		case 500: /* command not recognized */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 501: /* command syntax error */
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 502: /* access restriction or permission denied */
			set_send_status(posting_article, SEND_FAILED);
			news_io_cb_error_return(rxbuf);
			return;
			break;
		case 503: /* program fault - command not performed */
			set_send_status(posting_article, SEND_FAILED);
			news_io_cb_error_return(rxbuf);
			return;
			break;
		default: /* some unknown code */
			sprintf(temp, "unknowm numeric response %d from server\n",\
			num_response);
			news_io_cb_error_return(temp);
			return;
		}/* end switch num_response */	
	return;
	}/* end expect_numeric_flag */

/*
not numeric, but numeric command in progress (num_response is a static )
*/
switch(num_response)
	{
	case 215: /* list of newsgroups folows */
		add_group(rxbuf);
		return;
		break;
	case 221: /* reading header, implied  */
		/* store header lines */

		if(debug_flag)fprintf(stdout, "HDR LI=%s\n", rxbuf);		

		fprintf(headerfile, "%s\n", rxbuf);

		/* get next line if not header end */
		return;
		break;
	case 222: /* reading body */
		/* store body lines */

		if(debug_flag)fprintf(stdout, "BODY LI=%s\n", rxbuf);		

		/* 
		if the first 2 characters in a line are periods, it was
		a quoted period 
		*/
		if(strstr(rxbuf, "..") == rxbuf)
			{
			fprintf(bodyfile, "%s\n", rxbuf + 1);/* scip the first dot */
			}
		else fprintf(bodyfile, "%s\n", rxbuf);
		last_body_line_time = time(0);
			
		/* get next line if not body end */
		return;
		break;
	}/* end switch num_response */		
}/* end function news_io_read_cb */


int news_io_cb_error_return(char *text)
{
if(header_file_open_flag) 
	{
	fclose(headerfile);
	/* file may be incomplete, delete */
	unlink(rx_header_path_filename);
	}
if(body_file_open_flag)
	{
	fclose(bodyfile);
	/* file may be incomplete, delete */
	unlink(rx_body_path_filename);
	}

fl_remove_io_callback(socketfd, FL_READ, (FL_IO_CALLBACK) news_io_read_cb);

send_to_news_server("QUIT\n");

close(socketfd);

news_server_status = DISCONNECTED;

/*
this will only disconnect if ALL server connections are closed,
the function check for news- mail- http- ftp- server status
If the disconnect fails, process_command_queue will try again.
*/
if(auto_disconnect_from_net_flag) auto_disconnect_from_net();

my_command_status_display(text);
to_error_log(text);

command_in_progress = 0;

refresh_screen("");

if(insert_headers_offline_flag)
	{
	add_new_headers_from_list("*");/* this will clear the list */
	}

set_new_data_flag(1);
 		
if(filters_enabled_flag) set_all_filter_flags_for_new_articles();

return(1);
}/* end function news_io_cb_error_return */


int idle_cb(XEvent *ev, void *data)
{
long diff;
char temp[TEMP_SIZE];
char weekday[TEMP_SIZE], month[TEMP_SIZE];
int monthday, hour, minute, second, year;
int get_headers_hour, get_headers_minute;
float receive_speed;
float transmit_speed;
unsigned long seconds_elapsed;
float kilobytes_received;
float kilobytes_send;
static float old_kilobytes_received;/* prevent flashing display offline */
static float old_kilobytes_send;
static unsigned long online_seconds;/* preserve after going offline */
int status;

/* this will kill any zombies left when viewer is destroyed */
waitpid (WAIT_ANY, &status, WNOHANG);

if(audio_alarm_flag)
	{
	fl_ringbell(100);
	audio_alarm_flag++;
	if(audio_alarm_flag == 65535) audio_alarm_flag = 0;
	/* beep,beep,,,,,beep,.,beep,,,,,beep.......... */
	}

allocate_space();

/* set the color of the stop button */
set_stop_button_color();

set_offline_indicator();

/*
test for new mail in /var/spool/mail/user when specified time elapses,
and update mail display.
*/
if(check_incoming_mail_periodically_flag)
	{
	if( (time(0) - last_mail_check_time) >= local_mail_check_interval)
		{
		check_mail_local();
		last_mail_check_time = time(0);
		}
	}

/* 
handle if article did not have the required termination line,
a line with only a period on it
the program will hang if the termination line is not received
*/
if(body_file_open_flag)
	{
	now = time(0);
	diff = (long) now - last_body_line_time;
	if(diff == ARTICLE_BODY_TIMEOUT)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"in idle_cb(): * T I ME  O U T * article body, seconds=%ld\n",\
			diff); 
			}
		/* 
		make sure group command is only send once, and not several times 
		whithin this second 
		*/
		last_body_line_time--;
	
		test_if_article_finished_flag = 1;
		
		/*	
		This will indicate that we check the data stream for a reply 
		to the stat command, even though we do not realy expect numeric.
		This will only fail if the article did contain exactly the stat
		reply at this point.
		This is unlikely (Tjernobyl) since the author of the article
		could not know the article id at the time of writing.
		However the server COULD accidently send it.
		In that case the article would be corrupted.
		*/ 
		sprintf(temp, "STAT %ld\n", acquire_article);
		send_to_news_server(temp);
		}
	}/* end body_file_open_flag */

/* show the transfer speed */
seconds_elapsed = time(0) - online_timer;
/* avoid the divide by zero exeption :) */
if(seconds_elapsed)
	{
	if(news_server_status != DISCONNECTED)/* else speed shown will decrease */
		{
		receive_speed = (float) bytes_received / seconds_elapsed;
		transmit_speed = (float) bytes_send / seconds_elapsed;
		/* clear display */
		fl_set_object_label(fdui -> line_speed_display, "");
/*		sprintf(temp, "%.2f K / S", receive_speed / 1000.0);*/
		sprintf(temp, "%.2f %.2f",\
		receive_speed / 1000.0, transmit_speed / 1000.0);
		fl_set_object_label(fdui -> line_speed_display, temp);
 		online_seconds = seconds_elapsed;
 		old_kilobytes_received = -1;/* even if same amount display */
 		old_kilobytes_send = -1;
 		}
 	else
 		{
		kilobytes_received = (float) bytes_received / 1000.0;
		kilobytes_send = (float) bytes_send / 1000.0;
		if( (kilobytes_received != old_kilobytes_received) ||\
		(kilobytes_send != old_kilobytes_send) )
			{
			/* clear display */
			fl_set_object_label(fdui -> line_speed_display, "");
			/* not much space in display */
/*
			if(kilobytes_received < 1)
				{
				sprintf(temp, "%.2f %ld",\
				kilobytes_received, online_seconds);
				}
			else if(kilobytes_received < 10)
				{
				sprintf(temp, "%.1f %ld",\
				kilobytes_received, online_seconds);
				}	
			else
*/
				{
				sprintf(temp, "%.0f %.0f %ld",\
				kilobytes_received, kilobytes_send, online_seconds);
				}	
			fl_set_object_label(fdui -> line_speed_display, temp);
			old_kilobytes_received = kilobytes_received;
			old_kilobytes_send = kilobytes_send;
			}
 		}
 	}/* end if seconds_elapsed */
 	
if(ask_question_flag == 1)
	{
	fl_set_form_position(fd_NewsFleX -> question,\
	-fd_NewsFleX->question->w - 50,\
	0);
	fl_show_form(fd_NewsFleX -> question,\
	FL_PLACE_GEOMETRY, FL_NOBORDER,"");
/*	FL_PLACE_CENTER, FL_NOBORDER, "");*/
	fl_set_object_label(fdui -> question_text, question_string);
	}
if(ask_question_flag == 0)
	{
	fl_hide_form(fd_NewsFleX -> question);
	ask_question_flag = -1;
	}

if(go_search_next_article_from_idle_cb_flag)
	{
	search_next_article(search_groups_keywords);/* resets flag */
	}

if(command_status_string_flag)
	{
	fl_set_object_label(fdui -> command_status_display,\
	command_status_string);
	command_status_string_flag = 0;
	}

if(line_status_string_flag)
	{
	fl_set_object_label(fdui -> line_status_display,\
	line_status_string);
	line_status_string_flag = 0;
	}

/* test if time to get new headers in subscribed groups */
if(auto_get_new_headers_flag && (! time_match_flag) )
	{
	/* get present date and time */
	ogettime(weekday, month, &monthday, &hour, &minute, &second, &year);
 	
 	/* parse the user specified get_new_headers_time */
	sscanf(get_new_headers_time, "%d:%d",\
	&get_headers_hour, &get_headers_minute);
	if(debug_flag)
		{
		fprintf(stdout, "ref hour=%d ref min=%d hour=%d min=%d\n",\
		get_headers_hour, get_headers_minute, hour, minute);
		}
 	if( (hour == get_headers_hour) && (minute == get_headers_minute) &&\
 	/* if not back here within one minute (fast connection or error) */
 	(time(0) - start_time > 60) )
 		{
		if(debug_flag)
			{
			fprintf(stdout, "in idle_cb: match on %s\n",\
			get_new_headers_time);
 			}

 		time_match_flag = 1;
		process_command_queue_error_flag = 0;

		/* this is executed first */
		/* if we crash here, not so bad, worse if after all headers are in */
		add_marked_urls_to_command_queue();
				
		/* post marked postings */
	    posting_source = ONLINE_MENU;
		add_marked_posting_bodies_to_command_queue();
	
		/* get marked article bodies */
		add_marked_article_bodies_in_subscribed_groups_to_command_queue();

		/* this is executed last */
		/* get new headers (now we should see our own postings). */
		add_new_headers_in_subscribed_groups_to_command_queue();

 		/* 
 		Just before NewsFleX goes offline (when ALL new headers have been
 		received, and the command queue is empty), 
 		if there were headers marked for retrieval because of threads,
 		it will do an other:
 		add_marked_article_bodies_in_subscribed_groups_to_command_queue()
 		*/
 		start_time = time(0);
 		return(1); /* not really needed here */
 		}
 	}/* end if auto_get_new_headers_flag */
 	
if(command_in_progress) return(1);/* wait for io_callback to process command */

if(process_command_queue_error_flag) return(1);/* disconnected */

if(debug_flag)fprintf(stdout, "Calling process_command_queue\n");

if(! process_command_queue() )
	{
	process_command_queue_error_flag = 1; /* que empty or error */
	time_match_flag = 0;
	return(1);
	}

return(1);
}/*end function idle_cb */

