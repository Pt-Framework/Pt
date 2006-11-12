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

#ifndef Pt_System_FileSystemNode_h
#define Pt_System_FileSystemNode_h

#include <string>
#include <Pt/Types.h>
#include <Pt/Api.h>

namespace Pt {

namespace System {

class PT_API FileSystemNode 
{
	public:
		FileSystemNode()
		{}

		virtual ~FileSystemNode()
		{}

		//! Returns the path of the file system node.
		virtual const std::string& path() const = 0;

		//! Returns the size of the file system node.
		virtual std::size_t size() const = 0;

		virtual void remove() = 0;

		// virtual void rename(const std::string& newname);
		virtual void move(const std::string& newname) = 0;
};

} // namespace System

} // namespace Pt

#endif
