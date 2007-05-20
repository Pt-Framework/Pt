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
#ifndef Pt_LogDevice_h
#define Pt_LogDevice_h

#include <Pt/Log/Api.h>
#include <Pt/Log/LogLevel.h>
#include <Pt/NonCopyable.h>
#include <list>
#include <string>
#include <sstream>


namespace Pt {

namespace Log {

class Target;


class PT_LOG_API Logger : protected Pt::NonCopyable
{
    public:
        Logger(const std::string& name, LogLevel level = Trace);

        Logger(Target& target, LogLevel level = Trace);

        ~Logger();

        void setLogLevel(LogLevel level);

        LogLevel logLevel() const;

        bool enabled() const;

        Target& target() const;

        template <typename T>
        Logger& write(const T& value)
        {
            if( this->enabled() )
            {
                _ss << value;
            }

            return *this;
        }

        void endlog();

    private:
        class Target*     _target;
        LogLevel          _level;
        class Message*    _msg;
        std::stringstream _ss;
        void* _reserved;
};


template <typename T>
Logger& operator<<(Logger& logger, const T& value)
{
    return logger.write(value);
}


inline Logger& operator<<(Logger& logger, std::ios_base& (*pf)(std::ios_base&))
{
    return logger;
}


inline Logger& operator<<(Logger& stream, Logger& (*pf)(Logger&))
{
    return pf(stream);
}


inline Logger& fatal(Logger& str)
{ str.setLogLevel(Pt::Log::Fatal); return str; }


inline Logger& error(Logger& str)
{ str.setLogLevel(Pt::Log::Error); return str; }


inline Logger& warn(Logger& str)
{ str.setLogLevel(Pt::Log::Warn); return str; }


inline Logger& info(Logger& str)
{ str.setLogLevel(Pt::Log::Info); return str; }


inline Logger& debug(Logger& str)
{ str.setLogLevel(Pt::Log::Debug); return str; }


inline Logger& trace(Logger& str)
{ str.setLogLevel(Pt::Log::Trace); return str; }


inline Logger& endlog(Logger& str)
{ str.endlog(); return str; }

}

}

#endif


