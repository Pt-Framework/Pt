CvsID: 20050826
Name: Platinum
Version: 0.0.4
Release: 1ark
Summary: The Platínum Framework
%if "%cvsid" != ""
Source: %name-%cvsid.tar.bz2
%else
Source: %name-%version.tar.bz2
%endif
URL: http://www.platinum-framework.org
License: LGPL
Group: Libraries
Prefix: %{_prefix}
BuildRoot: %{_tmppath}/%{name}-root

%description
A portable C++ framework.


### devel ###
%package devel
Summary: Development files for %name
Group: Development/C++
Requires: %name = %version-%release

%description devel
Development files for %name.

Install %name-devel if you wish to develop or compile applications
that use %name.


### AV ###
%package AV
Summary: AV classes for %name
Group: Libraries
Requires: %name = %version-%release

%description AV
AV classes for %name.


### AV-devel ###
%package AV-devel
Summary: Development files for %name
Group: Development/C++
Requires: %name = %version-%release

%description AV-devel
Development files for %name


### Net ###
%package Net
Summary: Network classes for %name
Group: Libraries
Requires: %name = %version-%release

%description Net
Network classes for %name.


### Net-devel ###
%package Net-devel
Summary: Development files for %name
Group: Development/C++
Requires: %name = %version-%release

%description Net-devel
Development files for %name


### XML ###
%package XML
Summary: XML classes for %name
Group: Libraries
Requires: %name = %version-%release

%description XML
XML classes for %name.


### XML-devel ###
%package XML-devel
Summary: Development files for %name
Group: Development/C++
Requires: %name = %version-%release

%description XML-devel
Development files for %name


### XML-RPC ###
%package XML-RPC
Summary: XML-RPC classes for %name
Group: Libraries
Requires: %name = %version-%release

%description XML-RPC
XML-RPC classes for %name.


### XML-RPC-devel ###
%package XML-RPC-devel
Summary: Development files for %name
Group: Development/C++
Requires: %name = %version-%release

%description XML-RPC-devel
Development files for %name


%package alsa
Summary: ALSA audio support plugin for %name
Group: Development/C++
Requires: %name = %version-%release

%description alsa
ALSA audio support plugin for %name


%package ffmpeg
Summary: ffmpeg video decoder support plugin for %name
Group: Development/C++
Requires: %name = %version-%release

%description ffmpeg
ffmpeg video decoder support plugin for %name


%package oss
Summary: OSS audio support plugin for %name
Group: Development/C++
Requires: %name = %version-%release

%description oss
OSS audio support plugin for %name


%package theora
Summary: Theora codec support for %name
Group: Development/C++
Requires: %name = %version-%release

%description theora
Theora codec support plugin for %name

### static ###
%package static
Summary: Library files for linking statically to %name
Group: Development/C++/Static
Requires: %name-devel = %version-%release

%description static
Static libraries for linking to %name.

Install %name-static if you wish to develop or compile applications
that use %name without needing %name installed on the target system.


%prep

%setup -q -n %name
test -f Makefile.cvs && make -f Makefile.cvs

%configure

%build
make %?_smp_mflags

%install
rm -rf $RPM_BUILD_ROOT
make %?_smp_mflags install DESTDIR="$RPM_BUILD_ROOT"

%files
%defattr(-,root,root)
%_libdir/libPt.so*
%_libdir/libPt.la
%dir %_libdir/Pt/Plugins

%files devel
%defattr(-,root,root)
%dir %_includedir/Pt
%_includedir/Pt/*.h
%_includedir/Pt/System
%_includedir/Pt/Text

%files AV
%defattr(-,root,root)
%_libdir/libPt-AV.so*
%_libdir/libPt-AV.la
%dir %_libdir/Pt/Plugins/AV

%files AV-devel
%defattr(-,root,root)
%dir %_includedir/Pt/AV
%_includedir/Pt/AV/*.h

%files Net
%defattr(-,root,root)
%_libdir/libPt-Net.so*
%_libdir/libPt-Net.la

%files Net-devel
%defattr(-,root,root)
%dir %_includedir/Pt/Net
%_includedir/Pt/Net/*.h

%files XML
%defattr(-,root,root)
%_libdir/libPt-Xml.so*
%_libdir/libPt-Xml.la

%files XML-devel
%defattr(-,root,root)
%dir %_includedir/Pt/Xml
%_includedir/Pt/Xml/*.h

%files XML-RPC
%defattr(-,root,root)
%_libdir/libPt-XmlRpc.so*
%_libdir/libPt-XmlRpc.la

%files XML-RPC-devel
%defattr(-,root,root)
%_includedir/Pt/XmlRpc

%files alsa
%defattr(-,root,root)
%_libdir/Pt/Plugins/AV/ALSAAudio.la
%_libdir/Pt/Plugins/AV/ALSAAudio.so

%files ffmpeg
%defattr(-,root,root)
%_libdir/Pt/Plugins/AV/FfmpegDecoder.la
%_libdir/Pt/Plugins/AV/FfmpegDecoder.so

%files oss
%defattr(-,root,root)
%_libdir/Pt/Plugins/AV/OSSAudio.la
%_libdir/Pt/Plugins/AV/OSSAudio.so

%files theora
%defattr(-,root,root)
%_libdir/Pt/Plugins/AV/TheoraDecoder.la
%_libdir/Pt/Plugins/AV/TheoraDecoder.so

%files static
%defattr(-,root,root)
%_libdir/libPt.a
%_libdir/libPt-*.a
%_libdir/Pt/Plugins/AV/*.a

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%changelog
* Fri Aug 26 2005 Bernhard Rosenkraenzer <bero@arklinux.org> 0.0.4-1ark
- Update, spec file fixes

* Sat May 14 2005 marcd
- 0.0.1 initial build
