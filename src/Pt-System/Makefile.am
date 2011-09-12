AM_CPPFLAGS = -I$(top_srcdir)/src/Pt-System/posix -I$(top_srcdir)/include

lib_LTLIBRARIES = libPt-System.la

noinst_PROGRAMS = Pt-System-test

libPt_System_la_SOURCES = \
	posix/ApplicationImpl.cpp \
	posix/ClockImpl.cpp \
	posix/ConditionImpl.cpp \
	posix/DirectoryImpl.cpp \
	posix/FileDeviceImpl.cpp \
	posix/FileImpl.cpp \
	posix/IODeviceImpl.cpp \
	posix/LibraryImpl.cpp \
	posix/MainLoopImpl.cpp \
	posix/MutexImpl.cpp \
	posix/PipeImpl.cpp \
	posix/ProcessImpl.cpp \
	posix/SemaphoreImpl.cpp \
	posix/SerialDeviceImpl.cpp \
	posix/ThreadImpl.cpp \
	Application.cpp \
	Clock.cpp \
	Condition.cpp \
	Directory.cpp \
	EventLoop.cpp \
	EventSink.cpp \
	EventSource.cpp \
	File.cpp \
	FileDevice.cpp \
	FileInfo.cpp \
	IODevice.cpp \
	IOError.cpp \
	IOStream.cpp \
	Library.cpp \
	MainLoop.cpp \
	Mutex.cpp \
	Pipe.cpp \
	Plugin.cpp \
	Process.cpp \
	Selectable.cpp \
	Semaphore.cpp \
	SerialDevice.cpp \
	StreamBuffer.cpp \
	SystemError.cpp \
	Thread.cpp \
	Timer.cpp \
	Url.cpp \
    ConsoleChannel.cpp \
    FileChannel.cpp \
    Logger.cpp \
    LogManager.cpp \
    SerialChannel.cpp \
    Uri.cpp

Pt_System_test_SOURCES = \
	Pt-System-test.cpp \
	ApplicationTest.cpp \
	DirectoryTest.cpp \
	EventLoopTest.cpp \
	LoggerTest.cpp \
	PipeTest.cpp \
	ProcessTest.cpp \
	ThreadTest.cpp \
	TimerTest.cpp \
	UriTest.cpp

noinst_HEADERS = \
	posix/ApplicationImpl.h \
	posix/ClockImpl.h \
	posix/ConditionImpl.h \
	posix/DirectoryImpl.h \
	posix/FileDeviceImpl.h \
	posix/FileImpl.h \
	posix/FileInfoImpl.h \
	posix/IODeviceImpl.h \
	posix/LibraryImpl.h \
	posix/MainLoopImpl.h \
	posix/MutexImpl.h \
	posix/PipeImpl.h \
	posix/ProcessImpl.h \
	posix/SelectableImpl.h \
	posix/SemaphoreImpl.h \
	posix/SerialDeviceImpl.h \
	posix/ThreadImpl.h \
	ConsoleChannel.h \
	FileChannel.h \
	LogManager.h \
	SerialChannel.h


libPt_System_la_LIBADD = $(top_builddir)/src/Pt/libPt.la
libPt_System_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libPt_System_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

Pt_System_test_LDADD = $(top_builddir)/src/Pt-System/libPt-System.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la
