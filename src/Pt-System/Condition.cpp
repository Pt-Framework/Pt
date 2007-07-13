/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr Marc Boris Dürner                       *
 *   Copyright (C) 2005-2006 by Sebastian Pieck                            *
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
#include "ConditionImpl.h"

#include "Pt/System/Condition.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"
#include <iostream>
using namespace std;


namespace Pt {

namespace System {


Condition::Condition()
{
    _impl = new ConditionImpl();
}


Condition::~Condition()
{
    _impl->broadcast();
    delete _impl;
}


void Condition::wait(Mutex& mtx)
{
   if (mtx.mode() == Mutex::Recursive)
        throw SystemError("Condition accepts only non recursive mutexes: ", PT_SOURCEINFO);
   _impl->wait(mtx);
}


bool Condition::wait(Mutex& mtx, unsigned int ms)
{
     if (mtx.mode() == Mutex::Recursive)
        throw SystemError("Condition accepts only non recursive mutexes: ", PT_SOURCEINFO);
    return _impl->wait(mtx, ms);
}


void Condition::signal()
{
    _impl->signal();
}


void Condition::broadcast()
{
    _impl->broadcast();
}


} // namespace System

} // !namespace Pt

