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


char *strstr_case_insensitive(char *s, char *t)
{
int i, j, k;
for(i = 0; s[i] != '\0'; i++)
	{
	for(j = i, k = 0; t[k] != '\0' &&  toupper(s[j]) == toupper(t[k]);\
	j++, k++)
	;
	if(t[k] == '\0') return(s + i);
	}
return(0);
}/* end function strstr_case_insensitive */


int show_search_form()
{
fl_show_form(fd_NewsFleX -> search, FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function show_search_form */


int search_all(char *group, char *article, int encoding,\
char *attachment, int have_body_flag, int search_what)
{
int a, c;
char temp[READSIZE];
int match;
FILE *search_filefd;
char path[TEMP_SIZE];
int iptr;
char *strsave();

if(debug_flag)
	{
	fprintf(stdout,\
	"search_all(): arg group=%s article=%s\n\
	encoding=%d attachment=%s have_body_flag=%d, search_what=%d\n\
	using search_flags, search_keywords, search_groups_keywords\n",\
	group, article, encoding, attachment, have_body_flag, search_what);
	}

/* argument check */
if(! group) return(0);
if(! article) return(0);	
if(strlen(group) == 0) return(0);
if(strlen(article) == 0) return(0);
if( (search_what != SEARCH_POSTINGS) && (search_what != SEARCH_ARTICLES) )
	{
	return(0);
	}

/*
int search_articles_flag;
int search_headers_flag;
int search_headers_from_flag;
int search_headers_to_flag;
int search_headers_subject_flag;
int search_headers_references_flag;
int search_headers_others_flag;

int search_bodies_flag;
int search_attachments_flag;
int search_postings_flag;
int search_case_sensitive_flag;
char *search_keywords;
char *search_groups;
*/

/*
whatever we do, we read a file line by line,
and check against search_keywords
*/

if(search_what == SEARCH_ARTICLES)
	{
	sprintf(path, "%s/.NewsFleX/%s/%s/%s/",\
	home_dir, news_server_name, database_name, group);
	}
if(search_what == SEARCH_POSTINGS)
	{
	sprintf(path, "%s/.NewsFleX/postings/%s/",\
	home_dir, postings_database_name);
	}

match = 0;

if(search_headers_flag)
	{
	sprintf(temp, "%shead.%s", path, article);
	search_display_progress("Searching", temp);
	search_filefd = fopen(temp, "r");
	if(!search_filefd)
		{
		fl_show_alert("Cannot open", temp, "for read", 0);
		}		
	else
		{
		while(1)/* for all lines in the file */
			{
			a = readline(search_filefd, temp);
			if(a == EOF) break;

			if(search_headers_from_flag)
				{
				if(strstr(temp, "From:") == temp)
					{
					if(search_in_for(\
					temp, search_keywords, search_case_sensitive_flag) )
						{
						fl_set_object_label(fdui -> command_status_display,\
						"Search match in header From:");
						fclose(search_filefd);
						match = 1;
						break;
						}
					}
				}
			if(search_headers_to_flag)
				{
				if(strstr(temp, "To:") == temp)
					{
					if(search_in_for(\
					temp, search_keywords, search_case_sensitive_flag) )
						{
						fl_set_object_label(fdui -> command_status_display,\
						"Search match in header To:");
						fclose(search_filefd);
						match = 1;
						break;
						}
					}
				}
			if(search_headers_subject_flag)
				{
				if(strstr(temp, "Subject:") == temp)
					{
					if(search_in_for(\
					temp, search_keywords, search_case_sensitive_flag) )
						{
						fl_set_object_label(fdui -> command_status_display,\
						"Search match in header Subject:");
						fclose(search_filefd);
						match = 1;
						break;
						}
					}
				}
			if(search_headers_references_flag)
				{
				if(strstr(temp, "References:") == temp)
					{
					if(search_in_for(\
					temp, search_keywords, search_case_sensitive_flag) )
						{
						fl_set_object_label(fdui -> command_status_display,\
						"Search match in header References:");
						fclose(search_filefd);
						match = 1;
						break;
						}
					}
				}
			if(search_headers_others_flag)
				{
				if(strstr(temp, "From:") == 0)
					{
					if(strstr(temp, "To:") ==  0)
						{
						if(strstr(temp, "Subject:") ==  0)
							{		
							if(strstr(temp, "References:") ==  0)
								{
/*								if(strstr(temp, search_keywords) != 0)*/
/*									{*/
									if(search_in_for(strstr(temp, ": "),\
									search_keywords,\
									search_case_sensitive_flag) )
										{
										fl_set_object_label(\
										fdui -> command_status_display,\
										"Search match in other header lines");
										fclose(search_filefd);
										match = 1;
										break;
										}
/*									}*/
								}/* end no references */
							}/* end no subject */
						}/* end no to */
					}/* end no from */
				}/* end if search_other_headers_flag */
			}/* end while all lines in file */
		}/* end if search headers flag */
	}/* end else file opened ok */
/* what do you mean 'so many brackets' ? */
if(search_bodies_flag)
	{
	if(have_body_flag)
		{
		sprintf(temp, "%sbody.%s", path, article);
		search_display_progress("Searching", temp);
		search_filefd = fopen(temp, "r");
		if(! search_filefd)
			{
			fl_show_alert("Cannot open", temp, "for read", 0);
			}		
		else
			{
			while(1)/* for all lines in the file */
				{
				a = readline(search_filefd, temp);
				if(a == EOF) break;
				if(search_in_for(temp, search_keywords,\
				search_case_sensitive_flag) )
					{
					fl_set_object_label(fdui -> command_status_display,\
					"Search match in body");
					fclose(search_filefd);
					match = 1;
					break;
					}
				}/* end while all lines in file */
			}/* end if file opened OK */
		}/* end if have_body_flag */
	}/* end if search_bodies_flag */
if(search_attachments_flag && (strlen(attachment) != 0) )
	{
	/*
	This will allow a search for the string(s) in data in the possibly
	binary.
	*/
	if(search_what == SEARCH_ARTICLES)
		{
		/* the location in the .NewsFleX/group/ */
		sprintf(temp, "%s%s", path, attachment);
		}
	if(search_what == SEARCH_POSTINGS)
		{
		/* the actual path/filename on this machine */
		sprintf(temp, "%s", attachment);
		}
			
	if(debug_flag) fprintf(stdout, "searching attachment %s\n", temp);
	search_display_progress("Searching attachment", temp);
	search_filefd = fopen(temp, "r");
	if(! search_filefd)
		{
		fl_show_alert("Cannot open", temp, "for read", 0);
		}		
	else
		{
		/* NOTE: file may be BINARY, so readline will fail */
		/*
		now gona split in small 0 terminated blocks, so I can use
		search_in_for()
		*/
		while(1)/* for all characters in the file */
			{
			iptr = 0;
			errno = 0;
			while(1)/* read characters from stream */
				{
				c = getc(search_filefd);

				a = ferror(search_filefd);
				if(a)
					{
	 				if(debug_flag)
	 					{
	 					perror("\nsearch_all(): read failed");
						}
					continue; /* read failed */
					}

				if(feof(search_filefd) ) 
					{
					fclose(search_filefd);
					break;/* EOF */
					}

				temp[iptr] = c;
				if(c == 0) break; /* use this as string */
				iptr++;
				if(iptr >= READSIZE) break;
				}/* end while read a character */ 

			if(feof(search_filefd) ) break;

			temp[iptr] = 0;/* add a string termination, 
												so search_in_for() can work */
			if(search_in_for(\
			temp, search_keywords, search_case_sensitive_flag) )
				{
				fl_set_object_label(fdui -> command_status_display,\
				"Search match in attachment");
				match = 1;
				break;
				}
			}/* end while all parts of file */
		}/* end if attachment file exists */
	}/* end if search_attachments_flag */
	
if(! match) return(0);

if(debug_flag)
	{
	fprintf(stdout, "FOUND group=%s article=%s temp=%s\n",\
	group, article, temp);
	}

/* show why we show */
if(search_what == SEARCH_ARTICLES)
	{
	sprintf(question_string, "FOUND\ngroup %s\narticle %s", group, article);
	}
if(search_what == SEARCH_POSTINGS)
	{
	sprintf(question_string, "FOUND\nposting %s", article);
	}

/* force display */
command_status_string_flag = 0;
ask_question_flag = 1;
return(1);/* match */
}/*end function search_all */


int search_in_for(char *database, char *data, int case_sensitive)
{
/* This function returns 1 if all words in data are present in database */
char *ptra, *ptrb, *ptrc;
int match_failed_flag;
int last_string_flag;
char *strsave();
char *strstr_case_insensitve();

if(debug_flag)
	{
	fprintf(stdout,\
	"search_in_for(): arg database=%s data=%s case_sensitive=%d\n",\
	database, data, case_sensitive);
	}

/* argument check */
if(! database) return(0);
if(! data) return(0);
if( (case_sensitive != 0) && (case_sensitive != 1) ) return(0);

/*
Test if some data,
data may consist of one or more words separated by spaces
*/
if(! data[0]) return(0);/* no data */

/* test if data present in database */
/* for all entries in data */
ptra = data;
ptrb = strsave(data);/* create space */
match_failed_flag = 0;
last_string_flag = 0;
while(1)
	{
	ptrc = ptrb;/* point to start availeble space */
	while(1)/* string in data to ptrb */
		{
		*ptrc = *ptra;
		if(! *ptra) break;/* end data */
		if(*ptra == ' ')/* end of string in data */
			{
			*ptrc = 0;
			ptra++;/* scip the space */
			break;
			}
		ptra++;
		ptrc++;
		}/* end while all characters in string */
	/* test if string present in database */
	if(case_sensitive)
		{
		if(! strstr(database, ptrb) )
			{
			match_failed_flag = 1;
			break;
			}
		}
	else
		{
		if(! strstr_case_insensitive(database, ptrb) )
			{
			match_failed_flag = 1;
			break;
			}
		}/* end if not case sensitive */
	if(! *ptra) break;/* end filter */
	}/* end while all strings in filter */	
free(ptrb);/* release space */
if(match_failed_flag) return(0);
return(1);
}/* end function search_in_for */


int search_display_progress(char *text, char *subject)
{
char temp[TEMP_SIZE];

sprintf(temp, "%s %s", text, subject);
fl_set_object_label(fdui -> command_status_display, temp);
return(1);
}/* end function search_display_progress */


int search_in_for_or(char *database, char *data, int case_sensitive)
{
/* This function returns 1 if any word in data is present in database */
char *ptra, *ptrb, *ptrc;
char *strsave();
char *strstr_case_insensitve();

if(debug_flag)
	{
	fprintf(stdout,\
	"search_in_for_or(): arg database=%s data=%s case_sensitive=%d\n",\
	database, data, case_sensitive);
	}

/* argument check */
if(! database) return(0);
if(! data) return(0);
if( (case_sensitive != 0) && (case_sensitive != 1) ) return(0);

/*
Test if some data,
data may consist of one or more words separated by spaces
*/
if(! data[0]) return(0);/* no data */

/* test if data present in database */
/* for all entries in data */
ptra = data;
ptrb = strsave(data);/* create space */
while(1)
	{
	ptrc = ptrb;/* point to start availeble space */
	while(1)/* string in data to ptrb */
		{
		*ptrc = *ptra;
		if(! *ptra) break;/* end data */
		if(*ptra == ' ')/* end of string in data */
			{
			*ptrc = 0;
			ptra++;/* scip the space */
			break;
			}
		ptra++;
		ptrc++;
		}/* end while all characters in string */
	/* test if string present in database */
	if(case_sensitive)
		{
		if(strstr(database, ptrb) ) return(1);
		}
	else
		{
		if(strstr_case_insensitive(database, ptrb) ) return(1);
		}/* end if not case sensitive */
	if(! *ptra) break;/* end filter */
	}/* end while all strings in filter */	
free(ptrb);/* release space */
return(0);
}/* end function search_in_for */



