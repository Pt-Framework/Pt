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

#include "Pt/Log/Message.h"
#include "Pt/Log/Logger.h"
#include "Pt/Log/Target.h"
#include "Pt/System/Clock.h"
#include <iostream>

namespace Pt {

namespace Log {

Message::Message(Logger& logger, const LogLevel level, const SourceInfo& source)
: _logger(&logger)
, _text()
, _level(level)
, _source(source)
, _dateTime()
, _threadId(-1)
, _procId(-1)
, _reserved(0)
{
}


Message::~Message()
{
}


void Message::send()
{
	if( _logger->enabled() )
	{
		_target = _logger->target().name();
		setTimestamp( System::Clock::getLocalTime() );
		setThreadId(0);
		setProcessId(0);
		setTimestamp( System::Clock::getLocalTime() );
		_logger->target().log( *this );
	}
}

} // namespace Log

} // namespace Pt
