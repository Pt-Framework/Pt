AM_CPPFLAGS = -I$(top_srcdir)/include

noinst_PROGRAMS = Pt-System-test ProcessTestChild

Pt_System_test_SOURCES = \
	ApplicationTest.cpp \
	DirectoryTest.cpp \
	EventLoopTest.cpp \
	LoggerTest.cpp \
	PipeTest.cpp \
	ProcessTest.cpp \
	Pt-System-test.cpp \
	ThreadTest.cpp \
	TimerTest.cpp \
	UriTest.cpp

Pt_System_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-System/libPt-System.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la

ProcessTestChild_LDADD = $(top_builddir)/src/Pt/libPt.la
