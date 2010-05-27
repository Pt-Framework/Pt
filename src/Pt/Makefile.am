AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt.la

libPt_la_SOURCES = \
	Atomicity.cpp \
	Char.cpp \
	Connection.cpp \
	Connectable.cpp \
	ConversionError.cpp \
	Date.cpp \
	DateTime.cpp \
	Deserializer.cpp \
	SerializationError.cpp \
	SerializationInfo.cpp \
	Serializer.cpp \
	SerializationContext.cpp \
	Settings.cpp \
	SettingsReader.cpp \
	SettingsWriter.cpp \
	Signal.cpp \
	StringStream.cpp \
	TextBuffer.cpp \
	TextCodec.cpp \
	TextStream.cpp \
	Time.cpp \
	Utf8Codec.cpp \
	Regex.cpp \
	regexp.cpp \
	Chunk.cpp \
	PoolFactory.cpp \
	PoolAllocator.cpp \
	Page.cpp \
	PageAllocator.cpp

noinst_HEADERS = \
	regexp.h \
	SerializationData.h \
	SettingsReader.h \
	SettingsWriter.h \
	Unicode.h

libPt_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libPt_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

