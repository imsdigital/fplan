/*
 * $Id: misc.c,v 2.8 1999/04/20 06:18:21 jcp Exp $
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

static char rcsid[] = "$Id: misc.c,v 2.8 1999/04/20 06:18:21 jcp Exp $";

#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include "common.h"

/*----------------------------------------------------------------------------*/
void 
decimal_2_deg_min_sec (double dec, int *deg, int *min, int *sec)
{
   int ideg;
   int imin;
   int isec;
   long nsec;

   dec = fabs (dec);			/* ignore the sign */

   nsec = ROUND (3600.0 * dec);		/* convert to seconds and round */

   ideg = nsec / 3600L;
   nsec -= ideg * 3600L;
   imin = nsec / 60L;
   nsec -= imin * 60L;
   isec = nsec;

   *deg = ideg;
   *min = imin;
   *sec = isec;
}

/* -------------------------------------------------------------------------- */
char *
is_kident(char *ident)
{
   if (ident) {
      if ((strlen(ident) == 4) && (ident[0] == 'K')) {
	 /* test for all upper case alpha characters */
	 if (isupper(ident[1]) && isupper(ident[2]) && isupper(ident[3])) {
	    return &ident[1];
	 }
      }
   }

   return (char *)NULL;
}

/*----------------------------------------------------------------------------
 * Previous versions of fplan used strtok() to parse records from the
 * database.  However, since the database often has empty fields, (ie;
 * sequences like ":::"), the (correct) behavior of strtok() for empty
 * fields is to process any number of delimiters until finding a valid
 * token (*nonempty* string of characters that are not delimiters).
 * The following function is a sort of "replacement" for strtok() that
 * behaves the way we want. Note that it is "hard wired" for the set of
 * delimiters for our application (':' and '\n' to be specific).
 */

static char *tok_ptr;	/* to hold pointer to next char between calls */

char *
str_tok (char *str)
{
   char *ptr = (char *) "";

   if (str)
     tok_ptr = str;

   if (tok_ptr) {
      ptr = tok_ptr;
      do {
	 if ((*tok_ptr == ':') || (*tok_ptr == '\n')) {
	    *(tok_ptr++) = '\0';
	    return ptr;
	 }
      } while (*(tok_ptr++) != '\0');
      /* reset internal pointer */
      tok_ptr = (char *) NULL;
   }

   return ptr;
}

/*----------------------------------------------------------------------------*/
void 
str_upcase (char *str)
{
   int i;

   for (i = 0; str[i] != '\0'; i++)
      if (islower (str[i]))
	  str[i] = toupper (str[i]);
}

/*----------------------------------------------------------------------------*/
void 
min_max_lat_lon (double *min_lat, double *max_lat,
		 double *min_lon, double *max_lon)
{
   int i;

   *min_lat =  360.0;
   *max_lat = -360.0;
   *min_lon =  360.0;
   *max_lon = -360.0;

   for (i = 0; i < num_waypoints; i++) {
      *min_lat = MIN (waypoints[i].db->latitude, *min_lat);
      *max_lat = MAX (waypoints[i].db->latitude, *max_lat);
      *min_lon = MIN (waypoints[i].db->longitude, *min_lon);
      *max_lon = MAX (waypoints[i].db->longitude, *max_lon);
   }

   for (i = 0; i < num_cached; i++) {
      *min_lat = MIN (cache[i]->latitude, *min_lat);
      *max_lat = MAX (cache[i]->latitude, *max_lat);
      *min_lon = MIN (cache[i]->longitude, *min_lon);
      *max_lon = MAX (cache[i]->longitude, *max_lon);
   }
}
