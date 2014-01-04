/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#include "Pt/System/Api.h"
#include "Pt/System/FileInfo.h"
#include <string>
#include <dirent.h>

namespace Pt {

namespace System {

class DirectoryIteratorImpl
{
    public:
        DirectoryIteratorImpl()
        : _refs(1)
        , _handle(0)
        , _current(0)
        { }

        DirectoryIteratorImpl(const std::string& path);
        
        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        const FileInfo& get() const
        { return _finfo; }

        int ref()
        { return ++_refs; }

        int deref()
        { return --_refs; }

        bool advance();

    private:
        void init(const char*);

    private:
        unsigned int _refs;
        FileInfo _finfo;
        //std::string _path;
        std::size_t _pathlen;
        DIR* _handle;
        ::dirent* _current;
};


//class DirectoryImpl
//{
//    public:
//        static void create(const std::string& path);
//
//        static void remove(const std::string& path);
//
//        static void move(const std::string& oldName, const std::string& newName);
//
//        static void chdir(const std::string& path);
//
//        static std::string cwd();
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
//            return "/";
//        }
//
//        static std::string tmpdir();
//
//        static std::string sep()
//        {
//            return "/";
//        }
//};

} // namespace System

} // namespace Pt
