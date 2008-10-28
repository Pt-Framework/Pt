/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_SYSTEM_API_H
#define PT_SYSTEM_API_H

#include <Pt/Api.h>

#define PT_SYSTEM_VERSION_MAJOR 1
#define PT_SYSTEM_VERSION_MINOR 0
#define PT_SYSTEM_VERSION_REVISION 0

#if defined(PT_SYSTEM_API_EXPORT)
#    define PT_SYSTEM_API PT_EXPORT
#  else
#    define PT_SYSTEM_API PT_IMPORT
#  endif

#endif

namespace Pt {

/** @namespace Pt::System
    @brief %System programming

    This module offers support for multithreaded programming, API's for
    file system operations such as traversing through directories and files,
    creating and handling of subprocesses transparent, synchronous or
    asynchronous IO, and shared libraries.
*/
namespace System {

    class Application;
    class Clock;
    class Condition;
    class Directory;
    class EventSource;
    class EventSink;
    class EventLoop;
    class EventLoopBase;
    class File;
    class FileDevice;
    class FileInfo;
    class IODevice;
    class IOStream;
    class IStream;
    class Mutex;
    class OStream;
    class Pipe;
    class PluginId;
    class Process;
    class ReadWriteMutex;
    class Selectable;
    class Selector;
    class SelectorBase;
    class Semaphore;
    class SerialDevice;
    class SharedLib;
    class StreamBuffer;
    class SystemError;
    class SpinLock;
    class Thread;
    class Timer;
    class Url;
}

}
