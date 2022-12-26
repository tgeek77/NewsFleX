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

#define FULL_CHECK

char *p_database_name;

struct posting
	{
	char *name;/* this is the posting id long decimal */
	char *subject;
	char *from;
	char *to;
	char *content_type;
	char *references;
	char *summary;
	char *attachment;
	char *date;
	int browser_line;
	int lock_flag;
	int encoding;
	long lines;
	int send_status;
	int type;
	int custom_headers_flag;
	int filter_flags;
	struct posting *nxtentr;
	};
struct posting *postingtab[1];


int reset_postings()
{
if(debug_flag)
	{
	fprintf(stdout, "reset_postings(): arg none\n");
	}

/* clear the posting list */
fl_clear_browser(fdui -> posting_list_browser);

delete_all_postings();

/* clear any visible text in the posting body editor */
fl_set_input(fdui -> posting_body_editor, "");

return(1);
}/* end function reset_postings */


struct posting *lookup_posting(char *name)
{
struct posting *pa;

if(debug_flag) fprintf(stdout, "lookup_posting(): arg =%s\n", name);

for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}
return(0);/*not found*/
}/* end function lookup_posting */


struct posting *install_posting_at_end_of_list(char *name)
{
struct posting *pa, *pb, *pprev, *lookup_posting();

if(debug_flag)
	{
	fprintf(stdout, "install_posting_at_end_of_list arg=%s\n", name);
	}

pprev = 0;/* only for clean compilation with -Wall */

/* find end of the chain of structures */
pa = postingtab[0];/* start chain */
while(1)
	{
	if(! pa)/* found end of chain */
		{
		/* enter new structure at the end of the chain */
		pb = (struct posting *) malloc( sizeof(*pb) );
		if(! pb) return(0);
		pb -> name = strsave(name);
		if(! pb -> name) return(0);
		pb -> nxtentr = 0;/* new points to zero (is end) */

		if(! postingtab[0] ) postingtab[0] = pb;/* first one */
		else pprev -> nxtentr = pb;/* previous points to new */
		pb -> summary = strsave("");	/* NO NO MORE NULL or UNDEFINED
											pointers!! */
		pb -> custom_headers_flag = NO_CUSTOM_HEADERS;
		pb -> filter_flags = FILTER_OFF;
		return(pb);/* pointer to new structure */
		}
	pprev = pa;/* remember previous structure */
	pa = pa -> nxtentr;
	}/* end while all structures */
}/* end function install_posting_at_end_of_list */


int delete_posting(char *name)/* delete entry from table */
{
struct posting *pa, *pprev;

if(debug_flag)
	{
	fprintf(stdout, "delete_posting(): arg name=%s\n", name);
	}

pprev = 0;
for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(name, pa -> name) == 0)/* found it */
		{
		if(pprev == 0)/* first entry */
			{
			postingtab[0] = pa -> nxtentr;
			}
		else/* not first one */
			{
			pprev -> nxtentr = pa -> nxtentr;
			}
		free(pa -> name);/* free name */
		free(pa -> subject);
		free(pa -> from);
		free(pa -> to);		
		free(pa -> content_type);
		free(pa -> references);
		free(pa -> summary);
		free(pa -> attachment);
		free(pa -> date);
		free(pa);/* free structure */
		return(1);
		}/* end found */
	pprev = pa;/* save this pointer */
	}/* end for all structures */
return(0);/* not found */
}/* end function delete_posting */


int delete_all_postings()/* delete all entries from table */
{
struct posting *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_postings(): arg none\n");
	show_structure();
	}	

while(1)
	{	
	pa = postingtab[0];
	if(! pa) break;
	postingtab[0] = pa -> nxtentr;/* postingtab entry points to next one,
														this could be 0 */
	free(pa -> name);/* free name */
	free(pa -> subject);
	free(pa -> from);
	free(pa -> to);
	free(pa -> content_type);
	free(pa -> references);
	free(pa -> summary);
	free(pa -> attachment);
	free(pa -> date);
	free(pa);/* free structure */
	}/* end while all structures */

if(debug_flag)fprintf(stdout, "returning from delete_all_postings\n");

return(1);
}/* end function delete_all_postings */


int show_structure()
{
struct posting *pa;

if(debug_flag)fprintf(stdout, "show_structure(): arg none\n");

pa = postingtab[0];
while(1)
	{	
	if(! pa) break;
	fprintf(stdout, "pa=%ld pa -> nxtentr=%ld\n",\
	(long) pa, (long) pa -> nxtentr);
	fprintf(stdout, "name=%s\n", pa -> name);
	fprintf(stdout, "subject=%s\n", pa -> subject);
	fprintf(stdout, "from=%s\n", pa -> from);
	fprintf(stdout, "to=%s\n", pa -> to);
	fprintf(stdout, "content_type=%s\n", pa -> content_type);
	fprintf(stdout, "references=%s\n", pa -> references);
	fprintf(stdout, "summary=%s\n", pa -> summary);
	fprintf(stdout, "browser_line=%d\n", pa -> browser_line);
	fprintf(stdout, "lock_flag=%d\n", pa -> lock_flag);
	fprintf(stdout, "encoding=%d\n", pa -> encoding);
	fprintf(stdout, "lines=%ld\n", pa -> lines);
	fprintf(stdout, "send_status=%d\n", pa -> send_status);
	fprintf(stdout, "type=%d\n", pa -> type);
	fprintf(stdout, "custom_headers_flag=%d\n", pa -> custom_headers_flag);
	fprintf(stdout, "filter_flags=%d\n", pa -> filter_flags);
	pa = pa -> nxtentr;
	}
return(1);
}/* end function show_structure */


int load_postings()
{
int a;
FILE *postings_dat_file;
char temp[READSIZE];
char name[TEMP_SIZE];
int lock_flag;
int encoding;
long lines;
int browser_line;
int send_status;
int type;
int custom_headers_flag;
int filter_flags;
extern long atol();
struct posting *pa;
char *subject;
char *from;
char *to;
char *references;
char *content_type;
char *summary;
char *attachment;
char *date;

if(debug_flag)
	{
	fprintf(stdout, "load_postings(): arg none\n");
	}

delete_all_postings();


free(p_database_name);
p_database_name = strsave("");
if(! p_database_name) return(0);

sprintf(temp, "%s/.NewsFleX/postings/%s/postings.dat",\
home_dir, postings_database_name);
postings_dat_file = fopen(temp, "r");
if(! postings_dat_file)
	{
	fl_show_alert("could not load file", temp, "", 0);
	return(0);
	} 

while(1)
	{
	/* 
	This should make it possible to read old formats, that do not have a
	filter_flags.
	*/
	filter_flags = 0;
	
	a = readline(postings_dat_file, temp);
	if(a == EOF)
		{
		free(p_database_name);
		p_database_name = strsave(postings_database_name);
		if(! p_database_name) break;

		return(1);
		}

	sscanf(temp, "%s %d %d %ld %d %d %d %d %d",\
	name, &lock_flag, &encoding, &lines, &browser_line, &send_status,\
	&type, &custom_headers_flag, &filter_flags);
	
	/* Subject: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	subject = strsave(temp);
	if(! subject) break;

	/* From: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	from = strsave(temp);
	if(! from) break;
	 
	/* Newsgroup: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	to = strsave(temp);
	if(! to) break;
	 
	/* Content-Type: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	content_type = strsave(temp);
	if(! content_type) break;

	/* References: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	references = strsave(temp);
	if(! references) break;

	/* Summary: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	summary = strsave(temp);
	if(! summary) break;

	/* Attachment: */
	a = readline(postings_dat_file, temp);
	if(a == EOF) break;/* file format error */
	attachment = strsave(temp);
	if(! attachment) break;

	/* Date: */
	a = readline(postings_dat_file, temp);
	date = strsave(temp);
	if(! date) break;

	pa = install_posting_at_end_of_list(name);
	if(! pa)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
			"load_postings: cannot install posting %s\n", temp);
			}
		break;
		}
	pa -> subject = subject;
	pa -> from = from;
	pa -> to = to;
	pa -> content_type = content_type;
	pa -> references = references;
	pa -> summary = summary;
	pa -> attachment = attachment;
	pa -> date = date;
	
	pa -> lock_flag = lock_flag;
	pa -> encoding = encoding;
	pa -> lines = lines;
	pa -> browser_line = browser_line;
	pa -> send_status = send_status;
	pa -> type = type;
	pa -> custom_headers_flag = custom_headers_flag;
	pa -> filter_flags = filter_flags;
	if(a == EOF)
		{
		return(1);
		}
	}

sprintf(temp, "%s/.NewsFleX/postings/%s/postings.dat",\
home_dir, postings_database_name);
if(a == EOF)/* file format error */
	{
	if(debug_flag)
		{
		fprintf(stdout, "load_postings(): file format error\n");
		}
	fl_show_alert("load_postings():", "File format error", temp, 0);
	}
else /* memory allocation error */
	{
	if(debug_flag)
		{
		fprintf(stdout, "load_postings(): memory allocation error\n");
		}
	}

return(0);
}/* end function load_postings */


int save_postings()
{
FILE *postings_dat_file;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
struct posting *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"save_postings(): arg none using p_database_name=%s\n",\
	p_database_name);
	}

if(! p_database_name)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"save_postings(): p_database_name is NULL cancel\n");
		}
	return(0);
	}
if(strlen(p_database_name) == 0)
	{
	my_show_alert(\
	"save_postings():", "p_database_name is empty string", "cancel");
	return(0);
	}

sprintf(temp, "%s/.NewsFleX/postings/%s/postings.tmp",
home_dir, p_database_name);
postings_dat_file = fopen(temp, "w");
if(! postings_dat_file)
	{
	my_show_alert("save_postings():\ncould not open file", temp, "cancel");
	return(0);
	} 

for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	fprintf(postings_dat_file, "%s %d %d %ld %d %d %d %d %d\n",\
	pa -> name, pa -> lock_flag, pa -> encoding, pa -> lines,\
	pa -> browser_line, pa -> send_status, pa -> type,\
	pa -> custom_headers_flag, pa -> filter_flags);
	fprintf(postings_dat_file, "%s\n", pa -> subject);
	fprintf(postings_dat_file, "%s\n", pa -> from);
	fprintf(postings_dat_file, "%s\n", pa -> to);
	fprintf(postings_dat_file, "%s\n", pa -> content_type);
	fprintf(postings_dat_file, "%s\n", pa -> references);
	fprintf(postings_dat_file, "%s\n", pa -> summary);
	fprintf(postings_dat_file, "%s\n", pa -> attachment);
	fprintf(postings_dat_file, "%s\n", pa -> date);
	}/* end for all structures in the chain starting from postingtab[0] */
fclose(postings_dat_file);

/* set some path file names */
sprintf(temp, "%s/.NewsFleX/postings/%s/postings.dat",\
home_dir, p_database_name);
sprintf(temp2, "%s/.NewsFleX/postings/%s/postings.dat~",\
home_dir, p_database_name);

/* unlink the old .dat~ */
unlink(temp2);

/* rename .dat to .dat~ */
if( rename(temp, temp2) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_postings(); rename %s into %s failed\n",\
		temp, temp2);
		}
	return(0);
	}

/* rename .tmp to .dat */
sprintf(temp2, "%s/.NewsFleX/postings/%s/postings.tmp",\
home_dir, p_database_name);
if( rename(temp2, temp) == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout, "save_postings(); rename %s into %s failed\n",\
		temp, temp2);
		}
	return(0);
	}
return(1);/* ok */
}/* end function save_postings */


int show_postings()
{
int i;
char temp[READSIZE];
struct posting *pa;
char formatstr[20];
char infostr[40];
int browser_line;
int browser_topline;
int browser_last_selected_line;
char *ptr;
char *cptr;

if(debug_flag)
	{
	fprintf(stdout, "show_postings(): arg none\n");
	}

/* remember the last line selected */
browser_last_selected_line = fl_get_browser(fdui -> posting_list_browser);

/* remember vertical slider position */
browser_topline = fl_get_browser_topline(fdui -> posting_list_browser);

/* first clear display in order not to confuse reader with old data */
fl_clear_browser(fdui -> posting_list_browser);

/* if load failed (no postings.dat file yet), continue anyway */	

browser_line = 1;

for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	/* test if diplay allowed */
	if(filters_enabled_flag)
		{
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
		}

	strcpy(formatstr, "@C18");/* dark green, error */
	if(pa -> send_status == TO_BE_SEND) strcpy(formatstr, "@C1");/* red */
	if(pa -> send_status == SEND_NOW) strcpy(formatstr, "@C5");/* magenta */
	if(pa -> send_status == SEND_LATER) strcpy(formatstr, "@C4");/* blue */
	if(pa -> send_status == SEND_OK) strcpy(formatstr, "@C0");/* black */
	if(pa -> send_status == SEND_FAILED) strcpy(formatstr, "@C18");/* dark green */
	if(pa -> lock_flag) strcpy(infostr, "*LOCK*");
	else strcpy(infostr, "            ");
	if(pa -> send_status == SEND_OK) strcpy(infostr, "*SENT*");

	if(highlight_postings(pa) )
		{
		strcat(formatstr, "@_");/* underscore */
		}

	if(!strstr(pa -> subject, "\001" ) )
		{
		sprintf(temp, "%s %s %s %ld %s %s (%s)",\
		formatstr, infostr, pa -> subject, pa -> lines, pa -> to,\
		pa -> date, pa -> name);

		fl_add_browser_line(fdui -> posting_list_browser, temp);
		pa -> browser_line = browser_line;
		browser_line++;
		}
	else /* a formatted subject, read from header file */
		{
		pa -> browser_line = browser_line;
		ptr = (char *) get_formatted_header_data(\
		"postings", atol(pa -> name), "Subject:");
		if(ptr)
			{
 			strcat(infostr, " ");
 			cptr = ptr;
			while(1)/* for all lines in the subject field */
				{
				/* ident some more for display clarity */
				sprintf(temp, "%s %s", formatstr, infostr);
				i = strlen(temp);
				while(1)/* extract a line */
					{
					if(*cptr == '\n') break;
					temp[i] = *cptr;
					if(! *cptr) break;
					i++;
					cptr++;
					}/* end while all characters in a line */
				temp[i] = 0;
				fl_add_browser_line(fdui -> posting_list_browser, temp);
				browser_line ++;

				if(! *cptr) break;
				cptr++;
				}/* end while for all lines in subject */
			free(ptr);
			}/* end if subject read OK */

		sprintf(temp, "%s %s %s %ld %s (%s) %s",\
		formatstr, infostr, "", pa -> lines, pa -> to,\
		pa -> date, pa  -> name);
		fl_add_browser_line(fdui -> posting_list_browser, temp);
		browser_line++;
		}/* end else formatted subject */

	/* if a summary field is present, show it*/	
	if(strlen(pa -> summary) != 0)
		{
		ptr = (char *) get_formatted_header_data(\
		"postings", atol(pa -> name), "Summary:");
		if(ptr)
			{
			strcat(infostr, "  ");

			sprintf(temp, "%s %sSummary:", formatstr, infostr);
			fl_add_browser_line(fdui -> posting_list_browser, temp);
			browser_line++;

 			cptr = ptr;
			while(1)/* for all lines in the summary field */
				{
				sprintf(temp, "%s %s", formatstr, infostr);
				i = strlen(temp);
				while(1)/* extract a line */
					{
					if(*cptr == '\n') break;
					temp[i] = *cptr;
					if(! *cptr) break;
					i++;
					cptr++;
					}/* end while all characters in a line */
				temp[i] = 0;
				fl_add_browser_line(fdui -> posting_list_browser, temp);
				browser_line ++;

				if(! *cptr) break;
				cptr++;
				}/* end while for all lines in summary */
			free(ptr);
			}/* end if summary read OK */
		}/* end if strlen pa -> summary != 0 */

	}/* end for all structures */

/* select the line that was last double_clicked again */
fl_select_browser_line(fdui -> posting_list_browser,\
browser_last_selected_line);

/* re adjust browser for same position of top line (vertical slider)*/	
fl_set_browser_topline(fdui -> posting_list_browser, browser_topline);

/*
maybe this should not be done, only browser line was modified?, will be
reassigned on read.
*/
/* structure was modified */
save_postings();

return(1);
}/* end function show_postings */


int show_posting_body(long posting_id)
{
int c;
struct posting *pa, *lookup_posting();
char temp[READSIZE];	
FILE *load_filefd;
struct stat *statptr;
char *space, *spaceptr;
char *ptr;
char *expanded_space;

if(debug_flag)
	{
	fprintf(stdout, "show_posting_body(): arg posting_id=%ld\n",\
	posting_id);
	}

/* parameter check */
if(posting_id < 0) return(0);

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

/* first clear display in order not to confuse reader with old data */
fl_set_input(fdui -> posting_body_editor, "");

fl_set_tabstop("aaaaaaaa");

fl_set_input(fdui -> posting_to_input_field, pa -> to);

if(!strstr(pa -> subject, "\001") )/* single line */
	{
	ptr = pa -> subject;
	}
else /* multi line subject, get from header */
	{
	ptr = get_formatted_header_data("postings", posting_id, "Subject:");
	if(! ptr) return(0);
	}
/*
xforms does not seem to listen the first time, the data is there,
if you read it back, but no display.
*/
fl_set_input(fdui -> posting_subject_input_field, ptr);
fl_set_input(fdui -> posting_subject_input_field, ptr);

fl_set_input(fdui -> posting_attachment_input_field, pa -> attachment);

/* display the summary in the summary_editor_input_field */
/* test if summary present */
if(strlen(pa -> summary) != 0)
	{
	/* read from header */
	ptr = (char *) get_formatted_header_data(\
	"postings", posting_id, "Summary:");
	if(! ptr) return(0);

	fl_set_input(fdui -> summary_editor_input_field, ptr);
	fl_set_object_color(fdui -> posting_summary_button,\
	FL_DARKORANGE,  FL_DARKORANGE);
	}
else /* no summary */
	{
	fl_set_input(fdui -> summary_editor_input_field, "");
	fl_set_object_color(fdui -> posting_summary_button, FL_COL1, FL_COL1);
	}

custom_headers_state = pa -> custom_headers_flag;

if( (pa -> custom_headers_flag & GLOBAL_HEADERS_ENABLED) ||\
(pa -> custom_headers_flag & LOCAL_HEADERS_ENABLED) )
	{
	fl_set_object_color(fdui -> posting_custom_headers_button,\
	FL_DARKORANGE, FL_DARKORANGE);
	}
else
	{	
	fl_set_object_color(fdui -> posting_custom_headers_button, FL_COL1, FL_COL1);
	}

if(pa -> custom_headers_flag & GLOBAL_HEADERS_ENABLED)
	{
	fl_set_button(fdui -> global_custom_headers_enable_button, 1);
	/* 
	Note: display of global headers by posting_custom_headers_button_cb() in
	NewsFleX_cb.c
	*/
	}
else
	{
	fl_set_button(fdui -> global_custom_headers_enable_button, 0);
	}	

if(! pa -> custom_headers_flag)
	{
	fl_set_input(fdui -> local_custom_headers_editor_input_field, "");
	}

if(pa -> custom_headers_flag & LOCAL_HEADERS_ENABLED)
	{	
	fl_set_button(fdui -> local_custom_headers_enable_button, 1);

	/* display the custom headers in the custom headers editor */
	sprintf(temp, "%s/.NewsFleX/postings/%s/custom.%ld",\
	home_dir, postings_database_name, posting_id);
	load_filefd = fopen(temp, "r");
	if(! load_filefd)
		{
		my_show_alert("could not load file", temp, "");
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

	fl_set_input(fdui -> local_custom_headers_editor_input_field, space);
	free(space);
	}
else
	{
/*	fl_set_input(fdui -> local_custom_headers_editor_input_field, "");*/
	fl_set_button(fdui -> local_custom_headers_enable_button, 0);
	}

/* posting present, load it */
sprintf(temp, "%s/.NewsFleX/postings/%s/body.%ld",\
home_dir, postings_database_name, posting_id);
load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	my_show_alert("could not load file", temp, "");
	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(!statptr) return(0);

fstat(fileno(load_filefd), statptr);

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_posting_body(): malloc could not allocate space for body\n");
		}
	return(0);
	}

/* speed up things */
fl_freeze_form(fd_NewsFleX -> posting_editor);

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(load_filefd);
		if(! ferror(load_filefd) ) break;
		perror("show_posting_body(): body read failed ");
		}/* end while error re read */	

	if(feof(load_filefd) )
		{
		fclose(load_filefd);
		break;
		}

	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */
*spaceptr = 0;/* string termination */

expanded_space = (char *) expand_tabs(space, tab_size);
free(space);

fl_set_input(fdui -> posting_body_editor, expanded_space);

/* used by extract url (in posting_body_editor_cb) */
/*
free(posting_body_copy);
posting_body_copy = strsave(expanded_space);
*/

free(expanded_space);

fl_unfreeze_form(fd_NewsFleX -> posting_editor);

/*if(! get_posting_encoding(posting_id, &posting_encoded_flag) ) return(0);*/

return(1);
}/* end function show_posting_body */


int line_to_posting_id(int line, long *posting_id)
{
struct posting *pa;
extern long atol();

if(debug_flag)fprintf(stdout, "line_to_posting_id(): arg line=%d\n", line);

for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		*posting_id = atol(pa -> name);
		return(1);
		}
	}
/* no save_postings, nothing was changed */
return(0);
}/* end function line_to_posting_id */


int fill_in_input_fields(char *group, long article_id, int type)
{
char *ptr;
char *ptr2;

if(debug_flag)
	{
	fprintf(stdout,\
	"fill_in_input_fields(): arg group=%s article_id=%ld type=%d\n",\
	group, article_id, type);
	}

/* argument check */
if(! group) return(0);
if(! type) return(0);
if( (type != NEW_ARTICLE) && (article_id < 0) ) return(0);

/* set for no attachment */
fl_set_input(fdui -> posting_attachment_input_field, "");

/* clear the editor if a new article */
if(type == NEW_ARTICLE)
	{
	/* set the "To" input field label  */
	fl_set_object_label(fdui -> posting_to_input_field, "Groups");

	/* set the "To" input field to the selected group */
	fl_set_input(fdui -> posting_to_input_field, selected_group);

	/* clear the subject */
	fl_set_input(fdui -> posting_subject_input_field, "");

	/* clear the summary */
	fl_set_input(fdui -> summary_editor_input_field, "");
	fl_set_object_color(fdui -> posting_summary_button,\
	FL_COL1, FL_COL1);

	/* clear the editor */
	fl_set_input(fdui -> posting_body_editor, "");
	}
if(type == FOLLOW_UP_ARTICLE)
	{
	/* set the "To" input field label */
	fl_set_object_label(fdui -> posting_to_input_field, "Groups");

	/* set the "To" input field the same as the original article */
	ptr = (char *) get_header_data(group, article_id, "Newsgroups:");
	if(! ptr) return(0);
	fl_set_input(fdui -> posting_to_input_field, ptr);
	free(ptr);
	/* 
	set the "Subject" input field the same as the original article,
	prepend "Re: " if not already there.
	*/
	ptr = (char *) get_formatted_header_data(group, article_id, "Subject:");
	if(! ptr) return(0);
	if(strstr(ptr, "Re:") != ptr)
		{
		ptr2 = malloc(strlen(ptr) + strlen("Re: ") + 1);
		strcpy(ptr2, "Re: ");
		strcat(ptr2, ptr);
		fl_set_input(fdui -> posting_subject_input_field, ptr2);
		free(ptr2);
		}
	else /* already there */
		{
		/* 
		for some reason xforms does not always print subject input field,
		possibly illegal characters? or maybe xform bug?
		*/
		fl_set_input(fdui -> posting_subject_input_field, ptr);
		}	
	free(ptr);
	
	/* get the summary from the original article */
	ptr = (char *) get_formatted_header_data(group, article_id, "Summary:");
	if(ptr)
		{
		fl_set_input(fdui -> summary_editor_input_field, ptr);
		free(ptr);
		fl_set_object_color(fdui -> posting_summary_button,\
		FL_DARKORANGE,  FL_DARKORANGE);
		}
	else
		{
		fl_set_input(fdui -> summary_editor_input_field, "");
		fl_set_object_color(fdui -> posting_summary_button,\
		FL_COL1, FL_COL1);
		}

	/*
	add the source reference line, and insert the original text,
	idented with '>'
	*/
	load_and_ident_article(group, article_id);
	}
if( type == REPLY_VIA_EMAIL)
	{
	/* set the to input field label */
	fl_set_object_label(fdui -> posting_to_input_field, "Email");

	/* 
	set the to input field to the email of the sender.
	First test for a Repy-To:, if not present use From:
	*/
	ptr = (char *) get_header_data(group, article_id, "Reply-To:");
	if(! ptr)/* not present */
		{
		ptr = (char *) get_header_data(group, article_id, "From:");
		if(! ptr) return(0);
		}
	fl_set_input(fdui -> posting_to_input_field, ptr);
	free(ptr);/* from the first OR from the second call to get_header_data */
	
	/* 
	set the "Subject" input field the same as the original article,
	prepend "Re: " if not already there.
	*/
	ptr = (char *) get_formatted_header_data(group, article_id, "Subject:");
	if(! ptr) return(0);
	if(strstr(ptr, "Re:") != ptr)
		{
		ptr2 = malloc(strlen(ptr) + strlen("Re: ") + 1);
		strcpy(ptr2, "Re: ");
		strcat(ptr2, ptr);
		fl_set_input(fdui -> posting_subject_input_field, ptr2);
		free(ptr2);
		}
	else /* already there */
		{
		/* 
		for some reason xforms does not always print subject input field,
		possibly illegal characters? or maybe xform bug?
		*/
		fl_set_input(fdui -> posting_subject_input_field, ptr);
		}	
	free(ptr);
	
	/* get the subject from the original article */
/*
	ptr = (char *) get_formatted_header_data(group, article_id, "Subject:");
	if(! ptr) return(0);
	fl_set_input(fdui -> posting_subject_input_field, ptr);
	free(ptr);
*/
	
	/* get the summary from the original article */
	ptr = (char *) get_formatted_header_data(group, article_id, "Summary:");
	if(ptr)
		{
		fl_set_input(fdui -> summary_editor_input_field, ptr);
		/* remember this text, in case modified and cancel pressed */
		free(ptr);
		fl_set_object_color(fdui -> posting_summary_button,\
		FL_DARKORANGE,  FL_DARKORANGE);
		}
	else
		{
		fl_set_input(fdui -> summary_editor_input_field, "");
		fl_set_object_color(fdui -> posting_summary_button,\
		FL_COL1, FL_COL1);
		}

	/* include the original text idented with '>' */
	load_and_ident_article(group, article_id);	
	}

custom_headers_state = 0;/* not know at this time yet */
fl_set_object_color(fdui -> posting_custom_headers_button, FL_COL1, FL_COL1);
fl_set_button(fdui -> global_custom_headers_enable_button, 0);
fl_set_button(fdui -> local_custom_headers_enable_button, 0);
fl_set_input(fdui -> local_custom_headers_editor_input_field, "");

posting_modified_flag = 0;
/* 
NOTE: at this point in time, the user may modify:
posting_to_input_field, posting_subject_input_field,
posting_attachment_input_field.
press summary and modify summary_editor_input_field,
press headers, then modify anything on the header form.
Then press send now -, send later - , or cancel button.
*/ 	
/* 
Note: all above data may be modified by show_posting_body().
*/
return(1);
}/* end function fill_in_input_fields */


int save_posting(char *group, long article_id, int mode)
{
char temp[TEMP_SIZE];
char name[50];
struct posting *pa, *pold, *lookup_posting();
char from[TEMP_SIZE];
char *ptr, *ptr2, *ptr3;
long lines;
int type;
FILE *exec_filefd;
int new_posting;
FILE *custom_headers_filefd;
char pathfilename[TEMP_SIZE];
int cross_posts;

if(debug_flag)
	{
	fprintf(stdout,\
	"save_posting(): arg group=%s article_id=%ld mode=%d\n\
	using posting_source=%d posting_menu_selection=%d\n",\
	group, article_id, mode, posting_source, posting_menu_selection);
	}

/* there are 3 possible sources, posting list, posting menu, online menu.
*/

/* argument check */
if(posting_source == POSTING_MENU)
	{
	if(! group) return(0);
	if( (posting_menu_selection == FOLLOW_UP_ARTICLE) ||\
	(posting_menu_selection == REPLY_VIA_EMAIL) )
		{
		if(article_id < 0) return(0);
		}
	
	if(!auto_disconnect_from_net_flag)
		{
		/* check for excessive cross posts */
		ptr = (char *) fl_get_input(fdui -> posting_to_input_field);
		if(! ptr) return(0);
		ptr2 = ptr;
	
		cross_posts = 0;
		while(1)
			{
			if(! *ptr2) break;
			if(*ptr2 == ',') cross_posts++;
			ptr2++;
			}
		if(cross_posts > WARNING_CROSS_POSTS)
			{
			sprintf(temp,\
			"This posting is cross posted to %d groups", cross_posts);
			fl_show_alert(temp, "expect flames" ,"", 0);
			}/* end to many cross posts */
		}/* end if posting source is posting menu */

	}/* end if not auto_disconnect_from_net_flag */
/*
Because time changes, and we need the correct time in a send now,
we always create a new structure entry if send_now.
The only time we do not create a new structure is if
not modified and not send now.
*/
if( (! posting_modified_flag) && (mode == SEND_LATER)) return(1);
	
new_posting = 0;
if(posting_source == POSTING_MENU)
	{
	if(posting_menu_selection == NEW_ARTICLE) new_posting = 1;
	if(posting_menu_selection == FOLLOW_UP_ARTICLE) new_posting = 1;
	if(posting_menu_selection == REPLY_VIA_EMAIL) new_posting = 1;
	type = posting_menu_selection;
	} 
if(posting_source == ONLINE_MENU)
	{
	/* use old type */
	}
if(posting_source == POSTING_LIST)
	{
	/* use old type */
	}

pold = 0;
if(! new_posting)
	{
	/* send something we already have */
	/* get the old structure address */
	sprintf(name, "%ld", body_posting);
	pold = lookup_posting(name);
	if(! pold) return(0);
	type = pold -> type;
	}
else type = posting_menu_selection;
			
/* get a new unused structure name */
if(! get_new_posting_id(&body_posting) ) return(0);
	
/* create a new structure entry */
sprintf(name, "%ld", body_posting);
pa = install_posting_at_end_of_list(name);
if(! pa ) return(0);
/* pa now points to new structure data */

/* copy data to new structure */
/* set from */
sprintf(from, "%s (%s)", user_email_address, real_name);
pa -> from = strsave(from);

if(debug_flag) fprintf(stdout, "FROM=%s\n", pa -> from);

/* get subject from the subject input field (may be modified by user ) */
ptr = (char *) fl_get_input(fdui -> posting_subject_input_field);
if(! ptr)
	{
	my_show_alert(\
	"save_posting():", "posting_subject_input_field returns NULL", "cancel");
	return(0);
	} 

/*free(pa -> subject);*/
/* if multiline, signal to use header */
if(!strstr(ptr, "\n") ) pa -> subject = strsave(ptr);
else pa -> subject = strsave("use header\001");

/* get the destination from the to input field (maybe modified by user) */
ptr = (char *) fl_get_input(fdui -> posting_to_input_field); 
if(! ptr)
	{
	my_show_alert(\
	"save_posting():", "posting_to_input_field returns NULL", "cancel");
	return(0);
	}
pa -> to = strsave(ptr);

/* get the universal date and time from the system */
ptr = get_universal_time_as_string();
if(! ptr) return(0);
pa -> date = strsave(ptr); 
free(ptr);

/* 
get the attachment from the attachment input field (set by user,
either directly or via pushing attachment button and selecting a file
with the file browser)
*/
ptr = (char *) fl_get_input(fdui -> posting_attachment_input_field);
if(! ptr)
	{
	my_show_alert(\
	"save_posting():", "posting_attachment_input_field returns NULL", "cancel");
	return(0);
	}
pa -> attachment = strsave(ptr);	

/* encode the attachment with mpack */
/* mpack -s no_subject -o outputfile frog.jpg */
if(strlen(pa -> attachment) != 0)
	{
	sprintf(temp,\
	"mpack -s none -o %s/.NewsFleX/postings/%s/attachment.%ld %s",\
	home_dir, postings_database_name, body_posting, pa -> attachment);
	if(debug_flag) fprintf(stdout, "opening file=%s\n", temp);
	exec_filefd = popen(temp, "r");
	pclose(exec_filefd);
	}
if(type == NEW_ARTICLE)
	{
	pa -> references = strsave("");/* no references */
	}
if(type == FOLLOW_UP_ARTICLE)
	{
	/* get the original article id */
	if(posting_source == POSTING_MENU)
		{
		ptr = (char *) get_header_data(group, article_id, "Message-ID:");
		if(! ptr) return(0);

		/* test if the original article has a references line */
		ptr2 = (char *) get_header_data(group, article_id, "References:");
	
		/*
		if no references line present, make one containing the original
		article id
		*/
		if(! ptr2)
			{
			pa -> references = strsave(ptr);
			}
		else
			{	
			/*
			if an original references line is present:
			add the original article id to the list of references.
			*/
			ptr3 =\
			(char *) malloc(strlen(ptr) + strlen(" ") + strlen(ptr2) + 1);
			strcpy(ptr3, ptr2);/* references */
			strcat(ptr3, " ");/* separator */
			strcat(ptr3, ptr);/* id */
			pa -> references = ptr3;
			free(ptr2);
			}
		free(ptr);
		}/* end posting_source == POSTING_MENU */
	else /* posting list, or online menu, group and article_id not set */
		{
		ptr = pold -> references;
		if(ptr) pa -> references = strsave(ptr);
		else pa -> references = strsave("");
		}	
	}/* end type = follow up article */

if(type == REPLY_VIA_EMAIL)
	{
	pa -> references = strsave("");/* no references */
	}/* end type is reply via email */

ptr = (char *) fl_get_input(fdui -> summary_editor_input_field);
if(! ptr)
	{
	my_show_alert(\
	"save_posting():", "summary_editor_input_field returns NULL", "cancel");
	return(0);
	}
/*
Note: pa is a new structure, pa -> summary is undefined, so DO NOT FREE
*/
/*free(pa -> summary);*/
if(strlen(ptr) != 0) pa -> summary = strsave("use header");
else pa -> summary = strsave("");
	
pa -> type = type;
pa -> content_type = strsave("");/* to be done */
pa -> encoding = posting_encoded_flag;/* global */
pa -> lock_flag = 0;
pa -> browser_line = 0;
pa -> send_status = mode;
pa -> custom_headers_flag = custom_headers_state; /* this last is a global */	

/* calculate the number of lines */
ptr = (char *) fl_get_input(fdui -> posting_body_editor);
if(! ptr)
	{
	my_show_alert(\
	"save_posting():", "posting_body_editor returns NULL", "cancel");
	return(0);
	}
lines = 0;
while(*ptr++)
	{
	if(*ptr == '\n') lines++;
	}
pa -> lines = lines;

if(! save_posting_header(body_posting) ) return(0);
if(! save_posting_body(body_posting) ) return(0);
if(posting_encoded_flag)
	{
	if(! save_unencoded_body(unencoded_data, body_posting) ) return(0);
	posting_encoded_flag = 0;
	}
if(custom_headers_state & LOCAL_HEADERS_MODIFIED)
	{
	ptr =\
	(char *) fl_get_input(fdui -> local_custom_headers_editor_input_field);
	if(! ptr)
		{
		my_show_alert(\
		"save_posting():", "custom_headers_editor returns NULL", "cancel");
		return(0);
		}
	/* save the custom headers_file */
	sprintf(pathfilename, "%s/.NewsFleX/postings/%s/custom.%ld",\
	home_dir, postings_database_name, body_posting);
	custom_headers_filefd = fopen(pathfilename, "w");
	if(! custom_headers_filefd)
		{
		my_show_alert("Cannot open file", pathfilename, "for write, cancel");
		return(0);
		}
	fprintf(custom_headers_filefd, "%s", ptr);
	/* if the user did not provide the terminating cr */
/*
fprintf(stdout, "END=%c(%d)\n",\
*(ptr + strlen(ptr) - 1), *(ptr + strlen(ptr) - 1) );
*/
	if(*(ptr + strlen(ptr) - 1) != '\n')
		{
		fprintf(custom_headers_filefd, "\n");
		}
	fclose(custom_headers_filefd);
	}/* end local headers modified */

if(! new_posting)
	{
	pa -> lock_flag = pold -> lock_flag;/* over rule 0 above */

	/*
	if the old structure send_status was a SEND_OK, keep it,
	else delete it.
	*/
	if(pold -> send_status != SEND_OK)
		{
 		delete_posting_entry_header_body_attachment_unencoded(\
 		atol(pold -> name) );
/*		save_postings();*/
		}
	}

/* save the structure, it was modified */
save_postings();

return(1);
}/* end function save_posting */


int set_send_status(long posting_id, int status)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"set_send_status(): arg  posting_id=%ld status=%d\n",\
	posting_id, status);
	}

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

if( (status == TO_BE_SEND) ||\
(status == SEND_LATER) ||\
(status == SEND_NOW) )
	{
	if(pa -> send_status == SEND_OK)
		{
		sprintf(temp,\
		"Posting %s (%s)\nis already send, send again?",\
		pa -> name, pa -> subject);
		if(! fl_show_question(temp, 0) )/* mouse on NO */
			{
			/* scip */
			return 1;		
			}/* end if user cancel */
		}/* end if already send */
	}/* end if send command */

pa -> send_status = status;

/* done by calling */
save_postings();

return(1);
}/* end function set_send_status */


int get_send_status()
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)fprintf(stdout, "get_send_mode(): arg using body_posting=%ld\n",\
body_posting);

sprintf(temp, "%ld", body_posting);
pa = lookup_posting(temp);
if(! pa) return(0);

return(pa -> send_status);
}/* end function set_send_status */


int save_posting_header(long posting_id)
{
char temp[TEMP_SIZE];	
FILE *header_filefd;
struct posting *pa;
char *ptr;
char *folded_text_ptr;
char *trimmed_text_ptr;

if(debug_flag)
	{
	fprintf(stdout, "save_posting_header(): arg posting_id=%ld\n",\
	posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
home_dir, postings_database_name, posting_id);
header_filefd = fopen(temp, "w");
if(! header_filefd)
	{
	my_show_alert(\
	"save_posting_header():", "could not open file for write, cancel", temp);
	return(0);
	}

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

/*
From: pante@epsilon.nl (Jan Mourer)
Newsgroups: comp.os.linux.development.apps
or
To:pante@epsilon.nl (Jan Mourer)
Subject: Posting test, I am writing an offline newsreader.
Date: 18 Feb 1997 04:28:10 -0100
Lines: 8
*/

/* Required lines */

/* From */
fprintf(header_filefd, "From: %s\n", pa -> from);

/* Newsgroups or To */
ptr = (char *) fl_get_input(fdui -> posting_to_input_field); 
if(! ptr) 
	{
	my_show_alert(\
	"save_posting_header():", "to input returns NULL", "cancel");
	return(0);
	}
pa -> to = strsave(ptr);

if( ( pa -> type == NEW_ARTICLE) || (pa -> type == FOLLOW_UP_ARTICLE) )
	{
	folded_text_ptr = fold_header_line(pa -> to);
	if(! folded_text_ptr) return 0;
	fprintf(header_filefd, "Newsgroups: %s\n", folded_text_ptr);
	free(folded_text_ptr);
	}
if(pa -> type == REPLY_VIA_EMAIL)
	{
	folded_text_ptr = fold_header_line(pa -> to);
	if(! folded_text_ptr) return 0;
	fprintf(header_filefd, "To: %s\n", folded_text_ptr);
	free(folded_text_ptr);
	}

/* Subject */
ptr = (char *) fl_get_input(fdui -> posting_subject_input_field);
if(! ptr) return(0);
folded_text_ptr = fold_text(ptr);
if(! folded_text_ptr) return 0;
fprintf(header_filefd, "Subject: %s\n", folded_text_ptr);
free (folded_text_ptr);

/* Date */
fprintf(header_filefd, "Date: %s\n", pa -> date);

/* References (in case followup) */
if(pa -> type == FOLLOW_UP_ARTICLE)
	{
	trimmed_text_ptr = trim_references_line(pa -> references);	
	if(! trimmed_text_ptr) return 0;

	folded_text_ptr = fold_header_line(trimmed_text_ptr);
	if(! folded_text_ptr) return 0;
	free(trimmed_text_ptr);

	fprintf(header_filefd, "References: %s\n", folded_text_ptr);
	free(folded_text_ptr);
	}

/* Optional lines */

/* Summary */
ptr = (char *) fl_get_input(fdui -> summary_editor_input_field);
if(! ptr)
	{
	my_show_alert(\
	"save_posting_header():", "summary input returns NULL", "cancel");
	return(0);
	}
/* if usefull pa -> summary then write to header file */
if(strlen(ptr) != 0)
	{
	folded_text_ptr = fold_text(ptr);
	if(! folded_text_ptr) return 0;
	fprintf(header_filefd, "Summary: %s\n", folded_text_ptr);
	free (folded_text_ptr);
	}

/* MIME type */
fprintf(header_filefd,\
"Mime-Version: 1.0\n");

/* ID */
fprintf(header_filefd,\
"X-Newsreader-location: %s (c) 'LIGHTSPEED' off line news reader for the Linux platform\n",\
VERSION);
fprintf(header_filefd,\
" NewsFleX homepage: http://www.panteltje.demon.nl/newsflex/ and somewhere on ftp://sunsite.unc.edu/pub/linux/\n");

fprintf(header_filefd,\
"User-Agent: %s\n", USER_AGENT);

if(debug_flag) fprintf(stdout, "\n");

fclose(header_filefd);	

/* save structure, it has changed */
save_postings();

return(1);
}/* end function save_posting_header */


int save_posting_body(long posting_id)
{
char temp[TEMP_SIZE];	
FILE *save_filefd;
char *text;

if(debug_flag)
	{
	fprintf(stdout,\
	"save_posting_body(): arg posting_id=%ld\n", posting_id);
	}

sprintf(temp, "%s/.NewsFleX/postings/%s/body.%ld",\
home_dir, postings_database_name, posting_id);
save_filefd = fopen(temp, "w");
if(! save_filefd)
	{
	my_show_alert(\
	"save_posting_body():\ncould not open file for write", temp, "cancel");
	return(0);
	}

text = (char *) fl_get_input(fdui -> posting_body_editor);
if(! text)
	{
	my_show_alert(\
	"save_posting_body():", "posting body editor returns NULL", "cancel");
	return(0);/* no text to send */
	}

fprintf(save_filefd, "%s", text);
/*
quarantee a '\n at the end, else if attachment or .\n follows,
attachment will overwrite last line body (on screen only) or no end of 
text marker.
*/
if(text[strlen(text) - 1] != '\n') fprintf(save_filefd, "\n");

fclose(save_filefd);	
return(1);
}/* end function save_posting_body */


int save_unencoded_body(char *u_data, long posting_id)
{
char temp[TEMP_SIZE];	
FILE *save_filefd;

if(debug_flag)
	{
	fprintf(stdout,\
	"save_unencoded_body(): arg u_data=%s posting_id=%ld\n",\
	u_data, posting_id);
	}

/* argument check */
if(! u_data) return(0);
if(posting_id < 0) return(0);

sprintf(temp, "%s/.NewsFleX/postings/%s/unencoded.%ld",\
home_dir, postings_database_name, posting_id);
save_filefd = fopen(temp, "w");
if(! save_filefd)
	{
	my_show_alert(\
	"save_unencoded_body():\ncould not open file for write", temp, "cancel");
	return(0);
	}

fprintf(save_filefd, "%s", u_data);
/*
quarantee a '\n at the end, else if attachment or .\n follows,
attachment will overwrite last line body (on screen only) or no end of 
text marker.
*/
if(u_data[strlen(u_data) - 1] != '\n') fprintf(save_filefd, "\n");

fclose(save_filefd);	
return(1);
}/* end function save_unencoded_body */


int load_and_ident_article(char *group, long article_id)
{
int c, i;
char temp[TEMP_SIZE];
struct stat *statptr;
char *space, *spaceptr;
FILE *load_filefd;
char *expanded_space;
char *headed_space;
int encoding, view_permission;
int hpos;
char *lastspaceptr;
int in_ident_flag;
char *ptr1;
char *ptr2;
int offset;

if(debug_flag)
	{
	fprintf(stdout,\
	"load_and_ident_article(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* parameter check */
if(! group) return(0);

/* posting present, load it */
/* test if decoded.nn present */
get_article_encoding(group, article_id, &encoding, &view_permission);
if(encoding & DECODED_PRESENT)
	{
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}
else
	{
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/body.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}
load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	my_show_alert(\
	"load_and_ident_article():\ncould not load file", temp, "cancel");
	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(load_filefd), statptr);

/* speed is important, no memory use here */
space = malloc(2 * (statptr -> st_size + 1) );/* 2 in case all LF */
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"load_and_ident_article(): malloc could not allocate space\n");
		}
	return(0);
	}

/* start */
spaceptr = space;

/* ident */
*spaceptr = '>';
spaceptr++;

in_ident_flag = 1;
temp[0] = 0;
hpos = 0;
lastspaceptr = 0;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(load_filefd);
		if(! ferror(load_filefd) ) break;
		perror("load_and_ident_article(): read failed ");
		}/* end while error re read */	

	if(feof(load_filefd) )
		{
		fclose(load_filefd);
		break;
		}

	*spaceptr = c;
	if(c == ' ') lastspaceptr = spaceptr;

	/* save ident characters on this line in temp */
	if(in_ident_flag)
		{
		if( (c == '>') || (c == ' ') )
			{
			temp[hpos] = c;
			}
		else
			{
			temp[hpos] = 0;
			in_ident_flag = 0;
			}
		}

	if(hpos >= LINE_LENGTH)
		{
		if(lastspaceptr) /* break line at space */
			{
			}
		else	/*
				cannot break line, no space found, break in word,
				alert user
				*/
			{
			spaceptr++;
			lastspaceptr = spaceptr;
/*			fl_ringbell(100);*/
			}

		*lastspaceptr = '\n';
			
		/* copy up */
		ptr1 = spaceptr;/* source */
		offset = strlen(temp) + 1;
		ptr2 = spaceptr + offset;/* dest */
		while(1)
			{
			if(ptr1 == lastspaceptr) break;
			*ptr2 = *ptr1;
			ptr1--;
			ptr2--;
			}

		/* insert ident */
		lastspaceptr++;
		*lastspaceptr = '>';			

		/* insert temp */
		lastspaceptr++;
		i = 0;
		while(1)
			{
			if(temp[i] == 0) break;
			*lastspaceptr = temp[i];
			lastspaceptr++;
			i++;
			}

		spaceptr += offset;
		lastspaceptr = 0;
		hpos = 0;
		}/* end if hpos >= LINE_LENGTH */

	spaceptr++;
	if(c == '\n')
		{
		in_ident_flag = 1;
		lastspaceptr = 0;
		hpos = 0;
		*spaceptr = '>';
		spaceptr++;
		}
	else hpos++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

/*
If we want the signature not to appear if preceded by an empty line
starting with >-- then we must stop here storing characters.
*/

if(debug_flag)
	{
	fprintf(stdout,\
	"load_and_ident_article(): global_cut_of_sig_in_reply_flag=%d\n",\
	global_cut_of_sig_in_reply_flag);
	}

if(global_cut_of_sig_in_reply_flag) cut_off_sig(space);

if(global_show_posting_source_flag)
	{
	/* add a source info line at the start of the article */
	headed_space = add_source_info_line(space, group, article_id);
	free(space);

	expanded_space = expand_tabs(headed_space, tab_size);
	free(headed_space);
	}
else
	{
	expanded_space = expand_tabs(space, tab_size);
	free(space);
	}

fl_set_input(fdui -> posting_body_editor, expanded_space);
free(expanded_space);

return(1);
}/* end function load_and_ident_article */


int get_new_posting_id(long *posting_id)
{
struct posting *pa;
long li;
char temp[50];

if(debug_flag) fprintf(stdout, "get_new_posting_id(): arg none\n");

for(li = 0; li < MAX_POSTINGS; li++)
	{
	sprintf(temp, "%ld", li);
	pa = lookup_posting(temp);/* place was free */
	if(! pa)	
		{
		*posting_id = li;
		return(1);
		}
	}
return(0);/* no free space */
}/* end function get_new_posting_id */


char *get_universal_time_as_string()
{
time_t now;
static char temp[TEMP_SIZE];
size_t strftime();
struct tm *universal_time, *gmtime();
char *ptr;
char *strsave();

if(debug_flag)
	{
	fprintf(stdout, "get_universal_time_as_string(): arg none\n");
	}

/* 
decided to adapt to universal time (UT == GMT).
New format:
from rfc1036.html:
Wdy, DD Mon YY HH:MM:SS TIMEZONE 
*/

now = time(0);
universal_time = gmtime(&now);
/*strftime (temp, 511, "%a, %d %b %y %H:%M:%S GMT", universal_time);*/
strftime (temp, 511, "%a, %d %b %Y %H:%M:%S GMT", universal_time);

ptr = strsave(temp);

if(debug_flag)
	{
	fprintf(stdout, "get_universal_time_as_string(): ptr=%s\n", ptr);
	}

return(ptr);
}/* end function get_universal_time_as_string */


char *get_header_data(char *group, long article_id, char *field)
{
int c, i;
FILE *header_file;
char temp[READSIZE];
char *ptr;
int lf_flag;
int first_char_flag;
int first_char;
char *strsave();
int file_open_flag;

if(debug_flag)
	{
	fprintf(stdout,
	"get_header_data(): arg group=%s article_id=%ld field=%s\n",\
	group, article_id, field);
	}

/* argument check */
if(! group) return(0);
if(! field) return(0);
if(article_id < 0) return(0);

first_char = 0;/* only for clean compilation with -Wall */

if(strcmp(group, "postings") == 0)
	{
	sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
	home_dir, postings_database_name, article_id);
	}
else
	{	
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}

header_file = fopen(temp, "r");
if(! header_file)
	{
	perror("get_header_data():");
	sprintf(temp, ">%s/head.%ld", group, article_id);
	my_show_alert(\
	"get_header_data():\ncannot open file for read", temp, "cancel");
	return(0);
	}

file_open_flag = 1;
first_char_flag = 0;
while(1)/* all entries (unfolded header lines) in header file */
	{
	i = 0;
	lf_flag = 0;
	while(1)/* all characters in a line */
		{
		if(first_char_flag)
			{
			temp[i] = first_char;
			i++;
			first_char_flag = 0;
			}

		while(1)/* error re read */
			{
			c = getc(header_file);
			if(! ferror(header_file) ) break;
			perror("get_header_data(): read failed ");
			}/* end while error re read */	

		if(feof(header_file) )
			{
			fclose(header_file);
			file_open_flag = 0;
			if( !lf_flag) return(0);
			break;
			}
		
		if(c == EOF)
			{
			fclose(header_file);
			file_open_flag = 0;
			if( !lf_flag) return(0);
			break;
			}
		
		if(lf_flag)
			{
			if( (c != ' ') && (c != 9) ) /* TAB = 9, no LWS */
				{
				/*
				line ends, save this character, it is the first one in the
				next line.  
				*/
				first_char = c;
				first_char_flag = 1;
				break;
				}
			/* line continous */
			lf_flag = 0;
			/* store char in line */
			}
			
		if(c == '\n')
			{
			lf_flag = 1;		
			continue;/* get next char, test for linear white space */
			}
			
		temp[i] = c;
		i++;
		if(i >= READSIZE - 1) break; /* prevent overflow */
		}/* end while all characters in a line */
	temp[i] = 0;/* string termination */

	if(strcasestr(temp, field) == temp)/* found */
		{
		if(file_open_flag)
			{
			fclose(header_file);
			file_open_flag = 0;
			}
		ptr = strsave(strstr(temp, ":") + 2);
		return(ptr);/* must be freed by calling */
		}

	if(c == EOF) return(0);
	if(feof(header_file) ) return(0);

	}/* end while all entries (unfolded header lines) in header file */
}/* end function get_header_data */


char *get_formatted_header_data(\
char *group, long article_id, char *field)
{
int c, i;
FILE *header_file;
char temp[READSIZE];
char *ptr;
int lf_flag;
int first_char_flag;
int first_char;
char *strsave();
int file_open_flag;

if(debug_flag)
	{
	fprintf(stdout,
	"get_formatted_header_data(): arg group=%s article_id=%ld field=%s\n",\
	group, article_id, field);
	}

/* argument check */
if(! group) return(0);
if(! field) return(0);
if(article_id < 0) return(0);

first_char  = 0;/* only for clean compilation with -Wall */

if(strcmp(group, "postings") == 0)
	{
	sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
	home_dir, postings_database_name, article_id);
	}
else
	{	
	sprintf(temp, "%s/.NewsFleX/%s/%s/%s/head.%ld",\
	home_dir, news_server_name, database_name, group, article_id);
	}
header_file = fopen(temp, "r");
if(! header_file)
	{
	perror("get_formatted_header_data():");
	sprintf(temp, ">%s/head.%ld", group, article_id);
	my_show_alert(\
	"get_formatted_header_data():\ncannot open file for read", temp, "cancel");
	return(0);
	}

file_open_flag = 1;
first_char_flag = 0;
while(1)/* all entries (unfolded header lines) in header file */
	{
	i = 0;
	lf_flag = 0;
	while(1)/* all characters in a line */
		{
		if(first_char_flag)
			{
			temp[i] = first_char;
			i++;
			first_char_flag = 0;
			}

		while(1)/* error re read */
			{
			c = getc(header_file);
			if(! ferror(header_file) ) break;
			perror("get_formatted_header_data(): read failed ");
			}/* end while error re read */	

		if( (c == EOF) || (feof(header_file) ) )
			{
			fclose(header_file);
			file_open_flag = 0;
			if( !lf_flag) return(0);
			c = EOF;
			break;
			}
		
		if(lf_flag)
			{
			if( (c != ' ') && (c != 9) ) /* TAB = 9, no LWS */
				{
				/*
				line ends, save this character, it is the first one in the
				next line.  
				*/
				first_char = c;
				first_char_flag = 1;
				break;
				}
			/* line continous */
			temp[i] = '\n';
			i++;
			lf_flag = 0;
			/* store char in line */
			}
			
		if(c == '\n')
			{
			lf_flag = 1;		
			continue;/* get next char, test for linear white space */
			}
			
		temp[i] = c;
		i++;
		if(i >= READSIZE - 1) break; /* prevent overflow */
		}/* end while all characters in a line */
	temp[i] = 0;/* string termination */

	if(strcasestr(temp, field) == temp)/* found */
		{
		if(file_open_flag)
			{
			fclose(header_file);
			file_open_flag = 0;
			}
		ptr = strsave(strstr(temp, ":") + 2);
		return(ptr);/* must be freed by calling */
		}
	if(c == EOF) return(0);
	if(feof(header_file) ) return(0);

	}/* end while all entries (unfolded header lines) in header file */
}/* end function get_formatted_header_data */


int delete_posting_entry_header_body_attachment_unencoded(long posting_id)
{
int a;
struct posting *pa;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout,\
	"delete_posting_entry_header_body_attachment_unencoded(): arg posting_id=%ld\n",\
	posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

/* delete header */
sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
home_dir, postings_database_name, posting_id);
unlink(temp);

/* delete attachment */
if(! test_if_posting_has_attachment(posting_id, &a) ) return(0);
if(a)
	{
	sprintf(temp, "%s/.NewsFleX/postings/%s/attachment.%ld",\
	home_dir, postings_database_name, posting_id);
	unlink(temp);
	}

/* delete body */
sprintf(temp, "%s/.NewsFleX/postings/%s/body.%ld",\
home_dir, postings_database_name, posting_id);
unlink(temp);

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);
if(pa -> custom_headers_flag)
	{
	/* delete custom header */
	sprintf(temp, "%s/.NewsFleX/postings/%s/custom.%ld",\
	home_dir, postings_database_name, posting_id);
	unlink(temp);
	}

/* delete unencoded.nn */
if(pa -> encoding)/* encoded or clear signed */
	{
	sprintf(temp, "%s/.NewsFleX/postings/%s/unencoded.%ld",\
	home_dir, postings_database_name, posting_id);
	unlink(temp);
	}

/* delete entry from structure */
sprintf(temp, "%ld", posting_id);
delete_posting(temp);

/* structure has changed, save it */
/* done by calling */
save_postings();

return(1);
}/* end function delete_posting_entry_header_body_attachment_unencoded */


int post_email_via_sendmail(long posting_id)
{
int a;
FILE *header_filefd;
FILE *custom_headers_filefd;
FILE *body_filefd;
FILE *attachment_filefd;
FILE *sendmail_filefd;
char temp[READSIZE];
char temp2[READSIZE];
char path_filename[TEMP_SIZE];
int custom_headers_flag;

struct posting *lookup_posting();

if(debug_flag)
	{
	fprintf(stdout,\
	"post_email_via_sendmail(): arg posting_id=%ld\n", posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

/*
get original "From", it will become Reply-To
original_from = (char *) get_header_data("postings", posting_id, "From:");
if(! original_from) return(0);
*/

/* open channel to sendmail */
/*
Options used: (see man sendmail)
-t			scan To: Cc: for recipients, delete Bcc:.
-oem		mail errors.
-odb		run in background.
-oi			do not take a single dot on a line as message termination.
-oc			do not immediatly initiate connection ('expensive
			mailers')
Not used (using reply to):
-fname		use specified name.
-Ffullname	set the full name of the sender.
*/

sprintf(temp, "sendmail -oc -t -oem -odb -oi");
sendmail_filefd = popen(temp, "w");
if(! sendmail_filefd)
	{
	if(auto_disconnect_from_net_flag)
		{
		/* write to error log */
		to_error_log("Cannot start sendmail");
		}
	else
		{
		my_show_alert(\
		"post_email_via_sendmail():",\
		"Cannot start sendmail",\
		"cancel");
		}
	return(0);
	}

/* open header file */
sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
home_dir, postings_database_name, posting_id);
header_filefd = fopen(temp, "r");
if(! header_filefd)
	{
	pclose(sendmail_filefd);
	sprintf(temp2, "Cannot open file %s for read", temp);
	my_show_alert("Cannot send email", "cannot open file for read", temp);
	return(0);
	}
while(1)
	{
	a = readline(header_filefd, temp);
	if(a == EOF) break;
	/*
	sendmail will use user@host as From, replies must go to the real email
 	address
 	*/

	/* If a From line, replace the original From with Reply-To */
	if(strstr(temp, "From:") == temp)
		{
		sprintf(temp2, "Reply-To: %s", strstr(temp, ":") + 2); 
		strcpy(temp, temp2);
		}	
	
	if(debug_flag) fprintf(stdout, "header to sendmail=%s\n", temp);	
	fprintf(sendmail_filefd, "%s\n", temp);
	if(ferror(sendmail_filefd) )
		{
		my_show_alert("Cannot send email", "cannot write to sendmail", "");
		fclose(header_filefd);
		pclose(sendmail_filefd);
		return(0);
		}	
	}/* end while read header file */

/* get custom_headers_flag */
if(! get_custom_headers_flag(posting_id, &custom_headers_flag) )
	{
	sprintf(temp, "Cannot get custom headers flag for posting_id %ld",\
	posting_id);
	my_show_alert("Cannot send email", temp, "");
	pclose(sendmail_filefd);
	return(0);
	}

/* test for global custom headers enabled */
if(custom_headers_flag & GLOBAL_HEADERS_ENABLED)
	{
	sprintf(path_filename, "%s/.NewsFleX/global_custom_head",\
	home_dir);
	custom_headers_filefd = fopen(path_filename, "r");			
	if(! custom_headers_filefd)
		{
		sprintf(temp, "Cannot open file %s for read", path_filename);
		my_show_alert("Cannot send email", temp, "");
		pclose(sendmail_filefd);
		return(0);
		}
	/* inform user what is happening */
	sprintf(temp, "Posting %s", path_filename);
	my_command_status_display(temp);
	while(1)
		{
		a = readline(custom_headers_filefd, temp);
		if(a == EOF) break;
		/* prevent end of header detection if empty line */
		if(strlen(temp) == 0) continue;
		if(debug_flag)
			{
			fprintf(stdout, "post_email_via_sendmail():temp=%s\n", temp);
			}
		fprintf(sendmail_filefd, "%s\n", temp);
		if(ferror(sendmail_filefd) )
			{
			my_show_alert(\
			"Cannot send email", "cannot write to sendmail", "");
			pclose(sendmail_filefd);
			fclose(custom_headers_filefd);
			return(0);
			}	
		}/* end while all characters in custom headers file */
	}/* end if a global custom headers enabled */

/* test for local custom headers enabled */
if(custom_headers_flag & LOCAL_HEADERS_ENABLED)
	{
	sprintf(path_filename, "%s/.NewsFleX/postings/%s/custom.%ld",\
	home_dir, postings_database_name, posting_id);
	custom_headers_filefd = fopen(path_filename, "r");			
	if(! custom_headers_filefd)
		{
		sprintf(temp, "Cannot open file %s for read", path_filename);
		my_show_alert("Cannot send email", temp, "");
		pclose(sendmail_filefd);
		return(0);
		}
	/* inform user what is happening */
	sprintf(temp, "Posting %s", path_filename);
	my_command_status_display(temp);
	while(1)
		{
		a = readline(custom_headers_filefd, temp);
		if(a == EOF) break;
		/* prevent end of header detection if empty line */
		if(strlen(temp) == 0) continue;
		if(debug_flag)
			{
			fprintf(stdout, "post_email_via-sendmail():temp=%s\n", temp);
			}
		fprintf(sendmail_filefd, "%s\n", temp);
		if(ferror(sendmail_filefd) )
			{
			my_show_alert(\
			"Cannot send email", "cannot write to sendmail", "");
			pclose(sendmail_filefd);
			fclose(custom_headers_filefd);
			return(0);
			}	
		}/* end while all characters in custom headers file */
	}/* end if local custom headers enabled */

/* a blank line separates header and body */
fprintf(sendmail_filefd, "\n");
if(ferror(sendmail_filefd) )
	{
	my_show_alert("Cannot send email", "cannot write to sendmail", "");
	pclose(sendmail_filefd);
	return(0);
	}	

/* open body file */
sprintf(temp, "%s/.NewsFleX/postings/%s/body.%ld",\
home_dir, postings_database_name, posting_id);
body_filefd = fopen(temp, "r");
if(! body_filefd)
	{
	pclose(sendmail_filefd);
	sprintf(temp2, "Cannot open file %s for read", temp);
	my_show_alert("Cannot send email", temp2, "");
	return(0);
	}
while(1)
	{
	a = readline(body_filefd, temp);
	if(a == EOF) break;

	/*
	if the first character of a line is a period, quote it
	with another period
	*/
	if(temp[0] == '.') sprintf(temp2, ".%s", temp);
	else sprintf(temp2, "%s", temp);
	strcpy(temp, temp2);

	if(debug_flag)
		{
		fprintf(stdout, "post_email_via_sendmail():temp=%s\n", temp);
		}
	fprintf(sendmail_filefd, "%s\n", temp);
	if(ferror(sendmail_filefd) )
		{
		my_show_alert("Cannot send email", "cannot write to sendmail", "");
		pclose(sendmail_filefd);
		fclose(body_filefd);
		return(0);
		}	
	}/* end while body file */

/* test if there is an attachment */
if(! test_if_posting_has_attachment(posting_id, &a) ) return(0);
if(a)
	{
	/* Try to open the attachment file attachment.%ld */
	sprintf(temp, "%s/.NewsFleX/postings/%s/attachment.%ld",\
	home_dir, postings_database_name, posting_id);
	attachment_filefd = fopen(temp, "r");
	if(! attachment_filefd)
		{
		pclose(sendmail_filefd);
		sprintf(temp2, "Cannot open file %s for read", temp);
		my_show_alert("Cannot send email", temp2, "");
		return(0);
		}
	while(1)
		{
		a = readline(attachment_filefd, temp);
		if(a == EOF) break;
		if(debug_flag)
			{
			fprintf(stdout, "post_email_via_sendmail():temp=%s\n", temp);
			}
		/* 
		if the first character of a line is a period, quote it
		with another period
		*/
		if(temp[0] == '.') sprintf(temp2, ".%s", temp);
		else sprintf(temp2, "%s", temp);
		strcpy(temp, temp2);

		fprintf(sendmail_filefd, "%s\n", temp);
		if(ferror(sendmail_filefd) )
			{
			my_show_alert("Cannot send email", "cannot write to sendmail", "");
			pclose(sendmail_filefd);
			fclose(attachment_filefd);
			return(0);
			}	
		}/* end while attachment file */
	}/* end strlen pa -> attachment not 0 */

/* Note: last line attachment not guaranteed to have a \n, up to mpack */

/* a single period on a line indicates end of text */
fprintf(sendmail_filefd, ".\n");
if(ferror(sendmail_filefd) )
	{
	my_show_alert("Cannot send email", "cannot write to sendmail", "");
	pclose(sendmail_filefd);
	return(0);
	}	
	
pclose(sendmail_filefd);
set_send_status(posting_id, SEND_OK);
my_command_status_display("");
return(1);
}/* end function post_email_via_sendmail */


int get_posting_type(long posting_id)
{
struct posting *pa, *lookup_posting();
char temp[50];

if(debug_flag)
	{
	fprintf(stdout, "get_posting_type(): arg posting_id=%ld\n", posting_id);
	}

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

return(pa -> type);
}/* end function get_posting_type */


int test_if_posting_has_attachment(long posting_article, int *present_flag)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"test_if_posting_has_attachment(): arg posting_article=%ld\n",\
	posting_article);
	}

/* argument check */
if(posting_article < 0) return(0);

sprintf(temp, "%ld", posting_article);
pa = lookup_posting(temp);
if(! pa)
	{
	if(debug_flag) fprintf(stdout, "could not lookup_posting %s\n", temp);
	return(0);
	}
if(! pa -> attachment)
	{
	if(debug_flag) fprintf(stdout, "pa -> attachent == NULL\n");
	return(0);
	}
if(strlen(pa -> attachment) == 0)
	{
	*present_flag = 0;/* no attachment */
	return(1);
	}
*present_flag = 1;/* some attachment */	
return(1);
}/* end function test_if_posting_has_attachment */
 
 
int set_lock_posting(long posting_id, int status)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout, "set_lock_posting(); arg posting_id=%ld status=%d\n",\
	posting_id, status);
	}
	
/* parameter check */
if(posting_id < 0) return(0);
if( (status != 0) && (status != 1) ) return(0);	

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);
pa -> lock_flag = status;

/* done by calling */
/*save_postings();*/

return(1);
}/* end function set_lock_posting */


int get_posting_status(long posting_id, int *send, int *lock)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout, "get_posting_status(); arg posting_id=%ld\n",\
	posting_id);
	}
	
/* parameter check */
if(posting_id < 0) return(0);

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

*send = pa -> send_status;
*lock = pa -> lock_flag;

if(debug_flag) fprintf(stdout, "send=%d lock=%d\n", *send, *lock);

return(1);
}/* end function get_posting_status */


int add_marked_posting_bodies_to_command_queue()
{
struct posting *pa;
int found_flag;
int type;

if(debug_flag)
	{
	fprintf(stdout, "add_marked_postings_to_commands_queue(): arg none\n");
	}

fl_set_object_label(fdui -> command_status_display,\
"Searching for marked postings");
XSync(fl_get_display(), 0);

while(1)/* while there are postings to send (see below) */
	{
	found_flag = 0;
	for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
		{
		if (pa -> send_status == TO_BE_SEND)
			{
			if(debug_flag ) fprintf(stdout, "pa -> status is TO_BE_SEND\n");
			
			/* fill in the input fields (not really showing them) */
			body_posting = atol(pa -> name);
			
			if(! show_posting_body(body_posting) )	/* filter:
														permission denied
													*/
				{
				continue;/* _______possible bug____*/
				}
			
			/* Note: save_posting() uses body_posting */

			posting_source = POSTING_LIST;
			if(! save_posting("dummy", 99999999, SEND_NOW) ) return(0);
			/* Note: save_posting() has done a save_postings(); */	
			
			/* Note: structure posting now modified, cannot use pa */
			type = get_posting_type(body_posting);
			if(debug_flag) fprintf(stdout, "type detected=%d\n", type);
			
			if( (type == NEW_ARTICLE) || (type == FOLLOW_UP_ARTICLE))
				{
				if(debug_flag) fprintf(stdout, "ARTICLE\n");
				add_to_command_queue("dummy", body_posting, POST_ARTICLE);
				}
			if(type == REPLY_VIA_EMAIL)
				{
				if(debug_flag) fprintf(stdout, "EMAIL\n");
				add_to_command_queue("dummy", body_posting, EMAIL_ARTICLE);
				}
			/*
			this is tricky, the structure posting is now modified by 
			save_posting(), must leave the for, but there may be more
			to be send.
			*/
			found_flag = 1;
			}/* end if status to be send */
		if(found_flag) break;
		}/* end for all structures */
	if(debug_flag) fprintf(stdout, "re entering\n");
	if(! found_flag) break;/* no more to post */
	}/* end while something to post */	

fl_set_object_label(fdui -> command_status_display, "");
XSync(fl_get_display(), 0);

return(1);
}/* end function add_marked_posting_bodies_to_command_queue */


int get_custom_headers_flag(long posting_article, int *present_flag)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"get_custom_headers_flag(): arg posting_article=%ld\n",\
	posting_article);
	}

/* argument check */
if(posting_article < 0) return(0);

sprintf(temp, "%ld", posting_article);
pa = lookup_posting(temp);
if(! pa)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"get_custom_headers_flag(): could not lookup_posting %s\n", temp);
		}
	return(0);
	}
*present_flag = pa -> custom_headers_flag;/* some custom_headers */	
return(1);
}/* end function get_custom_headers_flag */
 
 
/*
int set_custom_headers_flag(long posting_article, int present_flag)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"set_custom_headers_flag(): arg posting_article=%ld present_flag=%d\n",\
	posting_article, present_flag);
	}

*/
/* argument check */
/*
if(posting_article < 0) return(0);

sprintf(temp, "%ld", posting_article);
pa = lookup_posting(temp);
if(! pa)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"set_custom_headers_flag(): could not lookup_posting %s\n", temp);
		}
	return(0);
	}
pa -> custom_headers_flag = present_flag;	

*/
/* save structure, it was modified */
/*
save_postings();

return(1);
}*/ /* end function get_custom_headers_flag */
 
 
int set_all_filter_flags_in_postings()
{
struct posting *pa;
/*char temp[TEMP_SIZE];*/

if(debug_flag)
	{
	fprintf(stdout, "set_all_filter_flags_in_postings(): arg none\n");
	}
	
for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* tell the user what is happening */
/*
	sprintf(temp, "Setting filter flags for posting %s", pa -> name);
	fl_set_object_label(fdui -> command_status_display, temp);
	XSync(fl_get_display(), 0);
*/
	/* get the filter result */
	pa -> filter_flags = 0;/* the default */

	get_filter_flags(\
	"dummy", pa -> name, pa -> encoding, pa -> attachment,\
	1, SEARCH_POSTINGS, &(pa -> filter_flags) );
											/* Note: have_body_flag = 1 */
	}/* end for all structures */

/* clear the command status display */
/*
fl_set_object_label(fdui -> command_status_display, "");
XSync(fl_get_display(), 0);
*/

/* save the structure, it was modified */
save_postings();

return(1);
}/* end function set_all_filter_flags_in_postings */


int posting_show_permission(long posting_id)
{
struct posting *pa, *lookup_posting();
char temp[50];

if(debug_flag)
	{
	fprintf(stdout, "posting_show_permission(): arg posting_id=%ld\n",\
	posting_id);
	}

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa) return(0);

/* test if allowed */
if(filters_enabled_flag)
	{
	if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY)
		{
		return(0);
		}
	}/* end if filters_enabled_flag */
return(1);
}/* end function posting_show_permission */


int set_posting_encoding(long posting_id, int state)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"set_posting_encoding(): arg posting_id=%ld\n",\
	posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"get_posting_encoding_flag(): could not lookup_posting %s\n", temp);
		}
	return(0);
	}
pa -> encoding = state;

/* save the structure, it was modified */
save_postings();

return(1);
}/* end function set_posting_encoding */


int get_posting_encoding(long posting_id, int *state)
{
struct posting *pa, *lookup_posting();
char temp[80];

if(debug_flag)
	{
	fprintf(stdout,\
	"get_posting_encoding(): arg posting_id=%ld\n",\
	posting_id);
	}

/* argument check */
if(posting_id < 0) return(0);

sprintf(temp, "%ld", posting_id);
pa = lookup_posting(temp);
if(! pa)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"get_posting_encoding(): could not lookup_posting %s\n", temp);
		}
	return(0);
	}
*state = pa -> encoding;
return(1);
}/* end function get_posting_encoding */


int highlight_postings(struct posting *pa)
{
int c, i;
int found;
int first_char_flag, folded_flag, lf_flag;
int first_char;
char *database;
char temp[READSIZE];
FILE *header_fd;

if(debug_flag)
	{
	fprintf(stdout,\
	"highlight_postings(): pa=%lu\n", (long)pa);
	}

/* argument check */
if(! pa) return(0);

first_char = 0;/* just to keep gcc -Wall happy */

if(! highlight_postings_flag) return(0);

/* headers */
if(highlight_headers_flag)
	{
	database = 0; /*strsave(""); keep gcc -Wall happy */

	/* subject */
	if(highlight_headers_subject_flag)
		{
/*
		if(strstr(pa -> subject, "\001") )
			{
			database = (char *) get_formatted_header_data(\
			"postings", atol(pa -> name), "Subject:");
			if(! database) return(0);
			}
		else
*/
			{
			database = strsave(pa -> subject);
			}
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_headers_flag */

	/* summary */
	/* FLAG DOES NOT EXIST, use others */
	/*
	if(highlight_headers_summary_flag)
		{
		if(pa -> summary_flag)
			{
			database = (char *) get_formatted_header_data(\
			group, atol(pa -> name), "Summary");
			if(! database) return(0);
			found = search_in_for_or(\
			database, highlight_keywords, highlight_case_sensitive_flag);
			free(database);
			if(found) return(1);
			}
		}
	*/
	
	/* from */
	if(highlight_headers_from_flag)
		{
		database = strsave(pa -> from);
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_from_flag */
	
	/* references */
	if(highlight_headers_references_flag)
		{
		database = strsave(pa -> references);
		found = search_in_for_or(\
		database, highlight_keywords, highlight_case_sensitive_flag);
		free(database);
		if(found) return(1);
		}/* end if highlight_references_flag */

	/* To: (email) or Newsgroups: */
	if(highlight_headers_to_flag)
		{
		/* Newsgroups */
		database = (char *) get_formatted_header_data(\
		"postings", atol(pa -> name), "Newsgroups:");
		if(database) /* "Newsgroups:" */
			{
			found = search_in_for_or(\
			database, highlight_keywords, highlight_case_sensitive_flag);
			free(database);
			if(found) return(1);
			}
		else /* "To:" */
			{
			/* To (email) */
			database = (char *) get_formatted_header_data(\
			"postings", atol(pa -> name), "To:");
			if(! database) return(0);/* defective header */

			found = search_in_for_or(\
			database, highlight_keywords, highlight_case_sensitive_flag);
			free(database);
			if(found) return(1);
			}
		}/* end if highlight_to_flag */

	/* others */
	if(highlight_headers_others_flag)
		{
		sprintf(temp, "%s/.NewsFleX/postings/%s/head.%ld",\
		home_dir, postings_database_name, atol(pa -> name) );
		header_fd = fopen(temp, "r");
		if(!header_fd)
			{
			fl_show_alert(\
			"highlight_postings():\nCannot open", temp, "for read", 0);
			return(0);
			}		
		else
			{
			/* scan the headerfile */
			temp[0] = 0;/* mark empty */
			first_char_flag = 0;
			while(1) /* all entries (unfolded header lines)
						in header file */
				{
				folded_flag = 0; /* the function of this variable is to
									find out if the line was folded */
				i = 0;
				lf_flag = 0;
				while(1)/* all characters in a line */
					{
					if(first_char_flag)
						{
						temp[i] = first_char;
						i++;
						first_char_flag = 0;
						}

					while(1)/* error re read */
						{
						c = getc(header_fd);
						if(! ferror(header_fd) ) break;
						perror("highlight(): read failed ");
						}/* end while error re read */	

					if(feof(header_fd) || (c == EOF) )
						{
						fclose(header_fd);
						c = EOF;
						break;
						}
		
					if(lf_flag)
						{
						if( (c != ' ') && (c != 9) )/* TAB = 9, no LWS */
							{
							/*
							line ends, save this character,
							it is the first one in the next line.
							*/
							first_char = c;
							first_char_flag = 1;
							break;
							}
						/* line continous */
						lf_flag = 0;
						folded_flag = 1;
						/* store char in line */
						}
			
					if(c == '\n')
						{
						lf_flag = 1;		
						continue;/* get next char,
									test for linear white space */
						}
			
					temp[i] = c;
					i++;
					if(i >= READSIZE - 1) break; /* prevent overflow */
					}/* end while all characters in a line */
				temp[i] = 0;/* string termination */
					
				if(strstr(temp, "From:") == 0)
					{
					if(strstr(temp, "To:") ==  0)
						{
						if(strstr(temp, "Newsgroups:") == 0)
							{
							if(strstr(temp, "Subject:") ==  0)
								{		
								if(strstr(temp, "References:") ==  0)
									{
									found = search_in_for_or(\
									temp, highlight_keywords,\
									highlight_case_sensitive_flag);
									if(found)
										{
										if(c != EOF) fclose(header_fd);
										return(1);
										}
									}/* end no references */
								}/*	end no subject */
							}/* end no newsgroups */
						}/* end no to */
					}/* end no from */

				if(c == EOF) break;
					
				}/* end while all lines in file */
			}/* end header_fd != 0 */
		}/* end highlight_headers_others_flag */
	}/* end if highlight_headers_flag */

return(0);
}/* end function highlight_postings */

 
int do_alt_editor()
{
int c;
FILE *exec_fd;
char temp[TEMP_SIZE];
char file[TEMP_SIZE];
FILE *edit_fileptr;
struct stat *statptr;
char *space, *spaceptr;
char *expanded_space;
char *ptr, *cptr;
char tempfile[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "do_alt_editor(): arg none\n");
	}

/* argument check */
if(! alt_editor_command) return(0);

/* prevent user from pressing any buttons */
fl_deactivate_all_forms();

/* get text */
ptr = (char *)fl_get_input(fdui -> posting_body_editor);
if(! ptr)
	{
	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}

/* save text to temp file */
sprintf(tempfile, "%s/.NewsFleX/postings/%s/temp",\
home_dir, postings_database_name);
edit_fileptr = fopen(tempfile, "w");
if(! edit_fileptr)
	{
	fl_show_alert("Cannot open file", tempfile, "for write ", 0);

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}

cptr = ptr;
while(1)
	{
	c = *cptr;
	if(! c) break;/* EOF */

	while(1)
		{
		fputc(c, edit_fileptr);
		if(ferror(edit_fileptr) )
			{
			perror("do_alt_editor(): write failed ");
			continue;
			}
		break;
		}
	cptr++;
	}/* end while all characters in editor */
fclose(edit_fileptr);

/* edit temp file */
/*xterm -e joe $posting*/
/*color_xterm -geometry 77x28+1026+1712 -sb -sl 500 -j -ls -fn 10x20 &*/

/* replace '$posting' with path filename of tempfile */
sprintf(file, "%s/.NewsFleX/postings/%s/temp",\
home_dir, postings_database_name);
replace(alt_editor_command, "$posting", file, temp);

exec_fd = popen(temp, "r");
if(! exec_fd)
	{
	fl_show_alert("Cannot execute file:", temp, "", 0);

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}
pclose(exec_fd);

/* load edited text */
sprintf(temp, "%s/.NewsFleX/postings/%s/temp",\
home_dir, postings_database_name);
edit_fileptr = fopen(temp, "r");
if(! edit_fileptr)
	{
	fl_show_alert("could not load file", temp, "", 0);

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(!statptr)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"do_alt_editor(): could not allocate memory for statptr\n");
		}

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}

if(fstat(fileno(edit_fileptr), statptr) == -1)
	{
	fclose(edit_fileptr);

	if(debug_flag)
		{
		fprintf(stdout,\
		"do_alt_editor(): could not do stat on file %s\n",\
		temp);
		}	

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return 0;
	}

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"do_alt_editor(): could not allocate memory for space\n");
		}

	/* allow user to press buttons again */
	fl_activate_all_forms();

	return(0);
	}

/* speed up things */
fl_freeze_form(fd_NewsFleX -> posting_editor);

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(edit_fileptr);
		if(! ferror(edit_fileptr) ) break;
		perror("show_posting_body(): body read failed ");
		}/* end while error re read */	

	if(feof(edit_fileptr) )
		{
		fclose(edit_fileptr);
		break;
		}

	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */
*spaceptr = 0;/* string termination */

/* expand any tabs */
expanded_space = (char *) expand_tabs(space, tab_size);
free(space);
if(! expanded_space)
	{
	if(debug_flag)
		{
		fprintf(stdout, "do_alt_editor(): could not expand tabs\n");
		}		

	/* allow user to press buttons again */
	fl_activate_all_forms();

	fl_unfreeze_form(fd_NewsFleX -> posting_editor);

	return 0;
	}

/* edited text to display */
fl_set_input(fdui -> posting_body_editor, expanded_space);
free(expanded_space);

fl_unfreeze_form(fd_NewsFleX -> posting_editor);

/* allow user to press buttons again */
fl_activate_all_forms();

/* delete the temp file */
unlink(tempfile);

return(1);
}/* end function do_alt_editor */


int regenerate_posting(\
long posting, int body_flag, int local_custom_headers_flag,\
int attachment_flag)
{
struct posting *pa;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout,\
	"regenerate_posting():\n\
arg posting=%ld body_flag=%d custom_headers_flag=%d attachment_flag=%d\n",\
	posting, body_flag, local_custom_headers_flag, attachment_flag);
	}

/* argument check */
if(posting < 0) return(0);

sprintf(temp, "%ld", posting);
pa = install_posting_at_end_of_list(temp);
if(! pa)
	{
	if(debug_flag) fprintf(stdout, "install failed\n");	
	return(0);
	}

pa -> subject = (char *) get_header_data("postings", posting, "Subject:");
if(! pa -> subject)
	{
	if(debug_flag) fprintf(stdout, "subject failed\n");
	return(0);
	}

pa -> from = (char *) get_header_data("postings", posting, "From:");
if(! pa -> from)
	{
	if(debug_flag) fprintf(stdout, "from failed\n");
	return(0);
	}

pa -> to = (char *) get_header_data("postings", posting, "To:");
if(! pa -> to)
	{
	pa -> to = (char *) get_header_data("postings", posting, "Newsgroups:");
	if(! pa -> to)
		{
		if(debug_flag) fprintf(stdout, "newsgroup failed\n");
		return(0);
		}
	else
		{
		/* determine type */
		if(strstr(pa -> subject, "Re:") == pa -> subject)
			{
			pa -> type = FOLLOW_UP_ARTICLE;
			}
		else
			{
			pa -> type = NEW_ARTICLE;
			}
		}
	}/* end not email */
else
	{
	pa -> type = REPLY_VIA_EMAIL;
	}/* end if email */

pa -> date = (char *) get_header_data("postings", posting, "Date:");
if(! pa -> date)
	{
	if(debug_flag) fprintf(stdout, "date failed\n");
	return(0);
	}

pa -> content_type = (char *) get_header_data(\
"postings", posting, "Content-Type:");
if(! pa -> content_type)
	{
	pa -> content_type = strsave("");
	if(! pa -> content_type)
		{
		if(debug_flag) fprintf(stdout, "malloc content-type failed\n");
		return(0);
		}
	}

pa -> references = (char *) get_header_data(\
"postings", posting, "References:");
if(! pa -> references)
	{
	pa -> references = strsave("");
	if(! pa -> references)
		{
		if(debug_flag) fprintf(stdout, "malloc references failed\n");
		return(0);
		}
	}

pa -> summary = (char *) get_header_data("postings", posting, "Summary:");
if(! pa -> summary)
	{
	pa -> summary = strsave("");
	if(! pa -> summary)
		{
		if(debug_flag) fprintf(stdout, "malloc summary failed\n");
		return(0);
		}
	}

/* attachment name cannot be obtained from header file */	
if(attachment_flag)
	{
	pa -> attachment = strsave(\
	"unknown attachment present (because of regenerate postings.dat)");
	if(! pa -> attachment)
		{
		if(debug_flag) fprintf(stdout, "malloc attachment failed\n");
		return(0);
		}
	}
else 
	{
	pa -> attachment = strsave("");
	if(! pa -> attachment)
		{
		if(debug_flag) fprintf(stdout, "malloc attachment failed\n");		
		return(0);
		}
	}

/* set custom headers flag */
pa -> custom_headers_flag = 0;
if(local_custom_headers_flag)
	{
	pa -> custom_headers_flag |= LOCAL_HEADERS_ENABLED;
	}
/* cannot determine global */

/* calculate lines */
/* read from body */
pa -> lines = 0;

/* 
It seems encoding cannot be detected, unless a pilot file was introduced 
and encoding stands for encryption that is
*/
/*
if(attachment_flag) pa -> encoding |= ATTACHMENT_PRESENT;
else pa -> encoding = 0;
*/
pa -> encoding = 0;
pa -> send_status = SEND_OK;/* not nessecarely true, but most probable */
pa -> lock_flag = 1;
pa -> browser_line = 0;
pa -> filter_flags = 0;

return(1);
}/* end function regenerate_posting */


char *reformat_text(char *text)
{
char *textptr;
char *last_spaceptr;
int position;

if(debug_flag)
	{
	fprintf(stdout, "reformat_text(): text=%s\n", text);
	}
	
/* reformat text (all of it, from the start) */
textptr = text;
position = 0;
last_spaceptr = 0;
while(1)
	{
	if(*textptr == 0) break;
	if(*textptr == ' ') last_spaceptr = textptr;
	if(*textptr == '\n')
		{
		last_spaceptr = 0;
		position = 0;
		}
	if(position >= LINE_LENGTH)
		{
		if(last_spaceptr) /* break line at space */
			{
			*last_spaceptr = '\n';
			last_spaceptr = 0;
			position = 0;
			}
		else	/*
				cannot break line, no space found, break in word,
				alert user
				*/
			{
			*textptr = '\n';
			position = 0;
			fl_ringbell(100);
			}
/*		fl_set_input(ob, text);*/
		return(strsave(text) );
		} /* end if position >= LINE_LENGTH */
	else /* before max columns */
		{
		position++;
		}
	textptr++;
	}/* en while all characters in text */

/* never here */
return(0);
}/* end function reformat_text */


struct posting *find_next_allowed_posting(char *posting)
{
struct posting *pa, *pstart, *lookup_posting();

if(debug_flag)
	{
	fprintf(stdout, "get_next_allowed_posting(): arg posting=%s\n", posting);
	}

/* parameter check */
if(! posting) return(0);

pa = lookup_posting(posting);
if(! pa)
	{
	/* if not found, start at start list */
	pstart = postingtab[0];
	}
else
	{
	/* set start search for allowed postings */
	pa = pa -> nxtentr;/* point to next posting */
	if(! pa) /* last one in the chain, or empty chain */
		{
		pstart = postingtab[0];
		}
	else
		{
		pstart = pa;
		}
	}/* end else if posting found */	
	
/* search from here to end of this chain */
for(pa = pstart; pa != 0; pa = pa -> nxtentr)
	{
	if(filters_enabled_flag)
		{
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY) continue;
		}

/*fprintf(stdout, "found in this chain name=%s\n", pa -> name);*/
	return(pa);
	}/* end for */

/* search from start chain to end of chain */	
for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(filters_enabled_flag)
		{
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY) continue;
		}

/*fprintf(stdout, "found in second half name=%s\n", pa -> name);*/
	return(pa);
	}/* end for */
return(0);/* not found */
}/* end function find_next_allowed_posting */


int get_first_allowed_posting(long *first)
{
struct posting *pa;

if(debug_flag)
	{
	fprintf(stdout, "get_first_allowed_posting(): arg none\n");
	}

*first = -1;
for(pa = postingtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(filters_enabled_flag)
		{
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_HEADER) continue;
		if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY) continue;
		}

	*first = atol(pa -> name);
	return(1);
	}

return(0);
}/* end function get_first_allowed_posting */


int search_next_posting()
{
struct posting *pa, *lookup_posting();
char temp[80];
static int posting_shown_flag;
char *ptr;
long last;

if(debug_flag)
	{
	fprintf(stdout, "search_next_posting(): arg none, using:\n");
	fprintf(stdout, " selected_posting=%ld\n", selected_posting);	
	fprintf(stdout, " start_posting=%ld\n", start_posting);
	fprintf(stdout, " first_matching_posting=%ld\n", first_matching_posting);
	fprintf(stdout, " first_matching_postings_database_name=%s\n",\
	first_matching_postings_database_name);
	fprintf(stdout, " postings_database_name=%s\n", postings_database_name); 
	}

search_last_search = SEARCH_POSTINGS;

if(posting_shown_flag)
	{
	/* for alt_editor_command_input_field */
	load_general_settings();/* this will set the variables from disk */

	fl_hide_form(fd_NewsFleX -> posting_editor);
	posting_shown_flag = 0;
	}

/*
Note: start_posting set to selected_posting by search_accept_button_cb.
*/

/* for all postings_database_name */
while(1)
	{
	/* for all postings */
	while(1)
		{
		if(debug_flag)
			{
			fprintf(stdout, "search_next_posting(): entering inner loop\n");
			fprintf(stdout, " start_posting=%ld\n", start_posting);
			fprintf(stdout, " selected_posting=%ld\n", selected_posting);
			fprintf(stdout, " postings_database_name=%s\n",\
			postings_database_name);
			fprintf(stdout, " first_matching_posting=%ld\n",\
			first_matching_posting);
			fprintf(stdout, " first_matching_postings_database_name=%s\n",\
			first_matching_postings_database_name);
			}

		/* test for empy group */
		if(selected_posting == -1) break;
		
		/* find next posting */
		last = selected_posting;
		sprintf(temp, "%lu", selected_posting);
		pa = find_next_allowed_posting(temp);
		if(! pa)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
			"inner loop break find_next_allowed_posting returned pa=0\n");			
				}
			break;/* try next postings_database_name */
			}

		if(first_matching_posting == atol(pa -> name) )
			{
			if(first_matching_postings_database_name)
				{
				if(strcmp(\
				first_matching_postings_database_name,\
				postings_database_name) == 0)
					{
					if(debug_flag)
						{
						fprintf(stdout,\
						"search_next_posting(): break inner loop round\n");
						}
					break;
					}			
				}
			}

		if(atol(pa -> name) == start_posting)
			{
			if(debug_flag)
				{
				fprintf(stdout,\
				"break inner loop pa->name==start_posting\n");
				}
			break;		
			}

		selected_posting = atol(pa -> name);

		if(start_posting == -1) start_posting = atol(pa -> name);

		if(search_all("dummy", pa -> name, pa -> encoding,\
		pa -> attachment, 1, SEARCH_POSTINGS) ) /* have_body = 1 */
			{
			/* show the editor */
			show_posting_body(selected_posting);

			/* fill in alt_editor_command_input_field */
			load_general_settings();

			fl_show_form(fd_NewsFleX -> posting_editor,\
			FL_PLACE_CENTER, FL_UP_FRAME, "");

			posting_shown_flag = 1;
			
			if(first_matching_posting == -1)
				{
				first_matching_posting = atol(pa -> name);
				if(first_matching_postings_database_name)
					{
					free(first_matching_postings_database_name);
					}
				first_matching_postings_database_name =\
				strsave(postings_database_name);
				if(! first_matching_postings_database_name) return(0);
				}
			return(1);
			}/* end found */

		/*
		In case only one entry in postings database, then
		find_next_allowed returned the same as it was handed.
		*/
		if(atol(pa -> name) == last) break;

		}/* end while all postings */

	ptr = find_next_posting_period(postings_database_name);
	if(! ptr) return(0);
	if(debug_flag)
		{
		fprintf(stdout,\
		"search_next_posting(): after select next postings_database_name=%s\n",\
		postings_database_name);
		}

	select_posting_period(ptr);

	if(strcmp(start_postings_database_name, ptr) == 0)
		{
		if(! search_articles_flag)
			{
			select_posting_period(start_postings_database_name);
			if(debug_flag)
				{
				fprintf(stdout,\
		"search_next_posting(): return outer loop all has been seached\n");
				}
			fl_show_alert(\
			"All postings have been searched", "", "", 0);

			/* this will close question form from news_cb.c */
			ask_question_flag = 0;
			return(0);/* round */
			}
		if(start_newsserver_and_database)\
		free(start_newsserver_and_database);
		start_newsserver_and_database = malloc(\
		strlen(news_server_name) + strlen(" ") +\
		strlen(database_name) + 1);
		if(! start_newsserver_and_database) return(0);
		strcpy(start_newsserver_and_database, news_server_name);
		strcat(start_newsserver_and_database, " ");
		strcat(start_newsserver_and_database, database_name);

		strcpy(start_group, selected_group);
		start_article = selected_article;
				
		first_matching_article = -1;

		if(first_matching_group) free(first_matching_group);
		first_matching_group = (char *) strsave("");
		go_search_next_article_from_idle_cb_flag = 1;
		return(0);
		}

	/* force no match, try all structures in postingtab */
	get_first_allowed_posting(&selected_posting);
	start_posting = selected_posting;

	}/* end while all posting_periods */

/* never here */
return(0);
}/* end function search_next_posting */


char *fold_header_line(char *text)
{
char *last_space_ptr;
int position;
char *folded_text_ptr;
int input_len;
char *inptr;
char *outptr;

#define MAX_HEADER_LINE_LENGTH 512

if(debug_flag)
	{
	fprintf(stdout, "fold_header_line(): arg text=%s\n", text);
	}

/* argument check */
if(! text) return 0;

input_len = strlen(text);
/* 
for every  MAX_HEADER_LINE_LENGTH add space for LF + a space + string term.
*/
folded_text_ptr =\
(char *)malloc(input_len + (2 * (input_len / MAX_HEADER_LINE_LENGTH) ) + 1);
if(! folded_text_ptr)
	{
	if(debug_flag)
		{
		fprintf(stdout, "fold_header_line(): cannot allocate space\n");
		}
	return 0;
	}

inptr = text;
outptr = folded_text_ptr;
position = 0;
last_space_ptr = 0;
while(1)
	{
	*outptr = *inptr;
	if(*inptr == 0) break;
	if(*inptr == ' ') last_space_ptr = outptr;

	if(position >= MAX_HEADER_LINE_LENGTH)
		{
		if(last_space_ptr)
			{
			*last_space_ptr = '\n';
			inptr = inptr - (outptr - last_space_ptr);
			outptr = last_space_ptr;
			outptr++;
			*outptr = ' ';
			position = 0;
			last_space_ptr = 0;
			}
		else
			{
/*			return 0;*/
			}
		}		

	inptr++;
	outptr++;
	position++;
	}/* end while all characters in text */

return folded_text_ptr;
}/* end function fold_header_line */


char *trim_references_line(char *text)
{
/*
trim above 31 entries,
remove 6th entry , and leave at least last 9 entries.
*/
char *trimmed_text_ptr;
char *inptr;
char *outptr;
int starts;
int ends;
int entries;


if(debug_flag)
	{
	fprintf(stdout, "trim_references_line(): arg text=%s\n", text);
	}

/* argument check */
if(! text) return 0;

/* determine number of entries */
starts = 0;
ends = 0;
inptr = text;
while(1)
	{
	if(! *inptr) break;
	if(*inptr == '<') starts++;
	if(*inptr == '>') ends++;
	inptr++;
	}
if(ends != starts)
	{
	/* fix the line here */
	}
entries = starts;

/* copy the text */
trimmed_text_ptr = strsave(text);

/* using 20, 5 + 9 = 14 actually minimum */
if(entries > 20)
	{
	/* trim the line here */
	starts = 0;
	inptr = text;
	outptr = trimmed_text_ptr;
	while(1)
		{
		if(*inptr == '<') starts++;
		if( (starts >= 6) && (starts < (entries - 9) ) )
			{
			inptr++;
			continue;
			}
		/* also copy string termination */
		*outptr = *inptr;
		if(! *inptr) break;

		inptr++;
		outptr++;
		}/* end while all char */
	}/* end if entries > 31 */

return trimmed_text_ptr;
}/* end function trim_references_line */



int cut_off_sig(char *text)
{
int c, hpos;
char* text_ptr;
int sig_flag;
char *sig_ptr;
char *cut_text;

if(debug_flag)
	{
	fprintf(stdout, "cut_off_sig arg text=%s\n", text);
	}

if(! text)return 0;

text_ptr = text;
hpos = 1; /* the first line does not start with LF, so in fact char 1 */
sig_ptr = text;
sig_flag = 0;
while(1)
	{
	/*
	If we want the signature not to appear if preceded by an empty line
	starting with -- then we must stop here storing characters.
	Note: text is already idented with '>' as this is a reply
	*/
	c = *text_ptr;

	/* stop at end of text */
	if(c == 0) break;

	/* detect line feed */
	if(c == 10)
		{
		if(sig_flag == 2)
			{
			/*
			break from while, sig_ptr holds start of last line,
			leave the LF in if present (first line has none).
			*/
			if(*sig_ptr == 10) sig_ptr++;

			/* string termination */
			*sig_ptr = 0;

			/* ready */
			return 1;
			}/* end if sig_flag */

		hpos = 0;
		sig_flag = 0;

		/* point to start of line */
		sig_ptr = text_ptr;
		}	

	if( (hpos == 2) && (c == '-') ) sig_flag = 1;

	if( (hpos == 3) && (c == '-') ) sig_flag++;

	/*
	any non space characters following '>--' as the first 3 is no start sig
	*/
	if( (hpos > 3) && (c != ' ') ) sig_flag = 0;		

	/* increment horizontal character counter */
	hpos++;

	/* increment the pointer in the txt */
	text_ptr++;
	}/* end while all chars in space */

/* return here if no sig cut */
return 1;
}/* end function cut_off_sig */
