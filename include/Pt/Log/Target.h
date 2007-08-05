/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Log_Target_h
#define Pt_Log_Target_h

#include <Pt/Log/Api.h>
#include <Pt/Log/Logger.h>
#include <Pt/Reflex/Reflectable.h>
#include <string>


namespace Pt {

namespace Log {

    class Channel;
    class Message;

    /** @brief Target of log-messages
        @ingroup Logging

        All created targets form a hierachy within the logging manager.
        To add an instance to this hierachy use the static Target::get
        method. The naming scheme of the targets follows the property
        dot-syntax. If there is a target foobar.ping and a target foobar.pong
        it means that ping and pong are children of the foobar target.
        Once a target is created a channel can be assigned to it. If no channel
        is assigned to a target, it will use the channel of the next of its parent
        targets. Channels can either be assigned by the class API or in
        the properties file that the logging-manager reads ion startup.
        Here is an example of how ping and pong would be configured:

        @code
        foobar.channel = console://

        foobar.ping.channel = comm:///dev/ttyS0
        foobar.ping.async = true
        foobar.ping.logLevel = Error

        foobar.pong.logLevel = Trace
        @endcode

        In the example, pong would write messages with a log-level of Trace
        or higher to the console channel it inherited from its parent. The
        target ping writes messages with a log-level of Error of higher
        asnychronously to the serial port.
    */
    class PT_LOG_API Target : public Reflex::Reflectable, protected Pt::NonCopyable
    {
        friend class LogManager;
        friend class Logger;

        protected:
            //! @internal Used within logging-manager
            Target(const std::string& name, Target* parent = 0);

        public:
            //! @brief Destructor
            virtual ~Target();

            /** @brief Returns the ID of the target

                This method is thread-safe.
            */
            const std::string& name() const;

            /** @brief Returns true if target is in async-mode

                This method is thread-safe.
            */
            bool async() const;

            /** @brief Enables or disables the async-mode

                This method is thread-safe. The async-mode can also be set
                in the properties file of the logging-manager.
            */
            void setAsync(bool isAsync);

            /** @brief Returns the log-level of the target

                This method is thread-safe.
            */
            LogLevel logLevel() const;

            /** @brief Sets the log-level of the target

                This method is thread-safe. The log-level can also be set
                in the properties file of the logging-manager.
            */
            void setLogLevel(LogLevel level);

            /** @brief Returns the URL of the channel used by the target

                This method is thread-safe.
            */
            std::string channel() const;

            /** @brief Sets the channel to be used by this target

                Throws a invalid_argument exception if the channel can not
                be created. This function might block until the channel could
                be opened. This method is thread-safe.The channel can also be
                set in the properties file of the logging-manager.
            */
            void setChannel(const std::string& url);


            /** @brief Get a target from the logging manager

                The target is created if it does not exist, otherwise the
                existing target is returned. If the target is created it is
                initialised with the properties from the configuration file
                of the loggin manager. This method is thread-safe.
            */
            static Target& get(const std::string& name);

        protected:
            //! @internal Used by Logger
            void log(const Message& msg);

            //! @internal Property callback
            std::string logLevelString() const;

            //! @internal Property callback
            void setLogLevel(const std::string&);

            //! @internal Only used on LogManager initialisation
            void assignChannel(Channel& ch);

        private:
            //! @internal
            std::string _name;
            //! @internal
            bool _async;

            //! @internal
            LogLevel _logLevel;

            //! @internal
            Target* _parent;

            //! @internal
            Channel* _channel;

            //! @internal
            void* _reserved;
    };

} // namespace Log

} // namespace Pt


#endif


