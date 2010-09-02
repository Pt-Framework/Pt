/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#define PT_SYSTEM_VERSION_MINOR 0
#define PT_SYSTEM_VERSION_REVISION 0

#if defined(PT_SYSTEM_API_EXPORT)
#    define PT_SYSTEM_API PT_EXPORT
#  else
#    define PT_SYSTEM_API PT_IMPORT
#  endif

#endif

/** @defgroup Logging Logging
    @brief Configurable and thread-safe logging framework

    There are two ways to log, either by using static loggers or by
    using specific logger instances for a program scope. To use a 
    static logger it has to be defined first in a source file, and 
    then log macros can be used to send log messages via the static logger:
    
    @code
    #include <Pt/System/Logger.h>
    log_define("mylogger")
    
    int multiply(int a, int b)
    {
        log_info("multipy " << a << " by " << b)
        return a*b;
    }
    @endcode
    
    log_define and log_info are macros that expand to nothing if NLOG is
    defined. if NLOG is not defined, log_define expands to a static logger
    object.
    The alternative is to use specific loggers. To do so, a Logger 
    object must be instanciated, for example as a class member variable.
    
    @code
    class Calculator
    {
        public:
            Calculator();
            
            int multiply(int a, int b);

        private:
            Pt::System::Logger _logger;
    };
    @endcode
    
    Messages can then be written to the Logger using the appropriate 
    log macro.
    
    @code
    int Calculator::multiply(int a, int b)
    {
        log_message_info(_logger, "multipy " << a << " by " << b);
        return a*b;
    }
    @endcode
    
    As before log_message_info expands to nothing if NLOG is defined.
    It is also possible to not use any macros, but create message 
    objects and write them with logger instances.
    
    @code
    Pt::System::Logger logger("mylogger");
    Pt::System::LogMessage message(logger, Pt::System::Info);
    message << "hello world" << Pt::System::endlog;
    @endcode
    
    Now, if NLOG is defined somewhere, it will not affect this logger
    and the message is send.
    
    There are two ways how the logging framework can be initialized.
    Either use log_init at program start to load a settings file, or
    initialize logger targets using the logging API. To change the
    log level or channel of a target, one has to get a reference to
    the target first. Then the log level and channel can be changed.
    
    @code
    Pt::System::LogTarget& target = Pt::System::LogTarget::get("");
    target.setLogLevel(Pt::System::Info);
    target.setChannel("file://myfile.log");
    @endcode
    
    The code example above changes the log level and channel of the
    root logger target to write messages of a level of Pt::System::Info
    or higher to a specified log file.
*/

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
    class File;
    class FileDevice;
    class FileInfo;
    class IODevice;
    class IOStream;
    class IStream;
    class MainLoop;
    class Mutex;
    class OStream;
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
}

}
