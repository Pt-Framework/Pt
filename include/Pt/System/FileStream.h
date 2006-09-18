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

#ifndef Pt_System_FileStream_h
#define Pt_System_FileStream_h

#include <Pt/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/IO/IOBuffer.h>
#include <Pt/IO/IOStream.h>
#include <Pt/System/FileDevice.h>


namespace Pt {

namespace System {


	class PT_EXPORT FileBuffer : public IO::IOBuffer {
		public:
			FileBuffer(const char* name, FileDevice::OpenMode omode) throw(IO::IOError);
	
			const FileDevice& fileDevice() const
			{return _file;}

		private:
			FileDevice _file;
	};


	class PT_EXPORT FileStream : public IO::IOStream {
	public:
			FileStream(const char* name, FileDevice::OpenMode omode) throw(IO::IOError);
	
			~FileStream() throw();
	
		private:
			FileBuffer _buffer;
	};


} // namespace System

} // namespace Pt

#endif
