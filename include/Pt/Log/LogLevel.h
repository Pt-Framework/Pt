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


namespace Pt {

namespace Log {


/** @brief Severity of the log-message
    @ingroup Logging
*/
enum LogLevel {
    Fatal = 0,
    Error = 100,
    Warn  = 200,
    Info  = 300,
    Debug = 400,
    Trace = 500
};


/** @brief Converts a log-level to a string
    @ingroup Logging
*/
inline std::string toString(LogLevel level)
{
    std::string s;

    switch( level )
    {
        case Fatal:
            s = "FATAL";
            break;

        case Error:
            s = "ERROR";
            break;

        case Warn:
            s = "WARN";
            break;

        case Info:
            s = "INFO";
            break;

        case Debug:
            s = "DEBUG";
            break;

        default: s = "TRACE";
    };

    return s;
}


}

}

#endif


