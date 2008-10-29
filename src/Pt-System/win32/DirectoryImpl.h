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
#include "Pt/System/Api.h"
#include <string>
#include <windows.h>

namespace Pt {

namespace System {

class DirectoryIteratorImpl 
{
    public:
        DirectoryIteratorImpl();
        : _refs(1),
        _findHandle(INVALID_HANDLE_VALUE),
        _dirty(true)
        { }

        DirectoryIteratorImpl(const std::string& path);

        ~DirectoryIteratorImpl();

        int ref()
        {
            return ++_refs;
        }

        int deref()
        {
            return --_refs;
        }

        bool advance();

        const std::string& name() const;
        {
            return _name;
        }

        const std::string& path() const;

    private:
        unsigned int _refs;
        mutable std::string _path;
        mutable std::string _name;
        HANDLE _findHandle;
        WIN32_FIND_DATA _current;
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

        static void chdir(const std::string& path);

        static std::string cwd();

        std::string DirectoryImpl::curdir()
        {
            return ".";
        }


        std::string DirectoryImpl::updir()
        {
            return "..";
        }


        std::string DirectoryImpl::rootdir()
        {
            return "c:\\";
        }

        static std::string tmpdir();

        std::string DirectoryImpl::sep()
        {
            return "\\";
        }
};

} // namespace System

} // namespace Pt


