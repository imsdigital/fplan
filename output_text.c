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

static char rcsid[] = "$Id: output_text.c,v 1.4 1999/04/20 06:18:23 jcp Exp $";

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "output.h"

static char *headers[] = 
{
   "", "CITY, ST", "WAYPOINT", "COMMENTS", "ELEV", "FREQ", "TYPE",
   "IDENT", "TC", "MC", "MH", "WIND", "DIST", "TOTD", "REMD", "ETE",
   "ATE", "ETA", "ATA", "FUEL", "FRATE", "FUSED", "ALT", "TAS", "EGS",
   "AGS", "IDENT", "FREQ", "RAD", "LAT LON (DD.MMSS)", "DME", "", "",
   "", "", "", "", "", "", "", "", ""
};

typedef enum {
   END_OF_FORMAT,	/* we count on ((int)END_OF_FORMAT == 0) */
   CITY, NAME, COMMENT, ELEV, FREQ, TYPE, IDENT, TC, MC, MH, WIND,
   DIST_LEG, DIST, DIST_REMAIN, ETE_LEG, ATE_LEG, ETA, ATA, FUEL_AMT,
   FUEL_RATE, FUEL_LEG, ALT, TAS, EGS, AGS, VOR_IDENT, VOR_FREQ,
   VOR_RADIAL, LAT_LON, VOR_DME, FORWARD_ARROW, BACKWARD_ARROW,
   SEPARATOR, BAR, CROSS, VOR_ARROW, VOR_BAR, END_OF_VOR, IGNORE,
   EMPTY, EOL
} FORMAT_KIND;

typedef enum {R, L} JUSTIFICATION;

typedef enum {U, S} SHIFTED_OR_UNSHIFTED;

typedef struct {
   FORMAT_KIND		kind;
   int			field_width;
   JUSTIFICATION	justification;
   int			arg;		/* vor receiver or no decimal points */
   SHIFTED_OR_UNSHIFTED	shifted;
} FORMAT_SPEC_CELL;

/*
 * the default format (the VORs are not shown for narrow output)
 *
 *                                                              VOR1    VOR2
 *+---------//-------------+----+-----+                      +--------+--------+
 *|WAYPOINT // IDENT   FREQ|    | FUEL|                      |1  IDENT|2  IDENT|
 *|CITY, ST //  TYPE   ELEV|TDIS|  ETA|------+----+----+-----|    FREQ|    FREQ|
 *|COMMENT//LAT LON DD.MMSS|DREM|  ATA|  WIND| TAS|DIST|  ALT|RAD  DME|RAD  DME|
 *+-------//---------------+----+----->    MC| EGS| ETE|FRATE<--------+--------+
 *|       //               |    |     |    MH| AGS| ATE|FUSED|        |        |
 *|       //               |    |     |------+----+----+-----|        |        |
 *|       //               |    |     |      |    |    |     |        |        |
 *+-------//---------------+----+----->      |    |    |     <--------+--------+
 *
 */

#define MAX_NUM_FIELDS 256

static FORMAT_SPEC_CELL default_format_wide[] = 
{
   /* line 0 */
   /* WAYPOINT INFO */
   {CROSS}, {SEPARATOR,44,L},
   {CROSS}, {SEPARATOR,4},
   {CROSS}, {SEPARATOR,5},
   {FORWARD_ARROW},

   /* LEG INFO */
   {MC,6,R,0,S}, {BAR,1,R,0,S},
   {EGS,4,R,0,S}, {BAR,1,R,0,S},
   {ETE_LEG,4,R,0,S}, {BAR,1,R,0,S},
   {FUEL_RATE,5,R,1,S}, {VOR_ARROW,1,R,0,U},
   /* WAYPOINT INFO */
   {END_OF_VOR,1,R,0,U}, {SEPARATOR,8,R,0,U}, {CROSS},
   {END_OF_VOR,1,R,1,U}, {SEPARATOR,8,R,1,U}, {CROSS},
   {END_OF_VOR,1,R,2,U}, {SEPARATOR,8,R,2,U}, {CROSS},
   {END_OF_VOR,1,R,3,U}, {SEPARATOR,8,R,3,U}, {CROSS},
   {END_OF_VOR,1,R,4,U}, {SEPARATOR,8,R,4,U}, {CROSS},
   {END_OF_VOR,1,R,5,U}, {SEPARATOR,8,R,5,U}, {CROSS},
   {END_OF_VOR,1,R,6,U}, {SEPARATOR,8,R,6,U}, {CROSS},
   {END_OF_VOR,1,R,7,U}, {SEPARATOR,8,R,7,U}, {CROSS},
   {EOL}, 

   /* line 1 */
   /* WAYPOINT INFO */
   {BAR}, {NAME,31,L}, {IDENT,6}, {EMPTY,1}, {FREQ,6}, {BAR},
   {EMPTY,4}, {BAR},
   {FUEL_AMT,5,R,1}, {BAR},
   /* LEG INFO */
   {MH,6,R,0,S}, {BAR,1,R,0,S},
   {AGS,4,R,0,S}, {BAR,1,R,0,S},
   {ATE_LEG,4,R,0,S}, {BAR,1,R,0,S},
   {FUEL_LEG,5,R,1,S}, {VOR_BAR,1,R,0,U},
   /* WAYPOINT INFO */
   {VOR_IDENT,8,R,0,U}, {BAR},
   {VOR_IDENT,8,R,1,U}, {BAR},
   {VOR_IDENT,8,R,2,U}, {BAR},
   {VOR_IDENT,8,R,3,U}, {BAR},
   {VOR_IDENT,8,R,4,U}, {BAR},
   {VOR_IDENT,8,R,5,U}, {BAR},
   {VOR_IDENT,8,R,6,U}, {BAR},
   {VOR_IDENT,8,R,7,U}, {BAR},
   {EOL}, 

   /* line 2 */
   /* WAYPOINT INFO */
   {BAR}, {CITY,25,L}, {TYPE,12}, {EMPTY,1}, {ELEV,6}, {BAR},
   {DIST,4}, {BAR},
   {ETA,5}, {BAR},
   /* LEG INFO */
   {SEPARATOR,6,R,0,U}, {CROSS,1,R,0,U},
   {SEPARATOR,4,R,0,U}, {CROSS,1,R,0,U},
   {SEPARATOR,4,R,0,U}, {CROSS,1,R,0,U},
   {SEPARATOR,5,R,0,U}, {VOR_BAR,1,R,0,U},
   /* WAYPOINT INFO */
   {VOR_FREQ,8,R,0,U}, {BAR},
   {VOR_FREQ,8,R,1,U}, {BAR},
   {VOR_FREQ,8,R,2,U}, {BAR},
   {VOR_FREQ,8,R,3,U}, {BAR},
   {VOR_FREQ,8,R,4,U}, {BAR},
   {VOR_FREQ,8,R,5,U}, {BAR},
   {VOR_FREQ,8,R,6,U}, {BAR},
   {VOR_FREQ,8,R,7,U}, {BAR},
   {EOL}, 

   /* line 3 */
   /* WAYPOINT INFO */
   {BAR}, {COMMENT,25,L}, {LAT_LON,19}, {BAR},
   {DIST_REMAIN,4}, {BAR},
   {ATA,5}, {BAR},
   /* LEG INFO */
   {WIND,6,R,0,S}, {BAR,1,R,0,S},
   {TAS,4,R,0,S}, {BAR,1,R,0,S},
   {DIST_LEG,4,R,0,S}, {BAR,1,R,0,S},
   {ALT,5,R,0,S}, {VOR_BAR,1,R,0,U},
   /* WAYPOINT INFO */
   {VOR_RADIAL,4,L,0,U}, {VOR_DME,4,R,0,U}, {BAR},
   {VOR_RADIAL,4,L,1,U}, {VOR_DME,4,R,1,U}, {BAR},
   {VOR_RADIAL,4,L,2,U}, {VOR_DME,4,R,2,U}, {BAR},
   {VOR_RADIAL,4,L,3,U}, {VOR_DME,4,R,3,U}, {BAR},
   {VOR_RADIAL,4,L,4,U}, {VOR_DME,4,R,4,U}, {BAR},
   {VOR_RADIAL,4,L,5,U}, {VOR_DME,4,R,5,U}, {BAR},
   {VOR_RADIAL,4,L,6,U}, {VOR_DME,4,R,6,U}, {BAR},
   {VOR_RADIAL,4,L,7,U}, {VOR_DME,4,R,7,U}, {BAR},
   {EOL}, 

   {END_OF_FORMAT}
};

#define PC_GRAPHIC_V_BAR	0xB3
#define PC_GRAPHIC_H_BAR	0xC4
#define PC_GRAPHIC_CROSS	0xC5
#define PC_GRAPHIC_L_ARROW	0xC3
#define PC_GRAPHIC_R_ARROW	0xB4

#define V_BAR_CHAR	((use_graphic_chars) ? PC_GRAPHIC_V_BAR : '|')
#define H_BAR_CHAR	((use_graphic_chars) ? PC_GRAPHIC_H_BAR : '-')
#define CROSS_CHAR	((use_graphic_chars) ? PC_GRAPHIC_CROSS : '+')
#define L_ARROW_CHAR	((use_graphic_chars) ? PC_GRAPHIC_L_ARROW : '<')
#define R_ARROW_CHAR	((use_graphic_chars) ? PC_GRAPHIC_R_ARROW : '>')

static int line_num = 0;
static BOOLEAN use_graphic_chars = FALSE;
static FORMAT_SPEC_CELL output_format[MAX_NUM_FIELDS];


/*----------------------------------------------------------------------------*/
void 
set_text_format (int max_nav, BOOLEAN use_pc_graphic_chars)
{
   int i;
   FORMAT_SPEC_CELL *spec = default_format_wide;

   use_graphic_chars = use_pc_graphic_chars;

   for (i = 0; i < MAX_NUM_FIELDS; i++) {
      if (((spec[i].kind == END_OF_VOR)
	|| (spec[i].kind == VOR_IDENT)
	|| (spec[i].kind == VOR_FREQ)
	|| (spec[i].kind == VOR_DME)
	|| (spec[i].kind == VOR_RADIAL)) && (max_nav < spec[i].arg)) {
	 for (; (spec[i].kind != EOL) && (i < MAX_NUM_FIELDS); i++) {
	    output_format[i].kind = IGNORE;
	 }
	 i--;
      } else {
	 output_format[i] = spec[i];
      }
   }
}

/*----------------------------------------------------------------------------
 * The following functions print a string to the specified output
 * stream. In the event that the string is longer than the specified
 * field width, it is truncated accordingly with a '>' in the last
 * position.
 */

static char *
format_string(int fw, char *str)
{
   static char buffer[256];
   fw = ABS(fw);
   if (fw > 255) fw = 255;
   strncpy (buffer, str, sizeof(buffer));
   if (strlen (str) > fw)
      buffer[fw-1] = '>';
   buffer[fw] = '\0';
   return buffer;
}

static void 
put_string (FILE *out_fp, int fw, char *str)
{
   fprintf (out_fp, "%*s", fw, format_string(fw, str));
}

static void 
put_time (FILE *out_fp, int fw, double hours)
{
   fprintf (out_fp, "%*s", fw, format_time(fw, hours));
}

static void 
put_lat_lon (FILE *out_fp, int fw, double latitude, double longitude)
{
   fprintf (out_fp, "%*s", fw, format_lat_lon(latitude, longitude));
}

static void 
put_db_type (FILE *out_fp, int fw, WP_TYPE type)
{
   put_string (out_fp, fw, format_db_type(type));
}

/*----------------------------------------------------------------------------*/
static void 
put_header_cell (FILE *out_fp, FORMAT_SPEC_CELL cell, BOOLEAN force)
{
   int fw;

   fw = ((cell.justification == R) ? cell.field_width : -1 * cell.field_width);

   if ((cell.kind == IGNORE) || (cell.kind == END_OF_VOR))
      return;

   if (cell.kind == EOL) {
      line_num++;
      fprintf (out_fp, "\n");
      return;
   }

   if ((!force) && (cell.shifted == S) && ((line_num == 0)||(line_num == 1))) {
      put_string (out_fp, cell.field_width, "");
      return;
   }

   switch (cell.kind) {
   case BAR:
      fprintf (out_fp, "%c", V_BAR_CHAR);
      break;
   case FORWARD_ARROW:
      fprintf (out_fp, "%c", CROSS_CHAR);
      break;
   case BACKWARD_ARROW:
      fprintf (out_fp, "%c", CROSS_CHAR);
      break;
   case CROSS:
      fprintf (out_fp, "%c", CROSS_CHAR);
      break;
   case SEPARATOR:
      { 
	 int i;
	 for (i = 0; i < cell.field_width; i++)
	    fprintf (out_fp, "%c", H_BAR_CHAR);
      }
      break;
   case VOR_ARROW:
      if (max_nav > -1)
	 fprintf (out_fp, "%c", CROSS_CHAR);
      break;
   case VOR_BAR:
      if (line_num < 2) {
	 if (max_nav > -1)
	    fprintf (out_fp, "%c", V_BAR_CHAR);
      } else {
	 if (line_num == 2) {
	    if (max_nav > -1)
	       fprintf (out_fp, "%c", V_BAR_CHAR);
	    else
	       fprintf (out_fp, "%c", CROSS_CHAR);
	 } else {
	    fprintf (out_fp, "%c", V_BAR_CHAR);
	 }
      }
      break;
   case VOR_IDENT:
      fprintf (out_fp, "%1d", cell.arg + 1);
      put_string (out_fp, fw-1, headers[(int) cell.kind]);
      break;
   default:
      put_string (out_fp, fw, headers[(int) cell.kind]);
   }
}

/*----------------------------------------------------------------------------*/
static void 
put_info_cell (FILE *out_fp,
	       FORMAT_SPEC_CELL cell,
	       WP_INFO *prev_wp,
	       WP_INFO *curr_wp)
{ 
   char buffer[80];
   BOOLEAN valid;
   WP_INFO *wp;
   double d_val;
   char *s_val;
   int fw;

   valid = FALSE;
   s_val = (char *)NULL;
   fw = ((cell.justification == R) ? cell.field_width : -1 * cell.field_width);
   
   if ((cell.kind == IGNORE) || (cell.kind == END_OF_VOR))
      return;
   if ((cell.shifted == S) && ((line_num == 0) || (line_num == 1))) {
      if (! prev_wp) {
	 put_header_cell (out_fp, cell, TRUE);
	 return;
      } else
	 wp = prev_wp;
   } else
      wp = curr_wp;

   if ((cell.shifted == S) && (wp->kind == WP_TO)) {
      put_string (out_fp, cell.field_width, "");
      return;
   }

   switch (cell.kind) {
   case EOL:
      line_num++;
      fprintf (out_fp, "\n");
      break;
   case BAR:
      fprintf (out_fp, "%c", V_BAR_CHAR);
      break;
   case CROSS:
      fprintf (out_fp, "%c", CROSS_CHAR);
      break;
   case FORWARD_ARROW:
      fprintf (out_fp, "%c", R_ARROW_CHAR);
      break;
   case BACKWARD_ARROW:
      fprintf (out_fp, "%c", L_ARROW_CHAR);
      break;
   case SEPARATOR:
      { 
	 int i;
	 for (i = 0; i < cell.field_width; i++)
	    fprintf (out_fp, "%c", H_BAR_CHAR);
      }
      break;
   case VOR_ARROW:
      if (max_nav > -1)
	 fprintf (out_fp, "%c", L_ARROW_CHAR);
      else
	 fprintf (out_fp, "%c", V_BAR_CHAR);
      break;
   case VOR_BAR:
      if (wp->kind == WP_TO) {
	 if (line_num > 2) {
	    if (max_nav > -1)
	       fprintf (out_fp, "%c", V_BAR_CHAR);
	 } else
	    if (line_num == 2) {
	       if (max_nav > -1)
		  fprintf (out_fp, "%c", V_BAR_CHAR);
	       else
		  fprintf (out_fp, "%c", CROSS_CHAR);
	    } else {
	       fprintf (out_fp, "%c", V_BAR_CHAR);
	    }
      } else {
	 if (line_num == 2) {
	    if (max_nav > -1)
	       fprintf (out_fp, "%c", V_BAR_CHAR);
	    else
	       fprintf (out_fp, "%c", CROSS_CHAR);
	 } else {
	    fprintf (out_fp, "%c", V_BAR_CHAR);
	 }
      }
      break;
      /*
       * strings
       */
   case NAME:
   case IDENT:
   case VOR_IDENT:
   case CITY:
   case COMMENT:
      switch (cell.kind) {
      case NAME:
	 valid = TRUE;
	 s_val = wp->db->name;
	 break;
      case IDENT:
	 valid = TRUE;
	 s_val = wp->db->ident;
	 break;
      case VOR_IDENT:
	 valid = wp->vor_fix[cell.arg].db_valid;
	 if (valid)
	    s_val = wp->vor_fix[cell.arg].db->ident;
	 break;
      case CITY:
	 valid = TRUE;
	 s_val = wp->db->city;
	 break;
      case COMMENT:
	 valid = TRUE;
	 s_val = wp->db->comment;
	 break;
      default:
	 valid = FALSE;
	 s_val = NULL;
	 break;
      }
      if (! s_val)
	 valid = FALSE;
      if (valid) {
	 put_string (out_fp, fw, s_val);
      } else
	 fprintf (out_fp, "%*s", fw, "");
      break;
      /*
       * course and headings
       */
   case MC:
   case MH:
   case TC:
      switch (cell.kind) {
      case MC:
	 valid = wp->mc.valid;
	 d_val = wp->mc.value;
	 break;
      case MH:
	 valid = wp->mh.valid;
	 d_val = wp->mh.value;
	 break;
      case TC:
	 valid = wp->tc.valid;
	 d_val = wp->tc.value;
	 break;
      default:
	 valid = FALSE;
	 d_val = 0.0;
	 break;
      }
      if (valid) {
	 sprintf (buffer, "%03.*f", cell.arg, d_val);
	 fprintf (out_fp, "%*s", fw, buffer);
      } else {
	 fprintf (out_fp, "%*s", fw, "");
      }
      break;
      /*
       * frequencies
       */
   case FREQ:
   case VOR_FREQ:
      switch (cell.kind) {
      case FREQ:
	 valid = wp->db->freq.valid;
	 d_val = wp->db->freq.value;
	 break;
      case VOR_FREQ:
	 valid = wp->vor_fix[cell.arg].db_valid;
	 if (valid) 
	    d_val = wp->vor_fix[cell.arg].db->freq.value;
	 break;
      default:
	 valid = FALSE;
	 d_val = 0.0;
	 break;
      }
      if (valid) {
	 fprintf (out_fp, "%*.2f", fw, d_val);
      } else
	 fprintf (out_fp, "%*s", fw, "");
      break;
      /*
       * fuel amounts, rates
       */
   case FUEL_AMT:
   case FUEL_LEG:
   case FUEL_RATE:
      switch (cell.kind) {
      case FUEL_AMT:
	 valid = wp->fuel_amt.valid;
	 d_val = wp->fuel_amt.value;
	 break;
      case FUEL_LEG:
	 valid = wp->fuel_leg.valid;
	 d_val = wp->fuel_leg.value;
	 break;
      case FUEL_RATE:
	 valid = wp->fuel_rate.valid;
	 d_val = wp->fuel_rate.value;
	 break;
      default:
	 valid = FALSE;
	 d_val = 0.0;
	 break;
      }
      if (valid) {
	 if (d_val < pow (10.0, fw-cell.arg-1))
	    fprintf (out_fp, "%*.*f", fw, cell.arg, d_val);
	 else
	    fprintf (out_fp, "%*.0f", fw, d_val);
      } else
	 fprintf (out_fp, "%*s", fw, "");
      break;
      /*
       * altitude, elevations
       */
   case ALT:
   case ELEV:
      switch (cell.kind) {
      case ALT:
	 valid = wp->altitude.valid;
	 d_val = wp->altitude.value;
	 break;
      case ELEV:
	 valid = wp->db->altitude.valid;
	 d_val = wp->db->altitude.value;
	 break;
      default:
	 valid = FALSE;
	 d_val = 0.0;
	 break;
      }
      if (valid) {
	 fprintf (out_fp, "%*.0f", fw, d_val);
      } else {
	 fprintf (out_fp, "%*s", fw, "");
      }
      break;
      /*
       * distance, speeds
       */
   case DIST:
   case DIST_LEG:
   case DIST_REMAIN:
   case EGS:
   case TAS:
      switch (cell.kind) {
      case DIST:
	 valid = wp->dist.valid;
	 d_val = NM_OR_MI (wp->dist.value);
	 break;
      case DIST_LEG:
	 valid = wp->dist_leg.valid;
	 d_val = NM_OR_MI (wp->dist_leg.value);
	 break;
      case DIST_REMAIN:
	 valid = wp->dist_remain.valid;
	 d_val = NM_OR_MI (wp->dist_remain.value);
	 break;
      case EGS:
	 valid = wp->egs.valid;
	 d_val = NM_OR_MI (wp->egs.value);
	 break;
      case TAS:
	 valid = wp->tas.valid;
	 d_val = NM_OR_MI (wp->tas.value); 
	 break;
      default:
	 valid = FALSE;
	 d_val = 0.0;
	 break;
      }
      if (valid) {
	 fprintf (out_fp, "%*.*f", fw, cell.arg, d_val);
      } else
	 fprintf (out_fp, "%*s", fw, "");
      break;
      /*
       * times:
       */
   case ETA:
   case ETE_LEG:
      valid = (cell.kind == ETA) ? wp->eta.valid : wp->eta_leg.valid;
      d_val = (cell.kind == ETA) ? wp->eta.value : wp->eta_leg.value;
      if (valid)
	 put_time (out_fp, fw, d_val);
      else
	 fprintf (out_fp, "%*s", fw, "");
      break;
   case VOR_DME:
      if (wp->vor_fix[cell.arg].fix_valid)
	 if (wp->vor_fix[cell.arg].distance < 10.0)
	    fprintf (out_fp, "%*.1f", fw,
		     NM_OR_MI (wp->vor_fix[cell.arg].distance));
	 else
	    fprintf (out_fp, "%*.0f", fw,
		     NM_OR_MI (wp->vor_fix[cell.arg].distance));
      else
	 fprintf (out_fp, "%*s", fw, "");
      break;
   case VOR_RADIAL:
      if (wp->vor_fix[cell.arg].fix_valid) {
	 sprintf (buffer, "%03.0f%c", wp->vor_fix[cell.arg].radial, 
		  (wp->vor_fix[cell.arg].from_to == FROM ? 'F' : 'T') );
	 fprintf (out_fp, "%*s", fw, buffer);
      } else {
	 fprintf (out_fp, "%*s", fw, "");
      }
      break;
   case LAT_LON:
      put_lat_lon (out_fp, fw, wp->db->latitude, wp->db->longitude);
      break;
   case WIND:
      if (wp->wind_direction.valid && wp->wind_speed.valid) {
	 sprintf (buffer, "%03.0f@%02.0f",
		  wp->wind_direction.value, 
		  NM_OR_MI (wp->wind_speed.value));
	 fprintf (out_fp, "%*s", fw, buffer);
      } else {
	 fprintf (out_fp, "%*s", fw, "");
      }
      break;
   case TYPE:
      put_db_type (out_fp, fw, wp->db->type);
      break;
   case AGS:
   case ATA:
   case ATE_LEG:
   case EMPTY:
   default:
      fprintf (out_fp, "%*s", fw, "");
      break;
   }    
}

/*----------------------------------------------------------------------------*/
static void 
put_info_cells (FILE *out_fp,
		FORMAT_SPEC_CELL cells[],
		WP_INFO *prev_wp,
		WP_INFO *wp)
{
   int i;

   line_num = 0;
   for (i = 0; (cells[i].kind != END_OF_FORMAT); i++) 
      put_info_cell (out_fp, cells[i], prev_wp, wp);
}

/*----------------------------------------------------------------------------*/
static void 
put_header_cells (FILE *out_fp,
		  FORMAT_SPEC_CELL cells[],
		  BOOLEAN first_line_only)
{
   int i;

   line_num = 0;
   for (i = 0; (cells[i].kind != END_OF_FORMAT); i++) {
      if (first_line_only && line_num)
	 return;
      else
	 put_header_cell (out_fp, cells[i], FALSE);
   }
}

/*----------------------------------------------------------------------------*/
void 
print_plan_text (void)
{
   int i;

   for (i = 0; i < num_waypoints; i++) {
      if (waypoints[i].kind == WP_FROM) {
	 if (i)
	    fprintf (stdout, "\f");
	 printf ("\nSpeeds in %s; Distances in %s miles\n",
		 (output_units == NAUTICAL) ? "knots" : "mph (wind in knots)",
		 (output_units == NAUTICAL) ? "nautical" : "statute");
	 put_header_cells (stdout, output_format, FALSE);
      }
      put_info_cells (stdout, output_format,
		      (waypoints[i].kind == WP_FROM) ? 0 : &waypoints[i-1],
		      &waypoints[i]);
      if (waypoints[i].kind == WP_TO)
	 put_header_cells (stdout, output_format, TRUE);
   }
}

/*----------------------------------------------------------------------------*/
void 
put_db_text (FILE *out_fp, DB_INFO *db)
{
   fprintf (out_fp, "- - - - - - - - - - - - - - - - - - - - ");
   fprintf (out_fp, "- - - - - - - - - - - - - - - - - - - - ");
   fprintf (out_fp, "\n");
   fprintf (out_fp, "%s\t", db->ident);
   put_db_type (out_fp, -12, db->type);
   fprintf (out_fp, "\n");
   if (db->name) {
      if (db->name[0] != 0)
	 fprintf (out_fp, "NAME    : %s\n", db->name);
   }
   if (db->city) {
      if (db->city[0] != 0)
	 fprintf (out_fp, "CITY    : %s\n", db->city);
   }
   fprintf (out_fp, "LAT/LON : ");
   put_lat_lon (out_fp, 1, db->latitude, db->longitude);
   fprintf (out_fp, "\n");
   fprintf (out_fp, "MAG VAR : %.2f%c\n",
	    fabs(db->mag_variation), (db->mag_variation < 0.0) ? 'E' : 'W');
   if (db->altitude.valid)
      fprintf (out_fp, "ALTITUDE: %g FT\n", db->altitude.value);
   if (db->freq.valid)
      fprintf (out_fp, "FREQ    : %.2f\n", db->freq.value);
   if (db->comment)
      if (db->comment[0] != 0)
	 fprintf (out_fp, "COMMENTS: %s\n", db->comment);
}
