/*
 * $Id: config.h,v 1.2 1998/11/28 06:15:03 jcp Exp $
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

/*
 * Names of environment variables for where the databases are located
 */

#define FPLAN_COMMON_DBDIR	"FPLAN_COMMON_DBDIR"
#define FPLAN_USER_DBDIR	"FPLAN_USER_DBDIR"
#define HOME 			"HOME"		/* used to expand ~ */

/*
 * Default directories for the common, system wide databases
 */

#ifndef DEFAULT_COMMON_DBDIR
#if defined(OS2) || defined(MSDOS)
#define DEFAULT_COMMON_DBDIR	"\\fplan\\commondb"
#else
#define DEFAULT_COMMON_DBDIR	"/usr/share/fplan"
#endif
#endif

/*
 * Default directories for the user's personal databases
 */

#ifndef DEFAULT_USER_DBDIR
#if defined(OS2) || defined(MSDOS)
#define DEFAULT_USER_DBDIR	"\\fplan\\userdb"
#else
#define DEFAULT_USER_DBDIR	"~/.fplan"
#endif
#endif

/*
 * Default names for the database files
 */

#if defined(OS2) || defined(MSDOS)
#define AIRPORTS_NAME "\\airports.nav"
#define VORS_NAME     "\\vors.nav"
#else
#define AIRPORTS_NAME "/airports.nav"
#define VORS_NAME     "/vors.nav"
#endif

