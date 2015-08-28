#
# $Id: Makefile,v 2.27.2.7 1999/05/02 06:33:25 jcp Exp $
#
# Copyright (C) 1998, John C. Peterson <mailto:jaypee@netcom.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License,
# version 2. A copy is included in this distribution in the file
# named "LICENSE".
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2, for more details.
#
# Note that the version 1.3 release of fplan (and older releases)
# are the work of Steve Tynor <mailto:tynor@atlanta.twr.com>. Those
# versions of fplan are in the public domain, (provided the source
# code headers are left intact). Here is the original header;
# 
#----------------------------------------------------------------------------
#	FPLAN - Flight Planner
#	Steve Tynor
#	tynor@@prism.gatech.edu
#
#	This program is in the public domain. Permission to copy,
# distribute, modify this program is hearby given as long as this header
# remains. If you redistribute this program after modifying it, please
# document your changes so that I do not take the blame (or credit) for
# those changes.  If you fix bugs or add features, please send me a
# patch so that I can keep the 'official' version up-to-date.
#
#	Bug reports are welcome and I'll make an attempt to fix those
# that are reported.
#
#	USE AT YOUR OWN RISK! I assume no responsibility for any
# errors in this program, its database or documentation. I will make an
# effort to fix bugs, but if you crash and burn because, for example,
# fuel estimates in this program were inaccurate, it's your own fault
# for trusting somebody else's code! Remember, as PIC, it's _your_
# responsibility to do complete preflight planning. Use this program as
# a flight planning aid, but verify its results before using them.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# EDIT THE FOLLOWING DEFINITIONS TO REFLECT YOUR ENVIRONMENT;
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Define which C compiler to use (with any flags to specify extended ANSI C)
CC = gcc
#CC = acc
#CC = cc -Ae
#CC = cc

# ---------------------------------------------------------------------------
# Define which graphics implementation to use. Currently, the options are
# -DGFX_GNOME for Gnome, or -DGFX_XVIEW for XView, or empty for no previewer
# under OS2, DOS.
#GFX_OPTIONS = -DGFX_GNOME
GFX_OPTIONS = -DGFX_XVIEW
#GFX_OPTIONS =

# ---------------------------------------------------------------------------
# Define which directories contain the graphics header files. The lines
# below are for the Gnome, XView and no previewer cases;
#GFX_INCLUDE = $(shell gnome-config --cflags glib gtk gnome gnomeui)
GFX_INCLUDE = -I/usr/openwin/include
#GFX_INCLUDE = 

# ---------------------------------------------------------------------------
# Define which graphics libraries are required when linking the executable.
# The lines below are for the Gnome, XView, and no previewer cases;
#GFX_LIBS = $(shell gnome-config --libs glib gtk gnome gnomeui)
GFX_LIBS = -L/usr/openwin/lib -lxview -lolgx -L/usr/X11R6/lib -lX11
#GFX_LIBS =

# ---------------------------------------------------------------------------
# Define any additional flags required by the C compiler during the
# compilation phase (such as flags for debugging or optimization,
# include -DOS2 when building under OS2/EMX, and -DDOS under MS-DOS)
CCOPTS = -O2 -m486 -fomit-frame-pointer
#CCOPTS = -g -Wall
#CCOPTS = -O -DOS2
#CCOPTS = -O -DMSDOS

# ---------------------------------------------------------------------------
# Define any additional flags required by the loader for the link phase,
# (such as flags for debugging, or to strip symbols from the executable)
LDOPTS = -s
#LDOPTS = -g

# ---------------------------------------------------------------------------
# Define which lexical analyzer generator, (for flex, define as 'flex -l')
# You can ignore these settings as long as you refrain from executing a
# 'make veryclean' (this deletes the default scanner in fp_lex.c)
LEX = flex -l
#LEX = lex

# ---------------------------------------------------------------------------
# Define which parser generator to use, (works with bison, byacc, or yacc)
# You can ignore these settings as long as you refrain from executing a
# 'make veryclean' (this deletes the default parser in fp_yacc.c)
#YACC = bison -d -y
YACC = byacc -d
#YACC = yacc -d

# ---------------------------------------------------------------------------
# Define where 'make install' should put the executable and man pages
BINDIR = /usr/bin

# ---------------------------------------------------------------------------
# Define where 'make install-dbs' should put the databases (be *sure*
# this matches with the value assigned to FPLAN_PUBLIC in config.h)
DBSDIR = /usr/share/fplan

# ---------------------------------------------------------------------------
# Define where 'make install-man' should put the unformatted man pages
# and the formatted and plain text documentation files respectively
MANDIR = /usr/man

# ---------------------------------------------------------------------------
# Define the command to be used by 'make doc' to create formatted man
# pages, and the suffix (without the dot) for the formatted output files.
# For ascii use 'nroff', for postscript use either 'groff' or 'troff'
# You can ignore these settings as long as you refrain from executing
# a 'make veryclean' which deletes the formatted documentation files.
MAN_FORMAT = groff -man -Tps
MAN = ps
#MAN_FORMAT = nroff -man
#MAN = txt

# ---------------------------------------------------------------------------
# Define the command to be used by 'make install' to copy the files (use
# 'install' if you have it, otherwise you can use 'cp' or 'copy' instead)
INSTALL  = install
#INSTALL = cp -a
#INSTALL = copy

# ---------------------------------------------------------------------------
# Define the suffix of executable, object and html files on your system
# (for executables, include the "." separator character if one is present)
EXE =
OBJ = o
HTML = html
#EXE = .exe
#OBJ = obj
#HTML = htm

# ---------------------------------------------------------------------------
# NO NEED TO EDIT BELOW HERE. (I THINK...)
# ---------------------------------------------------------------------------

SHELL = /bin/sh

DIST_NAME = fplan-1.4.2

EXES = fplan$(EXE) paddb$(EXE)

CFLAGS = $(CCOPTS) $(GFX_INCLUDE) $(GFX_OPTIONS)

C_SRC = add.c compute.c db.c main.c misc.c output.c \
	output_text.c paddb.c preview.c reverse.c

OTHER_SRC = fp_lex.l fp_yacc.y common.h config.h \
	output.h patchlvl.h

FPLAN_DEF =

FPLAN_OBJS = \
	add.$(OBJ) compute.$(OBJ) db.$(OBJ) fp_lex.$(OBJ) \
	fp_yacc.$(OBJ) main.$(OBJ) misc.$(OBJ) output.$(OBJ) \
	output_text.$(OBJ) preview.$(OBJ) reverse.$(OBJ) \
	$(FPLAN_DEF)

PADDB_DEF =

PADDB_OBJS = paddb.$(OBJ) $(PADDB_DEF)

# items that poor DOS users will need and can't rebuild without serious grief

DOS_FILES = fp_lex.c fp_yacc.c fp_tok.h

DBS_FILES = airports.nav vors.nav

EXAMPLES = EXAMPLE1.fpl EXAMPLE2.fpl EXAMPLE3.fpl

FMT_TXT = BUGS CHANGE.LOG FAQ

TXT_DOC = ANNOUNCE AUTHORS INSTALL LICENSE README TODO

RAW_MAN = fplan.1 fplan.5 paddb.1 guide.sgml

FMT_MAN = fplan_1.$(MAN) fplan_5.$(MAN) paddb_1.$(MAN)

FMT_DOC = $(FMT_TXT) $(FMT_MAN) guide.$(HTML) guide.ps guide.rtf guide.txt

FMT_DOCS = $(FMT_DOC) guide-*.$(HTML) guide-*.rtf

ALL_DOC = $(EXAMPLES) $(TXT_DOC) $(RAW_MAN) $(FMT_DOCS)

MAKEFILES = Makefile dosbuild.bat fplan.spec fplan-db.spec

DIST_FILES = $(C_SRC) $(OTHER_SRC) $(DOS_FILES) $(MAKEFILES) $(ALL_DOC)

all: $(EXES)

doc: $(FMT_DOC)

fplan$(EXE): $(FPLAN_OBJS)
	$(CC) -o fplan$(EXE) $(LDOPTS) $(FPLAN_OBJS) $(GFX_LIBS) -lm

paddb$(EXE): $(PADDB_OBJS)
	$(CC) -o paddb$(EXE) $(LDOPTS) $(PADDB_OBJS)

fp_lex.c: fp_lex.l fp_tok.h
	$(LEX) fp_lex.l
	mv lex.yy.c fp_lex.c

fp_yacc.c fp_tok.h: fp_yacc.y common.h
	$(YACC) fp_yacc.y
	mv y.tab.h fp_tok.h
	mv y.tab.c fp_yacc.c

fplan_1.$(MAN): fplan.1
	cat fplan.1 | $(MAN_FORMAT) > fplan_1.$(MAN)

fplan_5.$(MAN): fplan.5
	cat fplan.5 | $(MAN_FORMAT) > fplan_5.$(MAN)

paddb_1.$(MAN): paddb.1
	cat paddb.1 | $(MAN_FORMAT) > paddb_1.$(MAN)

guide.htm: guide.sgml
	sgml2html -fat guide.sgml

guide.html: guide.sgml
	sgml2html guide.sgml

guide.ps: guide.sgml
	sgml2latex -2e guide.sgml
	export TEXINPUTS=${TEXINPUTS}:/usr/lib/sgml-tools \
	; latex guide.tex \
	; latex guide.tex # run again to get the cross references right
	dvips -t letter -o guide.ps guide.dvi

guide.rtf: guide.sgml
	sgml2rtf guide.sgml

guide.txt: guide.sgml
	sgml2txt -f guide.sgml

BUGS: BUGS.sgml
	sgml2txt -f BUGS.sgml
	mv BUGS.txt BUGS

CHANGE.LOG: CHANGE.sgml
	sgml2txt -f CHANGE.sgml
	mv CHANGE.txt CHANGE.LOG

FAQ: FAQ.sgml
	sgml2txt -f FAQ.sgml
	mv FAQ.txt FAQ

airports.nav: airports.txt paddb$(EXE)
	sort -t : airports.txt -o airports.txt
	paddb airports.txt airports.nav

vors.nav: vors.txt paddb$(EXE)
	sort -t : vors.txt -o vors.txt
	paddb vors.txt vors.nav

install: $(EXES)
	$(INSTALL) $(EXES) $(BINDIR)

install-man: $(RAW_MAN)
	$(INSTALL) fplan.1 $(MANDIR)/man1/
	$(INSTALL) fplan.5 $(MANDIR)/man5/
	$(INSTALL) paddb.1 $(MANDIR)/man1/
	chmod 0644 $(MANDIR)/man1/fplan.1
	chmod 0644 $(MANDIR)/man5/fplan.5
	chmod 0644 $(MANDIR)/man1/paddb.1

install-dbs: $(DBS_FILES)
	chmod 0644 airports.nav vors.nav
	$(INSTALL) airports.nav vors.nav $(DBSDIR)/

dist: $(DIST_FILES)
	-rm -rf /tmp/$(DIST_NAME)
	mkdir /tmp/$(DIST_NAME)
	chmod 0755 /tmp/$(DIST_NAME)
	tar cvf - `ls $(DIST_FILES)` | \
	  ( cd /tmp/$(DIST_NAME) ; umask 022 ; tar xvpf - )
	chmod -R a+r /tmp/$(DIST_NAME)
	chmod -R u+w /tmp/$(DIST_NAME)
	chmod -R go-w /tmp/$(DIST_NAME)
	( \
	  cd /tmp; tar cvf $(DIST_NAME).src.tar $(DIST_NAME); \
	  cd /tmp/$(DIST_NAME); flip -bmv `ls $(DIST_FILES)`; \
	  cd /tmp; rm -rf /tmp/fplan; mv /tmp/$(DIST_NAME) /tmp/fplan; \
	  echo "$(DIST_NAME)" | zip -9rz $(DIST_NAME).zip fplan; \
	)
	mv /tmp/$(DIST_NAME).src.tar ./
	mv /tmp/$(DIST_NAME).zip ./
	bzip2 -kv9 $(DIST_NAME).src.tar
	compress -c $(DIST_NAME).src.tar > $(DIST_NAME).src.tar.Z
	gzip -cNv9 $(DIST_NAME).src.tar > $(DIST_NAME).src.tar.gz
	touch -r $(DIST_NAME).src.tar \
	  $(DIST_NAME).src.tar.bz2 \
	  $(DIST_NAME).src.tar.gz \
	  $(DIST_NAME).src.tar.Z \
	  $(DIST_NAME).zip
	-rm -rf $(DIST_NAME).src.tar /tmp/fplan

depend:
	makedepend -Y $(SRCS)

clean: 
	-rm -f $(EXES) *.$(OBJ) guide*.aux guide*.dvi guide*.log guide*.tex

realclean: clean
	-rm -f $(DOS_FILES) *~

veryclean: clean
	-rm -f $(DOS_FILES) $(FMT_DOCS) *~

# Define our own compilation recipe (for those brain damaged make utilities)

.c.$(OBJ):
	$(CC) -c $(CFLAGS) $*.c

# DO NOT DELETE

add.$(OBJ)	: add.c common.h 
compute.$(OBJ)	: compute.c common.h
db.$(OBJ)	: db.c common.h config.h output.h
fp_lex.$(OBJ)	: fp_lex.c common.h fp_tok.h
fp_yacc.$(OBJ)	: fp_yacc.c common.h
main.$(OBJ)	: main.c common.h output.h patchlvl.h
misc.$(OBJ)	: misc.c common.h
output.$(OBJ)	: output.c common.h output.h
output_text.$(OBJ)    : output_text.c common.h output.h
paddb.$(OBJ)	: paddb.c patchlvl.h
preview.$(OBJ)	: preview.c common.h
reverse.$(OBJ)	: reverse.c common.h

