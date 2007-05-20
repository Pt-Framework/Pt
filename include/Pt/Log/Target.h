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
#include <string>


namespace Pt {

namespace Log {

    class Channel;
    class Message;


    class PT_LOG_API Target : protected Pt::NonCopyable
    {
        friend class LogManager;

        protected:
            Target(const std::string& name, Target* parent = 0, Channel* channel = 0);

        public:
            virtual ~Target();

            const std::string& name() const;

            LogLevel logLevel() const;

            void setLogLevel(LogLevel level);

            void setChannel(const std::string& url);

            void log(const Message& msg);

            static Target& get(const std::string& name);

        protected:
            void setChannel(Channel& ch);

        private:
            std::string _name;
            LogLevel _logLevel;
            Target* _parent;
            Channel* _channel;
            void* _reserved;
    };

} // namespace Log

} // namespace Pt


#endif


