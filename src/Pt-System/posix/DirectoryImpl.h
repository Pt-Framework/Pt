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

#include <dirent.h>


namespace Pt {

namespace System {

    class FileSystemNode;


    class PT_API DirectoryIteratorImpl {
        public:
            DirectoryIteratorImpl();

            DirectoryIteratorImpl(const char* path);

            ~DirectoryIteratorImpl();

            int ref();

            int deref();

            void advance();

            const char* path() const
            { return _path.c_str(); }

            FileSystemNode& node();

            std::string name() const;

            bool operator==(const DirectoryIteratorImpl& impl) const;

        private:
            unsigned int _refs;
            std::string _path;
            FileSystemNode* _node;
            DIR* _handle;
            ::dirent* _current;
    };


    class PT_API DirectoryImpl {
        public:
            DirectoryImpl() {}
            ~DirectoryImpl() {}

        public:
            static void create(const char* dirpath);

            static void remove(const std::string& path);

            static void move(const std::string& oldname, const std::string& newname);

            static bool exists(const std::string& path);

            static std::string current();

            static std::string system();

            static char separator()
            { return '/'; }

            static void changeCurrent(const char* dirpath);
    };

} // namespace System

} // namespace Pt


