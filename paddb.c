/*
 * $Id: paddb.c,v 1.7 1999/05/02 05:09:10 jcp Exp $
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

static char rcsid[] = "$Id: paddb.c,v 1.7 1999/05/02 05:09:10 jcp Exp $";

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "patchlvl.h"

#if defined(OS2) || defined(MSDOS)
#define CREATE_MODE "wb"
#define LINE_TERMINATOR_LEN 2	/* CR-LF */
#else
#define CREATE_MODE "w"
#define LINE_TERMINATOR_LEN 1	/* LF */
#endif

#define BUFSIZE (256+LINE_TERMINATOR_LEN+1)

/*----------------------------------------------------------------------------*/
static void
usage (char *progname, int status)
{
   fprintf (stderr, "paddb version: %s\n", VERSION);
   fprintf (stderr, "usage: %s [options] input-file output-file\n", progname);
   fprintf (stderr, "\t-h - print help summary\n");
   fprintf (stderr, "\t-v - verbose operation\n");
   exit (status);
}

/*----------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
   FILE *fpi, *fpo;
   char *filei, *fileo;
   char buffer[BUFSIZE];
   char c, errflag, verbose;
   int i, reclen, maxlen, outlen;
   int numrec, numtrunc;

   /*
    * process any command line options
    */

   opterr = 0;
   errflag = FALSE;
   verbose = FALSE;

   while ((c = getopt (argc, argv, "hv")) != -1) {
      switch (c) {
      case 'h':
	 usage (argv[0], EXIT_GOOD);
	 break;
      case 'v':
	 verbose = TRUE;
	 break;
      default:
	 fprintf (stderr, "ERROR: unknown option: %s\n", argv[optind-1]);
	 errflag++;
	 break;
     }
     if (errflag)
	usage (argv[0], EXIT_BAD);
   }

   /*
    * process file arguments
    */

   filei = NULL;
 
   if (optind < argc) {
      filei = argv[optind];
   } else {
      fprintf (stderr, "ERROR: missing input file argument\n");
      usage (argv[0], EXIT_BAD);
   }

   fileo = NULL;
 
   if (++optind < argc) {
      fileo = argv[optind];
   } else {
      fprintf (stderr, "ERROR: missing output file argument\n");
      usage (argv[0], EXIT_BAD);
   }

   if (++optind < argc) {
      fprintf (stderr, "ERROR: too many file arguments\n");
      usage (argv[0], EXIT_BAD);
   }

   /*
    * open the specified files
    */

   if (! (fpi = fopen (filei, "r"))) {
      fprintf (stderr, "ERROR: opening input file: %s\n", filei);
      usage (argv[0], EXIT_BAD);
   }
 
   if (! (fpo = fopen (fileo, CREATE_MODE))) {
      fprintf (stderr, "ERROR: opening output file: %s\n", fileo);
      usage (argv[0], EXIT_BAD);
   }

   /*
    * first pass: determine the maximum record length
    */

   maxlen = numrec = 0;

   while (fgets (buffer, BUFSIZE, fpi)) {
      i = reclen = strlen (buffer);
      /* verify we read the newline char */
      while (buffer[i-1] != '\n') {
	 fgets (buffer, BUFSIZE, fpi);
	 i = strlen (buffer);
	 reclen += i;
      }
      maxlen = MAX (reclen, maxlen);
      numrec++;
   }

   maxlen -= LINE_TERMINATOR_LEN;

   fclose (fpi);

   /*
    * second pass: copy each record to the output and pad with NULL chars
    */

   numtrunc = 0;

   outlen = MIN (BUFSIZE-LINE_TERMINATOR_LEN-1, maxlen);

   if (! (fpi = fopen (filei, "r"))) {
      fprintf (stderr, "ERROR: opening input file: %s\n", argv[1]);
      usage (argv[0], EXIT_BAD);
   }

   while (fgets (buffer, BUFSIZE, fpi)) {
      reclen = strlen (buffer);
      c = buffer[reclen-1];
      reclen -= LINE_TERMINATOR_LEN;
      buffer[reclen] = '\0';
      fprintf (fpo, "%s", buffer);
      /* pad short records with NULL chars */
      for (i = reclen; i < outlen; i++)
	 putc ('\0', fpo);
      putc ('\n', fpo);
      /* verify we read the newline char */
      if (c != '\n') {
	 numtrunc++;
	 while (c != '\n') {
	    fgets (buffer, BUFSIZE, fpi);
	    reclen = strlen (buffer);
	    c = buffer[reclen-1];
	 }
      }
   }

   fclose (fpi);
   fclose (fpo);

   if (verbose) {
      fprintf (stderr, "number of records ............... %d\n", numrec);
      fprintf (stderr, "output record length ............ %d\n", outlen);
      fprintf (stderr, "maximum input record length ..... %d\n", maxlen);
      fprintf (stderr, "number of truncated records ..... %d\n", numtrunc);
   }

   exit(EXIT_GOOD);
}
