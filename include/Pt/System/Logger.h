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
#include <Pt/System/LogTarget.h>
#include <Pt/System/LogRecord.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

class Settings;

namespace System {

/** @brief Writes log records to a target.

    The Logger is the central class of the logging framework on the client
    side. It is used to write log records to a logging target maintained
    by the logging framework. A logger is created with a category string that
    identifies it's target to log to. The category string is in dot syntax, so
    the category string "app.module" refers to the target named "module", which
    is a child of the target named "app", which is a child of the root target.
    See LogTarget for more information.

    If the target of a logger does not exist yet, it will be created. If several
    loggers are created with the same target string they will indeed use the
    same target. The creation of a logger requires a target lookup in the logging 
    manager, so it is beneficial to keep created loggers at the class level 
    for as long as they are needed. The logger provides a set of static methods
    to configure targets. The following code configures a log level and a channel
    for a target named "app.module":

    @code
    Pt::System::Logger::setLogLevel("app.module", Pt::System::Info);
    Pt::System::Logger::setChannel("app.module", "console://");
    @endcode

    Channels and log levels can either be assigned by the API or in the 
    settings file that is loaded by calling Pt::System::Logger::init.
    Here is an example of a settings file:

    @code
    foobar.channel = console://

    foobar.pong.logLevel = Trace

    foobar.ping.channel = file:///log.txt
    foobar.ping.logLevel = Error
    @endcode

    In the example, pong would write messages with a log level of Trace
    or higher to the console channel, which it inherited from its parent.
    The target ping writes messages with a log level of Error of higher
    to a file.

    The main purpose of the logger is to route log records to its target. If
    the log level of the record is less severe than the current log level of
    the target, the logger will discard the record. If a target has a log
    level of Info, the logger will reject records with the levels Trace or
    Debug. See LogRecord and LogMessage for more information. 

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

        /** @brief Initialize logging targets with a settings file

            The given settings file is parsed and all listed targets are
            created and initialized. If a target exists already, it is
            reinitialized.

            @param file Path to a settings file
        */
        static void init(const std::string& file);

        //! @internal
        static void initTargets()
        { init("log.properties"); }

        /** @brief Initialize logging targets with a settings

            All targets listed in the given settings are created and
            initialized. If a target exists already, it is reinitialized.

            @param settings Settings to apply to target list
        */
        static void init(const Settings& settings);

        /** @brief Set the pattern for log records.

            This determines how the logging records are formatted to the
            channels. The format pattern can contain text and specifiers, 
            which are placeholders for the various elements of the log
            records. Specifiers are escaped with a percent sign in the
            format pattern string. For example, the following pattern would
            write out the time and the message for each log record:

            @code
            olib::log::Logger::setPattern("%t - %m");
            @endcode

            Here is a list of possible specifiers:
            - %c logging category
            - %d current date
            - %l log level (severity)
            - %m message text
            - %t current time
            - %F file where the record was logged
            - %L line number where the record was logged
            - %M method/function where the record was logged

            @todo
            - %T thread id
            - %P process id
        */
        static void setPattern(const std::string& pattern);

        /** @brief Sets the log-level of the target and its children.

            This method is thread-safe. The log-level can also be set
            in the settings file of the used for initialization. All
            children of this target inherit the given LogLevel unless
            they are already set to a log level explicitly i.e. this
            method has een called on a child before.
        */
        static void setLogLevel(const std::string& target, LogLevel level)
        { LogTarget::get(target).setLogLevel(level); }

        /** @brief Sets the channel to be used by the target and its children

            Throws a invalid_argument exception if the channel can not
            be created. This function might block until the channel could
            be opened. This method is thread-safe. The channel can also be
            set in the properties file of the logging-manager.
        */
        static void setChannel(const std::string& target, const std::string& url)
        { LogTarget::get(target).setChannel(url); }

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
            logger if the records log level allows it, or if @a always is
            set to true.
        */
        void log(const LogRecord& record, bool always = false)
        {
            if( always || this->enabled( record.logLevel() ) )
            {
                _target->log( record );
            }
        }

        //! @internal Only for unit-tests
        LogTarget& target() const
        { return *_target; }

    protected:
        //! @internal
        Logger(LogTarget& t)
        : _target( &t )
        {}

    private:
        LogTarget& initLogger(const std::string& name);

    private:
        //! @internal
        LogTarget* _target;
};

/** @brief Logs records with a logger. 
 
    Log messages can be used to log records with a specific logger. They
    maintain a log record and a reference to a logger. The log record text
    can be formatted with the stream output operator, just like for the log 
    records itself:

    @code
    Pt::System::Logger logger("app.module");

    Pt::System::LogMessage msg(logger, Pt::System::Info);
    if(msg)
    {
      msg << "Pi is exactly " << 3
          << " No, I was kidding, its really " << 3.1415
          << Pt::System::endlog;
    }
    @endcode

    To avoid the costs for formatting, it can be checked if the log level is
    enabled for the target. A log message can be send mutliple times, so
    formatting has to be done only once and the logging  performance can be
    increased.
 
    @code
    Pt::System::Logger logger("app.module");
    Pt::System::LogMessage msg(logger, Pt::System::Info);
    msg << "pi is: " << 3.1415;
 
    // log for the first time...
    msg.log();
 
    // ... and later the second time
    msg.log();
    @endcode
 
    @ingroup Logging
*/
class LogMessage : protected Pt::NonCopyable
{
    public:
        /** @brief Constructs a log message for a logger

            Contructs a log message, which uses @a logger to log records of
            the severity level specified by @a level. 
            If @a always is true, the message is send to the target without 
            checking the log level. It is assumed that the caller has already
            checked the log level of the underlying logger and set this flag
            accordingly. This flag only persists until the message has been 
            sent.
        */
        LogMessage(Logger& logger, const LogLevel& level, bool always = false)
        : _record(level)
        , _logger(&logger)
        , _enabled(always)
        { }

        //! @brief Destructor.
        ~LogMessage()
        {}
        
        //! @brief Returns the underlying log record.
        const LogRecord& record() const
        { return _record; }

        //! @brief Sends the message's log record to the logger.
        void log()
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

//! @brief Manipulator to end and send a log-message
inline LogMessage& endlog(LogMessage& msg)
{
    msg.log();
    return msg;
}

struct LoggerStaticInit
{
    template <typename F>
    LoggerStaticInit(F initfunc)
    { initfunc(); }
};

} // namespace System

} // namespace Pt

//! @internal @brief Log to a logger if the log level permits it.
#define logger_begin_impl(logger, level)            \
    if( ! logger.enabled( Pt::System::level ) )     \
        ;                                           \
    else Pt::System::LogMessage(static_cast<Pt::System::Logger&>(logger), Pt::System::level, true)

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
    #define log_init(file) Pt::System::Logger::init(file);

    //! @internal @brief Define a named global logger instance.
    #define log_define_impl(instance, category)                   \
    inline static Pt::System::Logger& instance()                  \
    {                                                             \
        static Pt::System::Logger instance##_instance(category);  \
        return instance##_instance;                               \
    }                                                             \
    static const Pt::System::LoggerStaticInit instance##_static_init( &instance );

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

#endif // Pt_System_Logger_h
