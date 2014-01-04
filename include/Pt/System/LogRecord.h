/*
 * Copyright (C) 2005-2012 by Dr. Marc Boris Duerner
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

#ifndef Pt_System_LogRecord_h
#define Pt_System_LogRecord_h

#include <Pt/System/Api.h>
#include <Pt/System/LogLevel.h>
#include <Pt/SourceInfo.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <iostream>
#include <sstream>

namespace Pt {

namespace System {

/** @brief Log records can be added to a log.

    Log records represent the text entries that can be added to log. Each
    log record has a log level, which indicates it's severity. The text of
    a log record can be formatted with the stream operator, just like writing
    to std::cout. All stream output operators defined for std::ostream can
    be used including the manipulators.

    @code
    Pt::System::LogRecord record(Pt::System::Info);
    record << "pi is: " << 3.1415;
    @endcode

    Once a log record is created it can be added to a log with a logger. The
    record might be ignored if the log level is disabled for the logger's
    target. Unneccessary formatting can be avoided by checking if the record's
    log level is enabled for the logger's target.

    @code
    Pt::System::Logger logger("app.module");
    
    Pt::System::LogRecord record(Pt::System::Info);
    if( logger.enabled(record) )
    {
        record << "pi is: " << 3.1415;
        logger.log(record);
    }
    @endcode

    The same log record can be sent multiple times to a logger or to several
    loggers. This way formatting is only done once and logging performance
    can be increased.
 
    @code
    Pt::System::Logger logger1("app.module1");
    Pt::System::Logger logger2("app.module2");
    
    Pt::System::LogRecord record(Pt::System::Info);
    msg << "pi is: " << 3.1415;
 
    logger1.log(record);
    logger2.log(record);
    @endcode
 
    @ingroup Logging
*/
class LogRecord : protected Pt::NonCopyable
{
    public:
        //! @brief Construct a log record with a severity level.
        explicit LogRecord(const LogLevel& level)
        : _level(level)
        , _source("unknown", "unknown", "unknown")
        { }

        //!@ Destructor.
        ~LogRecord()
        {}

        //! @brief Clears all content of the record.
        void clear()
        {
            _text.str( std::string() );
            _text.clear();
        }

        //! @brief Returns the textual of the record.
        std::string text() const
        { return _text.str(); }

        //! @brief Returns the severity level.
        LogLevel logLevel() const
        { return _level; }

        //! @brief Returns the location in the source where the record was generated.
        const Pt::SourceInfo& sourceInfo() const
        { return _source; }

        //! @brief Appends @a value to the log record's text.
        template <typename T>
        LogRecord& operator<<(const T& value)
        {
            _text << value;
            return *this;
        }

        //! @brief Sets the source location of the log record.
        LogRecord& operator<<( const Pt::SourceInfo& si )
        {
            _source = si;
            return *this;
        }

        //! @brief Applies a manipulator when a log record text is built.
        LogRecord& operator<<(std::ios_base& (*pf)(std::ios_base&))
        {
            pf(_text);
            return *this;
        }

    private:
        std::ostringstream _text;
        LogLevel           _level;
        Pt::SourceInfo     _source;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_LogRecord_h
