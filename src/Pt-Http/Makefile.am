AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Http.la

libPt_Http_la_SOURCES = \
	ChunkedReader.cpp \
	Client.cpp \
	ClientImpl.cpp \
	MessageHeader.cpp \
	NotAuthenticatedResponder.cpp \
	NotAuthenticatedService.cpp \
	NotFoundResponder.cpp \
	NotFoundService.cpp \
	Parser.cpp \
	Responder.cpp \
	Server.cpp \
	ServerImpl.cpp \
	Service.cpp \
	Socket.cpp \
	Worker.cpp

noinst_HEADERS = \
	ChunkedReader.h \
	ClientImpl.h \
	Listener.h \
	NotAuthenticatedResponder.h \
	NotAuthenticatedService.h \
	NotFoundResponder.h \
	NotFoundService.h \
	Parser.h \
	ServerImpl.h \
	Socket.h \
	Worker.h

libPt_Http_la_LIBADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-System/libPt-System.la \
	$(top_builddir)/src/Pt-Net/libPt-Net.la

libPt_Http_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
