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
#include "Pt/Log/Logger.h"
#include "Pt/Log/Target.h"
#include "Pt/Log/Message.h"
#include "LogManager.h"


namespace Pt {

namespace Log {

Logger::Logger(const std::string& name, LogLevel level)
: _target( &LogManager::instance().target(name) )
, _level(level)
, _msg( 0 )
{
    _msg = new Message(name, level);
}


Logger::Logger(Target& target, LogLevel level)
: _target( &target )
, _level(level)
, _msg( 0 )
{
    _msg = new Message(target.name(), level);
}


Logger::~Logger()
{
    delete _msg;
}


void Logger::setLogLevel(LogLevel level)
{
    _level = level;
    _msg->setLogLevel(level);
}


LogLevel Logger::logLevel() const
{
    return _level;
}


Target& Logger::target() const
{
    return *_target;
}


void Logger::endlog()
{
    if( this->enabled() )
    {
        _msg->setText(_ss.str() );
        _target->log( *_msg );

        _ss.str("");
        _ss.clear();
    }
}


bool Logger::enabled() const
{
    return this->logLevel() <= _target->logLevel();
}

} // namespace Log

} // namespace Pt
