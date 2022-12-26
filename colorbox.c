#include "NewsFleX.h"

extern FD_NewsFleX *fd_NewsFleX;   
extern FD_NewsFleX *fdui;


typedef struct { int counter; char *text;} SPEC;
/* no. of times pushed, text in colbox */

static int hpos, vpos, cupos, ihpos, ivpos, width, height;

static int handle_colbox(FL_OBJECT *ob,\
	int event, FL_Coord mx, FL_Coord my, int key, void *xev)
{
char *temp;
char ttt[2];

switch (event)
	{
	case FL_KEYBOARD:
fprintf(stdout, "KEY=%d (%c)\n", key, key);
	
		if(((SPEC *)ob->spec) -> text)
			{
			temp = malloc(strlen(((SPEC *)ob->spec) -> text) + 2);
			if(! temp) return 0;

			strcpy(temp, ((SPEC *)ob->spec) -> text);
			free(((SPEC *)ob->spec) -> text);
			}
		else
			{
			temp = malloc(1);
			if(! temp) return 0;

			strcpy(temp, "");
			}

		/* convert ENTER key (13) to LF (10) for correct display */
		if(key == 13) ttt[0] = 10;
		else ttt[0] = key;
		ttt[1] = 0;
		strcat(temp, ttt);
		((SPEC *)ob->spec) -> text = temp;

/*
		fl_drw_text_cursor(FL_ALIGN_INSIDE, hpos, vpos,\
		10, 10, FL_COL1, FL_NORMAL_STYLE, FL_LARGE_SIZE,\
		ttt, FL_BLUE, 1);
*/
		fl_drw_text_cursor(FL_ALIGN_TOP_LEFT, ihpos, ivpos,\
		10, 10, FL_COL1, FL_FIXED_STYLE, FL_LARGE_SIZE,\
		((SPEC *)ob->spec) -> text, FL_BLUE, 1);

		hpos += 10;

		/* test for end of line */
fprintf(stdout, "WIDTH=%d HPOS=%d\n", width, hpos);

		if(hpos > (width - 5 -  5) )
			{
/* do the wrap here */
			if( ((SPEC *)ob->spec) -> text )
				{
				temp = malloc(strlen(((SPEC *)ob->spec) -> text) + 2);
				if(! temp) return 0;

				strcpy(temp, ((SPEC *)ob->spec) -> text);
				free(((SPEC *)ob->spec) -> text);

				strcat(temp, "\n");			
				((SPEC *)ob->spec) -> text = temp;

				hpos = ihpos;
				vpos += 10;
				}
			else
				{
				return 0;
				}
			}/* end if hpos > line length */
/*
   void fl_drw_text_cursor(int align, FL_Coord x, FL_Coord y, 
            FL_Coord w, FL_Coord h, FL_COLOR c, int style, int size,
            char *str, int FL_COLOR cucol, int cupos)			

  void fl_get_align_xy(int align, int x, int y, int w, int h,
                        int obj_xsize, int obj_ysize, 
                        int xmargin, int ymargin, int *xpos, int *ypos)
*/
			return 0;
			break;
	case FL_DRAW:	/* Draw box */
		if (ob->pushed)
			fl_drw_box(ob->boxtype,ob->x,ob->y,ob->w,ob->h,ob->col2,ob->bw);
		else
			fl_drw_box(ob->boxtype,ob->x,ob->y,ob->w,ob->h,ob->col1,ob->bw);
		/* fall through */
	case FL_DRAWLABEL: /* Draw label */
		fl_draw_object_label(ob);
		return 0;
	case FL_PUSH:
		((SPEC *)ob->spec)->counter++;
		fl_redraw_object(ob);
		return 0;
	case FL_RELEASE:
		fl_redraw_object(ob);
		return 1;
	case FL_FREEMEM:
		fl_free(ob->spec);
		return 0;
	}/* end switch */
return 0;
}


FL_OBJECT *fl_create_colbox(int type, FL_Coord x, FL_Coord y, 
                               FL_Coord w, FL_Coord h, const char *label)
{
FL_OBJECT *ob;

/* create a generic object class with an appropriate ID */
ob = fl_make_object(FL_COLBOX, type, x, y, w, h, label, handle_colbox);

/* initialize some members */
ob->col1 = FL_RED; 
ob->col2 = FL_BLUE; 
ob -> input = 1; /* receive key input */	
ob -> wantkey = FL_KEY_TAB; /* tabs + cursor direction keys + return */

/* create class specific structures and initialize */
ob->spec = fl_calloc(1, sizeof(SPEC));
return ob;
}



FL_OBJECT *fl_add_colbox(int type, FL_Coord x, FL_Coord y,\
	FL_Coord w,  FL_Coord h, const char *label)
{
FL_OBJECT *ob;

ihpos = x + 5;
ivpos = y + 5;
hpos = ihpos;
vpos = ivpos;
width = w;
height = h;

ob = fl_create_colbox(type,x,y,w,h,label);
fl_add_object(fl_current_form,ob);
return ob;
}


int fl_get_colbox(FL_OBJECT *ob)
   {
       if(!ob || ob->objclass != FL_COLBOX)
       {
          fprintf(stderr, "get_colbox: Bad argument or wrong type");
          return 0;
       }
       return ((SPEC *)ob->spec)->counter;
   }


char *fl_get_colbox_text(FL_OBJECT *ob)
{
if(!ob || ob->objclass != FL_COLBOX)
	{
	fprintf(stderr, "get_colbox: Bad argument or wrong type");
	return 0;
	}

return ((SPEC *)ob->spec) -> text;
}



void posting_color_box_cb(FL_OBJECT *ob, long data)
{
int a;
char *text;

a = fl_get_colbox(ob);

fprintf(stdout, "COLBOX1 %d\n", a);

text = fl_get_colbox_text(ob);
fprintf(stdout, "COLBOX2 %s\n", text); 

}

