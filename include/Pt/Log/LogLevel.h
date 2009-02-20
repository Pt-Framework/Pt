/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Drner
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
    else if(levelString == "Warn")
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

    throw std::logic_error("invalid logevel");
}

inline LogLevel trace()
{
    return Pt::Log::Trace;
}


inline LogLevel info()
{
    return Pt::Log::Info;
}


inline LogLevel debug()
{
    return Pt::Log::Debug;
}


inline LogLevel warn()
{
    return Pt::Log::Warn;
}


inline LogLevel error()
{
    return Pt::Log::Error;
}


inline LogLevel fatal()
{
    return Pt::Log::Fatal;
}

}

}

#endif


