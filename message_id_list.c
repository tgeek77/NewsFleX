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

/* 
 The cross post filter list is used prevent the same article to appear
 in more then one group.
 This is done by storing the message_id of all incoming headers,
 and comparing it against the ones already received in this session.
 */


#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

#define MESSAGE_ID_HASH_SIZE 100

struct message_id
	{
	char *name;/* this is the message_id of the header */
	struct message_id *nxtentr;
	}; 
struct message_id *message_idtab[MESSAGE_ID_HASH_SIZE];


struct message_id *lookup_message_id(char *name)
{
struct message_id *pa;

if(debug_flag)
	{
	fprintf(stdout, "lookup_message_id(): arg name=%s\n", name);
	}
	
for(pa = message_idtab[hash(name)]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);/* found sequence entry */
	}
return(0); /* not found */
}/* end function lookup_message_id */


struct message_id *install_message_id(char *name)
{
struct message_id *pnew, *pnext, *lookup_message_id();
int hashval;

if(debug_flag)
	{
	fprintf(stdout, "install_message_id(): arg name=%s\n", name);
	}
	
/* create new structure */
pnew = (struct message_id *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get next structure */
hashval = hash(name);
pnext = message_idtab[hashval];/* may be zero, if there was nothing */

/* insert before next structure (if any, else at start) */
message_idtab[hashval] = pnew;

/* set pointers for new structure */
pnew -> nxtentr = pnext;

return(pnew);/* pointer to new structure */
}/* end function install_message_id */


int delete_all_message_ids()
{
struct message_id *pa;
int i;

if(debug_flag)
	{
	fprintf(stdout, "delete_all_message_ids(): arg none\n");
	}

for(i = 0; i < MESSAGE_ID_HASH_SIZE; i++)/* for all structures at this position */
	{
	while(1)
		{
		pa = message_idtab[i];
		if(! pa) break;
		message_idtab[i] = pa -> nxtentr;/* message_idtab entry points to next one,
															this could be 0
															*/
		free(pa -> name);/* free name */
		free(pa);/* free structure */
		}/* end while all structures hashing to this value */ 
	}/* end for all entries in message_idtab */
return(0);/* not found */
}/* end function delete_all_message_ids */


int load_message_ids()
{
int a;
FILE *message_id_fptr;
char temp[TEMP_SIZE];/* lines in message_ids dat are of know size shorter then this */
char pathfilename[TEMP_SIZE];
char name[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "load_message_ids(): arg none\n");
	}

delete_all_message_ids();

return(1);

to_command_status_display("Loading message_ids file, this may take a while");

sprintf(pathfilename, "%s/.NewsFleX/message_ids.dat", home_dir);
message_id_fptr = fopen(pathfilename, "r");
if(! message_id_fptr)
	{
	fl_show_alert("Could not open file ", pathfilename, "for read", 0);
	return(0);
	} 

while(1)
	{
	a = readline(message_id_fptr, temp);/* closes file if EOF */
	if(a == EOF)
		{
/*		fclose(message_id_fptr);*/
		to_command_status_display("");		
		return(1);
		}

	if(! install_message_id(name) )
		{
		fl_show_alert("load_message_ids():", "cannot install", name, 0);
		fclose(message_id_fptr);
		return(0);
		}
	}/* end while all lines in file */
}/* end function load_message_id */


int save_message_ids()
{
int i;
FILE *message_id_fptr;
char pathfilename[TEMP_SIZE];
struct message_id *pa;
extern char *strsave();

if(debug_flag) 
	{
	fprintf(stdout, "save_message_ids(): arg none\n");
	}

sprintf(pathfilename, "%s/.NewsFleX/message_ids.dat", home_dir);
message_id_fptr = fopen(pathfilename, "w");
if(! message_id_fptr)
	{
	fl_show_alert("Cannot open file", pathfilename, "for write", 0);
	return(0);
	} 

for(i = 0; i < MESSAGE_ID_HASH_SIZE; i++)/* for all structures at this position */
	{
	for(pa = message_idtab[i]; pa != 0; pa = pa -> nxtentr)
		{
		fprintf(message_id_fptr, "%s\n", pa -> name);
		}
	}/* end for all entries in message_idtab */

fclose(message_id_fptr);

return(1);
}/* end function save_message_ids */


int add_to_message_id_list(char *message_id, int *present)
{
struct message_id *pa, *install_message_id();

if(debug_flag)
	{
	fprintf(stdout,\
	"add_to_message_id_list(): arg message_id=%s\n", message_id);
	}

/* argument check */
if(! message_id) return(0);

*present = 0;

/* test if already in list */
pa = lookup_message_id(message_id);
if(pa)
	{
	*present = 1;
	return(1);/* already there */
	}

pa = install_message_id(message_id);
if(! pa) return(0);

return(1);
}/* end function add_to_message_id_list */


