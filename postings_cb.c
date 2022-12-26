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

int fontsizes_modified_flag;


void posting_editor_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(posting_body_editor_fontsize < 5) posting_body_editor_fontsize++;
size = integer_to_fontsize(posting_body_editor_fontsize);

fl_set_object_lsize(fdui -> posting_body_editor, size);

fontsizes_modified_flag = 1;
}/* end function posting_editor_fontsize_up_button_cb */


void posting_editor_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(posting_body_editor_fontsize > 0) posting_body_editor_fontsize--;
size = integer_to_fontsize(posting_body_editor_fontsize);

fl_set_object_lsize(fdui -> posting_body_editor, size);

fontsizes_modified_flag = 1;
}/* end function posting_editor_fontsize_down_button_cb */


void posting_send_now_button_cb(FL_OBJECT *ob, long data)
{
int type;
int a;
int topline;
FILE *attachment_fileptr;
struct stat *statptr;
char temp[TEMP_SIZE];
char *ptr;
char *ptr2;

if(debug_flag)
	{
	fprintf(stdout, "send_now_button_cb(): arg none\n");
	}

/* parameter check :) */

/* test if already send */
if(get_send_status(body_posting) == SEND_OK)
	{
	sprintf(temp,\
	"Posting is already send, send again?");
	if(! fl_show_question(temp, 0) )/* mouse on NO */
		{
		/* scip */
		return;		
		}/* end if user cancel */
	}/* end if already send */

/* test if attachment file exists */
ptr = (char *) fl_get_input(fdui -> posting_attachment_input_field);
if(strlen(ptr) != 0)
	{
	attachment_fileptr = fopen(ptr, "r");
	if(! attachment_fileptr)
		{
		fl_show_alert("Cannot open attachment", ptr, "file not found", 0); 
		return;/* abort send_now */
		}

	/* if file is very long, alert user, and ask if he wants to proceed */
	/* create space for total */
	statptr = (struct stat*) malloc(sizeof(struct stat) );
	if(! statptr) return;
	
	fstat(fileno(attachment_fileptr), statptr);

	if( (statptr -> st_size) > WARNING_FILESIZE)
		{
		sprintf(temp, "WARNING:\nsize of this file is %ld bytes\ncontinue?",\
		statptr -> st_size);
		if(!fl_show_question(temp, 1) )/* 1 = mouse on yes */
			{
			free(statptr);
			fclose(attachment_fileptr);
			return;/* abort send_now */
			}
		}
	free(statptr);
	fclose(attachment_fileptr);
	}/* end length attachment input field is not zero */

if(strlen(fl_get_input(fdui -> posting_to_input_field) ) == 0)
	{
	fl_show_alert(\
	"No newsgroup or email address specified",\
	"fill in destination", "", 0);
	return;
	}

if(strlen(fl_get_input(fdui -> posting_subject_input_field) ) == 0)
	{
	fl_show_alert("No subject specified", "fill in subject field", "", 0);
	return;
	}

if(strlen(fl_get_input(fdui -> posting_body_editor) ) == 0)
	{
	fl_show_alert("No text specified", "enter some text", "", 0);
	return;
	}

a = save_posting(selected_group, selected_article, SEND_NOW);
if(a)
	{
	type = get_posting_type(body_posting);
	if( (type == NEW_ARTICLE) || (type == FOLLOW_UP_ARTICLE) )
		{
		add_to_command_queue("dummy", body_posting , POST_ARTICLE);
		}
	if(type == REPLY_VIA_EMAIL)
		{
		add_to_command_queue("dummy", body_posting , EMAIL_ARTICLE);
		}
	}

if(posting_source == POSTING_MENU)
	{
	if(posting_menu_selection == REPLY_VIA_EMAIL)
		{
		if(! reply_to_incoming_mail_flag)
			{
			ptr = (char *) get_header_data(\
			selected_group, selected_article, "Newsgroups:");
			if(! ptr)
				{
				return;
				}
			/* ptr used here in question ! */
			sprintf(temp, "Copy to newsgroup(s)?\n%s", ptr);
			if(fl_show_question(temp, 0) )/* mouse on NO */
				{
				/*
				set the "To" input field the same as the original article.
				*/
				fl_set_input(fdui -> posting_to_input_field, ptr);

				/*
				If subject starts with Re: set To input field the same as
				the original article, else prepend Re:.
				*/
				ptr2 = (char *)fl_get_input(fdui -> posting_subject_input_field);
				if(! ptr2) return;
				
				if(strncasecmp(ptr2, "Re:", 3) == 0)
					{
					/* set the "To" input field the same as the original article */
					fl_set_input(fdui -> posting_subject_input_field, ptr2);
					}
				else
					{
					/* prepend Re: */
					sprintf(temp, "Re: %s", ptr2);
					fl_set_input(fdui -> posting_subject_input_field, temp);
					}

				posting_menu_selection = FOLLOW_UP_ARTICLE;
				a = save_posting(selected_group, selected_article, SEND_LATER);
				if(a)
					{
					type = get_posting_type(body_posting);
					if( (type == NEW_ARTICLE) || (type == FOLLOW_UP_ARTICLE) )
						{
						add_to_command_queue(\
						"dummy", body_posting , POST_ARTICLE);
						}
					if(type == REPLY_VIA_EMAIL)
						{
						add_to_command_queue(\
						"dummy", body_posting , EMAIL_ARTICLE);
						}
					}/*end if a */

				}/* end answere Yes to copy to news groups question */

			/* free ptr set by get_header_data() */
			free(ptr);
			}/* end if ! reply_to_incoming_mail_flag */
		else
			{
			reply_to_incoming_mail_flag = 0;
			}
		}/* end if posting menu selection is reply via email */
	}/* end if postings_source is posting menu */

fl_hide_form(fd_NewsFleX -> posting_editor);

/* show transmit status */
fl_set_object_label(fdui -> description_display, "POSTING LIST");
show_browser(POSTING_LIST_BROWSER);

refresh_screen(selected_group);

/* make the status of the last article visible */
topline = fl_get_browser_maxline(fdui -> posting_list_browser);
fl_set_browser_topline(fdui -> posting_list_browser, topline);

if(a) process_command_queue_error_flag = 0;

/* for alt_editor_command_input_field */
if(posting_modified_flag)
	{
	save_general_settings();/* this will global settings to disk (word wrap) */
	load_general_settings();/* this will set global variables from disk */
	}

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function posting_send_now_button_cb */


void posting_send_later_button_cb(FL_OBJECT *ob, long data)
{
int a, topline;
FILE *attachment_fileptr;
struct stat *statptr;
char temp[READSIZE];
char *ptr;
char *ptr2;

if(debug_flag)
	{
	fprintf(stdout, "send_later_button_cb(): arg none\n");
	}

/* parameter check :) */

/* at least test if attachment file exists */
/*
it is possible the user did not enter ENTER in the input field,
in that case the posting_modified_flag would not be set.
*/

ptr = (char *) fl_get_input(fdui -> posting_attachment_input_field);
if(! ptr) return;

if(strlen(ptr) != 0)
	{
	/* force modification of the posting structure */
	/*
	Because it is possibe to mark the article for transmit from the postings
	list, the test must be done before the send, else the system will stall,
	waiting for the answere to yes / no, or maybe abort due to a failing open.
	*/
	posting_modified_flag = 1;

	attachment_fileptr = fopen(ptr, "r");
	if(! attachment_fileptr)
		{
		fl_show_alert("Cannot open attachment", ptr, "file not found", 0); 
		return;/* abort send_later */
		}

	/* if file is very long, alert user, and ask if he wants to proceed */
	/* create space for total */
	statptr = (struct stat*) malloc(sizeof(struct stat) );
	if(! statptr) return;
	
	fstat(fileno(attachment_fileptr), statptr);

	if( (statptr -> st_size) > WARNING_FILESIZE)
		{
		sprintf(temp, "WARNING:\nsize of this file is %ld bytes\ncontinue?",\
		statptr -> st_size);
		if(!fl_show_question(temp, 1) )/* 1 = mouse on yes */
			{
			free(statptr);
			fclose(attachment_fileptr);
			return;/* abort send_now */
			}
		}
	free(statptr);
	fclose(attachment_fileptr);
	}/* end length attachment input field is not zero */

a = save_posting(selected_group, selected_article, SEND_LATER);

if(posting_source == POSTING_MENU)
	{
	if(posting_menu_selection == REPLY_VIA_EMAIL)
		{
		if(! reply_to_incoming_mail_flag)
			{
			ptr = (char *) get_header_data(\
			selected_group, selected_article, "Newsgroups:");
			if(! ptr)
				{
				return;
				}

			/* ptr used herein question! */
			sprintf(temp, "Copy to newsgroup(s)?\n%s?", ptr);
			if(fl_show_question(temp, 0) )/* mouse on NO */
				{
				/*
				set the "To" input field the same as the original article.
				*/
				fl_set_input(fdui -> posting_to_input_field, ptr);

				/*
				If subject starts with Re: set To input field the same as
				the original article, else prepend Re:.
				*/
				ptr2 = (char *)fl_get_input(fdui ->posting_subject_input_field);
				if(! ptr2) return;
				
				if(strncasecmp(ptr2, "Re:", 3) == 0)
					{
					/* set the "To" input field the same as the original article */
					fl_set_input(fdui -> posting_subject_input_field, ptr2);
					}
				else
					{
					/* prepend Re: */
					sprintf(temp, "Re:%s", ptr2);
					fl_set_input(fdui -> posting_subject_input_field, temp);
					}

				posting_menu_selection = FOLLOW_UP_ARTICLE;
				a = save_posting(selected_group, selected_article, SEND_LATER);
				}
			/* free ptr set by get header data */
			free(ptr);
			}/* end if ! reply_to_incoming_mail_flag */
		else
			{
			reply_to_incoming_mail_flag = 0;
			}
		}/* end if posting_menu_selection is reply via email */
	}/* end if postings_source is posting menu */

fl_hide_form(fd_NewsFleX -> posting_editor);

fl_set_object_label(fdui -> description_display, "POSTING LIST");
show_browser(POSTING_LIST_BROWSER);

refresh_screen(selected_group);

/* make the status of the last article visible */
topline = fl_get_browser_maxline(fdui -> posting_list_browser);
fl_set_browser_topline(fdui -> posting_list_browser, topline);

/* for alt_editor_command_input_field */
if(posting_modified_flag)
	{
	save_general_settings();/* this will write global vars disk */
	load_general_settings();/* this will set global from disk */
	}

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function posting_send_later_button_cb */


void posting_cancel_button_cb(FL_OBJECT *ob, long data)
{
fl_set_object_label(fdui -> description_display, "POSTING LIST");

fl_hide_form(fd_NewsFleX -> posting_editor);

/* for alt_editor_command_input_field */
load_general_settings();/* this will set the variables from disk */

/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

reply_to_incoming_mail_flag = 0;

/* for alt_editor_command_input_field */
if(posting_modified_flag)
	{
	load_general_settings();/* this will set global from disk */
	}
}/* end function posting_cancel_button_cb */


void posting_to_input_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)fprintf(stdout, "TO INPUT CB\n");
posting_modified_flag = 1;
}/* end function posting_to_input_cb */


void posting_subject_input_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)fprintf(stdout, "SUBJECT INPUT CB\n");
posting_modified_flag = 1;
}/* end function posting_subject_input_cb */


void posting_attachment_input_cb(FL_OBJECT *ob, long data)
{
char *ptr;
FILE *attachment_fileptr;
struct stat *statptr;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "posting_attachment_input_cb\n");
	}
	
/* test if attachment file exists */
ptr = (char *) fl_get_input(fdui -> posting_attachment_input_field);
attachment_fileptr = fopen(ptr, "r");
if(! attachment_fileptr)
	{
	fl_show_alert("Cannot open attachment", ptr, "file not found", 0); 
	return;
	}

/* if file is very long, alert user, and ask if he wants to proceed */
/* create space for total */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return;

fstat(fileno(attachment_fileptr), statptr);

if( (statptr -> st_size) > WARNING_FILESIZE)
	{
	sprintf(temp, "WARNING:\nsize of this file is %ld bytes\ncontinue?",\
	statptr -> st_size);
	if(!fl_show_question(temp, 1) )/* 1 = mouse on yes */
		{
		fl_set_input(fdui -> posting_attachment_input_field, "");
		/*
		Posting may be modified, if changing existing posting,
		and NO was answered, the now empty field must be written to the 
		structure.
		*/
		}
	}
free(statptr);
fclose(attachment_fileptr);

posting_modified_flag = 1;
}/* end function posting_attachment_input_cb */


void posting_body_editor_cb(FL_OBJECT *ob, long data)
{
int cux, cuy;
int cupos;
int position;
char *last_spaceptr;
char *text;
char *textptr;

if(debug_flag)
	{
	fprintf(stdout, "posting_body_editor_cb(): arg none\n");
	}

if(posting_word_wrap_flag)
	{
	/*
	Trying to keep readable on other news readers that maybe do not have a
	scroll bar.
	Funny effects may appear to happen if a variable font is used, as it seems
	the reformatting is not correct, but both with  a variable- and fixed spaced
	font reading is now guaranteed.
	*/

	cupos = fl_get_input_cursorpos(ob, &cux, &cuy);
	if(cupos == -1) return;/* no focus */

/*
	fprintf(stdout, "cupos=%d cux=%d cuy=%d\n", cupos, cux, cuy);
*/

	if(cux >= global_posting_wrap_chars)
		{
		text = (char *) fl_get_input(ob);

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
			if(position >= global_posting_wrap_chars)
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
				fl_set_input(ob, text);
				}
			else /* before max columns */
				{
				position++;
				}
			textptr++;
			}/* en while all characters in text */
		}/* end cux > global_posting_wrap_chars */
	
	fl_set_input_cursorpos(ob, cux, cuy);
	}/* end if posting_word_wrap_flag */

/* used by extract url (in posting_body_editor_cb) */
/*
free(posting_body_copy);
posting_body_copy = strsave((char *) fl_get_input(ob));
*/

posting_modified_flag = 1;
}/* end function posting_body_editor_cb */


void posting_summary_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;

ptr = (char *) fl_get_input(fdui -> summary_editor_input_field);
if(! ptr) return;

free(summary_input);
summary_input = strsave(ptr);

fl_show_form(fdui -> summary_editor, FL_PLACE_CENTER, FL_NOBORDER, "");
}/* end function posting_summary_button_cb */


void posting_custom_headers_button_cb(FL_OBJECT *ob, long data)
{
int c;
char temp[TEMP_SIZE];
FILE *load_filefd;
struct stat *statptr;
char *space, *spaceptr;

/* display the global headers in the global headers editor (if any ) */
sprintf(temp, "%s/.NewsFleX/global_custom_head", home_dir);
load_filefd = fopen(temp, "r");

/* clear display */
fl_set_input(fdui -> global_custom_headers_editor_input_field, "");
if(load_filefd)
	{
	statptr = (struct stat*) malloc(sizeof(struct stat) );
	if(! statptr) return;
	fstat(fileno(load_filefd), statptr);
	
	space = malloc(statptr -> st_size + 1);
	free(statptr);
	if(! space)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
	"posting_custom_headers_button_cb(): malloc could not allocate space\n");
			}
		return;
		}

	spaceptr = space;
	while(1)
		{
		while(1)/* error re read */
			{
			c = getc(load_filefd);
			if(! ferror(load_filefd) ) break;
			perror(\
			"custom_headers_button_cb(): global_custom_header read failed ");
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

	fl_set_input(fdui -> global_custom_headers_editor_input_field, space);
	free(space);

	}/* end if global headers file open ok (present) */

get_custom_headers_flag(selected_posting, &custom_headers_state);
headers_modified_flag = 0;

fl_show_form(fd_NewsFleX -> custom_headers_editor,\
FL_PLACE_CENTER, FL_NOBORDER, "");
}/* end function posting_custom_headers_button_cb */


void posting_attach_file_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;

ptr = (char *) fl_show_fselector("SELECT AN ATTACHMENT", home_dir, "*", "*");
if(! ptr) return;

if(debug_flag)
	{
	fprintf(stdout, "attach_file_button_cb(): selected=%s\n", ptr);
	}

fl_set_input(fdui -> posting_attachment_input_field, ptr);
posting_modified_flag = 1;
}/* end function posting_attach_file_buttun_cb */


void posting_insert_file_button_cb(FL_OBJECT *ob, long data)
{
int c, i;
int cupos, cux, cuy;
int topline;
char *ptr;
char *input_field_ptr;
struct stat *statptr;
char *space, *spaceptr;
FILE *insert_filefd;
char insert_filename[TEMP_SIZE];
char temp[TEMP_SIZE];

/* get cursor position */
cupos = fl_get_input_cursorpos(fdui -> posting_body_editor, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	fl_show_alert(\
	"Select a position in the posting body editor first", "", "", 0);
	return;
	}

topline = fl_get_input_topline(fdui -> posting_body_editor);

/* start file browser */
ptr = (char *) fl_show_fselector("SELECT A FILE", home_dir, "*", "*");
if(! ptr) return;
if(debug_flag)
	{
	fprintf(stdout,\
	"posting_insert_file_button_cb(): selected=%s\n", ptr);
	}

/* try to open file for read */
sprintf(insert_filename, ptr);
insert_filefd = fopen(insert_filename, "r");
if(! insert_filefd)
	{
	fl_show_alert("Cannot open file", insert_filename, "for read", 0);
	return;
	}

/* get length from input */
input_field_ptr = (char *) fl_get_input(fdui -> posting_body_editor);
if(! input_field_ptr) return;

/* create space for total */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return;

fstat(fileno(insert_filefd), statptr);

/* alert user, and ask if he wants to proceed if file is very long */
if( (statptr -> st_size) > WARNING_FILESIZE)
	{
	sprintf(temp, "WARNING:\nsize of this file is %ld bytes\ncontinue?",\
	statptr -> st_size);
	if(!fl_show_question(temp, 1) )/* 1 = mouse on yes */
		{
		fclose(insert_filefd);
		free(statptr);
		return;
		}
	}
space = malloc(statptr -> st_size + strlen(input_field_ptr) + 1);
if(! space) return;

free(statptr);
	
/* read from input field until cursor position into space */
spaceptr = space;
for(i = 0; i < cupos; i++)
	{
	c = input_field_ptr[i];
	*spaceptr = c;
	if(c == 0) break;/* end of string */
	spaceptr++;
	}

/* read from file */
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(insert_filefd);
		if(! ferror(insert_filefd) ) break;
		perror("posting_insert_file(): read failed ");
		}/* end while error re read */	

	if(feof(insert_filefd) )
		{
		fclose(insert_filefd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */

/* read from input field again */
while(1)
	{
	c = input_field_ptr[i];
	*spaceptr = c;
	if(c == 0) break;/* end of string */
	spaceptr++;
	i++;
	}
*spaceptr = 0;/* string termination */

fl_set_input(fdui -> posting_body_editor, space);
fl_set_input_topline(fdui -> posting_body_editor, topline);
fl_set_input_cursorpos(fdui -> posting_body_editor, cux, cuy);

free(space);

posting_modified_flag = 1;
}/* end function posting_insert_file_button_cb */


/* UNTESTED */
void posting_write_file_button_cb(FL_OBJECT *ob, long data)
{
int c, i;
char temp[TEMP_SIZE];
int cupos, start, end;
char *ptr;
char *input_field_ptr;
FILE *write_filefd;
char write_filename[TEMP_SIZE];

/* get cursor position */
cupos = 0;
start = 0;
end = 0;

/*
Most unfortunatly this function does not exist yet in xforms.086
cupos = fl_get_input_selected(fdui -> posting_body_editor, &start, &end);
*/
if(cupos == -1)/* field not selected */
	{
	fl_show_alert(\
	"Select a position in the posting body editor first", "", "", 0);
	return;
	}

/* start file browser */
ptr = (char *) fl_show_fselector("SELECT A FILE", home_dir, "*", "*");
if(! ptr) return;
if(debug_flag)
	{
	fprintf(stdout,\
	"posting_write_file_button_cb(): selected=%s\n", ptr);
	}

/* test if file already exists */
/* try to open file for read */
sprintf(write_filename, ptr);
write_filefd = fopen(write_filename, "r");
if(write_filefd)
	{
	fclose(write_filefd);
	sprintf(temp, "File %s already exists, overwrite?", write_filename);
	if(! fl_show_question(temp, 3) ) return;
	}

/* try to open file for write */
sprintf(write_filename, ptr);
write_filefd = fopen(write_filename, "w");
if(! write_filefd)
	{
	fl_show_alert("Cannot open file", write_filename, "for write", 0);
	return;
	}

/* get input */
input_field_ptr = (char *) fl_get_input(fdui -> posting_body_editor);
if(! input_field_ptr) return;

for(i = start; i < end; i++)
	{
	c = input_field_ptr[i];
	while(1)
		{
		fputc(c, write_filefd);
		if(ferror(write_filefd) )
			{
			perror("posting_write_file_button_cb(): write failed ");
			continue;
			}
		else break;
		}
	}/* end for all selected chars */
fclose(write_filefd);
}/* end function posting_write_file_button_cb */


void posting_editor_signature_button_cb(FL_OBJECT *ob, long data)
{
int c, i;
int cupos, cux, cuy;
int topline;
char *input_field_ptr;
struct stat *statptr;
char *space, *spaceptr;
FILE *insert_filefd;
char insert_filename[TEMP_SIZE];

topline = fl_get_input_topline(fdui -> posting_body_editor);

/* get cursor position */
cupos = fl_get_input_cursorpos(fdui -> posting_body_editor, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	fl_show_alert(\
	"Select the posting body editor first", "", "", 0);
	return;
	}

/* try to open file for read */
sprintf(insert_filename, "%s/.NewsFleX/signature", home_dir);
insert_filefd = fopen(insert_filename, "r");
if(! insert_filefd)
	{
	fl_show_alert("Cannot open file", insert_filename, "for read", 0);
	return;
	}

/* get length from input */
input_field_ptr = (char *) fl_get_input(fdui -> posting_body_editor);
if(! input_field_ptr) return;

/* create space for total */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return;

fstat(fileno(insert_filefd), statptr);

space = malloc(statptr -> st_size + strlen(input_field_ptr) + 1);
free(statptr);
if(! space) return;

/* read from input field until cursor position into space */
i = 0;
spaceptr = space;
while(1)
	{
	c = input_field_ptr[i];
	*spaceptr = c;
	if(c == 0) break;/* end of string */
	spaceptr++;
	i++;
	}

/* read from file */
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(insert_filefd);
		if(! ferror(insert_filefd) ) break;
		perror("posting_editor_signature_button_cb(): read failed ");
		}/* end while error re read */	

	if(feof(insert_filefd) )
		{
		fclose(insert_filefd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */

*spaceptr = 0;/* string termination */

fl_set_input(fdui -> posting_body_editor, space);

fl_set_input_topline(fdui -> posting_body_editor, topline);
fl_set_input_cursorpos(fdui -> posting_body_editor, cux, cuy);

free(space);

posting_modified_flag = 1;

}/* end function posting_editor_signature_button_cb */


void posting_editor_clear_button_cb(FL_OBJECT *ob, long data)
{
/*
fprintf(stdout, "MIN MAX INT_MIN=%d INT_MAX=%d\n", INT_MIN, INT_MAX);
return;
*/

fl_set_input(fdui -> posting_body_editor, "");
}/* end function posting_editor_clear_button_cb */


void posting_coding_menu_cb(FL_OBJECT *ob, long data)
{
int posting_coding_menu_selection;

posting_coding_menu_selection = fl_get_menu(ob);

/* if nothing selected, but mouse in menu, no action */
if(posting_coding_menu_selection <= 0) return;

if(posting_coding_menu_selection == PGP_ENCRYPT)
	{
	pgp_encrypt_posting();
	}
if(posting_coding_menu_selection == PGP_CLEAR_SIGN)
	{
	pgp_clear_sign_posting();
	}
if(posting_coding_menu_selection == PGP_APPEND_PUBLIC_KEY)
	{
	pgp_append_public_key_to_posting();
	}
if(posting_coding_menu_selection == OTP_ENCRYPT)
	{
	otp_encrypt_posting();
	}
if(posting_coding_menu_selection == OTP_CREATE_RANDOM_KEY_FILE)
	{
	otp_create_random_key_file();
	}
if(posting_coding_menu_selection == SHOW_UNENCRYPTED)
	{
	show_unencrypted_posting(selected_posting);
	}
if(posting_coding_menu_selection == SEARCH_KEYWORD)
	{
	search_for_keyword(fdui -> posting_body_editor);
	}
if(posting_coding_menu_selection == WORD_WRAP)
	{
	posting_word_wrap_flag = 1 - posting_word_wrap_flag;
	if(posting_word_wrap_flag)
		{
		fl_set_menu_item_mode(fdui -> posting_coding_menu,\
		WORD_WRAP, FL_PUP_CHECK);
		}
	else
		{
		fl_set_menu_item_mode(fdui -> posting_coding_menu,\
		WORD_WRAP, FL_PUP_BOX);
		}

/*	save_general_settings();*/
	}/* end if WORD_WRAP */

posting_modified_flag = 1;
}/* end function posting_coding_menu_cb */


void posting_cross_post_button_cb(FL_OBJECT *ob, long data)
{
char *ptr, *ptr2, *ptr3;
int cross_posts;
char temp[TEMP_SIZE];

ptr = (char *) fl_show_input("Group key words (AND)", "");
if(! ptr) return;

ptr2 = extract_keyword_groups(ptr);
if(!ptr2) return;

/* check for excessive cross posting */
ptr3 = ptr2;
cross_posts = 0;
while(1)
	{
	if(! *ptr3) break;
	if(*ptr3 == ',') cross_posts++;
	ptr3++;
	}
if(cross_posts > WARNING_CROSS_POSTS)
	{
	sprintf(temp,\
	"This posting is cross posted to %d groups, continue?", cross_posts);
	if(!fl_show_question(temp, 0) )/* mouse on NO */
		{
		fl_set_object_label(fdui -> command_status_display,\
		"news group generation aborted");
		return;
		}
	}/* end to many cross posts */

fl_set_input(fdui -> posting_to_input_field, ptr2);
if(strcmp(ptr, ptr2) != 0) posting_modified_flag = 1;

free(ptr2);
}/* end function posting_cross_post_button_cb */


void posting_add_url_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
ptr = (char*)fl_get_input(fdui -> posting_body_editor);
if(! ptr) return;
extract_url_from_input(fdui -> posting_body_editor,\
ptr, "", 0);
/*
extract_url_from_input(fdui -> posting_body_editor,\
posting_body_copy, "", 0);
*/
return;
}/* end function posting_add_url_button_cb */


void posting_alt_editor_button_cb(FL_OBJECT *ob, long data)
{
if( connected() )
	{
	fl_set_object_label(fdui -> command_status_display,\
	"ALT EDITOR NOT AVAILABLE ONLINE");
	return;
	}

do_alt_editor();
posting_modified_flag = 1;
}/* end function posting_alt_editor_button_cb */


void posting_alt_editor_command_input_field_cb(FL_OBJECT *ob, long data)
{
posting_modified_flag = 1;
}/* end function posting_alt_command_editor_input_field_cb */


