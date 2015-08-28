/*
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
 */

/* declare data structures */

typedef enum {
   WIDE,
   NARROW
} OUTPUT_FORMAT;

typedef enum {
   NAUTICAL,
   STATUTE
} OUTPUT_UNITS;

/*
 * The following macro converts both distances and speeds from the
 * units used internally (nautical miles and knots) to the desired
 * output units. The default is nautical miles and knots, or statute
 * miles and statute miles per hour when the -s option is used.
 */
#define NM_OR_MI(a) ((output_units == NAUTICAL) ? (a) : (a) * MI_PER_NM)

/* output.c */
extern OUTPUT_UNITS output_units;
extern void set_output_units ( int nautical );
extern char *format_time ( int fw, double hours );
extern char *format_lat_lon ( double latitude, double longitude );
extern char *format_db_type ( WP_TYPE type );

/* output_text.c */
extern void set_text_format ( int max_nav, BOOLEAN use_pc_graphic_chars );
extern void print_plan_text (void);
extern void put_db_text ( FILE *out_fp, DB_INFO *db );

