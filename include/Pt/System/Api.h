/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_H
#define PT_SYSTEM_API_H

#include <Pt/Api.h>

#define PT_SYSTEM_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_SYSTEM_VERSION_MINOR PT_VERSION_MINOR
#define PT_SYSTEM_VERSION_REVISION PT_VERSION_REVISION
#define PT_SYSTEM_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_SYSTEM_API_EXPORT)
#    define PT_SYSTEM_API PT_EXPORT
#  else
#    define PT_SYSTEM_API PT_IMPORT
#  endif

#endif

namespace Pt {

/** @namespace Pt::System
    @brief %System programming

    This module is the portable operating-system layer for a process. It
    covers the file system, threads and processes, event loops, I/O
    devices, logging, shared libraries, plugins, tar archives, and the
    clock.
*/
namespace System {

    class Application;
    class Clock;
    class Condition;
    class Directory;
    class EventSource;
    class EventSink;
    class EventLoop;
    class File;
    class FileDevice;
    class FileInfo;
    class IOBuffer;
    class IODevice;
    class IOStream;
    class IStream;
    class Logger;
    class LogChannel;
    class LogMessage;
    class LogTarget;
    class MainLoop;
    class Mutex;
    class OStream;
    class Path;
    class Pipe;
    class PluginId;
    class Process;
    class ReadWriteMutex;
    class Selectable;
    class Semaphore;
    class SerialDevice;
    class SharedLib;
    class StreamBuffer;
    class SystemError;
    class SpinLock;
    class Thread;
    class Timer;
    class Url;

} // namespace System

} // namespace Pt
