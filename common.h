/*
 * $Id: common.h,v 1.1 1999/04/20 06:08:33 jcp Exp $
 *
 * Copyright (C) 1998, John C. Peterson <mailto:jaypee@netcom.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2. A copy is included in this distribution in the file
 * named "LICENSE".
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2, for more details.
 *
 * Note that the version 1.3 release of fplan (and older releases)
 * are the work of Steve Tynor <mailto:tynor@atlanta.twr.com>. Those
 * versions of fplan are in the public domain, (provided the source
 * code headers are left intact). Here is the original header;
 * 
 */

/*
 *----------------------------------------------------------------------------
 *	FPLAN - Flight Planner
 *	Steve Tynor
 *	tynor@prism.gatech.edu
 *
 *	This program is in the public domain. Permission to copy,
 * distribute, modify this program is hearby given as long as this header
 * remains. If you redistribute this program after modifying it, please
 * document your changes so that I do not take the blame (or credit) for
 * those changes.  If you fix bugs or add features, please send me a
 * patch so that I can keep the 'official' version up-to-date.
 *
 *	Bug reports are welcome and I'll make an attempt to fix those
 * that are reported.
 *
 *	USE AT YOUR OWN RISK! I assume no responsibility for any
 * errors in this program, its database or documentation. I will make an
 * effort to fix bugs, but if you crash and burn because, for example,
 * fuel estimates in this program were inaccurate, it's your own fault
 * for trusting somebody else's code! Remember, as PIC, it's _your_
 * responsibility to do complete preflight planning. Use this program as
 * a flight planning aid, but verify its results before using them.
 *----------------------------------------------------------------------------
 */

/* define macros */

#define MAX_NUM_VOR_FIXES	6
#define MAX_NUM_WAYPOINTS	256
#define CACHE_SIZE		MAX_NUM_WAYPOINTS

#define EXIT_GOOD	0
#define EXIT_BAD	1

#define MI_PER_NM	1.1507794	/* statute miles per natutical mile */

typedef int BOOLEAN;

#ifdef TRUE
#undef TRUE
#endif
#define TRUE	((BOOLEAN)1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE	((BOOLEAN)0)

#ifdef PI
#undef PI
#endif
#define PI	((double) 3.1415926535897932384626433832795L)

#ifdef ABS
#undef ABS
#endif
#define ABS(x)	(((x) < 0) ? -(x) : (x))

#ifdef MAX
#undef MAX
#endif
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

#ifdef MIN
#undef MIN
#endif
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

#ifdef ROUND
#undef ROUND
#endif
#define ROUND(x) (((x) < 0.0) ? (-floor (0.5-(x))) : (floor (0.5+(x))))

#ifdef SQR
#undef SQR
#endif
#define SQR(x) ((x) * (x))

#define DEG2RAD(x) ((x)*PI/180.0)
#define RAD2DEG(x) ((x)*180.0/PI)

#define IS_VOR(type)	(((type) == WP_VOR) || ((type) == WP_DME))

#define IS_NAVAID(type)	(((type) == WP_VOR) || ((type) == WP_TAC) || \
			 ((type) == WP_DME) || ((type) == WP_NDB) || \
			 ((type) == WP_LMM) || ((type) == WP_LOM) || \
			 ((type) == WP_ILS))

/* declare data structures */

typedef enum {
   WP_FROM,
   WP_VIA,
   WP_TO
} WP_KIND;

typedef enum {
   WP_INCREMENTAL,
   WP_INTERSECTION,
   WP_LAT_LON,
   WP_RELATIVE,
   WP_AIRPORT,
   WP_VOR,
   WP_TAC,
   WP_DME,
   WP_ILS,
   WP_INT,
   WP_LMM,
   WP_LOM,
   WP_NDB,
   WP_WPT,
   WP_UNK
} WP_TYPE;

typedef struct {
   BOOLEAN valid;
   double value;
} OPT_DOUBLE;

typedef struct {
   WP_TYPE type;
   char *ident;
   char *name;
   char *city;
   OPT_DOUBLE freq;
   OPT_DOUBLE altitude;
   double mag_variation;
   double latitude;		/* fractional degrees - not degrees.min_sec */
   double longitude;   
   char *comment;
} DB_INFO;

typedef enum {
   FROM,
   TO
} FROM_TO;

typedef struct {
   DB_INFO *db;
   BOOLEAN db_valid;
   FROM_TO from_to;
   double radial;
   double distance;
   BOOLEAN fix_valid;
} VOR_FIX;

/*
 * NOTE: all values stored internally in NAUTICAL MILES and KNOTS!
 */
typedef struct {
   WP_KIND kind;
   DB_INFO *db;
   OPT_DOUBLE tc;
   OPT_DOUBLE mc;
   OPT_DOUBLE mh;
   OPT_DOUBLE dist_leg;
   OPT_DOUBLE dist;
   OPT_DOUBLE dist_remain;
   OPT_DOUBLE wind_speed; 
   OPT_DOUBLE wind_direction;		/* stored in degrees true */
   OPT_DOUBLE altitude;
   OPT_DOUBLE tas;
   OPT_DOUBLE egs;
   OPT_DOUBLE eta_leg;
   OPT_DOUBLE eta;
   OPT_DOUBLE fuel_leg;
   OPT_DOUBLE fuel_rate;
   OPT_DOUBLE fuel_extra;
   OPT_DOUBLE fuel_amt;
   BOOLEAN refuel;
   VOR_FIX vor_fix [MAX_NUM_VOR_FIXES];
   char *ident_1;	/* these used only for inc, int, rel waypoints */
   char *ident_2;
   double direction_1;
   double direction_2;
   double distance_1;
   double distance_2;
} WP_INFO;

/* global variables */

extern int max_nav;
extern int num_cached;
extern int num_waypoints;
extern DB_INFO *cache [CACHE_SIZE];
extern WP_INFO waypoints [MAX_NUM_WAYPOINTS];

/* function prototypes */

/* add.c */
extern void add_named_waypoint ( WP_KIND kind,
				 char *ident );
extern void add_inc_waypoint ( WP_KIND kind,
			       double distance,
			       char *name_str,
			       char *city_str,
			       char *comment_str );
extern void add_int_waypoint ( WP_KIND kind,
			       char *ident_1, double mag_direction_1,
			       char *ident_2, double mag_direction_2,
			       char *name_str,
			       char *city_str,
			       char *comment_str );
extern void add_lat_waypoint ( WP_KIND kind,
			       double latitude,
			       double longitude,
			       double mag_variation,
			       char *name_str,
			       char *city_str,
			       char *comment_str );
extern void add_rel_waypoint( WP_KIND kind,
			      char *ident,
			      double mag_direction,
			      double distance,
			      char *name_str,
			      char *city_str,
			      char *comment_str );
extern void set_comment ( char *comment_str );
extern void set_altitude ( double feet );
extern void set_brief ( BOOLEAN on_off );
extern void set_fuel_amt ( double amount );
extern void set_fuel_extra ( double amt );
extern void set_fuel_rate ( double rate );
extern void set_tas ( double tas );
extern void set_wind ( double heading, double speed );
extern void set_xfix ( int vor_num, char *vor_ident );

/* compute.c */
extern void great_circle_course (double latitude_1, double longitude_1,
				 double latitude_2, double longitude_2,
				 double *true_course, double *distance_nmi );
extern void rhumb_line_course ( double latitude_1, double longitude_1,
				double latitude_2, double longitude_2,
				double *true_course, double *distance_nmi );
extern BOOLEAN compute_plan ( BOOLEAN auto_nav1 );

/* db.c */
extern BOOLEAN open_dbs ( void );
extern BOOLEAN close_dbs ( void );
extern BOOLEAN lookup_ident ( WP_KIND kind, char *ident, DB_INFO **db );
extern void put_db_summary ( FILE *out_fp );

/* fp_lex.c */
extern FILE *yyin;
extern int yylineno;
extern double yydval;
extern char yytext[];
extern int yylex ( void );

/* fp_yacc.c */
extern int yyparse ( void );

/* main.c */
extern int yyerror ( char *s );
extern int yyreject ( void );
extern int yywrap ( void );

/* misc.c */
extern void decimal_2_deg_min_sec ( double dec,
                                    int *deg,
                                    int *min,
                                    int *sec );
extern void str_upcase ( char *str );
extern char *str_tok( char *str );
extern char *is_kident ( char *ident );
extern void min_max_lat_lon ( double *min_lat, double *max_lat,
			      double *min_lon, double *max_lon );

/* reverse.c */
extern void print_reverse ( void );

/* preview.c */
#ifdef GFX_GNOME
extern void gnome_draw ( BOOLEAN brief );
#endif
#ifdef GFX_XVIEW
extern void xv_draw ( BOOLEAN brief );
#endif

