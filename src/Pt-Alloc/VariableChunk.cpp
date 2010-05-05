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

#include <Pt/Alloc/VariableChunk.h>

namespace Pt {
namespace Alloc {

	VariableChunk::VariableChunk(VariableChunk* nextChunk, std::size_t chunkSize)
	: _nextChunk(0)
	, _mem(0)
	, _chunkSize(chunkSize > DEFAULT_VARIABLE_CHUNK_SIZE ? chunkSize : DEFAULT_VARIABLE_CHUNK_SIZE)
	, _bytesAlreadyAllocated(0)
	{
		_nextChunk = nextChunk;
		_mem = new char[ _chunkSize ];
	}

	VariableChunk::~VariableChunk()
	{
		delete [] _mem;
	}

	void* VariableChunk::allocate(std::size_t reqSize)
	{
		void* addr = &_mem  + _bytesAlreadyAllocated;
		_bytesAlreadyAllocated += reqSize;

		return addr;
	}
}
}
