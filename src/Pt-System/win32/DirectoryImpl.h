/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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

#include <Pt/WinVer.h>
#include <Pt/System/Api.h>
#include <Pt/System/FileInfo.h>
#include <string>
#include <cstddef>
#include <windows.h>

namespace Pt {

namespace System {

class DirectoryIteratorImpl 
{
    public:
        DirectoryIteratorImpl()
        : _refs(1)
        , _findHandle(INVALID_HANDLE_VALUE)
        { }

        DirectoryIteratorImpl(const std::string& path);

        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        int ref()
        { return ++_refs; }

        int deref()
        { return --_refs;}

        bool advance();

        const FileInfo& get() const
        { return _finfo; }

    private:
        void init(const char* path, std::size_t pathlen);

    private:
        unsigned int _refs;
        FileInfo _finfo;
        HANDLE _findHandle;
        WIN32_FIND_DATA _current;
};


//class DirectoryImpl
//{
//    public:
//        static void chdir(const std::string& path);
//
//        static std::string cwd();
//
//        static std::string tmpdir();
//
//        static std::string curdir()
//        {
//            return ".";
//        }
//
//        static std::string updir()
//        {
//            return "..";
//        }
//
//        static std::string rootdir()
//        {
//            return "c:\\";
//        }
//
//        static std::string sep()
//        {
//            return "\\";
//        }
//};

} // namespace System

} // namespace Pt
