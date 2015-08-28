/*
 * $Id: add.c,v 2.9 1999/04/20 06:18:13 jcp Exp $
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

static char rcsid[] = "$Id: add.c,v 2.9 1999/04/20 06:18:13 jcp Exp $";

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

static BOOLEAN brief_mode = FALSE;

/*----------------------------------------------------------------------------*/
static void 
init_db (DB_INFO *db)
{
   if (db) {
      db->type = WP_UNK;
      db->latitude = 0.0;
      db->longitude = 0.0;
      db->mag_variation = 0.0;
      db->ident = (char*)NULL;
      db->name = (char*)NULL;
      db->city = (char*)NULL;
      db->comment = (char*)NULL;
      db->freq.valid = FALSE;
      db->freq.value = 0.0;
      db->altitude.valid = FALSE;
      db->altitude.value = 0.0;
   }
}

/*----------------------------------------------------------------------------
 * This function is called once by all of the functions that implement
 * the addition of a new waypoint of a given kind. It handles the carry
 * over of waypoint information, and enforces the expected from, via,
 * ... to sequence of wp_kind values.
 */

static BOOLEAN expect_from = TRUE; /* if true next waypoint should be "from" */

static void 
new_waypoint (WP_KIND kind)
{
   if (num_waypoints >= MAX_NUM_WAYPOINTS)
      yyerror ("the number of waypoints has exceeded the maximum");

   if (expect_from) {
      if (kind != WP_FROM)
	 yyerror ("expecting 'from' type waypoint");
      else
	 expect_from = FALSE;
   } else {
      if (kind == WP_FROM)
	 yyerror ("expecting 'via' or 'to' type waypoint");
   }
   
   if (num_waypoints > 0) {
      waypoints[num_waypoints] = waypoints[num_waypoints-1]; 
      waypoints[num_waypoints].fuel_extra.valid = FALSE;
      waypoints[num_waypoints].refuel = FALSE;
   }

   if (kind == WP_TO)
      expect_from = TRUE;
   
   num_waypoints++;
}

/*----------------------------------------------------------------------------
 * This function computes the lat,lon for a relative waypoint. I use
 * the spherical triangle composed of the given reference point, the
 * desired point, and the north pole. An expression for the co-latitude
 * and the difference in the longitudes of the reference and desired
 * point can be found from the law of sines, and law of cosines, (for
 * spherical, not planar triangles).
 */

static void 
locate_relative (double latitude_ref,
		 double longitude_ref,
		 double mag_variation_ref,
		 double mag_direction,
		 double distance,
		 double *latitude,
		 double *longitude)
{
   double cos_lat1;
   double sin_lat1;
   double sin_lat2;
   double cos_direction;
   double sin_direction;
   double cos_distance;
   double sin_distance;
   double cos_delta;
   double sin_delta;
   double delta_lon;
   double true_dir;

   cos_lat1 = cos (DEG2RAD (latitude_ref));
   sin_lat1 = sin (DEG2RAD (latitude_ref));
   true_dir = mag_direction - mag_variation_ref;	/* convert to true */
   cos_direction = cos (DEG2RAD (true_dir));
   sin_direction = sin (DEG2RAD (true_dir));
   cos_distance = cos (DEG2RAD (distance / 60.0));
   sin_distance = sin (DEG2RAD (distance / 60.0));

   sin_lat2 = cos_distance * sin_lat1 + sin_distance * cos_lat1 * cos_direction;
   *latitude = RAD2DEG (asin (sin_lat2));

   cos_delta = cos_distance - sin_lat2 * sin_lat1;
   sin_delta = sin_distance * sin_direction * cos_lat1;
   delta_lon = RAD2DEG (atan2 (sin_delta, cos_delta));
   *longitude = longitude_ref - delta_lon; /* + for other lon sign convention */
   while (*longitude < -180.0) *longitude += 360.0;
   while (*longitude >  180.0) *longitude -= 360.0;
}

/*----------------------------------------------------------------------------
 * This function computes the lat,lon for an intersection waypoint.
 * Consider the spherical triangle composed of the two given points, and
 * the desired point. An expression for the angle of intersection of the
 * two radials can be obtained from the (spherical) law of cosines. From
 * this we can compute the respective distances from the reference points
 * to the intersection using the (spherical) law of sines. Finally, we
 * can use the locate_relative() function to find the lat, lon of the
 * intersection. We carry out this calculation for each of the two given
 * points and compute a weighted average of the results to improve
 * numerical stability.
 *
 * This computation becomes ill conditioned (overly sensitive to the
 * accuracy of the given radials) when the two defining radials are
 * almost parallel. In the interest of safety, I reject any definition
 * where the radials are within 7.5 degrees or so of being parallel. If
 * you want to change the threshold, edit the macro ILL_DEFINED below,
 * assign it the sin() of the desired threshold angle > 0.0
 *
 * We also need to come up with some reasonable value for the magnetic
 * variation at the intersection. We only have the variation at two
 * points, so it's very hard to get good estimates when the distance
 * from the intersection to the closest point on the baseline (the
 * great circle segment that connects the two reference points) is
 * large.
 *
 * The best approach seemed to be interpolation with respect to the
 * distance from the 1-st point to the point found by projecting the
 * intersection onto the baseline great circle. The interpolation is
 * linear and it transititions smoothly to extrapolation when the
 * projected point is no longer interior to the segment of the
 * baseline great circle that connects the two reference points.
 */

#define ILL_DEFINED (0.13052619222005159)	/* sin(7.5 degrees) */

static void
locate_intersection (WP_KIND kind,
		     char *ident_1, double mag_direction_1,
		     char *ident_2, double mag_direction_2,
		     double *latitude,
		     double *longitude,
		     double *mag_variation)
{
   char buffer[128];
   DB_INFO *db1;
   DB_INFO *db2;
   double latitude_1, longitude_1, mag_variation_1;
   double latitude_2, longitude_2, mag_variation_2;
   double latitude_1p, longitude_1p;
   double latitude_2p, longitude_2p;
   double true_direction_1;
   double true_direction_2;
   double true_course_12;	/* baseline angle between reference points */
   double distance_12;

   double distance_1p;
   double distance_2p;
   double cos_dist_12, sin_dist_12;
   double sin_dist_1p, sin_dist_2p;
   double cos_theta_A, sin_theta_A;  /* angle between radials at intersection */
   double cos_theta_B, sin_theta_B;  /* angle between radial1 and baseline */
   double cos_theta_C, sin_theta_C;  /* angle between radial2 and baseline */
   double theta_B;	/* degrees */
   double theta_C;

   double proj_dist;	/* projection of point onto baseline great circle */
   double wgt_1;
   double wgt_2;

   if (lookup_ident (kind, ident_1, &db1)) {
      if (lookup_ident (kind, ident_2, &db2)) {
	 latitude_1 = db1->latitude;
	 latitude_2 = db2->latitude;
	 longitude_1 = db1->longitude;
	 longitude_2 = db2->longitude;
	 mag_variation_1 = db1->mag_variation;
	 mag_variation_2 = db2->mag_variation;
      } else {
	 sprintf (buffer, "identifier %s not found in any database", ident_2);
	 yyerror (buffer);
      }
   } else {
      sprintf (buffer, "identifier %s not found in any database", ident_1);
      yyerror (buffer);
   }

   true_direction_1 = mag_direction_1 - mag_variation_1;
   true_direction_2 = mag_direction_2 - mag_variation_2;

   great_circle_course ( latitude_1, longitude_1,
			 latitude_2, longitude_2,
			 &true_course_12, &distance_12 );

   theta_B = true_course_12 - true_direction_1;
   theta_C = true_direction_2 - (true_course_12 + 180.0);

   cos_theta_B = cos (DEG2RAD (theta_B));
   sin_theta_B = sin (DEG2RAD (theta_B));
   cos_theta_C = cos (DEG2RAD (theta_C));
   sin_theta_C = sin (DEG2RAD (theta_C));

   /* test for no solution (negative area) */
   if (sin_theta_B*sin_theta_C < 0.0)
      yyerror ("the intersection does not exist");

   cos_dist_12 = cos (DEG2RAD (distance_12 / 60.0));
   sin_dist_12 = sin (DEG2RAD (distance_12 / 60.0));

   cos_theta_A = sin_theta_B * sin_theta_C * cos_dist_12
					- cos_theta_B * cos_theta_C;
   sin_theta_A = sqrt((1.0 + cos_theta_A) * (1.0 - cos_theta_A));

   /* test for ill defined solution */
   if (sin_theta_A < ILL_DEFINED)
      yyerror ("the intersection is ill defined");

   sin_dist_1p = sin_dist_12 * fabs (sin_theta_C) / sin_theta_A;
   distance_1p = 60.0 * RAD2DEG (asin (sin_dist_1p));
   sin_dist_2p = sin_dist_12 * fabs (sin_theta_B) / sin_theta_A;
   distance_2p = 60.0 * RAD2DEG (asin (sin_dist_2p));

   locate_relative (latitude_1, longitude_1, mag_variation_1,
		    mag_direction_1, distance_1p,
		    &latitude_1p, &longitude_1p);

   locate_relative (latitude_2, longitude_2, mag_variation_2,
		    mag_direction_2, distance_2p,
		    &latitude_2p, &longitude_2p);

   wgt_1 = distance_2p / (distance_1p + distance_2p);
   wgt_2 = distance_1p / (distance_1p + distance_2p);

   *latitude = wgt_1 * latitude_1p
	     + wgt_2 * latitude_2p;
   *longitude = wgt_1 * longitude_1p
	      + wgt_2 * longitude_2p;

   /* interpolate to find magnetic variation */

   proj_dist = 60.0 * RAD2DEG (asin (sin_theta_A * sin_dist_1p));
   proj_dist = (cos_theta_B < 0.0) ? -proj_dist : proj_dist;

   wgt_2 = proj_dist / distance_12;
   wgt_1 = 1.0 - wgt_2;

   *mag_variation = wgt_1 * mag_variation_1
		  + wgt_2 * mag_variation_2;
}

/*----------------------------------------------------------------------------*/
void 
add_named_waypoint (WP_KIND kind, char *ident)
{
   char buffer[128];
   DB_INFO *db;

   if (lookup_ident (kind, ident, &db)) {
      new_waypoint (kind);   
      waypoints[num_waypoints-1].kind = kind;
      waypoints[num_waypoints-1].db = db;
   } else {
      sprintf (buffer, "identifier %s not found in any database", ident);
      yyerror (buffer);
   }
}

/*----------------------------------------------------------------------------*/
void 
add_inc_waypoint (WP_KIND kind,
		  double distance,
		  char *name_str,
		  char *city_str,
		  char *comment_str)
{
   char buffer[128];

   if (brief_mode)
      return;

   if (kind != WP_VIA)
      yyerror ("expecting 'via' keyword for incremental waypoint");

   new_waypoint (kind);

   waypoints[num_waypoints-1].db = 
      (DB_INFO*) malloc (sizeof (DB_INFO));
   if (! waypoints[num_waypoints-1].db)
      yyerror ("memory allocation failure for incremental waypoint db");

   init_db (waypoints[num_waypoints-1].db);

   waypoints[num_waypoints-1].kind = WP_VIA;
   waypoints[num_waypoints-1].db->type = WP_INCREMENTAL;
   if (name_str)
      waypoints[num_waypoints-1].db->name = name_str;
   else {
      sprintf (buffer, "%.1f %s",
		       fabs (distance), (distance < 0.0) ? "TO" : "FROM");
      waypoints[num_waypoints-1].db->name = strdup (buffer);
   }
   waypoints[num_waypoints-1].db->city = city_str;
   waypoints[num_waypoints-1].db->comment = comment_str;

   /*
    * NOTE: we can't compute lat,lon since we don't know the true
    * course yet. Just store the distance for now, we'll find the
    * lat,lon after we have read the whole planfile.
    */

   waypoints[num_waypoints-1].distance_1 = distance;
}

/*----------------------------------------------------------------------------*/
void 
add_int_waypoint (WP_KIND kind,
		  char *ident_1, double mag_direction_1,
		  char *ident_2, double mag_direction_2,
		  char *name_str,
		  char *city_str,
		  char *comment_str)
{
   char buffer[128];

   new_waypoint (kind);

   waypoints[num_waypoints-1].db = 
      (DB_INFO*) malloc (sizeof (DB_INFO));
   if (! waypoints[num_waypoints-1].db)
      yyerror ("memory allocation failure for intersection waypoint db");

   init_db (waypoints[num_waypoints-1].db);
   waypoints[num_waypoints-1].kind = kind;
   waypoints[num_waypoints-1].db->type = WP_INTERSECTION;

   locate_intersection (kind,
			ident_1, mag_direction_1,
			ident_2, mag_direction_2, 
			&waypoints[num_waypoints-1].db->latitude,
			&waypoints[num_waypoints-1].db->longitude,
			&waypoints[num_waypoints-1].db->mag_variation);

   if (name_str)
      waypoints[num_waypoints-1].db->name = name_str;
   else {
      sprintf (buffer, "%s %.1fF, %s %.1fF",
		       ident_1, mag_direction_1, ident_2, mag_direction_2);
      waypoints[num_waypoints-1].db->name = strdup (buffer);
   }
   waypoints[num_waypoints-1].db->city = city_str;
   waypoints[num_waypoints-1].db->comment = comment_str;

   waypoints[num_waypoints-1].ident_1 = ident_1;
   waypoints[num_waypoints-1].ident_2 = ident_2;

   waypoints[num_waypoints-1].direction_1 = mag_direction_1;
   waypoints[num_waypoints-1].direction_2 = mag_direction_2;
}

/*----------------------------------------------------------------------------*/
void 
add_rel_waypoint (WP_KIND kind,
		  char *ident,
		  double mag_direction,
		  double distance,
		  char *name_str,
		  char *city_str,
		  char *comment_str)
{
   DB_INFO *db;
   char buffer[128];
   double latitude_ref;
   double longitude_ref;
   double mag_variation_ref;

   new_waypoint (kind);

   waypoints[num_waypoints-1].db = 
      (DB_INFO*) malloc (sizeof (DB_INFO));
   if (! waypoints[num_waypoints-1].db)
      yyerror ("memory allocation failure for intersection waypoint db");

   init_db (waypoints[num_waypoints-1].db);
   waypoints[num_waypoints-1].kind = kind;
   waypoints[num_waypoints-1].db->type = WP_RELATIVE;

   if (lookup_ident (kind, ident, &db)) {
      latitude_ref = db->latitude;
      longitude_ref = db->longitude;
      mag_variation_ref = db->mag_variation;
      locate_relative (latitude_ref, longitude_ref, mag_variation_ref,
		       mag_direction, distance,
		       &waypoints[num_waypoints-1].db->latitude, 
		       &waypoints[num_waypoints-1].db->longitude);
   } else {
      sprintf (buffer, "identifier %s not found in any database", ident);
      yyerror (buffer);
   }

   waypoints[num_waypoints-1].db->mag_variation = mag_variation_ref;
   if (name_str)
      waypoints[num_waypoints-1].db->name = name_str;
   else {
      sprintf (buffer, "%s %.1f/%.1f",
		       ident, mag_direction, distance);
      waypoints[num_waypoints-1].db->name = strdup (buffer);
   }
   waypoints[num_waypoints-1].db->city = city_str;
   waypoints[num_waypoints-1].db->comment = comment_str;

   waypoints[num_waypoints-1].ident_1 = ident;
   waypoints[num_waypoints-1].direction_1 = mag_direction;
   waypoints[num_waypoints-1].distance_1 = distance;
}

/*----------------------------------------------------------------------------*/
void 
add_lat_waypoint (WP_KIND kind,
		  double latitude,
		  double longitude,
		  double mag_variation,
		  char *name_str,
		  char *city_str,
		  char *comment_str)
{
   new_waypoint (kind);

   waypoints[num_waypoints-1].db = 
      (DB_INFO*) malloc (sizeof (DB_INFO));
   if (! waypoints[num_waypoints-1].db)
      yyerror ("memory allocation failure for latitude, longitude waypoint");

   init_db (waypoints[num_waypoints-1].db);
   waypoints[num_waypoints-1].kind = kind;
   waypoints[num_waypoints-1].db->type = WP_LAT_LON;
   waypoints[num_waypoints-1].db->latitude = latitude;
   waypoints[num_waypoints-1].db->longitude = longitude;
   waypoints[num_waypoints-1].db->mag_variation = mag_variation;
   if (name_str)
      waypoints[num_waypoints-1].db->name = name_str;
   else
      waypoints[num_waypoints-1].db->name = strdup ("Lat/Lon Waypoint");
   waypoints[num_waypoints-1].db->city = city_str;
   waypoints[num_waypoints-1].db->comment = comment_str;
}

/*----------------------------------------------------------------------------*/
void 
set_comment ( char *comment_str )
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].db->comment = comment_str;
}

/*----------------------------------------------------------------------------*/
void 
set_xfix (int rcv_num, char *vor_ident)
{
   char buffer[128];
   int i = MAX (0, num_waypoints-1);

   if ((rcv_num < 1) || (rcv_num > MAX_NUM_VOR_FIXES)) {
      sprintf (buffer, "expecting rcv in interval [1,%d]", MAX_NUM_VOR_FIXES);
      yyerror (buffer);
   }

   if (lookup_ident (WP_VIA, vor_ident, &waypoints[i].vor_fix[rcv_num-1].db)) {
      if (IS_VOR(waypoints[i].vor_fix[rcv_num-1].db->type)) {
	 waypoints[i].vor_fix[rcv_num-1].db_valid = TRUE;
	 waypoints[i].vor_fix[rcv_num-1].fix_valid = FALSE;
	 max_nav = MAX (max_nav, rcv_num-1);
      } else {
	 sprintf (buffer, "identifier %s not a VOR station", vor_ident);
	 yyerror (buffer);
      }
   } else {
      sprintf (buffer, "identifier %s not found in any database", vor_ident);
      yyerror (buffer);
   }
}

/*----------------------------------------------------------------------------*/
void 
set_altitude (double feet)
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].altitude.valid = TRUE;
   waypoints[i].altitude.value = feet;
}

/*----------------------------------------------------------------------------*/
void 
set_fuel_amt (double amount)
{
   int i = MAX (0, num_waypoints-1);

   if ((i > 0) && (waypoints[i].kind != WP_FROM))
      yyerror ("fuel_amount applied to non departure waypoint");

   waypoints[i].refuel = TRUE;
   waypoints[i].fuel_amt.valid = TRUE;
   waypoints[i].fuel_amt.value = amount;
}

/*----------------------------------------------------------------------------*/
void 
set_fuel_extra (double used)
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].fuel_extra.valid = TRUE;
   waypoints[i].fuel_extra.value = used;
}

/*----------------------------------------------------------------------------*/
void 
set_fuel_rate (double rate)
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].fuel_rate.valid = TRUE;
   waypoints[i].fuel_rate.value = rate;
}

/*----------------------------------------------------------------------------*/
void 
set_tas (double tas)
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].tas.valid = TRUE;
   waypoints[i].tas.value = tas;
}

/*----------------------------------------------------------------------------*/
void 
set_wind (double heading, double speed)
{
   int i = MAX (0, num_waypoints-1);

   waypoints[i].wind_speed.valid = TRUE;
   waypoints[i].wind_speed.value = speed;
   waypoints[i].wind_direction.valid = TRUE;
   waypoints[i].wind_direction.value = heading;
}

/*----------------------------------------------------------------------------*/
void set_brief (BOOLEAN on_off)
{
   brief_mode = on_off;
}
