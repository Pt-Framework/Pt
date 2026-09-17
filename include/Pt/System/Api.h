/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
