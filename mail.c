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


int post_email(long posting_id)
{
/* called from process_command_queue,  which is called from idle_cb :) */
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "post_email(): arg posting_id=%ld\n", posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

/* set the global, so mail_io_read_cb() knows what is send */
mail_posting = posting_id;

/* mail is port 25 SMTP */
if(! connect_to_mail_server(mail_server_name, mail_server_port) )
	{
	/* clear command status display */
	sprintf(temp, "Connection to mail server %s port %d failed",\
	mail_server_name, mail_server_port);
	to_command_status_display(temp);
	to_error_log(temp);

	set_send_status(mail_posting, SEND_FAILED);
	command_in_progress = 0;

	if(auto_disconnect_from_net_flag) auto_disconnect_from_net();
	return(0);
	}

fl_add_io_callback(\
mail_socketfd, FL_READ, (FL_IO_CALLBACK) mail_io_read_cb, 0);

/* rest handled by mail_io_read_cb */
return(1);
}/* end function post email */		


int connect_to_mail_server(char *mail_server, int port)
{
struct hostent *hp;
struct sockaddr_in sa; /* -Wall gets confused */
int a, i;
char temp[TEMP_SIZE];
char dotted_quad[TEMP_SIZE];
int flags;
time_t connect_timer;
extern int idle_cb();
int connect_flag;
unsigned long in;

if(debug_flag)
	{
	fprintf(stdout, "connect_to_mail_server(): arg mail_server=%s port=%d\n",\
	mail_server, port);
	}
	
/* argument check */
if(! mail_server) return(0);
if(port <= 0) return(0);

mail_server_status = DISCONNECTED;

/* test for name or address */
hp = 0;
in = inet_addr (mail_server);
if(in != INADDR_NONE) /* address */
	{
	sprintf(temp, "getting host %s by address", mail_server);
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", temp);
		}
	to_command_status_display(temp);

	hp = gethostbyaddr((char *)&in, sizeof(in), AF_INET);
	if(! hp)
		{
		sprintf(temp,\
		"gethostbyaddress failed: cannot get host %s by address",\
		mail_server);
		if(debug_flag) fprintf(stdout, "%s\n", temp);
		}
	}
else /* name */
	{
	sprintf(temp, "getting host %s by name", mail_server);
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", temp);
		}
	to_command_status_display(temp);

	hp = gethostbyname(mail_server);
	if(! hp)
		{
		sprintf(temp,\
		"gethostbyname failed: cannot get host %s by name", mail_server);
		if(debug_flag) fprintf(stdout, "%s\n", temp);
		}
	}
if(! hp)
	{
	/* 
	make sure if running unattended that program does not wait for user
	to press OK in fl_show_alert, since the dialup would never disconnect.
	*/

	if(! auto_disconnect_from_net_flag)
		{
		an_idle_cb = fl_set_idle_callback(0, 0);
		fl_show_alert(temp, "", "", 0);
		fl_set_idle_callback(idle_cb, an_idle_cb);
		}
	else
		{
		/* NO WAIT */
		to_error_log(temp);
		to_command_status_display(temp);
		}

	return(0);
	}

/* gethostbyname() leaves port and host address in network byte order */

bzero(&sa, sizeof(sa) );
bcopy(hp -> h_addr, (char *)&sa.sin_addr, hp -> h_length);
sa.sin_family = AF_INET;
sa.sin_port = htons( (u_short)port);

/* sa.sin_addr and sa.sin_port now in network byte order */

/* create a socket */
mail_socketfd = socket(hp -> h_addrtype, SOCK_STREAM, 0);
if(mail_socketfd < 0)
	{
	sprintf(temp, "Cannot create socket");
	if(debug_flag) fprintf(stdout, "%s\n", temp);
				
	an_idle_cb = fl_set_idle_callback(0, 0);
	fl_show_alert(temp, "", "", 0);
	fl_set_idle_callback(idle_cb, an_idle_cb);
				
	return(0);
	}/* end mail_socketfd < 0 */

sprintf(dotted_quad, "%s", (char *)inet_ntoa(sa.sin_addr.s_addr) );

/* set for nonblocking socket */
if (fcntl(mail_socketfd, F_SETFL, O_NONBLOCK) < 0)
	{
	sprintf(temp, "Cannot set socket non blocking");
	if(debug_flag) fprintf(stdout, "%s\n", temp);
				
	if(! auto_disconnect_from_net_flag)
		{
		an_idle_cb = fl_set_idle_callback(0, 0);
		fl_show_alert(temp, "", "", 0);
		fl_set_idle_callback(idle_cb, an_idle_cb);
		}
	else
		{
		/* NO WAIT */
		to_error_log(temp);
		to_command_status_display(temp);
		}
	return(0);
	}

/* connect */
sprintf(temp, "Connecting to mail server %s (%s)  port %d  timeout %d",\
mail_server, dotted_quad, port, connect_to_mail_server_timeout);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

/* mail_server sometimes does not seem to answere to the first connect */
/* not so sure this is the way to do it, may change this later */
for(i = 0; i < MAIL_SERVER_CONNECT_RETRIES; i++)
	{
	/* start the connect timer */
	connect_flag = 0;
	connect_timer = time(0);
	while(1)
		{
		a = connect(mail_socketfd, (struct sockaddr*)&sa, sizeof(sa) );
		if(a == 0)
			{
			connect_flag = 1;
			break;/* connected */
			}
		if(debug_flag) fprintf(stdout, "In connect errno=%d\n", errno);
	
		/* test for connect time out */
		if( (time(0) - connect_timer) > connect_to_mail_server_timeout)
			{
			sprintf(temp,\
"Connecting to mail server %s (%s)  port %d  timeout %d  retry %d (of %d)",\
			mail_server, dotted_quad, port, connect_to_mail_server_timeout, i + 1,\
			MAIL_SERVER_CONNECT_RETRIES);
			if(debug_flag) fprintf(stdout, "Connect timeout %s\n", temp);
				
			/* tired of pressing OK many times */
			to_command_status_display(temp);
			to_error_log(temp);

			/* try again */
			break;
		
			/* close the socket */
			close(mail_socketfd);
		
			return(0);
			}/* end timeout */
		}/* end while connect */

	if(connect_flag) break;

	}/* end for */

if(! connect_flag)
	{
	/* close the socket */
	close(mail_socketfd);

	to_error_log("Connect to mail_server failed");

	return(0);
	}

/* now set blocking */
flags = fcntl(mail_socketfd, F_GETFL, 0);
flags &= ~O_NONBLOCK;
fcntl(mail_socketfd, F_SETFL, flags);

sprintf(temp, "Connected to mail server %s (%s)  port %d",\
mail_server, dotted_quad, port);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

mail_server_status = CONNECTED;

return(1);
}/* end function connect_to_mail_server */


int send_to_mail_server(char *text)/* send data to mail_server */
{
int a, c, i, j;
char txbuf[READSIZE];

if(debug_flag)
	{
	fprintf(stdout, "send_to_mail_server(): arg\ntext=%s (mail_socketfd=%d)\n",\
	text, mail_socketfd);
	}
	
/* argument check */
if(! text) return(0);

if(mail_server_status == DISCONNECTED) return(0);

/* expand any LF ('\n' 10 decimal) to CRLF (13 10) */
i = 0;
j = 0;
while(1)
	{
	c = text[i];
	if(c == '\n')
		{
		txbuf[j] = 13;
		j++;
		if(j > READSIZE)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"send_to_mail_server(): overflow in expanding text=%s\n", text);
				}
			return(0);
			}		
		}
	txbuf[j] = c;
	if(! c) break;
	i++;
	j++;
	}

a = write(mail_socketfd, txbuf, strlen(txbuf) );     
if(a < 0)
	{
	if(debug_flag)
		{
		fprintf(stdout, "\nwrite failed, errno=%d\n", errno);
		}
	perror("send_to_mail_server(): write failed:");
	return(0);
	}

/* update bytes_send */
bytes_send += (long) strlen(txbuf);

if(debug_flag)
	{
	fprintf(stdout, "txbuf=%s", txbuf);
	}

return(1);
}/* end function send_to_mail_server */


