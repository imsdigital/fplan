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
typedef union {
   int		ival;
   double	dval;
   char		*sval;
   WP_KIND	kval;
} YYSTYPE;
extern YYSTYPE yylval;
