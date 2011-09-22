AM_CPPFLAGS = -I$(top_srcdir)/include

noinst_PROGRAMS = Pt-Xml-test

Pt_Xml_test_SOURCES = \
	Pt-Xml-test.cpp \
	XmlSerializerTest.cpp \
	XmlTest.cpp \
	XmlWriterTest.cpp

Pt_Xml_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-Xml/libPt-Xml.la \
	$(top_builddir)/src/Pt-Reflex/libPt-Reflex.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la
