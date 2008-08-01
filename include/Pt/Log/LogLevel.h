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
        case None:  return "None";
        case Fatal: return "Fatal";
        case Error: return "Error";
        case Warn:  return "Warning";
        case Info:  return "Info";
        case Debug: return "Debug";
        case Trace: return "Trace";
        default:    throw std::logic_error("Unknown log level in LogLevel::toString() method.");
    };
}

/**
 * @brief Converts a string to the appropriate log level.
 * @ingroup Logging
 *
 * @param levelStr The log level string to convert.
 * @return The appropriate log level object.
 */
inline LogLevel toLogLevel(std::string levelString)
{
    if(levelString == "None")
    {
        return None;
    }
    else if(levelString == "Fatal")
    {
        return Fatal;
    }
    else if(levelString == "Error")
    {
        return Error;
    }
    else if(levelString == "Warning")
    {
        return Warn;
    }
    else if(levelString == "Info")
    {
        return Info;
    }
    else if(levelString == "Debug")
    {
        return Debug;
    }
    else if(levelString == "Trace")
    {
        return Trace;
    }

    throw std::logic_error("Specified log level string doesn't fit to a log level in LogLevel::toLogLevel() method.");
}


}

}

#endif


