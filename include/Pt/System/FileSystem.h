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

#ifndef Pt_System_FileSystem_h
#define Pt_System_FileSystem_h

#include <Pt/Types.h>
#include <Pt/Exception.h>
#include <Pt/Singleton.h>
#include <Pt/System/Api.h>


namespace Pt {

namespace System {

	class FileSystemNode;


	class PT_SYSTEM_API FileSystem : public Singleton<FileSystem> 
	{
		friend class Singleton<FileSystem>;

		public:
			FileSystemNode* create(const char* path) const;
	};

} // namespace System

} // namespace Pt

#endif
