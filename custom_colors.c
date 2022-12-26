/* 
NewsFleX offline NNTP news reader
NewsFleX is registered Copyright (C) 1997 <Jan Mourer>
mail: pante@pi.net
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


int load_custom_colors()
{
char temp[TEMP_SIZE];
char line_text[TEMP_SIZE];
FILE *color_fptr;
int a;
char color_name[512];
int color;
int red, green, blue;
int line;

if(debug_flag)
	{
	fprintf(stdout, "load_custom_colors(): arg none\n");
	}

sprintf(temp, "%s/.NewsFleX/setup/colors", home_dir);
color_fptr = fopen(temp, "r");
if(! color_fptr)
	{
	fprintf(stdout, "cannot load custom colors file %s\n", temp);
	/* no error, use defaults */
	return(1);
	}

color = 0;/* keep gcc -Wall happy */
line = 1;
while(1)
	{
	a = readline(color_fptr, line_text);
	if(a == EOF) break;/* readline() closes file */

	/* ignore empty lines */
	if(line_text[0] == 0)
		{
		line++;
		continue;
		}
	
	/* scip comments (lines starting with '#') */
	if(line_text[0] == '#')
		{
		line++;
		continue;
		}

	a = sscanf(line_text, "%s %d %d %d", color_name, &red, &green, &blue);
	if( (a < 4) ||\
	(red < 0) || (red > 255) ||\
	(green < 0) || (green > 255) ||\
	(blue < 0) || (blue > 255) )
		{
		fprintf(stdout, "Format error in file %s line %d line reads %s\n",\
		temp, line, line_text); 
		break;
		}

	/* think of some nice names */
	if(strcasecmp(color_name, "FL_RIGHT_BCOL") == 0) color = FL_RIGHT_BCOL;
	else if(strcasecmp(color_name, "FL_BLACK") == 0) color = FL_BLACK;
	else if(strcasecmp(color_name, "FL_RED") == 0) color = FL_RED;
	else if(strcasecmp(color_name, "FL_GREEN") == 0) color = FL_GREEN;
	else if(strcasecmp(color_name, "FL_YELLOW") == 0) color = FL_YELLOW;
	else if(strcasecmp(color_name, "FL_BLUE") == 0) color = FL_BLUE;
	else if(strcasecmp(color_name, "FL_CYAN") == 0) color = FL_CYAN;
	else if(strcasecmp(color_name, "FL_MAGENTA") == 0) color = FL_MAGENTA;
	else if(strcasecmp(color_name, "FL_WHITE") == 0) color = FL_WHITE;
	else if(strcasecmp(color_name, "FL_COL1") == 0) color = FL_COL1;
	else if(strcasecmp(color_name, "FL_MCOL") == 0) color = FL_MCOL;
	else if(strcasecmp(color_name, "FL_TOP_BCOL") == 0) color = FL_TOP_BCOL;
	else if(strcasecmp(color_name, "FL_BOTTOM_BCOL") == 0) color = FL_BOTTOM_BCOL;
	else if(strcasecmp(color_name, "FL_RIGHT_BCOL") == 0) color = FL_RIGHT_BCOL;
	else if(strcasecmp(color_name, "FL_LEFT_BCOL") == 0) color = FL_LEFT_BCOL;
	else if(strcasecmp(color_name, "FL_INACTIVE_COL") == 0) color = FL_INACTIVE_COL;
	else if(strcasecmp(color_name, "FL_TOMATO") == 0) color = FL_TOMATO;
	else if(strcasecmp(color_name, "FL_INDIANRED") == 0) color = FL_INDIANRED;
	else if(strcasecmp(color_name, "FL_SLATEBLUE") == 0) color = FL_SLATEBLUE;
	else if(strcasecmp(color_name, "FL_DARKGOLD") == 0) color = FL_DARKGOLD;
	else if(strcasecmp(color_name, "FL_PALEGREEN") == 0) color = FL_PALEGREEN;
	else if(strcasecmp(color_name, "FL_ORCHID") == 0) color = FL_ORCHID;
	else if(strcasecmp(color_name, "FL_DARKCYAN") == 0) color = FL_DARKCYAN;
	else if(strcasecmp(color_name, "FL_DARKTOMATO") == 0) color = FL_DARKTOMATO;
	else if(strcasecmp(color_name, "FL_WHEAT") == 0) color = FL_WHEAT;
	else if(strcasecmp(color_name, "FL_FREE_COL1") == 0) color = FL_FREE_COL1;
	else
		{
		fprintf(stdout,\
		"Format error unknown color %s ignored in file %s line %d\n",\
		color_name, temp, line);
		line++;
		continue;
		}
	fl_set_icm_color(color, red, green, blue);
	line++;
	}/* end while read all lines in colors */
/*
fl_set_icm_color(FL_COL1, 255, 185, 15);
fl_set_icm_color(FL_INACTIVE_COL, 255, 185, 15);
fl_set_icm_color(FL_MCOL, 255, 185, 15);
fl_set_icm_color(FL_LEFT_BCOL, 255, 185, 15);
fl_set_icm_color(FL_RIGHT_BCOL, 255, 185, 15);
*/

return(1);
}/* end function load custom_colors */

