Name:           fwbuilder
Summary:        Firewall Builder
Version:        5.1.0.3599
Release:        5%{?dist}

License:        GPLv2+
Group:          Applications/System
URL:            http://www.fwbuilder.org/
Source:         http://downloads.sourceforge.net/fwbuilder/%{name}-%{version}.tar.gz
Source1:        fwbuilder.desktop
Patch0:         http://patch-tracker.debian.org/patch/series/dl/fwbuilder/5.1.0-3/ftbfs-gcc-4.7.diff
Buildroot:      %{_tmppath}/%{name}-%{version}-root

BuildRequires:  qt4-devel
BuildRequires:  desktop-file-utils
BuildRequires:  glib2-devel
BuildRequires:  gawk
BuildRequires:  autoconf automake libtool
BuildRequires:  libxslt-devel
Obsoletes:      libfwbuilder < %{version}-%{release}
Provides:       libfwbuilder%{?_isa} = %{version}-%{release}

%description
Firewall Builder consists of a GUI and set of policy compilers for
various firewall platforms. It helps users maintain a database of
objects and allows policy editing using simple drag-and-drop
operations. GUI generates firewall description in the form of XML
file, which compilers then interpret and generate platform-specific
code. Several algorithms are provided for automated network objects
discovery and bulk import of data. The GUI and policy compilers are
completely independent, this provides for a consistent abstract model
and the same GUI for different firewall platforms.

%prep
%setup -q
%patch0 -p1 -b .ftbfs-gcc-4.7


%build
export CXXFLAGS="$RPM_OPT_FLAGS -fno-var-tracking-assignments"
./autogen.sh
%configure \
    --with-templatedir=%{_datadir}/fwbuilder \
    --disable-dependency-tracking
make %{?_smp_mflags} all

%install
rm -rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT/usr/share/doc/%{name}-%{version}

# Add an icon for the menu system
for width in 16 24 32 48 72 128 256 512
do
install -Dpm 644 src/res/Icons/${width}x${width}/fwbuilder.png $RPM_BUILD_ROOT%{_datadir}/icons/hicolor/${width}x${width}/apps/fwbuilder.png
done

# Add menu entry
desktop-file-install \
  --dir $RPM_BUILD_ROOT/%{_datadir}/applications   \
  --add-category X-Fedora                          \
  %{SOURCE1}

%clean
rm -rf $RPM_BUILD_ROOT

%post
gtk-update-icon-cache -qf %{_datadir}/icons/hicolor >/dev/null 2>&1 || :

%postun
gtk-update-icon-cache -qf %{_datadir}/icons/hicolor >/dev/null 2>&1 || :


%files
%defattr(-,root,root)
%{_datadir}/fwbuilder
%{_datadir}/applications/*.desktop
%{_datadir}/icons/hicolor/*/apps/fwbuilder.png
%{_bindir}/fwbuilder
%{_bindir}/fwbedit
%doc doc/AUTHORS
%doc doc/COPYING
%doc doc/Credits
%doc doc/ChangeLog
%doc doc/PatchAcceptancePolicy.txt
%doc doc/README.floppyfw
%doc doc/ReleaseNotes_*.html
%{_mandir}/man1/fwbuilder.1*
%{_mandir}/man1/fwbedit.1*


%package pf
Summary:  Policy Compiler for OpenBSD PF
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description pf
Policy compiler for OpenBSD PF

%files pf
%defattr(-,root,root)
%{_bindir}/fwb_pf
%doc doc/README.pf
%{_mandir}/man1/fwb_pf.1*


%package ipf
Summary:  Policy Compiler for ipfilter
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description ipf
Policy compiler for ipfilter

%files ipf
%defattr(-,root,root)
%{_bindir}/fwb_ipf
%doc doc/README.ipf
%{_mandir}/man1/fwb_ipf.1*


%package ipfw
Summary:  Policy Compiler for ipfw
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description ipfw
Policy compiler for ipfw

%files ipfw
%defattr(-,root,root)
%{_bindir}/fwb_ipfw
%doc doc/README.ipfw
%{_mandir}/man1/fwb_ipfw.1*


%package ipt
Summary:  Policy Compiler for IPTables
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description ipt
Policy compiler for IPTables

%files ipt
%defattr(-,root,root)
%{_bindir}/fwb_ipt
%doc doc/README.ipt
%{_mandir}/man1/fwb_ipt.1*


%package cisco
Summary:  Policy Compiler for Cisco routers/firewalls
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description cisco
Policy compiler for Cisco routers/firewalls

%files cisco
%defattr(-,root,root)
%{_bindir}/fwb_iosacl
%{_bindir}/fwb_pix
%doc doc/README.iosacl
%doc doc/README.pix
%{_mandir}/man1/fwb_iosacl.1*
%{_mandir}/man1/fwb_pix.1*

%package procurve
Summary:  Policy Compiler for Procurve devices
Group:    Applications/System
Requires: %{name} = %{version}-%{release}

%description procurve
Policy compiler for Procurve devices

%files procurve
%defattr(-,root,root)
%{_bindir}/fwb_procurve_acl

%changelog
* Sat Aug 03 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 5.1.0.3599-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_20_Mass_Rebuild

* Fri Mar 15 2013 Jon Ciesla <limburgher@gmail.com> - 5.1.0.3599-4
- Drop desktop vendor tag.

* Wed Feb 13 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 5.1.0.3599-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Mon Jul 23 2012 Till Maas <opensource@till.name> - 5.1.0.3599-2
- Add %%{?isa} to Provides:

* Mon Jul 23 2012 Till Maas <opensource@till.name> - 5.1.0.3599-1
- Update to new release
- Adjust Provides/Obsoletes/Requires: libfwbuilder does not exist anymore
- Remove unused patch

* Thu Jul 19 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 4.1.2-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Fri Jan 13 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 4.1.2-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Feb 08 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 4.1.2-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Sun Nov 28 2010 Ralf Ertzinger <ralf@skytale.nte> 4.1.2-1
- Update to 4.1.2

* Tue Nov 24 2009 Kevin Kofler <Kevin@tigcc.ticalc.org> 3.0.7-2
- Rebuild for Qt 4.6.0 RC1 in F13 (was built against Beta 1 with unstable ABI)

* Sat Nov 07 2009 Ralf Ertzinger <ralf@skytale.net> 3.0.7-1
- Update to 3.0.7
- Add -fno-var-tracking-assignments to CXXFLAGS due to
  https://bugzilla.redhat.com/show_bug.cgi?id=532763

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.0.5-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Sun Jun 21 2009 Ralf Ertzinger <ralf@skytale.net> 3.0.5-1
- Update to 3.0.5

* Mon Feb 23 2009 Ralf Ertzinger <ralf@skytale.net> 3.0.3-1
- Update to 3.0.3

* Wed Nov 19 2008 Ralf Ertzinger <ralf@skytale.net> 3.0.1-1
- Update to 3.0.1

* Sun Oct 19 2008 Ralf Ertzinger <ralf@skytale.net> 3.0.0-2
- Just keep fedora-fwbuilder.desktop (fixes BZ467618)

* Sat Sep 06 2008 Ralf Ertzinger <ralf@skytale.net> 3.0.0-1
- Update to 3.0.0

* Mon Feb 18 2008 Fedora Release Engineering <rel-eng@fedoraproject.org> - 2.1.16-2
- Autorebuild for GCC 4.3

* Thu Jan 03 2008 Ralf Ertzinger <ralf@skytale.net> 2.1.16-1
- Update to 2.1.16
- Add patch to enable compilation with GCC4.3

* Wed Oct 03 2007 Ralf Ertzinger <ralf@skytale.net> 2.1.14-1
- Update to 2.1.14
- Fix license tag

* Wed Aug 29 2007 Fedora Release Engineering <rel-eng at fedoraproject dot org> - 2.1.12-3
- Add gawk as a Build Require.

* Tue Aug 28 2007 Fedora Release Engineering <rel-eng at fedoraproject dot org> - 2.1.12-2
- Rebuild for selinux ppc32 issue.

* Fri Jul 06 2007 Ralf Ertzinger <ralf@skytale.net> 2.1.12-1.fc8
- Update to 2.1.12

* Wed Mar 14 2007 Ralf Ertzinger <ralf@skytale.net> 2.1.10-1.fc7
- Update to 2.1.10

* Sun Dec 24 2006 Ralf Ertzinger <ralf@skytale.net> 2.1.8-1.fc7
- Update to 2.1.8

* Wed Nov 29 2006 Ralf Ertzinger <ralf@skytale.net> 2.1.7-1.fc7
- Bump release for build
- Add fwbuilder symlink

* Mon Nov 27 2006 Ralf Ertzinger <ralf@skytale.net> 2.1.7-0.2.fc7
- Update to 2.1.7

* Tue Oct 24 2006 Ralf Ertzinger <ralf@skytale.net> 2.1.6-0.0.beta.fc7
- Update to beta release 2.1.6

* Mon Sep 11 2006 Ralf Ertzinger <ralf@skytale.net> 2.0.12-2.fc6
- Bumped and rebuilt for FC6

* Mon May 22 2006 Ralf Ertzinger <ralf@skytale.net> 2.0.12-1.fc6
- Update to 2.0.12
- Drop --ffriend-injection

* Wed Feb 22 2006 Ralf Ertzinger <ralf@skytale.net> 2.0.10-3.fc5
- Bump and rebuild for FC5

* Wed Feb 08 2006 Ralf Ertzinger <ralf@skytale.net> 2.0.10-2
- Fixes for BZ165308 (thanks to Ville Skyttä)
  - Don't strip binaries, fixes debuginfo
  - Bring desktop entry categories up to date, so the entry is actually shown
  - Make icon themeable
  - Own all installed dirs

* Sun Feb 05 2006 Ralf Ertzinger <ralf@skytale.net> 2.0.10-1 
- Update to 2.0.10
- Spec file cleanup

* Wed Jul 13 2005 Steve Grubb <sgrubb@redhat.com> 2.0.8-1
- New upstream version

* Sun May 22 2005 Jeremy Katz <katzj@redhat.com> - 2.0.6-3
- rebuild on all arches

* Thu Apr  7 2005 Michael Schwendt <mschwendt[AT]users.sf.net>
- rebuilt

* Thu Mar  3 2005 Steve Grubb <sgrubb@redhat.com> 2.0.6-1
- New upstream version

* Sun Jan 23 2005 Warren Togami <wtogami@redhat.com> 2.0.5-3
- Typo fix (#145887 Stefan Hoelldampf)

* Sat Jan 22 2005 Warren Togami <wtogami@redhat.com> 2.0.5-2
- minor spec cleanup

* Sat Jan 22 2005 Steve Grubb <sgrubb@redhat.com> 2.0.5
- Created base spec file

