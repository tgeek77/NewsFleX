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


void summary_editor_input_field_cb(FL_OBJECT *ob, long data)
{
}/* end function summary_editor_input_field_cb */


void summary_editor_insert_file_button_cb(FL_OBJECT *ob, long data)
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

/* get cursor position */
cupos =\
fl_get_input_cursorpos(fdui -> summary_editor_input_field, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	fl_show_alert(\
	"Select a position in the editor first", "", "", 0);
	return;
	}

topline = fl_get_input_topline(fdui -> summary_editor_input_field);

/* start file browser */
ptr = (char *) fl_show_fselector("SELECT A FILE", home_dir, "*", "*");
if(! ptr) return;
if(debug_flag)
	{
	fprintf(stdout,\
	"summary_editor_insert_file_button_cb(): selected=%s\n", ptr);
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
input_field_ptr = (char *) fl_get_input(fdui -> summary_editor_input_field);
if(! input_field_ptr) return;

/* create space for total */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return;

fstat(fileno(insert_filefd), statptr);

/* should be + 1 */
space = malloc(statptr -> st_size + strlen(input_field_ptr) + 2);
free(statptr);
if(! space) return;

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

fl_set_input(fdui -> summary_editor_input_field, space);
fl_set_input_topline(fdui -> summary_editor_input_field, topline);
fl_set_input_cursorpos(fdui -> summary_editor_input_field, cux, cuy);

free(space);

posting_modified_flag = 1;
}/* end function summary_editor_insert_file_button_cb */


void summary_editor_write_file_button_cb(FL_OBJECT *ob, long data)
{
/* waiting for xforms function fl_get_input_selected() */
}/* end function summary_editor_write_file_button_cb */


void summary_editor_clear_button_cb(FL_OBJECT *ob, long data)
{
fl_set_input(fdui -> summary_editor_input_field, "");
}/* end function summary_editor_clear_button_cb */


void summary_editor_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(summary_editor_fontsize < 5) summary_editor_fontsize++;
size = integer_to_fontsize(summary_editor_fontsize);

fl_set_object_lsize(fdui -> summary_editor_input_field, size);

fontsizes_modified_flag = 1;
}/* end function summary_editor_fontsize_up_button_cb */


void summary_editor_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

if(summary_editor_fontsize > 0) summary_editor_fontsize--;
size = integer_to_fontsize(summary_editor_fontsize);

fl_set_object_lsize(fdui -> summary_editor_input_field, size);

fontsizes_modified_flag = 1;
}/* end function summary_editor_fontsize_down_button_cb */


void summary_editor_cancel_button_cb(FL_OBJECT *ob, long data)
{
/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

/*
Note: summary_input was set to summary_editor_input_field before it was 
changed
*/
fl_set_input(fdui -> summary_editor_input_field, summary_input);

fl_hide_form(fd_NewsFleX -> summary_editor);
}/* end function summary_editor_cancel_button_cb */


void summary_editor_accept_button_cb(FL_OBJECT *ob, long data)
{
char *ptr;
char *strsave();

/* the new data */
ptr = (char *) fl_get_input(fdui -> summary_editor_input_field);
if(! ptr) return;
free(summary_input);
summary_input = strsave(ptr);

if(strlen(ptr) == 0)
	{
	fl_set_object_color(fdui -> posting_summary_button, FL_COL1, FL_COL1);
	}
else
	{
	fl_set_object_color(fdui -> posting_summary_button,\
	FL_DARKORANGE, FL_DARKORANGE);
	}	
posting_modified_flag = 1;
fl_hide_form(fd_NewsFleX -> summary_editor);

refresh_screen(selected_group);

if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

}/* end function summary_editor_accept_button_cb */


