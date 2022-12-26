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


int regenerate_articles_dat()
{
char *group;
DIR *dirptr;
struct dirent *eptr;
char directory[TEMP_SIZE];
char pathfilename[TEMP_SIZE];
int delete_flag;
FILE  *fileptr;
long article;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "regenerate_articles_dat(): arg none\n");
	}

/* ask user confirmation */
if(!fl_show_question("Overwrite your articles.dat?", 0) ) /* mouse on no */
	{
	return(0);
	}

/* user sanity test */
ptr = (char *) fl_show_input("Type: yes I want this", "No");
if(! ptr) return(0);
if(strcmp(ptr, "yes I want this") != 0)
	{
	fl_show_alert("Command cancelled", "", "", 0);
	return(0);
	}

group = (char *) fl_show_input("Group name?", "");
if(! group) return(0);
sprintf(directory, "%s/.NewsFleX/%s/%s/%s",\
home_dir, news_server_name, database_name, group);     

/* delete any old articles.dat file */
sprintf(pathfilename, "%s/.NewsFleX/%s/%s/%s/articles.dat",\
home_dir, news_server_name, database_name, group);     
unlink(pathfilename);

/* create an empty articles.dat file */
fileptr = fopen(pathfilename, "w");
if(! fileptr)
	{
	fl_show_alert(\
	"Cannot open file for write", pathfilename, "command cancelled", 0);
	return(0);
	}
fclose(fileptr);

/* load the empty file into the structure */
load_articles(group);

/* read what is in the group directory, and add it to the structure */
dirptr = opendir(directory);
if(! dirptr)
	{
	fl_show_alert(\
	"No such group directory", directory, "command cancelled", 0);
	return(0);
	}
 
to_command_status_display(\
"regenerating articles.dat, this may take a while");

while(1)
	{
	eptr = readdir(dirptr);
	if(!eptr) break;
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", eptr -> d_name);
		}

	/* test for head.xxxx */
	if(strstr(eptr -> d_name, "head.") == eptr -> d_name)
		{
		sscanf(eptr -> d_name, "head.%ld", &article);
		/* if valid article number, add to structure */
		if(article >= 0)
			{
			/* delete_flag not used here */
			if(! add_article_header(group, article, &delete_flag) )
				{
				fl_show_alert(\
				"regenerate_articles_dat():",\
				"add_article_header() failed",\
				"command cancelled", 0);
				return(0);
				}/* end if add_article_header() failed */
	
			/* lock this header */
			if(! set_lock_flag(group, article, 1) )
				{
				fl_show_alert(\
				"regenerate_articles_dat():",\
				"set_lock_flag() failed",\
				"command cancelled", 0);
				return(0);
				}
						
			/* test if body exists */
			sprintf(pathfilename, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
			home_dir, news_server_name, database_name, group, article);	
			fileptr = fopen(pathfilename, "r");
			if(fileptr)
				{
				fclose(fileptr);

				/* mark body present, read, locked */
				if(! regenerate_mark_article(group, article) )
					{
					fl_show_alert(\
					"regenerate_articles_dat():",\
					"regenerate_mark_article() failed",\
					"command cancelled", 0);
					return(0);
					}/* end if regenerate_mark_article() failed */
				}/* end if body exists */
			}/* end if article valid */
		}/* end if head. detected */
	}/* end while all directory entries */
(void) closedir(dirptr);

save_articles();

to_command_status_display("");

return(1);
}/* end function regenerate_articles_dat */


int regenerate_postings_dat()
{
char *ptr;
DIR *dirptr;
struct dirent *eptr;
char directory[TEMP_SIZE];
char pathfilename[TEMP_SIZE];
char pathfilename2[TEMP_SIZE];
FILE *fileptr;
long posting;
int body_flag;
int attachment_flag;
int local_custom_headers_flag;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "regenerate_postings_dat(): arg none\n");
	}

/* ask user confirmation */
if(!fl_show_question("Overwrite your postings.dat?", 0) ) /* mouse on no */
	{
	return(0);
	}

/* user sanity test */
ptr = (char *) fl_show_input("Type: yes I want this", "No");
if(! ptr) return(0);
if(strcmp(ptr, "yes I want this") != 0)
	{
	fl_show_alert("Command cancelled", "", "", 0);
	return(0);
	}

sprintf(directory, "%s/.NewsFleX/postings/%s/",\
home_dir, postings_database_name);

sprintf(pathfilename, "%s/.NewsFleX/postings/%s/postings.dat",\
home_dir, postings_database_name);
sprintf(pathfilename2, "%s/.NewsFleX/postings/%s/postings.dat.old",\
home_dir, postings_database_name);

/* remove any old backup */
unlink(pathfilename2);

/* rename .dat to .dat.old */
rename(pathfilename, pathfilename2);

/* create an empty postings.dat file */
fileptr = fopen(pathfilename, "w");
if(! fileptr)
	{
	fl_show_alert(\
	"Cannot open file for write", pathfilename, "command cancelled", 0);
	return(0);
	}
fclose(fileptr);

/* load the empty file into the structure */
load_postings();

/* read what is in the group directory, and add it to the structure */
dirptr = opendir(directory);
if(! dirptr)
	{
	fl_show_alert(\
	"No such directory", directory, "command cancelled", 0);
	return(0);
	}
 
sprintf(temp,\
"regenerating %s/.NewsFleX/postings/%s/postings.dat, this may take a while",\
home_dir, postings_database_name);
to_command_status_display(temp);

while(1)
	{
	eptr = readdir(dirptr);
	if(!eptr) break;
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", eptr -> d_name);
		}

	/* test for head.xxxx */
	if(strstr(eptr -> d_name, "head.") == eptr -> d_name)
		{
		sscanf(eptr -> d_name, "head.%ld", &posting);
		/* if valid posting number, add to structure */
		if(posting >= 0)
			{
			if(debug_flag) fprintf(stdout, "posting=%ld\n", posting);

			/* open header file for read */
			sprintf(pathfilename, "%s/.NewsFleX/postings/%s/head.%ld",\
			home_dir, postings_database_name, posting);	
			fileptr = fopen(pathfilename, "r");
			if(fileptr) fclose(fileptr);
			else
				{
				fl_show_alert(\
				"regenerate_postings_dat(): cannot open file",\
				pathfilename,\
				"command cancelled", 0);
				return(0);
				}/* end if header does not exist */

			/* test if body exists */
			sprintf(pathfilename, "%s/.NewsFleX/postings/%s/body.%ld",\
			home_dir, postings_database_name, posting);	
			fileptr = fopen(pathfilename, "r");
			if(fileptr)
				{
				fclose(fileptr);
				body_flag = 1;
				}
			else
				{
				body_flag = 0;
				fl_show_alert(\
				"regenerate_postings_dat(): cannot open file",\
				pathfilename,\
				"ignored", 0);
				}/* end if body does not exist */

			/* test if  custom header exists */
			sprintf(pathfilename, "%s/.NewsFleX/postings/%s/custom.%ld",\
			home_dir, postings_database_name, posting);	
			fileptr = fopen(pathfilename, "r");
			if(fileptr)
				{
				if(debug_flag) fprintf(stdout, "custom header exists\n");
				local_custom_headers_flag = 1;
				fclose(fileptr);
				}/* end if custom header exists */
			else
				{
				local_custom_headers_flag = 0;
				}

			/* test if attachment exists */
			sprintf(pathfilename, "%s/.NewsFleX/postings/%s/attachment.%ld",\
			home_dir, postings_database_name, posting);
			fileptr = fopen(pathfilename, "r");
			if(fileptr)
				{
				if(debug_flag) fprintf(stdout, "attachment exists\n");
				fclose(fileptr);
				attachment_flag = 1;
				}/* end if custom header exists */
			else
				{
				attachment_flag = 0;
				}

			if(! regenerate_posting(\
			posting, body_flag, local_custom_headers_flag, attachment_flag) )
				{
				fl_show_alert(\
				"regenerate_postings_dat(): regenerate_posting() failed",\
				"", "aborting", 0);
				return(0);
				}

			}/* end if posting valid */
		}/* end if head. detected */
	}/* end while all directory entries */
(void) closedir(dirptr);

save_postings();

to_command_status_display("");

return(1);
}/* end function regenerate_postings.dat */


