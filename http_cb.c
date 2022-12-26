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

time_t last_body_line_time;
time_t now;
int http_mode;
int expect_body_flag;/* else numeric */
char content_type[TEMP_SIZE];


void http_io_read_cb(int socket, long data)
{
int c, i;
char rxbuf[READSIZE];
char temp[READSIZE];/* beware of overflow rxbuf=512 + additional text */
char temp2[READSIZE];
static int num_response;/* preserve between calls !!!!! */
int cr_flag;
static int lf_flag;
int status;
size_t bytes_read;
ssize_t bytes_written;
static char header_space[READSIZE];
static int header_space_ptr;
static int in_header_flag;
char *ptr;
static int data_offset;
FILE *header_fileptr;
time_t now;

if(debug_flag)
	{
	fprintf(stdout, "http_io_read_cb(): socket=%d\n", socket);
	}

errno = 0;
bytes_read = read(http_socketfd, rxbuf, READSIZE - 1);
if(bytes_read < 0)
	{
	if(errno == EAGAIN) return;/* try again */
		
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_io_read_cb():read failed, connection to server lost errno=%d\n",\
		errno);
		}

	perror("read failed: connection to server lost");

	http_io_cb_error_return(\
	"read failed, connection to www server lost");
	return;
	}/* end if bytes_read < 0 */
if(bytes_read == 0)
	{
	sprintf(temp, "Connection closed by http server (EOF)");
	my_command_status_display(temp);
	if(debug_flag) fprintf(stdout, "http_io_read_cb(): %s\n", temp);

	fl_remove_io_callback(http_socketfd,\
	FL_READ, (FL_IO_CALLBACK) http_io_read_cb);

	/* close the server socket */
	close(socket);

	http_server_status = DISCONNECTED;

	if(http_command == HTTP_GET)
		{
		fclose(http_read_fileptr);
		expect_body_flag = 0;
		}
	http_command = HTTP_OFF;
		
	if( get_url_status(url_in_progress, &status) )
		{
		/* reset retieve url if not daily */
		if(! (status & DAILY_URL) )
			{
			status &= (0xffffff - RETRIEVE_URL);
			}/* end if ! daily */
		
		/* mark have url */
		status |= HAVE_URL;
		set_url_status(url_in_progress, status);
		
		now = time(0);
		set_url_update_time(url_in_progress, now);

		save_urls();
		show_urls(SHOW_SAME_POSITION);
		}
	
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_io_read_cb(): http connection closed by server (EOF)\n");
		}

	/*
	Here we start parsing any .html file with protocol http_version,
	any links (pictures, local refences, remote references, etc.), are
	extracted and added to the command queue.
	This must be done before signalling command is complete, in order to
	prevent disconnecting from the net.
	*/
	if(strcmp(content_type, "text/html") == 0)
		{
		/* this will add found links to command_queue */
		http_parse_file(\
		http_local_path_filename, http_version, url_flags);
		
		/*
		ad to the list of files to be modified for local references
		and moved url's.
		*/
		add_html_file(http_local_path_filename, url_flags);
		}/* end if text/html */

	command_in_progress = 0;

	return;
	}/* end if bytes_read == 0 */

/* valid byte received */

bytes_received += bytes_read;

/*
Have to extract the headers somehow, man are not binary formats beautiful
what a waste of resources this ASCII protocol is
Now what if server only suports 0.9 and sends no headers?
*/
if(bytes_read != 0)
	{
	if(http_first_byte_flag)
		{
		lf_flag = 0;
		/* first look for first character, if it is not 'H' it must be 0.9 */
		if(rxbuf[0] != 'H')
			{
			http_version = 0.9;
			in_header_flag = 0;
			}
		else
			{
			in_header_flag = 1;
			header_space_ptr = 0;
			cr_flag = 0;
			}
		data_offset = 0;
		http_first_byte_flag = 0;
		}/* end if http_first_byte_flag */
	}/* end if bytes read != 0 */

/* store the headers in array header_space_ptr */
if(in_header_flag)
	{
	for(i = 0; i < bytes_read; i++)
		{
		/* beware of overflow */
		if(header_space_ptr >= READSIZE - 1) break;
		c = rxbuf[i];

		header_space[header_space_ptr] = c;
			
		/* Test for empty line */
		if(lf_flag)
			{
			
			/* test for line not empty */
			if( (c != 10) && (c != 13) )
				{
				lf_flag = 0;
				header_space_ptr++;
				continue;
				}

			/* ignore any CR */
			if(c == 13)
				{
				header_space_ptr++;
				continue;
				}
			
			/* empty line, start code */
			if(c == 10)
				{
				if(debug_flag)
					{
					fprintf(stdout,\
					"http_io_read_cb(): %s\n", header_space);
					}

				/* save the header to disk */
				sprintf(temp, "%s.head", http_local_path_filename);

				header_fileptr = fopen(temp, "w");
				if(! header_fileptr)
					{
					if(debug_flag)
						{
						fprintf(stdout,\
						"http_io_read_cb(): Cannot open file %s for write\n",\
						temp);
						}
					/* we do NOT abort on this */
					/* yes we do */
					http_io_cb_error_return(\
					"http_io_cb(): cannot open header file for write");
					return;
					}
				/* not sure this will write all, maybe putc in loop?  */
				if(header_space_ptr !=\
				write(fileno(header_fileptr),\
				header_space, header_space_ptr) )
					{
					fclose(header_fileptr);
					http_io_cb_error_return(\
					"http_io_cb(): write header file failed");
					return;
					}
				fclose(header_fileptr);
				/* end save header to disk */
				/* mark the end of the string header_space for strstr */
				header_space[header_space_ptr + 1] = 0;
				
				in_header_flag = 0;
				/*
				Exit from for, now write data to file.
				i + 1  points to position in buffer where data starts.
				*/
				data_offset = i + 1;
				
				/* get the http version */
				/* HTTP/1.0 200 OK */
				ptr = strstr(header_space, "HTTP/");
				if(! ptr) http_version = 0.9;
				else
					{
					if(3 != sscanf(ptr, "HTTP/%f %d %s",\
					&http_version, &num_response, temp) )
						{
						http_io_cb_error_return(temp);
						return;
						}
					}/* end get http_version */				

				if(debug_flag)
					{
					fprintf(stdout,\
					"http_io_read_cb(): http_version=%.1f num_response=%d\n",\
					http_version, num_response); 				
					}

				sprintf(temp2,\
				"URL %s server says: %s", url_in_progress, rxbuf);

				/* cut the line for a printable display */
				ptr = strchr(temp2, 13);/* CR */
				if(ptr) *ptr = 0;	
				ptr = strchr(temp2, 10);/* LF */ 
				if(ptr) *ptr = 0;
				
				/* for the moment, accept only response 200 */
				switch(num_response)
					{
					case 200: /* OK */
						break;
					case 201: /* Created */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 204: /* Accepted */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 301: /*  Moved permanently */		
						http_io_cb_error_return(temp2);
						return;
						break;
					case 302: /* Moved temporarily */
						/*
						redirect, extract location,
						if not possible:
						unlink local filename (causes problems if trying
						to create a directory with the same name later),
						error return.
						*/
						
						/* beware of Content-Location: */
						ptr = strcasestr(header_space, "\nLocation:");
						if(! ptr)
							{
							ptr = strcasestr(header_space, "\rLocation:");
							}
						if(! ptr)
							{
							/* test for any */
							ptr = strcasestr(header_space, "Location:");
							/* test if first */
							if(ptr != header_space) ptr = 0;
							}
						if(ptr)
							{
							if(2 == sscanf(ptr, "%s %s", temp, temp2) )
								{
								/*
								this is the new location, the remote
								may or may not have the same pathfilename
								*/
								if(debug_flag)
									{
									fprintf(stdout,\
							"http_io_cb(): error 302 new location=%s\n",\
									temp2); 
									fprintf(stdout,\
									"http_local_path_filename=%s\n",\
									http_local_path_filename);
									}							
								/*
								combine with remote path if only / in
								new location?
								No rfc2068.html
								*/ 
								/*
								test for host only, this will set ptr to
								zero if different host
								*/
                                if(debug_flag)                           
                                    {
                                    fprintf(stdout,\
                          "http_io_cb(): http_host_name=%s temp2=%s=%s\n",\
									http_host_name, temp2);
									}
								if(url_flags & HOST_ONLY_URL)
									{
/*
If host (processed = http://cnn.com and new = http://www.cnn.com
then if HOST_ONLY_URL this would fail, so special case added.
http://cnn.com/QUICKNEWS frequently redirects to the same url,
http://www.cnn.com/QUICNEWS.
*/ 
									/* http://somehost/[somedir/]somefile */
									/* test for absolute link */
									if(strncasecmp(temp2,\
									"http://www.", 11) == 0)
										{
										if(debug_flag)
											{
											fprintf(stdout,\
		          	"http_io_cb(): HOST_ONLY and http://www. detected\n");
											}
										/*
										www is prepended, but never
										in http_host_name
										*/
										/* test for different host */
										if(strncmp(http_host_name,\
										temp2 + 11,\
										strlen(http_host_name)) != 0)
											{
											ptr = 0;
											}
										}/* end if http://www. */
									else if(strncasecmp(temp2, "http://", 7) == 0)
										{
										/* test for different host */
										if(debug_flag)
											{
											fprintf(stdout,\
			          	"http_io_cb(): HOST_ONLY and http:// detected\n");
											}
										if(strncmp(http_host_name,\
										temp2 + 7,\
										strlen(http_host_name)) != 0)
											{
											ptr = 0;
											}
										}/* end if http:// */
									else \
									if(strncasecmp(temp2, "ftp://", 6) == 0)
										{
										/* test for different host */
										if(strncmp(http_host_name,\
										temp2 + 6,\
										strlen(http_host_name)) != 0)
											{
											ptr = 0;
											}
										}/* end if ftp:// */
									}/* end if host only */
								if(ptr)
									{
									/*
									keep in moved_url database for
									http_write_modified_file()
									*/
									add_new_location(url_in_progress, temp2);
									
									/*
									add to command queue with depth depth.
									*/
									add_to_command_queue(\
									temp2, url_flags, URL_GET);								
									}					
								}/* end if location parsed */
							}/* end if Location: found */
						
						/* error if any error :) */
						if(! ptr)
							{
							http_io_cb_error_return(temp2);
							return;
							}
						break;
					case 304: /* not modified */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 400: /* Bad request */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 401: /* Unauthorized */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 403: /* Forbidden */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 404: /* Not found */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 500 :/* Internal server error */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 501: /* Not implemented */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 502: /* Bad gateway */
						http_io_cb_error_return(temp2);
						return;
						break;
					case 503: /* Service unavailable  */
						http_io_cb_error_return(temp2);
						return;
						break;
					default:
						http_io_cb_error_return(temp2);
						return;
						break;
					}/* end switch num_response */

				/*
				extract http_file_size from header_space here,
				if not possible, use -1 as signal for percentage display
				*/
				/* Content-length: 417514 */
				ptr = strcasestr(header_space, "Content-length:");
				if(! ptr) http_file_size = -1;
				else
					{
					if(2 != sscanf(ptr, "%s %ld", temp, &http_file_size) )
						{
						http_file_size = -1;
						}
					}
				if(debug_flag)
					{
					fprintf(stdout,\
					"http_io_read_cb(): http_file_size=%ld\n",\
					http_file_size);				
					}

				/*
				extract content type, so if html, and pictures we scan
				the file for pictures.
				If not possible, use "" (empty string).
				Content-type: text/html
				*/
				ptr = strcasestr(header_space, "Content-type:");
				if(! ptr) strcpy(content_type, "");
				else
					{
					if(2 != sscanf(ptr, "%s %s", temp, content_type) )
						{
						strcpy(content_type, "");
						}
					}
				if(debug_flag)
					{
					fprintf(stdout,\
					"http_io_read_cb(): content_type =%s\n", content_type);
					}
				
				break;
				}/* end if c == 10 */

			lf_flag = 0;
			header_space_ptr++;
			continue;
			}/* end if lf_flag */

		if(c == 10) lf_flag = 1;

		header_space_ptr++;
		}/* end for reading from rxbuf */
	}/* end if in_header_flag */

if(! in_header_flag)
	{
	ptr = rxbuf;
	ptr += data_offset;/* this should give correct offsett for type char */
		
	bytes_written =\
	write(fileno(http_read_fileptr), ptr, bytes_read - data_offset);
	if(bytes_written != bytes_read - data_offset)
		{
		
		if(debug_flag)
			{
			fprintf(stdout,
			"http_io_read_cb(): Aborting bytes_read=%ld bytes_written=%ld\n",\
			(long)bytes_read, (long)bytes_written);
			}

		http_io_cb_error_return(\
		"HTTP: bytes_written != bytes_read, aborting");
		return;
		}	

	http_bytes_received += bytes_read - data_offset;
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_io_read_cb(): http_file_size=%ld\n", http_file_size);
		fprintf(stdout,\
		"http_io_read_cb(): http_bytes_received=%ld\n", http_bytes_received);
		}

	/* display percentage received, avoid devide by zero and bogus */
	if( (http_file_size == -1) || (http_file_size == 0) )
		{
		sprintf(temp, "HTTP > %s (%c%c%c)",\
		http_local_path_filename, '?', '?', '%');
		}
	else
		{
		http_percentage_received =\
		((float)http_bytes_received / (float)http_file_size) * 100.0;
		sprintf(temp, "HTTP > %s (%2.0f%%)",\
		http_local_path_filename, http_percentage_received);
		}
	fl_set_object_label(fdui -> command_status_display, temp);

	/* from now on use all of the buffer */
	data_offset = 0;
	}/* end if ! header_flag */

return;
}/* end function http_io_read_cb */


int http_io_cb_error_return(char *error_message)
{
int status;

if(debug_flag)
	{
	fprintf(stdout,\
	"http_io_cb_error_return(): arg error_message=%s\n", error_message);
	}

/* argument check */
if(! error_message) return(0);

http_mode = HTTP_OFF;

fl_remove_io_callback(http_socketfd,\
FL_READ, (FL_IO_CALLBACK) http_io_read_cb);
close(http_socketfd);

/* do not hang around to wait for confirmation of quit */
http_server_status = DISCONNECTED;

command_in_progress = 0;

if(http_command == HTTP_GET)
	{
	fclose(http_read_fileptr);
	expect_body_flag = 0;

	/* unlink the file !!!!! */
	unlink(http_local_path_filename);
	}
http_command = HTTP_OFF;
		
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

return(1); 	
}/* end function http_io_cb_error_return */


char *strcasestr(char *str1, char *str2)
{
char *ptr1;
char *ptr2;
char *position;

position = str1;
while(1)
	{
	if(! *position) return(0);

	/* compare from position start */
	ptr1 = position;
	ptr2 = str2;
	while(1)
		{
		/* test for end string 2 first, that would be found */
		if(! *ptr2) return(position);

		/* test for end string 1 first, that would be failed */
		if(! *ptr1) break;

		if(tolower(*ptr1) == tolower(*ptr2) )
			{
			ptr1++;
			ptr2++;
			}
		else break;
		}/* end inner while */
	position++;
	}/* end outher while */
}/* end function strcasestr */



