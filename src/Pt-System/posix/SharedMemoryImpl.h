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
#include "Pt/Types.h"
#include "Pt/System/SharedMemory.h"

#include <string>


namespace Pt {

namespace System {

	class PT_API SharedMemoryImpl {
		public:

			SharedMemoryImpl(const char* name, size_t sz, SharedMemory::OpenMode omode) throw(SystemError);

			~SharedMemoryImpl();

			void unlink() throw(SystemError);

			void* map(const void* addr) throw(SystemError);

			void unmap(void* addr) throw(SystemError);

		private:
			std::string _name;
			SharedMemory::OpenMode _mode;
			size_t _size;
			int _fd;
	};

} // !namespace System

} // !namespace Pt

