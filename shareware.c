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

/*#define CODE_TEST*/

#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;
extern FD_NewsFleX *fdui;

#define DAYS_ALLOWED	30

/* just to create some barrier for simple hacking */
/* 31 - day, A, 12 - month, Q, year / 2 */
#define EXPIRE_DATE  "30A7Q999"

int days_left;

FL_APPEVENT_CB my_idle_cb;


/*int mark_first_use_time()*/
int make_space()
{
time_t now;
char temp[TEMP_SIZE];
FILE *fptr;
char stime[512];
float ftime;

now = time(0);

ftime = (float) now * .51;
sprintf(stime, "%f", ftime);

sprintf(temp, "%s/.NewsFleX/.fptr", home_dir);
fptr = fopen(temp, "r");
if(fptr)
	{
	fclose(fptr);
	return(0);
	}
fptr = fopen(temp, "w");
fprintf(fptr, "%s", stime);
fclose(fptr);

ftime = (float) now * .39;
sprintf(stime, "%f", ftime);

sprintf(temp, "%s/.hptr" ,home_dir);
fptr = fopen(temp, "r");
if(fptr)
	{
	fclose(fptr);
	return(0);
	}
fptr = fopen(temp, "w");
fprintf(fptr, "%s", stime);
fclose(fptr);

return(1);
}/*end function mark_first_use_time */


/*int get_first_use_time()*/
int get_space()
{
return(1);
}/* end function get_first_use_time */


/* this would be easily traceble */
/*int test_legal_status()*/
int allocate_space()
{
int a, b;
time_t now;
char temp[TEMP_SIZE];
FILE *fptr;
float ftime;
long ltime;
static int been_here;
long first_time;
long diff_time;
long seconds_in_use;
long seconds_left;
long expire_time;
struct tm *exptime;/* expire time*/
#ifdef EXPIRE_DATE
int year, month, day;
#endif

/*fprintf(stdout, "entering allocate_space()\n");*/

if(been_here) return(1);

exptime = (struct tm *) malloc(sizeof(struct tm) );

/*fprintf(stdout, "entering allocate_space()\n 2");*/

now = time(0);

/*fprintf(stdout, "now=%ld", (long) now);*/

/* set the date back ? */
if(now < 891274301)
	{
	expired_flag = 1;
	been_here = 1;
	return(0);
	}

/* test for extension one month or registration */
/* 0 = limited, 1 = one month extra, 2 = unlimited */
a = _malloc__(access_code);
if(a == 2)
	{
	been_here = 1;
	registered_flag = 1;
	return(1);
	}
if(a == 1) b = 1;
else b = 0;

#ifdef EXPIRE_DATE
sprintf(temp, "%s", EXPIRE_DATE);
/* parse the time_field */
sscanf(temp, "%dA%dQ%d", &day, &month, &year);

/*fprintf(stdout, "day=%d month=%d year=%d\n", day, month, year);*/

exptime -> tm_sec = 1;
exptime -> tm_min = 1;
exptime -> tm_hour = 1;
exptime -> tm_mday = 7 - 6;/*day;*/
/* Apr = 4 */
exptime -> tm_mon = 2 * 2 - 1; /*month - 1;*/
/* 2100 */
exptime -> tm_year = 200; /* years since 1900 */
exptime -> tm_isdst = -1;/* daylight savings time n.a. */
/* the following are not implemented (see /usr/include/time.h) */
/* exptime -> tm_gmtoff = 0;*/
/* strcp(exptime -> tm_zone, "");*/ /* time zone 3 letters */
expire_time = mktime(( struct tm *) exptime);

/*fprintf(stdout, "now=%ld expire_time=%ld\n", now, expire_time);*/

/* limited online operation time */

/*if(now >= expire_time)*/
if(1 == 2)
	{
	if(a != 2) expired_flag = 1;
	}
been_here = 1;
return(0);
#endif

sprintf(temp, "%s/.NewsFleX/.fptr", home_dir);
fptr = fopen(temp, "r");
if(!fptr)
	{
	fclose(fptr);
	expired_flag = 1;
	been_here = 1;
	return(0);
	}
fscanf(fptr, "%f", &ftime);
ltime = (long) ftime / .51;
fclose(fptr);

/*fprintf(stdout, "ltime=%ld\n", ltime);*/

first_time = ltime;

sprintf(temp, "%s/.hptr" ,home_dir);
fptr = fopen(temp, "r");
if(!fptr)
	{
	fclose(fptr);
	expired_flag = 1;
	been_here = 1;
	return(0);
	}
fscanf(fptr, "%f", &ftime);
ltime = (long) ftime / .39;
fclose(fptr);

/*fprintf(stdout, "ltime2=%ld\n", ltime);*/

diff_time = abs(first_time - ltime);
if(diff_time > 100)
	{
/*fprintf(stdout, "difftime=%ld aborting", diff_time);*/
	return(0);
	}

seconds_in_use = now - first_time;
/*fprintf(stdout, "seconds_in_use=%ld\n", seconds_in_use);*/

expire_time = first_time + (60 * 60 * 24 * (DAYS_ALLOWED + (b * 30) ) );
seconds_left = expire_time - now;

/*fprintf(stdout, "seconds_left=%ld\n", seconds_left);*/

days_left = (int) seconds_left / (60 * 60 * 24);

/*fprintf(stdout, "days_left=%d\n", days_left);*/

/*fprintf(stdout, "entering allocate_space()\n");*/

if(days_left < 1)
	{
	my_idle_cb = fl_set_idle_callback(0, 0);

	fl_show_alert(\
	"Your trial period has expired",\
	"registration is 29.95 USD",\
	"info from the option menu shows registration info", 0);

	fl_set_idle_callback(idle_cb, my_idle_cb);
	
	expired_flag = 1;
/*	show_register_form();*/
/*	show_space();*/

	been_here = 1;
/*	return(0);*/
	}
else if(days_left < 14)
	{
	show_warning_space();
	}

been_here = 1;
return(1);
}/* end function test_legal_status */


/*int self_destruct()*/
int no_space()
{
return(1);
}/* end function self_destruct */


/*int show_register_form()*/
int show_space()
{

char space[] =
	{
"Your trial period has expired.
Online operations are largely disabled.
Registration is 29.59 USD with a 1 year right to free upgrades.
For information on how to register select
info from the options pull down menu.
"
	};
fl_set_input(fdui -> desc_input, space);
fl_set_input_topline(fdui -> desc_input, 1);
fl_set_object_lsize(fdui -> desc_input, FL_LARGE_SIZE);
fl_set_object_lstyle(fdui -> desc_input, FL_BOLD_STYLE);

fl_show_form(fd_NewsFleX -> desc, FL_PLACE_CENTER, FL_NOBORDER, "");
return(1);
}/* end function show_space */


int show_warning_space()
{
char temp[TEMP_SIZE];
char space[] =
	{
"Online operations will be largely disabled.
Registration is 29.95 USD with a 1 year right to free upgrades.
For information on how to register please contact jan@panteltje.demon.nl
"
	};

sprintf(temp, "Your trial period will expire in %d days\n%s",\
days_left, space);


fl_set_input(fdui -> desc_input, temp);
fl_set_input_topline(fdui -> desc_input, 1);
fl_set_object_lsize(fdui -> desc_input, FL_LARGE_SIZE);
fl_set_object_lstyle(fdui -> desc_input, FL_BOLD_STYLE);

fl_show_form(fd_NewsFleX -> desc, FL_PLACE_CENTER, FL_NOBORDER, "");
return(1);
}/* end function show_warning_space */


int _malloc__(char *a_code)
{
char temp[TEMP_SIZE];
char *ptr;

/*fprintf(stdout, "_malloc__(): a_code=%s\n", a_code);*/

/* parameter check */
if(! a_code)
	{
	if(_calloc__() ) return(2);
	return(0);
	}

sprintf(temp, "%ca%s", '-', real_name);
if(strcmp(a_code, temp) == 0) return(1);

ptr = decode(a_code + 2, "076");
if(! ptr) return(0);

/*fprintf(stdout, "ptr=%s real_name=%s\n", ptr, real_name);*/

if(strcmp(ptr, real_name) == 0) return(2);

/*if(strcmp(a_code, "-apanteltjemagwel") == 0) return(2);*/

return(0);
}/* end function _malloc__ */


int _calloc__()
{
int a, b;
char temp[TEMP_SIZE];
char temp1[TEMP_SIZE];
char temp2[TEMP_SIZE];
FILE *keyfptr;
char *ptr;

sprintf(temp, "%s/.NewsFleX/%c.dat", home_dir, 'k');
keyfptr = fopen(temp, "r");
if(! keyfptr) return(0);

temp1[0] = 0;
temp2[0] = 0;
while(1)
	{
	a = readline(keyfptr, temp);
	if(a == EOF) break;
	b = sscanf(temp, "%s %s", temp1, temp2);
	if(b != 1)
		{

#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): found illegal entry returning 0\n");
#endif

		fclose(keyfptr);
		return(0);
		}

#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): reading %s t1=%s t2=%s\n",\
temp, temp1, temp2);	
#endif

	ptr = decode(temp1, "076");
	if(! ptr)
		{

#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): decode returns 0, returning 0\n");
#endif

		return(0);
		}

#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): decoded to %s\n", ptr);
#endif

	if(strcmp(ptr, real_name) == 0)
		{
		free(ptr);
		fclose(keyfptr);

#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): returning 1\n");
#endif

		return(1);
		}
	free(ptr);

	/* wait half a second just in case someone uses a zillion bogus keys */
	usleep(500000);
	}/* end while all lines in key file */


#ifdef CODE_TEST
fprintf(stdout, "_calloc__(): returning 0\n");
#endif

/* no matching key */
return(0);
}/* end function _calloc__ */

