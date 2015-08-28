#
# $Id$
#
# Copyright (C) 1998, John C. Peterson <mailto:jaypee@netcom.com>
# GUI interface Copyright © 2001, Edward A. Falk <mailto:falk@art.net>
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


# Note: version in Makefile, *.spec, xfplan.lsm

VERSION = 0.1
DIST_NAME = xfplan-${VERSION}
RELEASE=1

DBS_VERSION = 2005.04.03
DBS_NAME = fplan-db-${DBS_VERSION}


# ---------------------------------------------------------------------------
# Define which C compiler to use (with any flags to specify extended ANSI C)
#CC = gcc
#CC = acc
#CC = cc -Ae
#CC = cc


# ---------------------------------------------------------------------------
# Eventual final destination for program:
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
datadir = $(prefix)/share/fplan
mandir = $(prefix)/man
man1dir = $(mandir)/man1
man5dir = $(mandir)/man5
man1ext = .1
man5ext = .5

# ---------------------------------------------------------------------------
# Temporary final destination for program, for building packages, etc.
PROTODIR =



# ---------------------------------------------------------------------------
# Define which graphics implementation to use. Currently, the options are
# -DGFX_GNOME for Gnome, or empty for no gui interface
# under OS2, DOS.
GFX_OPTIONS = -DGFX_GNOME
#GFX_OPTIONS = -DGFX_XVIEW
#GFX_OPTIONS =

# ---------------------------------------------------------------------------
# Define which directories contain the graphics header files. The lines
# below are for the Gnome and no gui cases;
GFX_INCLUDE = $(shell gnome-config --cflags glib gtk gnome gnomeui)
#GFX_INCLUDE = -I/usr/openwin/include
#GFX_INCLUDE = 

# ---------------------------------------------------------------------------
# Define which graphics libraries are required when linking the executable.
# The lines below are for the Gnome, and no previewer cases;
GFX_LIBS = $(shell gnome-config --libs glib gtk gnome gnomeui)
#GFX_LIBS = -L/usr/openwin/lib -lxview -lolgx -L/usr/X11R6/lib -lX11
#GFX_LIBS =

# ---------------------------------------------------------------------------
# Define which source files are required when compiling
# The lines below are for the Gnome, and no previewer cases;
GFX_SRC = $(GNOME_SRC)
#GFX_SRC = $(XVIEW_SRC)
#GFX_SRC =

# ---------------------------------------------------------------------------
# Define any additional flags required by the C compiler during the
# compilation phase (such as flags for debugging or optimization,
# include -DOS2 when building under OS2/EMX, and -DDOS under MS-DOS)
CCOPTS = -O
#CCOPTS = -g
#
#CCOPTS = -O2 -m486 -fomit-frame-pointer
#CCOPTS = -g -Wall
#CCOPTS = -g -DDEBUG -Wall
#CCOPTS = -g -DDEBUG -Wall -pedantic
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
INSTALL_DAT = install -m 0444
#INSTALL_DAT = $(INSTALL)

# ---------------------------------------------------------------------------
# Define the suffix of executable, object and html files on your system
# (for executables, include the "." separator character if one is present)
EXE =
HTML = html
#EXE = .exe
#HTML = htm

# ---------------------------------------------------------------------------
#OS = BSD
#OS = SVr4
#OS = SOLARIS
OS = LINUX

# ---------------------------------------------------------------------------
# NO NEED TO EDIT BELOW HERE. (I THINK...)
# ---------------------------------------------------------------------------




EXES = xfplan$(EXE) paddb$(EXE)

INC = -I. -IBitmaps $(GFX_INCLUDE)

CFLAGS = $(CCOPTS) $(INC) $(GFX_OPTIONS) -DOS=$(OS)

LDFLAGS =

SRC =	xplan.c windows.c defaults.c callbacks.c canvas.c map_wp.c \
	map.c gps.c weight.c worksheet.c astro.c performance.c popups.c \
	airports.c plan.c waypoints.c airways.c compute.c \
	airway_map.c db.c output.c misc.c preferences.c \
	dialog_helper.c FileSelect.c

#SRC =	xplan.c airways.c compute.c db.c misc.c output.c \
#	output_text.c reverse.c waypoints.c \
#	xplan.c callbacks.c canvas.c support.c windows.c map_wp.c \
#	airway_map.c preferences.c gps.c map.c \
#	astro.c weight.c performance.c airports.c worksheet.c plan.c \
#	dialog_helper.c popups.c defaults.c


OTHER_SRC = Bitmaps/airport2.bm Bitmaps/airport3.bm Bitmaps/airport.bm \
	Bitmaps/grey50.bm Bitmaps/ndb2.bm Bitmaps/ndb3.bm Bitmaps/ndb.bm \
	Bitmaps/vor.bm Bitmaps/vortac.bm Bitmaps/waypoint.bm \
	Bitmaps/pan.xpm Bitmaps/zoomin.xpm Bitmaps/zoomout.xpm

HDRS =	airway_map.h airways.h callbacks.h canvas.h compute.h config.h \
	db.h defaults.h dialog_helper.h FileSelect.h map.h map_wp.h \
	misc.h output.h patchlvl.h plan.h popups.h utils.h waypoints.h \
	windows.h worksheet.h xplan.h xplan_types.h

XFPLAN_OBJS = $(SRC:.c=.o)

PADDB_SRCS = paddb.c

PADDB_OBJS = paddb.o

DBS_FILES = airports.nav vors.nav airways.nav us-map

#EXAMPLES = EXAMPLE1.fpl EXAMPLE2.fpl EXAMPLE3.fpl EXAMPLE4.fpl

FMT_TXT = BUGS CHANGE.LOG FAQ

TXT_DOC = ANNOUNCE AUTHORS INSTALL LICENSE README README.developers TODO NOTES

RAW_MAN = xfplan.1 fplan.5 paddb.1 guide.sgml

FMT_MAN = xfplan_1.$(MAN) fplan_5.$(MAN) paddb_1.$(MAN)


#FMT_DOC = $(FMT_TXT) $(FMT_MAN) guide.$(HTML) guide.ps guide.rtf guide.txt
FMT_DOC = $(FMT_TXT) $(FMT_MAN) guide.$(HTML) guide.rtf guide.txt

#FMT_DOCS = $(FMT_DOC) guide-*.$(HTML) guide-*.rtf
FMT_DOCS = $(FMT_DOC)

ALL_DOC = $(EXAMPLES) $(TXT_DOC) $(RAW_MAN) $(FMT_DOCS)

DIST_FILES = $(SRC) $(HDRS) $(OTHER_SRC) $(DOS_FILES) \
	Makefile dosbuild.bat xfplan.spec fplan-db.spec \
	$(PADDB_SRCS) $(ALL_DOC) xfplan.lsm

DIST_DIR = /tmp/$(DIST_NAME)
DBS_DIR = /tmp/$(DBS_NAME)


xfplan$(EXE): $(XFPLAN_OBJS)
	$(CC) -o $@ $(LDOPTS) $(XFPLAN_OBJS) $(GFX_LIBS) -lm

paddb$(EXE): $(PADDB_OBJS)
	$(CC) -o paddb$(EXE) $(LDOPTS) $(PADDB_OBJS)

fp_lex.c: fp_lex.l fp_tok.h
	$(LEX) fp_lex.l
	mv lex.yy.c fp_lex.c

fp_yacc.c fp_tok.h: fp_yacc.y common.h
	$(YACC) fp_yacc.y
	mv y.tab.h fp_tok.h
	mv y.tab.c fp_yacc.c

xfplan_1.$(MAN): xfplan.1
	cat xfplan.1 | $(MAN_FORMAT) > xfplan_1.$(MAN)

fplan_5.$(MAN): fplan.5
	cat fplan.5 | $(MAN_FORMAT) > fplan_5.$(MAN)

paddb_1.$(MAN): paddb.1
	cat paddb.1 | $(MAN_FORMAT) > paddb_1.$(MAN)

guide.htm: guide.sgml
	sgml2html -fat guide.sgml

guide.html: guide.sgml
	sgml2html guide.sgml

# redhat 6.2 version of sgml2latex doesn't accept -2e argument.  Comment
# this code out until that's dealt with.
#
#guide.ps: guide.sgml
#	sgml2latex -2e guide.sgml
#	export TEXINPUTS=${TEXINPUTS}:/usr/lib/sgml-tools \
#	; latex guide.tex \
#	; latex guide.tex # run again to get the cross references right
#	dvips -t letter -o guide.ps guide.dvi

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

DESTDIRS = $(PROTODIR)$(bindir) $(PROTODIR)$(datadir) \
	$(PROTODIR)$(man1dir) $(PROTODIR)$(man5dir)


install: install-exe install-man install-dbs

install-exe: $(EXES) $(PROTODIR)$(bindir)
	$(INSTALL) $(EXES) $(PROTODIR)$(bindir)

install-man: $(PROTODIR)$(man1dir) $(PROTODIR)$(man5dir)
	$(INSTALL) xfplan.1 $(PROTODIR)$(man1dir)
	$(INSTALL) paddb.1 $(PROTODIR)$(man1dir)
	$(INSTALL) fplan.5 $(PROTODIR)$(man5dir)
	chmod 0644 $(PROTODIR)$(man1dir)/xfplan.1
	chmod 0644 $(PROTODIR)$(man1dir)/paddb.1
	chmod 0644 $(PROTODIR)$(man5dir)/fplan.5

install-dbs: $(DBS_FILES) $(PROTODIR)$(datadir)
	chmod 0444 $(DBS_FILES)
	$(INSTALL_DAT) $(DBS_FILES) $(PROTODIR)$(datadir)

tags:	$(HDRS) $(SRC)
	ctags *.[ch] /usr/include/libgnome/*.h /usr/include/libgnomeui/*.h \
	/usr/include/gtk*/*/*.h /usr/include/gdk*/*.h /usr/include/glib*.h \
	/usr/include/*gnome*.h

clean:
	rm -f *.o guide*.aux guide*.dvi guide*.log guide*.tex *.s *.i

clobber: clean
	rm -f tags *.ps
	rm -f xfplan xfplan-${VERSION}.src.tar.bz2 xfplan-${VERSION}.src.tar.gz
	rm -f xfplan-${VERSION}.src.tar.Z xfplan-${VERSION}.zip
	rm -f paddb

realclean: clean
	rm -f $(EXES) $(DOS_FILES) *~
	rm -f xfplan-${VERSION}.src.tar.gz xfplan-${VERSION}.src.tar.Z
	rm -f xfplan-${VERSION}.src.tar.bz2 xfplan-${VERSION}.zip

veryclean: realclean
	rm -f $(FMT_DOCS) tags

.SUFFIXES: .i .s

.c.i:
	$(CC) -E $(CFLAGS) $*.c > $@

.c.s:
	$(CC) -S $(CFLAGS) $*.c

dist:	veryclean $(DIST_FILES)
	-rm -rf $(DIST_DIR)
	mkdir $(DIST_DIR)
	chmod 0755 $(DIST_DIR)
	tar cvfh - $(DIST_FILES) | ( cd $(DIST_DIR) ; umask 0222 ; tar xvpf - )
	chmod -R a+r $(DIST_DIR)
	chmod -R u+w $(DIST_DIR)
	chmod -R go-w $(DIST_DIR)
	( \
	  cd $(DIST_DIR)/..; tar cvf $(DIST_NAME).src.tar $(DIST_NAME); \
	  cd $(DIST_DIR); flip -bmv `ls $(DIST_FILES)`; \
	  cd $(DIST_DIR)/..; rm -rf ./xfplan; mv ./$(DIST_NAME) ./xfplan; \
	  echo "$(DIST_NAME)" | zip -9rz $(DIST_NAME).zip xfplan; \
	  mv ./xfplan ./$(DIST_NAME); \
	)
	mv $(DIST_DIR).src.tar ./
	mv $(DIST_DIR).zip ./
	bzip2 -kv9 $(DIST_NAME).src.tar
	compress -c $(DIST_NAME).src.tar > $(DIST_NAME).src.tar.Z
	gzip -cNv9 $(DIST_NAME).src.tar > $(DIST_NAME).src.tar.gz
	touch -r $(DIST_NAME).src.tar \
	  $(DIST_NAME).src.tar.bz2 \
	  $(DIST_NAME).src.tar.gz \
	  $(DIST_NAME).src.tar.Z \
	  $(DIST_NAME).zip
	-rm -rf $(DIST_NAME).src.tar $(DIST_DIR)


dbdist:	$(DBS_FILES)
	-rm -rf $(DBS_DIR)
	mkdir $(DBS_DIR)
	chmod 0755 $(DBS_DIR)
	cp -r Makefile $(DBS_FILES) $(DBS_DIR)
	chmod -R a+r $(DBS_DIR)
	chmod -R u+w $(DBS_DIR)
	chmod -R go-w $(DBS_DIR)
	cd $(DBS_DIR)/.. ; tar cvf $(DBS_NAME).tar $(DBS_NAME)
	cd $(DBS_DIR)/.. ; zip -9r $(DBS_NAME).zip $(DBS_NAME)
	mv $(DBS_DIR).tar .
	mv $(DBS_DIR).zip .
	gzip -v9 $(DBS_NAME).tar
	-rm -rf $(DBS_NAME).tar $(DBS_DIR)

# Build the .rpm

RPM_SOURCE_DIR = /tmp/rpmsource-$(DIST_NAME)
RPM_BUILD_DIR = /tmp/rpmbuild-$(DIST_NAME)

rpm:	$(DIST_NAME).src.tar.gz
	rm -rf $(RPM_SOURCE_DIR)
	rm -rf $(RPM_BUILD_DIR)
	mkdir -p $(RPM_SOURCE_DIR)
	mkdir -p $(RPM_BUILD_DIR)
	cp $(DIST_NAME).src.tar.gz $(RPM_SOURCE_DIR)

	rpmbuild -bb xfplan.spec \
	  --define "_sourcedir ${RPM_SOURCE_DIR}" \
	  --define "_builddir ${RPM_BUILD_DIR}" \
	  --define "_rpmdir ${RPM_SOURCE_DIR}"

	rm ${RPM_SOURCE_DIR}/${DIST_NAME}.src.tar.gz
	test -d RPMS || mkdir RPMS
	cp -af  ${RPM_SOURCE_DIR}/* RPMS
	rm -rf ${RPM_SOURCE_DIR} ${RPM_BUILD_DIR}



dbrpm:	$(DBS_NAME).tar.gz
	rm -rf $(RPM_SOURCE_DIR)
	rm -rf $(RPM_BUILD_DIR)
	mkdir -p $(RPM_SOURCE_DIR)
	mkdir -p $(RPM_BUILD_DIR)
	cp $(DBS_NAME).tar.gz $(RPM_SOURCE_DIR)

	rpmbuild -bb fplan-db.spec \
	  --define "_sourcedir ${RPM_SOURCE_DIR}" \
	  --define "_builddir ${RPM_BUILD_DIR}" \
	  --define "_rpmdir ${RPM_SOURCE_DIR}"

	rm ${RPM_SOURCE_DIR}/${DBS_NAME}.tar.gz
	test -d RPMS || mkdir RPMS
	cp -af  ${RPM_SOURCE_DIR}/* RPMS
	rm -rf ${RPM_SOURCE_DIR} ${RPM_BUILD_DIR}



#	build directories

$(DESTDIRS) $(DIST_DIR):
	mkdir -p $@

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(HDRS)

# DO NOT DELETE

