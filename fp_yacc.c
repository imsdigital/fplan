#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "fp_yacc.y"
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

#line 59 "fp_yacc.y"
typedef union {
   int		ival;
   double	dval;
   char		*sval;
   WP_KIND	kval;
} YYSTYPE;
#line 75 "y.tab.c"
#define TOK_FROM 257
#define TOK_VIA 258
#define TOK_TO 259
#define TOK_ALT 260
#define TOK_COMMENT 261
#define TOK_FUEL_AMOUNT 262
#define TOK_FUEL_RATE 263
#define TOK_FUEL_USED 264
#define TOK_NAV 265
#define TOK_TAS 266
#define TOK_WIND 267
#define TOK_SMI 268
#define TOK_MPH 269
#define TOK_NMI 270
#define TOK_KTS 271
#define TOK_NORTH 272
#define TOK_SOUTH 273
#define TOK_EAST 274
#define TOK_WEST 275
#define TOK_ATSIGN 276
#define TOK_COLON 277
#define TOK_FSLASH 278
#define TOK_LPAREN 279
#define TOK_RPAREN 280
#define TOK_SEMICOLON 281
#define TOK_REAL 282
#define TOK_INTEGER 283
#define TOK_STRING 284
#define TOK_IDENT 285
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,   17,   17,   18,   18,   18,   18,   18,   18,   18,
   18,   18,   18,   18,   18,   18,   18,   19,   20,   21,
   22,   23,   24,   26,   27,   28,   25,   29,   30,   31,
    1,    2,    2,    3,    4,    4,    4,    5,    5,    6,
    6,    7,    7,    8,    8,    8,    9,    9,    9,   10,
   11,   14,   14,   12,   12,   13,   13,   16,   15,
};
short yylen[] = {                                         2,
    1,    0,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    5,    8,
    8,    7,    2,    2,    2,    2,    2,    2,    4,    3,
    1,    0,    1,    1,    1,    1,    1,    2,    2,    2,
    2,    2,    2,    2,    2,    1,    2,    2,    1,    1,
    5,    1,    1,    1,    1,    1,    1,    1,    1,
};
short yydefred[] = {                                      2,
    0,    0,    0,   35,   37,   36,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    3,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   17,
   52,   53,   27,   31,   23,   56,   57,   24,   25,   54,
   55,   26,   59,    0,   28,    0,    0,   50,    0,   34,
    0,    0,    0,    0,    0,    0,    4,    5,    6,    7,
    8,    9,   10,   11,   12,   13,   14,   15,   16,   30,
   47,   48,    0,    0,   58,    0,    0,   33,    0,   38,
   39,   44,   45,    0,   29,    0,    0,    0,    0,   40,
   41,    0,    0,    0,    0,    0,   42,   43,   19,    0,
    0,    0,    0,   51,    0,    0,   22,   21,   20,
};
short yydgoto[] = {                                       1,
   35,   79,   51,   15,   52,   76,   88,   53,   45,   47,
   54,   42,   46,   48,   44,   56,    2,   16,   17,   18,
   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
   29,
};
short yysindex[] = {                                      0,
    0, -217, -276,    0,    0,    0, -249, -248, -231, -231,
 -229, -228, -231, -249, -268,    0, -223, -214, -212, -211,
 -210, -209, -208, -207, -206, -205, -204, -203, -201,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -220,    0, -260, -195,    0,    0,    0,
 -249, -200, -218, -216, -247, -193,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -231, -272,    0, -229, -215,    0, -218,    0,
    0,    0,    0, -200,    0, -249, -249, -218, -213,    0,
    0, -218, -192, -218, -218, -218,    0,    0,    0, -229,
 -218, -218, -218,    0, -218, -218,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,   68,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -199,    0,    0, -246,    0,
 -194,    0, -191,    0, -274,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -191,    0,
    0,    0,    0,    0,    0,    0,    0, -191,    0,    0,
    0, -191,    0, -191, -191, -191,    0,    0,    0,    0,
 -191, -191, -191,    0, -191, -191,    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,  -76,  -42,    0,    0,    0,    0,    2,   13,  -50,
   37,  -72,   54,   -7,    0,    7,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
#define YYTABLESIZE 91
short yytable[] = {                                      33,
   74,   70,   92,   89,   30,   86,   46,   55,   71,   46,
   72,   96,   50,   31,   49,   99,   50,  101,  102,  103,
   82,   53,   83,   53,  105,  106,  107,  104,  108,  109,
   58,   87,   31,   32,   53,   34,   95,   53,    3,    4,
    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
   36,   37,   40,   41,   43,   80,   81,   57,   90,   91,
   97,   98,   38,   39,   50,   78,   58,    1,   59,   60,
   61,   62,   63,   64,   65,   66,   67,   68,   55,   69,
   73,   49,   75,   84,  100,   85,   18,   94,   77,   32,
   93,
};
short yycheck[] = {                                       7,
   51,   44,   79,   76,  281,  278,  281,   15,  269,  284,
  271,   88,  285,  282,  283,   92,  285,   94,   95,   96,
  268,  268,  270,  270,  101,  102,  103,  100,  105,  106,
  277,   74,  282,  283,  281,  284,   87,  284,  256,  257,
  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,
  282,  283,  282,  283,  283,  272,  273,  281,  274,  275,
  274,  275,    9,   10,  285,  284,  281,    0,  281,  281,
  281,  281,  281,  281,  281,  281,  281,  281,   86,  281,
  276,  281,  283,  277,  277,   73,  281,   86,   52,  281,
   84,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 285
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOK_FROM","TOK_VIA","TOK_TO",
"TOK_ALT","TOK_COMMENT","TOK_FUEL_AMOUNT","TOK_FUEL_RATE","TOK_FUEL_USED",
"TOK_NAV","TOK_TAS","TOK_WIND","TOK_SMI","TOK_MPH","TOK_NMI","TOK_KTS",
"TOK_NORTH","TOK_SOUTH","TOK_EAST","TOK_WEST","TOK_ATSIGN","TOK_COLON",
"TOK_FSLASH","TOK_LPAREN","TOK_RPAREN","TOK_SEMICOLON","TOK_REAL","TOK_INTEGER",
"TOK_STRING","TOK_IDENT",
};
char *yyrule[] = {
"$accept : fplan",
"fplan : directives",
"directives :",
"directives : directives directive",
"directive : named_waypoint TOK_SEMICOLON",
"directive : inc_waypoint TOK_SEMICOLON",
"directive : int_waypoint TOK_SEMICOLON",
"directive : rel_waypoint TOK_SEMICOLON",
"directive : lat_waypoint TOK_SEMICOLON",
"directive : comment TOK_SEMICOLON",
"directive : flight_altitude TOK_SEMICOLON",
"directive : fuel_amount TOK_SEMICOLON",
"directive : fuel_rate TOK_SEMICOLON",
"directive : fuel_used TOK_SEMICOLON",
"directive : true_airspeed TOK_SEMICOLON",
"directive : winds_aloft TOK_SEMICOLON",
"directive : nav_fix TOK_SEMICOLON",
"directive : error TOK_SEMICOLON",
"named_waypoint : waypoint_kind identifier",
"inc_waypoint : waypoint_kind distance optional_string optional_string optional_string",
"int_waypoint : waypoint_kind identifier direction identifier direction optional_string optional_string optional_string",
"rel_waypoint : waypoint_kind identifier direction TOK_FSLASH distance optional_string optional_string optional_string",
"lat_waypoint : waypoint_kind latitude longitude mag_variation optional_string optional_string optional_string",
"comment : TOK_COMMENT string",
"fuel_amount : TOK_FUEL_AMOUNT pos_number",
"fuel_rate : TOK_FUEL_RATE pos_number",
"fuel_used : TOK_FUEL_USED non_neg_number",
"flight_altitude : TOK_ALT number",
"true_airspeed : TOK_TAS speed",
"winds_aloft : TOK_WIND direction TOK_ATSIGN speed",
"nav_fix : TOK_NAV pos_integer identifier",
"string : TOK_STRING",
"optional_string :",
"optional_string : TOK_STRING",
"identifier : TOK_IDENT",
"waypoint_kind : TOK_FROM",
"waypoint_kind : TOK_TO",
"waypoint_kind : TOK_VIA",
"latitude : deg_min_sec TOK_NORTH",
"latitude : deg_min_sec TOK_SOUTH",
"longitude : deg_min_sec TOK_EAST",
"longitude : deg_min_sec TOK_WEST",
"mag_variation : non_neg_number TOK_EAST",
"mag_variation : non_neg_number TOK_WEST",
"distance : number TOK_SMI",
"distance : number TOK_NMI",
"distance : number",
"speed : pos_number TOK_MPH",
"speed : pos_number TOK_KTS",
"speed : pos_number",
"direction : number",
"deg_min_sec : integer TOK_COLON integer TOK_COLON non_neg_number",
"number : TOK_REAL",
"number : TOK_INTEGER",
"non_neg_number : TOK_REAL",
"non_neg_number : TOK_INTEGER",
"pos_number : TOK_REAL",
"pos_number : TOK_INTEGER",
"integer : TOK_INTEGER",
"pos_integer : TOK_INTEGER",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 18:
#line 146 "fp_yacc.y"
{
			  add_named_waypoint (yyvsp[-1].kval, yyvsp[0].sval);
			}
break;
case 19:
#line 152 "fp_yacc.y"
{
			  add_inc_waypoint (yyvsp[-4].kval, yyvsp[-3].dval, yyvsp[-2].sval, yyvsp[-1].sval, yyvsp[0].sval);
			}
break;
case 20:
#line 161 "fp_yacc.y"
{
			  add_int_waypoint (yyvsp[-7].kval, yyvsp[-6].sval, yyvsp[-5].dval, yyvsp[-4].sval, yyvsp[-3].dval, yyvsp[-2].sval, yyvsp[-1].sval, yyvsp[0].sval);
			}
break;
case 21:
#line 168 "fp_yacc.y"
{
			  add_rel_waypoint (yyvsp[-7].kval, yyvsp[-6].sval, yyvsp[-5].dval, yyvsp[-3].dval, yyvsp[-2].sval, yyvsp[-1].sval, yyvsp[0].sval);
			}
break;
case 22:
#line 175 "fp_yacc.y"
{
			  add_lat_waypoint (yyvsp[-6].kval, yyvsp[-5].dval, yyvsp[-4].dval, yyvsp[-3].dval, yyvsp[-2].sval, yyvsp[-1].sval, yyvsp[0].sval);
			}
break;
case 23:
#line 180 "fp_yacc.y"
{ set_comment (yyvsp[0].sval); }
break;
case 24:
#line 183 "fp_yacc.y"
{ set_fuel_amt (yyvsp[0].dval); }
break;
case 25:
#line 186 "fp_yacc.y"
{ set_fuel_rate (yyvsp[0].dval); }
break;
case 26:
#line 189 "fp_yacc.y"
{ set_fuel_extra (yyvsp[0].dval); }
break;
case 27:
#line 192 "fp_yacc.y"
{ set_altitude (yyvsp[0].dval); }
break;
case 28:
#line 195 "fp_yacc.y"
{ set_tas (yyvsp[0].dval); }
break;
case 29:
#line 199 "fp_yacc.y"
{
					  set_wind (yyvsp[-2].dval, yyvsp[0].dval);
					}
break;
case 30:
#line 205 "fp_yacc.y"
{
					  set_xfix (yyvsp[-1].ival, yyvsp[0].sval);
					}
break;
case 31:
#line 210 "fp_yacc.y"
{ yyval.sval = strdup (yytext); }
break;
case 32:
#line 213 "fp_yacc.y"
{ yyval.sval = (char*) NULL; }
break;
case 33:
#line 214 "fp_yacc.y"
{ yyval.sval = strdup (yytext); }
break;
case 34:
#line 217 "fp_yacc.y"
{ yyval.sval = strdup (yytext); }
break;
case 35:
#line 220 "fp_yacc.y"
{ yyval.kval = WP_FROM; }
break;
case 36:
#line 221 "fp_yacc.y"
{ yyval.kval = WP_TO; }
break;
case 37:
#line 222 "fp_yacc.y"
{ yyval.kval = WP_VIA; }
break;
case 38:
#line 226 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 90.0))
			    yyerror ("expecting latitude in interval [0,90]");
			  yyval.dval = yyvsp[-1].dval;
			}
break;
case 39:
#line 232 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 90.0))
			    yyerror ("expecting latitude in interval [0,90]");
			  yyval.dval = -yyvsp[-1].dval;
			}
break;
case 40:
#line 240 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 180.0))
			    yyerror ("expecting longitude in interval [0,180]");
			  yyval.dval = -yyvsp[-1].dval;
			}
break;
case 41:
#line 246 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 180.0))
			    yyerror ("expecting longitude in interval [0,180]");
			  yyval.dval = yyvsp[-1].dval;
			}
break;
case 42:
#line 254 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 180.0))
			    yyerror ("expecting variation in interval [0,180]");
			  yyval.dval = -yyvsp[-1].dval;
			}
break;
case 43:
#line 260 "fp_yacc.y"
{
			  if ((yyvsp[-1].dval < 0.0) || (yyvsp[-1].dval > 180.0))
			    yyerror ("expecting variation in interval [0,180]");
			  yyval.dval = yyvsp[-1].dval;
			}
break;
case 44:
#line 267 "fp_yacc.y"
{ yyval.dval = yyvsp[-1].dval / MI_PER_NM; }
break;
case 45:
#line 268 "fp_yacc.y"
{ yyval.dval = yyvsp[-1].dval; }
break;
case 46:
#line 269 "fp_yacc.y"
{ yyval.dval = yyvsp[0].dval; }
break;
case 47:
#line 272 "fp_yacc.y"
{ yyval.dval = yyvsp[-1].dval / MI_PER_NM; }
break;
case 48:
#line 273 "fp_yacc.y"
{ yyval.dval = yyvsp[-1].dval; }
break;
case 49:
#line 274 "fp_yacc.y"
{ yyval.dval = yyvsp[0].dval; }
break;
case 50:
#line 278 "fp_yacc.y"
{
			  if ((yyvsp[0].dval < 0.0) || (yyvsp[0].dval > 360.0))
			    yyerror ("expecting direction in interval [0,360]");
			  yyval.dval = yyvsp[0].dval;
			}
break;
case 51:
#line 286 "fp_yacc.y"
{
			  if ((yyvsp[0].dval < 0.0) || (yyvsp[0].dval >= 60.0))
			    yyerror ("expecting seconds in interval [0,60)");
			  if ((yyvsp[-2].ival < 0.0) || (yyvsp[-2].ival >= 60.0))
			    yyerror ("expecting minutes in interval [0,60)");
			  yyval.dval = ((double)yyvsp[0].dval/3600.0) + ((double)yyvsp[-2].ival/60.0) + yyvsp[-4].ival;
			}
break;
case 52:
#line 295 "fp_yacc.y"
{ yyval.dval = yydval; }
break;
case 53:
#line 296 "fp_yacc.y"
{ yyval.dval = yydval; }
break;
case 54:
#line 300 "fp_yacc.y"
{
			  if (yydval >= 0.0)
			    yyval.dval = yydval;
			  else
			    yyerror ("expecting non-negative number");
			}
break;
case 55:
#line 307 "fp_yacc.y"
{
			  if (yydval >= 0.0)
			    yyval.dval = yydval;
			  else
			    yyerror ("expecting non-negative number");
			}
break;
case 56:
#line 316 "fp_yacc.y"
{
			  if (yydval > 0.0)
			    yyval.dval = yydval;
			  else
			    yyerror ("expecting positive number");
			}
break;
case 57:
#line 323 "fp_yacc.y"
{
			  if (yydval > 0.0)
			    yyval.dval = yydval;
			  else
			    yyerror ("expecting positive number");
			}
break;
case 58:
#line 331 "fp_yacc.y"
{ yyval.ival = yydval; }
break;
case 59:
#line 335 "fp_yacc.y"
{
			  if (yydval > 0.0)
			    yyval.ival = yydval;
			  else
			    yyerror ("expecting positive integer");
			}
break;
#line 684 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
