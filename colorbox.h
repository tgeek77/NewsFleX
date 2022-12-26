#define FL_COLBOX        (FL_USER_CLASS_START+1)
                           /* 1001 <= Class number <= 9999 */

#define FL_NORMAL_COLBOX 0     /* The only type */

extern FL_OBJECT *fl_create_colbox(int, FL_Coord, FL_Coord, FL_Coord, 
                                      FL_Coord, const char *);
extern FL_OBJECT *fl_add_colbox(int, FL_Coord, FL_Coord, FL_Coord, 
                                   FL_Coord, const char *);
extern int fl_get_colorbox(FL_OBJECT *);
