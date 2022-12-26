/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997 <Jan Mourer>
mail: pante@pi.net
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

FL_APPEVENT_CB an_idle_cb;


int free_urldata(struct urldata *pa)
{

if(! pa) return(0);

free(pa -> scheme);
free(pa -> user);
free(pa -> password);
free(pa -> host);
free(pa -> url_path);
free(pa -> localpathfilename);
free(pa -> localpath);
free(pa -> localfilename);
free(pa);
return(1);
}/* end function free_urldata */


struct urldata *parse_url(char *url)
{
struct urldata *pa;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
char *ptr1, *ptr2, *ptr3;
char *scheme_specific_part;
int c, i;
char remote_location[TEMP_SIZE];
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "parse_url(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

pa = (struct urldata*) malloc(sizeof(struct urldata) );
if(! pa) return(0);

/* parse URL (from rfc1738.txt */
/* <scheme>:<scheme_specific_part> */
strcpy(temp, url);
ptr1 = strstr(temp, ":");
if(! ptr1)
	{
	pa -> scheme = strsave("http");
	if(! pa -> scheme) return(0);
	sprintf(temp2, "//%s", url); 
	scheme_specific_part = strsave(temp2);
	if(! scheme_specific_part) return(0);
	}
else
	{
	scheme_specific_part = strsave(ptr1 + 1);/* the rest of it minus the ':' */
	if(! scheme_specific_part) return(0);
	*ptr1 = 0;

	/* temp now holds the scheme */
	pa -> scheme = strsave(temp);
	if(! pa -> scheme) return(0);
	}

if(debug_flag)
	{
	fprintf(stdout, "parse_url(): scheme=%s\n", pa -> scheme);
	fprintf(stdout,\
	"parse_url(): scheme_specific_part=%s\n", scheme_specific_part);
	}
/* parse the scheme_specific_part */
/* test for start with // */
/* //<user>:<password>@<host>:<port>/<url_path> */
if(strstr(scheme_specific_part, "//") != scheme_specific_part) return(0);

/* test if we have a username password */
ptr2 = strstr(scheme_specific_part, "@");
if(ptr2)
	{
	strcpy(temp, scheme_specific_part + 2);/* get rid of the leading // */

	/* extract password */
	ptr1 = strstr(temp, ":");
	if(! ptr1)
		{
		pa -> password = strsave("");/* no password */
		if(! pa -> password) return(0);
		}
	else
		{	
		strcpy(temp2, ptr1 + 1);/* get rid of leading : */
		ptr3 = strstr(temp2, "@");/* end of <user>:<password> */
		if(! ptr3) return(0);/* format error */
		*ptr3 = 0;
		pa -> password = strsave(temp2);
		if(! pa -> password) return(0);
		}

	/* extract user */		
	ptr1 = strstr(temp, ":");
	if(ptr1) *ptr1 = 0;/* cut of password */
	pa -> user = strsave(temp);
	if(! pa -> user) return(0);
	ptr2++;/* get rid of the @, point to the host */
	}
else
	{
	pa -> user = strsave("");
	if(! pa -> user) return(0);
	pa -> password = strsave("");
	if(! pa -> password) return(0);
	ptr2 = scheme_specific_part + 2;/* point to the host */
	}
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): password=%s\n", pa -> password);	
	fprintf(stdout, "parse_url(): user=%s\n", pa -> user);
	}
/* <host>:<porkut>/<url_path> to temp */
strcpy(temp, ptr2);
/*fprintf(stdout, "<host>:<port>/<url_path>=%s\n", temp);*/

/* extract the url_path */
ptr1 = strstr(temp, "/");
if(ptr1)
	{
	pa -> url_path = strsave(ptr1); /* leave leading / */
	if(! pa -> url_path) return(0);
	/* get <host>:port> */
	*ptr1 = 0;/* get rid of everything after / */
	}
else
	{
	pa -> url_path = strsave("/");
	if(! pa -> url_path) return(0);
	}
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): url_path=%s\n", pa -> url_path);
	}
/* temp now holds <host>:<port> */

/* test for port */
ptr2 = strstr(temp, ":");
if(! ptr2) pa -> port = -1;/* signal to calling to use default port */
else pa -> port = atoi(ptr2 + 1);/* get rid of the : */
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): port=%d\n", pa -> port);
	}

/* extract host */
if(ptr2) *ptr2 = 0;/* get rid of everything after host */
pa -> host = strsave(temp);
if(! pa -> host) return(0);
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): host=%s\n", pa -> host);
	}
free(scheme_specific_part);

/* some extra stuff */
/*
convert scheme to lower case, so HTTP is http, etc., to prevent
2 http / ftp directory trees from being created.
*/
i = 0;
while(1)
	{
	c = pa -> scheme[i];
	pa -> scheme[i] = tolower(c);
	if(c == 0) break;
	i++;
	}
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): tolower scheme=%s\n", pa -> scheme);		
	}

/* convert host to lower case. */
i = 0;
while(1)
	{
	c = pa -> host[i];
	pa -> host[i] = tolower(c);
	if(c == 0) break;
	i++;
	}
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): tolower host=%s\n", pa -> host);
	}

/* copy the filename part of url_path to localfilename */
/* test if any / present */
ptr2 = strrchr(pa -> url_path, '/');
if(! ptr2)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"parse_url(): cannot find / in url_path, returning 0\n");	
		}
	return(0);
	}
strcpy(temp, ptr2 + 1);
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): temp localfilename=%s\n", temp);
	}

/* set defaults ports and filename */
if( strcasecmp("http", pa -> scheme) == 0)
	{
	if(pa -> port == -1) pa -> port = 80; /* use default port */
	if(strlen(temp) == 0) strcat(temp, "index.html");
	}
if(strcasecmp(pa -> scheme, "ftp") == 0)
	{
	if(pa -> port == -1) pa -> port = 21; /* default port */
	if(strlen(temp) == 0)
		{
		if(debug_flag)
			{
			fprintf(stdout, "parse_url(): invalid ftp filename %s\n", temp);
			}
		return(0);
		}
	}
pa -> localfilename = strsave(temp);
if(! pa -> localfilename) return(0);
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): port set to=%d\n", pa -> port);
	fprintf(stdout, "parse_url(): filename set to=%s\n", pa -> localfilename);
	}

/*
extract the host plus remote path file name from the url.
*/
ptr = strstr(url, "://");/* scip the http:// stuff */
if(! ptr) sprintf(temp, "%s", url); /* like localhost/newsflex/ */
else sprintf(temp, "%s", ptr + 3); /* like http://localhost/newsflex/ */

/* find last '/' in temp, and cut of any filename */
ptr = strrchr(temp, '/');
if(ptr) *ptr = 0;
strcpy(remote_location, temp);
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): remote_location=%s\n", remote_location);
	}
	
 /* host part of remote location to lower case (up until /) */
i = 0;
while(1)
	{
	c = remote_location[i];
	if(c == '/') break;
	remote_location[i] = tolower(c);
	if(c == 0) break;
	i++;
	}

/* add all these together to form a local path */
/*
If host starts with 'www.' remove this (local dirs are the same for 
www.hostname and hostname)
*/
strcpy(temp2, remote_location);
if(strncasecmp(temp2, "www.", 4) == 0)
	{
	ptr = temp2 + 4;
	}
else ptr = temp2;

sprintf(temp, "%s/.NewsFleX/%s/%s",\
home_dir, pa -> scheme, ptr);
pa -> localpath = strsave(temp);
if(! pa -> localpath) return(0);
if(debug_flag)
	{
	fprintf(stdout, "parse_url(): localpath=%s\n", pa -> localpath);
	}

/* combine pa -> localpath  with extracted filename */
sprintf(temp, "%s/%s", pa -> localpath, pa -> localfilename); 
pa -> localpathfilename = strsave(temp);
if(! pa -> localpathfilename) return(0);
if(debug_flag)
	{
	fprintf(stdout,\
	"parse_url(): localpathfilename=%s\n", pa -> localpathfilename);	
	}

if(debug_flag)
	{
	fprintf(stdout, "parse_url(): leaving 1 \n");
	}

return(pa);
}/* end function parse_url */


int url_get(char *url, int options)
{
/* called from process_command_queue,  which is called from idle_cb :) */
int a, c, i, j;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
int status;
struct urldata *pa;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "url_get(): arg url=%s option=%d\n",\
	url, options);
	}

/* argument check */
if(! url)
	{
	command_in_progress = 0;
	return(0);
	}
/* parse URL (from rfc1738.txt */
pa = parse_url(url);
if(! pa)
	{
	command_in_progress = 0;
	return(0);
	}

/*
Remember the host, it is copied to initial_host at url_depth = 0,
then in case url_host_only_flag is set, compared against in http_parse()
*/
if(url_current_host) free(url_current_host);
url_current_host = strsave(pa -> host);
if(! url_current_host)
	{
	free_urldata(pa);
	return(0);
	}

/*
let the callback know what we are doing, so in case of error it can
mark that url as failed.
*/
if(url_in_progress) free(url_in_progress);
url_in_progress = strsave(url);
if(! url_in_progress)
	{
	free_urldata(pa);
	command_in_progress = 0;
	return(0);
	}

if(debug_flag)
	{
	fprintf(stdout, "url_in_progress set to=%s\n", url_in_progress);
	}

url_flags = options;

/* test for http scheme */
if( strcasecmp("http", pa -> scheme) == 0)
	{
/*
	url_depth = (options & 0xff0000) >> 16;
	if(debug_flag)
		{
		fprintf(stdout, "url_get(): options=%d url_depth set to=%d\n",\
		(int)options, url_depth);
		}
*/

	/*
	This is used by http_parse() and http_write_modified_file()
	for reconstruction of relative references.
	*/
	if(http_host_name) free(http_host_name);
	http_host_name = strsave(pa -> host);
	if(! http_host_name)
		{
		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}

	/* connect to http server */
	if(! connect_to_http_server(pa -> host, pa -> port) )
		{
		/* clear command status display */
		sprintf(temp, "Connection to http server %s port %d failed",\
		pa -> host, pa -> port);
		to_command_status_display(temp);
		to_error_log(temp);

		/* mark this entry as failed */
		if( get_url_status(url_in_progress, &status) )
			{
			status &= (0xffffff - RETRIEVE_URL);
			status |= FAILED_URL;

			set_url_status(url_in_progress, status);
			show_urls(SHOW_SAME_POSITION);
			save_urls();
			}

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}

	if(debug_flag)
		{
		fprintf(stdout, "url_get(): http adding io_callback\n");
		}

	fl_add_io_callback(\
	http_socketfd, FL_READ, (FL_IO_CALLBACK) http_io_read_cb, 0);

	/*
	Create a local destination directory.
	For http files go to ~/.NewsFleX/http/ip_address/url_path
	*/

	if(debug_flag)
		{
		fprintf(stdout,\
		"url_get(): http creating all directories in path=\n%s\n",\
		pa -> localpath);
		}

	/*
	now have to create directories for all the dirs in the localpath
	Any or all of these may already be there,
	*/

	/* do not want to try to create like ~/.NewsFleX all the time */
	sprintf(temp2, "%s/.NewsFleX", home_dir);

	if(debug_flag) fprintf(stdout, "starting directory creation\n");
	/* start at the very beginning */		
	i = 0;
	while(1)
		{
/*
XXXXXXX this does not work yet, see file nasa-failed,
local directory is correctly created,
and sending correctly to server:

creating directory=/root/.NewsFleX/http/nsf.gov/od/lpa/news/press/99
creating directory=/root/.NewsFleX/http/nsf.gov/od/lpa/news/press/99/../images
send_to_http_server(): arg
text=GET /od/lpa/news/press/99/../images/one.jpg HTTP/1.0
 (http_socketfd=4)
 txbuf=GET /od/lpa/news/press/99/../images/one.jpg HTTP/1.0M

server says:
http_io_read_cb(): socket=4
http_io_read_cb(): HTTP/1.0 404 Not foundM
Server: Netscape-Enterprise/2.01M
Date: Sat, 11 Dec 1999 15:51:14 GMTM
Content-length: 207M
Content-type: text/htmlM
M
0:36:03 GMTM
Content-length: 74M
Content-type: image/gifM
M

See also ftp below.
*/


		/* set marker if '..' preceeds '/' */
		a = 0;
		/* prevent out of range indexing */
		if(i >= 2)
			{
			if( (pa -> localpath[i - 1] == '.') &&\
			(pa -> localpath[i - 2] == '.') )
				{
				a = 1;
				}
			}/* end if i >= 2 */

		c = pa -> localpath[i];
		if( (c == 0) || (!a && (c == '/') ) )
			{
			/*
			create correct directory if ../
			example:
			http://www.nsf.gov/od/lpa/news/press/99/../images/one.jpg
			*/
			j = 0;
			while(1)
				{
				temp[j] = pa -> localpath[j];
				if(j == i) break;
				j++;
				}
			temp[j] = 0;
		
			if(strlen(temp) > strlen(temp2) )
				{
				if(debug_flag)
					{
					fprintf(stdout, "creating directory=%s\n", temp);
					}
				a = mkdir(temp, S_IRWXU); /* 0 = OK */
				if(a)
					{
					if(errno != EEXIST)
						{
						if(debug_flag)
							{
							fprintf(stdout,\
							"url_get(): cannot create directory %s\n",\
							temp);
							}
						perror("url_get(): ");

						http_command = HTTP_OFF;
		
						free_urldata(pa);
						sprintf(temp2,\
						"url_get(): Cannot create directory %s", temp);
                       http_io_cb_error_return(temp2);

						command_in_progress = 0;
						return(0);
						}
					}/* end if error creating directory */
				}/* end if strlen */
			}/* end if c is '/' or 0 */
		if(c == 0) break; /* end localpath */
		i++;
		}/* end while all characters in pa -> localpath */

	if(http_local_path_filename) free(http_local_path_filename);
	http_local_path_filename = strsave(pa -> localpathfilename);
	if(! http_local_path_filename)/* might as well exit, disk full ? */
		{
		http_command = HTTP_OFF;
		
		free_urldata(pa);

		http_io_cb_error_return(\
		"url_get(): Cannot malloc http_local_path_filename");

		command_in_progress = 0;
		return(0);
		}
	/*
	Going to try a trick to get results of a cgi-bin request in the form:
	 /photojournal.jpl.nasa.gov/cgi-bin/PIAGenCatalogPage.pl?PIA01610"
	written to a file.
	Note the question mark.
	Cannot use that in filenames (ambiguous).
	So replacing '?' with '~'	
	*/
	ptr = strchr(http_local_path_filename, '?');
	if(ptr) *ptr = '~';

	if(debug_flag)
		{
		fprintf(stdout,\
"url_get(): opening read_fileptr for local http_local_path_filename=%s\n",\
		http_local_path_filename);
		}
		
	/*
	Cannot do this in callback, no way to know about server, no usable
	handshake, server may be 0.9.
	*/
	http_read_fileptr = fopen(http_local_path_filename, "w");
	if(! http_read_fileptr)
		{
		if(debug_flag)
			{
			fprintf(stdout, "url_get(): cannot open %s for write\n",\
			http_local_path_filename);
			}

		free_urldata(pa);

		/*
		This forces http_io_cb_error_return() NOT to close http_read_fileptr
		*/
		http_command = HTTP_OFF;
		
		sprintf(temp,\
		"Cannot open %s for write",  http_local_path_filename);
		http_io_cb_error_return(temp);

		return(0);
		}

	sprintf(temp,\
	"GET %s HTTP/1.0\n", pa -> url_path);
	if(! send_to_http_server(temp) )
		{
		fclose(http_read_fileptr);
		unlink(temp);

		http_io_cb_error_return("could not sent to http server");

		free_urldata(pa);

		command_in_progress = 0;
		return(0);
		}

	if(options & KEEP_ALIVE_URL)
		{
		sprintf(temp,\
		"Connection: Keep-Alive\n");
		if(! send_to_http_server(temp) )
			{
			fclose(http_read_fileptr);
			unlink(temp);
			http_io_cb_error_return("could not sent to http server");

			free_urldata(pa);
			command_in_progress = 0;
			return(0);
			}
		}/* end if options & keep_alive_url */
			
	sprintf(temp,\
	"Pragma: no-cache\n");
	if(! send_to_http_server(temp) )
		{
		fclose(http_read_fileptr);
		unlink(temp);
		http_io_cb_error_return("could not sent to http server");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}
	
	sprintf(temp,\
	"Host: %s\n", pa -> host);
	if(! send_to_http_server(temp) )
		{
		fclose(http_read_fileptr);
		unlink(temp);
		http_io_cb_error_return("could not sent to http server");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}
	
	sprintf(temp,\
	"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*\n");
	if(! send_to_http_server(temp) )
		{
		fclose(http_read_fileptr);
		unlink(temp);
		http_io_cb_error_return("could not sent to http server");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}
		
	/* last line empty line marks end headers (\n\n) */
	sprintf(temp,\
	"User-Agent: %s\n\n", USER_AGENT);
	if(! send_to_http_server(temp) )
		{
		fclose(http_read_fileptr);
		unlink(temp);
		http_io_cb_error_return("could not sent to http server");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}

	http_command = HTTP_GET;
	
	http_bytes_received = 0;
	http_first_byte_flag = 1;
	
	}/* end if scheme is http */

else if(strcasecmp("ftp", pa -> scheme) == 0)/* test for ftp schemre */
	{
	if(strlen(pa -> user) == 0)
		{
		if(ftp_user) free(ftp_user);
		ftp_user = strsave("anonymous");
		if(! ftp_user)
			{
			free_urldata(pa);
			command_in_progress = 0;
			return(0);
			}
		}
	else
		{
		if(ftp_user) free(ftp_user);
		ftp_user = strsave(pa -> user);
		if(! ftp_user)
			{
			free_urldata(pa);
			command_in_progress = 0;
			return(0);
			}
		}
	if(debug_flag)
		{
		fprintf(stdout, "url_get(): ftp user=%s\n", ftp_user);
		}

	if(strlen(pa -> password) == 0)
		{
		if(ftp_password) free(ftp_password);
		ftp_password = strsave(user_email_address);
		if(! ftp_password)
			{
			free_urldata(pa);
			command_in_progress = 0;
			return(0);
			}	
		}
	else
		{
		if(ftp_password) free(ftp_password);
		ftp_password = strsave(pa -> password);
		if(! ftp_password)
			{
			free_urldata(pa);
			command_in_progress = 0;
			return(0);
			}
		}
	if(debug_flag)
		{
		fprintf(stdout, "url_get(): ftp password=%s\n", ftp_password);
		}
		
	if(! connect_to_ftp_server(pa -> host, pa -> port, &ftp_control_socketfd) )
		{
		/* clear command status display */
		sprintf(temp, "Connection to ftp server %s port %d failed",\
		pa -> host, pa -> port);
		to_command_status_display(temp);
		to_error_log(temp);
		ftp_control_server_status = DISCONNECTED;

		/* mark this entry as failed */
		if( get_url_status(url_in_progress, &status) )
			{
			status &= (0xffffff - RETRIEVE_URL);
			status |= FAILED_URL;
			set_url_status(url_in_progress, status);
			show_urls(SHOW_SAME_POSITION);
			save_urls();
			}

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}
	ftp_control_server_status = CONNECTED;
	
	ftp_control_port = pa -> port;
	ftp_data_port = pa -> port - 1;

	if(debug_flag)
		{
		fprintf(stdout, "ftp_control_port=%d ftp_data_port=%d\n",\
		ftp_control_port, ftp_data_port);
		}

	if(debug_flag)
		{
		fprintf(stdout, "url_get(): ftp adding ftp_control_io_callback\n");
		}	

	fl_add_io_callback(\
	ftp_control_socketfd, FL_READ,\
	(FL_IO_CALLBACK) ftp_control_io_read_cb, 0);

	/*
	Create a local destination directory.
	For ftp files go to ~/.NewsFleX/ftp/ip_address/url_path
	*/

	if(debug_flag)
		{
		fprintf(stdout,\
		"url_get(): ftp creating all directories in path=\n%s\n",\
		pa -> localpath);
		}

	/*
	now have to create directories for all the dirs in the localpath
	Any or all of these may already be there,
	*/

	/* do not want to try to create like ~/.NewsFleX all the time */
	sprintf(temp2, "%s/.NewsFleX", home_dir);

	if(debug_flag)
		{
		fprintf(stdout, "starting directory creation\n");
		}

	/* start at the very beginning */		
	i = 0;
	while(1)
		{
		/* set marker if '..' preceeds '/' */
		a = 0;
		/* prevent out of range indexing */
		if(i >= 2)
			{
			if( (pa -> localpath[i - 1] == '.') &&\
			(pa -> localpath[i - 2] == '.') )
				{
				a = 1;
				}
			}/* end if i >= 2 */

		c = pa -> localpath[i];
		if( (c == 0) || (!a && (c == '/') ) )
			{
			/*
			create correct directory if ../
			example:
			ftp://www.nsf.gov/od/lpa/news/press/99/../images/one.jpg
			*/
			j = 0;
			while(1)
				{
				temp[j] = pa -> localpath[j];
				if(j == i) break;
				j++;
				}
			temp[j] = 0;
		
			if(strlen(temp) > strlen(temp2) )
				{
				if(debug_flag)
					{
					fprintf(stdout, "creating directory=%s\n", temp);
					}
				a = mkdir(temp, S_IRWXU); /* 0 = OK */
				if(a)
					{
					if(errno != EEXIST)
						{
						if(debug_flag)
							{
							fprintf(stdout,\
							"url_get(): cannot create directory %s\n",\
							temp);
							}
						perror("url_get(): ");

						ftp_command = FTP_OFF;
						
						sprintf(temp2,\
						"url_get(): Cannot create directory %s", temp);
				         ftp_control_io_cb_error_return(temp2);

						free_urldata(pa);
						command_in_progress = 0;
						return(0);
						}
					}/* end if error creating directory */
				}/* end if strlen */
			}/* end if c is '/' or 0 */
		if(c == 0) break; /* end pa -> localpath  */
		i++;
		}/* end while all characters in pa -> localpath */

	if(ftp_local_path_filename) free(ftp_local_path_filename);
	ftp_local_path_filename = strsave(pa -> localpathfilename);
	if(! ftp_local_path_filename)
		{
		ftp_command = FTP_OFF;
		
		ftp_control_io_cb_error_return(\
		"url_get(): Cannot malloc ftp_local_path_filename");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}

	if(ftp_remote_path_filename) free(ftp_remote_path_filename);
	ftp_remote_path_filename = strsave(pa -> url_path);
	if(! ftp_remote_path_filename)
		{
		ftp_command = FTP_OFF;
		
		ftp_control_io_cb_error_return(\
		"url_get(): Cannot malloc ftp_remote_path_filename");

		free_urldata(pa);
		command_in_progress = 0;
		return(0);
		}
	if(debug_flag)
		{
		fprintf(stdout,\
		"ftp_remote_path_filename=%s\n", ftp_remote_path_filename);	
		}

	/* let cb know what to do */
	ftp_command = FTP_RECEIVE;
	ftp_mode = FTP_CONNECT;
	ftp_data_server_status = DISCONNECTED;

	/* now waiting for reply 200 OK (or other) in ftp_control_io_callback */
	/*
	if the ftp server closes the data connection after the control connection
	failed, this flag, if set, should prevent the url from getting marked HAVE,
	like it would have been on an end of file.
	This global flag is set in ftp_control_cb_error_return()
	*/
	ftp_failed_flag = 0;
	}/* end if ftp scheme */

/* free urldata */
free_urldata(pa);

if(debug_flag)
	{
	fprintf(stdout, "url_get(): returning 1\n");
	}

/* rest handled by xxx_io_read_cb */
return(1);
}/* end function url_get */


int http_post(char *source, char *url)
{

return(1);
}/* end function http_post */


int http_put(char *source, char *url)
{

return(1);
}/* end function http_put */


int host_allowed(char *url, int *allowed, int flags)
{
/* check for host only */
struct urldata *pa;

if(debug_flag)
	{
	fprintf(stdout, "host_allowed(): arg url=%s\n", url);
	}

*allowed = 0;

/* argument check */
if(! url) return(0);

if(! (flags & HOST_ONLY_URL) )
	{
	*allowed = 1;
	return(1);
	}

/*
I am aware that this cause calling parse_url() twice,
but that is why I made it a function :).
*/
pa = parse_url(url);
if(! pa) return(0);

if(strcmp(pa -> host, url_initial_host) == 0) *allowed = 1;

if(debug_flag)
	{
	fprintf(stdout, "host_allowed(): url_initial_host=%s, allowed=%d\n",\
	url_initial_host, *allowed);
	}

/* free url_data */
free_urldata(pa);

return(1);
}/* end function host_allowed */


