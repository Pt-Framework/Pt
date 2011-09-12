AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-Xml.la

libPt_Xml_la_SOURCES = \
	Characters.cpp \
	EndElement.cpp \
	EntityResolver.cpp \
	NamespaceContext.cpp \
	QName.cpp \
	StartElement.cpp \
	XmlDeserializer.cpp \
	XmlError.cpp \
	XmlFormatter.cpp \
	XmlReader.cpp \
	XmlSerializationContext.cpp \
	XmlSerializer.cpp \
	XmlTest.cpp \
	XmlWriter.cpp

libPt_Xml_la_LIBADD = $(top_builddir)/src/Pt/libPt.la
libPt_Xml_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
