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

#ifndef Pt_System_LogTarget_h
#define Pt_System_LogTarget_h

#include <Pt/System/Api.h>
#include <Pt/System/LogLevel.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

class SerializationInfo;

namespace System {

class LogChannel;
class LogMessage;

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
    friend class Logger;
    //friend PT_SYSTEM_API void operator >>= (const SerializationInfo& si, LogTarget& target);

    protected:
        //! @internal Used within logging-manager
        LogTarget(const std::string& name, LogTarget* parent = 0);

    public:
        //! @brief Destructor
        virtual ~LogTarget();

        /** @brief Returns the name of the target

            This method is thread-safe.
        */
        const std::string& name() const;

        /** @brief Returns the log-level of the target

            This method is thread-safe.
        */
        LogLevel logLevel() const
        {
            return _logLevel;
        }

        /** @brief Sets the log-level of the target explicitely

            This method is thread-safe. The log-level can also be set
            in the properties file of the logging-manager. All children of this
            target inherit the given LogLevel.
        */
        void setLogLevel(LogLevel level);

        /** @brief Returns the URL of the channel used by the target

            This method is thread-safe.
        */
        std::string channel() const;

        /** @brief Sets the channel to be used by this target

            Throws a invalid_argument exception if the channel can not
            be created. This function might block until the channel could
            be opened. This method is thread-safe. The channel can also be
            set in the properties file of the logging-manager.
        */
        void setChannel(const std::string& url);

        /** @brief Initialize logging targets with a settings file
        
            The given settings file is parsed and all listed targets are 
            created and initialized. If a target exists already, it is
            reinitialized.

            @param file Path to a setings file
        */
        static void init(const std::string& file);

        /** @brief Get a target from the logging manager

            The target is created if it does not exist, otherwise the
            existing target is returned. If the target is created it is
            initialised with the properties from the configuration file
            of the loggin manager. This method is thread-safe.
        */
        static LogTarget& get(const std::string& name);

    protected:
        //! @internal Used by Logger
        void log(const LogMessage& msg);

        //! @internal Only used on LogManager initialisation
        bool inheritsLogLevel() const;

        //! @internal Only used on LogManager initialisation
        void assignLogLevel(LogLevel level, bool inherited);

        //! @internal Only used on LogManager initialisation
        bool inheritsChannel() const;

        //! @internal Only used on LogManager initialisation
        void assignChannel(LogChannel& ch);

    private:
        //! @internal
        std::string _name;

        //! @internal
        LogLevel _logLevel;

        //! @internal
        bool _inheritLogLevel;

        //! @internal
        LogTarget* _parent;

        //! @internal
        LogChannel* _channel;
        
        //! @internal
        bool _inheritChannel;

        //! @internal
        void* _reserved;
};

//PT_SYSTEM_API void operator >>= (const SerializationInfo& si, LogTarget& target);

} // namespace Log

} // namespace Pt


#endif


