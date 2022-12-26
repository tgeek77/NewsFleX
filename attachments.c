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

int forked_flag;

int pop_up_desc_info(char *group, char *attachment_name)
{
int c;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
FILE *load_filefd;
char *ptr;
struct stat *statptr;
char *space, *spaceptr;


if(debug_flag)
	{
	fprintf(stdout, "pop_up_desc_inf(): arg group=%s attachment_name=%s",\
	group, attachment_name);
	}

/* parameter check */
if(! group) return(0);
if(! attachment_name) return(0);

/* gone change like: picture.jpg into picture.desc */

strcpy(temp2, attachment_name);
ptr = strstr(temp2, ".");
if(! ptr) return(0);

*ptr = 0;/* replace dot by string termination */
strcat(temp2, ".desc");
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
home_dir, news_server_name, database_name, group, temp2);

if(debug_flag) fprintf(stdout, "opening %s for read\n", temp);

load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
/*	Many times there is no .desc file */
/*	fl_show_alert("could not load file", temp, "", 0); */
	if(debug_flag) fprintf(stdout, "Could not load file %s\n", temp);
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
		"pop_up_desc_info(): malloc could not allocate space\n");
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
		perror("pop_up_desc_info(): read failed ");
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

fl_set_input(fdui -> desc_input, space);

free(desc_input_copy);
desc_input_copy = strsave(space);

fl_set_input_topline(fdui -> desc_input, 1);

free(space);

fl_show_form(fd_NewsFleX -> desc, FL_PLACE_CENTER, FL_NOBORDER, "description");
return(1);
}/* end function pop_up_desc_info */


int launch_binary_attachment(char *group, long article_id)
{
int c, i, j;
char temp[TEMP_SIZE];
extern char *lookup_content_type();
static char *content_type;/* preserve between calls */
char *ptr;
extern char *strsave();
extern long atol();
int mode;

#define WAIT_PARTIAL	0
#define WAIT_ID			1

/* argument check */
if(! group) return(0);
if(! article_id) return(0);

mode = 0;/* only for clean compilation with -Wall */

sprintf(temp, "Launching binary attachment for article %ld", article_id);
my_command_status_display(temp);

forked_flag = 0;/* no helper application for this article or group of
											partial articles started yet. */

/* get the content type */
ptr = lookup_content_type(group, article_id);
if(debug_flag)
	{
	fprintf(stdout,\
	"lookup_content_type returned len=%ld\ns=%s\n", (long) strlen(ptr), ptr);
	}

/* 
NOTE: not using this ptr directly, because it is a structure element
(which may be cleared by acquire).
*/
if(ptr)/* content_type found */
	{
	if(content_type) free(content_type);/* release any previous memory */
	content_type = strsave(ptr);
	if(! content_type) return(0);
	if(debug_flag)
		{
		fprintf(stdout, "content_type=%s\n", content_type);		
		}
	/* 
	a partial message header Content-Type looks like this:
	Content-Type: message/partial;M
	id="855981231@PHOTOG";M
	number=1; total=2M
	*/		          
	/* scan all fields in content_type */
	mode = WAIT_PARTIAL;
	i = 0;
	while(1)/* for all fields in content_type */
		{
		j = 0;
		while(1)/* for all characters in a field */
			{
			c = content_type[i];
			if( (c == ' ') || (c == 10) || (c == 13) ) 
				{
				i++;
				continue;
				}
			temp[j] = c;
			if(c == 0)
				{
				break;/* end of content_type */
				}
			if(c == ';')/* end of field in content_type */
				{
				temp[j] = 0;/* string termination */
				break;
				}
			i++;
			j++;
			}/* end while for all chatecters in a field */
		/* compare the field */
		if(mode == WAIT_PARTIAL)
			{
			ptr = strstr(temp, "message/partial");
			if(ptr) /* partial message */
				{
				/* 
				get the id so we can find the other parts, this id is
				the same for all parts, it is NOT the Message-ID 
				*/
				i++;
				mode = WAIT_ID;
				continue; /* go for "id=" */
				}
			}/* end mode == wait partial */
		if(mode == WAIT_ID)
			{
			ptr = strstr(temp, "id=") + 3;
			if(ptr)
				{
				if(debug_flag)
					{
					fprintf(stdout,\
					"PARTIAL found partial message id: id=%s\n", ptr);				
					}
				decode_articles_with_content_type_id(group, ptr);

				return(1);/* ready */
				}
			}/* end mode == wait id */
		if(c == 0) break;
		i++;/* scip field delimiter ';" */
		}/* end while all fields in content_type */
	}/* end if ptr (content_type present) */

if(mode == WAIT_PARTIAL) 
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"WHOLE content type present, but found no partial\n");
		}
	}
else if(mode == WAIT_ID)
	{
	if(debug_flag)fprintf(stdout, "WHOLE partial found, but no id\n");
	}
else if(debug_flag)fprintf(stdout, "WHOLE no content type present\n");

decode_article(group, article_id, content_type);

return(1);
}/* end function launch_binary_attachment */



int decode_article(char *group, long article_id, char *header_content_type)
{
int a, b, c, i, j, k;
char temp[TEMP_SIZE];
char report[TEMP_SIZE];
FILE *exec_filefd;
pid_t pid;
char attachment_name[TEMP_SIZE];
char content_type_and_subtype[TEMP_SIZE];
char content_type[TEMP_SIZE];
char content_subtype[TEMP_SIZE];
char helper_type[TEMP_SIZE];
char helper_subtypes[TEMP_SIZE];
char helper_program[TEMP_SIZE];
char helper_flags_start[TEMP_SIZE];
char helper_flags[TEMP_SIZE];
int lower_case_flag;
char execv_args[51][TEMP_SIZE];
char *flip[51];/* this way I do not have to free anything */
				/* arguments to ececlv are copied */
FILE *info_filefd;
char *ptr;
int go_by_extention_flag = 0;

if(debug_flag)
	{
	fprintf(stdout,\
	"\ndecode_article(): arg group=%s article_id=%ld header_content_type=%s\n",\
	group, article_id, header_content_type);
 	}

/* erase any old .desc files */
/*
sprintf(temp, "rm %s/.NewsFleX/%s/%s/%s/*.desc",\
home_dir, news_server_name, database_name, group);
exec_filefd = popen(temp, "r");
if(! exec_filefd) return(0);
pclose(exec_filefd);
*/
	
/* cat header plus body together and unpack, send file info to info */
my_command_status_display("Starting munpack");

sprintf(temp,\
"cat %s/.NewsFleX/%s/%s/%s/head.%ld %s/.NewsFleX/%s/%s/%s/body.%ld |\
munpack -f -q -C %s/.NewsFleX/%s/%s/%s > %s/.NewsFleX/%s/%s/%s/info",\
home_dir, news_server_name, database_name, group, article_id,\
home_dir, news_server_name, database_name, group, article_id,\
home_dir, news_server_name, database_name, group,\
home_dir, news_server_name, database_name, group);

exec_filefd = popen(temp, "r");
if(! exec_filefd) return(0);
pclose(exec_filefd);
	
/* read the info file generated by munpack */	
/* info looks like: K2301m.jpg.1 (image/jpeg) */

sprintf(temp, "%s/.NewsFleX/%s/%s/%s/info",\
home_dir, news_server_name, database_name, group);
info_filefd = fopen(temp, "r");
if(! info_filefd)
	{
	return(0);
	}

/* let munpack figure out the content */
/* read one line from info file */
a = readline(info_filefd, temp);/* closes file if only one line */
if(a != EOF) fclose(info_filefd);
	
if(temp[0] == 0)
	{
	fl_show_alert("Multipart picture, also start other parts", "", "", 0);
	return(0);
	}

if(strstr(temp,\
"Did not find anything to unpack from standard input") == temp)
	{
	fl_show_alert("Did not find anything to unpack", "", "", 0);
	return(0);
	}

/* extract attachment name from info file (first field) */	
sscanf(temp, "%s %s", attachment_name, content_type_and_subtype);

/* NOTE: to use header content_type un comment the next line */
/* strcpy(content_type_and_subtype, header_content_type);*/


/* NOTE already done here, if helper fails, we can still erase */
/* 
set the encoding variable, so that next time a double click
is done in the article_list_browser the attachment is launched
*/
set_article_encoding(group, article_id, ATTACHMENT_PRESENT, 1);

/* save the name of the attachment, so we can erase it later if needed */
set_article_attachment(group, article_id, attachment_name);

/* extract the content_subtype */
ptr = strstr(content_type_and_subtype, "/") + 1;
if(ptr)
	{
	strcpy(content_subtype, ptr);
	ptr = strstr(content_subtype, ")");
	if(ptr) *ptr = 0;
	}		
else /* not a usable content_type, set for text display */ 
	{
	strcpy(content_type, "text");
	strcpy(content_subtype, "plain");
	}

/* extract the content_type */
ptr = strstr(content_type_and_subtype, "(") + 1;
if(ptr)
	{
	strcpy(content_type, ptr);
	ptr = strstr(content_type, "/");
	if(ptr) *ptr = 0;
	}		
else /* not a usable content_type, set for text display */ 
	{
	strcpy(content_type, "text");
	strcpy(content_subtype, "plain");
	}

if(debug_flag)
	{
	fprintf(stdout, "munpack info file=%s\n", temp);
	fprintf(stdout,\
	"attachment_name=%s content_type=%s content_subtype=%s\n",\
	attachment_name, content_type, content_subtype);
	}

if(strcmp(content_type, "text") == 0)
	{
	}
else if(strcmp(content_type, "image") == 0)
	{
	}
else if(strcmp(content_type, "video") == 0)
	{
	}
else if(strcmp(content_type, "audio") == 0)
	{
	}
else if(strcmp(content_type, "application") == 0)
	{
	/* may be a type not know to munpack (likely) */
	/* now we go by the extention alone */
	/* extract the content subtype from the attachment name */
	go_by_extention_flag = 1;
	ptr = strstr(attachment_name, ".");
	if(! ptr)
		{
/*		fl_show_alert("Unknown content subtype", attachment_name, "", 0);*/
		ptr = (char *) fl_show_input("Unknow content subtype, subtype(gif/jpg/etc)?", "");
		if(!ptr) return(0);
		strcpy(content_subtype, ptr);
		}
	else
		{
		strcpy(content_subtype, ptr + 1);
		}
	}
else
	{
	fl_show_alert("Unknow content type", content_type, "", 0);
	return(0);
	}	

/*
select a helper program for this helper_type based on the content_subtype.
*/
lower_case_flag = 0;
while(1) /* try for normal and lower case of content_subtype */
	{
	/* get all helper programs of this type */
	b = 0;
	for(i = 0; i < 64; i++)
		{
		if(! get_helpers("helper", temp, i) ) break;
		a = sscanf(temp, "%s %s %s %s",\
		helper_type, helper_subtypes, helper_program, helper_flags_start);
		
		if(! go_by_extention_flag)
			{
			/* check if helper type matches content type */
			if(strcmp(helper_type, content_type) != 0)
				{
				continue;
				}
			}
	
		/* check the subtypes */
		if(strstr(helper_subtypes, content_subtype) != 0)
			{
			b = 1;
			/* get the arguments (flags) for the helper application */
			helper_flags[0] = 0;
			if(a == 4)
				{
				ptr = (char *) strstr(temp, helper_flags_start);
				if(ptr) strcpy(helper_flags, ptr);
				}
			break;
			}
		}/* end for all helper programs */
	if(b) break;/* match */
			
	if(! lower_case_flag)
		{
		/* content subtype to lower case (in may have been upper case) */
		i = 0;
		while(1)
			{
			c = content_subtype[i];
			content_subtype[i] = tolower(c);
			if(c == 0) break;
			i++;
			}
		lower_case_flag = 1;
		if(debug_flag)
			{
			fprintf(stdout,\
			"Changing to lower case for content_subtype=%s\n",\
			content_subtype);
			}
		/* try again, now with lower case */
		continue;
		}/* end not lower_case_flag */
	else
		{
		if(debug_flag)
			{
			fprintf(stdout, "\nCannot read any viewer from setupfile\n");
			}
		fl_show_alert(\
		"No apropriate viewer found in setupfile for type",\
		content_subtype, "", 0);
		return(0);
		}/* end if lower_case_flag */
	}/* end while normal and lower case for content_type_subtype */

if(debug_flag)
	{
	fprintf(stdout, "a=%d helper_program=%s helper_flags=%s\n",\
	a, helper_program, helper_flags);
	}

/* 
set the encoding variable, so that next time a double click
is done in the article_list_browser the attachment is launched
*/
/*set_article_encoding(group, article_id, ATTACHMENT_PRESENT, 1);*/

/* save the name of the attachment, so we can erase it later if needed */
/*set_article_attachment(group, article_id, attachment_name);*/

/* 
munpack seems to generate a file attachment.desc,
in case of nice_picture.jpg it is nice_picture.desc.
this file holds some text extracted from the article body.
now gona display this in a form.
*/
if(enable_description_popup_flag)
	{
	pop_up_desc_info(group, attachment_name);
	}

if(forked_flag) return(1);/* was already started */
	
/* start the helper program for this file */

/* add the path to the file to be processed by the helper program */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/%s",\
home_dir, news_server_name, database_name, group, attachment_name);

/* fill in the multidimensional argument array for execv() */
/* the first one is the helper program its self */
strcpy(execv_args[0], helper_program);
k = 0;
i = 1;
while(1)
	{
	if(helper_flags[k] == ' ')
		{
		k++;
		continue;
		}
	j = 0;
	while(1)
		{
		c = helper_flags[k];
		if(c == ' ') c = 0;
		execv_args[i][j] = c; 
		if(c == 0) break;
		j++;
		k++;
		}
	i++;
	if(helper_flags[k] == 0) break;
	}
execv_args[i][0] = 0;

/*
the reason I am using char *flip[] is that I cannot assign a NULL to
execv_args[i].
The reason I copy the (temporary stack variable) execv_args[][] into a *ptr[]
is that, by not using strsave(), I do not have to free later.
ececlv() will copy its arguments, flip[] may be destroyed after the call to
execlv().
*/
i = 0;
while(1)
	{
	flip[i] = execv_args[i];	
	if(execv_args[i][0] == 0)
		{
		flip[i] = temp;
		flip[i + 1] = NULL;
		break;
		}
	i++;
	}

if(debug_flag)
	{
	for(i = 0; execv_args[i][0] != 0; i++)
		{
		fprintf(stdout, "i=%d execv_args[i]=%s flip[i]=%s\n",\
		i, execv_args[i], flip[i]);
		}
	}

/* report to user */
sprintf(report, "Starting helper program %s %s",helper_program, temp);
my_command_status_display(report);

/* fork */
pid = fork();
if(pid == 0)/* in child */
	{
	/* 
	start the helper program
	NOTE: helper program  must be in the path or in the current directory.
	*/

	/* a NULL pointer is required as the last argument */
	a = execvp(helper_program, flip );
	if(a < 0)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"\nCannot start helper program execvp failed: %s %s errno=%d",\
			helper_program, temp, errno);
			}
		return(0);
		}
	}
else if(pid < 0)/* fork failed */
	{
	fl_show_alert("Helper program fork failed", "", "", 0);
	return(0);
	}
else/* parent */
	{	
	}
forked_flag = 1;

return(1);
}/* end function decode_article */


int show_info_form()
{
char temp[4096];
int lines;

char copyright[] =
{
" ** NewsFleX off line NNTP news reader **
    NewsFleX is registered Copyright (C) 1997, 1998, 1999, 2000 Jan Mourer
    email: jan@panteltje.demon.nl"
};


char expire[] =
{
""
};

char legal[] = \
{
"International legal rules and the ones for your country apply.

NewsFleX IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
The author is not responsible for any damages, caused by the use of this
program, or caused by not being able to use this program."
};

if(! registered_flag)
	{
	sprintf(temp,\
"%s

%s

%s",\
	copyright, expire, legal);
	}
else
	{
	sprintf(temp,\
"%s

                 This program is registered to:\n\
                         %s

%s",\
	copyright, real_name, legal);
	}
fl_set_input(fdui -> desc_input, temp);
lines = fl_get_input_numberoflines(fdui -> desc_input);

fl_set_input_topline(fdui -> desc_input, lines);

fl_set_input_topline(fdui -> desc_input, 1);

fl_show_form(fd_NewsFleX -> desc, FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function show_info_form */


