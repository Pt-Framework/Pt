/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef Pt_System_FileSystemNode_h
#define Pt_System_FileSystemNode_h

#include <Pt/System/Api.h>
#include <string>

namespace Pt {

namespace System {

class PT_SYSTEM_API FileSystemNode
{
    public:
        enum Type
        {
            Invalid   = 0,
            File      = 1,
            Directory = 2
        };

        virtual ~FileSystemNode()
        {}

        virtual Type type() const = 0;

        virtual std::string name() const = 0;

        //! Returns the path of the file system node.
        virtual const std::string& path() const = 0;

        virtual std::string dirName() const = 0;

        //! Returns the size of the file system node.
        virtual std::size_t size() const = 0;

        virtual void remove() = 0;

        virtual void move(const std::string& newname) = 0;

        //virtual void copy(const std::string& to) const = 0;

        static Type stat(const char* path);

        static bool exists(const char* path)
        { return FileSystemNode::stat(path) != FileSystemNode::Invalid; }

        static FileSystemNode* create(const char* path);
};

} // namespace System

} // namespace Pt

#endif
