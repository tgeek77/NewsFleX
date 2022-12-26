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


int show_posting_header(long posting_id)
{
int a, c;
struct stat *statptr;
char *posting_header_space, *global_header_space, *local_header_space;
char *total_space;
char *spaceptr;
char path_filename[TEMP_SIZE];
FILE *posting_headerfd;
FILE *global_headerfd;
FILE *local_headerfd;
int total_length;

if(debug_flag)
	{
	fprintf(stdout, "show_posting_header(): arg posting_id=%ld",\
	posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

/*
The comlete header file consists of:
head.%ld, where %ld is the posting_id,
global_custom_head (if enabled)
custom.%ld (if enabled, where %ld is the posting_id)
Reading files one by one and combining them in the display.
*/

/* read head.%ld */
sprintf(path_filename, "%s/.NewsFleX//postings/%s/head.%ld",\
home_dir, postings_database_name, posting_id);

posting_headerfd = fopen(path_filename, "r");			
if(! posting_headerfd)
	{
	fl_show_alert("Cannot open file", path_filename, "for read", 0);
	return(0);
	}

/* create space for file */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(posting_headerfd), statptr);

posting_header_space = malloc(statptr -> st_size + 1);
free(statptr);
if(! posting_header_space) return(0);

spaceptr = posting_header_space;
/* read from file */
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(posting_headerfd);
		if(! ferror(posting_headerfd) ) break;
		perror("show_posting_header(): read failed ");
		}/* end while error re read */	

	if(feof(posting_headerfd) )
		{
		fclose(posting_headerfd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */

*spaceptr = 0;/* string termination */

total_length = strlen(posting_header_space);

/* test for global custom_headers enabled */
if(! get_custom_headers_flag(posting_id, &a) )
	{
	return(0);
	}
if(a & GLOBAL_HEADERS_ENABLED)
	{
	sprintf(path_filename, "%s/.NewsFleX/global_custom_head",\
	home_dir);

	global_headerfd= fopen(path_filename, "r");			
	if(! global_headerfd)
		{
		fl_show_alert("Cannot open file", path_filename, "for read", 0);
		return(0);
		}

	/* create space for file */
	statptr = (struct stat*) malloc(sizeof(struct stat) );
	if(! statptr) return(0);

	fstat(fileno(global_headerfd), statptr);

	global_header_space = malloc(statptr -> st_size + 1);
	free(statptr);
	if(! global_header_space) return(0);

	spaceptr = global_header_space;
	/* read from file */
	while(1)
		{
		while(1)/* error re read */
			{
			c = getc(global_headerfd);
			if(! ferror(global_headerfd) ) break;
			perror("show_posting_header(): read failed ");
			}/* end while error re read */	

		if(feof(global_headerfd) )
			{
			fclose(global_headerfd);
			break;
			}
		*spaceptr = c;
		spaceptr++;
		}/* end while all lines from posting body */

	*spaceptr = 0;/* string termination */

	total_length += strlen(global_header_space);
	
	}/* end if global custom headers enabled */
else global_header_space = strsave("");

/* test for local custom headers enabled */
if(! get_custom_headers_flag(posting_id, &a) )
	{
	return(0);
	}
if(a & LOCAL_HEADERS_ENABLED)
	{
	sprintf(path_filename, "%s/.NewsFleX/postings/%s/custom.%ld",\
	home_dir, postings_database_name, posting_id);

	local_headerfd= fopen(path_filename, "r");			
	if(! local_headerfd)
		{
		fl_show_alert("Cannot open file", path_filename, "for read", 0);
		return(0);
		}

	/* create space for file */
	statptr = (struct stat*) malloc(sizeof(struct stat) );
	if(! statptr) return(0);

	fstat(fileno(local_headerfd), statptr);

	local_header_space = malloc(statptr -> st_size + 1);
	free(statptr);
	if(! local_header_space) return(0);

	spaceptr = local_header_space;
	/* read from file */
	while(1)	
		{
		while(1)/* error re read */
			{
			c = getc(local_headerfd);
			if(! ferror(local_headerfd) ) break;
			perror("show_posting_header(): read failed ");
			}/* end while error re read */	

		if(feof(local_headerfd) )
			{
			fclose(local_headerfd);
			break;
			}
		*spaceptr = c;
		spaceptr++;
		}/* end while all lines from posting body */

	*spaceptr = 0;/* string termination */

	total_length += strlen(local_header_space);

	}/* end if local custom headers enabled */
else local_header_space = strsave("");

total_space = malloc(total_length + 1);
sprintf(total_space, "%s%s%s",\
posting_header_space, global_header_space, local_header_space);

fl_set_input(fdui -> posting_header_form_input_field, total_space);
free(total_space);

return(1);
}/* end function show_posting_header */


