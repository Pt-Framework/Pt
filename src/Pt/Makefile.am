AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt.la

noinst_PROGRAMS = Pt-test

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

Pt_test_SOURCES = \
	Pt-test.cpp \
	AllocatorTest.cpp \
	AnyTest.cpp \
	AtomicTest.cpp \
	ByteorderTest.cpp \
	CharTest.cpp \
	ConversionTest.cpp \
	DateTest.cpp \
	DateTimeTest.cpp \
	DelegateTest.cpp \
	ExceptionTest.cpp \
	NonCopyableTest.cpp \
	RegExpTest.cpp \
	SerializationTest.cpp \
	SettingsTest.cpp \
	SingletonTest.cpp \
	SignalTest.cpp \
	SmartPtrTest.cpp \
	StringStreamTest.cpp \
	StringTest.cpp \
	TextStreamTest.cpp \
	TimeTest.cpp \
	TimespanTest.cpp \
	TypesTest.cpp \
	TypeTraitsTest.cpp \
	VariantTest.cpp

noinst_HEADERS = \
	regexp.h \
	SerializationData.h \
	SettingsReader.h \
	SettingsWriter.h \
	Unicode.h

libPt_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libPt_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

Pt_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la
