: $Id: dosbuild.bat,v 1.5 1998/02/17 05:11:50 jcp Exp $
:----------------------------------------------------------------------------
:	FPLAN - Flight Planner
:	Steve Tynor
:	tynor@prism.gatech.edu
:
:	This program is in the public domain. Permission to copy,
: distribute, modify this program is hearby given as long as this header
: remains. If you redistribute this program after modifying it, please
: document your changes so that I do not take the blame (or credit) for
: those changes.  If you fix bugs or add features, please send me a
: patch so that I can keep the 'official' version up-to-date.
:
:	Bug reports are welcome and I'll make an attempt to fix those
: that are reported.
:
: 	USE AT YOUR OWN RISK! I assume no responsibility for any
: errors in this program, its database or documentation. I will make an
: effort to fix bugs, but if you crash and burn because, for example,
: fuel estimates in this program were inaccurate, it's your own fault
: for trusting somebody else's code! Remember, as PIC, it's _your_
: responsibility to do complete preflight planning. Use this program as
: a flight planning aid, but verify its results before using them.
: ---------------------------------------------------------------------------

: ---------------------------------------------------------------------------
: This is a MessyDOS batch file to compile FPLAN - 
:
: Uncomment the approriate lines to compile with whatever compiler you have.
: ---------------------------------------------------------------------------

: ---------------------------------------------------------------------------
: Zortech:
ztc -c -ml -w -p fp_lex.c
ztc -c -ml -w -p fp_yacc.c
ztc -c -ml -w -p add.c
ztc -c -ml -w -p compute.c
ztc -c -ml -w -p db.c
ztc -c -ml -w -p main.c
ztc -c -ml -w -p misc.c
ztc -c -ml -w -p output.c
ztc -c -ml -w -p strings.c
ztc -c -ml -w -p reverse.c
ztc -ml -L -ofplan.exe fp_lex.obj fp_yacc.obj add.obj compute.obj db.obj main.obj misc.obj output.obj strings.obj reverse.obj

: ---------------------------------------------------------------------------
: Microsoft:
:cl /c /AL fp_lex.c
:cl /c /AL fp_yacc.c
:cl /c /AL add.c
:cl /c /AL compute.c
:cl /c /AL db.c
:cl /c /AL main.c
:cl /c /AL misc.c
:cl /c /AL output.c
:cl /c /AL strings.c
:cl /c /AL reverse.c
:cl /Fefplan.exe fp_lex fp_yacc add compute db main misc output strings reverse /link

