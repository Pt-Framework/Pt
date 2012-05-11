/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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
#include <Pt/SourceInfo.h>
#include <Pt/DateTime.h>
#include <Pt/NonCopyable.h>
#include <Pt/Atomicity.h>
#include <string>
#include <iostream>
#include <sstream>

namespace Pt {

class Settings;

namespace System {

class Logger;
class LogChannel;

/*
    TODO:
    - use formatting optimizations (cxxtools log)
*/

/** @brief %Log message. 
 
    Log message caching can be used for faster logging.
 
    @code
    Pt::System::LogMessage msg(mylogger, Pt::System::Info);
    msg << "pi is: " << 3.1415;
 
    // send later...
    msg.send();
 
    // send again even later...
    msg.send();
    @endcode
 
    @ingroup Logging
*/
class PT_SYSTEM_API LogMessage : protected Pt::NonCopyable
{
    friend class Logger;

    public:
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

        //! @brief Use the PT_SOURCEINFO macro to generate a SourceInfo object.
        LogMessage& operator<<( const Pt::SourceInfo& si )
        {
            setSourceInfo(si);
            return *this;
        }

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


/** @brief Target of log-messages.

    All created targets form a hierachy within the logging manager.
    To add an instance to this hierachy use the static Target::get
    method. The naming scheme of the targets follows the property
    dot-syntax. If there is a target foobar.ping and a target foobar.pong
    it means that ping and pong are children of the foobar target.
    Once a target is created a channel can be assigned to it. If no channel
    is assigned to a target, it will use the channel of the next of its parent
    targets. Channels can either be assigned by the class API or in
    the properties file that the logging-manager reads on startup.
    Here is an example of how ping and pong would be configured:

    @code
    foobar.channel = console://

    foobar.ping.channel = comm:///dev/ttyS0
    foobar.ping.logLevel = Error

    foobar.pong.logLevel = Trace
    @endcode

    In the example, pong would write messages with a log-level of Trace
    or higher to the console channel it inherited from its parent. The
    target ping writes messages with a log-level of Error of higher
    asnychronously to the serial port.

    @ingroup Logging
*/
class PT_SYSTEM_API LogTarget : protected Pt::NonCopyable
{
    friend class LogManager;

    protected:
        //! @internal Used within logging-manager
        LogTarget(const std::string& name, int concurrency, LogTarget* parent = 0);

    public:
        //! @brief Destructor.
        virtual ~LogTarget();

        /** @brief Returns the name of the target.
        */
        const std::string& name() const;

        //! @brief Returns the log-level of the target.
        int logLevel() const
        {
            return atomicGet(_loglevel);
        }

        /** @brief Sets the log-level of the target and its children.

            This method is thread-safe. The log-level can also be set
            in the settings file of the used for initialization. All
            children of this target inherit the given LogLevel unless
            they are already set to a log level explicitly i.e. this
            method has een called on a child before.
        */
        void setLogLevel(LogLevel level);

        /** @brief Returns the URL of the channel used by the target
        */
        std::string channelUrl() const;

        /** @brief Sets the channel to be used by this target

            Throws a invalid_argument exception if the channel can not
            be created. This function might block until the channel could
            be opened. This method is thread-safe. The channel can also be
            set in the properties file of the logging-manager.
        */
        void setChannel(const std::string& url);

        //! @brief Write log message to this target
        void log(const LogMessage& msg);

        /** @brief Initialize logging targets with a settings file

            The given settings file is parsed and all listed targets are
            created and initialized. If a target exists already, it is
            reinitialized.

            @param file Path to a settings file
        */
        static void initTargets(const std::string& file);

        static void initTargets()
        { initTargets("log.properties"); }

        /** @brief Initialize logging targets with a settings

            All targets listed in the given settings are created and
            initialized. If a target exists already, it is reinitialized.

            @param settings Settings to apply to target list
        */

        static void initTargets(const Settings& settings);

        /** @brief Get a target from the logging manager

            The target is created if it does not exist, otherwise the
            existing target is returned. If the target is created it is
            initialised with the properties from the configuration file
            of the loggin manager. This method is thread-safe.
        */
        static LogTarget& get(const std::string& name);

    protected:
        //! @internal
        LogTarget* parent() const
        { return _parent; }

        //! @internal
        bool inheritsLogLevel() const;

        //! @internal
        void assignLogLevel(int level, bool inherited);

        //! @internal
        bool inheritsChannel() const;

        //! @internal
        LogChannel* channel() const
        { return _channel; }

        //! @internal
        void removeChannel();

        //! @internal
        void assignChannel(LogChannel& ch);

    private:
        //! @internal
        LogTarget* _parent;

        //! @internal
        std::string _name;

        //! @internal
        mutable volatile atomic_t _loglevel;

        //! @internal
        bool _inheritLogLevel;

        //! @internal
        LogChannel* _channel;

        //! @internal
        bool _inheritChannel;

        //! @internal
        void* _reserved;
};


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
    logger.beginLog() << Pt::System::info << PT_SOURCEINFO)
                      << "Pi is exactly " << 3
                      << " No, I was kidding, its really " << 3.1415
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
        ~Logger();

        /** @brief Get a target from the logging manager

            The target is created if it does not exist, otherwise the
            existing target is returned. If the target is created it is
            initialised with the properties from the configuration file
            of the loggin manager. This method is thread-safe.
        */
        static LogTarget& getTarget(const std::string& name)
        { return LogTarget::get(name); }

        /** @brief Sets the log-level of the target and its children.

            This method is thread-safe. The log-level can also be set
            in the settings file of the used for initialization. All
            children of this target inherit the given LogLevel unless
            they are already set to a log level explicitly i.e. this
            method has een called on a child before.
        */
        static void setLogLevel(const std::string& target, LogLevel level)
        { LogTarget::get(target).setLogLevel(level); }

        /** @brief Returns true if the log level is enabled for the target
        */
        bool enabled(LogLevel level) const
        {
            return level <= _target->logLevel();
        }

        void log(const LogMessage& msg)
        {
            if( this->enabled( msg.logLevel() ) )
            {
                _target->log( msg );
            }
        }

        /** @brief Start a new log-message

            This method begins a new log-message. This method call is usually
            followed by several calls of the output operator. If the logger is
            disabled this function only performs a integer comparison.
        */
        LogMessage beginLog()
        {
            return LogMessage(*this, Pt::System::Trace);
        }

        LogMessage beginLog(LogLevel level)
        {
            return LogMessage(*this, level);
        }

        LogMessage operator<<( LogLevel (*pf)() )
        {
            return LogMessage( *this, pf() );
        }

        //! @internal Only for unit-tests
        LogTarget& target() const
        { return *_target; }

    protected:
        //! @internal
        Logger(LogTarget& target)
        : _target( &target )
        {}

    private:
        LogTarget& init(const std::string& name);

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

/** @internal
 */
struct LogStmt
{
    LogStmt(Logger& logger, Pt::System::LogLevel level)
    : _msg( logger, level )
    {}

    ~LogStmt()
    { _msg.send(); }

    LogMessage& msg()
    { return _msg; }

    LogMessage _msg;
};

}

}

#ifdef NLOG
    #define log_init(file)
    #define log_define(category)
    #define log_impl(level, expr)
    #define logger_log_impl(logger, level, message)
#else
    #define log_init(file) \
    Pt::System::LogTarget::initTargets(file);

    #define log_define(category)                                         \
    static Pt::System::Logger& getStaticLogger()                         \
    {                                                                    \
        static Pt::System::Logger pt_logger(category);                   \
        return pt_logger;                                                \
    }                                                                    \
    static Pt::System::Logger& pt_static_logger_init = getStaticLogger();

    #define log_impl(level, expr) \
       ( getStaticLogger().enabled(Pt::System::level) && &(getStaticLogger().beginLog(Pt::System::level) << expr << Pt::System::endlog) )

    #define logger_log_impl(logger, level, expr) \
       ( logger.enabled( Pt::System::level() ) && &(logger.beginLog( Pt::System::level() ) << expr << Pt::System::endlog) )

#endif

#define log_fatal(expr) log_impl(Fatal, expr)
#define log_error(expr) log_impl(Error, expr)
#define log_warn(expr)  log_impl(Warn, expr)
#define log_info(expr)  log_impl(Info, expr)
#define log_debug(expr) log_impl(Debug, expr)
#define log_trace(expr) log_impl(Trace, expr)

#define logger_log_fatal(logger, expr) logger_log_impl(logger, fatal, expr)
#define logger_log_error(logger, expr) logger_log_impl(logger, error, expr)
#define logger_log_warn(logger, expr)  logger_log_impl(logger, warn, expr)
#define logger_log_info(logger, expr)  logger_log_impl(logger, info, expr)
#define logger_log_debug(logger, expr) logger_log_impl(logger, debug, expr)
#define logger_log_trace(logger, expr) logger_log_impl(logger, trace, expr)

#define logger_begin_impl(logger, level)   \
    if( ! logger.enabled(Pt::System::level) ) ; \
    else Pt::System::LogStmt(logger, Pt::System::level).msg()

#define logger_begin_fatal(logger) logger_begin_impl(logger, Fatal)
#define logger_begin_error(logger) logger_begin_impl(logger, Error)
#define logger_begin_warn(logger)  logger_begin_impl(logger, Warn)
#define logger_begin_info(logger)  logger_begin_impl(logger, Info)
#define logger_begin_debug(logger) logger_begin_impl(logger, Debug)
#define logger_begin_trace(logger) logger_begin_impl(logger, Trace)

#endif
