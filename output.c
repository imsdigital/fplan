/*
 * $Id: output.c,v 2.15 1999/04/20 06:18:22 jcp Exp $
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

static char rcsid[] = "$Id: output.c,v 2.15 1999/04/20 06:18:22 jcp Exp $";

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "output.h"

OUTPUT_UNITS output_units = NAUTICAL;

/*----------------------------------------------------------------------------*/
void 
set_output_units (int nautical)
{
   if (nautical)
      output_units = NAUTICAL;
   else
      output_units = STATUTE;
}


/*----------------------------------------------------------------------------*/
char *
format_time (int fw, double hours)
{
   static char buffer[32];
   int ihrs;
   int imin;
   long nmin;

   fw = ABS(fw);
   if (fw > 31) fw = 31;
   nmin = ROUND (60.0*hours);	/* round time to nearest minute */
   ihrs = nmin / 60L;
   nmin -= ihrs * 60L;
   imin = nmin;

   if (ihrs > 0) {
      sprintf (buffer, "%d:%02d", ihrs, imin);
   } else {
      sprintf (buffer, ":%02d", imin);
   }
   return buffer;
}

/*----------------------------------------------------------------------------*/
char * 
format_lat_lon (double latitude, double longitude)
{
   static char buffer[32];
   int lat_deg, lat_min, lat_sec;
   int lon_deg, lon_min, lon_sec;

   decimal_2_deg_min_sec (latitude, &lat_deg, &lat_min, &lat_sec);
   decimal_2_deg_min_sec (longitude, &lon_deg, &lon_min, &lon_sec);

   sprintf (buffer, "%d.%02d%02d%c %d.%02d%02d%c", 
	   lat_deg, lat_min, lat_sec, (latitude > 0.0) ? 'N' : 'S',
	   lon_deg, lon_min, lon_sec, (longitude > 0.0) ? 'W' : 'E');

   return buffer;
}

/*----------------------------------------------------------------------------*/
char * 
format_db_type (WP_TYPE type)
{
   char *s_val;

   switch (type) {
   case WP_INCREMENTAL:
      s_val = "Incremental";
      break;
   case WP_INT:
   case WP_INTERSECTION:
      s_val = "Intersection";
      break;
   case WP_LAT_LON:
      s_val = "Lat/Lon";
      break;
   case WP_RELATIVE:
      s_val = "Relative";
      break;
   case WP_AIRPORT:
      s_val = "Airport";
      break;
   case WP_VOR:
      s_val = "VOR";
      break;
   case WP_DME:
      s_val = "VOR/DME";
      break;
   case WP_TAC:
      s_val = "TACAN";
      break;
   case WP_ILS:
      s_val = "ILS";
      break;
   case WP_LOM:
      s_val = "Outer Mkr";
      break;
   case WP_LMM:
      s_val = "Middle Mkr";
      break;
   case WP_NDB:
      s_val = "NDB";
      break;
   case WP_WPT:
      s_val = "Waypoint";
      break;
   default:
      s_val = "Unknown";
      break;
   }

   return s_val;
}

