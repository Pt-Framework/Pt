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

#ifndef Pt_System_LogTarget_h
#define Pt_System_LogTarget_h

#include <Pt/System/Api.h>
#include <Pt/System/LogLevel.h>
#include <Pt/NonCopyable.h>
#include <Pt/Atomicity.h>
#include <string>

namespace Pt {

namespace System {

class LogChannel;
class LogRecord;

/** @brief Target of log-messages.

    The logging manager singleton maintains a hierachy of log targets, which
    is used to route the log records to a channel. To add an instance
    to this hierachy use the static LogTarget::get method. The scheme of the
    target category name follows the property dot-syntax. If there is a target
    foobar.ping and a target foobar.pong it means that ping and pong are
    children of the foobar target, which is a child of the root target. 
    
    Targets are not used directly, but by the loggers in an application.
    Once a target is created, a channel can be assigned to it. If no channel
    is assigned to a target, it will use the channel of the next of its parent
    targets. Each target also has a log level which is used by the logger to
    determine if a record should be sent to a target.

    @ingroup Logging
*/
class PT_SYSTEM_API LogTarget : protected Pt::NonCopyable
{
    friend class LogManager;

    protected:
        //! @internal Used within logging-manager
        LogTarget(const std::string& name, LogTarget* parent = 0);

    public:
        //! @brief Destructor.
        virtual ~LogTarget();

        //! @brief Returns the name of the target.
        const std::string& name() const;

        //! @brief Returns the log level of the target.
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

        //! @brief Returns the URL of the channel used by the target
        std::string channelUrl() const;

        /** @brief Sets the channel to be used by this target

            Throws a invalid_argument exception if the channel can not
            be created. This function might block until the channel could
            be opened. This method is thread-safe. The channel can also be
            set in the properties file of the logging-manager.
        */
        void setChannel(const std::string& url);

        //! @brief Write log record to this target
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
};

} // namespace System

} // namespace Pt

#endif // Pt_System_LogTarget_h
