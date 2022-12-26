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
FILE *local_receiving_fileptr;
FILE *local_sending_fileptr;


void ftp_data_io_read_cb(int data_socketfd, long data)
{
char temp[READSIZE];/* beware of overflow rxbuf=512 + additional text */
int status;
char rxbuf[READSIZE];
size_t bytes_read;
ssize_t bytes_written;
time_t now;

if(debug_flag)
	{
	fprintf(stdout, "ftp_data_io_read_cb(): data_socketfd=%d\n",\
	data_socketfd);
	}

errno = 0;
bytes_read = read(data_socketfd, rxbuf, READSIZE - 1);
if(bytes_read < 0)
	{
	if(errno == EAGAIN) return;/* try again */
		
	if(debug_flag)
		{
		fprintf(stdout,\
		"\nread failed, connection to server lost errno=%d\n", errno);
		}

	perror("read failed: connection to server lost");

	ftp_data_io_cb_error_return(\
	"read failed, connection to www server lost");
	return;
	}/* end if bytes_read < 0 */
if(bytes_read == 0)
	{
	sprintf(temp, "FTP data connection closed by ftp server (EOF)");
	my_command_status_display(temp);
	if(debug_flag) fprintf(stdout, "%s\n", temp);

	fl_remove_io_callback(data_socketfd,\
	FL_READ, (FL_IO_CALLBACK) ftp_data_io_read_cb);

	/* close the server data_socketdf */
	close(data_socketfd);

	ftp_data_server_status = DISCONNECTED;

	/* close the data file */
	fclose(ftp_read_fileptr);

	/* mark file HAVE if not some error in ftp_control_cb */
	if(! ftp_failed_flag)
		{
		/* mark entry in url browser as have it */
		if( get_url_status(url_in_progress, &status) )
			{
			/* reset retriev url if not daily */
			if(! (status & DAILY_URL) )
				{
				status &= (0xffffff - RETRIEVE_URL);
				}

			/* mark have url */
			status |= HAVE_URL;
			set_url_status(url_in_progress, status);

			now = time(0);
			set_url_update_time(url_in_progress, now);

			show_urls(SHOW_SAME_POSITION);
			save_urls();
			show_urls(SHOW_SAME_POSITION);
			}
		}/* end if ! ftp_failed_flag */

	command_in_progress = 0;
	return;
	}/* end if bytes_read == 0 */

/* valid bytes received */

bytes_received += bytes_read;

ftp_bytes_received += bytes_read;
if(debug_flag)
	{
	fprintf(stdout, "ftp_file_size=%ld\n", ftp_file_size);
	fprintf(stdout, "ftp_bytes_received=%ld\n", ftp_bytes_received);
	}
if( (ftp_file_size != 0) && (ftp_file_size != -1) )
	{
	ftp_percentage_received =\
	((float)ftp_bytes_received / (float)ftp_file_size) * 100.0;

	sprintf(temp, "FTP > %s (%2.0f%c)",\
	ftp_local_path_filename, ftp_percentage_received, '%');
	}
else
	{
	if(debug_flag) fprintf(stdout, "unknow ftp file size\n");

	sprintf(temp, "FTP > %s (%c%c%c)", ftp_local_path_filename,\
	'?', '?', '%');
	}
fl_set_object_label(fdui -> command_status_display, temp);

/*
ssize_t write (int FILEDES, const void *BUFFER, size_t SIZE)
*/
          
/*
bytes_written =\
fwrite(rxbuf, bytes_read * sizeof(char), bytes_read, ftp_read_fileptr);
*/

bytes_written = write(fileno(ftp_read_fileptr), rxbuf, bytes_read);
if(bytes_written != bytes_read)
	{
	if(debug_flag)
		{
		fprintf(stdout,
		"Aborting bytes_read=%ld bytes_written=%ld\n",\
		(long)bytes_read, (long)bytes_written);
		}
	ftp_data_io_cb_error_return(\
	"FTP: bytes_written != bytes_read, aborting");
	return;
	}	

return;
}/* end function ftp_data_io_read_cb */


int ftp_data_io_cb_error_return(char *error_message)
{
int status;

if(debug_flag)
	{
	fprintf(stdout,\
	"ftp_data_io_cb_error_return(): arg error_message=%s\n", error_message);
	}

/* argument check */
if(! error_message) return(0);

ftp_mode = FTP_OFF;

fl_remove_io_callback(ftp_data_socketfd,\
FL_READ, (FL_IO_CALLBACK) ftp_data_io_read_cb);
close(ftp_data_socketfd);

/* do not hang around to wait for confirmation of quit */
ftp_data_server_status = DISCONNECTED;

process_command_queue_error_flag = 1;
command_in_progress = 0;

if(ftp_command == FTP_RECEIVE)
	{
	fclose(ftp_read_fileptr);

	/* unlink the file !!!!! */
	unlink(ftp_local_path_filename);
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

return(1); 	
}/* end function ftp_data_io_cb_error_return */


