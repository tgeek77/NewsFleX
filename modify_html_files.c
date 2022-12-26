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


#define EXPECT_SUB			1
#define IN_HREF				2
#define IN_SRC				3
#define IN_BACKGROUND		4
#define IN_URN				5
#define IN_REL				6
#define IN_REV				7
#define IN_TITLE			8
#define IN_METHODS			9
#define IN_TEXT				10
#define IN_BGCOLOR			11
#define IN_LINK				12
#define IN_YLINK			13
#define IN_ALINK			14
#define IN_ALIGN			15
#define IN_ISMAP			16
#define IN_ALT				17
#define IN_NAME				18
#define IN_COMMENT			19
#define IN_LANGUAGE			20
#define IN_WIDTH			21
#define IN_HEIGHT			22
#define IN_MARGINWIDTH		23
#define IN_MARGINHEIGHT		24
#define IN_HSPACE			25
#define IN_VSPACE			26
#define IN_FRAMEBORDER		27
#define IN_SCROLLING		28
#define IN_BORDERCOLOR		29

char *local_path;
char *local_path_f;

struct html_file
	{
	char *name;/* this is the html_file */
	int flags;
	struct html_file *nxtentr;
	struct html_file *prventr;
	};
struct html_file *html_filetab[2]; /* first element points to first entry,
			second element to last entry */


struct html_file *lookup_html_file(char *name)
{
struct html_file *pa;

if(! name) return(0);

/* pa points to next entry */
for(pa = html_filetab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(! pa -> name) continue;
	if(strcmp(pa -> name, name) == 0) return(pa);
	}

return(0);/*not found*/
}/* end function lookup_html_file */


struct html_file *install_html_file_at_end_of_list(char *name)
{
struct html_file *plast, *pnew;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_html_file_at_end_off_list(): arg name=%s\n", name);
	}

if(! name) return(0);

pnew = lookup_html_file(name);
if(pnew)
	{
	/* free previous definition */
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct html_file *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = html_filetab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !html_filetab[0] ) html_filetab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
html_filetab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_html_file */


int delete_html_file_entry(char *name)/* delete entry from double linked list */
{
struct html_file *pa, *pprev, *pdel, *pnext;

if(debug_flag)
	{
	fprintf(stdout, "delete_html_file_entry(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pa = html_filetab[0];
while(1)
	{
	/* if end list, return not found */
	if(! pa) return(0);

	/* test for match in name */
	if(strcmp(name, pa -> name) != 0) /* no match */
		{
		/* point to next element in list */
		pa = pa -> nxtentr;

		/* loop for next element in list */
		continue;
		}

	/* we now know which struture to delete */
	pdel = pa;

	/* get previous and next structure */
	pnext = pa -> nxtentr;
	pprev = pa -> prventr;

	/* set pointers for previous structure */
	/* if first one, modify html_filetab[0] */
	if(pprev == 0) html_filetab[0] = pnext;
	else pprev -> nxtentr = pnext;

	/* set pointers for next structure */
	/* if last one, modify html_filetab[1] */
	if(pnext == 0) html_filetab[1] = pprev;
	else pnext -> prventr = pprev;
	
	/* delete structure */	
	free(pdel -> name);
	free(pdel); /* free structure */

	/* return OK deleted */
	return(1);
	}/* end for all structures */
}/* end function delete_html_file_entry */


int delete_all_html_file_entries()/* delete all entries from table */
{
struct html_file *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_html_file_entries() arg none\n");
	}

while(1)
	{	
	pa = html_filetab[0];
	if(! pa) break;
	html_filetab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */

html_filetab[1] = 0;
return(1);
}/* end function delete_all_html_file_entries */


int add_html_file(char *filename, int flags)
{
struct html_file *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_html_file(): arg filename=%s flags=%d\n",\
	filename, flags);
	}

/* argument check */
if(! filename) return(0);

pa = install_html_file_at_end_of_list(filename);
if(! pa) return(0);

pa -> flags = flags;
return(1);
}/* end function add_html_file */


int modify_all_html_files()
{
struct html_file *pa;

if(debug_flag)
	{
	fprintf(stdout, "modify_all_html_files(): arg none\n");
	}

for(pa = html_filetab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(! pa -> name) continue;
	write_modified_html_file(pa -> name, pa -> flags);
	}

delete_all_html_file_entries();
return(1);
}/* end function modify_all_html_files */


int http_modify_url(char *url, FILE *outfileptr, int image_flag, char *host)
{
char *ptr1;
char *ptr2;

if(! url) return(0);
if(! outfileptr) return(0);
if(! host) return(0);

if(debug_flag)
	{
	fprintf(stdout, "http_modify_url(): url=%s host=%s\n", url, host);
	}

/* lookup in move url list (holds stripped entries), and replace if moved */
/* strip any [http://][www.] */
ptr1 = strip_http_www(url);
if(! ptr1) return(0);
ptr2 = lookup_new_location(ptr1);
if(ptr2) ptr1 = ptr2;
else ptr1 = url;

/*
Handle cgi-bin?count and the like.
These files are modified to look like:
cgi-bin~count
*/
/* replace any '?' with '~' */
ptr2 = strchr(ptr1, '?');
if(ptr2) *ptr2 = '~';

/*
if URL starts with http:// then file://home_dir is prepended.
http://somehost[/somedir]/somefile
file://home_dir/.NewsFleX/http/somehost/somedir/somefile 

if URL starts with / then file://home_dir/host is prepended.
/somedir/somefile
file://home_dir/.NewsFleX/http/host/somedir/somefile

FTP here????

else if URL ends in /. index.html is appended.

else if URL starts without '/':
 if it starts with '#' local path filename is prepended.
 #some_position_in_this_document
 file://local_path_filename/#some_position_in_this_document

 if it starts with mailto: it is left as it is.
 mailto:flip@flop.net
 mailto:flip@flop.net

 else local path is prepended.
 somefile
 file://local_path/somefile

*/

if(strncasecmp(ptr1, "HTTP://", 7) == 0) /* somewhere in the world */
	{
	/* strip any http://www. */
	if(strncasecmp(ptr1 + 7, "WWW.", 4) == 0)
		{
		fprintf(outfileptr, "file://%s/.NewsFleX/http/%s",\
		home_dir, ptr1 + 11);
		}
	else /* strip any http:// */
		{
		fprintf(outfileptr, "file://%s/.NewsFleX/http/%s",\
		home_dir, ptr1 + 7);
		}
	}
else if(strncmp(ptr1, "/", 1) == 0) /* absolute to host */
	{
	fprintf(outfileptr, "file://%s/.NewsFleX/http/%s%s",\
	home_dir, host, ptr1);
	}
else /*
	 relative to this dir, but we need an absolute path for the url
	 extractor (mouse click), now it can look for home_dir/.NewsFleX/http/
	 */
	{
	if(*ptr1 == '#')
		{
		fprintf(outfileptr, "file://%s%s", local_path_f, ptr1);
		}
	else if(strncasecmp(ptr1, "mailto:", 7) == 0)
		{
		fprintf(outfileptr, "%s", ptr1);
		}
	else
		{
		fprintf(outfileptr, "file://%s%s", local_path, ptr1);
		}
	}

if(! image_flag)
	{
	/* if url ends in /, append index.html */
	if(ptr1[strlen(ptr1) - 1] == '/')
		{
		fprintf(outfileptr, "index.html");
		}
	}

return(1);
}/* end function http_modify_url */


int modify_comment(char *position, char **new_pos, FILE *outfileptr)
{
char *ptr;
int in_comment_flag;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<!", 2) != 0) return(0);
ptr += 2;

fprintf(outfileptr, "<!");

in_comment_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(! in_comment_flag)
		{
		if(*ptr == '>')
			{
			*new_pos = ptr;
			return(1);
			}
		}

	if(strncasecmp(ptr, "--", 2) == 0)
		{
		in_comment_flag = 1 - in_comment_flag;

		fprintf(outfileptr, "--");
		ptr += 2;
		continue;
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */

}/* end function modify_comment */


int modify_base(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<BASE", 5) != 0) return(0);
ptr += 5;

fprintf(outfileptr, "<BASE");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			fprintf(outfileptr, "HREF");
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;
		
		http_modify_url(temp, outfileptr, 0, host);
		
		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_base */


int modify_body(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<BODY", 5) != 0) return(0);
ptr += 5;

fprintf(outfileptr, "<BODY");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_BACKGROUND) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "BACKGROUND", 10) == 0)
			{
			fprintf(outfileptr, "BACKGROUND");
			mode = IN_BACKGROUND;
			ptr += 10;
			continue;
			}
		else if(strncasecmp(ptr, "TEXT", 4) == 0)
			{
			fprintf(outfileptr, "TEXT");
			mode = IN_TEXT;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "BGCOLOR", 7) == 0)
			{
			fprintf(outfileptr, "BGCOLOR");
			mode = IN_BGCOLOR;
			ptr += 7;
			continue;
			}		
		else if(strncasecmp(ptr, "LINK", 4) == 0)
			{
			fprintf(outfileptr, "LINK");
			mode = IN_LINK;
			ptr += 4;
			continue;
			}		
		else if(strncasecmp(ptr, "YLINK", 5) == 0)
			{
			fprintf(outfileptr, "YLINK");
			mode = IN_YLINK;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "ALINK", 5) == 0)
			{
			fprintf(outfileptr, "YLINK");
			mode = IN_ALINK;
			ptr += 5;
			continue;
			}		
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_BACKGROUND) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 1, host);
		
		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_body */


int modify_link(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<LINK", 5) != 0) return(0);
ptr += 5;

fprintf(outfileptr, "<LINK");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			fprintf(outfileptr, "HREF");
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "URN", 3) == 0)
			{
			fprintf(outfileptr, "URN");
			mode = IN_URN;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "REL", 3) == 0)
			{
			fprintf(outfileptr, "REL");
			mode = IN_REL;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "REV", 3) == 0)
			{
			fprintf(outfileptr, "REV");
			mode = IN_REV;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "TITLE", 5) == 0)
			{
			fprintf(outfileptr, "TITLE");
			mode = IN_TITLE;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "METHODS", 7) == 0)
			{
			fprintf(outfileptr, "METHODS");
			mode = IN_METHODS;
			ptr += 7;
			continue;
			}		

		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 0, host);

		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_link */


int modify_a(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<A", 2) != 0) return(0);
ptr += 2;

fprintf(outfileptr, "<A");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			fprintf(outfileptr, "HREF");
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "NAME", 4) == 0)
			{
			fprintf(outfileptr, "NAME");
			mode = IN_NAME;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "URN", 3) == 0)
			{
			fprintf(outfileptr, "URN");
			mode = IN_URN;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "REL", 3) == 0)
			{
			fprintf(outfileptr, "REL");
			mode = IN_REL;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "REV", 3) == 0)
			{
			fprintf(outfileptr, "REV");
			mode = IN_REV;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "TITLE", 5) == 0)
			{
			fprintf(outfileptr, "TITLE");
			mode = IN_TITLE;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "METHODS", 7) == 0)
			{
			fprintf(outfileptr, "METHODS");
			mode = IN_METHODS;
			ptr += 7;
			continue;
			}		

		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_HREF) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 0, host);
/*		fprintf(outfileptr, "\042");*/

		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_a */


int modify_img(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<IMG", 4) != 0) return(0);
ptr += 4;

fprintf(outfileptr, "<IMG");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_SRC) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "SRC", 3) == 0)
			{
			fprintf(outfileptr, "SRC");
			mode = IN_SRC;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALT", 3) == 0)
			{
			fprintf(outfileptr, "ALT");
			mode = IN_ALT;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALIGN", 5) == 0)
			{
			fprintf(outfileptr, "ALIGN");
			mode = IN_ALIGN;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "ISMAP", 5) == 0)
			{
			fprintf(outfileptr, "ISMAP");
			mode = IN_ISMAP;
			ptr += 5;
			continue;
			}		

		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_SRC) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 1, host);

		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_img */


int modify_script(char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;
int file_arg_flag;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<SCRIPT", 7) != 0) return(0);
ptr += 7;

fprintf(outfileptr, "<SCRIPT");

file_arg_flag = 0;
mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_LANGUAGE) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "LANGUAGE", 8) == 0)
			{
			fprintf(outfileptr, "LANGUAGE");
			mode = IN_LANGUAGE;
			ptr += 8;
			continue;
			}
		}

	if( (mode != IN_SRC) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "SRC", 3) == 0)
			{
			fprintf(outfileptr, "SRC");
			mode = IN_SRC;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALT", 3) == 0)
			{
			fprintf(outfileptr, "ALT");
			mode = IN_ALT;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALIGN", 5) == 0)
			{
			fprintf(outfileptr, "ALIGN");
			mode = IN_ALIGN;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "ISMAP", 5) == 0)
			{
			fprintf(outfileptr, "ISMAP");
			mode = IN_ISMAP;
			ptr += 5;
			continue;
			}		

		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if( (in_arg_flag) && (mode == IN_SRC) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			/*
			this starts arguments after filename format filename;arg;arg
			*/
			if (*ptr == ';')
				{
				file_arg_flag = 1;
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 1, host);

		*new_pos = ptr;

		/* if no arguments, done with this script ? */
		if(! file_arg_flag) return(1);
/*		return 1;*/
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_script */


int modify_iframe(\
char *position, char **new_pos, FILE *outfileptr, char *host)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;

/*
IFRAME	SRC WIDTH HEIGHT MARGINWIDTH MARGINHEIGHT HSPACE VSPACE \
			FRAMEBORDER SCROLLING BORDERCOLOR
*/

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);
if(! outfileptr) return(0);

ptr = position;

if(strncasecmp(ptr, "<IFRAME", 7) != 0) return(0);
ptr += 7;

fprintf(outfileptr, "<IFRAME");

mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == '>')
		{
		*new_pos = ptr;
		return(1);
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		fprintf(outfileptr, "%c", *ptr);
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_SRC) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "SRC", 3) == 0)
			{
			fprintf(outfileptr, "SRC");
			mode = IN_SRC;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "WIDTH", 5) == 0)
			{
			fprintf(outfileptr, "WIDTH");
			mode = IN_WIDTH;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "HEIGHT", 6) == 0)
			{
			fprintf(outfileptr, "HEIGHT");
			mode = IN_HEIGHT;
			ptr += 6;
			continue;
			}
		else if(strncasecmp(ptr, "MARGINWIDTH", 11) == 0)
			{
			fprintf(outfileptr, "MARGINWIDTH");
			mode = IN_MARGINWIDTH;
			ptr += 11;
			continue;
			}		
		else if(strncasecmp(ptr, "MARGINHEIGHT", 12) == 0)
			{
			fprintf(outfileptr, "MARGINHEIGHT");
			mode = IN_MARGINHEIGHT;
			ptr += 12;
			continue;
			}		
		else if(strncasecmp(ptr, "HSPACE", 6) == 0)
			{
			fprintf(outfileptr, "HSPACE");
			mode = IN_HSPACE;
			ptr += 6;
			continue;
			}		
		else if(strncasecmp(ptr, "VSPACE", 6) == 0)
			{
			fprintf(outfileptr, "VSPACE");
			mode = IN_VSPACE;
			ptr += 6;
			continue;
			}		
		else if(strncasecmp(ptr, "FRAMEBORDER", 11) == 0)
			{
			fprintf(outfileptr, "FRAMEBORDER");
			mode = IN_FRAMEBORDER;
			ptr += 11;
			continue;
			}		
		else if(strncasecmp(ptr, "SCROLLING", 9) == 0)
			{
			fprintf(outfileptr, "SCROLLING");
			mode = IN_SCROLLING;
			ptr += 9;
			continue;
			}		
		else if(strncasecmp(ptr, "BORDERCOLOR", 11) == 0)
			{
			fprintf(outfileptr, "BORDERCOLOR");
			mode = IN_BORDERCOLOR;
			ptr += 11;
			continue;
			}		

		fprintf(outfileptr, "%c", *ptr);
		ptr++;
		continue;
		}
	if( (in_arg_flag) && (mode == IN_SRC) )
		{
		for(i = 0; i < 1024; i++)
			{
			if( (*ptr == '"') || (*ptr == 0) || (isspace(*ptr) ) )
				{
				break;
				}
			temp[i] = *ptr;
			ptr++;
			}
		temp[i] = 0;

		http_modify_url(temp, outfileptr, 1, host);

		*new_pos = ptr;
		return(1);
		}

	fprintf(outfileptr, "%c", *ptr);
	ptr++;
	}/* end while */
}/* end function modify_iframe */


int write_modified_html_file(char *filename, int flags)
{
/*
Possibilities (as seen so far):
BODY		BACKGROUND TEXT BGCOLOR LINK YLINK ALINK
BASE		HREF
A			HREF NAME URN REL REV TITLE METHODS
LINK		HREF URN REL REV TITLE METHODS
IMG			SRC ALT ALIGN ISMAP
INPUT
ISINDEX
FORM

added after looking at www.europe.cnn.com
SCRIPT		LANGUAGE SRC  need to translate SRC to local file, and leave its
			args.
NOSCRIPT	ignore for now
IFRAME		SRC WIDTH HEIGHT MARGINWIDTH MARGINHEIGHT SPACE VSPACE FRAMEBORDER\
			SCROLLING BORDERCOLOR

SELECT		NAME ONCHANGE not implemented
OPTION		VALUE
*/	

int a, c;
struct stat *statptr;
char *space, *spaceptr;
FILE *fileptr;
char *position;
char *ptr1;
char *ptr2;
long size;
char *outfile;
FILE *outfileptr;
char temp[TEMP_SIZE];
char *host;
char *new_pos;

if(debug_flag)
	{
	fprintf(stdout,\
	"write_modified_html_file(): arg filename=%s\n", filename);
	}

/* argument check */
if(! filename) return(0);

fileptr = fopen(filename, "r");
if(! fileptr)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"write_modified_html_file(): cannot open file %s for read\n",\
		filename);
		}
	return(0);
	}/* end if open fails */
/*fprintf(stdout, "file open\n");*/

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(fileptr), statptr);
size = statptr -> st_size;
if(size == 0)
	{
	free(statptr);
	return(0);
	}

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"write_modified_html_file(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(fileptr);
		if(! ferror(fileptr) ) break;
		perror("write_modified_html_file(): read failed ");
		}/* end while error re read */	

	if(feof(fileptr) )
		{
		fclose(fileptr);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
/* may not be there */
*spaceptr = 0;/* string termination */

/*
replace any absolute references with relative ones.
http://somehost/somedir/somefile becomes:
file:///home_dir/.NewsFleX/http/somedir/somefile

HREF="/somedir/somefile becomes:
file:///home_dir/.NewsFleX/http/somedir/somefile

HREF="somefile is left as it is
HREF="/[somedir/]" becomes:
file://home_dir/.NewsFleX/http/[somedir/]index.html
*/

if(debug_flag)
	{
	fprintf(stdout,\
	"write_modified_html_file(): arg filename=%s\nspace=%s size=%ld\n",\
	filename, space, size);
	}

/* keep gcc -Wall happy */
ptr1 = ptr2 = 0;

/*
extract host from filename:
/homedir/.NewsFleX/http/somehost[/somedir]/somefile
Note us user http may exists
*/
/* do not want to mess up filename */
strcpy(temp, filename);
ptr1 = strstr(temp, "/.NewsFleX/http/");
if(! ptr1) return(0);
ptr1 += strlen("/.NewsFleX/http/");

/* ptr1 now points to somehost[/somedir]/somefile */
ptr2 = strstr(ptr1, "/");
if(! ptr2) return(0);

/* cut of anything after host */
*ptr2 = 0;
host = strsave(ptr1);
if(! host) return(0);

if(debug_flag)
	{
	fprintf(stdout, "write_modified_html_file(): host=%s\n", host);
	}

/*
also need local path in case entry like A HREF music.wav
Then need to prepend the local path, so if link is taken from that,
real URL can be easily calculated.
Normally this would not be needed, as we would be in this directory
with the browser, but link extraction looks for a leading home_dir/.NewsFleX
making this a global save stack processing time.
*/
free(local_path_f);
local_path_f = strsave(filename);
if(! local_path_f) return(0);

if(debug_flag)
	{
	fprintf(stdout, "write_modified_html_file(): local_path_f=%s\n",\
	local_path_f);
	}

/* safe to modify temp */
strcpy(temp, filename);
ptr1 = strrchr(temp, '/');
if(! ptr1) return(0);
*(ptr1 + 1) = 0;/* cut off filename */

free(local_path);
local_path = strsave(temp);
if(! local_path) return(0);

if(debug_flag)
	{
	fprintf(stdout, "write_modified_html_file(): local_path=%s\n",\
	local_path);
	}

outfile = malloc(strlen(filename) + strlen(".mod.tmp") + 1);
if(! outfile) return(0);
strcpy(outfile, filename);
strcat(outfile, ".mod.tmp");

outfileptr = fopen(outfile, "w");
if(! outfileptr)
	{
	sprintf(temp, "write_modified_html_file(): cannot open %s for write",\
	outfile);	
	if(debug_flag)
		{
		fprintf(stdout, "%s\n", temp);
		}
	to_error_log(temp);
	free(outfile);
	free(host);
	return(0);
	}

/* for all characters in string space */
position = space;
while(1)
	{
	if(! position) break;
	if(*position == 0) break;

	if(*position == '<')
		{
		if(1) /* always */
			{
			/* print any comments */
			a = modify_comment(position, &new_pos, outfileptr);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}
			
			/* get any base address */
			a = modify_base(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}
			
			/* get any anchors */
			a = modify_a(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}

			/* get any links */
			a = modify_link(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}

			/* not supporting scripts (i.e. javascript), scipping it */
			a = modify_script(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}
			
			/* not supporting noscripts (whatever it is), scipping it */
/*
			a = modify_noscript(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}
*/
			
			/* modify iframe contents */
			a = modify_iframe(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}
			
			}/* end if always */

		if(flags & PICTURES_URL)
			{
			/* get any images */
			a = modify_img(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}

			/* get any background pictures */
			a = modify_body(position, &new_pos, outfileptr, host);
			position = new_pos;
			if(a)
				{
				fprintf(outfileptr, "%c", *position);
				position++;
				continue;
				}

			}/* end if url_get_pictures_flag */
	
		}/* end if c == '<' */

	fprintf(outfileptr, "%c", *position);
	position++;
	}/* end while all characters in space */
fclose(outfileptr);

/* remove original file */
unlink(filename);

/* move this file to the original */
rename(outfile, filename);

free(outfile);
free(host);

return(1);
}/* end function write_modified_html_file */


