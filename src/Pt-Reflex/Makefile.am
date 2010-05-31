AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Reflex.la

libPt_Reflex_la_SOURCES = \
	PropertyInfo.cpp \
	Reflectable.cpp \
	Type.cpp

libPt_Reflex_la_LIBADD = $(top_builddir)/src/Pt/libPt.la

libPt_Reflex_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
