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

#define PT_SYSTEM_VERSION_MAJOR 1
#define PT_SYSTEM_VERSION_MINOR 3
#define PT_SYSTEM_VERSION_REVISION 0

#if defined(PT_SYSTEM_API_EXPORT)
#    define PT_SYSTEM_API PT_EXPORT
#  else
#    define PT_SYSTEM_API PT_IMPORT
#  endif

#endif

/** @defgroup Logging Logging

    The logging framework offers an efficient, extensible system to
    log messages from programs with multiple threads to a number of
    channels. Logging can be completely disabled at compile time, when the
    logging macros are used. At runtime, log messages are filtered by
    a level of severity. Filtering is very efficient, because log messages
    are not even built if their log level is too low.
    Currently three types of output channels exist, logging to files
    with file rolling, to the console and to the serial port. The logging
    framework can be extended by new channels.
*/

/** @defgroup FileSystem File System Access
    
    The systems programming library (Pt::System) provides functionality to
    identify, create, rename, move or remove files and directories. An iterator
    based API can be used to traverse through the entries in a directory. It
    can be used with the iterator based algorithms in the C++ standard library.
    The FileDevice is an implementation of the IODevice to read and write
    files. 
*/

/** @defgroup Plugins Dynamic Libraries and Plugins
    
    Dynamic libraries can be loaded at runtime and symbols can be resolved
    using the Pt::System::Library class. Additionally, the plugin API provides
    a more advanced way of creating classes, that implement a common interface,
    from a dynamically loadable library or module. The mechanism is
    non-intrusive and can be used with an existing class hierachy. Plugins can
    be loaded and unloded by client application code. The concrete type of the
    created class is opaque to the application that uses the plugin, it only
    needs to know the interface.
*/

namespace Pt {

/** @namespace Pt::System
    @brief %System programming

    This module offers support for multithreaded programming, API's for
    file system operations such as traversing through directories and files,
    creating and handling of subprocesses, synchronous or asynchronous IO,
    logging, shared libraries and plugins. It introduces event loops as the
    central software interrupt to handle events across thread boundaries,
    timers and reactive I/O.
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
