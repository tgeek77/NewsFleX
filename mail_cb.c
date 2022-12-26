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

#define QUIT_SEND		0
#define RSET_SEND		1
#define HELO_SEND		2
#define FROM_SEND		3
#define RCPT_SEND		4
#define DATA_SEND		5
#define MESSAGE_SEND	6

FILE *header_filefd;/* must be preserved between calls */
FILE *body_filefd;
FILE *custom_headers_filefd;
FILE *attachment_filefd;

time_t last_body_line_time;
time_t now;
int mail_mode;


/*         
211 System status, or system help reply
214 Help message
     [Information on how to use the receiver or the meaning of a
     particular non-standard command; this reply is useful only
     to the human user]
220 <domain> Service ready
221 <domain> Service closing transmission channel
250 Requested mail action okay, completed
251 User not local; will forward to <forward-path>
 
354 Start mail input; end with <CRLF>.<CRLF>
 
421 <domain> Service not available,
     closing transmission channel
     [This may be a reply to any command if the service knows it
     must shut down]
450 Requested mail action not taken: mailbox unavailable
     [E.g., mailbox busy]
451 Requested action aborted: local error in processing
452 Requested action not taken: insufficient system storage
 
500 Syntax error, command unrecognized
     [This may include errors such as command line too long]
501 Syntax error in parameters or arguments
502 Command not implemented
503 Bad sequence of commands
504 Command parameter not implemented
550 Requested action not taken: mailbox unavailable
     [E.g., mailbox not found, no access]
551 User not local; please try <forward-path>
552 Requested mail action aborted: exceeded storage allocation
553 Requested action not taken: mailbox name not allowed
     [E.g., mailbox syntax incorrect]
554 Transaction failed
*/


void mail_io_read_cb(int socket, long data)
{
char rxbuf[READSIZE];
int a, i;
long li;
char c;
char temp[READSIZE];/* beware of overflow rxbuf=512 + additional text */
char temp2[READSIZE];
char arg0[TEMP_SIZE];
static int num_response;/* preserve between calls !!!!! */
char path_filename[TEMP_SIZE];
int cr_flag;
char hostname[TEMP_SIZE];
int custom_headers_flag;
char *recipient;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "mail_io_read_cb(): socket=%d\n", socket);
	}

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

		mail_io_cb_error_return("read failed, connection to mail server lost");
		}
	if(a == 0)
		{
		sprintf(temp, "Connection closed by server (EOF)");
		my_command_status_display(temp);
		if(debug_flag) fprintf(stdout, "%s\n", temp);

		fl_remove_io_callback(mail_socketfd,\
		FL_READ, (FL_IO_CALLBACK) mail_io_read_cb);

		/* close the server socket */
		close(socket);

		mail_server_status = DISCONNECTED;

		command_in_progress = 0;
		return;
		}

	/* valid byte received */

	bytes_received++;
	
	if(c == 10)/* LF */
		{
		if(cr_flag)
			{
			break;
			}
		continue;/* no store of LF */
		}
	
	if(c == 13)/* CR */
		{
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

/* get the numeric response if any */

arg0[0] = 0;
				
sscanf(rxbuf, "%s", arg0);
if( (! isdigit(arg0[0]) ) || (! isdigit(arg0[1]) ) || (! isdigit(arg0[2]) ) )
	{
	my_show_alert(\
	"An online error occurred", "expecting numeric but rxbuf is",\
	rxbuf);

	if(debug_flag)
		{
		fprintf(stdout,\
		"EN: An online error occured: expecting numeric but rxbuf=%s\n",\
		rxbuf);
		}
				
	mail_io_cb_error_return("no numeric responce from mail server");
	return;
	}

if(rxbuf[3] == '-') return; /* 220-something, multi line replies */

num_response = atoi(arg0);

/* process the different commands */
switch(num_response)
	{
	case 211:	/* System status, or system help reply */
		return; /* ignore */
		break;
	case 214:	/*
				Help message
				[Information on how to use the receiver or the meaning of
				a particular non-standard command; this reply is useful
				only to the human user]
				*/
		return; /* ignore */
		break;

	case 220:	/* 220 <domain> Service ready */
		/*
		220-panteltje.pi.net Sendmail 8.6.12/8.6.9 ready at Wed, 4 Feb 1998 16:20:54 +0100
		220 ESMTP spoken here
		*/	
			
		if(gethostname(hostname, TEMP_SIZE) == -1)/* 0 is succes */
			{
			/* exit error */
			mail_io_cb_error_return("cannot get host name");
			}
		if(mail_mode == 0)
			{
			sprintf(temp, "HELO %s\n", hostname);
			if(! send_to_mail_server(temp) )
				{
				mail_io_cb_error_return("could not send to mail server");
				}
			mail_mode = HELO_SEND;
			return;
			}
		return;
		break;
	case 221:	/* <domain> Service closing transmission channel */
		mail_io_cb_error_return("221 <domain> Service closing transmission channel");
		break;
	case 250:	/* repond to helo or mail or message send (\n.\n) */
				/* 250 Requested mail action okay, completed */
	case 251:	/* User not local; will forward to <forward-path> */
		if(mail_mode == HELO_SEND)
			{
			sprintf(temp2, "mailing %ld from %s DO NOT QUIT",\
			mail_posting, user_email_address);
			to_command_status_display(temp2);

			sprintf(temp, "MAIL From: <%s>\n", user_email_address);
			if(!send_to_mail_server(temp) )
				{
				mail_io_cb_error_return("cannot send to mail server");
				}
			mail_mode = FROM_SEND;
			return;
			}
		if(mail_mode == FROM_SEND)
			{
			recipient = (char *) get_header_data(\
			"postings", mail_posting, "To:");
			if(! recipient)
				{
				mail_io_cb_error_return("cannot read recipient from header file");
				}

			/* extract email field (containing '@') */
			ptr = strstr(recipient, "@");
			if(! ptr) mail_io_cb_error_return("Invalid email address");

			/* move backwards until space or < or start */
			while(1)
				{
				if(ptr == recipient)
					{
					break;/* start found, @ in first field */
					}
				if(*ptr == ' ')
					{
					ptr++;/* point to first valid char in field */
					break;
					}
				/* detect '<' will add later */
				if(*ptr == '<')
					{
					ptr++;/* point to first valid char in field */
					break;
					}
				ptr--;
				}/* end while move backwards */
			/* ptr points to first char in field */

			/* copy field until space or > or end */			
			for(i = 0; i < READSIZE; i++)
				{
				/* detect space */
				if(*ptr == ' ')
					{
					temp[i] = 0;
					break;
					}

				/* detect '>', will add later */
				if(*ptr == '>')
					{
					temp[i] = 0;
					break;
					}
				
				temp[i] = *ptr;

				/* detect end */
				if(*ptr == 0)
					{
					break;
					}
				
				ptr++;
				}/* end while move backwards */

			/*
			now  Jan Panteltje <root@localhost>  will be converted to:
			root@localhost  in string temp.
			*/
			
			sprintf(temp2, "mailing %ld to %s DO NOT QUIT",\
			mail_posting, temp);
			to_command_status_display(temp2);

			sprintf(temp2, "RCPT To: <%s>\n", temp);/*recipient);*/
			if(!send_to_mail_server(temp2) )
				{
				mail_io_cb_error_return("cannot send to mail server");
				}
			mail_mode = RCPT_SEND;
			return;
			}	
		if(mail_mode == RCPT_SEND)	
			{
			sprintf(temp, "DATA\n");
			if(!send_to_mail_server(temp) )
				{
				mail_io_cb_error_return("cannot send to mail server");
				}
			mail_mode = DATA_SEND;
			return;
			}		
		if(mail_mode == MESSAGE_SEND)
			{
			if(! send_to_mail_server("QUIT\n") ) 
				{
				mail_io_cb_error_return("cannot send to mail server");
				}
			mail_mode = QUIT_SEND;
			fl_remove_io_callback(mail_socketfd,\
			FL_READ, (FL_IO_CALLBACK) mail_io_read_cb);
			close(mail_socketfd);

			/* do not hang around to wait for confirmation of quit */
			mail_server_status = DISCONNECTED;
			
			set_send_status(mail_posting, SEND_OK);
				
			refresh_screen("");

			command_in_progress = 0;

			my_command_status_display("");
				
			return;
			}
		break;
	case 354:	/* 354 Start mail input; end with <CRLF>.<CRLF> */
		if(mail_mode != DATA_SEND)
			{
			/* problem */
			mail_io_cb_error_return("num responce 354, but data command was not send");
			return;
			}

		/* open header file */
		sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
		home_dir, postings_database_name, mail_posting);
		header_filefd = fopen(temp, "r");
		if(! header_filefd)
			{
			sprintf(temp2, "Cannot open file %s for read", temp);
			mail_io_cb_error_return(temp2);
			return;
			}
		while(1)
			{
			a = readline(header_filefd, temp);
			if(a == EOF) break;

			/* If a From line, replace the original From with Reply-To */
			if(strstr(temp, "From:") == temp)
				{
				sprintf(temp2, "Reply-To: %s", strstr(temp, ":") + 2); 
				strcpy(temp, temp2);
				}	
	
			sprintf(temp2, "%s\n", temp);
			if(! send_to_mail_server(temp2) )
				{
				fclose(header_filefd);
				mail_io_cb_error_return("cannot send to mail server");
				return;
				}	
			}/* end while read header file */

		/* get custom_headers_flag */
		if(! get_custom_headers_flag(mail_posting, &custom_headers_flag) )
			{
			sprintf(temp, "Cannot get custom headers flag for mail_posting %ld",\
			mail_posting);
			mail_io_cb_error_return(temp);
			return;
			}

		/* test for global custom headers enabled */
		if(custom_headers_flag & GLOBAL_HEADERS_ENABLED)
			{
			sprintf(path_filename, "%s/.NewsFleX/global_custom_head",\
			home_dir);
			custom_headers_filefd = fopen(path_filename, "r");			
			if(! custom_headers_filefd)
				{
				sprintf(temp, "Cannot open file %s for read", path_filename);
				mail_io_cb_error_return(temp);
				return;
				}
			/* inform user what is happening */
			sprintf(temp, "Posting %s", path_filename);
			my_command_status_display(temp);
			while(1)
				{
				a = readline(custom_headers_filefd, temp);
				if(a == EOF) break;

				/* prevent end of header detection if empty line */
				if(strlen(temp) == 0) continue;
				if(debug_flag)
					{
					fprintf(stdout, "post_email():temp=%s\n", temp);
					}
				sprintf(temp2, "%s\n", temp);
				if(!send_to_mail_server(temp2) )
					{
					fclose(custom_headers_filefd);
					mail_io_cb_error_return("cannot send to mail server");
					return;
					}
				}/* end while all characters in custom headers file */
			}/* end if a global custom headers enabled */

		/* test for local custom headers enabled */
		if(custom_headers_flag & LOCAL_HEADERS_ENABLED)
			{
			sprintf(path_filename, "%s/.NewsFleX/postings/%s/custom.%ld",\
			home_dir, postings_database_name, mail_posting);
			custom_headers_filefd = fopen(path_filename, "r");			
			if(! custom_headers_filefd)
				{
				sprintf(temp, "Cannot open file %s for read", path_filename);
				mail_io_cb_error_return(temp);
				return;
				}
			/* inform user what is happening */
			sprintf(temp, "Posting %s", path_filename);
			my_command_status_display(temp);
			while(1)
				{
				a = readline(custom_headers_filefd, temp);
				if(a == EOF) break;
				/* prevent end of header detection if empty line */
				if(strlen(temp) == 0) continue;
				if(debug_flag)
					{
					fprintf(stdout, "post_email():temp=%s\n", temp);
					}
				sprintf(temp2, "%s\n", temp);
				if(!send_to_mail_server(temp2) )
					{
					fclose(custom_headers_filefd);
					mail_io_cb_error_return("cannot send to mail server");
					return;
					}
				}/* end while all characters in custom headers file */
			}/* end if local custom headers enabled */
			
		/* a blank line separates header and body */
		if(!send_to_mail_server("\n") )
			{
			mail_io_cb_error_return("cannot send to mail server");
			return;
			}

		/* open body file */
		sprintf(path_filename, "%s/.NewsFleX/postings/%s/body.%ld",\
		home_dir, postings_database_name, mail_posting);
		body_filefd = fopen(path_filename, "r");
		if(! body_filefd)
			{
			sprintf(temp2, "Cannot open file %s for read", path_filename);
			mail_io_cb_error_return(temp2);
			return;
			}
		/* inform user what is happening */
		sprintf(temp, "Posting %s", path_filename);
		my_command_status_display(temp);
		while(1)
			{
			a = readline(body_filefd, temp);
			if(a == EOF) break;
			if(debug_flag)
				{
				fprintf(stdout, "post_email():temp=%s\n", temp);
				}
			/*
			if the first character of a line is a period, quote it
			with another period
			*/
			if(temp[0] == '.') sprintf(temp2, ".%s\n", temp);
			else sprintf(temp2, "%s\n", temp);
			if(!send_to_mail_server(temp2) )
				{
				fclose(body_filefd);
				mail_io_cb_error_return("cannot send to mail server");
				return;
				}
			}/* end while body file */

		/* test if there is an attachment */
		if(! test_if_posting_has_attachment(mail_posting, &a) )
			{
			mail_io_cb_error_return("cannot test if posting has attachment");
			return;
			}
		if(a)
			{
			/* Try to open the attachment file attachment.%ld */
			sprintf(path_filename,\
			"%s/.NewsFleX/postings/%s/attachment.%ld",\
			home_dir, postings_database_name, mail_posting);
			attachment_filefd = fopen(path_filename, "r");
			if(! attachment_filefd)
				{
				sprintf(temp2,\
				"Cannot open file %s for read", path_filename);
				mail_io_cb_error_return(temp2);
				return;
				}
			/* inform user what is happening */
			sprintf(temp, "Posting %s", path_filename);
			my_command_status_display(temp);
			while(1)
				{
				a = readline(attachment_filefd, temp);
				if(a == EOF) break;
				if(debug_flag)
					{
					fprintf(stdout, "post_email():temp=%s\n", temp);
					}
				/* 
				if the first character of a line is a period, quote it
				with another period
				*/
				if(temp[0] == '.') sprintf(temp2, ".%s\n", temp);
				else sprintf(temp2, "%s\n", temp);
				if(!send_to_mail_server(temp2) )
					{
					fclose(attachment_filefd);
					mail_io_cb_error_return("cannot send to mail server");
					return;
					}

				}/* end while attachment file */
			}/* end strlen pa -> attachment not 0 */

		/* 
		Note: last line attachment not guaranteed to have a \n, up to
		mpack
		*/

		/* a single period on a line indicates end of text */
		if(!send_to_mail_server(".\n") )
			{
			fclose(custom_headers_filefd);
			mail_io_cb_error_return("cannot send to mail server");
			return;
			}

		mail_mode = MESSAGE_SEND;
			
		return;
		break;

	case 421:	/*
				<domain> Service not available,
			   	closing transmission channel
			   	[This may be a reply to any command if the service knows
			   	it must shut down]
			   	*/
		mail_io_cb_error_return(rxbuf);
		break; 
	case 450:	/*
				Requested mail action not taken: mailbox unavailable
				[E.g., mailbox busy]
				*/
		mail_io_cb_error_return(rxbuf);
		break;		
	case 451:	/* Requested action aborted: local error in processing */
		mail_io_cb_error_return(rxbuf);
		break;
	case 452:	/*
				Requested action not taken: insufficient system storage
 				*/
 		mail_io_cb_error_return(rxbuf);
 		break;
	case 500:	/*
				Syntax error, command unrecognized
				[This may include errors such as command line too long]
				*/
		mail_io_cb_error_return(rxbuf);
		break;		
	case 501:	/* Syntax error in parameters or arguments */
		mail_io_cb_error_return(rxbuf);
		break;
	case 502:	/* Command not implemented */
		mail_io_cb_error_return(rxbuf);
		break;
	case 503:	/* Bad sequence of commands */
		mail_io_cb_error_return(rxbuf);
		break;
	case 504:	/* Command parameter not implemented */
		mail_io_cb_error_return(rxbuf);
		break;
	case 550:	/*
				Requested action not taken: mailbox unavailable
				[E.g., mailbox not found, no access]
				*/
		mail_io_cb_error_return(rxbuf);
		break;
	case 551:	/* User not local; please try <forward-path> */
		mail_io_cb_error_return(rxbuf);
		break;
	case 552:	/*
				Requested mail action aborted: exceeded storage
				allocation
				*/
		mail_io_cb_error_return(rxbuf);
		break;
	case 553:	/*
				Requested action not taken: mailbox name not allowed
			   	[E.g., mailbox syntax incorrect]
				*/
		mail_io_cb_error_return(rxbuf);
		break;
	case 554:	/* Transaction failed */
		mail_io_cb_error_return(rxbuf);
		break;
	default:
		mail_io_cb_error_return(rxbuf);
		break;
	}/* end switch num_response */	

}/* end function mail_io_read_cb */


int mail_io_cb_error_return(char *error_message)
{
if(debug_flag)
	{
	fprintf(stdout, "mail_io_cb_error_return(): arg error_message=%s\n", error_message);
	}

/* argument check */
if(! error_message) return(0);

send_to_mail_server("QUIT\n");
mail_mode = QUIT_SEND;

fl_remove_io_callback(mail_socketfd,\
FL_READ, (FL_IO_CALLBACK) mail_io_read_cb);
close(mail_socketfd);

/* do not hang around to wait for confirmation of quit */
mail_server_status = DISCONNECTED;

set_send_status(mail_posting, SEND_FAILED);

/* want to try any other commands */
/*process_command_queue_error_flag = 1;*/
command_in_progress = 0;

my_command_status_display(error_message);
to_error_log(error_message);
refresh_screen("");

return(1); 	
}/* end function mail_io_cb_error_return */


