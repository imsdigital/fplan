%{
/*
 * $Id: fp_lex.l,v 2.7 1999/04/20 06:18:16 jcp Exp $
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

static char rcsid[] = "$Id: fp_lex.l,v 2.7 1999/04/20 06:18:16 jcp Exp $";

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "fp_tok.h"

double yydval;

%}

%%

\#		{
		  char c;
		  do {
		    c = input();
		  } while (c != '\n');
		}

from		return TOK_FROM;
via		return TOK_VIA;
to		return TOK_TO;

alt		return TOK_ALT;
comment		return TOK_COMMENT;
fuel_amount	return TOK_FUEL_AMOUNT;
fuel_rate	return TOK_FUEL_RATE;
fuel_used	return TOK_FUEL_USED;
nav		return TOK_NAV;
tas		return TOK_TAS;
wind		return TOK_WIND;

mi		return TOK_SMI;
smi		return TOK_SMI;
mph		return TOK_MPH;
nm		return TOK_NMI;
nmi		return TOK_NMI;
knots		return TOK_KTS;
kts		return TOK_KTS;

north		return TOK_NORTH;
south		return TOK_SOUTH;
east		return TOK_EAST;
west		return TOK_WEST;

\@		return TOK_ATSIGN;
\:		return TOK_COLON;
\/		return TOK_FSLASH;
\(		return TOK_LPAREN;
\)		return TOK_RPAREN;
\;		return TOK_SEMICOLON;

[\+\-]?[0-9]+\.[0-9]+	{ yydval = strtod (yytext, (char **) NULL);
			  /* error checking not required (lex did it) */
			  return TOK_REAL;
			};

[\+\-]?[0-9]+   	{ yydval = strtod (yytext, (char **) NULL);
			  /* error checking not required (lex did it) */
			  return TOK_INTEGER;
			};

\"		{ int i;
		  for (i = 0; (yytext[i] = input()) != '\"'; i++) {
		     if (yytext[i] == '\"')
			break;
		     if (yytext[i] == '\\')
			yytext[i] = input();
		  }
		  yytext[i] = '\0';
		  return TOK_STRING;
		};

\_?[0-9A-Za-z]+ return TOK_IDENT; /* allow leading _ for personal waypoints */

[ ,\t\n]       	;	/* ignore whitespace characters */
.		;	/* ignore anything else too */
%%
