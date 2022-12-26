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


char dotted_quad[TEMP_SIZE];

struct in_addr local_ip_address;/* network byte order */
struct in_addr local_ip_address_according_to_server;/* n. b. o. */

time_t last_body_line_time;
time_t now;
int ftp_mode;


void ftp_control_io_read_cb(int control_socketfd, long data)
{
char rxbuf[READSIZE];
int a;
long li;
char c;
char temp[READSIZE];/* beware of overflow rxbuf=512 + additional text */
char arg0[TEMP_SIZE];
static int num_response;/* preserve between calls !!!!! */
int cr_flag;
static int ip[4];
static port[2];
char newhost[80];
int newport;

if(debug_flag)
	{
	fprintf(stdout, "ftp_control_io_read_cb(): control_socketfd=%d\n",\
	control_socketfd);
	}

cr_flag = 0;
/* read from control_socketfd */
li = 0;
while(1)
	{
	errno = 0;
	a = read(control_socketfd, &c, 1);
	if(a < 0)
		{
		if(errno == EAGAIN) continue;/* try again */
		
		if(debug_flag)
			{
			fprintf(stdout,\
			"\nread failed, connection to server lost errno=%d\n", errno);
			}

		perror("read failed: connection to server lost");

		ftp_control_io_cb_error_return(\
		"read failed, connection to www server lost");
		}
	if(a == 0)
		{
		sprintf(temp, "Connection closed by http server (EOF)");
		my_command_status_display(temp);
		if(debug_flag) fprintf(stdout, "%s\n", temp);

		fl_remove_io_callback(control_socketfd,\
		FL_READ, (FL_IO_CALLBACK) ftp_control_io_read_cb);

		/* close the server control_socketdf */
		close(control_socketfd);

		ftp_control_server_status = DISCONNECTED;

		if(debug_flag)
			{
			fprintf(stdout,\
			"ftp control  connection closed by server (EOF)\n");
			}
		return;
		}

	/* valid byte received */

	bytes_received++;

/*fprintf(stdout, "%c (%d)", c, c);*/
	
	if(c == 10)/* LF */
		{
		break;/* ignore any CR, line ends on LF */
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

/* get the numeric response (all resposes are numeric) */

arg0[0] = 0;
a = sscanf(rxbuf, "%s", arg0);
if( (! isdigit(arg0[0]) ) || (! isdigit(arg0[1]) ) || (! isdigit(arg0[2]) ) )
	{
	/* problem, not numeric */
	ftp_control_io_cb_error_return(rxbuf);
	}

/* handle multi line replies (ignore any lines starting with xxx-) */
if(rxbuf[3] == '-') return; /* 220-something, multi line replies */

num_response = atoi(arg0);
if(debug_flag)
	{
	fprintf(stdout, "num_response=%d\n", num_response);
	}

if( (ftp_mode == FTP_CONNECT) && (num_response != 220) )
	{
	ftp_control_io_cb_error_return(rxbuf);
	return;
	}

switch(num_response)
	{
    case 110: /* Restart marker reply.
			 	 In this case, the text is exact and not left to the
				 particular implementation; it must read:
				 MARK yyyy = mmmm
				 Where yyyy is User-process data stream marker, and mmmm
				 server's equivalent marker (note the spaces between markers
				 and "=").
			  */	
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 120: /*  Service ready in nnn minutes. */
		/* abort */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 125: /* Data connection already open; transfer starting. */
		/* ignore */
		return;
		break;
	case 150: /* File status okay; about to open data connection. */
		/* 150 Opening BINARY mode data connection for /etc/passwd (2241 bytes)*/
	     if(! sscanf(rxbuf, "%*[^(](%ld", &ftp_file_size) )
	     	{
	        ftp_file_size = -1;	
			}
			
		if(ftp_mode == FTP_RETR)
			{
			return;/* ignore this */
			}
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 200: /* Command okay. */
		if(ftp_mode == FTP_TYPE)
			{
			/* now in binary mode */
			
			/* open the local file for write */
			ftp_read_fileptr = fopen(ftp_local_path_filename, "w");
			if(! ftp_read_fileptr)
				{
				sprintf(temp, "Cannot open local file %s for write\n",\
				ftp_local_path_filename);
				ftp_control_io_cb_error_return(temp);
				return;
				}
			if(debug_flag)
				{
				fprintf(stdout,\
				"local file open=%s\n", ftp_local_path_filename);			
				}
			/*
			reset the byte counter, used for percentage progress indication.
			*/
			ftp_bytes_received = 0;

			/*
			Ask server to go into passive mode.
			Had a look at urlget, setting up a listening socket gives problems
			withe xforms cb, now just going to do a connect (like before).
			*/
			sprintf(temp, "PASV\n");
			if(ftp_control_server_status != CONNECTED)
				{
				ftp_control_io_cb_error_return(\
				"control connection to ftp server lost");
				return;
				}
			send_to_ftp_server(control_socketfd, temp);
			ftp_mode = FTP_PASV;
			/* wait for response 227 */
			return;			
			}/* end if ftp_mode is FTP_TYPE */
		else
			{
			/* some handshake error */
			ftp_control_io_cb_error_return(rxbuf);
			return;
			}
		return;
		break;
	case 202: /* Command not implemented, superfluous at this site. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 211: /* System status, or system help reply. */
		/* ignore */
		return;
		break;
	case 212: /* Directory status. */
		/* ignore */
		return;
		break;
    case 213: /* File status. */
		/* ignore */
		return;
		break;
    case 214: /* Help message.
				 On how to use the server or the meaning of a particular
				 non-standard command.  This reply is useful only to the
				 human user.
			  */
		/* ignore */
		return;
		break;
	case 215: /* NAME system type.
				Where NAME is an official system name from the list in the
				Assigned Numbers document.
			  */
		/* ignore */
		return;
		break;
	case 220: /* Service ready for new user. */
		
		sprintf(temp, "USER %s\n", ftp_user);
		ftp_mode = FTP_USER;
		if(ftp_control_server_status != CONNECTED)
			{
			ftp_control_io_cb_error_return(\
			"control connection to ftp server lost");
			return;
			}
		send_to_ftp_server(control_socketfd, temp);	
		return;
		break;
	case 221: /* Service closing control connection.
				 Logged out if appropriate.
			  */
		/* ignore, do not know what this is */
		return;
		break;
	case 225: /* Data connection open; no transfer in progress. */
		/* ignore */
		return;
		break;
	case 226: /* Closing data connection.
				 Requested file action successful (for example, file
				 transfer or file abort).
			  */
		/*
		THIS DOES ***NOT*** INDICATE THE FILE IS RECEIVED, ONLY
		THAT ALL PACKETS ARE ON THE WAY.
		*/
		if(ftp_mode == FTP_RETR)
			{
/*			fclose(ftp_read_fileptr);*/

			/* close the server control_socketdf */
			close(control_socketfd);
			ftp_control_server_status = DISCONNECTED;

			sprintf(temp, "FTP requested action successful (in progress)");

			fl_remove_io_callback(control_socketfd,\
			FL_READ, (FL_IO_CALLBACK) ftp_control_io_read_cb);

			my_command_status_display(temp);
			if(debug_flag) fprintf(stdout, "%s\n", temp);

			/* close the data socket */
/*
			close(ftp_data_socketfd);
			ftp_data_server_status = DISCONNECTED;
*/
			
			return;
			}/* end if ftp_mode is FTP_RETR */
		else
			{
			/* some handshake error */
			ftp_control_io_cb_error_return(rxbuf);
			return;
			}
		return;
		break;
	case 227: /* Entering Passive Mode (h1,h2,h3,h4,p1,p2). */
		if(ftp_mode == FTP_PASV)
			{
			/* parse rxbuf, get host and port we have to connect to */
			/* 227 Entering Passive Mode (127,0,0,1,4,5) */

			if(6 != sscanf(rxbuf, "%*[^(](%d,%d,%d,%d,%d,%d)",\
			&ip[0], &ip[1], &ip[2], &ip[3],\
			&port[0], &port[1]))
				{
				sprintf(temp,\
				"Incorrect format passive mode reply %s", rxbuf); 
				ftp_control_io_cb_error_return(temp);
				return;
				}

		    sprintf(newhost, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			newport = port[0]*256 + port[1];
	
			if(! connect_to_ftp_server(newhost, newport, &ftp_data_socketfd) )
				{
				sprintf(temp,\
				"Data connection to ftp server %s port %d failed",\
				newhost, newport);
				to_command_status_display(temp);
				to_error_log(temp);
				ftp_data_server_status = DISCONNECTED;
				if(auto_disconnect_from_net_flag) auto_disconnect_from_net();
				return;
				}
			ftp_data_server_status = CONNECTED;

			if(debug_flag)
				{
				fprintf(stdout,\
				"ftp_control_io_read_cb(): ftp adding ftp_data_io_callback\n");
				}
			fl_add_io_callback(\
			ftp_data_socketfd, FL_READ,\
			(FL_IO_CALLBACK) ftp_data_io_read_cb, 0);

			/* tell the server we are ready to receive */
			sprintf(temp, "RETR %s\n", ftp_remote_path_filename);
			ftp_mode = FTP_RETR;
			if(ftp_control_server_status != CONNECTED)
				{
				ftp_control_io_cb_error_return(\
				"control connection to ftp server lost");
				return;
				}
			send_to_ftp_server(control_socketfd, temp);	
			return;
			}/* end if ftp_mode == FTP_PASV */
		else
			{
			/* some handshake error */
			ftp_control_io_cb_error_return(rxbuf);
			return;
			}
		return;
		break;
	case 230: /* User logged in, proceed. */
		if(ftp_mode == FTP_PASS)
			{
			/* we are in */
			if(debug_flag) fprintf(stdout, "CODE 230 LOGGED IN\n");			
			/* we do all in binary */
			sprintf(temp, "TYPE I\n");
			ftp_mode = FTP_TYPE;
			if(ftp_control_server_status != CONNECTED)
				{
				ftp_control_io_cb_error_return(\
				"control connection to ftp server lost");
				return;
				}
			send_to_ftp_server(control_socketfd, temp);	
			return;
			}
		else
			{
			/* some handshake error */
			ftp_control_io_cb_error_return(rxbuf);
			return;
			}
		return;
		break;
	case 250: /* Requested file action okay, completed. */
		/* ignore */
		return;
		break;
	case 257: /* "PATHNAME" created. */
		/* ignore */
   		return;
		break;
	case 331: /* User name okay, need password. */
		if(ftp_mode == FTP_USER)
			{ 
			sprintf(temp, "PASS %s\n", ftp_password);
			ftp_mode = FTP_PASS;
			if(ftp_control_server_status != CONNECTED)
				{
				ftp_control_io_cb_error_return(\
				"control connection to ftp server lost");
				return;
				}
			send_to_ftp_server(control_socketfd, temp);	
			return;
			}
		else
			{
			/* some handshake error */
			ftp_control_io_cb_error_return(rxbuf);
			return;
			}
		return;
		break;
	case 332: /* Need account for login. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 350: /*  Requested file action pending further information. */
          
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 421: /* Service not available, closing control connection.
				 This may be a reply to any command if the service knows it
				 must shut down.
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 425: /* Can't open data connection. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 426: /* Connection closed; transfer aborted. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 450: /* Requested file action not taken.
				 File unavailable (e.g., file busy).
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 451: /* Requested action aborted: local error in processing. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
    case 452: /* Requested action not taken.
				 Insufficient storage space in system.
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 500: /* Syntax error, command unrecognized.
				 This may include errors such as command line too long.
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 501: /* Syntax error in parameters or arguments. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 502: /* Command not implemented. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 503: /* Bad sequence of commands. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 504: /* Command not implemented for that parameter. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 530: /* Not logged in. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 532: /* Need account for storing files. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 550: /* Requested action not taken.
				 File unavailable (e.g., file not found, no access).
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 551: /* Requested action aborted: page type unknown. */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 552: /* Requested file action aborted.
				 Exceeded storage allocation (for current directory or
				dataset).
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	case 553: /* Requested action not taken.
				 File name not allowed.
			  */
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	default:
		ftp_control_io_cb_error_return(rxbuf);
		return;
		break;
	}/* end switch num_response */
	
return;
}/* end function ftp_control_io_read_cb */


int ftp_control_io_cb_error_return(char *error_message)
{
int status;

if(debug_flag)
	{
	fprintf(stdout,\
	"ftp_control_io_cb_error_return(): arg error_message=%s\n", error_message);
	}

/* argument check */
if(! error_message) return(0);

ftp_mode = FTP_OFF;

fl_remove_io_callback(ftp_control_socketfd,\
FL_READ, (FL_IO_CALLBACK) ftp_control_io_read_cb);
close(ftp_control_socketfd);

/* do not hang around to wait for confirmation of quit */
ftp_control_server_status = DISCONNECTED;

/*process_command_queue_error_flag = 1;*/
command_in_progress = 0;

if(ftp_command == FTP_RECEIVE)
	{
/*	fclose(ftp_read_fileptr);*/

/* unlink the file !!!!! */
	}
ftp_command = FTP_OFF;
		
/* mark entry in url browser as failed */
if( get_url_status(url_in_progress, &status) )
	{
	status &= (0xffffff - RETRIEVE_URL);
	status |= FAILED_URL;
	set_url_status(url_in_progress, status);
	show_urls(SHOW_SAME_POSITION);
	save_urls();
	}
	
my_command_status_display(error_message);
to_error_log(error_message);
refresh_screen("");


ftp_failed_flag = 1;
return(1); 	
}/* end function ftp_control_io_cb_error_return */


