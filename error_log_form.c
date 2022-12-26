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


int show_error_log_form()
{
int c;
char temp[TEMP_SIZE];
FILE *log_fileptr;
struct stat *statptr;
char *space, *spaceptr;
char *expanded_space;

if(debug_flag)
	{
	fprintf(stdout, "show_error_log_form(): arg: none\n");
	}

/* make sure latest errors are written to disk */
/*fflush(error_log_fileptr);*/

sprintf(temp, "%s/.NewsFleX/error_log.txt", home_dir);
log_fileptr = fopen(temp, "r");
if(! log_fileptr)/* there were no errors */
	{
/*	fl_show_alert("could not open file", temp, "for read", 0);*/

	/* clear display  */
	fl_set_input(fdui -> error_log_form_input, "There were no errors");

	/* show form */
	fl_show_form(fd_NewsFleX -> error_log_form, FL_PLACE_CENTER, FL_NOBORDER, "");

	return(1);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(!statptr) return(0);

fstat(fileno(log_fileptr), statptr);

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_error_log_form(): malloc could not allocate space for body\n");
		}
	return(0);
	}

/* speed up things */
fl_freeze_form(fd_NewsFleX -> error_log_form);

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(log_fileptr);
		if(! ferror(log_fileptr) ) break;
		perror("show_error_log_form(): read failed ");
		}/* end while error re read */	

	if(feof(log_fileptr) )
		{
		fclose(log_fileptr);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */
*spaceptr = 0;/* string termination */

expanded_space = (char *) expand_tabs(space, tab_size);
free(space);

fl_set_input(fdui -> error_log_form_input, expanded_space);
free(expanded_space);

fl_unfreeze_form(fd_NewsFleX -> error_log_form);

fl_show_form(fd_NewsFleX -> error_log_form, FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function show_error_log_form */


int erase_error_log()
{
char temp[TEMP_SIZE];

/* close the global */
/*fclose(error_log_fileptr);*/

/* delete error_log.txt */
sprintf(temp, "%s/.NewsFleX/error_log.txt", home_dir);
unlink(temp);

return(1);
}/* end function erase_error_log */

