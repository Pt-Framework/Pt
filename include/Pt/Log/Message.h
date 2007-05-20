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
#include <Pt/Log/Logger.h>
#include <Pt/SourceInfo.h>
#include <Pt/DateTime.h>
#include <string>


namespace Pt {

namespace Log {

class PT_LOG_API Message
{
    public:
        Message(const std::string& target, LogLevel level);

        ~Message();

        const std::string& target() const
        { return _target; }

        void setText(const std::string& text)
        { _text = text; }

        const std::string& text() const
        { return _text; }

        void setLogLevel(LogLevel level)
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

        void setThreadId(long id)
        { _threadId = id; }

        long processId() const
        { return _procId; }

        void setProcessId(long id)
        { _procId = id; }

    private:
        std::string    _target;
        std::string    _text;
        LogLevel       _level;
        Pt::SourceInfo _source;
        Pt::DateTime   _dateTime;
        long           _threadId;
        long           _procId;
};

} // namespace Log

} // namespace Pt


#endif


