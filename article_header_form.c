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


int show_article_header(char *group, long article_id)
{
int c;
char temp[TEMP_SIZE];
FILE *load_filefd;
struct stat *statptr;
char *space;
char *spaceptr;
char *expanded_space;

if(debug_flag)
	{
	fprintf(stdout, "show_article_header(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(!group) return(0);
if(article_id < 0) return(0);

/* display the article header in the article_header_form_input_field */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
home_dir, news_server_name, database_name, group, article_id);
load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	sprintf(temp, ">%s/head.%ld", group, article_id);
	fl_show_alert("could not load file", temp, "", 0);
	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);
	
fstat(fileno(load_filefd), statptr);

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
"show_posting_body(): malloc could not allocate space for custom headers\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(load_filefd);
		if(! ferror(load_filefd) ) break;
		perror("show_posting_body(): get custom header read failed ");
		}/* end while error re read */	

	if(feof(load_filefd) )
		{
		fclose(load_filefd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

expanded_space = (char *) expand_tabs(space, tab_size);
free(space);

fl_set_input(fdui -> article_header_form_input_field, expanded_space);
free(expanded_space);

return(1);
}/* end function show_article_header */



