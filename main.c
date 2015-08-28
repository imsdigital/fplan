/*
 * $Id: main.c,v 2.21 1999/05/02 05:09:09 jcp Exp $
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

static char rcsid[] = "$Id: main.c,v 2.21 1999/05/02 05:09:09 jcp Exp $";

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef GFX_XVIEW
#include <xview/xview.h>
#endif

#include "common.h"
#include "output.h"
#include "patchlvl.h"

/*
 * if any of the GFX_* options are defined, define GFX
 */
#ifdef GFX_GNOME
#undef GFX
#define GFX
#endif
#ifdef GFX_XVIEW
#undef GFX
#define GFX
#endif
#ifdef GFX_POSTSCRIPT
#undef GFX
#define GFX
#endif

/* global variables */

int max_nav = -1;
int num_cached = 0;
int num_waypoints = 0;
DB_INFO *cache[CACHE_SIZE];
WP_INFO waypoints[MAX_NUM_WAYPOINTS];

/*----------------------------------------------------------------------------*/
static void 
do_lookup (char *ident)
{
   DB_INFO *db1 = (DB_INFO*) NULL;
   DB_INFO *db2 = (DB_INFO*) NULL;

   if (lookup_ident (WP_VIA, ident, &db1))
      put_db_text (stdout, db1);

   if (lookup_ident (WP_FROM, ident, &db2))
      if (!db1 || (db1->type != db2->type))
	 put_db_text (stdout, db2);
}

/*----------------------------------------------------------------------------*/
static void 
init (BOOLEAN brief_mode)
{
   int i;

   set_brief (brief_mode);

   max_nav = -1;
   num_cached = 0;
   num_waypoints = 0;

   waypoints[0].db = (DB_INFO *) NULL;
   waypoints[0].tc.value = 0.0;
   waypoints[0].tc.valid = FALSE;
   waypoints[0].mc.value = 0.0;
   waypoints[0].mc.valid = FALSE;
   waypoints[0].mh.value = 0.0;
   waypoints[0].mh.valid = FALSE;
   waypoints[0].dist_leg.value = 0.0;
   waypoints[0].dist_leg.valid = FALSE;
   waypoints[0].dist.value = 0.0;
   waypoints[0].dist.valid = FALSE;
   waypoints[0].dist_remain.value = 0.0;
   waypoints[0].dist_remain.valid = FALSE;
   waypoints[0].wind_speed.value = 0.0; 
   waypoints[0].wind_speed.valid = FALSE; 
   waypoints[0].wind_direction.value = 0.0;
   waypoints[0].wind_direction.valid = FALSE;
   waypoints[0].altitude.value = 0.0;
   waypoints[0].altitude.valid = FALSE;
   waypoints[0].tas.value = 0.0;
   waypoints[0].tas.valid = FALSE;
   waypoints[0].egs.value = 0.0;
   waypoints[0].egs.valid = FALSE;
   waypoints[0].eta_leg.value = 0.0;
   waypoints[0].eta_leg.valid = FALSE;
   waypoints[0].eta.value = 0.0;
   waypoints[0].eta.valid = FALSE;

   waypoints[0].fuel_leg.value = 0.0;
   waypoints[0].fuel_leg.valid = FALSE;
   waypoints[0].fuel_rate.value = 0.0;
   waypoints[0].fuel_rate.valid = FALSE;
   waypoints[0].fuel_extra.value = 0.0;
   waypoints[0].fuel_extra.valid = FALSE;
   waypoints[0].fuel_amt.value = 0.0;
   waypoints[0].fuel_amt.valid = FALSE;
   waypoints[0].refuel = FALSE;

   for (i = 0; i < MAX_NUM_VOR_FIXES; i++) {
      waypoints[0].vor_fix[i].db = (DB_INFO *) NULL;
      waypoints[0].vor_fix[i].db_valid = FALSE;
      waypoints[0].vor_fix[i].from_to = FROM;
      waypoints[0].vor_fix[i].radial = 0.0;
      waypoints[0].vor_fix[i].distance = 0.0;
      waypoints[0].vor_fix[i].fix_valid = FALSE;
   }

   waypoints[0].ident_1 = (char *)NULL;
   waypoints[0].ident_2 = (char *)NULL;
   waypoints[0].direction_1 = 0.0;
   waypoints[0].direction_2 = 0.0;
   waypoints[0].distance_1 = 0.0;
   waypoints[0].distance_2 = 0.0;
}

/*----------------------------------------------------------------------------*/
static BOOLEAN 
parse_script (void)
{
   return (BOOLEAN) (! yyparse ());
}

/*----------------------------------------------------------------------------*/
static void 
usage (char *progname, int status)
{
   fprintf (stderr, "fplan version: %s\n", VERSION);
   /*
    * LOOKUP mode:
    */
   fprintf (stderr, "usage: %s -l identifier ...\n", progname);
   fprintf (stderr, "\t-l - lookup the identifier(s) in the databases\n");

   /*
    * REVERSE mode:
    */
   fprintf (stderr, "    or\n");
   fprintf (stderr, "       %s -r (-|planfile)\n", progname);
   fprintf (stderr, "\t-r - compute the return trip, prints the reversed plan to stdout.\n");
   fprintf (stderr, "\t-  - read 'planfile' from the standard input.\n");

#ifdef GFX
   /*
    * GRAPHICS mode:
    */
   fprintf (stderr, "    or\n");

#ifdef GFX_GNOME
   fprintf (stderr, "       %s -g (-|planfile)\n", progname);
   fprintf (stderr, "\t-g - preview the route graphically in a GNOME window\n");
#endif /* GFX_GNOME */
#ifdef GFX_XVIEW
   fprintf (stderr, "       %s -g [xview options] (-|planfile)\n", progname);
   fprintf (stderr, "\t-g - preview the route graphically in an XView window\n");
#endif /* GFX_XVIEW */
#ifdef GFX_POSTSCRIPT
   fprintf (stderr, "       %s -g (-|planfile)\n", progname);
   fprintf (stderr, "\t-g - create a PostScript program to draw the route\n");
#endif /* GFX_POSTSCRIPT*/
   fprintf (stderr, "\t-  - read 'planfile' from the standard input.\n");
#endif /* GFX */

   /*
    * NORMAL mode:
    */
   fprintf (stderr, "    or\n");
   fprintf (stderr, "       %s [-b][-d][-e][-n|w][-s][-t] (-|planfile)\n", 
	    progname);
   fprintf (stderr, "\t-b - set BRIEF mode - ignore 'incremental' waypoints\n");
   fprintf (stderr, "\t-d - append a summary of all database objects used\n");
   fprintf (stderr, "\t-e - use Epson (PC) box characters on output form\n");
   fprintf (stderr, "\t-n - selects the narrow (no VOR fixes) format form [default]\n");
   fprintf (stderr, "\t-s - selects Statute miles for output format [default = Nautical]\n");
   fprintf (stderr, "\t-t - enable automatically tracking navaids in NAV1 [default = disabled]\n");
   fprintf (stderr, "\t-w - selects the wide (with VOR fixes) format form \n");
   fprintf (stderr, "\t-  - read 'planfile' from the standard input.\n");
   fprintf (stderr, "    the planfile format is described in the fplan user's guide\n");
   exit (status);
}

/*----------------------------------------------------------------------------*/
int 
main (int argc, char *argv[])
{
   int i;
   char c, errflag;
   BOOLEAN auto_track       = FALSE;
   BOOLEAN brief_mode       = FALSE;
   BOOLEAN db_summary       = FALSE;
#ifdef GFX_GNOME
   BOOLEAN draw_gnome       = FALSE;
#endif
#ifdef GFX_XVIEW
   BOOLEAN draw_xview       = FALSE;
#endif
#ifdef GFX_POSTSCRIPT
   BOOLEAN draw_postscript  = FALSE;
#endif
   BOOLEAN pc_graphic_chars = FALSE;
   BOOLEAN reverse          = FALSE;
   OUTPUT_FORMAT format     = NARROW;

   if (argc < 2)
      usage (argv[0], EXIT_BAD);

   /*
    * initialize
    */

   yyin = NULL;

   set_output_units (1);

   if (! open_dbs ())
      exit (EXIT_BAD);

   /*
    * LOOKUP mode
    */

   if (! strcmp (argv[1], "-l")) {
      if (argc > 2) {
	 for (i = 2; i < argc; i++)
	    do_lookup (argv[i]);
	 exit (EXIT_GOOD);
      } else {
	 usage (argv[0], EXIT_BAD);
      }
   }

   /*
    * REVERSE mode
    */

   if (! strcmp (argv[1], "-r")) {
      if (argc == 3) {
	 reverse = TRUE;
	 if (! (yyin = fopen (argv[2], "r"))) {
	    fprintf (stderr, "ERROR: could not open planfile: %s\n", argv[2]);
	    usage (argv[0], EXIT_BAD);
	 }
      } else {
	 if (argc < 3)
	    fprintf (stderr, "ERROR: missing planfile argument\n");
	 usage (argv[0], EXIT_BAD);
      }
   }

   /*
    * GRAPHICS mode
    */

   else if (! strcmp (argv[1], "-g")) {
#ifdef GFX
#ifdef GFX_GNOME
      draw_gnome = TRUE;
#endif
#ifdef GFX_XVIEW
      draw_xview = TRUE;
      xv_init (XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
#endif
#ifdef GFX_POSTSCRIPT
      draw_postscript = TRUE;
#endif
      if (argc == 3) {
	 if (! (yyin = fopen (argv[2], "r"))) {
	    fprintf (stderr, "ERROR: could not open planfile: %s\n", argv[2]);
	    usage (argv[0], EXIT_BAD);
	 }
      } else {
	 if (argc < 3)
	    fprintf (stderr, "ERROR: missing planfile argument\n");
	 usage (argv[0], EXIT_BAD);
      }
#else
      fprintf (stderr, "ERROR: graphics was not enabled at compile time\n");
      usage (argv[0], EXIT_BAD);
#endif
   } else {

      /*
       * NORMAL mode
       */

      opterr = 0;
      errflag = FALSE;

      while ((c = getopt (argc, argv, "bdeghnrstw")) != -1) {
	 switch (c) {
	 case 'b':
	    brief_mode = TRUE;
	    break;
	 case 'd':
	    db_summary = TRUE;
	    break;
	 case 'e':
	    pc_graphic_chars = TRUE;
	    break;
	 case 'g':
	    fprintf (stderr, "ERROR: -g must be the first option\n");
	    errflag++;
	    break;
	 case 'h':
	    usage (argv[0], EXIT_GOOD);
	    break;
	 case 'n':
	    format = NARROW;
	    break;
	 case 'r':
	    fprintf (stderr, "ERROR: -r must be the first option\n");
	    errflag++;
	    break;
	 case 's':
	    set_output_units (0);
	    break;
	 case 't':
	    auto_track = TRUE;
	    break;
	 case 'w':
	    format = WIDE;
	    break;
	 default:
	    fprintf (stderr, "ERROR: unknown option: %s\n", argv[optind-1]);
	    errflag++;
	    break;
	 }
	 if (errflag)
	    usage (argv[0], EXIT_BAD);
      }

      /*
       * process planfile argument
       */
    
      if (optind < argc) {
	 if (! strcmp(argv[optind], "-")) {
	    yyin = stdin;
	 } else {
	    if (! (yyin = fopen (argv[optind], "r"))) {
	       fprintf (stderr, "ERROR: could not open planfile: %s\n",
	                argv[optind]);
	       usage (argv[0], EXIT_BAD);
	    }
	 }
      } else {
	 fprintf (stderr, "ERROR: missing planfile argument\n");
	 usage (argv[0], EXIT_BAD);
      }
    
      if (++optind < argc) {
	 fprintf (stderr, "ERROR: only one planfile allowed: %s\n",
		  argv[optind]);
	 usage (argv[0], EXIT_BAD);
      }

   } /* else */

   /*
    * execute
    */

   init (brief_mode);

   if (! parse_script ())
      exit (EXIT_BAD);

   if (! compute_plan (auto_track))
      exit (EXIT_BAD);

   if (reverse) {
      print_reverse ();
#ifdef GFX_GNOME
   } else if (draw_gnome) {
      gnome_draw (brief_mode);
#endif
#ifdef GFX_XVIEW
   } else if (draw_xview) {
      xv_draw (brief_mode);
#endif
#ifdef GFX_POSTSCRIPT
   } else if (draw_postscript) {
      ps_draw (brief_mode);
#endif
   } else {
      if (format == NARROW)
	 max_nav = -1;
      else if (auto_track)
	 max_nav = MAX (0, max_nav);

      set_text_format (max_nav, pc_graphic_chars);

      print_plan_text ();

      if (db_summary)
	 put_db_summary (stdout);
   }

   if (! close_dbs ())
      exit (EXIT_BAD);

   exit(EXIT_GOOD);
}


/*----------------------------------------------------------------------------*/
int 
yyerror (char *s)
{
   fprintf (stderr, "ERROR: line %d: %s\n", yylineno, s);
   exit (EXIT_BAD);
}

/*----------------------------------------------------------------------------*/
int 
yywrap (void)
{
   return (1);
}

/*----------------------------------------------------------------------------*/
int 
yyreject (void)
{
   fprintf (stderr, "ERROR: scanner rejecting: %s\n", yytext);
   return (1);
}

