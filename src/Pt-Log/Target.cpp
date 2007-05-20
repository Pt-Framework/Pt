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

Target::Target(const std::string& name, Target* parent, Channel* channel)
: _name(name)
, _logLevel(Info)
, _parent(parent)
, _channel(channel)
{
}


Target::~Target()
{
}


const std::string& Target::name() const
{
    return _name;
}


LogLevel Target::logLevel() const
{
    return _logLevel;
}


void Target::setLogLevel(LogLevel level)
{
    _logLevel = level;
}


void Target::setChannel(const std::string& url)
{
    LogManager::instance().setChannel(this, url);
}


void Target::log(const Message& message)
{
    LogManager::instance().log(this, message);
}


}

}
