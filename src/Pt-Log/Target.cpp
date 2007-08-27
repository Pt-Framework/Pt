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
#include "LogManager.h"
#include "Pt/Log/Target.h"


namespace Pt {

namespace Log {

Target::Target(const std::string& name, Target* parent)
: Reflectable(name)
, _name(name)
, _logLevel(Fatal)
, _parent(parent)
, _channel(0)
, _logLevelExplicitelySet(false)
{
    void (Target::*setter)(const std::string&);
    setter = &Target::setLogLevel;

    this->registerProperty("logLevel", *this, &Target::logLevelString, setter);
    this->registerProperty("channel", *this, &Target::channel, &Target::setChannel);
    this->registerProperty("async", *this, &Target::async, &Target::setAsync);
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


LogLevel Target::logLevel() const
{
    return _logLevel;
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

}

}
