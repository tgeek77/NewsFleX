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


int delete_decoded_article(char *group, long article_id)
{
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout, "delete_decoded_article(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
home_dir, news_server_name, database_name, group, article_id);
unlink(temp);

return(1);
}/* end function delete_decoded_article */


int pgp_encrypt_posting()
{
int c;
char temp[TEMP_SIZE];
char *recipient;
FILE *pgp_fd;
char *ptr;
char *ptr2;
FILE *coded_fd;
struct stat *statptr;
char *space, *spaceptr;
FILE  *save_filefd;

if(debug_flag)
	{
	fprintf(stdout, "pgp_encrypt_posting(): arg none\n");
	}

/* ask for recipient name */
ptr = (char *) fl_show_input("Recipients name?", "");
if(! ptr) return(0);/* cancel pressed */
recipient = strsave(ptr);

/*
In case of a new posting there is no body.nn,
for this reason all operations are on the posting editor,
so (possibly body.nn > posting editor > temp file > pgp > temp2 file > posting 
editor.
*/

/* save the text in the editor to file temp1 */
sprintf(temp, "%s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name);
save_filefd = fopen(temp, "w");
if(! save_filefd)
	{
	fl_show_alert("could not open file for write ", temp, "", 0);
	return(0);
	}

ptr = (char *) fl_get_input(fdui -> posting_body_editor);	
if(! ptr) return(0);/* problems */

/* expand posting macros BEFORE encryption */
ptr2 = expand_posting_macros(ptr);
if(! ptr2) return 0;/* fatal, malloc failed */

/*fprintf(stdout, "WRITING file=%s data=%s\n", temp, ptr2);*/

fprintf(save_filefd, "%s", ptr2);
fclose(save_filefd);	

free (ptr2);

/* let pgp read from file temp1 write to file temp2 */
/* pgpe -r recipient -a infile -o outfile */
sprintf(temp,\
"xterm -title %c%s%c \
-e pgpe -r %c%s%c \
-a %s/.NewsFleX/postings/%s/temp1 -o %s/.NewsFleX/postings/%s/temp2",
'"', "PGP interactive", '"',\
'"', recipient, '"',\
home_dir, postings_database_name,\
home_dir, postings_database_name);
pgp_fd = popen(temp, "r");
if(! pgp_fd) return(0);
pclose(pgp_fd);

/* read from file temp2 into editor*/
sprintf(temp, "%s/.NewsFleX/postings/%s/temp2",\
home_dir, postings_database_name);
coded_fd = fopen(temp, "r");
if(! coded_fd) return(0);

statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(coded_fd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"pgp_encrypt_posting(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(coded_fd);
		if(! ferror(coded_fd) ) break;
		perror("encrypt_and_show_posting(): read failed ");
		}/* end while error re read */	

	if(feof(coded_fd) )
		{
		fclose(coded_fd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

/*
signal to send now or send later to make a not encoded backup of the body.nn,
else we will NEVER be able to regain the original text for inspection
or modification, since we need the recipients private key for that :).
*/
/* backup to global */
if(unencoded_data) free(unencoded_data);
unencoded_data =\
strsave( (char *)fl_get_input(fdui -> posting_body_editor) );
posting_encoded_flag = 1;

/* coded posting to input field */
/* later this will be written to body.nnn */
fl_set_input(fdui -> posting_body_editor, space);
free(space);

/* remove the temp files */
sprintf(temp, "%s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name);
unlink(temp);
sprintf(temp, "%s/.NewsFleX/postings/%s/temp2",\
home_dir, postings_database_name);
unlink(temp);

return(1);
}/* end function pgp_encrypt_posting */


int show_unencrypted_posting(long posting_id)
{
int c;
char temp[TEMP_SIZE];
FILE *precoded_fd;
struct stat *statptr;
char *space, *spaceptr;
	
if(debug_flag)
	{
	fprintf(stdout, "show_unencrypted_posting(): arg none\n");
	}

/* argument check */
if(posting_id < 0) return(0);

goto fd;

if(posting_encoded_flag)
	{
	fl_set_input(fdui -> posting_body_editor, unencoded_data);
	return(1);
	}
else/* for some reason there was no unencoded backup */
	{
	return(0);
	}
/* never here, unencoded messages not saved to disk */

fd:
c = 0;
get_posting_encoding(posting_id, &c);

/*fprintf(stdout, "posting c flag=%d\n", c);*/

if(! c)/* first time here */
	{
	if(posting_encoded_flag)
		{
		fl_set_input(fdui -> posting_body_editor, unencoded_data);
		return(1);
		}
	else/* for some reason there was no unencoded backup */
		{
		return(0);
		}
	}

/* read from precoded.nn */
sprintf(temp, "%s/.NewsFleX/postings/%s/unencoded.%ld",\
home_dir, postings_database_name, posting_id);
precoded_fd = fopen(temp, "r");
if(! precoded_fd) return(0);

statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(precoded_fd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"encrypt_and_show_posting(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(precoded_fd);
		if(! ferror(precoded_fd) ) break;
		perror("encrypt_and_show_posting(): read failed ");
		}/* end while error re read */	

	if(feof(precoded_fd) )
		{
		fclose(precoded_fd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

fl_set_input(fdui -> posting_body_editor, space);
free(space);

return(1);
}/* end function show_unencrypted_posting */


int pgp_append_public_key_to_posting()
{
int c, i;
int cupos, cux, cuy;
int topline;
char *input_field_ptr;
struct stat *statptr;
char *space, *spaceptr;
FILE *insert_fd;
char insert_filename[TEMP_SIZE];
char temp[TEMP_SIZE];
FILE *pgp_filefd;

if(debug_flag)
	{
	fprintf(stdout, "pgp_append_public_key_to_posting(): arg none\n");
	}

topline = fl_get_input_topline(fdui -> posting_body_editor);

/* get cursor position */
cupos = fl_get_input_cursorpos(fdui -> posting_body_editor, &cux, &cuy);
if(cupos == -1)/* field not selected */
	{
	fl_show_alert(\
	"Select the posting body editor first", "", "", 0);
	return(0);
	}

sprintf(insert_filename, "%s/.NewsFleX/setup/pgp_public_key.asc", home_dir);
insert_fd = fopen(insert_filename, "r");
/* if file does not exists, use pgpk -x userid to create a public key */
if(! insert_fd)
	{
	sprintf(temp,\
	"xterm -title %c%s%c -e \
	pgpk -x %c%s%c -o %s/.NewsFleX/setup/pgp_public_key.asc",\
	'"', "PGP interactive", '"',\
	'"', real_name, '"', home_dir);

	if(debug_flag)
		{
		fprintf(stdout,\
		"append_public_key_to_posting(): opening:%s\n", temp);
		}

	pgp_filefd = popen(temp, "r");
	if(! pgp_filefd)
		{
		fl_show_alert("Cannot execute", temp, "", 0);
		return(0);
		}
	pclose(pgp_filefd);

	/* try to open file for read */
	insert_fd = fopen(insert_filename, "r");
	if(! insert_fd)
		{
		fl_show_alert("Cannot open file", insert_filename, "for read", 0);
		return(0);
		}
	}/* end pgp_public_key.asc did not yet exists */
	
/* get length from input */
input_field_ptr = (char *) fl_get_input(fdui -> posting_body_editor);
if(! input_field_ptr) return(0);

/* create space for total */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(insert_fd), statptr);

space = malloc(statptr -> st_size + strlen(input_field_ptr) + 2);
free(statptr);
if(! space) return(0);

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
		c = getc(insert_fd);
		if(! ferror(insert_fd) ) break;
		perror("append_public_key_to_posting(): read failed ");
		}/* end while error re read */	

	if(feof(insert_fd) )
		{
		fclose(insert_fd);
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
return(1);
}/* end function pgp_append_public_key_to_posting */


int pgp_clear_sign_posting()
{
int a, c;
char temp[TEMP_SIZE];
FILE *pgp_fd;
FILE *coded_fd;
struct stat *statptr;
char *space, *spaceptr;
FILE  *save_filefd;
char *ptr;
char *ptr2;
FILE *passphrase_filefd;
char *pass_phrase;
	
if(debug_flag)
	{
	fprintf(stdout, "pgp_clear_sign_posting(): arg none\n");
	}

/* test if ~/.NewsFleX/setup/pgp_passphrase present */
sprintf(temp, "%s/.NewsFleX/setup/pgp_passphrase", home_dir);
passphrase_filefd = fopen(temp, "r");
if(! passphrase_filefd)
	{
	/* ask for pass phrase */
	ptr = (char *) fl_show_input("Pass phrase", "");
	if(! ptr) return(0);/* cancel pressed */
	pass_phrase = strsave(ptr);
	}
else
	{
	a = readline(passphrase_filefd, temp);
	if(a != EOF) fclose(passphrase_filefd);
	pass_phrase = strsave(temp);	
	}

/* save the text in the editor to file temp 1 */
sprintf(temp, "%s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name);
save_filefd = fopen(temp, "w");
if(! save_filefd)
	{
	fl_show_alert("could not open file for write ", temp, "", 0);
	return(0);
	}

ptr = (char *) fl_get_input(fdui -> posting_body_editor);	
if(! ptr) return(0);/* problems */

/* expand posting macros BEFORE encryption */
ptr2 = expand_posting_macros(ptr);
if(! ptr2) return 0;/* fatal, malloc failed */

/*fprintf(stdout, "WRITING file=%s data=%s\n", temp, ptr2);*/

fprintf(save_filefd, "%s", ptr2);
fclose(save_filefd);	

free (ptr2);

/* let pgp read from file temp1 and write to file temp2 */
/* pgps -u userid -a infile -o outfile */
sprintf(temp,\
"xterm -title %c%s%c \
-e pgps -z %c%s%c \
-u %c%s%c \
-a %s/.NewsFleX/postings/%s/temp1 \
-o %s/.NewsFleX/postings/%s/temp2",\
'"', "PGP interactive", '"',\
'"', pass_phrase, '"',\
'"', real_name, '"',\
home_dir, postings_database_name,\
home_dir, postings_database_name);

pgp_fd = popen(temp, "r");
if(! pgp_fd) return(0);
pclose(pgp_fd);

/* read from file temp2 into editor*/
sprintf(temp, "%s/.NewsFleX/postings/%s/temp2",\
home_dir, postings_database_name);
coded_fd = fopen(temp, "r");
if(! coded_fd) return(0);

statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(coded_fd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"pgp_encrypt_posting(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(coded_fd);
		if(! ferror(coded_fd) ) break;
		perror("encrypt_and_show_posting(): read failed ");
		}/* end while error re read */	

	if(feof(coded_fd) )
		{
		fclose(coded_fd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

/*
signal to send now or send later to make a not encoded backup of the body.nn,
else we will NEVER be able to regain the original text for inspection
or modification, since we need the recipients private key for that :).
*/
/* backup to global */
if(unencoded_data) free(unencoded_data);
unencoded_data =\
strsave( (char *)fl_get_input(fdui -> posting_body_editor) );
posting_encoded_flag = 1;

/* coded posting to input field */
/* later this will be written to body.nnn */
fl_set_input(fdui -> posting_body_editor, space);
free(space);

/* remove the temp files */
sprintf(temp, "%s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name);
unlink(temp);
sprintf(temp, "%s/.NewsFleX/postings/%s/temp2",\
home_dir, postings_database_name);
unlink(temp);

return(1);
}/* end function pgp_clear_sign_posting */


int pgp_check_signature_decode_and_show_article(char *group, long article_id)
{
int c;
char temp[TEMP_SIZE];
struct stat *statptr;
char *space;
char *spaceptr;
char path_filename[TEMP_SIZE];
FILE *pgp_fd;
FILE *info_fd;

if(debug_flag)
	{
	fprintf(stdout,\
	"pgp_check_signature_decode_and_show_artice;(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/*
stderror first, then decoded article to decoded file.
sterror shows key info obtained from file.
pgpv body.nn 2>> info -o decoded.nn
*/

/* unlink any decoded.nn, else pgp will ask for overwrite */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
home_dir, news_server_name, database_name, group, article_id);
unlink(temp);

fl_set_object_label(fdui -> command_status_display, "Starting PGP");
XSync(fl_get_display(), 0);
/* pgpv infile 2> infofile -o outfile */

sprintf(temp,\
/*"xterm -title %c%s%c -e \*/
"pgpv %s/.NewsFleX/%s/%s/%s/body.%ld \
2> %s/.NewsFleX/%s/%s/%s/pgp_info \
-o %s/.NewsFleX/%s/%s/%s/decoded.%ld",\
/*'"', "PGP interactive", '"',\*/
home_dir, news_server_name, database_name, group, article_id,\
home_dir, news_server_name, database_name, group,\
home_dir, news_server_name, database_name, group, article_id);

if(debug_flag)
	{
	fprintf(stdout,\
	"pgp_check_signature_decode_and_show_article(): opening:%s\n", temp);
	}

pgp_fd = popen(temp, "r");
if(! pgp_fd) return(0);

pclose(pgp_fd);

set_article_encoding(group, article_id, DECODED_PRESENT, 1);

/* display pgp_info */
sprintf(path_filename, "%s/.NewsFleX/%s/%s/%s/pgp_info",\
home_dir, news_server_name, database_name, group);

info_fd = fopen(path_filename, "r");			
if(! info_fd)
	{
	fl_show_alert("Cannot open file", path_filename, "for read", 0);
	return(0);
	}

/* create space for file */
statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(info_fd), statptr);

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space) return(0);

spaceptr = space;
/* read from file */
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(info_fd);
		if(! ferror(info_fd) ) break;
		perror(\
		"pgp_check_signature_decode_and_show_article(): read failed\n");
		}/* end while error re read */	

	if(feof(info_fd) )
		{
		fclose(info_fd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from posting body */

*spaceptr = 0;/* string termination */

/* show decoded article */
show_article_body(group, article_id, 1);

fl_set_object_label(fdui -> command_status_display, "");

/* show key info in separate form */
fl_set_input(fdui -> coding_form_input_field, space);
free(space);

/* delete pgp_info */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/pgp_info",\
home_dir, news_server_name, database_name, group);
unlink(temp);

/* showe the key info */
fl_show_form(fd_NewsFleX -> coding_form,\
FL_PLACE_CENTER, FL_NOBORDER, "");

return(1);
}/* end function pgp_check_signature_decode_and_show_article */


int pgp_decode_and_show_article(char *group, long article_id)
{
int a;
char temp[TEMP_SIZE];
char *pass_phrase;
FILE *pgp_filefd;
FILE *passphrase_filefd;
FILE *decoded_fd;
char *ptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"pgp_decode_and_show_article(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* test if ~/.NewsFleX/setup/pgp_passphrase present */
sprintf(temp, "%s/.NewsFleX/setup/pgp_passphrase", home_dir);
passphrase_filefd = fopen(temp, "r");
if(! passphrase_filefd)
	{
	/* ask for pass phrase */
	ptr = (char *) fl_show_input("Pass phrase", "");
	if(! ptr) return(0);/* cancel pressed */
	pass_phrase = strsave(ptr);
	}
else
	{
	a = readline(passphrase_filefd, temp);
	if(a != EOF) fclose(passphrase_filefd);
	pass_phrase = strsave(temp);	
	}

/* check if decoded.nn  exists */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/decoded.%ld",\
home_dir, news_server_name, database_name, group, article_id);
decoded_fd = fopen(temp, "r");
if(! decoded_fd)
	{
	fl_set_object_label(fdui -> command_status_display, "Starting PGP");
	XSync(fl_get_display(), 0);
	/*
	This allows you to correct if wrong pass phrase entered, and
	shows key info.
	*/
	sprintf(temp,\
	"xterm -title %c%s%c \
	-e pgpv -z %c%s%c \
	%s/.NewsFleX/%s/%s/%s/body.%ld \
	-o %s/.NewsFleX/%s/%s/%s/decoded.%ld",\
	'"', "PGP interactive", '"',\
	'"', pass_phrase, '"',\
	home_dir, news_server_name, database_name, group, article_id,\
	home_dir, news_server_name, database_name, group, article_id);

	if(debug_flag)
		{
		fprintf(stdout,\
		"pgp_decode_and_show_article(): opening:%s\n", temp);
		}

	pgp_filefd = popen(temp, "r");
	if(! pgp_filefd) return(0);

	pclose(pgp_filefd);
	free(pass_phrase);

	set_article_encoding(group, article_id, DECODED_PRESENT, 1);

	}/* end if decoded.nn does not exist */
else
	{
	fclose(decoded_fd);
	}

show_article_body(group, article_id, 1);

fl_set_object_label(fdui -> command_status_display, "");

return(1);
}/* end function pgp_decode_and_show_article */


int otp_encrypt_posting()
{
int c;
char temp[TEMP_SIZE];
char *mask_filename;
char *ptr, *ptrc;
char *ptr2;
FILE *mask_fd;
FILE *temp_fd;
char temp_filename[TEMP_SIZE];
struct stat *statptr;
char *space, *spaceptr;
FILE *exec_fd;

if(debug_flag)
	{
	fprintf(stdout, "otp_encrypt_posting(): arg none\n");
	}

/*
In case of a new posting there is no body.nn,
for this reason all operations are on the posting editor,
so (possibly body.nn > posting editor > xor mask file > posting editor >
posting
*/

/* ask for mask file name */
ptr = (char *) fl_show_input(\
"One time pad mask file (full pathname)?", "");
if(! ptr) return(0);/* cancel pressed */
mask_filename = strsave(ptr);

/* open the mask file for read */
mask_fd = fopen(mask_filename, "r");
if(! mask_fd)
	{
	fl_show_alert("Cannot open file", mask_filename, "for read", 0);
	return(0);
	}

sprintf(temp_filename, "%s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name); 
/* open the temp file for write */
temp_fd = fopen(temp_filename, "w");
if(! temp_fd)
	{
	fl_show_alert("Cannot open file", temp_filename, "for write", 0);
	fclose(mask_fd);
	return(0);
	}

/* get the text from the editor */
ptr = (char *)fl_get_input(fdui -> posting_body_editor);

/* backup to global */
if(unencoded_data) free(unencoded_data);
unencoded_data = strsave(ptr);

/* expand posting macros BEFORE encryption */
ptr2 = expand_posting_macros(ptr);
if(! ptr2) return 0;/* fatal, malloc failed */

/*fprintf(stdout, "WRITING file=%s data=%s\n", temp, ptr2);*/

/* xor with mask file and write to temp */
ptrc = ptr2;
while(1)
	{
	if(! (*ptrc) )
		{
		fclose(mask_fd);
		fclose(temp_fd);
		break;
		}

	while(1)/* error re read */
		{
		c = getc(mask_fd);
		if(! ferror(mask_fd) ) break;
		perror("otp_encrypt_posting(): read failed ");
		}/* end while error re read */	

	if(feof(mask_fd) )
		{
		fl_show_alert(\
		"Mask file", mask_filename, "was to short for message!", 0);
		fclose(mask_fd);
		free(mask_filename);
		fclose(temp_fd);
		unlink(temp_filename);		

		free (ptr2);
		return(0);
		}
	putc( (*ptrc ^= c), temp_fd);
	ptrc++;
	}/* end while characters */
free(mask_filename);

free (ptr2);

/* mpack -s no_subject -o outputfile frog.jpg */
sprintf(temp,\
"mpack -s none \
-o %s/.NewsFleX/postings/%s/temp2 %s/.NewsFleX/postings/%s/temp1",\
home_dir, postings_database_name,\
home_dir, postings_database_name);

exec_fd = popen(temp, "r");
pclose(exec_fd);
unlink(temp_filename);

sprintf(temp_filename, "%s/.NewsFleX/postings/%s/temp2",\
home_dir, postings_database_name);
temp_fd = fopen(temp_filename, "r");
if(! temp_fd) return(0);

statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(temp_fd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"otp_encrypt_posting(): malloc could not allocate space\n");
		}
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(temp_fd);
		if(! ferror(temp_fd) ) break;
		perror("otp_encrypt_posting(): read failed ");
		}/* end while error re read */	

	if(feof(temp_fd) )
		{
		fclose(temp_fd);
		break;
		}
	*spaceptr = c;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

/* encrypted text to editor */
fl_set_input(fdui -> posting_body_editor, space);
free(space);

unlink(temp_filename);

/*
signal to send now or send later to make a not encoded backup of the body.nn,
*/
posting_encoded_flag = 1;

return(1);
}/* end function otp_encrypt_posting */


int otp_decode_and_show_article(char *group, long article_id)
{
int c, d;
char temp[TEMP_SIZE];
FILE *mask_fd;
FILE *temp_fd;
FILE *exec_fd;
char *ptr;
char *mask_filename;
struct stat *statptr;
char *space, *spaceptr;

if(debug_flag)
	{
	fprintf(stdout,\
	"otp_decode_and_show_article(): arg group=%s article_id=%ld\n",\
	group, article_id);
	}

/* argument check */
if(! group) return(0);
if(article_id < 0) return(0);

/* ask for mask file name */
ptr = (char *) fl_show_input(\
"One time pad mask file (full pathname)?", "");
if(! ptr) return(0);/* cancel pressed */
mask_filename = strsave(ptr);

/* open  the mask file */
mask_fd = fopen(mask_filename, "r");
if(! mask_fd)
	{
	fl_show_alert("Cannot open file", mask_filename, "for read", 0);
	return(0);
	}

fl_set_object_label(fdui -> command_status_display, "Staring munpack");

/* start munpack */
sprintf(temp,\
"cat %s/.NewsFleX/%s/%s/%s/head.%ld %s/.NewsFleX/%s/%s/%s/body.%ld |\
munpack -f -q -C %s/.NewsFleX/%s/%s/%s > %s/.NewsFleX/%s/%s/%s/info",\
home_dir, news_server_name, database_name, group, article_id,\
home_dir, news_server_name, database_name, group, article_id,\
home_dir, news_server_name, database_name, group,\
home_dir, news_server_name, database_name, group);

exec_fd = popen(temp, "r");
if(! exec_fd) return(0);
pclose(exec_fd);
	
/* remove the info file generated by munpack */	
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/info",\
home_dir, news_server_name, database_name, group);
unlink(temp);

/*
decoded data now in ~/.NewsFleX/news_server_name/database_name/group/temp1
*/ 

/* open the temp1 file generated by munpack */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/temp1",\
home_dir, news_server_name, database_name, group);
temp_fd = fopen(temp, "r");
if(! temp_fd)
	{
	fl_show_alert("Cannot open file", temp, "for read", 0);
	fclose(mask_fd);
	return(0);
	}

/* read from temp1 and xor with mask_fd into space */
statptr = (struct stat*) malloc(sizeof(struct stat) );
fstat(fileno(temp_fd), statptr);
space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"otp_decode_and_show_article(): malloc could not allocate space\n");
		}
	fclose(temp_fd);
	fclose(mask_fd);
	return(0);
	}

spaceptr = space;
while(1)
	{
	while(1)/* error re read */
		{
		c = getc(temp_fd);
		if(! ferror(temp_fd) ) break;
		perror("encrypt_and_show_posting(): temp1 read failed ");
		}/* end while error re read */	

	if(feof(temp_fd) )
		{
		fclose(temp_fd);
		break;
		}

	while(1)/* error re read */
		{
		d = getc(mask_fd);
		if(! ferror(mask_fd) ) break;
		perror("encrypt_and_show_posting(): mask read failed ");
		}/* end while error re read */	

	if(feof(mask_fd) )
		{
		fl_show_alert("Mask file" , mask_filename, "is to short", 0);
		fclose(mask_fd);
		/* do not exit here */
		break;
		}
	*spaceptr = c;
	*spaceptr ^= d;
	spaceptr++;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

free(mask_filename);

/* encrypted text to display */
fl_set_input(fdui -> article_body_input_field, space);
free(space);

/* remove decoded file temp1 */
sprintf(temp, "%s/.NewsFleX/%s/%s/%s/temp1",\
home_dir, news_server_name, database_name, group);
/*unlink(temp);*/

fl_set_object_label(fdui -> command_status_display, "");

return(1);
}/* end function otp_decode_and_show_article */


int otp_create_random_key_file()
{
int c;
long li;
FILE *out_fd;
char *filename;
long filesize;
char *ptr;
char temp[TEMP_SIZE];

if(debug_flag)
	{
	fprintf(stdout,\
	"otp_create_random_key_file(): arg none\n");
	}

/* ask for file name */
ptr = (char *) fl_show_input("File name (full path)?", "");
if(! ptr) return(0);/* cancel pressed */
filename = strsave(ptr);

/* test if file exists */
out_fd = fopen(filename, "r");
if(out_fd)
	{
	fclose(out_fd);
	sprintf(temp, "File %s exists overwrite?", filename);
	if( !fl_show_question(temp, 0) )/* 0 = mouse on NO */
		{
		free(filename);
		return(0);
		}
	}

/* ask for file size */
ptr = (char *) fl_show_input("File size (bytes)?", "");
if(! ptr) return(0);/* cancel pressed */
filesize = atol(ptr);

/*fprintf(stdout, "opening %s lenght %ld\n", filename, filesize);*/

out_fd = fopen(filename, "w");
if(! out_fd)
	{
	fl_show_alert("Cannot open file", filename, "for write", 0);
	free(filename);
	return(0);
	}

li = 0;
while(1)
	{
	c = (int) (255.0 * rand() / (RAND_MAX + 1.0) );

/*	fprintf(stdout, "c=%d\n", c);*/

	while(1)
		{
		fputc(c, out_fd);
		if(! ferror(out_fd) ) break;
		perror("otp_create_random_key_file(): write failed ");
		}
	
	li++;
	if(li >= filesize) break;
	}
fclose(out_fd);
free(filename);

return(1);
}/* end function otp_create_random_key_file */


