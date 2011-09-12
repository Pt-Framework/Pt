AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Cma.la

libPt_Cma_la_SOURCES = \
	Component.cpp \
	ComponentFactory.cpp \
	ComponentLibrary.cpp \
	IUnknown.cpp

libPt_Cma_la_LIBADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-System/libPt-System.la

libPt_Cma_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
