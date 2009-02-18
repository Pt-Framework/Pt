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
#ifndef Pt_Log_Message_h
#define Pt_Log_Message_h

#include <Pt/Log/Api.h>
#include <Pt/Log/LogLevel.h>
#include <Pt/SourceInfo.h>
#include <Pt/DateTime.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <sstream>

namespace Pt {

namespace Log {

    class Logger;

    /** @brief %Log message
        @ingroup Logging
    */
    class PT_LOG_API Message : protected Pt::NonCopyable
    {
        public:
            Message(Logger& logger, LogLevel level, const SourceInfo& source);

            Message(const Message& other)
            : _source(other._source)
            {}

            ~Message();

            const std::string& target() const
            { return _target; }

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
			Message& operator<<(const T& value)
			{
				//if( _logger->enabled() )
				    _text << value;

				return *this;
			}

			Message& operator<<( Message& (*pf)(Message&) )
			{
				return pf(*this);
			}

            void send();

        private:
            Logger*            _logger;
            std::string        _target;
            std::ostringstream _text;
            LogLevel           _level;
            Pt::SourceInfo     _source;
            Pt::DateTime       _dateTime;
            long               _threadId;
            long               _procId;
            void*              _reserved;
    };

inline Message& info(Message& msg)
{
    msg.setLogLevel(Pt::Log::Info);
    return msg;
}

inline Message& endlog(Message& msg)
{
    msg.send();
    return msg;
}

} // namespace Log

} // namespace Pt

#endif

