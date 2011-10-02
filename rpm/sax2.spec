#
# spec file for package sax2 (Version 8.1)
#
# Copyright (c) 2010 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild

%if %{suse_version} > 1120
%define use_gcc43 0
%else
%define use_gcc43 0
%endif

Name:           sax2
Url:            http://sax.berlios.de
%define build_java 0
%if %{suse_version} > 1010
BuildRequires:  antlr bison doxygen flex gettext-devel ghostscript-fonts-std
BuildRequires:  graphviz hal-devel python-devel
BuildRequires:  libqt4 libqt4-devel libqt4-qt3support libqt4-x11
BuildRequires:  hwinfo-devel readline-devel swig update-desktop-files
BuildRequires:  xorg-x11-server-sdk
%if %{suse_version} > 1020
BuildRequires:  fdupes
%endif
%if %use_gcc43
BuildRequires:  gcc43 gcc43-c++ -gcc
%endif
%if %{build_java}
BuildRequires:  java-1_4_2-gcj-compat-devel
%endif
%endif
%if %{suse_version} <= 1010
BuildRequires:  doxygen ghostscript-fonts-std graphviz hal-devel python-devel
BuildRequires:  qt qt-devel qt-qt3support qt-x11
BuildRequires:  gcc-c++ readline-devel swig sysfsutils update-desktop-files
BuildRequires:  freetype2-devel libpng-devel
%if %{build_java}
BuildRequires:  java2-devel-packages
%endif
%endif
Requires:       perl = %{perl_version}
Requires:       readline ncurses hal dbus-1 sax2-libsax
Requires:       sax2-ident sax2-tools
Requires:       xorg-x11-server
Requires:       xdg-utils
PreReq:         /bin/rm /bin/mkdir /usr/bin/chroot %fillup_prereq %insserv_prereq
Summary:        SuSE advanced X Window System-configuration
Version:        8.1
Release:        591
Group:          System/X11/Utilities
License:        GPLv2+
Source:         sax2.tar.bz2
Source1:        sax2.desktop
Source2:        sax2-rpmlintrc
%if %use_gcc43
Patch0:         sax2-gcc43.diff
%endif
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
This package contains the SuSE Advanced X-Configuration



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-tools
License:        GPLv2+
Summary:        X Window System tools for SaX2
Group:          System/X11/Utilities
Requires:       coreutils
Provides:       saxtools
Obsoletes:      saxtools
Supplements:    packageand(yast2-installation:xorg-x11)
%ifarch s390x
Provides:       sax2
Obsoletes:      sax2
Provides:       sax2-gui
Obsoletes:      sax2-gui
Provides:       sax2-libsax
Obsoletes:      sax2-libsax
Provides:       sax2-libsax-perl
Obsoletes:      sax2-libsax-perl
Provides:       sax2-ident
Obsoletes:      sax2-ident
%endif

%description -n sax2-tools
Some small X Window System tools to handle input devices, for example,
mouse and keyboard.



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%ifnarch s390 s390x

%package -n sax2-ident
License:        LGPLv2.1+
Summary:        SaX2 identity and profile information
Group:          System/X11/Utilities
Provides:       sax2:/usr/share/sax/sysp/maps/Identity.map
Provides:       sax2-tools:/usr/share/sax/sysp/maps/Identity.map
Provides:       saxident
Obsoletes:      saxident
Requires:       sax2 sax2-libsax-perl

%description -n sax2-ident
This package contains information about the supported graphics hardware
and its special parameters. For some graphics cards a profile is needed
to describe configuration parameters outside the ordinary way of
setting up the card with SaX2.



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-gui
License:        GPLv2+
Requires:       netpbm sax2-tools icewm-bin sax2
Summary:        SuSE advanced X Window System-configuration GUI
Group:          System/X11/Utilities
Provides:       sax2:/usr/sbin/xapi

%description -n sax2-gui
This package contains the GUI for the SuSE Advanced X-Configuration



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-libsax
License:        GPLv2+
Requires:       perl = %{perl_version}
Requires:       sax2-tools sax2-libsax-perl icewm-bin
%if %{suse_version} <= 1020
%ifarch       %ix86 x86_64
Requires:       915resolution
%endif
%endif
Summary:        SaX management library for X Window System-configuration
Group:          Development/Libraries/X11
Provides:       sax2:/usr/%{_lib}/libsax.so

%description -n sax2-libsax
libsax provides a C++ written library to manage X11 configurations



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-libsax-devel
License:        GPLv2+
Requires:       sax2-libsax = %version
Summary:        SaX header files for X Window System-configuration
Group:          Development/Libraries/X11

%description -n sax2-libsax-devel
The devel package of libsax provides all header files needed to use
libsax in your own development environment



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-libsax-perl
License:        GPLv2+
Requires:       perl = %{perl_version}
Summary:        Language binding to use libsax with perl
Group:          Development/Libraries/X11

%description -n sax2-libsax-perl
This package provides a wrapper to be able to use libsax in perl
written programs



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%package -n sax2-libsax-python
License:        GPLv2+
Requires:       sax2-libsax
Summary:        Language binding to use libsax with python
Group:          Development/Libraries/X11
%py_requires

%description -n sax2-libsax-python
This package provides a wrapper to be able to use libsax in python
written programs



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%if %{build_java}

%package -n sax2-libsax-java
License:        GPL v2 or later
Requires:       sax2-libsax jre1.2.x
Summary:        Language binding to use libsax with java
Group:          Development/Libraries/X11

%description -n sax2-libsax-java
This package provides a wrapper to be able to use libsax in java
written programs



Authors:
--------
    Marcus Schaefer <ms@suse.de>

%endif
%endif

%prep
%setup -n sax
%if %use_gcc43
%patch0 -p1
%endif

%build
%if %use_gcc43
CC=gcc-4.3
CXX=g++-4.3
export CC CXX
%endif
# disable as-needed to fix build
export SUSE_ASNEEDED=0
test -e /.buildenv && . /.buildenv
test -z "$QTDIR" && export QTDIR=/usr/lib/qt-3.3
test -z "$JAVA_BINDIR" && export JAVA_BINDIR=/usr/bin
test -z "$BUILD_DISTRIBUTION_NAME" && export BUILD_DISTRIBUTION_NAME=OTHER
#=================================================
# add SuSE version to sax.sh script...
#-------------------------------------------------
cat ./startup/sax.sh | \
	sed -e s@SuSE-Linux@"$BUILD_DISTRIBUTION_NAME"@ \
> /tmp/sax.sh
cp /tmp/sax.sh ./startup/sax.sh
#=================================================
# build sources
#-------------------------------------------------
make bindlib=%{_lib} buildroot=$RPM_BUILD_ROOT
#=================================================
# install sources
#-------------------------------------------------

%install
%ifnarch s390 s390x
mkdir -p $RPM_BUILD_ROOT/var/log
touch $RPM_BUILD_ROOT/var/log/SaX.log
make buildroot=$RPM_BUILD_ROOT \
	 bindlib=%{_lib} \
	 lib_prefix=$RPM_BUILD_ROOT/usr/%{_lib} \
	 doc_prefix=$RPM_BUILD_ROOT/%{_defaultdocdir} \
	 man_prefix=$RPM_BUILD_ROOT/%{_mandir} \
	 install
find $RPM_BUILD_ROOT -name "*.bs" | xargs rm -f
%find_lang sax
# SuSE specific build instructions...
#=================================================
# check perl .packlist...
#-------------------------------------------------
%perl_process_packlist
#=================================================
# remove unpacked sources...
#-------------------------------------------------
rm -f $RPM_BUILD_ROOT/%{perl_vendorarch}/*.pl
#=================================================
# update desktop file
#-------------------------------------------------
%suse_update_desktop_file -i %name System SystemSetup
#=================================================
# file duplicates
#-------------------------------------------------
%if %{suse_version} > 1020
%fdupes $RPM_BUILD_ROOT/usr/share/sax/api/figures
%endif
%else
make buildroot=$RPM_BUILD_ROOT \
         bindlib=%{_lib} \
         lib_prefix=$RPM_BUILD_ROOT/usr/%{_lib} \
         doc_prefix=$RPM_BUILD_ROOT/%{_defaultdocdir} \
         man_prefix=$RPM_BUILD_ROOT/%{_mandir} \
         install-docs
cd tools
make buildroot=$RPM_BUILD_ROOT \
         bindlib=%{_lib} \
         lib_prefix=$RPM_BUILD_ROOT/usr/%{_lib} \
         doc_prefix=$RPM_BUILD_ROOT/%{_defaultdocdir} \
         man_prefix=$RPM_BUILD_ROOT/%{_mandir} \
         install
cd ..
rm -rf $RPM_BUILD_ROOT/usr/lib/perl5
rm -rf $RPM_BUILD_ROOT/usr/share/doc/packages/sax2/{LICENSE,README,sax.pdf}
rm $RPM_BUILD_ROOT/%{_mandir}/man1/sax2.1
%endif
#=================================================
# uninstall script stage:[previous]
#-------------------------------------------------

%preun
chroot . rm -f /var/cache/xfine/*
if [ ! -d /var/cache/xfine ];then
	mkdir -p /var/cache/xfine
fi
%ifnarch s390 s390x

%post -n sax2-ident
if ls var/lib/hardware/ids/* &> /dev/null; then
  >  var/lib/hardware/hd.ids
  for i in var/lib/hardware/ids/*; do
    cat $i >> var/lib/hardware/hd.ids
  done
fi

%postun -n sax2-ident
if [ "$1" -eq 0 ]; then
  if ls var/lib/hardware/ids/* &> /dev/null; then
    >  var/lib/hardware/hd.ids
    for i in var/lib/hardware/ids/*; do
      cat $i >> var/lib/hardware/hd.ids
    done
  else
    rm -f var/lib/hardware/hd.ids
  fi
fi

%post -n sax2-libsax -p /sbin/ldconfig

%postun -n sax2-libsax -p /sbin/ldconfig

%files
%defattr(-,root,root)
#=================================================
# SaX files...
#-------------------------------------------------
%ghost %config(noreplace) /var/log/SaX.log
%dir %{_datadir}/sax/api
%dir %{_datadir}/sax/api/data
%dir %{_defaultdocdir}/sax2
%dir %{_datadir}/sax
%dir %{_datadir}/sax/sysp/script
%dir %{_datadir}/sax/sysp
%dir %{_datadir}/sax/libsax
%dir %{_var}/lib/sax
%dir /var/cache/sax/sysp
%dir /var/cache/sax/sysp/rdbms
%dir /var/cache/sax/files
%dir /var/cache/sax
%dir /var/cache/xfine
%dir /etc/icewm
%{_datadir}/sax/libsax/*
%{_datadir}/pixmaps/sax2.png
%{_datadir}/sax/svnbuild
%{_datadir}/sax/init.pl
%{_datadir}/sax/xc.pl
%{_datadir}/sax/pci.pl
%{_datadir}/sax/modules
%{_sbindir}/sax.sh
%{_sbindir}/sax2-vesa
%{_sbindir}/SaX2
%{_sbindir}/sax2
%{_sbindir}/sysp
%{_sbindir}/xcmd
%{_datadir}/sax/api/data/fvwmrc.sax
%{_datadir}/sax/api/data/twmrc.sax
%config /etc/icewm/icerc.sax
%{_datadir}/sax/api/data/CardModules
%{_datadir}/sax/api/data/LangCodes
%{_datadir}/sax/api/data/PointerDevice
%{_datadir}/sax/api/data/LangFirstPath
%{_datadir}/sax/api/data/StaticFontPathList
%{_datadir}/sax/api/data/MonitorColors
%{_datadir}/sax/api/data/TabletModules
%{_datadir}/sax/api/data/MonitorRatio
%{_datadir}/sax/api/data/MonitorResolution
%{_datadir}/sax/api/data/MonitorTraversal
%{_datadir}/sax/sysp/script/installed.pl
%{_datadir}/sax/sysp/script/killdot.pl
%{_datadir}/sax/sysp/script/memory.pl
%{_datadir}/sax/sysp/script/machine.pl
%{_datadir}/sax/sysp/script/preparelog.pl
%{_datadir}/sax/sysp/script/checkmap.pl
%{_datadir}/sax/sysp/script/profilecount.pl
%{_datadir}/sax/sysp/script/psection.pl
%{_datadir}/sax/sysp/script/profiledriver.pl
%{_datadir}/sax/sysp/script/vendor.pl
%doc %{_defaultdocdir}/sax2/LICENSE
%doc %{_defaultdocdir}/sax2/README
%doc %{_mandir}/man1/sax2.1.gz
#=================================================
# SaX-GUI file list...
# ------------------------------------------------

%files -n sax2-gui -f sax.lang
%defattr(-,root,root)
%dir %{_datadir}/sax/api
%dir %{_datadir}/xfine
%{_datadir}/sax/api/macros
%{_datadir}/sax/api/figures
%{_datadir}/xfine/figures
%{_datadir}/xfine/xfine.gtx
%{_datadir}/sax/api/data/xapi.gtx
%{_sbindir}/xapi
%{_sbindir}/xfine
%{_datadir}/applications/sax2.desktop
%endif
#=================================================
# SaX-Tools file list...
# ------------------------------------------------

%files -n sax2-tools
%defattr(-,root,root)
%doc %{_mandir}/man1/xkbctrl.1.gz
%doc %{_mandir}/man1/xmode.1.gz
%doc %{_mandir}/man1/xquery.1.gz
%{_sbindir}/corner
%{_sbindir}/dots
%{_sbindir}/isax
%{_sbindir}/testX
%{_sbindir}/whereiam
%{_sbindir}/wmstart
%{_sbindir}/ximage
%{_sbindir}/xidle
%{_sbindir}/xkbctrl
%{_sbindir}/xlook
%{_sbindir}/xmode
%{_sbindir}/xquery
%{_sbindir}/vncp
%{_sbindir}/xw
%{_sbindir}/getPrimary
%ifnarch s390 s390x
#=================================================
# SaX-Ident file list...
# ------------------------------------------------

%files -n sax2-ident
%defattr(-,root,root)
#%dir /etc/udev/rules.d
%dir %{_datadir}/sax
%dir %{_datadir}/sax/api
%dir %{_datadir}/sax/api/data/cdb
%dir %{_datadir}/sax/api/data
%dir %{_datadir}/sax/sysp/maps
%dir %{_datadir}/sax/sysp
%dir /var/lib/hardware
#%config /etc/udev/rules.d/*.rules
%{_datadir}/sax/sysp/maps/Identity.map
%{_datadir}/sax/sysp/maps/Keyboard.map
%{_datadir}/sax/sysp/maps/Vendor.map
%{_datadir}/sax/sysp/maps/Input.map
%{_datadir}/sax/sysp/maps/Driver.map
%{_datadir}/sax/sysp/maps/IntelPatch.map
%{_datadir}/sax/api/data/cdb/Cards
%{_datadir}/sax/api/data/cdb/Pads
%{_datadir}/sax/api/data/cdb/Pens
%{_datadir}/sax/api/data/cdb/Pointers
%{_datadir}/sax/api/data/cdb/Tablets
%{_datadir}/sax/api/data/cdb/Touchscreens
%config %{_datadir}/sax/api/data/cdb/Monitors
%{_datadir}/sax/profile
/var/lib/hardware/ids
#=================================================
# SaX-libsax file list...
# ------------------------------------------------

%files -n sax2-libsax
%defattr(-,root,root)
%{_prefix}/%{_lib}/libsax.so.*
%{perl_vendorarch}/XFree.pm
%{perl_vendorarch}/auto/XFree
%{perl_vendorarch}/PLog.pm
%{perl_vendorarch}/auto/PLog
%{perl_vendorarch}/SPP.pm
%{perl_vendorarch}/auto/SPP
%{perl_vendorarch}/FBSet.pm
%{perl_vendorarch}/auto/FBSet
%{perl_vendorarch}/CVT.pm
%{perl_vendorarch}/auto/CVT
/var/adm/perl-modules/sax2
#=================================================
# SaX-libsax-devel file list...
# ------------------------------------------------

%files -n sax2-libsax-devel
%defattr(-,root,root)
%dir %{_includedir}/sax
%dir %{_defaultdocdir}/libsax
%doc %{_defaultdocdir}/libsax/html
%doc %{_defaultdocdir}/sax2/sax.pdf
%{_prefix}/%{_lib}/libsax.so
%{_includedir}/sax/*
#=================================================
# SaX-libsax-perl file list...
# ------------------------------------------------

%files -n sax2-libsax-perl
%defattr(-,root,root)
%{perl_vendorarch}/SaX.pm
%{perl_vendorarch}/auto/SaX
#=================================================
# SaX-libsax-python file list...
# ------------------------------------------------

%files -n sax2-libsax-python
%defattr(-,root,root)
%dir %{py_sitedir}/SaX
%{py_sitedir}/SaX.pth
%{py_sitedir}/SaX/*
#=================================================
# SaX-libsax-java file list...
# ------------------------------------------------
%if %{build_java}

%files -n sax2-libsax-java
%defattr(-,root,root)
%dir %{_prefix}/%{_lib}/sax
%dir %{_prefix}/%{_lib}/sax/plugins
%{_datadir}/java/SaX.jar
%{_prefix}/%{_lib}/sax/plugins/SaX.so
%endif
%endif


%changelog
