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


int url_form_modified_flag;
int fontsizes_modified_flag;

void url_form_get_pictures_button_cb(FL_OBJECT *ob, long data)
{
int status;
int selected_line;

selected_line = fl_get_browser(fdui -> url_form_browser);
if(! selected_line) return;

if(! get_url_status(selected_url, &status) ) return;

if( fl_get_button(ob) )
	{
	fl_set_button(ob, 1);
	fl_set_object_label(ob, "PICTURES");
	fl_set_object_color(ob,\
	FL_COL1, FL_YELLOW);
	status |= PICTURES_URL;
	}
else
	{
	fl_set_button(ob, 0);
	fl_set_object_label(ob,\
	"NO PICTURES");
	fl_set_object_color(ob,\
	FL_COL1, FL_COL1);
	status &= (0xffffff - PICTURES_URL);
	}

if(! set_url_status(selected_url, status) ) return;

show_urls(SHOW_SAME_POSITION);

save_urls();

fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

url_form_modified_flag = 1;
}/* end function url_form_get_pictures_button_cb */


void url_form_depth_input_field_cb(FL_OBJECT *ob, long data)
{
int status;
int selected_line;
char *ptr;
int depth;
int maximum_url_depth;

ptr = (char *) fl_get_input(ob);
if(! ptr) return;

maximum_url_depth = atoi(ptr);

selected_line = fl_get_browser(fdui -> url_form_browser);
if(! selected_line) return;

if(! get_url_status(selected_url, &status) ) return;

if(debug_flag)
	{
	fprintf(stdout, "old status=%d %x\n", status, status);
	}

depth = maximum_url_depth << 8;
depth &= 0x00ff00;
status &= 0xff00ff;
status |= depth;

if(debug_flag)
	{
	fprintf(stdout, "new status=%d %x\n",  status, status); 
	}

if(! set_url_status(selected_url, status) ) return;

show_urls(SHOW_SAME_POSITION);

save_urls();

fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

/* form was modified */
url_form_modified_flag = 1;
}/* end function url_form_depth_input_field_cb */


void url_form_host_only_button_cb(FL_OBJECT *ob, long data)
{
int status;
int selected_line;

selected_line = fl_get_browser(fdui -> url_form_browser);
if(! selected_line) return;

if(! get_url_status(selected_url, &status) ) return;

if( fl_get_button(ob) )
	{
	fl_set_button(ob, 1);
	fl_set_object_label(ob, "HOST ONLY");
	fl_set_object_color(ob,\
	FL_COL1, FL_DARKORANGE);
	status |= HOST_ONLY_URL;
	}
else
	{
	fl_set_button(ob, 0);
	fl_set_object_label(ob, "ALL");
	fl_set_object_color(ob,\
	FL_COL1, FL_COL1);
	status &= (0xffffff - HOST_ONLY_URL);
	}	 

if(! set_url_status(selected_url, status) ) return;

show_urls(SHOW_SAME_POSITION);

save_urls();

fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

url_form_modified_flag = 1;
}/* end function url_form_host_only_button_cb */


void url_form_browser_command_input_field_cb(FL_OBJECT *ob, long data)
{
url_form_modified_flag = 1;
}/* end function url_form_browser_command_input_field_cb */


void url_form_browse_online_button_cb(FL_OBJECT *ob, long data)
{
browse_online_flag = fl_get_button(ob);
if(browse_online_flag)
	{
	fl_set_object_label(ob, "BR ONLINE");
	}
else
	{
	fl_set_object_label(ob, "BR OFFLINE");
	}

url_form_modified_flag = 1;
}/* end function url_form_browse_online_button_cb */


void url_form_external_browser_button_cb(FL_OBJECT *ob, long data)
{
external_browser_flag = fl_get_button(ob);
if(external_browser_flag)
	{
	fl_set_object_label(ob, "EXT BROWSER");
	}
else
	{
	fl_set_object_label(ob, "INT BROWSER");
	}

url_form_modified_flag = 1;
}/* end function url_form_external_browser_button_cb */


void url_form_browser_cb(FL_OBJECT *ob, long data)
{
int selected_line;
char *ptr;
int status;
int depth;
char depthstr[20];

if(debug_flag)
	{
	fprintf(stdout, "url_form_browser_cb():\n");
	}

selected_line = fl_get_browser(ob);
if(! selected_line) return;

ptr = line_to_url(abs(selected_line) );
if(!ptr) return;

free(selected_url);
selected_url = strsave(ptr);

/* maintain visible indication that line is selected with single click  */
fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

set_url_input(selected_url);

/* set the buttons once / daily, no pictures / pictures, all / host only,
and input field depth (defaults are first) */

if(! get_url_status(selected_url, &status) ) return;

if(debug_flag)
	{
	fprintf(stdout, "url_form_browser_cb():  status=%d\n", status);
	}

if(status & DAILY_URL)
	{
	fl_set_button(fdui -> url_form_get_daily_button, 1);
	fl_set_object_label(fdui -> url_form_get_daily_button, "DAILY");
	fl_set_object_color(fdui -> url_form_get_daily_button,\
	FL_COL1, FL_DARKORANGE);
	}
else
	{
	fl_set_button(fdui -> url_form_get_daily_button, 0);
	fl_set_object_label(fdui -> url_form_get_daily_button, "ONCE");
	fl_set_object_color(fdui -> url_form_get_daily_button,\
	FL_COL1, FL_COL1);
	}
if(status & HOST_ONLY_URL)
	{
	fl_set_button(fdui -> url_form_host_only_button, 1);
	fl_set_object_label(fdui -> url_form_host_only_button, "HOST ONLY");
	fl_set_object_color(fdui -> url_form_host_only_button,\
	FL_COL1, FL_DARKORANGE);
	}
else
	{
	fl_set_button(fdui -> url_form_host_only_button, 0);
	fl_set_object_label(fdui -> url_form_host_only_button, "ALL");
	fl_set_object_color(fdui -> url_form_host_only_button,\
	FL_COL1, FL_COL1);
	}	 
if(status & PICTURES_URL)
	{
	fl_set_button(fdui -> url_form_get_pictures_button, 1);
	fl_set_object_label(fdui -> url_form_get_pictures_button, "PICTURES");
	fl_set_object_color(fdui -> url_form_get_pictures_button,\
	FL_COL1, FL_YELLOW);
	}
else
	{
	fl_set_button(fdui -> url_form_get_pictures_button, 0);
	fl_set_object_label(fdui -> url_form_get_pictures_button,\
	"NO PICTURES");
	fl_set_object_color(fdui -> url_form_get_pictures_button,\
	FL_COL1, FL_COL1);
	}

depth = (status & 0x00ff00) >> 8;
sprintf(depthstr, "%4d", depth);
fl_set_input(fdui -> url_form_depth_input_field, depthstr); 		

}/* end function url_form_browser_cb */


void url_form_browser_double_cb(FL_OBJECT *ob, long data)
{
int selected_line;
/*char *url;*/
int status;
struct urldata *pu; 
char *used_url;
FILE *fileptr;
int mouse_button;

if(debug_flag)
	{
	fprintf(stdout, "url_form_browser_double_cb():\n");
	}

mouse_button = fl_mouse_button();

status = 0;/* keep gcc -Wall happy */

selected_line = fl_get_browser(ob);
if(! selected_line) return;

/*
url = line_to_url(abs(selected_line) );
if(!url) return;

free(selected_url);
selected_url = strsave(url);
*/

/*show_article_that_url_came_from(selected_url);*/

/* launch external browser, or use internal http / ftp to get url */

if(! get_url_status(selected_url, &status) ) return;

if(mouse_button == FL_RIGHT_MOUSE)
	{
	if(browse_online_flag)
		{
		/* start internal or external browser */
		start_browser(selected_url);
		status |= ACCESSED_URL;
		}
	else /* off line: if have show, else mark for (later) retrieval */
		{
		if(status & HAVE_URL)
			{
			/*
			if browse offline, replace the remote URL with the local
			pathfilename.
			*/
			pu = parse_url(selected_url);
			if(! pu) return;
			used_url = strsave(pu -> localpathfilename);
			free_urldata(pu);
			if(! used_url) return;

			/* test if this used_url present on disk */
			fileptr = fopen(used_url, "r");
			if(fileptr)/* file exists */
				{
				/* close file */
				fclose(fileptr);

				/* start internal or external browser */
				start_browser(used_url);
				
				/* mark accessed */
				status |= ACCESSED_URL;
				}
			else /* file does not exists */
				{
				/* un mark have, file was erased by someone / something  */
				status &= (0xffffff - HAVE_URL);

				/* mark for retrieval */
				status |= RETRIEVE_URL;
				}
			free(used_url);
			}/* end if status is HAVE */
		else
			{
			/* toggle retrieval mode (magenta) */
			if(status & RETRIEVE_URL) status &= (0xffffff - RETRIEVE_URL); 
			else status |= RETRIEVE_URL;
	
			/* form was modified */
			url_form_modified_flag = 1;
			}/* end status is ! HAVE */
		}/* end if ! browse_online_flag */
	}/* end if right mouse button */
else if(mouse_button == FL_LEFT_MOUSE)/* left mouse button */
	{
	/* unconditionally toggle for retrieval if left mouse button */

	/* toggle retrieval flag */
	if(status & RETRIEVE_URL) status &= (0xffffff - RETRIEVE_URL); 
	else status |= RETRIEVE_URL;
	}/* end if left mouse button */
/* moved to single click, xform does not see double click on middle mouse */

/*else*/
 /*if(mouse_button == FL_MIDDLE_MOUSE)*/ /* middle mouse button */
if(1 == 2)
	{
	/* unconditionally toggle for daily retrieval if middle mouse button */
	
	if(status & DAILY_URL)
		{
		status &= (0xffffff - DAILY_URL); 
		}
	else status |= DAILY_URL;
	}/* end if middle mouse button */ 

set_url_status(selected_url, status);

show_urls(SHOW_SAME_POSITION);

save_urls();

fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

/* form was modified */
url_form_modified_flag = 1;

}/* end function url_form_browser_double_cb */


void url_form_input_field_cb(FL_OBJECT *ob, long data)
{
}/* end function url_form_input_field_cb */


void url_form_search_input_field_cb(FL_OBJECT *ob, long data)
{
url_form_reset_last_found_line();
}/* end function url_form_search_input_field_cb */


void url_form_search_button_cb(FL_OBJECT *ob, long data)
{
search_and_select_next_url();
}/* end function url_form_search_button_cb */


void url_form_get_daily_button_cb(FL_OBJECT *ob, long data)
{
int a;
int status;
int selected_line;

selected_line = fl_get_browser(fdui -> url_form_browser);
if(! selected_line) return;

if(! get_url_status(selected_url, &status) ) return;

a = fl_get_button(ob);
if(a)
	{
	fl_set_object_label(ob, "DAILY");
	fl_set_object_color(ob, FL_COL1, FL_DARKORANGE);
	status |= DAILY_URL;		
	}
else
	{
	fl_set_object_label(ob, "ONCE");
	fl_set_object_color(ob, FL_COL1, FL_COL1);
	status &= (0xffffff - DAILY_URL);
	}

if(! set_url_status(selected_url, status) ) return;

show_urls(SHOW_SAME_POSITION);

save_urls();

fl_select_browser_line(fdui -> url_form_browser, abs(selected_line) );

/* form was modified */
url_form_modified_flag = 1;

}/* end function url_form_get_daily_button_cb */


void url_form_search_case_button_cb(FL_OBJECT *ob, long data)
{
url_form_reset_last_found_line();
}/* end function url_form_search_case_button_cb */


void url_form_delete_button_cb(FL_OBJECT *ob, long data)
{
int total_lines;
int s_line;
char *url;

total_lines = fl_get_browser_maxline(fdui -> url_form_browser);
for(s_line = 1; s_line <= total_lines; s_line++)
	{
	if(fl_isselected_browser_line(fdui -> url_form_browser, s_line) )
		{
		url_form_modified_flag = 1;
		url = (char *) line_to_url(s_line);
		if(url)
			{
			/*
			delete file from disk and all files (links) referenced
			by it (if present)
			*/
			/* This uses the structure, it looks up url ! */
/*			delete_url_file_and_all_url_files_referenced_by_it(url);*/


			/* finally delete structure entry */
			delete_url(url);
			}
		}/* end selected line */
	}/* end for all lines */
show_urls(SHOW_SAME_POSITION);
}/* end function url_form_delete_button_cb */


void url_form_clear_button_cb(FL_OBJECT *ob, long data)
{
if(debug_flag)
	{
	fprintf(stdout, "url_form_clear_button_cb(): arg none\n");
	}

fl_set_input(fdui -> url_form_input_field, "");

XSync(fl_get_display(), 0);

/*url_form_modified_flag = 1;*/
}/* end function url_form_clear_button_cb */


void url_form_add_button_cb(FL_OBJECT *ob, long data)
{
int s_line;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout, "url_form_add_button_cb(): arg none\n");
	}

ptr = (char *)fl_get_input(fdui -> url_form_input_field);
if(! ptr) return;

/* prevent empty entries */
if(ptr[0] == 0) return;

if(! add_url(ptr, "", 0) ) return;

show_urls(SHOW_BOTTOM);

/* mark new entry in browser */
url_to_line(selected_url, &s_line);
fl_select_browser_line(fdui -> url_form_browser, s_line);

url_form_modified_flag = 1;
}/* end function url_form_add_button_cb */


void url_form_show_marked_only_button_cb(FL_OBJECT *ob, long data)
{
/* this flag tells show_urls() to show only urls marked for retrieval */
url_form_show_marked_only_flag =\
fl_get_button(fdui -> url_form_show_marked_only_button);

if(url_form_show_marked_only_flag)
	{
	fl_set_object_label(fdui -> url_form_show_marked_only_button,\
	"MARKED ONLY");
	}
else
	{
	fl_set_object_label(fdui -> url_form_show_marked_only_button,\
	"SHOW ALL");
	}	

show_urls(SHOW_BOTTOM);

url_form_modified_flag = 1;
}/* end function url_form_accept_button_cb */


void url_form_fontsize_up_button_cb(FL_OBJECT *ob, long data)
{
int size;

fontsizes_modified_flag = 1;
if(url_form_fontsize < 5) url_form_fontsize++;
size = integer_to_fontsize(url_form_fontsize);

fl_set_browser_fontsize(fdui -> url_form_browser, size);
/*fl_set_object_lsize(fdui -> url_form_input_field, size);*/
}/* end function url_form_fontsize_up_button_cb */


void url_form_fontsize_down_button_cb(FL_OBJECT *ob, long data)
{
int size;

fontsizes_modified_flag = 1;
if(url_form_fontsize > 0) url_form_fontsize--;
size = integer_to_fontsize(url_form_fontsize);

fl_set_browser_fontsize(fdui -> url_form_browser, size);
/*fl_set_object_lsize(fdui -> url_form_input_field, size);*/
}/* end function url_form_fontsize_down_button_cb */


void url_form_cancel_button_cb(FL_OBJECT *ob, long data)
{
/* restore from disc */
if(fontsizes_modified_flag)
	{
	load_fontsizes();
	fontsizes_modified_flag = 0;
	}

/* restore from disc */
if(url_form_modified_flag) 
	{
	load_urls();
	url_form_modified_flag = 0;
	}

fl_hide_form(fd_NewsFleX -> url_form);
}/* end function url_form_cancel_button_cb */


void url_form_accept_button_cb(FL_OBJECT *ob, long data)
{
if(fontsizes_modified_flag)
	{
	save_fontsizes();
	fontsizes_modified_flag = 0;
	}

fl_hide_form(fd_NewsFleX -> url_form);

if(!url_form_modified_flag)  return;

if(! html_file_shown) refresh_screen(selected_group);

save_urls();
}/* end function url_form_accept_button_cb */


char *url_to_local_file(char* url)
{

/* bogus for now */
return(url);
}/* end function url_to_local_file */


