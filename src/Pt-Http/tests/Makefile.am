AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/Pt-Http

noinst_PROGRAMS = Pt-Http-test

Pt_Http_test_SOURCES = \
	ParserTest.cpp \
	Pt-Http-test.cpp \
	ServerTest.cpp

Pt_Http_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-Http/libPt-Http.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la
