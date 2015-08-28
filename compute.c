/*
 * $Id: compute.c,v 2.10 1999/04/20 06:18:14 jcp Exp $
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

static char rcsid[] = "$Id: compute.c,v 2.10 1999/04/20 06:18:14 jcp Exp $";

#include <math.h>
#include <stdio.h>

#include "common.h"

/*----------------------------------------------------------------------------*/
static void 
normalize_heading (double *heading)
{
   while (*heading < 0.0)
      *heading += 360.0;

   while (*heading >= 360.0)
      *heading -= 360.0;
}

/*----------------------------------------------------------------------------
 * The following function computes the heading required to correct for
 * wind drift, as well as the resulting ground speed. It now detects
 * the fairly rare, but not impossible case of a wind that is too
 * strong to fly the desired course.
 */

static BOOLEAN 
wind_correct (double true_air_speed,
	      double mag_course,
	      double wind_speed,
	      double mag_wind_direction,
	      double *ground_speed,
	      double *mag_heading)
{
   double dtheta;
   double tx, ty;
   double sin_wca, wca;

   dtheta = DEG2RAD (mag_wind_direction - mag_course);
   tx = wind_speed * cos (dtheta);
   ty = wind_speed * sin (dtheta);
   sin_wca = ty / true_air_speed;
   if (fabs(sin_wca) > 1.0) {
     /* no solution */
     return FALSE;
   } else {
     wca = asin (sin_wca);
   }
   *ground_speed = true_air_speed * cos (wca) - tx;
   *mag_heading = mag_course + RAD2DEG (wca);
   normalize_heading (mag_heading);
   return TRUE;
}

/*----------------------------------------------------------------------------
 * The following function computes the great circle course between the
 * two given points. The great circle course minimizes the distance
 * traveled and is defined by the intersection of the earth's surface
 * with the unique plane defined by the two given points and the center
 * of the earth.
 *
 * For computational purposes we use the spherical triangle composed
 * of the two given points and the north pole. An expression for the
 * distance can be obtained directly from the spherical law of cosines for
 * sides (point 1 to point 2). The expression is then manipulated slightly
 * to provide better numerical stability when the two given points are close
 * together. The cosine of the initial heading follows from the spherical
 * law of cosines for sides (point 2 to the north pole), and the sine of
 * the initial heading follows from the spherical law of sines. The initial
 * heading can then be computed uniquely using the atan2 function.
 */

void
great_circle_course (double latitude_1, double longitude_1,
		     double latitude_2, double longitude_2,
		     double *true_course,
		     double *distance_nmi)
{
   double cos_latitude1, sin_latitude1;
   double cos_latitude2, sin_latitude2;
   double sin_hdlatitude;
   double sin_hdlongitude;
   double sin_dlongitude;

   double distance_rad;
   double cos_distance;
   double sin_hdist_sq;
   double course_x;
   double course_y;

   if ((latitude_1 == latitude_2) && (longitude_1 == longitude_2)) {
      *true_course = 0.0;
      *distance_nmi = 0.0;
      return;
   }

   cos_latitude1 = cos (DEG2RAD (latitude_1));
   sin_latitude1 = sin (DEG2RAD (latitude_1));
   cos_latitude2 = cos (DEG2RAD (latitude_2));
   sin_latitude2 = sin (DEG2RAD (latitude_2));

   sin_hdlatitude = sin (DEG2RAD (0.5 * (latitude_1 - latitude_2)));
   sin_hdlongitude = sin (DEG2RAD (0.5 * (longitude_1 - longitude_2)));
   sin_dlongitude = sin (DEG2RAD (longitude_1 - longitude_2));

   /* compute great circle distance from point 1 to point 2 */

   sin_hdist_sq = SQR (sin_hdlatitude) + 
      cos_latitude1 * cos_latitude2 * SQR (sin_hdlongitude);
   distance_rad = 2.0 * asin (sqrt (sin_hdist_sq));
   *distance_nmi = 60.0 * RAD2DEG (distance_rad);

   /* compute true course from point 1 to point 2 */

   cos_distance = cos (distance_rad);
   course_x = (sin_latitude2 - sin_latitude1 * cos_distance) / cos_latitude1;
   course_y = cos_latitude2 * sin_dlongitude;
   *true_course = RAD2DEG (atan2 (course_y, course_x));
   normalize_heading (true_course);
}

/*----------------------------------------------------------------------------
 * The following function computes a rhumb line course between the two
 * given points. A rhumb line course is one that uses a constant heading
 * (assuming calm winds). The distance is only slightly further than
 * the shortest possible (great circle) course.  The function correctly
 * handles the case where the course crosses the 180 E/W meridian. It
 * does fail when one of the points is a pole, (however, it is well
 * behaved for points as close as one second of arc to a pole).
 */

#define COS_EPSILON 0.00001

void 
rhumb_line_course (double latitude_1, double longitude_1,
		   double latitude_2, double longitude_2,
		   double *true_course, double *distance_nmi)
{
   double course_rad;
   double delta_lon;

   if ((latitude_1 == latitude_2) && (longitude_1 == longitude_2)) {
      *true_course = 0.0;
      *distance_nmi = 0.0;
      return;
   }

   latitude_1 = DEG2RAD (latitude_1);
   latitude_2 = DEG2RAD (latitude_2);
   longitude_1 = DEG2RAD (longitude_1);
   longitude_2 = DEG2RAD (longitude_2);

   delta_lon = 2.0 * asin (sin ((longitude_1 - longitude_2) / 2.0));

   course_rad = atan2 (delta_lon,
		       log (tan (0.5 * latitude_2 + 0.25 * PI)) - 
		       log (tan (0.5 * latitude_1 + 0.25 * PI)) );

   if (sin (longitude_1-longitude_2) >= 0.0)
      *true_course = RAD2DEG (fabs(course_rad));
   else
      *true_course = 360.0 - RAD2DEG (fabs(course_rad));

   if (fabs (cos (course_rad)) < COS_EPSILON)
      *distance_nmi = 60.0 * fabs (RAD2DEG (delta_lon)) * cos (latitude_1);
   else
      *distance_nmi = 60.0 *
          RAD2DEG (latitude_2 - latitude_1) / cos (course_rad);
}

/*----------------------------------------------------------------------------
 * The following function computes the latitude, longitude of a point
 * described by a rhumb line course and a distance along that course. It
 * is used by the locate_incrementals() function to compute the latitude,
 * longitude of incremental waypoints.
 */

static void 
rhumb_line_locate (double latitude_1, double longitude_1,
		   double true_course, double distance_nmi,
		   double *latitude, double *longitude)
{
   double latitude_2;
   double longitude_2;
   double delta_lon;

   if (distance_nmi == 0.0) {
      *latitude = latitude_1;
      *longitude = longitude_1;
      return;
   }

   latitude_1 = DEG2RAD (latitude_1);
   longitude_1 = DEG2RAD (longitude_1);
   true_course = DEG2RAD (true_course);
   distance_nmi = DEG2RAD (distance_nmi / 60.0);

   latitude_2 = latitude_1 + distance_nmi * cos (true_course);

   if (fabs (cos (true_course)) < COS_EPSILON)
      delta_lon = distance_nmi * sin (true_course) / cos (latitude_1);
   else
      delta_lon = tan (true_course) *
		  ( log (tan (0.5 * latitude_2 + 0.25 * PI)) -
		    log (tan (0.5 * latitude_1 + 0.25 * PI)) );

   longitude_2 = longitude_1 - delta_lon;

   latitude_2 = RAD2DEG (latitude_2);
   longitude_2 = RAD2DEG (longitude_2);

   while (longitude_2 < -180.0) longitude_2 += 360.0;
   while (longitude_2 > +180.0) longitude_2 -= 360.0;

   *latitude = latitude_2;
   *longitude = longitude_2;
}

/*----------------------------------------------------------------------------*/
static int 
next_non_incremental (int start)
{
   int i = start;

   for (i = start; i < num_waypoints; i++)
      if (waypoints[i].db->type != WP_INCREMENTAL)
	 return i;
   return -1;
}

/*----------------------------------------------------------------------------*/
static void 
locate_incrementals (void)
{
   int j;
   int prev;
   int next;
   double alpha_p;
   double alpha_n;
   double true_course;
   double distance_nmi;
   double distance_inc;

   for (prev = 0; ((prev >= 0) && (prev < num_waypoints)); ) {
      next = next_non_incremental (prev+1);
      if (next >= 0) {
	 rhumb_line_course (waypoints[prev].db->latitude,
			    waypoints[prev].db->longitude, 
			    waypoints[next].db->latitude, 
			    waypoints[next].db->longitude, 
			    &true_course, &distance_nmi);
	 for (j = prev+1; j < next; j++) {
	    if (waypoints[j].distance_1 < 0.0) {
	       distance_inc = distance_nmi + waypoints[j].distance_1;
	       alpha_p = -waypoints[j].distance_1 / distance_nmi;
	       alpha_n = 1.0 - alpha_p;
	    } else {
	       distance_inc = waypoints[j].distance_1;
	       alpha_n = waypoints[j].distance_1 / distance_nmi;
	       alpha_p = 1.0 - alpha_n;
	    }
	    rhumb_line_locate (waypoints[prev].db->latitude,
			       waypoints[prev].db->longitude,
			       true_course, distance_inc,
			       &waypoints[j].db->latitude,
			       &waypoints[j].db->longitude);
	    /* linear interpolation to find mag variation */
	    waypoints[j].db->mag_variation =
	       alpha_p * waypoints[prev].db->mag_variation +
	       alpha_n * waypoints[next].db->mag_variation;
	 }
      }
      prev = next;
   }
}

/*----------------------------------------------------------------------------
 * The following function is designed to decide if a VOR is located on
 * or near a given airport. Since the VOR will usually have a slightly
 * different lat/lon value than the airport it is associated with, we need
 * to implement a "fuzzy" compare. We compute the great circle (shortest
 * path) distance between the two, and compare it to the fuzzy threshold
 * defined by the macro FUZZ (units of nautical miles).
 */

#define FUZZ 5.0	/* nautical miles */

static BOOLEAN 
is_colocated (DB_INFO *db_1, DB_INFO *db_2)
{
   double true_course;
   double distance_nmi;

   great_circle_course(db_1->latitude, db_1->longitude,
		       db_2->latitude, db_2->longitude,
		       &true_course, &distance_nmi);

   return (distance_nmi < FUZZ);
}

/*----------------------------------------------------------------------------
 * This function tries to identify a VOR station associated with the
 * given waypoint identifier. The function first checks to see if the
 * waypoint is itself a VOR station. If not, it checks to see if the
 * waypoint is an airport, and if so, searches the database for a VOR
 * station collocated at the field with a similar identifier.
 * 
 */

static DB_INFO *
get_vor (char *ident_wpt)
{
   DB_INFO *db_wpt;
   DB_INFO *db_vor;
   char *ident_vor;

   if (lookup_ident (WP_VIA, ident_wpt, &db_wpt)) {
      if (IS_VOR (db_wpt->type)) {
	 return db_wpt;
      } else if (db_wpt->type == WP_AIRPORT) {
      /* look for navaid with same similar identifier */
	 if ((ident_vor = is_kident (ident_wpt))) {
	    if (lookup_ident (WP_VIA, ident_vor, &db_vor)) {
	       if (IS_VOR (db_vor->type) && is_colocated (db_wpt, db_vor))
		  return db_vor;
	    }
	 }
      }
   }

   return ((DB_INFO*) NULL);
}

/*----------------------------------------------------------------------------
 * This function computes VOR fixes for a given position (described
 * only by lat, lon). The provided course should be the aircraft's true
 * course *to* the waypoint, except for departure waypoints, where it
 * should be the true course *from* the waypoint. The course is used to
 * determine if the aircraft is moving TO / FROM the given VOR station.
 */

static void
compute_fix (double tc_aircraft,
	     double latitude,
	     double longitude,
	     VOR_FIX *vor_fix,
	     FROM_TO from_to)
{
   double lat_1, lon_1;
   double lat_2, lon_2;

   double tc_to_vor;
   double dist_nmi;

   double dot_product;
   double cos_tc_aircraft, sin_tc_aircraft;
   double cos_tc_to_vor, sin_tc_to_vor;
   
   if ((! vor_fix->db_valid) || (! vor_fix->db))
      return;

   lat_1 = latitude;
   lon_1 = longitude;
   lat_2 = vor_fix->db->latitude;
   lon_2 = vor_fix->db->longitude;

   if ((lat_1 == lat_2) && (lon_1 == lon_2)) {
      /* apparently, this waypoint is a VOR station */
      vor_fix->fix_valid = TRUE;
      vor_fix->radial = tc_aircraft + vor_fix->db->mag_variation;
      normalize_heading (&vor_fix->radial);
      vor_fix->distance = 0.0;
      vor_fix->from_to = from_to;
      return;
   }

   great_circle_course (lat_1, lon_1, lat_2, lon_2, &tc_to_vor, &dist_nmi); 

   cos_tc_to_vor = cos (DEG2RAD (tc_to_vor));
   sin_tc_to_vor = sin (DEG2RAD (tc_to_vor));
   cos_tc_aircraft = cos (DEG2RAD (tc_aircraft));
   sin_tc_aircraft = sin (DEG2RAD (tc_aircraft));

   dot_product = cos_tc_to_vor * cos_tc_aircraft
               + sin_tc_to_vor * sin_tc_aircraft;

   if (dot_product > 0.0) {
      vor_fix->fix_valid = TRUE;
      vor_fix->radial = tc_to_vor + vor_fix->db->mag_variation;
      normalize_heading (&vor_fix->radial);
      vor_fix->distance = dist_nmi;
      vor_fix->from_to = TO;
   } else {
      vor_fix->fix_valid = TRUE;
      vor_fix->radial = tc_to_vor + vor_fix->db->mag_variation + 180.0;
      normalize_heading (&vor_fix->radial);
      vor_fix->distance = dist_nmi;
      vor_fix->from_to = FROM;
   }
}

/*----------------------------------------------------------------------------
 * The following function implements the "auto track" feature. For
 * each waypoint in the plan, we make an educated guess as to which
 * VOR station we should have NAV receiver 1 tuned to.
 *
 * We begin by going through the entire route and identify as many
 * VOR stations as possible. Some of the waypoints may themselves
 * be VOR stations, and some of the waypoints that are airports may
 * have a VOR station located on or near the field (with the same or
 * similar identifier). We also try to use VOR stations associated
 * with reference identifiers in the definition of Intersection and
 * Relative waypoints.
 *
 * We then make a second pass through the entire plan. For waypoints
 * that don't have a VOR station associated with them, we use the
 * closest of any associated with the previous or next waypoint.
 *
 */

static void 
track_nav1 (void)
{
   int i;
   int j;
   int prev;
   int next;
   DB_INFO *db1;
   DB_INFO *db2;
   double true_course_1;
   double true_course_2;
   double distance_1;
   double distance_2;

   /*
    * invalidate all the user specified NAV receiver 1 settings
    */
   for (i = 0; i < num_waypoints; i++) {
      waypoints[i].vor_fix[0].db_valid = FALSE;
      waypoints[i].vor_fix[0].fix_valid = FALSE;
   }

   /*
    * locate as many VOR stations as we can over each plan
    */
   for (i = 0; i < num_waypoints; i++) {
      if (IS_VOR (waypoints[i].db->type)) {
	 waypoints[i].vor_fix[0].db_valid = TRUE;
	 waypoints[i].vor_fix[0].db = waypoints[i].db;
	 waypoints[i].vor_fix[0].fix_valid = FALSE;
	 if (waypoints[i].kind == WP_FROM) {
	    if (waypoints[i].tc.valid) {
	       waypoints[i].vor_fix[0].fix_valid = TRUE;
	       waypoints[i].vor_fix[0].radial =
		  waypoints[i].tc.value + waypoints[i].db->mag_variation;
	       normalize_heading (&waypoints[i].vor_fix[0].radial);
	       waypoints[i].vor_fix[0].distance = 0.0;
	       waypoints[i].vor_fix[0].from_to = FROM;
	    }
	 } else {
	    if (i > 0) {
	       if (waypoints[i-1].tc.valid) {
		  waypoints[i].vor_fix[0].fix_valid = TRUE;
		  waypoints[i].vor_fix[0].radial =
		     waypoints[i-1].tc.value + waypoints[i].db->mag_variation;
		  normalize_heading (&waypoints[i].vor_fix[0].radial);
		  waypoints[i].vor_fix[0].distance = 0.0;
		  waypoints[i].vor_fix[0].from_to = TO;
	       }
	    }
	 }
      } else {
	 switch (waypoints[i].db->type) {
	 case WP_AIRPORT:
	    if ((db1 = get_vor (waypoints[i].db->ident))) {
	       waypoints[i].vor_fix[0].db = db1;
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    }
	    break;
	 case WP_INTERSECTION:
	    db1 = get_vor (waypoints[i].ident_1);
	    db2 = get_vor (waypoints[i].ident_2);
	    if (db1 && db2) {
	       great_circle_course (waypoints[i].db->latitude,
				    waypoints[i].db->longitude,
				    db1->latitude, db1->longitude,
				    &true_course_1, &distance_1);
	       great_circle_course (waypoints[i].db->latitude,
				    waypoints[i].db->longitude,
				    db2->latitude, db2->longitude,
				    &true_course_2, &distance_2);
	       /* choose the closest of the two */
	       if (distance_1 < distance_2) {
		  waypoints[i].vor_fix[0].db = db1;
		  waypoints[i].vor_fix[0].db_valid = TRUE;
		  waypoints[i].vor_fix[0].fix_valid = FALSE;
	       } else {
		  waypoints[i].vor_fix[0].db = db2;
		  waypoints[i].vor_fix[0].db_valid = TRUE;
		  waypoints[i].vor_fix[0].fix_valid = FALSE;
	       }
	    } else {
	       if (db1) {
		  waypoints[i].vor_fix[0].db = db1;
		  waypoints[i].vor_fix[0].db_valid = TRUE;
		  waypoints[i].vor_fix[0].fix_valid = FALSE;
	       }
	       if (db2) {
		  waypoints[i].vor_fix[0].db = db2;
		  waypoints[i].vor_fix[0].db_valid = TRUE;
		  waypoints[i].vor_fix[0].fix_valid = FALSE;
	       }
	    }
	    break;
	 case WP_RELATIVE:
	    if ((db1 = get_vor (waypoints[i].ident_1))) {
	       waypoints[i].vor_fix[0].db = db1;
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    }
	    break;
	 default:
	 }
      }
   }

   /*
    * select VOR stations for all other waypoints
    */
   prev = -1;
   next = -1;
   for (i = 0; i < num_waypoints; i++) {
      if (waypoints[i].vor_fix[0].db_valid) {
	 prev = i;
      } else {
	 if (next <= i) {
	    for (j = i;(j<num_waypoints) && (waypoints[j].kind != WP_TO); j++) {
	       if (waypoints[j].vor_fix[0].db_valid) {
		  next = j;
		  break;
	       }
	    }
	 }
	 if ((prev > -1) && (next > i)) {
	    great_circle_course (waypoints[i].db->latitude,
				 waypoints[i].db->longitude,
				 waypoints[prev].db->latitude,
				 waypoints[prev].db->longitude,
				 &true_course_1, &distance_1);
	    great_circle_course (waypoints[i].db->latitude,
				 waypoints[i].db->longitude,
				 waypoints[next].db->latitude,
				 waypoints[next].db->longitude,
				 &true_course_2, &distance_2);
	    /* choose the closest of the two */
	    if (distance_1 < distance_2) {
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].db = waypoints[prev].vor_fix[0].db;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    } else {
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].db = waypoints[next].vor_fix[0].db;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    }
	 } else {
	    if (prev > -1) {
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].db = waypoints[prev].vor_fix[0].db;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    }
	    if (next >  i) {
	       waypoints[i].vor_fix[0].db_valid = TRUE;
	       waypoints[i].vor_fix[0].db = waypoints[next].vor_fix[0].db;
	       waypoints[i].vor_fix[0].fix_valid = FALSE;
	    }
	 }
      }
      if (waypoints[i].kind == WP_TO) {
	 prev = -1;
	 next = -1;
      }
   }
}

/*----------------------------------------------------------------------------*/

#define MAX_NUM_PLANS 64

BOOLEAN 
compute_plan (BOOLEAN auto_track)
{
   int i;
   int vor;
   int num_plans = 0;
   double true_course;
   double distance_nmi;
   double mag_wind_direction;
   double total_dist[MAX_NUM_PLANS];

   /*
    * compute latitude, longitude, mag variation of all incremental waypoints
    */
   locate_incrementals ();

   /*
    * accumulate the total distance between each from/to pair
    */
   num_plans = 0;
   for (i = 0; i < num_waypoints; i++) {
      if (waypoints[i].kind == WP_FROM) {
	 waypoints[i].eta.valid = TRUE;
	 waypoints[i].eta.value = 0.0;
	 total_dist[num_plans] = 0.0;
      }
      if ((i+1 < num_waypoints) && (waypoints[i].kind != WP_TO)) {
	 rhumb_line_course (waypoints[i].db->latitude,
			    waypoints[i].db->longitude,
			    waypoints[i+1].db->latitude,
			    waypoints[i+1].db->longitude,
			    &true_course, &distance_nmi);
	 waypoints[i].tc.valid = TRUE;
	 waypoints[i].tc.value = true_course;
	 normalize_heading (&waypoints[i].tc.value);
	 waypoints[i].mc.valid = TRUE;
	 waypoints[i].mc.value = true_course + waypoints[i].db->mag_variation;
	 normalize_heading (&waypoints[i].mc.value);
	 waypoints[i].dist_leg.valid = TRUE;
	 waypoints[i].dist_leg.value = distance_nmi;
      } else {
	 waypoints[i].tc.valid = FALSE;
	 waypoints[i].mc.valid = FALSE;
	 waypoints[i].dist_leg.valid = FALSE;
      }

      waypoints[i].dist.valid = TRUE;
      waypoints[i].dist.value = total_dist[num_plans];

      total_dist[num_plans] += waypoints[i].dist_leg.value;

      if (waypoints[i].kind == WP_TO)
	 num_plans++;
   }

   /*
    * now, do the distance remaining:
    */
   num_plans = 0;
   for (i = 0; i < num_waypoints; i++) {
      waypoints[i].dist_remain.value = 
	 total_dist[num_plans] - waypoints[i].dist.value;
      waypoints[i].dist_remain.valid = TRUE;
      if (waypoints[i].kind == WP_TO)
	 num_plans++;
   }

   /*
    * wind correction (magnetic heading, ground speed)
    */
   for (i = 0; i < num_waypoints; i++) {

      if (waypoints[i].tas.valid && waypoints[i].mc.valid) {

	 if (! waypoints[i].wind_speed.valid)
	    waypoints[i].wind_speed.value = 0.0; 
	 if (! waypoints[i].wind_direction.valid)
	    waypoints[i].wind_direction.value = 0.0;

	 /* convert wind direction from true to magnetic (locally) */

	 mag_wind_direction = waypoints[i].wind_direction.value +
	    waypoints[i].db->mag_variation;

	 normalize_heading (&mag_wind_direction);

	 /* compute magnetic heading corrected for wind direction, speed */
 
	 if (wind_correct (waypoints[i].tas.value, 
			   waypoints[i].mc.value, 
			   waypoints[i].wind_speed.value,
			   mag_wind_direction,
			   &waypoints[i].egs.value,
			   &waypoints[i].mh.value)) {

	    waypoints[i].mh.valid = TRUE;
	    normalize_heading (&waypoints[i].mh.value);
	    waypoints[i].egs.valid = TRUE;

	    /* compute time for this leg in units of hours */
	    waypoints[i].eta_leg.valid = waypoints[i].dist_leg.valid;
	    waypoints[i].eta_leg.value = waypoints[i].dist_leg.value / 
	       waypoints[i].egs.value;

	 } else {

	    waypoints[i].mh.valid = FALSE;
	    waypoints[i].egs.valid = FALSE;
	    waypoints[i].eta.valid = FALSE;
	    waypoints[i].eta_leg.valid = FALSE;

	 }

      } else {

	 waypoints[i].mh.valid = FALSE;
	 waypoints[i].egs.valid = FALSE;
	 waypoints[i].eta.valid = FALSE;
	 waypoints[i].eta_leg.valid = FALSE;

      }

      waypoints[i].eta.valid = (BOOLEAN) (waypoints[i].kind != WP_FROM);
      waypoints[i].eta.value = (waypoints[i].dist.value == 0.0) ? 0.0 :
	  waypoints[i-1].eta.value + waypoints[i-1].eta_leg.value;

   }

   /*
    * fuel burn:
    */
   for (i = 0; i < num_waypoints; i++) {

      waypoints[i].fuel_leg.valid = (BOOLEAN)
	 (waypoints[i].fuel_rate.valid && waypoints[i].eta_leg.valid);
      if (waypoints[i].fuel_leg.valid)
	 waypoints[i].fuel_leg.value = 
	    waypoints[i].fuel_rate.value * waypoints[i].eta_leg.value;
      if (waypoints[i].refuel) {
	 waypoints[i].fuel_amt.valid = TRUE;
      } else if (i && (waypoints[i].kind == WP_FROM) &&
		 (! waypoints[i].refuel)) {
	 waypoints[i].fuel_amt.valid = waypoints[i-1].fuel_amt.valid;
	 waypoints[i].fuel_amt.value = waypoints[i-1].fuel_amt.value;
      } else if (i && waypoints[i-1].fuel_leg.valid &&
		 waypoints[i-1].fuel_amt.valid) {
	 waypoints[i].fuel_amt.value = 
	    waypoints[i-1].fuel_amt.value - waypoints[i-1].fuel_leg.value;
      }
      if (waypoints[i].fuel_extra.valid)
	 waypoints[i].fuel_amt.value -= waypoints[i].fuel_extra.value;

   }

   /*
    * auto track:
    */
   if (auto_track)
      track_nav1 ();

   /*
    * VOR fixes:
    */
   for (i = 0; i < num_waypoints; i++) {
      for (vor = 0; vor < MAX_NUM_VOR_FIXES; vor++) {
	 if (waypoints[i].vor_fix[vor].db_valid) {
	    if (! waypoints[i].vor_fix[vor].fix_valid) {
	       if (waypoints[i].kind == WP_FROM) {
		  if (waypoints[i].tc.valid) {
		     compute_fix (waypoints[i].tc.value,
				  waypoints[i].db->latitude,
				  waypoints[i].db->longitude,
				  &waypoints[i].vor_fix[vor],
				  FROM);
		  }
	       } else {
		  if (i > 0) {
		     if (waypoints[i-1].tc.valid) {
			compute_fix (waypoints[i-1].tc.value,
				     waypoints[i].db->latitude,
				     waypoints[i].db->longitude,
				     &waypoints[i].vor_fix[vor],
				     TO);
		     }
		  }
	       }
	    }
	 }
      }
   }

   return TRUE;
}

