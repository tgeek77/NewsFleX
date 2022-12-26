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

struct html_table_entry
	{
	char *name;/* this is the text */
	int size;
	int row;
	int column;
	struct html_table_entry *nxtentr;
	struct html_table_entry *prventr;
	};
struct html_table_entry *html_table_entrytab[2]; /* first element points to first entry,
											second element to last entry */

int maxsizes[TABLE_MAX_COLUMNS];
int maxcolumns;

struct html_table_entry *install_html_table_entry_at_end_of_list(char *name)
{
struct html_table_entry *plast, *pnew;

if(debug_flag)
	{
	fprintf(stdout,\
	"install_html_table_entry_at_end_off_list(): arg name=%s\n", name);
	}

if(! name) return(0);


/* multiple enties with the same name are possible */

/* create new structure */
pnew = (struct html_table_entry *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = html_table_entrytab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previuos structure pointers */
if( !html_table_entrytab[0] ) html_table_entrytab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
html_table_entrytab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_html_table_entry */


int delete_all_html_table_entries()/* delete all entries from table */
{
struct html_table_entry *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_html_table_entrys() arg none\n");
	}

while(1)
	{	
	pa = html_table_entrytab[0];
	if(! pa) break;
	html_table_entrytab[0] = pa -> nxtentr;
	free(pa -> name);
	free(pa);/* free structure */
	}/* end while all structures */

html_table_entrytab[1] = 0;

for(i = 0; i < TABLE_MAX_COLUMNS - 1; i++)
	{
	maxsizes[i] = 0;
	}
maxcolumns = 0;

return(1);
}/* end function delete_all_html_table_entries */


int add_html_table_entry(char *text, int row, int column, int type)
{
struct html_table_entry *pa;

if(debug_flag)
	{
	fprintf(stdout,\
	"add_html_table_entry(): arg text=%s row=%d column=%d type=%d\n",\
	text, row, column, type);
	}

if(! text) return(0);
if(column > TABLE_MAX_COLUMNS - 1) return(0);

pa = install_html_table_entry_at_end_of_list(text);
if(! pa) return(0);

pa -> row = row;
pa -> column = column;

pa -> size = strlen(text);

if(pa -> size > maxsizes[column]) maxsizes[column] = pa -> size;
if(type == TABLE_HEADER)
	{
	if(pa -> column > maxcolumns) maxcolumns = pa -> column;
	}

return(1);
}/* end function add_html_table_entry */


char *print_formatted_html_table(int center)
{
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
struct html_table_entry *pa;
char *ptr;
int c;
int i, j, k;
int line_length;
int max_line_length;
int leading_spaces;

line_length = 0;
max_line_length = 0;
strcpy(temp, "\n");
for(pa = html_table_entrytab[0]; pa != 0; pa = pa -> nxtentr)
	{
	/* if first column prepend LF */
	if(pa -> column == 0)
		{
		strcat(temp, "\n");
		line_length = 0;
		}

	/* the text */
	strcat(temp, pa -> name);
	line_length += strlen(pa -> name);

	/* if not last column, then append spaces */
	if(pa -> column < maxcolumns)
		{
		for(i = 0; i < (maxsizes[pa -> column] - pa -> size); i++)
			{
			strcat(temp, " ");
			line_length++;
			}			
		}
	/* closing space */
	strcat(temp, " ");
	line_length++;
	
	if(line_length > max_line_length) max_line_length = line_length;
	}/* end for all structure entries */


/*fprintf(stdout, "MAX_LINE_LENGTH=%d\n", max_line_length);*/
if(! center)
	{
	/* closing LF */
	strcat(temp, "\n");

	ptr = strsave(temp);
	if(! ptr) return(0);

	return(ptr);
	}

/* prepend fixed number of spaces after each LF */
leading_spaces = (HOR_CHARS - max_line_length) / 2;
i = 0;
j = 0;
while(1)
	{
	c = temp[i];
	temp2[j] = c;
	if(c == 0) break;

	if(c == 10)
		{
		for(k = 0; k < leading_spaces; k++)
			{
			j++;
			temp2[j] = ' ';
			}
		}/* end for k */	
	i++;
	j++;
	}/* end while */

/* closing LF */
strcat(temp2, "\n");

ptr = strsave(temp2);
return(ptr);
}/* end function print_formatted_html_table */


