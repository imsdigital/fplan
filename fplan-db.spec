
Summary: airport and navaid database files for fplan
Name: fplan-db
Version: 1998.05.07
%define DB_VERSION pub
Release: 1%{DB_VERSION}
Group: Applications/Aviation
Copyright: Public Domain, NO WARRANTY!
URL: http://metalab.unc.edu/fplan/
Packager: John C. Peterson <jaypee@netcom.com>
Source: http://metalab.unc.edu/fplan/fplan-db-%{PACKAGE_VERSION}.%{DB_VERSION}.tar.gz
BuildArchitectures: noarch
Buildroot: /var/tmp/fplan-db-root

%description
This package contains the airports.nav and vors.nav database files
for fplan. They were created using the avdbtools software package
from the database files distributed by the FAA National Flight Data
Center, which can be found at;

          http://www.tgf.tc.faa.gov/nfdc/index.html

You should be aware of the fact that the database files from the
National Flight Data Center are not represented as official products
approved for navigation. To be specific, the NFDC world wide web
site clearly states; "FOR RESEARCH PURPOSES ONLY -- NOT CERTIFIED
FOR NAVIGATION". See the fplan user's guide for a discussion of
this and other issues related to database quality.

%prep
%setup -c

%build
# all too easy

%install
# clean start
rm -rf $RPM_BUILD_ROOT
# make the directories we will need
install -d $RPM_BUILD_ROOT/usr/share/fplan
# move database files to current directory
mv share/fplan/*.nav ./
# move the files with tar to the buildroot
tar cf - airports.nav vors.nav | (cd $RPM_BUILD_ROOT/usr/share/fplan; tar xf -)
# change permissions to read only
chmod 0444 $RPM_BUILD_ROOT/usr/share/fplan/*.nav

%clean
# remove buildroot
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
/usr/share/fplan/*.nav

