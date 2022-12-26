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


struct helpers
	{
	char *name;
	char *def;
	struct helpers *nxtentr;
	};
static struct helpers *helperstab[1];


struct helpers *lookup_helpers(char *name, int sequence)
{
struct helpers *pa;
int j;

j = 0;
/*pa points to next entry*/
for(pa = helperstab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0)
		{
		if(sequence == j) return(pa);/* found sequence entry */
		j++;
		}
	}
return(0);/*not found*/
}


struct helpers *install_helpers(char *name, char *def)
{
struct helpers *pa, *lookup_helpers();

/* Note: more then one entry with the same nam may exist */
	
pa = (struct helpers *) malloc( sizeof(*pa) );
if(!pa) return(0);
pa -> name = strsave(name);
if(! pa -> name) return(0);
pa -> def = strsave(def);
if(!pa -> def) return(0);
pa -> nxtentr = helperstab[0];
helperstab[0] = pa;
return(pa);
}


struct helpers *reverse_lookup_helpers(char *name, int sequence)
{
struct helpers *pa, *plastentry, *allentries[256];
int i, j;

/* want the first entry in the helpersfile to be the first to be tried */
/* there may be smarter ways, but I have tried ! */
/* load all structure addresses in array */
i = 0;
for(pa = helperstab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(i >= 256)
		{
		if(debug_flag)fprintf(stdout,\
		"\nreverse_lookup_helpers: To many entries in helpers\n");
		return(0);
		}
	allentries[i] = pa;
	if(pa) plastentry = pa;
	i++;
	}
/* read the array from the end */
j = 0;
for(i = i - 1; i >= 0; i--)
	{
	pa = allentries[i];
	if(strcmp(pa -> name, name) == 0)
		{
		if(sequence == j) return(pa);
		j++;
		}
	}
return(0);/*not found*/
}


int set_helpers(char *name, char *def)
{
struct helpers *install_helpers();

if(! install_helpers(name, def) ) return(0);
return(1);
}


int get_helpers(char *name, char *def, int sequence)
{
struct helpers *pa, *lookup_helpers(), *install_helpers();

pa = reverse_lookup_helpers(name, sequence);
if(! pa) return(0);
strcpy(def, pa -> def);
return(1);
}


int delete_helpers()
{
struct helpers *pa;

if(debug_flag)
	{
	fprintf(stdout, "delete helpers(): arg none\n");
	}

while(1)
	{
	pa = helperstab[0];
	if(! pa) break;
	helperstab[0] = pa -> nxtentr; /* helperstab entry points to next one,
											this could be 0 */
	free(pa -> name);
	free(pa -> def);
	free(pa);/* free structure */
	}/* end while all structures */ 

return(1);
}/* end function delete_helpers */


int show_helpers()
{
int c;
char temp[TEMP_SIZE];
FILE *load_filefd;
struct stat *statptr;
char *space, *spaceptr;

sprintf(temp, "%s/.NewsFleX/setup/helpers", home_dir);
load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	fl_show_alert("could not load file", temp, "", 0);
	return(0);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(load_filefd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_helpers(): malloc could not allocate space\n");
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
		perror("show_helpers(): read failed ");
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

fl_set_input(fdui -> helpers_form_input, space);
free(space);

return(1);
}/* end function show_helpers */


int save_helpers()
{
int a;
char helpers[TEMP_SIZE];	
char helpers_bak[TEMP_SIZE];
FILE *save_filefd;
extern char *strsave();
char *text;

if(debug_flag) fprintf(stdout, "\nsave_helpers(): arg none\n");

/* make a path_filename */
sprintf(helpers, "%s/.NewsFleX/setup/helpers", home_dir);

/* make a backup filename */
sprintf(helpers_bak, "%s.bak", helpers);

/* delete any previous backup filename */
unlink(helpers_bak);

/* rename the present helpers to helpers.bak */
a = rename(helpers, helpers_bak);
if(a == -1)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"\nCould not rename %s into %s\n", helpers, helpers_bak);
		}	
	return(0);
	}
save_filefd = fopen(helpers, "w");
if(! save_filefd)
	{
	fl_show_alert("could not open file for write ", helpers, "", 0);
	return(0);
	}
text = (char *) fl_get_input(fdui -> helpers_form_input);	

fprintf(save_filefd, "%s", text);

/* if the user did not provide the terminating LF */
if(*(text + strlen(text) - 1) != '\n')
	{
	fprintf(save_filefd, "\n");
	}

fclose(save_filefd);	
return(1);
}/* end function save_helpers */


int load_helpers()
{
int a;
char pathfilename[TEMP_SIZE];
char temp[READSIZE];
char arg0[TEMP_SIZE], arg1[TEMP_SIZE], arg2[TEMP_SIZE];
extern char *strsave();
char *cp;
FILE *helpersfile;

if(debug_flag) fprintf(stdout, "load_helpers(): arg none\n");

/* clear the structure first */
delete_helpers();
/* test if a helpersfile helpers is present */
/* combine with home directory */
sprintf(pathfilename, "%s/.NewsFleX/setup/helpers", home_dir);
helpersfile = fopen(pathfilename, "r");
if(! helpersfile)/* no helpers file present */
	{
	if(debug_flag)
		{
		fl_show_alert("Cannot open file for read:", pathfilename, "", 0);
		fprintf(stdout, "\nCannot open helpers file %s\n", pathfilename);
		}
	return(0);
	}
else /* a helpers file present */
	{
	/* read data from helpers helpersfile into database */
	while(1)
		{
		/* read a line from the helpers file */
		a = readline(helpersfile, temp);
		if(a == EOF)break;

		/* lines starting with '#' are scipped */
		if(temp[0] == '#') continue; 

		/* scip comments to end of line */
		cp = strstr(temp, "//");

		/* force a string termination on "//" */
		if(cp) *cp = 0;
		
		/* get the start of the second field */
		a = sscanf(temp, "%s %s %s", arg0, arg1, arg2);

		/* scip empty lines */
		if (a == -1) continue;

		if(debug_flag)
			{
			fprintf(stdout,\
			"\nInStAl:a=%d temp=%s arg0=%s arg1=%s",\
			a, temp, arg0, arg1);
			}

		/*
		test if any second field was present,
		need at least 3 arguments for a valid entry.
		*/
		if( (a < 3) || (! set_helpers(arg0, strstr(temp, arg1) ) ) )
			{
			fl_show_alert("Error in helpers file in line:", temp, "", 0);
			if(debug_flag)
				{
				fprintf(stdout,\
"\nCannot install data from helpersfile in database, line reads %s\n", temp);
				}
			if(helpersfile) fclose(helpersfile);
			return(0);
			}
		}/* end while read all lines from helpersfile */

	}/* end if helpers file present */

return(1);
}/* end function load_helpers */


int show_helpers_form()
{
if(debug_flag)
	{
	fprintf(stdout, "show_helpers_form(): arg none\n");
	}

fl_show_form(fd_NewsFleX -> helpers_form,  FL_PLACE_CENTER, FL_NOBORDER, "");

show_helpers();

return(1);
}/* end function show_helpers_form */

