/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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

#include "Pt/System/FileStream.h"


namespace Pt {

namespace System {


FileBuffer::FileBuffer(const char* name, FileDevice::OpenMode omode) throw(IO::IOError)
: IO::IOBuffer(),
  _file(name, omode)
{
	IO::IOBuffer::init(_file);
}




FileStream::FileStream(const char* path, FileDevice::OpenMode omode) throw(IO::IOError)
: IO::IOStream( &_buffer ),
  _buffer(path, omode)
{
	// no std::locale on WinCE
	#ifndef _WIN32_WCE
	this->imbue( std::locale(this->getloc(), new std::ctype<char>()) );
	this->imbue( std::locale(this->getloc(), new std::num_get<char>()) );
	this->imbue( std::locale(this->getloc(), new std::num_put<char>()) );
	this->imbue( std::locale(this->getloc(), new std::numpunct<char>()) );
	#endif
}


FileStream::~FileStream() throw()
{
	try {
		IO::IOStream::sync();
	}
	catch(...) {}
}


} // namespace System

} // namespace Pt


