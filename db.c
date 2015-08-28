/*
 * $Id: db.c,v 2.15 1999/04/20 06:18:15 jcp Exp $
 *
 * Copyright (C) 1998,1999 John C. Peterson <mailto:jaypee@netcom.com>
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

static char rcsid[] = "$Id: db.c,v 2.15 1999/04/20 06:18:15 jcp Exp $";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>

#include "common.h"
#include "config.h"
#include "output.h"

/*
 * the database file pointers
 */

typedef enum {
   AIRPORT,
   VOR
} DB_KIND;

typedef struct {
   FILE *fp;
   char *filename;
   DB_KIND db_kind;
   long rec_size;
   long num_recs;
   long file_size;
} DB_FILE;

static DB_FILE pub_airports, pvt_airports, pub_vors, pvt_vors;

#define FSEEK_FROM_BOF	SEEK_SET
#define FSEEK_FROM_CUR	SEEK_CUR
#define FSEEK_FROM_EOF	SEEK_END

/*----------------------------------------------------------------------------*/
#ifdef BUFSIZE
#undef BUFSIZE
#endif

#define BUFSIZE 2048

static void
get_rec_size (DB_FILE *db_file)
{
   char buffer[BUFSIZE];
   long i;

   fgets (buffer, BUFSIZE, db_file->fp);

   for (i = 0; i < BUFSIZE; i++) {
      if (buffer[i] == '\n') {
	 db_file->rec_size = i + 1L;
	 return;
      }
   }

   fprintf (stderr,
	    "ERROR: record length > internal buffer size, db file: %s\n",
	    db_file->filename);
   fclose (db_file->fp);
   db_file->fp = NULL;
}

/*----------------------------------------------------------------------------*/
static void
get_num_recs (DB_FILE *db_file)
{
   long file_size;

   if (! db_file->fp)
      return;

   if (fseek (db_file->fp, 0L, FSEEK_FROM_EOF)) {
      fprintf (stderr,
	       "ERROR: fseek to end of db file: %s failed\n",
	       db_file->filename);
      fclose (db_file->fp);
      db_file->fp = NULL;
      return;
   }

   file_size = ftell (db_file->fp);

   /*
    * verify the file size is an integer multiple of the record length
    */

   if ((file_size % db_file->rec_size) != 0L) {
      fprintf (stderr, 
	       "ERROR: inconsistent file, record sizes for db file: %s\n", 
	       db_file->filename);
      fprintf (stderr, 
	       "file size = %ld, record size = %ld\n", 
	       file_size, db_file->rec_size);
      fclose (db_file->fp);
      db_file->fp = NULL;
      return;
   }

   db_file->num_recs = (file_size / db_file->rec_size);
   db_file->file_size = file_size;
}

/*----------------------------------------------------------------------------*/
BOOLEAN open_dbs (void)
{
   char filename[MAXPATHLEN];
   char *pub_dir;
   char *pvt_dir;
   BOOLEAN ok;

#if defined(OS2) || defined(MSDOS)
#define OPEN_MODE "rb"
#else
#define OPEN_MODE "r"
#endif

   if (! (pub_dir = getenv (FPLAN_COMMON_DBDIR)))
      pub_dir = DEFAULT_COMMON_DBDIR;

   if (! (pvt_dir = getenv (FPLAN_USER_DBDIR)))
      pvt_dir = DEFAULT_USER_DBDIR;

#if !defined(MSDOS)
   if (pvt_dir[0] == '~') {
      strcpy (filename, getenv (HOME));
      strcat (filename, &pvt_dir[1]);
      pvt_dir = strdup (filename);
   }
#endif

   strcpy (filename, pub_dir);
   strcat (filename, AIRPORTS_NAME);
   pub_airports.filename = strdup (filename);
   pub_airports.db_kind = AIRPORT;

   if ((pub_airports.fp = fopen (filename, OPEN_MODE))) {
      get_rec_size (&pub_airports);
      get_num_recs (&pub_airports);
   }

   strcpy (filename, pvt_dir);
   strcat (filename, AIRPORTS_NAME);
   pvt_airports.filename = strdup (filename);
   pvt_airports.db_kind = AIRPORT;

   if ((pvt_airports.fp = fopen (filename, OPEN_MODE))) {
      get_rec_size (&pvt_airports);
      get_num_recs (&pvt_airports);
   }

   strcpy (filename, pub_dir);
   strcat (filename, VORS_NAME);
   pub_vors.filename = strdup (filename);
   pub_vors.db_kind = VOR;

   if ((pub_vors.fp = fopen (filename, OPEN_MODE))) {
      get_rec_size (&pub_vors);
      get_num_recs (&pub_vors);
   }

   strcpy (filename, pvt_dir);
   strcat (filename, VORS_NAME);
   pvt_vors.filename = strdup (filename);
   pvt_vors.db_kind = VOR;

   if ((pvt_vors.fp = fopen (filename, OPEN_MODE))) {
      get_rec_size (&pvt_vors);
      get_num_recs (&pvt_vors);
   }

   ok = TRUE;

   if ((! pub_airports.fp) && (! pvt_airports.fp)) {
      fprintf (stderr, "ERROR: neither common or personal airports db found\n");
      ok = FALSE;
   }

   if ((! pub_vors.fp) && (! pvt_vors.fp)) {
      fprintf (stderr, "ERROR: neither common or personal vors db found\n");
      ok = FALSE;
   }

   return (ok);
}

/*----------------------------------------------------------------------------*/
BOOLEAN close_dbs (void)
{
   if (pub_airports.fp)
      fclose (pub_airports.fp);
   if (pvt_airports.fp)
      fclose (pvt_airports.fp);
   if (pub_vors.fp)
      fclose (pub_vors.fp);
   if (pvt_vors.fp)
      fclose (pvt_vors.fp);

   return (TRUE);
}

/*----------------------------------------------------------------------------
 * This function decodes the next two fields from the current database
 * record, which are assumed to be the degrees and minutes fields of a
 * magnetic variation, latitude, or longitude. The sign of the degrees
 * entry is inherited by the minutes field. This is actually a little more
 * tricky than it seems at first blush. An example of where it gets tricky;
 * consider the case of -0 degrees, 30 minutes. (The sign gets "lost" in a
 * simple numeric conversion of -0).
 */

static BOOLEAN
decode_deg_min (double *value)
{
   char sign;
   char *tok;
   char *ptr;
   double deg;
   double min;

   /*
    * parse degrees field
    */

   tok = str_tok ((char*)NULL);

   if (tok[0] != '\0') {
      deg = strtod (tok, &ptr);
      if (ptr == tok) {
	 fprintf (stderr, "error in numeric conversion of degrees field\n");
	 return FALSE;
      }
      if (fabs (deg) > 180.0) {
	 fprintf (stderr, "expecting degrees in interval [-180, +180]\n");
	 return FALSE;
      }
   } else {
      fprintf (stderr, "degrees field is empty\n");
      return FALSE;
   }

   sign = (strchr (tok, '-') != (char*)NULL) ? ('-') : ('+');

   /*
    * parse minutes field
    */

   tok = str_tok ((char*)NULL);

   if (tok[0] != '\0') {
      min = strtod (tok, &ptr);
      if (ptr == tok) {
	 fprintf (stderr, "error in numeric conversion of minutes field\n");
	 return FALSE;
      }
      if ((min < 0.0) || (min > 60.0)) {
	 fprintf (stderr, "expecting minutes in interval [0, 60]\n");
	 return FALSE;
      }
   } else {
      fprintf (stderr, "minutes field is empty\n");
      return FALSE;
   }

   /*
    * compute signed decimal degrees
    */

   *value = ((sign == '-') ? (-1.0) : (+1.0)) * (fabs (deg) + (min / 60.0));

   return TRUE;
}

/*----------------------------------------------------------------------------*/
static BOOLEAN
decode_common (DB_INFO *db)
{
   char *tok;
   char *ptr;
   double deg;

   /*
    * parse altitude
    */

   tok = str_tok ((char*)NULL);

   if (tok[0] != '\0') {
      db->altitude.value = strtod (tok, &ptr);
      if (ptr == tok)
         db->altitude.valid = FALSE;
      else
         db->altitude.valid = TRUE;
   } else
      db->altitude.valid = FALSE;

   /*
    * parse magnetic variation
    */

   if (! decode_deg_min (&deg)) {
      fprintf (stderr, "error while decoding magnetic variation\n");
      return FALSE;
   } else {
      db->mag_variation = deg;
   }

   /*
    * parse latitude
    */

   if (! decode_deg_min (&deg)) {
      fprintf (stderr, "error while decoding latitude\n");
      return FALSE;
   } else {
      db->latitude = deg;
   }

   /*
    * parse longitude
    */

   if (! decode_deg_min (&deg)) {
      fprintf (stderr, "error while decoding longitude\n");
      return FALSE;
   } else {
      db->longitude = deg;
   }

   return TRUE;
}

/*----------------------------------------------------------------------------*/
static BOOLEAN 
decode_airport (DB_INFO *db)
{
   char *tok;
   char *ptr;

   db->type = WP_AIRPORT;
   db->city = strdup (str_tok ((char*)NULL));   
   db->name = strdup (str_tok ((char*)NULL));   

   if (! decode_common (db))
      return FALSE;

   tok = str_tok ((char*)NULL);   

   if (tok[0] != '\0') {
      db->freq.value = strtod (tok, &ptr);
      if (ptr == tok)
         db->freq.valid = FALSE;
      else
         db->freq.valid = TRUE;
   } else
      db->freq.valid = FALSE;

   db->comment = strdup (str_tok ((char*)NULL));

   return TRUE;
}

/*----------------------------------------------------------------------------*/
static BOOLEAN 
decode_vor (DB_INFO *db)
{
   char *tok;
   char *ptr;

   db->name = strdup (str_tok ((char*)NULL));
   db->city = "";

   tok = str_tok ((char*)NULL);

   if (tok[0] != '\0') {
      db->freq.value = strtod (tok, &ptr);
      if (ptr == tok)
         db->freq.valid = FALSE;
      else
         db->freq.valid = TRUE;
   } else
      db->freq.valid = FALSE;

   if (! decode_common (db))
      return FALSE;

   tok = str_tok ((char*)NULL);

   if (!strcmp ("VOR", tok))
      db->type = WP_VOR;
   else if (!strcmp ("DME", tok))
      db->type = WP_DME;
   else if (!strcmp ("TAC", tok))
      db->type = WP_TAC;
   else if (!strcmp ("ILS", tok))
      db->type = WP_ILS;
   else if (!strcmp ("LOM", tok))
      db->type = WP_LOM;
   else if (!strcmp ("LMM", tok))
      db->type = WP_LMM;
   else if (!strcmp ("NDB", tok))
      db->type = WP_NDB;
   else if (!strcmp ("INT", tok))
      db->type = WP_INT;
   else if (!strcmp ("WPT", tok))
      db->type = WP_WPT;
   else 
      db->type = WP_UNK;

   db->comment = strdup (str_tok ((char*)NULL));

   return TRUE;
}

/*----------------------------------------------------------------------------*/
static BOOLEAN
lookup_ident_dbfile (DB_FILE db, char *ident, int  buffer_size, char *buffer)
{
   long high;
   long mid;
   long low;
   int cmp;

   if (! db.fp) 
      return FALSE;

#ifdef NO_BINARY_SEARCH
   rewind (db.fp);   
   while (fgets (buffer, buffer_size, db.fp)) {
      if (! strcmp (ident, str_tok (buffer)))
	 return TRUE;
   }
   return FALSE;
#else
   low = 0L;
   high = db.num_recs;
   while (low <= high) {
      mid = (low + high) / 2L;
      fseek (db.fp, mid * db.rec_size, FSEEK_FROM_BOF);
      fgets (buffer, buffer_size, db.fp);
      cmp = strcmp (ident, str_tok (buffer));
      if (! cmp)
	 return TRUE;
      else if (cmp > 0)
	 low = mid + 1;
      else
	 high = mid - 1;
   }
   return FALSE;
#endif
}

/*----------------------------------------------------------------------------*/
static BOOLEAN
in_cache (WP_TYPE type, char *ident)
{
   int i;

   for (i = 0; i < num_cached; i++) {
      if ((!strcmp (ident, cache[i]->ident)) && (type == cache[i]->type)) {
	 return TRUE;
      }
   }

   return FALSE;
}

/*----------------------------------------------------------------------------*/
BOOLEAN lookup_ident (WP_KIND kind, char *ident, DB_INFO **db)
{
   char buffer[BUFSIZE];
   char *filename;
   DB_FILE fps[4];
   DB_KIND db_kind;
   BOOLEAN found;
   int i;

   str_upcase (ident);

   if (kind == WP_VIA) {
      /* 
       * search the VOR databases first
       */
      fps[0] = pvt_vors;
      fps[1] = pub_vors;
      fps[2] = pvt_airports;
      fps[3] = pub_airports;
   } else {
      /* 
       * search the AIRPORT databases first
       */
      fps[0] = pvt_airports;
      fps[1] = pub_airports;
      fps[2] = pvt_vors;
      fps[3] = pub_vors;
   }

   /*
    * search all four databases
    */

   for (i = 0; i < 4; i++) {
      if ((found = lookup_ident_dbfile (fps[i], ident, BUFSIZE, buffer))) {
	 filename = fps[i].filename;
	 db_kind = fps[i].db_kind;
	 break;
      }
   }

   if (! found) {
      return FALSE;
   }

   /*
    * parse the record and store in db struct
    */

   if (! (*db = (DB_INFO*) malloc (sizeof (DB_INFO)))) {
      fprintf (stderr,
	       "ERROR: memory allocation failure for db element, ident:%s\n",
	       ident);
      return FALSE;
   }

   (*db)->ident = ident;	/* no need to strdup - the parser already did */

   switch (db_kind) {
   case AIRPORT:
      if (! decode_airport (*db)) {
	 fprintf (stderr, "unable to parse db record for: %s\n", ident);
	 fprintf (stderr, "ERROR: bad records in db file %s\n", filename);
	 return FALSE;
      }
      break;
   case VOR:
      if (! decode_vor (*db)) {
	 fprintf (stderr, "unable to parse db record for: %s\n", ident);
	 fprintf (stderr, "ERROR: bad records in db file %s\n", filename);
	 return FALSE;
      }
      break;
   }

   /*
    * append to the cache: (NOTE: it might already be there if we didn't
    * grab it out of the cache because of incompatible types - e.g. the
    * cached wp is an airport, but this is a via node - we forced a
    * recheck in case there was a navaid with the same identifier)
    */

   if (num_cached < CACHE_SIZE) {
      if (! in_cache ((*db)->type, ident)) {
	 cache[num_cached++] = *db;
      }
   }

   return TRUE;
}

/*----------------------------------------------------------------------------*/
static int 
db_compare (const void *arg1, const void *arg2)
{
   const DB_INFO **db1 = (const DB_INFO **) arg1;
   const DB_INFO **db2 = (const DB_INFO **) arg2;
   return (strcmp ((*db1)->ident, (*db2)->ident));
}

/*----------------------------------------------------------------------------*/
void 
put_db_summary (FILE *out_fp)
{
   int i;

   qsort ((char*) &cache[0], num_cached, sizeof (DB_INFO*), db_compare);

   fprintf (out_fp, "\f\n");
   for (i = 0; i < num_cached; i++)
      put_db_text (out_fp, cache[i]);
}
