AM_CPPFLAGS = -I$(top_srcdir)/include

noinst_PROGRAMS = Pt-test

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

Pt_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la
