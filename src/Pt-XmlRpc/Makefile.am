AM_CPPFLAGS =  -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-XmlRpc.la

libPt_XmlRpc_la_SOURCES = \
	Client.cpp \
	ClientImpl.cpp \
	Fault.cpp \
	Formatter.cpp \
	HttpClient.cpp \
	HttpClientImpl.cpp \
	RemoteProcedure.cpp \
	Responder.cpp \
	Scanner.cpp \
	Service.cpp

noinst_HEADERS = \
	ClientImpl.h \
	HttpClientImpl.h

libPt_XmlRpc_la_LIBADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-System/libPt-System.la \
	$(top_builddir)/src/Pt-Net/libPt-Net.la \
	$(top_builddir)/src/Pt-Http/libPt-Http.la \
	$(top_builddir)/src/Pt-Xml/libPt-Xml.la

libPt_XmlRpc_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
