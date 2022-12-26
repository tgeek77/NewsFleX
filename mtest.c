#include <stdio.h>
#include <strings.h>

int debug_flag;
int replace_in( char *instr, char *namestr, char *defstr, char **resstr);
char *expand_posting_macros (char *text);

char *strsave(char *s) /* save char array s somewhere */
{
char *p;

p = (char *) malloc(strlen(s) +  1);
if(p) strcpy(p, s);
return(p);
}/* end function strsave */



main(int argc, char ** argv)
{
char temp[655350];
char *ptr;
int c, i;

debug_flag = 1;

i = 0;
while(1)
	{
	c = getc(stdin);
	if (c == EOF) break;
	if(c == 26) break;
	temp[i] = c;
	i++;
	}
temp[i] = 0;

ptr = expand_posting_macros (temp);
printf("The expanded text=\n%s\n", ptr);
free(ptr);

return 0;
}



char *expand_posting_macros (char *text)
{
char *ptr;
int current_macro;
char *macro_name;
char *macro_def;
char *in_space;
static char *macros[] = {"$DATE", "$NAME", NULL};

if(debug_flag)
	{
	fprintf (stdout, "expand_posting_macros(): arg text=%s\n", text);
	}

/* start with text */
in_space = strsave(text);
if(! in_space) return 0;

macro_def = 0;/* for -Wall */

current_macro = 0;
/* for all macros */
while(1)
	{
	/* point to current macro */
	macro_name = macros [current_macro]; 
	if (! macro_name) break;

	switch (current_macro)
		{
		case 0:/* time */
			macro_def = strsave("flip");
			break;
		case 1:/* name */
			macro_def = strsave("flap");
			break;
		}

	while(1)
		{
		if (! replace_in(in_space, macro_name, macro_def, &ptr) )
			{
			return 0;
			}
		if(! ptr) break;

		free (in_space);
		in_space = ptr;
		}
		
	/* next macro */
	free(macro_def);

	current_macro++;
	}/* end for al macros */

return in_space;
}/* end function expand_macros */


int replace_in( char *instr, char *namestr, char *defstr, char **resstr)
{
/*
replaces FIRST namestr by defstr in instr, return a new pointer
resstr is 0 if not found
return 1 for success, 0 for error.
*/
char *outstr;
char *inptr;
char *nameptr;
int c;
char *aptr, *bptr;

if(debug_flag)
	{
	fprintf(stdout,
	"replace_in(): arg instr=%s\nnamestr=%s defstr=%s\n",\
	instr, namestr, defstr);	
	}

*resstr = 0;

inptr = (char *) strstr (instr, namestr);
if(! inptr) return 1;

nameptr = namestr;

/*if (strncmp (inptr, namestr, strlen(namestr) ) != 0) return 0;*/

aptr = inptr;
bptr = namestr;
while(1)
	{
	if(! *aptr) break;
	if(! *bptr) break;
	if( *aptr == ' ') return 0;
	if( *aptr != *bptr) return 0;
	aptr++;
	bptr++;	
	}

/* create new space with the correct size */
outstr = \
(char *)malloc ( strlen(instr) + strlen(defstr) - strlen(namestr) + 1);
if (! outstr) return 0;

c = *inptr;
*inptr = 0;
		
strcpy (outstr, instr);
strcat (outstr, defstr);
		
*inptr = c;
strcat (outstr, inptr + strlen (namestr) );

*resstr = outstr;
return 1;
} /* end function replace_in */

