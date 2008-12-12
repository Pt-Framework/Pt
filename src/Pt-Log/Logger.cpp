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
#include "LogManager.h"
#include "Message.h"
#include "Pt/Log/Logger.h"
#include "Pt/Log/Target.h"
#include "Pt/System/Clock.h"
#include <memory>


namespace Pt {

namespace Log {

Logger::Logger(const std::string& name, LogLevel level)
: _target( &LogManager::instance().target(name) )
, _level(level)
, _msg( 0 )
{
    _msg = this->init( name, level );
}


Logger::Logger(Target& target, LogLevel level)
: _target( &target )
, _level(level)
, _msg( 0 )
{
    _msg = this->init( target.name(), level );
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


Logger& Logger::beginLog(const Pt::SourceInfo& si)
{
    if( this->enabled() )
    {
        _msg->setSourceInfo(si);
        _msg->setTimestamp( System::Clock::getLocalTime() );
    }

    return *this;
}


Logger& Logger::trace(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::trace(*this);
}


Logger& Logger::debug(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::debug(*this);
}


Logger& Logger::info(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::info(*this);
}


Logger& Logger::warn(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::warn(*this);
}


Logger& Logger::error(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::error(*this);
}


Logger& Logger::fatal(const Pt::SourceInfo& si)
{
    this->beginLog(si);
    return Pt::Log::fatal(*this);
}


Logger& Logger::trace()
{
    return Pt::Log::trace(*this);
}


Logger& Logger::debug()
{
    return Pt::Log::debug(*this);
}


Logger& Logger::info()
{
    return Pt::Log::info(*this);
}


Logger& Logger::warn()
{
    return Pt::Log::warn(*this);
}


Logger& Logger::error()
{
    return Pt::Log::error(*this);
}


Logger& Logger::fatal()
{
    return Pt::Log::fatal(*this);
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


Message* Logger::init(const std::string& name, LogLevel level)
{
    std::auto_ptr<Message> msg( new Message(name, level) );
    msg->setThreadId(0);
    msg->setProcessId(0);
    msg->setTimestamp( System::Clock::getLocalTime() );
    return msg.release();
}


} // namespace Log

} // namespace Pt
