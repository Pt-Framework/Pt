/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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

#ifndef Pt_SystemDevice_h
#define Pt_SystemDevice_h

#include <Pt/System/Api.h>
#include <Pt/System/LogLevel.h>
#include <Pt/System/LogTarget.h>
#include <Pt/SourceInfo.h>
#include <Pt/DateTime.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <iostream>
#include <sstream>

class LoggerTest;

namespace Pt {

namespace System {

class Target;
class Logger;

/*
    TODO:
    - use formatting optimizations (cxxtools log)
*/

/** @brief %Log message.
    @ingroup Logging
*/
class PT_SYSTEM_API LogMessage : protected Pt::NonCopyable
{
    friend class Logger;

    public:
        LogMessage(Logger& logger, const LogLevel level, const SourceInfo& source)
        : _logger(&logger)
        , _level(level)
        , _source(source)
        {}

        LogMessage(Logger& logger, const LogLevel level)
        : _logger(&logger)
        , _level(level)
        , _source("unknown", "unknown", "unknown")
        {}

        ~LogMessage()
        {}

        void clear()
        {
            _text.str( std::string() );
            _text.clear();
        }

        std::string text() const
        { return _text.str(); }

        void setLogLevel(const LogLevel level)
        { _level = level; }

        LogLevel logLevel() const
        { return _level; }

        const Pt::SourceInfo& sourceInfo() const
        { return _source; }

        void setSourceInfo(const SourceInfo& source)
        { _source = source; }

        template <typename T>
        LogMessage& operator<<(const T& value);

        LogMessage& operator<<( LogLevel (*pf)() )
        {
            setLogLevel( pf() );
            return *this;
        }

        LogMessage& operator<<( LogMessage& (*pf)(LogMessage&) )
        {
            return pf(*this);
        }

        inline LogMessage& operator<<(std::ios_base& (*pf)(std::ios_base&))
        {
            pf(_text);
            return *this;
        }

        /** @brief Sends a log message

            This method ends a log message and sends it to the logger target
            if the logger is enabled. Alternatively, the stream API can be used
            to end a log-message. If the logger is disabled this function only
            performs an integer comparison.
        */
        void send();

    protected:
        LogMessage(const LogMessage& other)
        : _logger(other._logger)
        , _level(other._level)
        , _source(other._source)
        {}

    private:
        Logger*            _logger;
        std::ostringstream _text;
        LogLevel           _level;
        Pt::SourceInfo     _source;
};


/** @brief Manipulator to end a log-message
*/
inline LogMessage& endlog(LogMessage& msg)
{
    msg.send();
    return msg;
}

/** @brief Write log-messages to a target.

    The Logger is the central class of the logging framework on the client
    side. It is used to write log-messages to a logging target maintained
    by the logging framework. A logger is created by passing a string that
    identifies the target uniquely to the constructor. If the target does not
    exist yet, it will be created. If several loggers are created with the
    same target string they will indeed use the same target. The creation of
    a logger requires a lookup in the logging manager, so it is beneficial to
    keep created loggers at the class level for as-long as they are needed.
    Logging is most convenient using the stream API. The complete IOStreams
    API is supported, but a few manipulators should be avoided that would
    conflict with the typical format of a log-message. Some extra manipulators
    exist to set the state of the logger, most notably endlog or the manipulators
    to set the log-level. This is a typical example how a logger is used to
    produce a log-message:

    @code
    Pt::System::Logger logger("mylog");
    logger.beginLog(Pt::System::Info, PT_SOURCEINFO) << "Pi is exactly " << 3 <<
                                  << " No, I was kidding, its really " << 3.14
                                  << Pt::System::endlog;
    @endcode

    If the logger is disabled, meaning that the log-level of its target is
    lower than the log-level of its target, the log message is discarded.
    To avoid this cost, either check wheter the logger is enabled at the
    desired level by calling Logger::enabled, or use one of the logging
    macros instead.

    @ingroup Logging
*/
class PT_SYSTEM_API Logger : protected Pt::NonCopyable
{
    friend class LogManager;
    friend class LoggerTest;

    public:
        /** @brief Constructs a new logger for a target and log-level

            The constructed logger will log to the target with the given name.
            If the target does not exist yet within the loggin framework it
            will be created.
        */
        Logger(const std::string& name);

        /** @brief Constructs a new logger for a target and log-level

            The constructed logger will log to the target with the given name.
            If the target does not exist yet within the loggin framework it
            will be created.
        */
        Logger(const char* name);

        /** @brief Destructor
        */
        ~Logger()
        {}

        /** @brief Returns true if the log level is enabled for the target
        */
        bool enabled(LogLevel level) const
        { return level <= _target->logLevel(); }

        void log(const LogMessage& msg)
        {
            if( this->enabled( msg.logLevel() ) )
            {
                _target->log( msg );
            }
        }
        /** @brief Start a new log-message

            This method begins a new log-message. Use the PT_SOURCEINFO
            macro to generate a SourceInfo object. This method call is
            usually followed by several calls of the output operator. If
            the logger is disabled this function only performs a integer
            comparison.
        */
        LogMessage beginLog(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Trace, si);
        }

        LogMessage beginLog(LogLevel level, const Pt::SourceInfo& si)
        {
            return LogMessage(*this, level, si);
        }

        LogMessage trace(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Trace, si);
        }

        LogMessage debug(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Debug, si);
        }

        LogMessage info(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Info, si);
        }

        LogMessage warn(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Warn, si);
        }

        LogMessage error(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Error, si);
        }

        LogMessage fatal(const Pt::SourceInfo& si)
        {
            return LogMessage(*this, Pt::System::Fatal, si);
        }

        LogMessage trace()
        {
            return LogMessage(*this, Pt::System::Trace);
        }

        LogMessage debug()
        {
            return LogMessage(*this, Pt::System::Debug);
        }

        LogMessage info()
        {
            return LogMessage(*this, Pt::System::Info);
        }

        LogMessage warn()
        {
            return LogMessage(*this, Pt::System::Warn);
        }

        LogMessage error()
        {
            return LogMessage(*this, Pt::System::Error);
        }

        LogMessage fatal()
        {
            return LogMessage(*this, Pt::System::Fatal);
        }

        LogMessage operator<<( LogLevel (*pf)() )
        {
            return LogMessage( *this, pf() );
        }

    protected:
        //! @internal Used by the LogManager on initialisation
        Logger(LogTarget& target)
        : _target( &target )
        {}

        /** @brief Returns the target of this logger
        */
        LogTarget& target() const
        { return *_target; }

    private:
        //! @internal
        LogTarget* _target;
};


template <typename T>
inline LogMessage& LogMessage::operator<<(const T& value)
{
    if( _logger->enabled(_level) )
        _text << value;

    return *this;
}


inline void LogMessage::send()
{
    _logger->log(*this);
}


/** @brief Sentry class to log a scope.

    The constructor of this class send a log-message that a scope was entered,
    the destructor sends a log-message the the scope was left. This makes tracing
    functions with multiple return points or exceptions very easy.

    @code
    void complexfunction(bool a, bool b, bool c)
    {
        Pt::System::LoggerScope scope(logger, Pt::System;;Trace, PT_SOURCEINFO);

        if(a) return; // Destructor sends log-message

        mayThrowException(); // Destructor sends log-message

        if(b) return; // Destructor sends log-message

        if(c) return; // Destructor sends log-message
    }
    @endcode

    @ingroup Logging
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

#ifdef NLOG
    #define log_message_impl(logger, level, message)

	#define log_init(file)
    #define log_define(category)
    #define log_xxxx(level, expr)
#else
    #define log_message_impl(logger, level, expr) \
    if( logger.enabled( Pt::System::level() ) ) \
    { \
        logger.beginLog( Pt::System::level(), PT_SOURCEINFO) << expr << Pt::System::endlog; \
    }

    #define log_init(file) \
    Pt::System::LogTarget::init(file);
    
    #define log_define(category) \
    static Pt::System::Logger pt_logger(category);

    #define log_xxxx(level, expr)   \
    do { \
        if( pt_logger.enabled(Pt::System::level) ) \
        { \
            pt_logger.beginLog(Pt::System::level, PT_SOURCEINFO) << expr << Pt::System::endlog; \
        } \
    } while (false)
#endif

// deprecated
#define PT_LOG_FATAL(logger, expr) log_message_impl(logger, fatal, expr)
#define PT_LOG_ERROR(logger, expr) log_message_impl(logger, error, expr)
#define PT_LOG_WARN(logger, expr)  log_message_impl(logger, warn, expr)
#define PT_LOG_INFO(logger, expr)  log_message_impl(logger, info, expr)
#define PT_LOG_DEBUG(logger, expr) log_message_impl(logger, debug, expr)
#define PT_LOG_TRACE(logger, expr) log_message_impl(logger, trace, expr)

#define log_fatal(expr) log_xxxx(Fatal, expr)
#define log_error(expr) log_xxxx(Error, expr)
#define log_warn(expr)  log_xxxx(Warn, expr)
#define log_info(expr)  log_xxxx(Info, expr)
#define log_debug(expr) log_xxxx(Debug, expr)
#define log_trace(expr) log_xxxx(Trace, expr)

#define log_message_fatal(logger, expr) PT_LOG(logger, fatal, expr)
#define log_message_error(logger, expr) PT_LOG(logger, error, expr)
#define log_message_warn(logger, expr)  PT_LOG(logger, warn, expr)
#define log_message_info(logger, expr)  PT_LOG(logger, info, expr)
#define log_message_denug(logger, expr) PT_LOG(logger, debug, expr)
#define log_message_trace(logger, expr) PT_LOG(logger, trace, expr)

#endif
