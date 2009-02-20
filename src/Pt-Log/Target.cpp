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
#include "Pt/Log/Target.h"

namespace Pt {

namespace Log {

Target::Target(const std::string& name, Target* parent)
: _name(name)
, _async(false)
, _logLevel(Fatal)
, _parent(parent)
, _channel(0)
, _logLevelExplicitelySet(false)
{
}


Target::~Target()
{
}


const std::string& Target::name() const
{
    return _name;
}


bool Target::async() const
{
    return _async;
}


void Target::setAsync(bool isAsync)
{
    _async = isAsync;
}


void Target::setLogLevel(LogLevel level)
{
    _logLevel = level;
    _logLevelExplicitelySet = true;

    // Iterate through childs and set new LogLevels
    LogManager::instance().updateChildLogLevels(*this);
}


std::string Target::channel() const
{
    return "";
}


void Target::setChannel(const std::string& url)
{
    // thread-safe
    LogManager::instance().setChannel(*this, url);
}


void Target::log(const Message& message)
{
    // thread-safe
    LogManager::instance().log(*this, message, _async);
}


Target& Target::get(const std::string& name)
{
    // thread-safe
    return LogManager::instance().target(name);
}


bool Target::logLevelExplicitelySet()
{
    return _logLevelExplicitelySet;
}


std::string Target::logLevelString() const
{
    return toString(_logLevel);
}


void Target::setLogLevel(const std::string& level)
{
    if(level == "None")
    {
        this->setLogLevel(None);
    }
    if(level == "Fatal")
    {
        this->setLogLevel(Fatal);
    }
    else if(level == "Error")
    {
        this->setLogLevel(Error);
    }
    else if(level == "Warn")
    {
        this->setLogLevel(Warn);
    }
    else if(level == "Info")
    {
        this->setLogLevel(Info);
    }
    else if( level == "Debug")
    {
        this->setLogLevel(Debug);
    }
    else if(level == "Trace")
    {
        this->setLogLevel(Trace);
    }
}


void Target::setLogLevelImplicitely(LogLevel level)
{
    _logLevel = level;
}


void Target::assignChannel(Channel& ch)
{
    _channel = &ch;
}


void operator>>= (const SerializationInfo& si, Target& target)
{
    const SerializationInfo* member = si.findMember("logLevel");
    if(member)
    {
        target.setLogLevel( member->toString().narrow() );
    }

    member = si.findMember("channel");
    if(member)
    {
        target.setChannel( member->toString().narrow() );
    }

    member = si.findMember("async");
    if(member)
    {
        target.setAsync( member->toValue<bool>() );
    }
}

}

}
