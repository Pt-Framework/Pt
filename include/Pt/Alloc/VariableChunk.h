/***************************************************************************
 *   Copyright (C) 2008-2010 by Bendri Batti                               *
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

#ifndef VARIABLE_CHUNK_H
#define VARIABLE_CHUNK_H

#include <Pt/Alloc/Api.h>
#include <cstddef>

namespace Pt {
namespace Alloc {

class VariableChunk
{
	public:
		VariableChunk(VariableChunk* nextChunk, std::size_t chunkSize);
		
		~VariableChunk();

		void* allocate(std::size_t reqSize);

		VariableChunk *nextVariableChunk()
		{ return _nextChunk; }

		std::size_t spaceAvailable()
		{ return _chunkSize - _bytesAlreadyAllocated; }

		enum { DEFAULT_VARIABLE_CHUNK_SIZE = 4096 };

	private:
		VariableChunk* _nextChunk;
		void* _mem;
		std::size_t _chunkSize;
		std::size_t _bytesAlreadyAllocated;

};

}
}
#endif