/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997 <Jan Mourer>
www: pante@pi.net
snail www: PO BOX 61  SINT ANNA  9076ZP HOLLAND

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

#define HTTP_SERVER_CONNECT_RETRIES	5

FL_APPEVENT_CB an_idle_cb;

#define DISPLAY_PLAIN	1
#define DISPLAY_HTML	2

/*
Possibilities:
BODY		BACKGROUND TEXT BGCOLOR LINK YLINK ALINK
BASE		HREF
A			HREF NAME URN REL REV TITLE METHODS
LINK		HREF URN REL REV TITLE METHODS
IMG			SRC ALT ALIGN ISMAP
INPUT
ISINDEX
FORM
*/	


struct quote
{
char *name;
const int value;
};

struct quote quote_tab[]=
{
{"nbsp", 160},
{"iexcl", 161},
{"cent", 162},
{"pound", 163},
{"curren", 164},
{"yen", 165},
{"brvbar", 166},
{"sect", 167},
{"uml", 168},
{"copy", 169},
{"ordf", 170},
{"laquo", 171},
{"not", 172},
{"shy", 173},
{"reg", 174},
{"macr", 175},
{"deg", 176},
{"plusmn", 177},
{"sup2", 178},
{"sup3", 179},
{"acute", 180},
{"micro", 181},
{"para", 182},
{"middot", 183},
{"cedil", 184},
{"sup1", 185},
{"ordm", 186},
{"raquo", 187},
{"frac14", 188},
{"frac12", 189},
{"frac34", 190},
{"iquest", 191},
{"Agrave", 192},
{"Aacute", 193},
{"Acirc", 194},
{"Atilde", 195},
{"Auml", 196},
{"Aring", 197},
{"AElig", 198},
{"Ccedil", 199},
{"Egrave", 200},
{"Eacute", 201},
{"Ecirc", 202},
{"Euml", 203},
{"Igrave", 204},
{"Iacute", 205},
{"Icirc", 206},
{"Iuml", 207},
{"ETH", 208},
{"Ntilde", 209},
{"Ograve", 210},
{"Oacute", 211},
{"Ocirc", 212},
{"Otilde", 213},
{"Ouml", 214},
{"times", 215},
{"Oslash", 216},
{"Ugrave", 217},
{"Uacute", 218},
{"Ucirc", 219},
{"Uuml", 220},
{"Yacute", 221},
{"THORN", 222},
{"szlig", 223},
{"agrave", 224},
{"aacute", 225},
{"acirc", 226},
{"atilde", 227},
{"auml", 228},
{"aring", 229},
{"aelig", 230},
{"ccedil", 231},
{"egrave", 232},
{"eacute", 233},
{"ecirc", 234},
{"euml", 235},
{"igrave", 236},
{"iacute", 237},
{"icirc", 238},
{"iuml", 239},
{"eth", 240},
{"ntilde", 241},
{"ograve", 242},
{"oacute", 243},
{"ocirc", 244},
{"otilde", 245},
{"ouml", 246},
{"divide", 247},
{"oslash", 248},
{"ugrave", 249},
{"uacute", 250},
{"ucirc", 251},
{"uuml", 252},
{"yacute", 253},
{"thorn", 254},
{"yuml", 255}
};

#define QUOTE_SIZE (sizeof(quote_tab) / sizeof(struct quote) )

int hsize;

#define EXPECT_BODY		1
#define EXPECT_BASE		2
#define EXPECT_A		3
#define EXPECT_LINK		4
#define EXPECT_IMG		5
#define EXPECT_SUB		6
#define IN_HREF			7
#define IN_SRC			8
#define IN_BACKGROUND	9
#define IN_URN			10
#define IN_REL			11
#define IN_REV			12
#define IN_TITLE		13
#define IN_METHODS		14
#define IN_TEXT			15
#define IN_BGCOLOR		16
#define IN_LINK			17
#define IN_YLINK		18
#define IN_ALINK		19
#define IN_ALIGN		20
#define IN_ISMAP		21
#define IN_ALT			22
#define IN_NAME			23
#define IN_COMMENT		24
#define IN_WIDTH		25
#define IN_HEIGHT		26
#define IN_BORDER		27


int scip_tag(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(*ptr != '<') return(0);
ptr += 1;

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

	ptr++;
	}/* end while */
}/* end function scip_tag */


/* do link here */


char *do_img(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp_src[1024];
char temp_alt[1024];
char temp_out[2048];
int src_pos;
int alt_pos;
int mode;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<IMG", 4) != 0) return(0);
ptr += 4;

temp_src[0] = 0;
temp_alt[0] = 0;
temp_out[0] = 0;
src_pos = 0;
alt_pos = 0;
mode = EXPECT_SUB;
in_arg_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if( isspace(*ptr) || (*ptr == '<') )
		{
		/* leave spaces in text */
		if(mode != IN_ALT)
			{
			if(in_arg_flag)
				{
				ptr++;
				continue;
				}
			}
		}
/*	
	if(*ptr == '<')
		{
		ptr++;
		continue;
		}
*/
	if(*ptr == '>')
		{
		*new_pos = ptr;
		
		strcpy(temp_out, "\n");
		strcat(temp_out, temp_src);
		strcat(temp_out, "\n");
		strcat(temp_out, temp_alt);
		strcat(temp_out, "\n");

		return( strsave(temp_out) );
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if(! in_arg_flag)
		{
		if(strncasecmp(ptr, "SRC", 3) == 0)
			{
			mode = IN_SRC;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALT", 3) == 0)
			{
			mode = IN_ALT;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "ALIGN", 5) == 0)
			{
			mode = IN_ALIGN;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "ISMAP", 5) == 0)
			{
			mode = IN_ISMAP;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "WIDTH", 5) == 0)
			{
			mode = IN_WIDTH;
			ptr += 5;
			continue;
			}
		else if(strncasecmp(ptr, "HEIGHT", 6) == 0)
			{
			mode = IN_HEIGHT;
			ptr += 6;
			continue;
			}
		else if(strncasecmp(ptr, "BORDER", 6) == 0)
			{
			mode = IN_BORDER;
			ptr += 6;
			continue;
			}
		ptr++;
		continue;
		}
	if(in_arg_flag)
		{
		if(mode == IN_SRC)
			{
			temp_src[src_pos] = *ptr;
			temp_src[src_pos + 1] = 0;
			src_pos++;
			}

		if(mode == IN_ALT)
			{
			temp_alt[alt_pos] = *ptr;
			temp_alt[alt_pos + 1] = 0;
			alt_pos++;
			}
		}/* end if in_arg_flag */
	ptr++;
	}/* end while */
}/* end function do_img */


char *do_a(char *position, char **new_pos)
{
char *ptr;
int in_arg_flag;
char temp[1024];
int mode;
int i;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<A", 2) != 0) return(0);
ptr += 2;

mode = EXPECT_SUB;
in_arg_flag = 0;
temp[0] = 0;
while(1)
	{
	if( *ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if( *ptr == '>')
		{
		*new_pos = ptr;
		
		return( strsave(temp) );
		}

	if( (*ptr == '<') || (isspace(*ptr) ) )
		{
		ptr++;
		continue;
		}

	if(*ptr == '"')
		{
		in_arg_flag = 1 - in_arg_flag;	
		ptr++;
		continue;
		}

	if( (mode != IN_HREF) && (! in_arg_flag) )
		{
		if(strncasecmp(ptr, "HREF", 4) == 0)
			{
			mode = IN_HREF;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "NAME", 4) == 0)
			{
			mode = IN_NAME;
			ptr += 4;
			continue;
			}
		else if(strncasecmp(ptr, "URN", 3) == 0)
			{
			mode = IN_URN;
			ptr += 3;
			continue;
			}
		else if(strncasecmp(ptr, "REL", 3) == 0)
			{
			mode = IN_REL;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "REV", 3) == 0)
			{
			mode = IN_REV;
			ptr += 3;
			continue;
			}		
		else if(strncasecmp(ptr, "TITLE", 5) == 0)
			{
			mode = IN_TITLE;
			ptr += 5;
			continue;
			}		
		else if(strncasecmp(ptr, "METHODS", 7) == 0)
			{
			mode = IN_METHODS;
			ptr += 7;
			continue;
			}		
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
		}
	ptr++;
	}/* end while */
}/* end function do_a */


char *do_quoted_char(char *position, char **new_pos)
{
char *ptr;
int c;
int ignore_flag;
int i;
int s;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if( *ptr != '&') return(0);
if( *(ptr + 1) == 0) return(0);

if( *(ptr + 1) != '#')
	{
	for(i = 0; i < QUOTE_SIZE; i++)
		{
		s = strlen(quote_tab[i].name);
		if( *(ptr + 1 + s) == ';')
			{
			if(strncasecmp(quote_tab[i].name, ptr + 1, s) == 0)
				{
				*new_pos = ptr + s + 1;		

				ptr = malloc(2);
				if(! ptr) return(0);
				sprintf(ptr, "%c", quote_tab[i].value);
				return(ptr);
				}
			}
		}/* end for all entries in qouote_tab */ 

	/* return not a valid quote */
	return(0);
	}/* end not &# */


if( *(ptr + 1) == '#')
	{
	if( 1 != sscanf(ptr + 2, "%d", &c) ) return(0);
	if(c <= 127) ignore_flag = 0;
	else ignore_flag = 1;
	}
else
	{
	ignore_flag = 1;
	}

ptr += 2;

while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	if(*ptr == ';')
		{
		*new_pos = ptr;
		
		if(ignore_flag)
			{
			ptr = malloc(1);
			*ptr = 0;
			}
		else
			{
			ptr = malloc(2);
			sprintf(ptr, "%c", c);
			}
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_quoted_char */


char *dequote(char *text)
{
char *position;
char *ptr1;
char *outptr;
char *new_pos;
char *ptr2;
char *out_space;

if(! text) return(0);

/* the dequoted will be shorter */
out_space = malloc(strlen(text) + 1);

outptr = out_space;
position = text;
while(1)
	{
	if( *position == '&')
		{
		ptr1 = do_quoted_char(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);
			}/* end do_quoted_char */

		position++;
		continue;
		}/* end if & */
	
	*outptr = *position;
	/* also copy string termination */
	if( *position == 0) break;

	position++;
	outptr++;
	}/* end while */

return(out_space);
}/* end function dequote */
	

char *do_pre_on(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<PRE", 4) != 0) return(0);
ptr += 4;

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
		
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_pre_on */


char *do_pre_off(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "</PRE", 5) != 0) return(0);
ptr += 5;

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
		
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_pre_off */


char *do_center_on(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<CENTER", 7) != 0) return(0);
ptr += 7;

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
		
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_center_on */


char *do_center_off(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "</CENTER", 8) != 0) return(0);
ptr += 8;

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
		
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_center_off */


char *do_title_on(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<TITLE", 6) != 0) return(0);
ptr += 6;

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
		
		ptr = strsave("*Document title: ");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_title_on */


char *do_title_off(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "</TITLE", 7) != 0) return(0);
ptr += 7;

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
		
		ptr = strsave("*\n");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_title_off */


char *do_table_end(char *position, char **new_pos)
{
/* table end, only do a LF */
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "</TABLE", 6) != 0) return(0);
ptr += 6;

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
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_table_end */


char *do_tr(char *position, char **new_pos)
{
/* table row start, only do a LF */
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<TR", 3) != 0) return(0);
ptr += 3;

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
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_tr */


char *do_th(char *position, char **new_pos)
{
/* table head start, do nothing */
/* h formatting must be identical to td */
char *ptr;
char temp[TEMP_SIZE];
int start_flag;
int end_flag;
int temp_pos;
int valid_flag;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<TH", 3) != 0) return(0);
ptr += 3;

valid_flag = 0;
temp_pos = 0;
start_flag = 0;
end_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}
	
	if(*ptr == '>')
		{
		if(end_flag)
			{
			*new_pos = ptr;
			if(debug_flag)
				{
				fprintf(stdout, "TH=%s\n", temp);
				}

			return( dequote(temp) );
			}
		
		if(! start_flag) start_flag = 1;
		valid_flag = 1 - valid_flag;

		ptr++;
		continue;
		}/* end if > */
	
	if(*ptr == '<')
		{
		if(strncasecmp(ptr, "</TH", 4) == 0)
			{
			temp[temp_pos] = 0;

			end_flag = 1;
			ptr += 4;
			
			valid_flag = 0;
			continue;
			}
		
		valid_flag = 1 - valid_flag;

		ptr++;
		continue;
		}/* end if < */
		
	if(valid_flag)
		{
		temp[temp_pos] = *ptr;
		temp_pos++;
		}
	ptr++;
	}/* end while */

}/* end function do_th */


char *do_td(char *position, char **new_pos)
{
/* table data start */
char *ptr;
char temp[TEMP_SIZE];
int start_flag;
int end_flag;
int temp_pos;
int valid_flag;
char *ptr1;
char *newp;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<TD", 3) != 0) return(0);
ptr += 3;

temp[0] = 0;
valid_flag = 0;
temp_pos = 0;
start_flag = 0;
end_flag = 0;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}
	
	if(*ptr == '>')
		{
		if(end_flag)
			{
			*new_pos = ptr;
			if(debug_flag)
				{
				fprintf(stdout, "TD=%s\n", temp);
				}

			return( dequote(temp) );
			}
		
		if(! start_flag) start_flag = 1;
		valid_flag = 1 - valid_flag;

		ptr++;
		continue;
		}/* end if > */
	
	if(*ptr == '<')
		{
		if(strncasecmp(ptr, "</TD", 4) == 0)
			{
/*			temp[temp_pos] = 0;*/

			end_flag = 1;
			ptr += 4;
			
			valid_flag = 0;
			continue;
			}
		
		ptr1 = do_img(ptr, &newp);
		ptr = newp;
		if(ptr1)
			{
			strcat(temp, ptr1);
			temp_pos = strlen(temp);
			ptr++;
			
			free(ptr1);
			continue;
			}/* end do_img */

		valid_flag = 1 - valid_flag;

		ptr++;
		continue;
		}/* end if < */
		
	if(valid_flag)
		{
		temp[temp_pos] = *ptr;
		temp[temp_pos + 1] = 0;
		temp_pos++;
		}
	ptr++;
	}/* end while */

}/* end function do_td */


char *do_table_start(char *position, char **new_pos, int center)
{
/* table start, reformat table and return it */

/*
To format the table correctly we insert spaces.
Need to know lenght of all elements in a vertical column to calculate
spaces to be inserted.
*/
char *ptr;
int column;
int row;
char *ptr1;
char *newp;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<TABLE", 6) != 0) return(0);
ptr += 6;

/* clear the table */
delete_all_html_table_entries();

/* look ahead for tr, th, td get size  */

column = 0;
row = 0;
ptr = position;
while(1)
	{
	if(*ptr == 0)
		{
		*new_pos = ptr;
		return(0);
		}

	ptr1 = do_tr(ptr, &newp);
	ptr = newp;
	if(ptr1)
		{
		column = 0;
		row++;

		ptr++;
		continue;
		}/* end do_tr */
		
	ptr1 = do_td(ptr, &newp);
	ptr = newp;
	if(ptr1)
		{
		add_html_table_entry(ptr1, row, column, TABLE_DATA);
		free(ptr1);
		
		column++;
		
		ptr++;
		continue;
		}/* end do_td */
		
	ptr1 = do_th(ptr, &newp);
	ptr = newp;
	if(ptr1)
		{
		add_html_table_entry(ptr1, row, column, TABLE_HEADER);

		column++;
		
		ptr++;
		continue;
		}/* end do_th */
		
	ptr1 = do_table_end(ptr, &newp);
	ptr = newp;
	if(ptr1)
		{
		*new_pos = ptr;

		ptr = print_formatted_html_table(center);
		return(ptr);

		}/* end do_table_end */
		
	/* scip any OTHER tags */
/*
	a = scip_tag(position, &newp);
	ptr = newp;
	if(a)
		{
		ptr++;
		continue;
		}
*/
	ptr++;
	}/* end while */

}/* end function do_table_start */


char *do_p(char *position, char **new_pos)
{
/* paragraph: insert empty line */
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

/* avoid confusion with <PRE */
if(strncasecmp(ptr, "<P>", 3) != 0) return(0);

/* NOT 3 WAIT FOR '>' */
ptr += 2;

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
		ptr = strsave("\n\n");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_p */


char *do_b(char *position, char **new_pos)
{
/* BIG */
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<B", 2) != 0) return(0);
ptr += 2;

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
		ptr = strsave("");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_b */


char *do_br(char *position, char **new_pos)
{
char *ptr;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<BR", 3) != 0) return(0);
ptr += 3;
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
		ptr = strsave("\n");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_br */


char *do_hr(char *position, char **new_pos)
{
char *ptr;
char *ptr2;
int i;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<HR", 3) != 0) return(0);
ptr += 3;

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
		ptr = malloc(hsize + 2);
		if(! ptr) return(0);
		
		ptr2 = ptr;
		*ptr2 = '\n';
		ptr2++;

		for(i = 0; i < hsize; i++)
			{
			*(ptr2 + i) = '-';
			}
		*(ptr2 + i) = 0;
		*(ptr2 + i) = '\n';
		*(ptr2 + i + 1) = 0;
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_hr */


char *do_hx(char *position, char **new_pos)
{
char *ptr;
int c;

*new_pos = position;

if(! position) return(0);
if(*position == 0) return(0);

ptr = position;

if(strncasecmp(ptr, "<H", 2) != 0) return(0);
	{
	if(strlen(ptr) < 3) return(0);
	c = *(ptr + 2);
	if(! isdigit(c) ) return(0);
	if( (c < '1') || (c > '6') ) return(0);	
	}

ptr += 3;

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
		ptr = strsave("\n");
		return(ptr);
		}

	ptr++;
	}/* end while */

}/* end function do_hx */


int http_browser(char *url, int *add_to_history, int mode)
{
/* the add_to_history is a flag, and set if text/html, not on pics */
char temp[READSIZE];	
FILE *load_filefd;
char *space;
char *spaceptr;
struct stat *statptr;
char *out_space;
char *pass2_space;
int bytes_read;
size_t size;
char *position;
char *outptr;
int a;
char *new_pos;
char *ptr1;
char *ptr2;
int title;
int text_length;
int total_spaces;
int leading_spaces;
int trailing_spaces;
int i;
int hpos;
int center;
int pre;
int line_wrapped_flag;
char *content_type;
int display_mode;
char *anchor;

if(debug_flag)
	{
	fprintf(stdout, "http_browser(): arg url=%s mode=%d\n", url, mode);
	}

/* argument check */
if(! url) return(0);

hsize = HOR_CHARS;

anchor = 0;
/* test if allowed */
/*
if(filters_enabled_flag)
	{
	if(pa -> filter_flags & FILTER_DO_NOT_SHOW_BODY)
		{
		fl_set_input(fdui -> article_body_input_field,\
		"Filter: Permission denied");
		return(1);
		}
	}
*/

/* little problem here, if only host or host/ */
if(mode == 0)/* usr */
	{
	ptr1 = strstr(url, "http");
	if(! ptr1) return(0);
	/* point to start host */
	ptr1 += 5;
	/* test if host only */
	ptr2 = strchr(ptr1, '/');
	if(! ptr2)
		{
		sprintf(temp, "%s/index.html", url);
		}
	else
		{
		sprintf(temp, "%s", url);
		/* look for anchor here */
		ptr1 = strstr(temp, "#");
		if(ptr1)
			{
			if(debug_flag)
				{
				fprintf(stdout, "anchor found\n");
				}
			/* cut of #..... */
			*ptr1 = 0;
			if(debug_flag)
				{
				fprintf(stdout, "new temp=%s\n", temp);	
				}
			anchor = strsave(ptr1 + 1);
			if(! anchor) return(0);
			}
		}/* end http not host only */
	}/* end if mode == 0, url */
if(mode == 1)/* article body from newsgroup */
	{
	sprintf(temp, "%s", url);
	}

/*
Figure out the content type, maybe we need a helper for this.
The file url.head is the original header, and should contain.
Content-Type: image/gif
*/
/* keep gcc -Wall happy */
content_type = 0;
if(anchor)
	{
	content_type = strsave("text/html");
	if(! content_type) return(0);
	} 
else
	{
	if(mode == 0)/* url */
		{
		content_type = get_content_type(url);
		if(! content_type) return(0);
		}
	if(mode == 1)/* article body from newsgroup */
		{
		content_type = strsave("text/html");
		if(! content_type) return(0);
		}
	}/* end if not anchor */

if(debug_flag)
	{
	fprintf(stdout,\
	"http_browser(): content_type=%s anchor=%s\n",\
	content_type, anchor);
	}

*add_to_history = 0;
display_mode = 0;
if( strncasecmp(content_type, "text/html", 9) == 0)
	{
	display_mode = DISPLAY_HTML;
	*add_to_history = 1;
	}
else if( strncasecmp(content_type, "text/plain", 9) == 0)
	{
	display_mode = DISPLAY_PLAIN;
	*add_to_history = 1;
	}
else if( strncasecmp(content_type, "application", 11) == 0)
	{
	free(content_type);
	return(0);
	}
else
	{
	a = start_helper(url, content_type);
	free(content_type);
	return(a);
	}

/* content_type now for sure text/html or text/plain */
free(content_type);

/* load file */
load_filefd = fopen(temp, "r");
if(! load_filefd)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"show_article_body(): cannot open %s for read\n", temp);
		}
	fl_show_alert(\
	"http_browser():", "cannot open file for read", temp, 0);

	return(0);
	}

/* clear screen */
fl_set_input(fdui -> article_body_input_field, "");

/* show URL */
sprintf(temp, "%s", url);
fl_set_object_label(fdui -> description_display, temp);

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(load_filefd), statptr);
size = statptr -> st_size;

space = malloc(statptr -> st_size + 1);
free(statptr);
if(! space)
	{
	if(debug_flag)
		{
		fprintf(stdout,\
		"http_browser(): malloc could not allocate space\n");
		}
	return(0);
	}

bytes_read = 0;
spaceptr = space;
while(1)
	{
	errno = 0;
/*
fprintf(stdout, "filenoload_filefd=%d spaceptr=%lu size=%d\n",\
fileno(load_filefd), (long)spaceptr, size);
*/
	/*
	Note that argument size must not cause spaceptr to point to a
	bad address (outsize malloc).
	*/
	bytes_read = read(fileno(load_filefd), spaceptr, size - bytes_read);
	if(bytes_read < 0)/* some error */
		{
		if(errno == EAGAIN) continue;/* try again */
		
		fclose(load_filefd);
		perror("http_browser():");
		
		free(space);
		return(0);
		}/* end if bytes_read < 0 */

	if(bytes_read == 0)/* EOF */
		{
		fclose(load_filefd);
		break;
		}
	
	/* if we have all, ready, no need to call read again */
	if(bytes_read == size)
		{
		fclose(load_filefd);
		spaceptr += bytes_read;
		break;
		}

	spaceptr += bytes_read;
	}/* end while all lines from article body */
*spaceptr = 0;/* string termination */

if(display_mode == DISPLAY_PLAIN)
	{
	fl_set_input(fdui -> article_body_input_field, space);

	/* used by extract url (in article_body_input_field_cb) */
	free(article_body_copy);
	article_body_copy = strsave(space);

	article_body_topline =\
	fl_get_input_topline(fdui -> article_body_input_field);

	/* free the destination */
	free(space);

	/* set scrollbar to top of article */
	fl_set_input_topline(fdui -> article_body_input_field, 1);

	/* show the result */
	show_browser(ARTICLE_BODY_BROWSER);

	/* well, eh, NO */
	/*refresh_screen(selected_group);*/
	/* let the mouse know: not cut and paste but get clicked URLs */
	html_file_shown = 1; /* show_browser will reset this */

	/* set buttons to RELOAD BACK FORWARD */
	set_browser_buttons();

	/* remember the url so we can reload */
	if(reload_url) free(reload_url);
	reload_url = strsave(url);
	if(! reload_url) return(0);

	return(1);
	}

/* ASSUMING out_space < space (getting rid of all the <tag> info) */
out_space = malloc(size);
if(! out_space) return(0);

center = 0;
pre = 0;
hpos = 0;
title = 0;
position = space;
outptr = out_space;
while(1)
	{
	if(! position) break;
	if( *position == 0) break;

	/* ignore CR */
	if( *position == 13)
		{
/*
		position++;
		continue;
*/
		}

/*
	if( *position == '&')
		{
		ptr1 = do_quote(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);
			}
		}

*/
	if(! pre)
		{
		/* replace any LF with spaces (reformat follows later) */
		if( *position == 10)
			{
			*outptr = ' ';
			outptr++;
			position++;
			hpos++;
			continue;
			
			/* if no space preceeds, replace with space */
			if( *(position - 1) != ' ')
				{
				*outptr = ' ';

				hpos++;
/*				position++;*/
				outptr++;
				}
			else /* scip it */
				{
				position++;
				continue;
				}
			}/* end if LF */

		/* reformat */
		if(hpos >= hsize)
			{
			line_wrapped_flag = 0;
			for(i = 0; i < hpos; i++)
				{
				ptr1 = outptr - i;
				if(ptr1 < out_space) break;
				if( *ptr1 == ' ')
					{
					*ptr1 = 10;
					hpos = i;
					line_wrapped_flag = 1;
					break;
					}
				}
			/* if no space found insert '-' and break line */
			if(! line_wrapped_flag)
				{
				*outptr = '-';
				outptr++;
				*outptr = 10;
				outptr++;
				hpos = 0;
				}
		
			}/* end in hpos > hsize*/

		}/* end if not pre (formatted ) */

	if(*position == '<')
		{
/*
fprintf(stdout, "L E F T  %c%c%c%c\n",\
*position, *(position+1), *(position +2), *(position+3));
*/
		/* scip any comments */
		ptr1 = scip_comment(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			position++;
			continue;
			}/* end comment */

		/* get any images (and ALT) */
		ptr1 = do_img(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			*outptr = '\n';
			outptr++;
			
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			*outptr = '\n';
			outptr++;

			position++;
			continue;
			}/* end do_img */

		/* get any anchors */
		ptr1 = do_a(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			*outptr = '\n';
			outptr++;
			
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			*outptr = '\n';
			outptr++;

			position++;
			continue;
			}/* end do_a */

		ptr1 = do_hx(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			hpos = 0;

			position++;
			continue;
			}/* end do_hx */
		
		ptr1 = do_hr(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			hpos = 0;
			
			position++;
			continue;
			}/* end do_hr */
		
		ptr1 = do_table_start(position, &new_pos, center);
		position = new_pos;
		if(ptr1)
			{
			/*
			Undo center for the tabe if it was set,
			center is handled within table.
			*/
			if(center)
				{
				*outptr = 2;
				outptr++;
				}
				
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			hpos = 0;

			/* restore center mode if it was set */
			if(center)
				{
				*outptr = 1;
				outptr++;
				}

			position++;
			continue;
			}/* end do_table_start */
		
		ptr1 = do_p(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			hpos = 0;

			position++;
			continue;
			}/* end do_p */
		
		ptr1 = do_br(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			hpos = 0;

			position++;
			continue;
			}/* end do_br */
		
		/* note that this must come after do_br() */
		ptr1 = do_b(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			position++;
			continue;
			}/* end do_b */
		
		ptr1 = do_center_on(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			*outptr = 1;
			outptr++;
			
			center = 1;

			position++;
			continue;
			}/* end do_center_on */
		
		ptr1 = do_center_off(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			*outptr = 2;
			outptr++;

			center = 0;

			position++;
			continue;
			}/* end do_center_off */

		ptr1 = do_pre_on(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			pre = 1;
			
			position++;
			continue;
			}/* end do_pre_on */
		
		ptr1 = do_pre_off(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			pre = 0;
			
			position++;
			continue;
			}/* end do_pre_off */

		ptr1 = do_title_on(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			title = 1;
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			position++;
			continue;
			}/* end do_title_on */
		
		ptr1 = do_title_off(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
			title = 0;
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			position++;
			continue;
			}/* end do_title_off */

		/* scip any OTHER tags */
		a = scip_tag(position, &new_pos);
		position = new_pos;
		if(a)
			{
			position++;
			continue;
			}
		}/* end if '<' */

	if( *position == '&')
		{
		ptr1 = do_quoted_char(position, &new_pos);
		position = new_pos;
		if(ptr1)
			{
/*			title = 0;*/
			ptr2 = ptr1;
			while(1)
				{
				if(*ptr2 == 0) break;
				*outptr = *ptr2;
				outptr++;
				ptr2++;
				}
			free(ptr1);

			position++;
			continue;
			}/* end do_quoted_char */
		}/* end if & */

	hpos++;
	*outptr = *position;
	position++;
	outptr++;
	}/* end while all characters in space */
*outptr = 0;

/* free the source */
/*free(space);*/

/* PASS 2 */

/* ASSUMING pass2_space < space (getting rid of all the <tag> info) */
/*
DO NOT ASSUME, ADDING TO MANY CHARACTERS (leading spaces).
*/
pass2_space = malloc(size * 2);
if(! pass2_space) return(0);

/*
fprintf(stdout, "%s", out_space);
return(1);
*/

center = 0;
position = out_space;
outptr = pass2_space;
while(1)
	{
	if(! position) break;
	if( *position == 0) break;

	/* test for start line */
	if( *position == 10)
		{
		hpos = 0;
		}

	if(*position == 1)
		{
		center = 1;
		position++;
		continue;
		}
	if(*position == 2)
		{
		center = 0;
		position++;
		continue;
		}

	/* test for center token */
	if( *position == 10)
		{
		*outptr = 10;
		outptr++;
		position++;

		if(center)
			{
			/* look ahead for length until LF */
			ptr1 = position;
			text_length = 0;
			while(1)
				{
				if(! ptr1) break;
				if( *ptr1 == 0) break;
				if( *ptr1 == 10)
					{
					break;
					}
				text_length++;
				ptr1++;
				}
			total_spaces = hsize - text_length;
			trailing_spaces = total_spaces / 2;
			leading_spaces = total_spaces - trailing_spaces;
			for(i = 0; i < leading_spaces; i++)
				{
				*outptr = ' ';
				outptr++;
				}
			}/* end if center */

		continue;
		}/* end if LF */
	
	*outptr = *position;
	position++;
	outptr++;
	}
*outptr = 0;

free(out_space);

fl_set_input(fdui -> article_body_input_field, pass2_space);

/*fprintf(stdout, "%s\n", pass2_space);*/

/* used by extract url (in article_body_input_field_cb) */
free(article_body_copy);
article_body_copy = strsave(pass2_space);
if( !article_body_copy) return(0);

article_body_topline =\
fl_get_input_topline(fdui -> article_body_input_field);

/* free the destination */
free(pass2_space);

/* reposition topline for match to the anchor */
if(anchor)
	{
	sprintf(temp, "NAME=\042%s", anchor);
	free(anchor);

	a = 0;
	/* 
	space holds the original file, before the anchors were removed,
	just an approximation for the moment.
	*/
	if(! find_anchor_line_number(space, temp, &a) )
		{
		/* big problem */
		return(0);		
		}

	if(debug_flag)
		{
		fprintf(stdout, "anchor_line_number=%d\n", a);
		}

	/* keep same position if not found */
	/* oh well, this will not work*/
	if(a != 0)
		{
		/* set scrollbar to top of article */
		fl_set_input_topline(fdui -> article_body_input_field, a);
		}
	}/* end if anchor */
else
	{
	/* set scrollbar to top of article */
	fl_set_input_topline(fdui -> article_body_input_field, 1);
	}/* end if ! anchor_flag */

/* free the original text */
free(space);

/* show the result */
show_browser(ARTICLE_BODY_BROWSER);

/* well, eh, NO */
/*refresh_screen(selected_group);*/
if(mode == 0)/* if url, not article body  */
	{
	/* let the mouse know: not cut and paste but get clicked URLs */
	html_file_shown = 1; /* show_browser will reset this */

	/* set buttons to RELOAD BACK FORWARD */
	set_browser_buttons();
		
	/* remember the url so we can reload */
	if(reload_url) free(reload_url);
	reload_url = strsave(url);
	if(! reload_url) return(0);
	}

return(1);
}/* end function http_browser */


char *get_content_type(char *url)
{
char *filename;
FILE *fileptr;
char *space;
char *spaceptr;
struct stat *statptr;
int bytes_read;
size_t size;
char *ptr;
char *ptr2;

if(debug_flag)
	{
	fprintf(stdout, "get_content_type(): arg url=%s\n", url);
	}

/* argument check */
if(! url) return(0);

filename = malloc(strlen(url) + strlen(".head") + 1);
if(! filename) return(0);
strcpy(filename, url);
strcat(filename, ".head");

fileptr = fopen(filename, "r");
if(! fileptr)
	{
	fprintf(stdout,\
	"get_content_type(): could not open file %s for read\n",\
	filename);
	free(filename);
	return(0);
	}

free(filename);

statptr = (struct stat*) malloc(sizeof(struct stat) );
if(! statptr) return(0);

fstat(fileno(fileptr), statptr);
size = statptr -> st_size;

space = malloc(size + 1);
free(statptr);
if(! space) return(0);

bytes_read = 0;
spaceptr = space;
while(1)
	{
	errno = 0;
/*
fprintf(stdout, "filenoload_filefd=%d spaceptr=%lu size=%d\n",\
fileno(load_filefd), (long)spaceptr, size);
*/
	/*
	Note that argument size must not cause spaceptr to point to a
	bad address (outsize malloc).
	*/
	bytes_read = read(fileno(fileptr), spaceptr, size - bytes_read);
	if(bytes_read < 0)/* some error */
		{
		if(errno == EAGAIN) continue;/* try again */
		
		fclose(fileptr);
		perror("get_content_type():");
		
		free(space);
		return(0);
		}/* end if bytes_read < 0 */

	if(bytes_read == 0)/* EOF */
		{
		fclose(fileptr);
		break;
		}
	
	/* if we have all, ready, no need to call read again */
	if(bytes_read == size)
		{
		fclose(fileptr);
		spaceptr += bytes_read;
		break;
		}

	spaceptr += bytes_read;
	}/* end while */
*spaceptr = 0;/* string termination */

if(debug_flag)
	{
	fprintf(stdout, "get_content_type(): space=%s\n", space);
	}

ptr = strcasestr(space, "Content-Type: ");
if(! ptr)
	{
	free(space);
	return(0);
	}

ptr2 = strsave(ptr + 14);

/* cut at LF */
ptr = strchr(ptr2, 10);
if(ptr) *ptr = 0;

/* cut at CR */
ptr = strchr(ptr2, 13);
if(ptr) *ptr = 0;

free(space);
return(ptr2);
}/* end function get_content_type */


int start_helper(char *url, char *content_type_and_subtype)
{
int a, b, c, i, j, k;
char temp[TEMP_SIZE];
char report[TEMP_SIZE];
pid_t pid;
char content_type[TEMP_SIZE];
char content_subtype[TEMP_SIZE];
char helper_type[TEMP_SIZE];
char helper_subtypes[TEMP_SIZE];
char helper_program[TEMP_SIZE];
char helper_flags_start[TEMP_SIZE];
char helper_flags[TEMP_SIZE];
int lower_case_flag;
char execv_args[51][TEMP_SIZE];
char *flip[51];/* this way I do not have to free anything */
				/* arguments to ececlv are copied */
char *ptr;
int go_by_extention_flag = 0;

if(debug_flag)
	{
	fprintf(stdout,\
	"start_helper(): arg url=%s content_type_and_subtype=%s\n",\
	url, content_type_and_subtype);
	}

/* argument check */
if(! url) return(0);
if(! content_type_and_subtype) return(0);

/* keep gcc -Wall happy */
a = 0;

/* extract the content_subtype */
ptr = strstr(content_type_and_subtype, "/");
if(ptr)
	{
	strcpy(content_subtype, ptr + 1);
	strcpy(content_type, content_type_and_subtype);
	/* cut of the /subtype */	
	ptr = strchr(content_type, '/');
	if(! ptr) return(0);
	*ptr = 0;
	}		
else /* not a usable content_type, set for text display */ 
	{
	strcpy(content_type, "text");
	strcpy(content_subtype, "plain");
	}

if(strcmp(content_type, "text") == 0)
	{
 	}
else if(strcmp(content_type, "image") == 0)
	{
	}
else if(strcmp(content_type, "video") == 0)
	{
	}
else if(strcmp(content_type, "audio") == 0)
	{
	}
else if(strcmp(content_type, "application") == 0)
	{
	}
else
	{
	fl_show_alert("Unknow content type", content_type, "", 0);
	return(0);
	}	


/*
select a helper program for this helper_type based on the content_subtype.
*/
lower_case_flag = 0;
while(1) /* try for normal and lower case of content_subtype */
	{
	/* get all helper programs of this type */
	b = 0;
	for(i = 0; i < 64; i++)
		{
		if(! get_helpers("helper", temp, i) ) break;
		a = sscanf(temp, "%s %s %s %s",\
		helper_type, helper_subtypes, helper_program, helper_flags_start);
		
		if(! go_by_extention_flag)
			{
			/* check if helper type matches content type */
			if(strcmp(helper_type, content_type) != 0)
				{
				continue;
				}
			}
	
		/* check the subtypes */
		if(strstr(helper_subtypes, content_subtype) != 0)
			{
			b = 1;
			/* get the arguments (flags) for the helper application */
			helper_flags[0] = 0;
			if(a == 4)
				{
				ptr = (char *) strstr(temp, helper_flags_start);
				if(ptr) strcpy(helper_flags, ptr);
				}
			break;
			}
		}/* end for all helper programs */
	if(b) break;/* match */
			
	if(! lower_case_flag)
		{
		/* content subtype to lower case (in may have been upper case) */
		i = 0;
		while(1)
			{
			c = content_subtype[i];
			content_subtype[i] = tolower(c);
			if(c == 0) break;
			i++;
			}
		lower_case_flag = 1;
		if(debug_flag)
			{
			fprintf(stdout,\
			"Changing to lower case for content_subtype=%s\n",\
			content_subtype);
			}
		/* try again, now with lower case */
		continue;
		}/* end not lower_case_flag */
	else
		{
		if(debug_flag)
			{
			fprintf(stdout, "\nCannot read any viewer from setupfile\n");
			}
		fl_show_alert(\
		"No apropriate viewer found in setupfile for type",\
		content_subtype, "", 0);
		return(0);
		}/* end if lower_case_flag */
	}/* end while normal and lower case for content_type_subtype */

if(debug_flag)
	{
	fprintf(stdout,\
	"start_helper(): a=%d helper_program=%s helper_flags=%s\n",\
	a, helper_program, helper_flags);
	}

/* start the helper program for this file */

/* fill in the multidimensional argument array for execv() */
/* the first one is the helper program its self */
strcpy(execv_args[0], helper_program);
k = 0;
i = 1;
while(1)
	{
	if(helper_flags[k] == ' ')
		{
		k++;
		continue;
		}
	j = 0;
	while(1)
		{
		c = helper_flags[k];
		if(c == ' ') c = 0;
		execv_args[i][j] = c; 
		if(c == 0) break;
		j++;
		k++;
		}
	i++;
	if(helper_flags[k] == 0) break;
	}
execv_args[i][0] = 0;

/*
the reason I am using char *flip[] is that I cannot assign a NULL to
execv_args[i].
The reason I copy the (temporary stack variable) execv_args[][] into a *ptr[]
is that, by not using strsave(), I do not have to free later.
ececlv() will copy its arguments, flip[] may be destroyed after the call to
execlv().
*/
i = 0;
while(1)
	{
	flip[i] = execv_args[i];	
	if(execv_args[i][0] == 0)
		{
		flip[i] = url;
/*		flip[i] = temp;*/
		flip[i + 1] = NULL;
		break;
		}
	i++;
	}

if(debug_flag)
	{
	for(i = 0; execv_args[i][0] != 0; i++)
		{
		fprintf(stdout, "i=%d execv_args[i]=%s flip[i]=%s\n",\
		i, execv_args[i], flip[i]);
		}
	}

/* report to user */
sprintf(report, "Starting helper program %s %s",helper_program, url);
my_command_status_display(report);

/* fork */
pid = fork();
if(pid == 0)/* in child */
	{
	/* 
	start the helper program
	NOTE: helper program  must be in the path or in the current directory.
	*/

	/* a NULL pointer is required as the last argument */
	a = execvp(helper_program, flip );
	if(a < 0)
		{
		if(debug_flag)
			{
			fprintf(stdout,\
	"start_helper(): \nCannot start helper program execvp failed: %s %s",\
			helper_program, url);
			perror("start_helper():");
			}
		return(0);
		}
	}
else if(pid < 0)/* fork failed */
	{
	fl_show_alert("Helper program fork failed", "", "", 0);
	return(0);
	}
else/* parent */
	{	
	}

return(1);
}/* end function start_helper */


int set_browser_buttons()
{
if(debug_flag)
	{
	fprintf(stdout,\
	"set_browser_buttons(): arg none, using html_file_shown=%d\n",\
	html_file_shown);
	}

if(html_file_shown)
	{
	fl_set_object_color(fdui -> view_article_header_button,\
	FL_YELLOW, FL_WHITE);
	fl_set_object_label(fdui -> view_article_header_button,\
	"RELOAD");

	fl_set_object_color(fdui -> article_body_button,\
	FL_YELLOW, FL_WHITE);
	fl_set_object_label(fdui -> article_body_button,\
	"RELOAD");

	fl_set_object_color(fdui -> next_unread_button,\
	FL_YELLOW, FL_WHITE);
	fl_set_object_label(fdui -> next_unread_button, "BACK");

	fl_set_object_color(fdui -> next_thread_button,\
	FL_YELLOW, FL_WHITE);	
	fl_set_object_label(fdui -> next_thread_button, "FORWARD");

	fl_set_object_color(fdui -> launche_button,\
	FL_YELLOW, FL_WHITE);	
	fl_set_object_label(fdui -> launche_button,\
	"ADD URL");
	}
else
	{
	fl_set_object_color(fdui -> view_article_header_button,\
	FL_COL1, FL_COL1);
	fl_set_object_label(fdui -> view_article_header_button,\
	"VIEW HEADER");

	fl_set_object_color(fdui -> article_body_button,\
	FL_COL1, FL_COL1);
	fl_set_object_label(fdui -> article_body_button,\
	"ARTICLE BODY");

	fl_set_object_color(fdui -> next_unread_button,\
	FL_COL1, FL_COL1);
	fl_set_object_label(fdui -> next_unread_button,\
	"NEXT UNREAD");

	fl_set_object_color(fdui -> next_thread_button,\
	FL_COL1, FL_COL1);	
	fl_set_object_label(fdui -> next_thread_button,\
	"ON SUBJECT");

	fl_set_object_color(fdui -> launche_button,\
	FL_COL1, FL_COL1);	
	fl_set_object_label(fdui -> launche_button,\
	"LAUNCH");
	}

return(1);
}/* end function set_browser_buttons */


int http_browser_reload()
{
int a;
char *ptr;
int add_to_history;

if(debug_flag)
	{
	fprintf(stdout,\
	"http_browser_reload(): arg none, using reload_url=%s\n",\
	reload_url);
	}

/* argument check */
if(! reload_url) return(0);

/*
NOTE: cannot use reload_url as argument to http_browser(), because
http_browser uses it to set a new reload_url, freeing the old one first,
which is this one !!!
*/
ptr = strsave(reload_url);
if(! ptr) return(0);

/* not using add to history, maybe already there */
a = http_browser(ptr, &add_to_history, 0);
free(ptr);

return(a);
}/* end function http_browser_reload */


int find_anchor_line_number(char *space, char *anchor, int *line)
{
char *ptr, *match;

if(debug_flag)
	{
	fprintf(stdout,\
	"find_anchor_line_number(): arg space=%s anchor=%s\n",\
	space, anchor);
	}

/* argument check */
if(! space) return(0);
if(! anchor) return(0);

match = strcasestr(space, anchor);
if(! match) return(0);

ptr = space;
*line = 0;
while(1)
	{
	if(*ptr == 0) break;
	if(*ptr == 10)
		{
		(*line)++;
		}
	if(ptr >= match) break;
	ptr++;
	}

if(debug_flag)
	{
	fprintf(stdout, "*line=%d\n", *line);
	fprintf(stdout, "space=%ld match=%ld\n", (long) space, (long) match);
	}

return(1);
}/* end function find_anchor_line_number */


