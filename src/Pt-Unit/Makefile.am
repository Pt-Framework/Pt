AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Unit.la

libPt_Unit_la_SOURCES = \
	Application.cpp \
	Assertion.cpp \
	RegisterTest.cpp \
	Reporter.cpp \
	Test.cpp \
	TestCase.cpp \
	TestContext.cpp \
	TestProtocol.cpp \
	TestSuite.cpp

libPt_Unit_la_LIBADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-System/libPt-System.la

libPt_Unit_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
