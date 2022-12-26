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

FD_NewsFleX *fdui;


char *extract_keyword_groups(char *keywords)
{
/* read from the groups.dat file, if keywords match append to result */
int a;
char *result;
FILE *allgroupsfile;
char temp[TEMP_SIZE];
char group_name[TEMP_SIZE];
char posting_allowed_flag;
int subscribed_flag, filter_flags, group_status;
long article_ptr, new_articles;
char *ptr;
int first_group_flag;

if(debug_flag)
	{
	fprintf(stdout, "extract_keyword_groups(): arg keywords=%s\n", keywords);
	}

/* argument check */
if(! keywords) return(0);

sprintf(temp, "%s/.NewsFleX/%s/groups.dat", home_dir, news_server_name);
allgroupsfile = fopen(temp, "r");
if(! allgroupsfile)
	{
	fl_show_alert("Cannot open file", temp, "for read", 0);
	return(0);
	}

result = strsave("");

/* let user know what is happening */
fl_set_object_label(fdui -> command_status_display,\
"Scanning all groups, this may take a while");
/* force display */
XSync(fl_get_display(), 0);

/* read all lines from groups.dat */
first_group_flag = 1;
while(1)
	{	
	a = readline(allgroupsfile, temp);
	if(a == EOF)
		{
		fclose(allgroupsfile);
		break;
		}

	sscanf(temp, "%s %c %d %ld  %d %ld %d",\
	group_name, &posting_allowed_flag, &subscribed_flag, &article_ptr,\
	&group_status, &new_articles, &filter_flags);

	if(search_in_for(group_name, keywords, 0) )/* 0 for case insensitive */
		{
		if(first_group_flag)
			{
			/* create space */
			ptr = malloc(strlen(group_name) + 1);

			sprintf(ptr, "%s", group_name);	
			first_group_flag = 0;
			}
		else
			{
			/* create space */
			ptr =\
			malloc(strlen(result) + strlen(",") + strlen(group_name) + 1);

			sprintf(ptr, "%s,%s", result, group_name);
			}

		/* free old result */
		free(result);

		/* new result is ptr */
		result = ptr;
		}/* end keywords present in group_name */
	}/* end all lines in groups.dat */	

/* clear status info */
fl_set_object_label(fdui -> command_status_display, "");

return(result);
}/* end function extract_keyword_groups */

