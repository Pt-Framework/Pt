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
#ifndef Pt_LogLevel_h
#define Pt_LogLevel_h

#include <Pt/Log/Api.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace Log {


/** @brief Severity of the log-message
    @ingroup Logging
*/
enum LogLevel {
    None  = 0,
    Fatal = 100,
    Error = 200,
    Warn  = 300,
    Info  = 400,
    Debug = 500,
    Trace = 600
};


/** @brief Converts a log-level to a string
    @ingroup Logging
*/
inline std::string toString(LogLevel level)
{
    switch (level)
    {
        case None:  return "NONE";
        case Fatal: return "FATAL";
        case Error: return "ERROR";
        case Warn:  return "WARN";
        case Info:  return "INFO";
        case Debug: return "DEBUG";
        case Trace: return "TRACE";
        default:    throw std::logic_error("Unknown log level in LogLevel::toString() method.");
    };
}


}

}

#endif


