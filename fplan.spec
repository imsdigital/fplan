
Summary: cross country flight planning tool for pilots
Name: fplan
Version: 1.4.2
Release: 1
Group: Applications/Aviation
URL: http://metalab.unc.edu/fplan/
Copyright: GNU General Public License, version 2
Packager: John C. Peterson <jaypee@netcom.com>
Source: http://metalab.unc.edu/fplan/fplan-%{PACKAGE_VERSION}.src.tar.gz
Requires: fplan-db >= 1997.08.15
Buildroot: /var/tmp/fplan-root

%description
The fplan application is designed to assist general aviation pilots in
creating flight plans for cross country flights in powered aircraft.
It reads a planfile containing a description of the flight; departure
and destination airports, navigation aids, intermediate waypoints, winds
aloft, fuel consumption rates, and produces a flight plan including; wind
corrected magnetic headings, distance, estimated time and fuel consumption
for each leg; latitude, longitude and VOR fixes for each checkpoint, etc.

%prep
%setup
make clean

%build
# use this to make an executable with dynamic linked XView libs
# make CC="gcc" CCOPTS="-O2 -m486 -fomit-frame-pointer" DBS_LIBDIR=/usr/share/fplan GFX_OPTIONS="-DGFX_XVIEW" GFX_INCLUDE="-I/usr/openwin/include" GFX_LIBS="-L/usr/openwin/lib -lxview -lolgx -L/usr/X11R6/lib -lX11" LEX="flex -l" LDOPTS="-s" YACC="byacc -d"
# use this to make an executable with static linked XView libs
make CC="gcc" CCOPTS="-O2 -m486 -fomit-frame-pointer" DBS_LIBDIR=/usr/share/fplan GFX_OPTIONS="-DGFX_XVIEW" GFX_INCLUDE="-I/usr/openwin/include" GFX_LIBS="-L/usr/openwin/lib -Wl,-Bstatic -lxview -lolgx -Wl,-Bdynamic -L/usr/X11R6/lib -lX11" LEX="flex -l" LDOPTS="-s" YACC="byacc -d"

%install
# clean start
rm -rf $RPM_BUILD_ROOT
# make the buildroot sub-directories we will need
install -d $RPM_BUILD_ROOT/usr/bin
install -d $RPM_BUILD_ROOT/usr/man/man1
install -d $RPM_BUILD_ROOT/usr/man/man5
# use the provided makefile to install everything
make BINDIR=$RPM_BUILD_ROOT/usr/bin install
make MANDIR=$RPM_BUILD_ROOT/usr/man install-man
# compress both the plain text and postscript format user's guide
gzip -9 guide.ps guide.txt

%clean
# remove buildroot
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%doc ANNOUNCE AUTHORS BUGS EXAMPLE?.fpl FAQ CHANGE.LOG LICENSE README TODO
%doc guide*.html guide.ps.gz guide.txt.gz
/usr/bin/fplan
/usr/bin/paddb
/usr/man/man1/fplan.1
/usr/man/man1/paddb.1
/usr/man/man5/fplan.5

