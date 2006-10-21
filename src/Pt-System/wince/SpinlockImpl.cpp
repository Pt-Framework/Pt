/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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
#include "SpinlockImpl.h"

#include <windows.h>


namespace Pt {

namespace System {


SpinlockImpl::SpinlockImpl()
: _count(0)
{
}


SpinlockImpl::~SpinlockImpl()
{
}


void SpinlockImpl::lock()
{
	// busy loop until unlock
	while( InterlockedExchange((long*)&_count,_count) != 0 ) {
		;
	}

	// set locked
	InterlockedExchange((long*)&_count, 1);
}


void SpinlockImpl::unlock()
{
	// set unlocked
	InterlockedExchange((long*)&_count, 0);
}


}

}
