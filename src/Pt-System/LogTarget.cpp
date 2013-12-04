/*
 * Copyright (C) 2005-2012 by Dr. Marc Boris Duerner
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
#include <Pt/System/LogTarget.h>
#include <cassert>

namespace Pt {

namespace System {

LogTarget::LogTarget(const std::string& targetName, LogTarget* parentTarget)
: _parent(parentTarget)
, _name(targetName)
, _loglevel(Fatal)
, _inheritLogLevel(true)
, _channel(0)
, _inheritChannel(true)
{
    if(parentTarget)
        atomicSet(_loglevel, atomicGet(_parent->_loglevel));
}


LogTarget::~LogTarget()
{
}


const std::string& LogTarget::name() const
{
    return _name;
}


void LogTarget::setLogLevel(LogLevel level)
{
    // thread-safe
    LogManager::instance().setLogLevel(*this, level);
}


std::string LogTarget::channelUrl() const
{
    // thread-safe
    return LogManager::instance().getChannel(*this);
}


void LogTarget::setChannel(const std::string& url)
{
    // thread-safe
    LogManager::instance().setChannel(*this, url);
}


LogTarget& LogTarget::get(const std::string& name)
{
    // thread-safe
    return LogManager::instance().target(name);
}


void LogTarget::log(const LogRecord& record)
{
    // thread-safe
    LogManager::instance().log(*this, record);
}


bool LogTarget::inheritsLogLevel() const
{
    return _inheritLogLevel;
}


void LogTarget::assignLogLevel(int level, bool inherited)
{
    atomicSet(_loglevel, level);
    _inheritLogLevel = inherited;
}


bool LogTarget::inheritsChannel() const
{
    assert(_inheritChannel == (! _channel) );
    return _inheritChannel;
}


void LogTarget::removeChannel()
{
    _channel = 0;
    _inheritChannel = true;
}


void LogTarget::assignChannel(LogChannel& ch)
{
    _channel = &ch;
    _inheritChannel = false;
}

} // namespace System

} // namespace Pt
