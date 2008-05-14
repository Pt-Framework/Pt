/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#include "Pt/System/FileSystemNode.h"
#include "Pt/System/File.h"
#include "Pt/System/Directory.h"
#include <string>
#include <windows.h>


namespace Pt {

namespace System {

class DirectoryIteratorImpl 
{
    public:
        DirectoryIteratorImpl();

        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        int ref();

        int deref();

        bool advance();

        FileSystemNode& node();

        const char* name() const;

        DirectoryEntry& entry();

    private:
        unsigned int    _refs;
        std::string _path;
        std::string _name;
        FileSystemNode* _node;
        File _file;
        Directory _dir;
        HANDLE          _findHandle;
        WIN32_FIND_DATA _current;
        DirectoryEntry _entry;
        bool _dirty;
};


class DirectoryImpl
{
    public:
        DirectoryImpl();

        ~DirectoryImpl();

    public:
        static void create(const std::string& path);

        static void remove(const std::string& path);

        static void move(const std::string& oldName, const std::string& newName);

        static bool exists(const std::string& path);

        static void changeCurrent(const std::string& path);
};

} // namespace System

} // namespace Pt


