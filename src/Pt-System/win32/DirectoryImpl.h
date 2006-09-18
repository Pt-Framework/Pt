/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include "Pt/Api.h"
#include "Pt/System/FileSystemNode.h"
#include <string>
#include <windows.h>


namespace Pt {

namespace System {

class PT_EXPORT DirectoryIteratorImpl {
	public:
		DirectoryIteratorImpl();

		DirectoryIteratorImpl(const char* path);

		~DirectoryIteratorImpl();

		int ref();

		int deref();

		void advance();

		FileSystemNode& node();

		std::string name() const;

		bool operator==(const DirectoryIteratorImpl& impl) const;

	private:
		unsigned int _refs;
		std::string _path;
		FileSystemNode* _node;
		HANDLE _findHandle;
		WIN32_FIND_DATA _current;
};


class PT_EXPORT DirectoryImpl {
	public:
		DirectoryImpl();
		~DirectoryImpl();
		
	public:
		static void create(const char* dirpath);

		static void remove(const char* dirpath);
		
		static std::string current();
		
		static std::string system();

		static void changeCurrent(const char* dirpath);
};

} // namespace System

} // namespace Pt


