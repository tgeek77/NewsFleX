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
Note:
I have written this without having the format specification of
/var/spool/mail/user
*/


#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

#define IN_START	0
#define IN_HEADER	1
#define IN_BODY		2

struct mail
	{
	char *name;/* this is the mail id long decimal */
	char *_from;
	char *return_path;
	char *received;
	char *date;
	char *from;
	char *message_id;
	char *to;
	char *subject;
	char *status;
	char *x_status;
	char *cc;
	char *reply_to;
	char *mime_version;
	char *content_type;
	char *summary;
	char *text;
	long lines;
	int browser_line;
	struct mail *nxtentr;
	struct mail *prventr;
	}; 
struct mail *mailtab[2]; /* first element points to first entry,
									second element to last entry */


struct mail *lookup_mail(char *name)
{
struct mail *pa;

for(pa = mailtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> name, name) == 0) return(pa);
	}

return(0);/*not found*/
}/* end function lookup_mail */


struct mail *install_mail_at_end_of_list(char *name)
{
struct mail *plast, *pnew;
struct mail *lookup_mail();

if(debug_flag)
	{
	fprintf(stdout,\
	"install_mail_at_end_of_list(): arg name=%s\n", name);
	}

/* argument check */
if(! name) return(0);

pnew = lookup_mail(name);
if(pnew)
	{
	/* free previous definition */
	free(pnew -> _from);
	free(pnew -> return_path);
	free(pnew -> received);
	free(pnew -> date);
	free(pnew -> from);
	free(pnew -> message_id);
	free(pnew -> to);
	free(pnew -> subject);
	free(pnew -> status);
	free(pnew -> x_status);
	free(pnew -> cc);
	free(pnew -> reply_to);
	free(pnew -> mime_version);
	free(pnew -> content_type);
	free(pnew -> summary);
	free(pnew -> text);
	return(pnew);/* already there */
	}

/* create new structure */
pnew = (struct mail *) calloc(1, sizeof(*pnew) );
if(! pnew) return(0);
pnew -> name = strsave(name);
if(! pnew -> name) return(0);

/* get previous structure */
plast = mailtab[1]; /* end list */

/* set new structure pointers */
pnew -> nxtentr = 0; /* new points top zero (is end) */
pnew -> prventr = plast; /* point to previous entry, or 0 if first entry */

/* set previous structure pointers */
if( !mailtab[0] ) mailtab[0] = pnew; /* first element in list */
else plast -> nxtentr = pnew;

/* set array end pointer */
mailtab[1] = pnew;

return(pnew);/* pointer to new structure */
}/* end function install_mail_at_end_of_list */


int delete_all_mails()/* delete all entries from table */
{
struct mail *pa;

if(debug_flag) fprintf(stdout, "delete_all_mails() arg none\n");

while(1)
	{	
	pa = mailtab[0];
	if(! pa) break;
	mailtab[0] = pa -> nxtentr;
	free(pa -> _from);
	free(pa -> return_path);
	free(pa -> received);
	free(pa -> date);
	free(pa -> from);
	free(pa -> message_id);
	free(pa -> to);
	free(pa -> subject);
	free(pa -> status);
	free(pa -> x_status);
	free(pa -> cc);
	free(pa -> reply_to);
	free(pa -> mime_version);
	free(pa -> content_type);
	free(pa -> summary);
	free(pa -> text);
	free(pa);/* free structure */
	}/* end while all structures */

mailtab[1] = 0;
if(debug_flag) fprintf(stdout, "returning from delete_all_mails\n");
return(1);
}/* end function delete_all_mail */


int load_incoming_mail(int *messages)
{
/* load from /var/spool/mail/user_name int structure mail*/
struct mail *pa;
int a;
char temp[TEMP_SIZE];
char temp2[TEMP_SIZE];
FILE *user_mail_fptr;
char *ptr;
char *_from;
char *return_path;
char *received;
char *date;
char *from;
char *message_id;
char *to;
char *subject;
char *status;
char *x_status;
char *cc;
char *reply_to;
char *mime_version;
char *content_type;
char *summary;
char *text;
long lines;
long entry;
int first_line_flag;
int where_flag;

if(debug_flag)
	{
	fprintf(stdout,\
	"load_incoming_mail(): arg none, using user_name=%s\n",\
	user_name);
	}

(*messages) = 0;

/* load the structure from the (most recent) /var/spool/mail */
sprintf(temp, "/var/spool/mail/%s", user_name);
if(debug_flag) fprintf(stdout, "opening %s\n", temp);
user_mail_fptr = fopen(temp, "r");
if(! user_mail_fptr)
	{
	/* no mail, no file, no error report */
/*	fl_show_alert("Cannot open file", temp, "for read", 0);*/
/*
	sprintf(temp, "No mail for %s in /var/spool/mail/", user_name);
	fl_set_object_label(fdui -> command_status_display, temp);
*/
	return(1);
	}
/*
else
	{
	sprintf(temp, "You have mail");
	fl_set_object_label(fdui -> command_status_display, temp);
	}
*/

/* clear structure */
delete_all_mails();

/* prevent null pointers */
_from = strsave("");
return_path = strsave("");
received = strsave("");
date = strsave("");
from = strsave("");
message_id = strsave("");
to = strsave("");
subject = strsave("");
status = strsave("");
x_status = strsave("");
cc = strsave("");
reply_to = strsave("");
mime_version = strsave("");
content_type = strsave("");
summary = strsave("");
text = strsave("");
lines = 0;
entry = 0;
first_line_flag = 1;
where_flag = IN_HEADER;
/*ptr = 0;*/ /* keep gcc -Wall happy */

/* read from /var/spool/mail/user_name */
while(1)
	{
	a = readline(user_mail_fptr, temp);/* closes file if EOF */
	if(a == EOF)
		{
		if(where_flag == IN_HEADER) break;/* some format error */
		if(first_line_flag) break;/* no entries in file */

		sprintf(temp2, "%ld", entry);
		pa = (struct mail *) install_mail_at_end_of_list(temp2);
		if(! pa) return(0);

		pa -> _from = strsave(_from);
		pa -> return_path = strsave(return_path);
		pa -> received = strsave(received);
		pa -> date = strsave(date);
		pa -> from = strsave(from);
		pa -> message_id = strsave(message_id);
		pa -> to = strsave(to);
		pa -> subject = strsave(subject);
		pa -> status = strsave(status);
		pa -> x_status = strsave(x_status);
		pa -> cc = strsave(cc);
		pa -> reply_to = strsave(reply_to);
		pa -> mime_version = strsave(mime_version);
		pa -> content_type = strsave(content_type);
		pa -> summary = strsave(summary);
		pa -> text = strsave(text);
		pa -> lines = lines - 1;
		pa -> browser_line = 0;
		
		(*messages)++;

		break;
		}/* end if a == EOF */

if(debug_flag) fprintf(stdout, "%s\n", temp);	

/*
From root@panteltje.pi.net Fri Feb  6 13:50:25 1998
Return-Path: root
Received: (from root@localhost) by panteltje.pi.net (8.6.12/8.6.9) id NAA076
Date: Fri, 6 Feb 1998 13:50:24 +0100
From: root <root@panteltje.pi.net>
Message-Id: <199802061250.NAA07604@panteltje.pi.net>
To: root@panteltje.pi.net
Subject: flap
Status: RO
X-Status:

text
text
*/

	if(where_flag == IN_BODY)
		{
		/*
		It seems that 'From ' in the body is prepended by '>',
		so it is guaranteed never to occur.
		So if it occusrs, it is the start of the next header.
		*/
		ptr = strstr(temp, "From ");
		if(ptr == temp)
			{
			where_flag = IN_HEADER;
			/* store previous header and body in structure */
			sprintf(temp2, "%ld", entry);
			pa = (struct mail *) install_mail_at_end_of_list(temp2);
			if(! pa) return(0);

			pa -> _from = strsave(_from);
			pa -> return_path = strsave(return_path);
			pa -> received = strsave(received);
			pa -> date = strsave(date);
			pa -> from = strsave(from);
			pa -> message_id = strsave(message_id);
			pa -> to = strsave(to);
			pa -> subject = strsave(subject);
			pa -> status = strsave(status);
			pa -> x_status = strsave(x_status);
			pa -> cc = strsave(cc);
			pa -> reply_to = strsave(reply_to);
			pa -> mime_version = strsave(mime_version);
			pa -> content_type = strsave(content_type);
			pa -> summary = strsave(summary);
			pa -> text = strsave(text);
			pa -> lines = lines - 1;
			pa -> browser_line = 0;

			free(_from);
			_from = strsave("");
			free(return_path);
			return_path = strsave("");
			free(received);
			received = strsave("");
			free(date);
			date = strsave("");
			free(from);
			from = strsave("");
			free(message_id);
			message_id = strsave("");
			free(to);
			to = strsave("");
			free(subject);
			subject = strsave("");
			free(status);
			status = strsave("");
			free(x_status);
			x_status = strsave("");
			free(cc);
			cc = strsave("");
			free(reply_to);
			reply_to = strsave("");
			free(mime_version);
			mime_version = strsave("");
			free(content_type);
			content_type = strsave("");
			free(summary);
			summary = strsave("");
			free(text);
			text = strsave("");
			
			(*messages)++;

			entry++;
			}/* end if new header detected, end of body */
		else /* body text */
			{
			/* make space */
			ptr = malloc(strlen(text) + strlen(temp) + strlen("\n") + 1);
			sprintf(ptr, "%s%s\n", text, temp);
			free(text);
			text = ptr;
			lines++;
			}/* end if body text */
		}/* end if whereflag == IN_BODY */

	if(where_flag == IN_HEADER)
		{
		/* End header detection, empty line in header. */
		if(strlen(temp) == 0)
			{
			lines = 0;
			where_flag = IN_BODY;
			/* clear any previous text */
			free(text);
			text = strsave("");
			}/* end if 'From ' */
		else /* header */
			{
			ptr = strstr(temp, "From ");
			if(ptr == temp)
				{
				free(_from);
				_from = strsave(ptr + 5);
				}
			ptr = strstr(temp, "From: ");
			if(ptr == temp)
				{
				free(from);
				from = strsave(ptr + 6);
				}
			ptr = strstr(temp, "To: ");
			if(ptr == temp)
				{
				free(to);
				to = strsave(ptr + 4);
				}
			ptr = strstr(temp, "Subject: ");
			if(ptr == temp)
				{
				free(subject);
				subject = strsave(ptr + 9);
				}
			ptr = strstr(temp, "Status: ");
			if(ptr == temp)
				{
				free(status);
				status = strsave(ptr + 8);
				}
			ptr = strstr(temp, "Reply-To: ");
			if(ptr == temp)
				{
				free(reply_to);
				reply_to = strsave(ptr + 10);
				}
			ptr = strstr(temp, "Summary: ");
			if(ptr == temp)
				{
				free(summary);
				summary = strsave(ptr + 9);
				}
			}/* end if not end header */
		}/* end if where_flag == IN_HEADER */

	first_line_flag = 0;
	}/* end while all lines in /var/spool/mail/user_name */ 

return(1);
}/* end function load_incoming_mail  */


int show_incoming_mail_form()
{
if(debug_flag)
	{
	fprintf(stdout, "show_incoming_mail_form(): arg none\n");
	}

fl_freeze_form(fdui -> incoming_mail_form);
show_incoming_mail();
fl_unfreeze_form(fdui -> incoming_mail_form);

fl_show_form(\
fd_NewsFleX -> incoming_mail_form, FL_PLACE_CENTER, FL_NOBORDER, "");

if(debug_flag) fprintf(stdout, "returning from show_incoming_mail_form\n");
return(1);
}/* end function show_incoming_mail_form */


int show_incoming_mail()
{
struct mail *pa;
char temp[TEMP_SIZE];
char formatstr[20];
int browser_line;
int messages;
int have_mail;

if(debug_flag)
	{
	fprintf(stdout, "show_incoming_mail():arg none\n");
	}

/* show the structure in the browser */

/* clear the browser */
fl_clear_browser(fdui -> incoming_mail_form_browser);

messages = 0;
have_mail = load_incoming_mail(&messages);
update_mail_display(messages);
if(! have_mail) return(0);
if(messages == 0) return(1);

strcpy(formatstr, "@f");
browser_line = 1;
for(pa = mailtab[0]; pa != 0; pa = pa -> nxtentr)
	{
	if(strcmp(pa -> status, "RD") == 0) strcat(formatstr, "@C0");/* black */
	if(strcmp(pa -> status, "O") == 0) strcat(formatstr, "@C4");/* blue */
	
	sprintf(temp, "%s%s  %s", formatstr, pa -> from, pa -> subject);
	fl_add_browser_line(fdui -> incoming_mail_form_browser, temp);
	pa -> browser_line = browser_line;
	browser_line++;
	}

return(1);
}/* end function show_incoming_mail */


int line_to_mail_id(int line, long *mail_id)
{
struct mail *pa;

if(debug_flag)
	{
	fprintf(stdout, "line_to_mail_id(): arg line=%d\n", line);
	}

/* argument check */
if(line < 0) return(0);

for(pa = mailtab[0]; pa != 0; pa = pa -> nxtentr)	
	{
	if(pa -> browser_line == line)
		{
		*mail_id = atol(pa -> name);
		return(1);
		}
	}

return(0);
}/* end function line_to_mail_id */


int reply_to_incoming_mail(long mail_id)
{
struct mail *pa;
struct mail *lookup_mail();
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "reply_to_incoming_mail(): arg mail_id=%ld\n", mail_id);
	}

/* argument check */
if(mail_id < 0) return(0);

sprintf(temp, "%ld", mail_id);
pa = lookup_mail(temp);
if(! pa) return(0);

if(debug_flag)
	{
	fprintf(stdout, "From=%s\n", pa -> from);
	fprintf(stdout, "To=%s\n", pa -> to);	
	fprintf(stdout, "Subject=%s\n", pa -> subject);
	fprintf(stdout, "Reply-To=%s\n", pa -> reply_to);
	fprintf(stdout, "Status=%s\n", pa -> status);
	fprintf(stdout, "lines=%ld\n", pa -> lines);
	fprintf(stdout, "text=\n%s", pa -> text);
	fprintf(stdout, "summary=%s\n", pa -> summary);
	}

posting_source = POSTING_MENU;
posting_menu_selection = REPLY_VIA_EMAIL;

/* fill in the input fields for the posting editor */
if(! mail_fill_in_input_fields(mail_id) ) return(0);

/* hide the incoming mail form */
/*fl_hide_form(fd_NewsFleX -> incoming_mail_form);*/

/* show the posting editor */
fl_show_form(fd_NewsFleX -> posting_editor,\
FL_PLACE_CENTER,  FL_UP_FRAME, "");

return(1);
}/* end function reply_to_incoming_mail */


int mail_fill_in_input_fields(long mail_id)
{
struct mail *pa, *lookup_mail();
extern char *strsave();
char temp[TEMP_SIZE];
char *ptr, *sptr, *dptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"mail_fill_in_input_fields(): arg mail_id=%ld\n", mail_id);
	}

/* argument chaeck */
if(mail_id < 0) return(0);

sprintf(temp, "%ld", mail_id);
pa = lookup_mail(temp);
if(! pa) return(0);

 /* set for no attachment */
fl_set_input(fdui -> posting_attachment_input_field, "");

/* set the to input field label */
fl_set_object_label(fdui -> posting_to_input_field, "Email");

/* 
set the to input field to the email of the sender.
First test for a Repy-To:, if not present use From:
*/
if(strlen(pa -> reply_to) == 0)
	{
	fl_set_input(fdui -> posting_to_input_field, pa -> from);
	}
else fl_set_input(fdui -> posting_to_input_field, pa -> reply_to);


/* prepend Re: to the subject if not already there */
if(strncmp(pa -> subject, "Re:", 3) == 0)
	{
	/* already there */
	fl_set_input(fdui -> posting_subject_input_field, pa -> subject);
	}
else
	{
	ptr = malloc(strlen("Re: ") + strlen(pa -> subject) + 1);
	strcpy(ptr, "Re: ");
	strcat(ptr, pa -> subject);
	
	fl_set_input(fdui -> posting_subject_input_field, ptr);
	free(ptr);
	}

/* get the summary */
if(strlen(pa -> summary) == 0)
	{
	fl_set_input(fdui -> summary_editor_input_field, "");
	fl_set_object_color(fdui -> posting_summary_button,\
	FL_COL1, FL_COL1);
	}
else
	{
	fl_set_input(fdui -> summary_editor_input_field, pa -> summary);
	fl_set_object_color(fdui -> posting_summary_button,\
	FL_DARKORANGE,  FL_DARKORANGE);
	}

/* include the original text idented with '>' */
ptr = malloc(strlen(pa -> text) * 2 + 1);/* 2 x in case all LF */
sptr = pa -> text;/* source */
dptr = ptr;/* dest */
/* start with ident */
*dptr = '>';
dptr++;
while(1)
	{
	*dptr = *sptr;
	if(*sptr == 0) break;
	if(*sptr == '\n')
		{
		dptr++;
		*dptr = '>';
		}
	sptr++;
	dptr++;
	}
fl_set_input(fdui -> posting_body_editor, ptr);

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

/*
This inhibits the question 'Reply to newsgroups?'
when send_now or send_later is pressed in the posting editor.
*/
reply_to_incoming_mail_flag = 1;
return(1);
}/* end function mail_fill_in_input_fields */


int update_mail_display(int messages)
{
char temp[80];

if(debug_flag)
	{
	fprintf(stdout, "update_mail_display(): arg messages=%d\n", messages);
	}

if(messages)
	{
	sprintf(temp, "%d", messages);
	fl_set_object_label(fdui -> mail_display, temp); 
	fl_set_object_color(fdui -> mail_display, FL_YELLOW, FL_COL1);
/*
	sprintf(temp, "You have mail");
	fl_set_object_label(fdui -> command_status_display, temp);
*/
	}
else
	{
	fl_set_object_label(fdui -> mail_display, ""); 
	fl_set_object_color(fdui -> mail_display, FL_WHITE, FL_COL1);
/*
	sprintf(temp, "No mail for %s", user_name);
	fl_set_object_label(fdui -> command_status_display, temp);
*/
	}

return(1);
}/* end function update_mail_display */


int check_mail_local()
{
int messages;
int a;
struct stat *statptr;
static time_t last_modification_time;
char temp[TEMP_SIZE];
static int old_messages;

if(debug_flag)
	{
	fprintf(stdout, "check_mail_local(): arg none\n");
	}

/*
Only want to access the mail file (and read it) if it has been modified,
else, if it is very long, the system will slow down.
*/
/* get stat /var/spool/mail */
sprintf(temp, "/var/spool/mail/%s", user_name);
if(debug_flag)
	{
	fprintf(stdout, "opening %s\n", temp);
	}

statptr = (struct stat*) malloc(sizeof(struct stat) );
a = stat(temp, statptr);
if(a == 0)/* file exists */
	{
	if(statptr -> st_mtime == last_modification_time)
		{
		free(statptr);
		if(debug_flag)
			{
			fprintf(stdout,\
			"check_local_mail(): %s was not modified returning\n", temp);
			}
		return(0);
		}
	last_modification_time = statptr -> st_mtime;
	
	if(debug_flag)
		{
		fprintf(stdout, "check_local_mail(): %s was modified\n", temp);
		}
	}/* end if mail file exists */
free(statptr);

messages = 0;
a = load_incoming_mail(&messages);
update_mail_display(messages);
	
if(global_beep_on_mail_flag)
	{
	/*
	only beep if more messages, not if some other mail program deletes some.
	*/

	if(messages > old_messages)
		{
		/*
		Note: both beeps will sound the same if started from an rxvt.
		If started from the window manager the ANSI sequences seem to work.
		*/

		printf("\33[10;%d]", 2000); /* Hz */
		printf("\33[11;%d]", 200); /* mS */
		putchar('\a');

		/* use this if ANSI beep does not work */
//		fl_ringbell(100); /* volume in % */
		
		fflush(stdout);

		/* wait .5 seconds */
		usleep(500000);

		printf("\33[10;%d]", 1800); /* Hz */
		printf("\33[11;%d]", 300); /* mS */
		putchar('\a');

		/* use this if ANSI beep does not work */
//		fl_ringbell(100); /* volume in % */
		
		fflush(stdout);

		}/* end if more messages */
	}/* end if global_beep_on_mail_flag */

/* remember how many messages */
old_messages = messages;

if(! a) return(0);

/* prevent flashing display */
fl_freeze_form(fdui -> incoming_mail_form);
/*
refresh the display structure (incoming mail form may not be visible)
*/
show_incoming_mail();
fl_unfreeze_form(fdui -> incoming_mail_form);

return(1);
}/* end function check_mail_local */


