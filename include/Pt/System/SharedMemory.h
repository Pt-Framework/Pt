/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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

#ifndef Pt_System_SharedMemory_h
#define Pt_System_SharedMemory_h

#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>


namespace Pt {

namespace System {

	class PT_SYSTEM_API SharedMemory: public NonCopyable {
		private:
			class SharedMemoryImpl* _impl;

		public:
			enum OpenMode {
				Read,
				Write,
			};

			SharedMemory(const char* name, size_t sz, OpenMode omode) throw(SystemError);

			~SharedMemory();

			void unlink() throw(SystemError);

			void* map(const void* addr) throw(SystemError);

			void unmap(void* addr) throw(SystemError);
	};

} // !namespace System

} // !namespace Pt

#endif
