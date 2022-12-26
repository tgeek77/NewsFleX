/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997 <Jan Mourer>
www: pante@pi.net
snail www: PO BOX 61  SINT ANNA  9076ZP HOLLAND

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

#define FTP_SERVER_CONNECT_RETRIES	5

FL_APPEVENT_CB an_idle_cb;

/*
control connection port 21
data connection port 20 default
*/

int connect_to_ftp_server(char *ftp_server, int port, int *socketfd)
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
	fprintf(stdout, "connect_to_ftp_server(): arg ftp_server=%s port=%d\n",\
	ftp_server, port);
	}
	
/* argument check */
if(! ftp_server) return(0);
if(port <= 0) return(0);

/*ftp_control_server_status = DISCONNECTED;*/

/* test for name or address */
hp = 0;
in = inet_addr (ftp_server);
if(in != INADDR_NONE) /* address */
	{
	sprintf(temp, "getting host %s by address", ftp_server);
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
		ftp_server);
		if(debug_flag) fprintf(stdout, "%s\n", temp);
		}
	}
else /* name */
	{
	sprintf(temp, "getting host %s by name", ftp_server);
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", temp);
		}
	to_command_status_display(temp);

	hp = gethostbyname(ftp_server);
	if(! hp)
		{
		sprintf(temp,\
		"gethostbyname failed: cannot get host %s by name", ftp_server);
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
bcopy(hp -> h_addr,\
(char *)&sa.sin_addr, hp -> h_length);
sa.sin_family = AF_INET;
sa.sin_port = htons( (u_short)port);

/* sa.sin_addr and sa.sin_port now in network byte order */

/* create a socket */
*socketfd = socket(hp -> h_addrtype, SOCK_STREAM, 0);
if(*socketfd < 0)
	{
	sprintf(temp, "Cannot create socket");
	if(debug_flag) fprintf(stdout, "%s\n", temp);
				
	an_idle_cb = fl_set_idle_callback(0, 0);
	fl_show_alert(temp, "", "", 0);
	fl_set_idle_callback(idle_cb, an_idle_cb);
				
	return(0);
	}/* end socketfd < 0 */

sprintf(dotted_quad, "%s", (char *)inet_ntoa(sa.sin_addr.s_addr) );

/* set for nonblocking socket */
if (fcntl(*socketfd, F_SETFL, O_NONBLOCK) < 0)
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
sprintf(temp, "Connecting to ftp server %s (%s)  port %d  timeout %d",\
ftp_server, dotted_quad, port, connect_to_ftp_server_timeout);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

/* ftp_server sometimes does not seem to answere to the first connect */
/* not so sure this is the way to do it, may change this later */
for(i = 0; i < FTP_SERVER_CONNECT_RETRIES; i++)
	{
	/* start the connect timer */
	connect_flag = 0;
	connect_timer = time(0);
	while(1)
		{
		a = connect(*socketfd, (struct sockaddr*)&sa, sizeof(sa) );
		if(a == 0)
			{
			connect_flag = 1;
			break;/* connected */
			}
		if(debug_flag) fprintf(stdout, "In connect errno=%d\n", errno);
	
		/* test for connect time out */
		if( (time(0) - connect_timer) > connect_to_ftp_server_timeout)
			{
			sprintf(temp,\
"Connecting to ftp server %s (%s)  port %d  timeout %d  retry %d (of %d)",\
			ftp_server, dotted_quad, port, connect_to_ftp_server_timeout, i + 1,\
			FTP_SERVER_CONNECT_RETRIES);
			if(debug_flag) fprintf(stdout, "Connect timeout %s\n", temp);
				
			/* tired of pressing OK many times */
			to_command_status_display(temp);
			to_error_log(temp);

			/* try again */
			break;
		
			/* close the socket */
			close(*socketfd);
		
			return(0);
			}/* end timeout */
		}/* end while connect */

	if(connect_flag) break;

	}/* end for */

if(! connect_flag)
	{
	/* close the socket */
	close(*socketfd);

	to_error_log("Connect to ftp_server failed");

	return(0);
	}

/* now set blocking */
flags = fcntl(*socketfd, F_GETFL, 0);
flags &= ~O_NONBLOCK;
fcntl(*socketfd, F_SETFL, flags);

sprintf(temp, "Connected to ftp server %s (%s)  port %d",\
ftp_server, dotted_quad, port);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

/*ftp_control_server_status = CONNECTED;*/

return(1);
}/* end function connect_to_ftp_server */


int send_to_ftp_server(int socketfd, char *text)
/* send data to ftp_server */
{
int a, c, i, j;
char txbuf[READSIZE];

if(debug_flag)
	{
	fprintf(stdout, "send_to_ftp_server(): arg socketfd=%d\ntext=%s\n",\
	socketfd, text);
	}
	
/* argument check */
if(! text) return(0);

/*if(ftp_control_server_status == DISCONNECTED) return(0);*/

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
				"send_to_ftp_server(): overflow in expanding text=%s\n",\
				text);
				}
			return(0);
			}		
		}
	txbuf[j] = c;
	if(! c) break;
	i++;
	j++;
	}

a = write(socketfd, txbuf, strlen(txbuf) );     
if(a < 0)
	{
	if(debug_flag)
		{
		fprintf(stdout, "\nwrite failed, errno=%d\n", errno);
		}
	perror("send_to_ftp_server(): write failed:");
	return(0);
	}

/* update bytes_send */
bytes_send += (long) strlen(txbuf);

if(debug_flag)
	{
	fprintf(stdout, "txbuf=%s", txbuf);
	}

return(1);
}/* end function send_to_ftp_server */


