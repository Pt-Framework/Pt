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

#ifndef PT_SYSTEM_API_LOGGING_H
#define PT_SYSTEM_API_LOGGING_H

/** @defgroup Logging Logging

    @brief Efficient multi-threaded logging with configurable output channels.

    The logging framework offers an efficient, extensible system to
    log messages from programs with multiple threads to a number of
    channels. Logging can be completely disabled at compile time, when the
    logging macros are used. At runtime, log messages are filtered by
    a level of severity. Filtering is very efficient, because log messages
    are not even built if their log level is too low.
    Currently three types of output channels exist, logging to files
    with file rolling, to the console and to the serial port. The logging
    framework can be extended by new channels.

    Logging is an important feature of many applications. It can be used for
    debugging during the development process and to trace how a program executes
    once it is deployed. Crucial features of a logging framework are:

    - \b Performance \n
    High  peformance can only be achieved if formatting of log records is avoided
    for disabled log statements. The costs boil down to an atomic integer
    comparison, and a branch. Further, logging can be disabled at compile time
    with via the preprocessor, in which case no instruction at all will be
    generated.\n\n

    - \b Thread-safety \n
    It is obvious that any modern library needs to be thread-safe. However, 
    great care needs to be taken that synchronization overhead does not affect
    performance.\n\n

    - \b Easy \b Formatting \n
    In order to minimize the impact of a logging framework on the size of the 
    source code, logging statements should be easy to write. This library allows
    to format log records with stream output operator just like for std::ostream.\n\n

    - \b Configuration \n
    The logging system should be configurable by both, API calls and configuration
    files. It must provide fine grained control in which parts of the application
    logging is enabled. It must be possible route the log records to different 
    logs, for example the console or log files. The format of the records in the
    log should be configurable, and only contain the required information,
    which leads to smaller logs and a higher performance.\n\n

    - \b Usable \b during \b static \b initialization \b and \b deinitialization \n
    For example, logging statements may be executed indirectly when global objects
    are constructed or destructed. This should result in a static initialization
    fiasco.\n\n

    - \b Portability \n
    This library is extremly portable and has minimal dependencies. It requires 
    only standard C++ and a standard C++ library.

    The heart of the logging framwork is a hierarchy of log targets, which have
    a unique string ID. Applications format log records and use logger objects to
    write them to a target. Each target is configured with a threshold log level. 
    Only records that are equally or more severe than the threshold are logged. In
    this case, the log records are written to the targets channel. Targets can log
    to the same channel, in fact, often all targets log to the same channel. The 
    log channels perform output of log records i.e. to the console, if a console 
    channel is selected or to a file if a file channel is selected, respectively.
    The threshold log level and channel of each log target in the hierarchy can
    be configured at runtime. Here is a typical example:

    @verbatim
                           <root> id: ""
                             |    channel: console://
                             |    level: info
                             |
                             |
                           <app> id: "app"
                             |   channel: INHERIT
                             |   level: INHERIT
                             |
                             |
        .--------------------+-------------------.                
        |                                        |      
        |                                        |
    <module1> id: "app.module1"              <module2> id: "app.module2"
              channel: file:///log.txt                 channel: INHERIT
              level: trace                             level: INHERIT

    @endverbatim

    In this setup, three log targets are created. The root target is always persent
    and has the special empty string ID. The root target has one direct child, the
    target named "app". In this case, no log channel and level have been explicitly
    set for the target, so it inherits the attributes from its parent. The target
    "app" has two children, named "app.module1" and "app.module2". The string IDs
    indicate the position (path) of the target in the hierarchy. The target 
    "app.module2" inherits all attributes from it parent, while the target 
    "app.module1" overrides the log channel and level. With this mechanism based on
    inheritance, the parts of interest of the hierarchy can be enabled, while other
    parts of the hierarchy are suppressed.

    Applications can use the API to set the threshold log levels and the channels
    of the targets. Possible log levels are:

    - Pt::System::Fatal
    - Pt::System::Error
    - Pt::System::Warn
    - Pt::System::Info
    - Pt::System::Debug
    - Pt::System::Trace

    Channels are configured by a channel URL. Possible channel URLs are:

    - file:///mylog.log?size=1000000&files=5
    - console://

    The first opens a file channel writing to the file mylog.log. If the file
    size of 1000000 bytes is reached it will be renamed, and logging continues
    to a newly created mylog.log file. The parameter 'files' limits file rolling
    to a total number of five files. The second URL opens a channel to log to the
    console.

    The format of the logging records can be configured with a format pattern
    string. The format pattern can contain text and specifiers, which are 
    placeholders for the various elements of the log records. Specifiers are
    escaped with a percent sign in the format pattern string. For example, the
    pattern "%t %m" would write the time and the message for each log record
    separated by a space.

    Here is a list of possible specifiers:
    - \%c logging category
    - \%d current date
    - \%l log level (severity)
    - \%m message text
    - \%t current time
    - \%F file where the record was logged
    - \%L line number where the record was logged
    - \%M method/function where the record was logged

    \b TODO:
    - \%T thread id
    - \%P process id

    The following code example above changes the log level and channel of the
    target named "app" to write records with a threshold level of Pt::System::Info
    to a log file:

    @code
    Pt::System::Logger::setLogLevel("app", Pt::System::Info);
    Pt::System::Logger::setChannel("app", "file:///myfile.log");
    Pt::System::Logger::setPattern("[%c] %t - %m");
    @endcode

    A log record pattern is applied to print the target ID, the time and the
    message text of the records to the logs.
*/

#endif
