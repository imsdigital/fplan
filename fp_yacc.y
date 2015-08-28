%{
/*
 * $Id: fp_yacc.y,v 2.7 1999/04/20 06:18:17 jcp Exp $
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

static char rcsid[] = "$Id: fp_yacc.y,v 2.7 1999/04/20 06:18:17 jcp Exp $";

#include <stdio.h>
#include <string.h>

#include "common.h"

%}

%union {
   int		ival;
   double	dval;
   char		*sval;
   WP_KIND	kval;
}

%type <sval> string
%type <sval> optional_string
%type <sval> identifier
%type <kval> waypoint_kind
%type <dval> latitude
%type <dval> longitude
%type <dval> mag_variation
%type <dval> distance
%type <dval> speed
%type <dval> direction
%type <dval> deg_min_sec
%type <dval> non_neg_number
%type <dval> pos_number
%type <dval> number
%type <ival> pos_integer
%type <ival> integer

%token TOK_FROM
%token TOK_VIA
%token TOK_TO

%token TOK_ALT
%token TOK_COMMENT
%token TOK_FUEL_AMOUNT
%token TOK_FUEL_RATE
%token TOK_FUEL_USED
%token TOK_NAV
%token TOK_TAS
%token TOK_WIND

%token TOK_SMI
%token TOK_MPH
%token TOK_NMI
%token TOK_KTS

%token TOK_NORTH
%token TOK_SOUTH
%token TOK_EAST
%token TOK_WEST

%token TOK_ATSIGN
%token TOK_COLON
%token TOK_FSLASH
%token TOK_LPAREN
%token TOK_RPAREN
%token TOK_SEMICOLON

%token TOK_REAL
%token TOK_INTEGER
%token TOK_STRING
%token TOK_IDENT

%start fplan

%%

fplan		: directives
		;

directives 	: /* empty */
		| directives directive
		;

directive	: named_waypoint	TOK_SEMICOLON
		| inc_waypoint		TOK_SEMICOLON
		| int_waypoint		TOK_SEMICOLON
		| rel_waypoint		TOK_SEMICOLON
		| lat_waypoint		TOK_SEMICOLON
		| comment		TOK_SEMICOLON
		| flight_altitude	TOK_SEMICOLON
		| fuel_amount		TOK_SEMICOLON
		| fuel_rate		TOK_SEMICOLON
		| fuel_used		TOK_SEMICOLON
		| true_airspeed		TOK_SEMICOLON
		| winds_aloft		TOK_SEMICOLON
		| nav_fix		TOK_SEMICOLON
		| error			TOK_SEMICOLON
		;

named_waypoint	: waypoint_kind identifier
			{
			  add_named_waypoint ($1, $2);
			}

inc_waypoint	: waypoint_kind distance
		  optional_string optional_string optional_string
			{
			  add_inc_waypoint ($1, $2, $3, $4, $5);
			}
		;

int_waypoint	: waypoint_kind
		  identifier direction
		  identifier direction
		  optional_string optional_string optional_string
			{
			  add_int_waypoint ($1, $2, $3, $4, $5, $6, $7, $8);
			}
		;

rel_waypoint	: waypoint_kind identifier direction TOK_FSLASH distance
		  optional_string optional_string optional_string
			{
			  add_rel_waypoint ($1, $2, $3, $5, $6, $7, $8);
			}
		;

lat_waypoint	: waypoint_kind latitude longitude mag_variation
		  optional_string optional_string optional_string
			{
			  add_lat_waypoint ($1, $2, $3, $4, $5, $6, $7);
			}
		;

comment		: TOK_COMMENT string		{ set_comment ($2); }
		;

fuel_amount	: TOK_FUEL_AMOUNT pos_number	{ set_fuel_amt ($2); }
		;

fuel_rate	: TOK_FUEL_RATE pos_number	{ set_fuel_rate ($2); }
		;

fuel_used	: TOK_FUEL_USED non_neg_number	{ set_fuel_extra ($2); }
		;

flight_altitude	: TOK_ALT number	{ set_altitude ($2); }
		;

true_airspeed	: TOK_TAS speed		{ set_tas ($2); }
		;

winds_aloft	: TOK_WIND direction TOK_ATSIGN speed
					{
					  set_wind ($2, $4);
					}
		;

nav_fix		: TOK_NAV pos_integer identifier
					{
					  set_xfix ($2, $3);
					}
		;

string		: TOK_STRING		{ $$ = strdup (yytext); }
		;

optional_string	: /* empty */		{ $$ = (char*) NULL; }
		| TOK_STRING		{ $$ = strdup (yytext); }
		;

identifier	: TOK_IDENT	    	{ $$ = strdup (yytext); }
		;

waypoint_kind	: TOK_FROM		{ $$ = WP_FROM; }
		| TOK_TO		{ $$ = WP_TO; }
		| TOK_VIA		{ $$ = WP_VIA; }
		;

latitude	: deg_min_sec TOK_NORTH
			{
			  if (($1 < 0.0) || ($1 > 90.0))
			    yyerror ("expecting latitude in interval [0,90]");
			  $$ = $1;
			}
		| deg_min_sec TOK_SOUTH
			{
			  if (($1 < 0.0) || ($1 > 90.0))
			    yyerror ("expecting latitude in interval [0,90]");
			  $$ = -$1;
			}
		;

longitude	: deg_min_sec TOK_EAST
			{
			  if (($1 < 0.0) || ($1 > 180.0))
			    yyerror ("expecting longitude in interval [0,180]");
			  $$ = -$1;
			}
		| deg_min_sec TOK_WEST
			{
			  if (($1 < 0.0) || ($1 > 180.0))
			    yyerror ("expecting longitude in interval [0,180]");
			  $$ = $1;
			}
		;

mag_variation	: non_neg_number TOK_EAST
			{
			  if (($1 < 0.0) || ($1 > 180.0))
			    yyerror ("expecting variation in interval [0,180]");
			  $$ = -$1;
			}
		| non_neg_number TOK_WEST
			{
			  if (($1 < 0.0) || ($1 > 180.0))
			    yyerror ("expecting variation in interval [0,180]");
			  $$ = $1;
			}
		;

distance	: number TOK_SMI	{ $$ = $1 / MI_PER_NM; }
		| number TOK_NMI	{ $$ = $1; }
		| number		{ $$ = $1; }
		;

speed		: pos_number TOK_MPH	{ $$ = $1 / MI_PER_NM; }
		| pos_number TOK_KTS	{ $$ = $1; }
		| pos_number		{ $$ = $1; }
		;

direction	: number
			{
			  if (($1 < 0.0) || ($1 > 360.0))
			    yyerror ("expecting direction in interval [0,360]");
			  $$ = $1;
			}
		;

deg_min_sec	: integer TOK_COLON integer TOK_COLON non_neg_number
			{
			  if (($5 < 0.0) || ($5 >= 60.0))
			    yyerror ("expecting seconds in interval [0,60)");
			  if (($3 < 0.0) || ($3 >= 60.0))
			    yyerror ("expecting minutes in interval [0,60)");
			  $$ = ((double)$5/3600.0) + ((double)$3/60.0) + $1;
			}
		;

number		: TOK_REAL		{ $$ = yydval; }
		| TOK_INTEGER		{ $$ = yydval; }
		;

non_neg_number	: TOK_REAL
			{
			  if (yydval >= 0.0)
			    $$ = yydval;
			  else
			    yyerror ("expecting non-negative number");
			}
		| TOK_INTEGER
			{
			  if (yydval >= 0.0)
			    $$ = yydval;
			  else
			    yyerror ("expecting non-negative number");
			}
		;

pos_number	: TOK_REAL
			{
			  if (yydval > 0.0)
			    $$ = yydval;
			  else
			    yyerror ("expecting positive number");
			}
		| TOK_INTEGER
			{
			  if (yydval > 0.0)
			    $$ = yydval;
			  else
			    yyerror ("expecting positive number");
			}
		;

integer		: TOK_INTEGER		{ $$ = yydval; }
		;

pos_integer	: TOK_INTEGER
			{
			  if (yydval > 0.0)
			    $$ = yydval;
			  else
			    yyerror ("expecting positive integer");
			}
		;

