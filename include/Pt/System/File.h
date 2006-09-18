/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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

#ifndef Pt_System_File_h
#define Pt_System_File_h

#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/FileSystemNode.h>


namespace Pt {

namespace System {

class PT_EXPORT File : public FileSystemNode, public NonCopyable {
	private:
		class FileImpl* _impl;

	public:
		File() throw(SystemError);

		File(const char* path) throw(SystemError);

		~File() throw();

		void open(const char* path) throw(SystemError);

		void close() throw(SystemError);

		const char* path() const;

		std::size_t size() const;
				
		void resize(std::size_t newSize);

		void remove();

		//void copy(const char* to) const;

		//void move(const char* to);
};

} // namespace System

} // namespace Pt

#endif
