/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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

#include "Pt/System/File.h"

#include "FileImpl.h"


namespace Pt {

namespace System {

File::File(const std::string& path, mode mode)
{
    // TODO: consider better exception type
	_impl = new FileImpl(path);
	switch(mode) {
		case UseExisting:
			if ( !_impl->exists() )
				throw std::invalid_argument("File " + path + " does not exist." + PT_SOURCEINFO);
			break;

		case Create: 
			if ( !_impl->exists() )
				_impl->create();
			break;
		default: std::invalid_argument("wrong mode for constructor" + PT_SOURCEINFO);
			 break;

	}
}


File::~File()
{
}


const std::string& File::path() const
{
	return _impl->path();
}


std::size_t File::size() const
{
	return _impl->size();
}


void File::resize(std::size_t newSize)
{
	return _impl->resize(newSize);
}


void File::remove()
{
	return _impl->remove();
}

/*
void File::copy(const char* to) const
{
	return _impl->copy(to);
}
*/

void File::move(const std::string& newname)
{
	return _impl->move(newname);
}

} // namespace System

} // namespace Pt
