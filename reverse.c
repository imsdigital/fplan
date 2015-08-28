/*
 * $Id: reverse.c,v 1.4 1999/04/20 06:18:26 jcp Exp $
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

static char rcsid[] = "$Id: reverse.c,v 1.4 1999/04/20 06:18:26 jcp Exp $";

#include <math.h>
#include <stdio.h>

#include "common.h"

/*----------------------------------------------------------------------------*/
void 
print_reverse (void)
{
   int i, j;
   int lat_deg, lat_min, lat_sec;
   int lon_deg, lon_min, lon_sec;
   double mag_variation;
   double latitude;
   double longitude;

   for (i = num_waypoints - 1; i >= 0; i--) {

      switch (waypoints[i].kind) {
      case WP_FROM:
	 printf ("to");
	 break;
      case WP_VIA:
	 printf ("via");
	 break;
      case WP_TO:
	 printf ("from");
	 break;
      default:
	 break;
      }

      switch (waypoints[i].db->type) {
      case WP_INCREMENTAL:
	 printf (" %.2f;\n", -waypoints[i].distance_1);
	 break;
      case WP_INTERSECTION:
	 printf (" %s %.2f %s %.2f;\n",
			waypoints[i].ident_1, waypoints[i].direction_1,
			waypoints[i].ident_2, waypoints[i].direction_2);
	 break;
      case WP_RELATIVE:
	 printf (" %s %.2f / %.2f;\n",
			waypoints[i].ident_1,
			waypoints[i].direction_1,
			waypoints[i].distance_1);
	 break;
      case WP_LAT_LON:
	 latitude = waypoints[i].db->latitude;
	 longitude = waypoints[i].db->longitude;
	 mag_variation = waypoints[i].db->mag_variation;
	 decimal_2_deg_min_sec (latitude, &lat_deg, &lat_min, &lat_sec);
	 decimal_2_deg_min_sec (longitude, &lon_deg, &lon_min, &lon_sec);
	 printf (" %d:%02d:%02d %s\n    %d:%02d:%02d %s\n    %.2f %s;\n", 
	    lat_deg, lat_min, lat_sec, (latitude > 0.0) ? "north" : "south",
	    lon_deg, lon_min, lon_sec, (longitude > 0.0) ? "west" : "east",
            fabs (mag_variation), (mag_variation > 0.0) ? "west" : "east");
	 break;
      default:
	 printf (" %s;\n", waypoints[i].db->ident);
	 break;
      }

      for (j = 1; j <= max_nav; j++) {
	 if ((i == num_waypoints - 1) ||
	     (strcmp (waypoints[i].vor_fix[j].db->ident, 
		      waypoints[i+1].vor_fix[j].db->ident))) {
	    printf ("nav %d %s;\n", j+1, waypoints[i].vor_fix[j].db->ident);
	 }
      }

   }
}
