AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt.la

libPt_la_SOURCES = \
	Atomicity.cpp \
	Char.cpp \
	Connection.cpp \
	Convert.cpp \
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
	Chunk.h \
	Page.h \
	PoolFactory.h \
	SerializationData.h \
	SettingsReader.h \
	SettingsWriter.h \
	Unicode.h \
	Atomicity.cw.x86.cpp \
	Atomicity.gcc.arm.cpp \
	Atomicity.gcc.avr32.cpp \
	Atomicity.gcc.mips.cpp \
	Atomicity.gcc.ppc.cpp \
	Atomicity.gcc.sparc.cpp \
	Atomicity.gcc.x86_64.cpp \
	Atomicity.gcc.x86.cpp \
	Atomicity.generic.cpp \
	Atomicity.pthread.cpp \
	Atomicity.sun.cpp \
	Atomicity.windows.cpp \
	Facets.cpp

libPt_la_CXXFLAGS = $(PTHREAD_CFLAGS) -D@PT_ATOMICITY@
libPt_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

