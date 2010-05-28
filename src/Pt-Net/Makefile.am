AM_CPPFLAGS = -I$(top_srcdir)/src/Pt-Net/posix -I$(top_srcdir)/src/Pt-System/posix -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Net.la

libPt_Net_la_SOURCES = \
	posix/AddrInfoImpl.cpp \
	posix/TcpServerImpl.cpp \
	posix/TcpSocketImpl.cpp \
	AddrInfo.cpp \
	TcpServer.cpp \
	TcpSocket.cpp

noinst_HEADERS = \
	posix/AddrInfoImpl.h \
	posix/TcpServerImpl.h \
	posix/TcpSocketImpl.h

libPt_Net_la_LIBADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt/libPt-System.la

libPt_Net_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
