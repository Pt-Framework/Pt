/*
 * Copyright (C) 2012 by Marc Boris Duerner
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
#ifndef Pt_Xml_ElementDeclaration_h
#define Pt_Xml_ElementDeclaration_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/AttributeListDeclaration.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

class ContentParticle;

class ElementDeclaration
{
    public:
        enum Type
        {
            Invalid = 0,
            Expression = 1,
            Empty = 2,
            Any = 3
        };

    public:
        ElementDeclaration()
        : _start(0)
        , _size(0)
        , _type(Invalid)
        {}

        bool isEmpty() const
        { return _type == Empty; }

        bool isAny() const
        { return _type == Any; }

        bool isExpression() const
        { return _type == Expression; }

        void setExpression(ContentParticle& start, unsigned n)
        { 
            _start = &start; 
            _size = n;
            _type = Expression;
        }

        void setEmpty()
        { 
            _start = 0;
            _size = 0;
            _type = Empty;
        }

        void setAny()
        { 
            _start = 0;
            _size = 0;
            _type = Any;
        }

        ContentParticle* contentModel()
        { return _start; }

        unsigned size() const
        { return _size; }

        AttributeListDeclaration& attlist()
        { return _attr; }

    private:
        AttributeListDeclaration _attr;
        ContentParticle* _start;
        unsigned _size;
        Type _type;
};

} // namespace Xml

} // namespace Pt

#endif
