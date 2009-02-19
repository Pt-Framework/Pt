/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Drner
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
#ifndef Pt_LogDevice_h
#define Pt_LogDevice_h

#include <Pt/Log/Api.h>
#include <Pt/Log/LogLevel.h>
#include <Pt/SourceInfo.h>
#include <Pt/DateTime.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <iostream>
#include <sstream>

namespace Pt {

namespace Log {

class Target;
class Message;
class Logger;

/// TODO:
/*
    - join Target into Logger
    - Hierachy of Targets
*/

/** @brief %Log message
    @ingroup Logging
*/
class PT_LOG_API Message : protected Pt::NonCopyable
{
    friend class Logger;

    public:
        Message(Logger& logger, const LogLevel level, const SourceInfo& source)
        : _logger(&logger)
        , _level(level)
        , _source(source)
        , _threadId(-1)
        , _procId(-1)
        {}

        Message(Logger& logger, const LogLevel level)
        : _logger(&logger)
        , _level(level)
        , _source("unknown", "unknown", "unknown")
        , _threadId(-1)
        , _procId(-1)
        {}

        ~Message()
        {}

        const std::string& target() const
        { return _target; }

        /** @brief Append a type as string to the message

            This method allows to append arbitrary types to the log-message.
            All types that have a stream output operator defined can be used
            here.
        */
        template <typename T>
        void append(const T& t)
        { _text << t; }

        std::string text() const
        { return _text.str(); }

        void setLogLevel(const LogLevel level)
        { _level = level; }

        LogLevel logLevel() const
        { return _level; }

        const DateTime& timestamp() const
        { return _dateTime; }

        void setTimestamp(const Pt::DateTime& dateTime)
        { _dateTime = dateTime; }

        const Pt::SourceInfo& sourceInfo() const
        { return _source; }

        void setSourceInfo(const SourceInfo& source)
        { _source = source; }

        long threadId() const
        { return _threadId; }

        void setThreadId(const long id)
        { _threadId = id; }

        long processId() const
        { return _procId; }

        void setProcessId(const long id)
        { _procId = id; }

        template <typename T>
        Message& operator<<(const T& value);

        Message& operator<<( LogLevel (*pf)() )
        {
            setLogLevel( pf() );
            return *this;
        }

        Message& operator<<( Message& (*pf)(Message&) )
        {
            return pf(*this);
        }

        inline Message& operator<<(std::ios_base& (*pf)(std::ios_base&))
        {
            pf(_text);
            return *this;
        }

        /** @brief Sends a log-message

            This method ends a log message and sends it to the logger target
            if the logger is enabled. Alternatively, the stream API can be used
            to end a log-message. If the logger is disabled this function only
            performs an integer comparison.
        */
        void send();

    protected:
        Message(const Message& other)
        : _logger(other._logger)
        , _level(other._level)
        , _source(other._source)
        {}

    private:
        Logger*            _logger;
        std::string        _target;
        std::ostringstream _text;
        LogLevel           _level;
        Pt::SourceInfo     _source;
        Pt::DateTime       _dateTime;
        long               _threadId;
        long               _procId;
};


/** @brief Manipulator to end a log-message
*/
inline Message& endlog(Message& msg)
{
    msg.send();
    return msg;
}


/** @brief Write log-messages to a target
    @ingroup Logging

    The Logger is the central class of the logging framework on the client
    side. It is used to write log-messages to a logging target maintained
    by the logging framework. A logger is created by passing a string that
    identifies the target uniquely to the constructor. If the target does not
    exist yet, it will be created. If several loggers are created with the
    same target string they will indeed use the same target. A logger should
    be kept within a single thread, but different loggers can log to the
    same target from  different threads. The creation of a logger requires
    a lookup in the logging manager, so it is beneficial to keep created logger
    at the class level for as-long as they are needed.
    Logging is most convenient using the stream API. The complete IOStreams
    API is supported, but a few manipulators should be avoided that would
    conflict with the typical format of a log-message. Some extra manipulators
    exist to set the state of the logger, most notably endlog or the manipulators
    to set the log-level. This is a typical example how a logger is used to
    produce a log-message:

    @code
    Pt::Logger logger(unique.loggerid);
    logger.beginLog(PT_SOURCEINFO) << Pt::Log::info << "Pi is exactly " << 3 <<
                                  << " No, I was kidding, its really " << 3.14
                                  << Pt::Log::endlog;
    @endcode

    If the logger is disabled, meaning its log-level is lower than the log-level
    of its target, each call to the logger will only cost an integer comparison.
*/
class PT_LOG_API Logger : protected Pt::NonCopyable
{
    friend class LogManager;

    public:
        /** @brief Constructs a new logger for a target and log-level

            The constructed logger will log to the target name with at an
            initial log-level level. If the target does not exist yet within
            the loggin framework it will be created and configured.
        */
        Logger(const std::string& name);

        /** @brief Destructor
        */
        ~Logger();

        /** @brief Returns the current log-level of the target
        */
        LogLevel logLevel() const;

        /** @brief Returns true if the log level is enabled
        */
        bool enabled(LogLevel level) const;

        /** @brief Returns the target of this logger

            The life-time of the target is bound to the life-time of the
            logger.
        */
        Target& target() const;

        /** @brief Start a new log-message

            This method begins a new log-message. Use the PT_SOURCEINFO
            macro to generate a SourceInfo object. This method call is
            usually followed by several calls of the output operator. If
            the logger is disabled this function only performs a integer
            comparison.
        */
		Message beginLog(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Trace, si);
		}

		Message beginLog(LogLevel level, const Pt::SourceInfo& si)
		{
			return Message(*this, level, si);
		}

		Message trace(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Trace, si);
		}

		Message debug(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Debug, si);
		}

		Message info(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Info, si);
		}

		Message warn(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Warn, si);
		}

		Message error(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Error, si);
		}

		Message fatal(const Pt::SourceInfo& si)
		{
			return Message(*this, Pt::Log::Fatal, si);
		}

		Message trace()
		{
			return Message(*this, Pt::Log::Trace);
		}

		Message debug()
		{
			return Message(*this, Pt::Log::Debug);
		}

		Message info()
		{
			return Message(*this, Pt::Log::Info);
		}

		Message warn()
		{
			return Message(*this, Pt::Log::Warn);
		}

		Message error()
		{
			return Message(*this, Pt::Log::Error);
		}

		Message fatal()
		{
			return Message(*this, Pt::Log::Fatal);
		}

		Message operator<<( LogLevel (*pf)() )
		{
			return Message( *this, pf() );
		}

    protected:
        //! @internal Used by the LogManager on initialisation
        Logger(Target& target);

    private:
        //! @internal
        Target* _target;

        //! @internal
        void* _reserved;
};


template <typename T>
Message& Message::operator<<(const T& value)
{
    if( _logger->enabled(_level) )
        _text << value;

    return *this;
}

/** @brief Sentry class to log a scope
    @ingroup Logging

    The constructor of this class send a log-message that a scope was entered,
    the destructor sends a log-message the the scope was left. This makes tracing
    functions with multiple return points or exceptions very easy.

    @code
    void complexfunction(bool a, bool b, bool c)
    {
        Pt::Log::LoggerScope scope(logger, Pt::Log;;Trace, PT_SOURCEINFO);

        if(a) return; // Destructor sends log-message

        mayThrowException(); // Destructor sends log-message

        if(b) return; // Destructor sends log-message

        if(c) return; // Destructor sends log-message
    }
    @endcode
*/
class LoggedScope
{
    public:
        /** @brief Constructor

            The constructor send a log-message with a given log-level that
            a scope was eneterd using the passed logger. Use the PT_SOURCEINFO
            macro to create a meaningful SourceInfo object.
        */
        LoggedScope(Logger& logger, LogLevel level, const Pt::SourceInfo& si)
        : _logger(logger)
        , _si(si)
        , _level(level)
        {
            _logger.beginLog(_level, _si) << "Enter " << _si.func() << endlog;
        }

        /** @brief Destructor

            The Destructor will send a log-message that a scope was left.
        */
        ~LoggedScope()
        {
            _logger.beginLog(_level, _si) << "Leave " << _si.func() << endlog;;
        }

    private:
        Logger& _logger;
        Pt::SourceInfo _si;
        LogLevel _level;
};

}

}

#endif
