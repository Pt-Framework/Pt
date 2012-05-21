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

#ifndef Pt_System_Logger_h
#define Pt_System_Logger_h

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

/** @brief Log records.

    Log record caching can be used for faster logging or to write the same
    record to multiple loggers.
 
    @code
    Pt::System::LogRecord record(Pt::System::Info);
    msg << "pi is: " << 3.1415;
 
    // log to logger1
    logger1->log(record);
 
    // same record goes to logger2
    logger2->log(record);
    @endcode
 
    @ingroup Logging
*/
class LogRecord : protected Pt::NonCopyable
{
    public:
        explicit LogRecord(const LogLevel& level)
        : _level(level)
        , _source("unknown", "unknown", "unknown")
        { }

        ~LogRecord()
        {}

        void clear()
        {
            _text.str( std::string() );
            _text.clear();
        }

        std::string text() const
        { return _text.str(); }

        LogLevel logLevel() const
        { return _level; }

        const Pt::SourceInfo& sourceInfo() const
        { return _source; }

        template <typename T>
        LogRecord& operator<<(const T& value)
        {
            _text << value;
            return *this;
        }

        //! @brief Use the PT_SOURCEINFO macro to generate a SourceInfo object.
        LogRecord& operator<<( const Pt::SourceInfo& si )
        {
            _source = si;
            return *this;
        }

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
        void log(const LogRecord& record);

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
    logger.beginLog() << Pt::System::info << PT_SOURCEINFO
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

        /** @brief Initialize logging targets with a settings file

            The given settings file is parsed and all listed targets are
            created and initialized. If a target exists already, it is
            reinitialized.

            @param file Path to a settings file
        */
        static void initTargets(const std::string& file);

        //! @internal
        static void initTargets()
        { initTargets("log.properties"); }

        /** @brief Initialize logging targets with a settings

            All targets listed in the given settings are created and
            initialized. If a target exists already, it is reinitialized.

            @param settings Settings to apply to target list
        */
        static void initTargets(const Settings& settings);

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

        /** @brief Returns true if the log level is enabled for the target
        */
        bool enabled(const LogRecord& record) const
        {
            return record.logLevel() <= _target->logLevel();
        }

        /** @brief Write a log record to the target.
          
            The log record @a record will be written to the target of this
            logger if the records log level allows it, or if @a enabled is
            set to true.
        */
        void log(const LogRecord& record, bool enabled = false)
        {
            if( enabled || this->enabled( record.logLevel() ) )
            {
                _target->log( record );
            }
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

/** @brief Log message. 
 
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
class LogMessage : protected Pt::NonCopyable
{
    public:
        /** @brief Constructs a log message for a logger

            Contructs a log message, which uses @a logger to log records of
            the log level specified by @a level. 
            If @a enabled is true, the message is send to the target without 
            checking the log level. It is assumed that the caller has already
            checked the log level of the underlying logger and set this flag
            accordingly. This flag only persists until the message has been 
            sent.
        */
        LogMessage(Logger& logger, const LogLevel& level, bool enabled = false)
        : _record(level)
        , _logger(&logger)
        , _enabled(enabled)
        { }

        //! @brief Destructor.
        ~LogMessage()
        {}
        
        //! @brief Returns the underlying log record.
        const LogRecord& record() const
        { return _record; }

        //! @brief Sends the message's log record to the logger.
        void send()
        { 
            _logger->log( _record, _enabled ); 
            _enabled = false;
        }

        //! @brief Returns true if the record's log level is enabled for the target.
        bool enabled() const
        { return _logger->enabled( _record.logLevel() ); }

        //! @brief Returns true if the record's log level is enabled for the target.
        operator bool() const
        { return enabled(); }

        //! @brief Returns true if the record's log level is disabled for the target.
        bool operator!() const
        { return ! enabled(); }

        //! @brief Appends @a value to the log record.
        template <typename T>
        LogMessage& operator<<(const T& value)
        {
            _record << value;
            return *this;
        }

        //! @brief Applies a manipulator to the log message.
        LogMessage& operator<<( LogMessage& (*pf)(LogMessage&) )
        {
            return pf(*this);
        }

    private:
        //! @internal
        LogRecord _record;
        
        //! @internal
        Logger* _logger;
        
        //! @internal
        bool _enabled;
};

/** @brief Manipulator to end and send a log-message
*/
inline LogMessage& endlog(LogMessage& msg)
{
    msg.send();
    return msg;
}

} // namespace System

} // namespace Pt

//! @internal @brief Log to a logger if the log level permits it.
#define logger_begin_impl(logger, level)            \
    if( ! logger.enabled( Pt::System::level ) )   \
        ;                                           \
    else Pt::System::LogMessage(logger, Pt::System::level, true)

#define logger_begin_fatal(logger) logger_begin_impl(logger, Fatal)
#define logger_begin_error(logger) logger_begin_impl(logger, Error)
#define logger_begin_warn(logger) logger_begin_impl(logger, Warn)
#define logger_begin_info(logger) logger_begin_impl(logger, Info)
#define logger_begin_debug(logger) logger_begin_impl(logger, Debug)
#define logger_begin_trace(logger) logger_begin_impl(logger, Trace)

#ifdef NLOG
    #define log_init(file)
    #define log_define_impl(instance, category)
    #define log_to_impl(instance, level, message)
    #define logger_log_impl(logger, level, expr)
#else
    //! @brief Initialize the logging library.
    #define log_init(file) Pt::System::LogTarget::initTargets(file);

    //! @internal @brief Define a named global logger instance.
    #define log_define_impl(instance, category)                   \
    inline static Pt::System::Logger& instance()                  \
    {                                                             \
        static Pt::System::Logger instance##_instance(category);  \
        return instance##_instance;                               \
    }                                                             \
    static Pt::System::Logger& instance##_static_init = instance();

    //! @internal @brief Log to a named global logger instance.
    #define log_to_impl(instance, level, expr) logger_log_impl(instance(), level, expr)

    //! @internal @brief Log to a logger instance.
    #define logger_log_impl(logger, level, expr) logger_begin_impl(logger, level) << expr << Pt::System::endlog
#endif

#define log_define(category) log_define_impl(static_logger, category)
#define log_fatal(expr) log_to_impl(static_logger, Fatal, expr)
#define log_error(expr) log_to_impl(static_logger, Error, expr)
#define log_warn(expr)  log_to_impl(static_logger, Warn, expr)
#define log_info(expr)  log_to_impl(static_logger, Info, expr)
#define log_debug(expr) log_to_impl(static_logger, Debug, expr)
#define log_trace(expr) log_to_impl(static_logger, Trace, expr)

#define log_define_instance(instance, category) log_define_impl(instance, category)
#define log_fatal_to(instance, expr) log_to_impl(instance, Fatal, expr)
#define log_error_to(instance, expr) log_to_impl(instance, Error, expr)
#define log_warn_to(instance, expr)  log_to_impl(instance, Warn, expr)
#define log_info_to(instance, expr)  log_to_impl(instance, Info, expr)
#define log_debug_to(instance, expr) log_to_impl(instance, Debug, expr)
#define log_trace_to(instance, expr) log_to_impl(instance, Trace, expr)

#define logger_log_fatal(logger, expr) logger_log_impl(logger, Fatal, expr)
#define logger_log_error(logger, expr) logger_log_impl(logger, Error, expr)
#define logger_log_warn(logger, expr)  logger_log_impl(logger, Warn, expr)
#define logger_log_info(logger, expr)  logger_log_impl(logger, Info, expr)
#define logger_log_debug(logger, expr) logger_log_impl(logger, Debug, expr)
#define logger_log_trace(logger, expr) logger_log_impl(logger, Trace, expr)

#endif
