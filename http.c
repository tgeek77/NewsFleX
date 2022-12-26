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

#define HTTP_SERVER_CONNECT_RETRIES	5

FL_APPEVENT_CB an_idle_cb;

/*
Possibilities:
BODY		BACKGROUND TEXT BGCOLOR LINK YLINK ALINK
BASE		HREF
A			HREF NAME URN REL REV TITLE METHODS
LINK		HREF URN REL REV TITLE METHODS
IMG			SRC ALT ALIGN ISMAP
INPUT
ISINDEX
FORM
*/	


#define EXPECT_BODY		1
#define EXPECT_BASE		2
#define EXPECT_A		3
#define EXPECT_LINK		4
#define EXPECT_IMG		5
#define EXPECT_SUB		6
#define IN_HREF			7
#define IN_SRC			8
#define IN_BACKGROUND	9
#define IN_URN			10
#define IN_REL			11
#define IN_REV			12
#define IN_TITLE		13
#define IN_METHODS		14
#define IN_TEXT			15
#define IN_BGCOLOR		16
#define IN_LINK			17
#define IN_YLINK		18
#define IN_ALINK		19
#define IN_ALIGN		20
#define IN_ISMAP		21
#define IN_ALT			22
#define IN_NAME			23
#define IN_COMMENT		24


char *base_url;

/*
To prevent recursive from going into a loop if .html documents
refer to each other, we keep a list of all successfull URLS.
Once in this list, no new attempt is made.
The list is cleared when going online.
This is debatable, but then the files are fresh.
*/
struct inurl
	{
	char *name;
	char *host;
	struct inurl *nxtentr;
	struct inurl *prventr;
	};
struct inurl *inurltab[2]; /* first element points to first entry,
			second element to last entry */

struct inurl *lookup_inurl(char *name)
{
struct inurl *pa;

if(! name) return(0);

for(pa = inurltab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_inurl */


struct inurl *install_inurl_at_end_of_list(char *name)
{
struct inurl *plast, *pnew;
struct inurl *lookup_inurl();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_inurl_at_end_off_list(): arg name=%s\n", name);
	}

if(! name) return(0);

pnew = lookup_inurl(name);
if(pnew)
	{
	/* free previous definition */
/*	free(pnew -> subject);*/
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct inurl *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = inurltab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points to zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !inurltab[0] ) inurltab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
inurltab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_inurl */


int delete_all_inurls()/* delete all entries from table */
{
struct inurl *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_inurls() arg none\n");
	}

while(1)
	{	
	pa = inurltab[0];
	if(! pa) break;
	inurltab[0] = pa -> nxtentr;
	free(pa -> name);

	free(pa);/* free structure */
	}/* end while all structures */

inurltab[1] = 0;
return(1);
}/* end function delete_all_inurls */


int add_inurl(char *url, int *present_flag)
{
struct inurl *pa;

if(debug_flag)
	{
	fprintf(stdout, "add_inurl(): arg url=%s\n", url);
	}

if(! url) return(0);

*present_flag = 0;
pa = lookup_inurl(url);
if(pa) 
	{
	*present_flag = 1;
	return(1);/* have it */
	}

pa = install_inurl_at_end_of_list(url);
if(! pa) return(0);/* cannot install */

return(1);
}/* end function add_inurl */


int connect_to_http_server(char *http_server, int port)
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
	fprintf(stdout, "connect_to_http_server(): arg http_server=%s port=%d\n",\
	http_server, port);
	}
	
/* argument check */
if(! http_server) return(0);
if(port <= 0) return(0);

http_server_status = DISCONNECTED;

/*
- Function: int inet_aton (const char *NAME, struct in_addr *ADDR)
This function converts the Internet host address NAME from the
standard numbers-and-dots notation into binary data and stores it
in the truct in_addr' that ADDR points to.  	net_aton' returns 
nonzero if the address is valid, zero if not.
*/
/*a = inet_aton (http_server, hp);*/
/*
 - Function: unsigned long int inet_addr (const char *NAME)
     This function converts the Internet host address NAME from the
     standard numbers-and-dots notation into binary data.  If the input
     is not valid, `inet_addr' returns `INADDR_NONE'.  This is an
     obsolete interface to `inet_aton', described immediately above; it
     is obsolete because `INADDR_NONE' is a valid address
     (255.255.255.255), and `inet_aton' provides a cleaner way to
     indicate error return.
*/

/* test for name or address */
hp = 0;
in = inet_addr (http_server);
if(in != INADDR_NONE) /* address */
	{
	sprintf(temp, "getting host %s by address", http_server);
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
		http_server);
		if(debug_flag) fprintf(stdout, "%s\n", temp);
		}
	}
else /* name */
	{
	sprintf(temp, "getting host %s by name", http_server);
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", temp);
		}
	to_command_status_display(temp);

	hp = gethostbyname(http_server);
	if(! hp)
		{
		sprintf(temp,\
		"gethostbyname failed: cannot get host %s by name", http_server);
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

/*
gethostbyname() or gethostbyaddress leaves port and host address in network
byte order.
*/

bzero(&sa, sizeof(sa) );
bcopy(hp -> h_addr, (char *)&sa.sin_addr, hp -> h_length);
sa.sin_family = AF_INET;
sa.sin_port = htons( (u_short)port);

/* sa.sin_addr and sa.sin_port now in network byte order */

/* create a socket */
http_socketfd = socket(hp -> h_addrtype, SOCK_STREAM, 0);
if(http_socketfd < 0)
	{
	sprintf(temp, "Cannot create socket");
	if(debug_flag) fprintf(stdout, "%s\n", temp);
				
	an_idle_cb = fl_set_idle_callback(0, 0);
	fl_show_alert(temp, "", "", 0);
	fl_set_idle_callback(idle_cb, an_idle_cb);
				
	return(0);
	}/* end http_socketfd < 0 */

sprintf(dotted_quad, "%s", (char *)inet_ntoa(sa.sin_addr.s_addr) );

/* set for nonblocking socket */
if (fcntl(http_socketfd, F_SETFL, O_NONBLOCK) < 0)
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
sprintf(temp, "Connecting to http server %s (%s)  port %d  timeout %d",\
http_server, dotted_quad, port, connect_to_http_server_timeout);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

/* http_server sometimes does not seem to answere to the first connect */
/* not so sure this is the way to do it, may change this later */
for(i = 0; i < HTTP_SERVER_CONNECT_RETRIES; i++)
	{
	/* start the connect timer */
	connect_flag = 0;
	connect_timer = time(0);
	while(1)
		{
		a = connect(http_socketfd, (struct sockaddr*)&sa, sizeof(sa) );
		if(a == 0)
			{
			connect_flag = 1;
			break;/* connected */
			}
		if(debug_flag) fprintf(stdout, "In connect errno=%d\n", errno);
	
		/* test for connect time out */
		if( (time(0) - connect_timer) > connect_to_http_server_timeout)
			{
			sprintf(temp,\
"Connecting to http server %s (%s)  port %d  timeout %d  retry %d (of %d)",\
			http_server, dotted_quad, port, connect_to_http_server_timeout, i + 1,\
			HTTP_SERVER_CONNECT_RETRIES);
			if(debug_flag) fprintf(stdout, "Connect timeout %s\n", temp);
				
			/* tired of pressing OK many times */
			to_command_status_display(temp);
			to_error_log(temp);

			/* try again */
			break;
		
			/* close the socket */
			close(http_socketfd);
		
			return(0);
			}/* end timeout */
		}/* end while connect */

	if(connect_flag) break;

	}/* end for */

if(! connect_flag)
	{
	/* close the socket */
	close(http_socketfd);

	to_error_log("Connect to http_server failed");

	return(0);
	}

/* now set blocking */
flags = fcntl(http_socketfd, F_GETFL, 0);
flags &= ~O_NONBLOCK;
fcntl(http_socketfd, F_SETFL, flags);

sprintf(temp, "Connected to http server %s (%s)  port %d",\
http_server, dotted_quad, port);
if(debug_flag)
	{
	fprintf(stdout, "%s\n", temp);
	}
to_command_status_display(temp);

http_server_status = CONNECTED;

return(1);
}/* end function connect_to_http_server */


int send_to_http_server(char *text)/* send data to http_server */
{
int a, c, i, j;
char txbuf[READSIZE];

if(debug_flag)
	{
	fprintf(stdout, "send_to_http_server(): arg\ntext=%s (http_socketfd=%d)\n",\
	text, http_socketfd);
	}
	
/* argument check */
if(! text) return(0);

if(http_server_status == DISCONNECTED) return(0);

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
				"send_to_http_server(): overflow in expanding text=%s\n",\
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

a = write(http_socketfd, txbuf, strlen(txbuf) );     
if(a < 0)
	{
	if(debug_flag)
		{
		fprintf(stdout, "\nwrite failed, errno=%d\n", errno);
		}
	perror("send_to_http_server(): write failed:");
	return(0);
	}

/* update bytes_send */
bytes_send += (long) strlen(txbuf);

if(debug_flag)
	{
	fprintf(stdout, "txbuf=%s", txbuf);
	}

return(1);
}/* end function send_to_http_server */


int http_parse_file(\
char *filename, float version, int flags)
{
int c;
struct stat *statptr;
char *space, *spaceptr;
FILE *fileptr;
char *position;
char *dir;
char *ptr1;
char *ptr2;
size_t size;
char *new_pos;
char temp[TEMP_SIZE];
int host_only_flag;
int get_pictures_flag;
int maximum_url_depth;
int current_url_depth;
int new_flags;/* new depth */

if(debug_flag)
	{
	fprintf(stdout,\
	"http_parse_file(): arg filename=%s version=%.2f flags=%d\n",\
	filename, version, flags);	
	}

/* argument check */
if(! filename) return(0);



if(flags & HOST_ONLY_URL) host_only_flag = 1;
else host_only_flag = 0;

if(flags & PICTURES_URL) get_pictures_flag = 1;
else get_pictures_flag = 0;

c = flags & 0x00ff00;
maximum_url_depth = c >> 8;

fileptr = fopen(filename, "r");
if(! fileptr)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_parse_file(): cannot open file %s for read\n", filename);
		}
	return(0);
	}/* end if open fails */
/*fprintf(stdout, "file open\n");*/

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(fileptr), statptr);
size = statptr -> st_size;
space = malloc(statptr -> st_size + 1);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_parse_file(): malloc could not allocate space\n");
		}
	free(statptr);
	return(0);
	}

free(statptr);

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(fileptr);
		if(! ferror(fileptr) ) break;
		perror("http_parse_file(): read failed ");
		}/* end while error re read */	

	if(feof(fileptr) )
		{
		fclose(fileptr);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
/* may not be there */
*spaceptr = 0;/* string termination */

/* get the directory relative to / on the server from the filename */
/* copy until last / */
/* this or more: /root/.NewsFleX/http/newsflex/something/index.html */
/* at least: /root/.NewsFleX/http/ */
/* unique indentifier */

ptr1 = strstr(filename, ".NewsFleX/http");
if(! ptr1) return(0);

/* scip the .NewsFleX/http/, point to host/remote_path (could be host/ only) */
ptr1 += 15;/* somehost/[somedir/]somefile */

/* ptr1 now at start host */
ptr2 = strstr(ptr1, "/");

/* there should be a / after host */
if(! ptr2) return(0);

/* point to / after host */

strcpy(temp, ptr2);
/*ptr1 = ptr2;*/ /* /[somedir/]somefile */

ptr1 = temp;

/* find last / if any */
ptr2 = strrchr(ptr1, '/');/* points to start filename or / */
/* there is always at least .NewsFleX/http/host/ */
if(! ptr2) return(0);

 /* cut of the filename part */
/* scip / */
ptr2++;
/* no not want to permanently cut in filename */
c = *ptr2;/* may be zero if dir was empty */
*ptr2 = 0;

dir = strsave(ptr1);
if(! dir) return(0);

/* restore filename */
*ptr2 = c;

if(debug_flag)
	{
	fprintf(stdout, "dir set to=%s\n", dir);
	}

current_url_depth = (flags & 0xff0000) >> 16;
if(debug_flag)
	{
	fprintf(stdout, "http_parse_file(): current_url_depth=%d\n",\
	current_url_depth);
	}

/*
if current_url_depth == 0, the initial host is set to host.
if current_url_depth > 0, and url_host_only_flag is set, the host is compared,
and if it differs, the url is not added to the command queue.
*/
if(current_url_depth == 0)
	{
	if(url_initial_host) free(url_initial_host);
	url_initial_host = strsave(url_current_host);
	if(! url_initial_host) return(0);
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_parse_file(url_initial_host set to=%s\n", url_initial_host);
		}
	}/* end if current_url_depth == 0 */

/* the next one */
current_url_depth++;

/* modify flags */
new_flags = flags &0x00ffff;
new_flags += 65536 * current_url_depth;

base_url = strsave("");
if(! base_url) return(0);

position = space; 
while(1)
	{
	if(! position) break;
	if(*position == 0) break;
	if(*position == '<')
		{
		/* scip any comments */
		ptr1 = scip_comment(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			position++;
			continue;
			}/* end comment */

		/* get any base address */
		ptr1 = find_base(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"http_parse_file(): setting base=%s\n",\
				ptr1);
				}

			free(base_url);
			base_url = strsave(ptr1);
			if(! base_url) return(0);

			free(ptr1);
			position++;
			continue;
			}/* end base */

		if(current_url_depth <= maximum_url_depth)/* was at least 1 */
			{
			/* get any anchors */
			ptr1 = find_a(position, &new_pos);
			position = new_pos;
			if(ptr1)
				{
				/*
				Note flags has been changed by http_parse_file for
				current_url_depth++
				*/
				http_extract_url_and_add_to_command_queue(\
				dir, ptr1, new_flags);
				free(ptr1);
				position++;
				continue;
				}/* end anchor */

			/* get any links */
			ptr1 = find_link(position, &new_pos);
			position = new_pos;
			if(ptr1)
				{
				/*
				Note flags has been changed by http_parse_file for
				current_url_depth++
				*/
				http_extract_url_and_add_to_command_queue(\
				dir, ptr1, new_flags);
				free(ptr1);
				position++;
				continue;
				}/* end link */
			}/* end if url_depth <= maximum_url_depth */

		if(get_pictures_flag)
			{
			/* get any images */
			ptr1 = find_img(position, &new_pos);
			position = new_pos;
			if(ptr1)
				{
				/*
				Note flags has been changed by http_parse_file for
				current_url_depth++
				*/
				http_extract_url_and_add_to_command_queue(\
				dir, ptr1, new_flags);
				free(ptr1);
				position++;
				continue;
				}/* end image */

			/* get any background pictures */
			ptr1 = find_body(position, &new_pos);
			position = new_pos;
			if(ptr1)
				{
				/*
				Note flags has been changed by http_parse_file for
				current_url_depth++
				*/
				http_extract_url_and_add_to_command_queue(\
				dir, ptr1, new_flags);
				free(ptr1);
				position++;
				continue;
				}/* end background picture */
			}/* end if get_pictures_flag */
		}/* end if '<' */
	position++;
	}/* end while all characters in file */

free(space);
free(dir);

return(1);
}/* end function http_parse_file */


int http_extract_url_and_add_to_command_queue(\
char *dir, char *field, int flags)
{
char temp[TEMP_SIZE];
char *ptr;
int already_present_flag;
int allowed;

if(debug_flag)
	{
	fprintf(stdout,\
	"http_extract_url_and_add_to_command_queue():\n\
	arg dir=%s\nfield=%s\nflags=%d\n",\
	dir, field, flags);
	}

/* argument check */
if(! dir) return(0);
if(! field) return(0);

/* do not mess up field */
strcpy(temp, field);

/* do not get local references */
if(temp[0] == '#') return(0);

/* some pit falls */
if(strncasecmp(temp, "mailto", 6) == 0) return(1);
/*
In cgi-bin requests, the /cgi-bin/somescript?someid is converted
to /cgi-bin/somescript~someid (question mark replaced by '~').
This way we have a valid filename.
*/
/*if(strncasecmp(temp, "/cgi-bin", 8) == 0) return(1);*/

/* no java, get some real coffee */
/* to many weard characters, have to think about this function() */
/* if(strncasecmp(temp, "javascript:", 11) == 0) return(1); */

/*
ftp:
http:
/somefile
somefile
*/

if(strncasecmp(temp, "ftp", 3) == 0)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_extract_url_and_add_to_command_queue(): found ftp: abs entry\n");
		}	

	if(! add_inurl(temp, &already_present_flag) ) return(0);
	if(! already_present_flag)
		{
		host_allowed(temp, &allowed, flags);
		if(allowed)
			{
			add_to_command_queue(temp, NONE, URL_GET);
			}
		}
	}/* end if absolute ftp link */
else if(strncasecmp(temp, "http", 4) == 0)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_extract_url_and_add_to_command_queue(): found http: abs entry\n");
		}

	if(! add_inurl(temp, &already_present_flag) ) return(0);
	if(! already_present_flag)
		{
		host_allowed(temp, &allowed, flags);
		if(allowed)
			{
			add_to_command_queue(temp, flags, URL_GET);
			}
		}

	}/* end if absolute http link */
else  /* a relative link, may start with or without '/' */
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_extract_url_and_add_to_command_queue found /rel entry\n");
		}

	/* test if BASE specified (base is not empty string) */

	if(base_url[0] == 0)/* no BASE */
		{
		if(temp[0] == '/')
			{
			/*
			prepend host (http_host_name)
			/image/flower.jpg becomes:
			www.flowers.com/image/flower.jpg
			*/
			
			ptr = malloc(strlen(http_host_name) + strlen(temp) + 1);
			strcpy(ptr, http_host_name);
			strcat(ptr, temp);
			}
		else /* no leading / */
			{
			if(debug_flag)
				{
				fprintf(stdout,\
		"http_extract_url_and_add_to_command_queue found dir rel entry\n");
				}
			/*
			prepend host (http_host_name) and dir (dir always starts with /)
			image/flower.jpg becomes:
			www.flowers.com/dir/image/flower.jpg
			*/
			
			ptr =\
			malloc(strlen(http_host_name) + strlen(dir) + strlen(temp) + 1);
			strcpy(ptr, http_host_name);
			strcat(ptr, dir);
			strcat(ptr, temp);
			}/* end no leading / */
	
		if(! add_inurl(ptr, &already_present_flag) ) return(0);
		if(! already_present_flag)
			{
			host_allowed(ptr, &allowed, flags);
			if(allowed)
				{
				/*
				Note flags has been changed by http_parse_file for
				current_url_depth++
				*/
				add_to_command_queue(ptr, flags, URL_GET);
				}
			}
		}/* end no BASE */	
	else /* BASE specified */	
		{
		if(debug_flag)
			{
			fprintf(stdout,\
"http_extract_url_and_add_to_command_queue(): found base=%s\n", base_url);
			}

		if(temp[0] == '/')
			{
			/*
			prepend base
			/image/flower.jpg becomes:
			base/image/flower.jpg
			*/
		
			ptr = malloc(strlen(base_url) + strlen(temp) + 1);
			strcpy(ptr, base_url);
			strcat(ptr, temp);
			}
		else /* no leading / */
			{
			/*
			prepend base
			image/flower.jpg becomes:
			base/image/flower.jpg
			*/
		
			/* XXXXX */
			/* probably not add dir, more like: base?temp */
			ptr =\
			malloc(strlen(base_url) + strlen(temp) + 1);
			strcpy(ptr, base_url);
			strcat(ptr, temp);

			}/* end no leading / */

		if(! add_inurl(ptr, &already_present_flag) ) return(0);
		if(! already_present_flag)
			{
			host_allowed(ptr, &allowed, flags);
			if(allowed)
				{
				add_to_command_queue(ptr, flags, URL_GET);
				}
			}
		}/* end if BASE specified */
	free(ptr);
	}/* end if relative link */

return(1);
}/* end function http_extract_url_and_add_to_command_queue */


char *scip_comment(char *position, char **new_pos)
{
char *ptr;
int in_comment_flag;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<!", 2) != 0) return(0);
ptr += 2;

in_comment_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(ptr);
		}

	if(! in_comment_flag)
		{
		if(*ptr == '>')
			{
			*new_pos = ptr;
			return(ptr);
			}
		}

/*
	if(*ptr != '-')
		{
		ptr++;
		continue;
		}
*/

	if(strncasecmp(ptr, "--", 2) == 0)
		{
		in_comment_flag = 1 - in_comment_flag;
		ptr += 2;
		continue;
		}

	ptr++;
	}/* end while */
}/* end function scip_comment */


char *find_base(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
char *ptr2;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<BASE", 5) != 0) return(0);
ptr += 5;

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if( (*ptr == 0) || (*ptr == '>') )
		{
		*new_pos = ptr;
		return(0);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;
/*		if(*ptr == '"') in_arg_flag = 0;*/

/*fprintf(stdout, "find_base(): temp=%s\n", temp);*/

		ptr2 = strsave(temp);
		if(! ptr2) return(0);

		*new_pos = ptr;
		return(ptr2);
		}
	ptr++;
	}/* end while */
}/* end function find_base */


char *find_body(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
char *ptr2;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<BODY", 5) != 0) return(0);
ptr += 5;

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if( (*ptr == 0) || (*ptr == '>') )
		{
		*new_pos = ptr;
		return(0);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_BACKGROUND) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "BACKGROUND", 10) == 0)
			{
			mode = IN_BACKGROUND;
			ptr += 10;
			continue;
			}
		else if(strncasecmp(ptr, "TEXT", 4) == 0)
			{
			mode = IN_TEXT;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "BGCOLOR", 7) == 0)
			{
			mode = IN_BGCOLOR;
			ptr += 7;
			continue;
			}		
		else if(strncasecmp(ptr, "LINK", 4) == 0)
			{
			mode = IN_LINK;
			ptr += 4;
			continue;
			}		
		else if(strncasecmp(ptr, "YLINK", 5) == 0)
			{
			mode = IN_YLINK;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "ALINK", 5) == 0)
			{
			mode = IN_ALINK;
			ptr += 5;
			continue;
			}		
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_BACKGROUND) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

/*fprintf(stdout, "find_body(): temp=%s\n", temp);*/

		ptr2 = strsave(temp);
		if(! ptr2) return(0);
		
		*new_pos = ptr;
		return(ptr2);
		}
	ptr++;
	}/* end while */
}/* end function find_body */


char *find_link(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
char *ptr2;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<LINK", 5) != 0) return(0);
ptr += 5;

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if( (*ptr == 0) || (*ptr == '>') )
		{
		*new_pos = ptr;
		return(0);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "URN", 3) == 0)
			{
			mode = IN_URN;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "REL", 3) == 0)
			{
			mode = IN_REL;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "REV", 3) == 0)
			{
			mode = IN_REV;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "TITLE", 5) == 0)
			{
			mode = IN_TITLE;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "METHODS", 7) == 0)
			{
			mode = IN_METHODS;
			ptr += 7;
			continue;
			}		
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

/*fprintf(stdout, "find_link(): temp=%s\n", temp);*/

		ptr2 = strsave(temp);
		if(! ptr2) return(0);
		
		*new_pos = ptr;
		return(ptr2);
		}
	ptr++;
	}/* end while */
}/* end function find_link */


char *find_a(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
char *ptr2;
*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<A", 2) != 0) return(0);
ptr += 2;

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if( (*ptr == 0) || (*ptr == '>') )
		{
		*new_pos = ptr;
		return(0);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "NAME", 4) == 0)
			{
			mode = IN_NAME;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "URN", 3) == 0)
			{
			mode = IN_URN;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "REL", 3) == 0)
			{
			mode = IN_REL;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "REV", 3) == 0)
			{
			mode = IN_REV;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "TITLE", 5) == 0)
			{
			mode = IN_TITLE;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "METHODS", 7) == 0)
			{
			mode = IN_METHODS;
			ptr += 7;
			continue;
			}		
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

/*fprintf(stdout, "find_a(): temp=%s\n", temp);*/

		ptr2 = strsave(temp);
		if(! ptr2) return(0);
		
		*new_pos = ptr;
		return(ptr2);
		}
	ptr++;
	}/* end while */
}/* end function find_a */


char *find_img(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
char *ptr2;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<IMG", 4) != 0) return(0);
ptr += 4;

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if( (*ptr == 0) || (*ptr == '>') )
		{
		*new_pos = ptr;
		return(0);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_SRC) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "SRC", 3) == 0)
			{
			mode = IN_SRC;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALT", 3) == 0)
			{
			mode = IN_ALT;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALIGN", 5) == 0)
			{
			mode = IN_ALIGN;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "ISMAP", 5) == 0)
			{
			mode = IN_ISMAP;
			ptr += 5;
			continue;
			}		
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_SRC) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;
/*		if(*ptr == '"') in_arg_flag = 0;*/

/*fprintf(stdout, "find_img(): temp=%s\n", temp);*/

		ptr2 = strsave(temp);
		if(! ptr2) return(0);
		
		*new_pos = ptr;
		return(ptr2);
		}
	ptr++;
	}/* end while */
}/* end function find_img */


