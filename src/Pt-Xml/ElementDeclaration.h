/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#include "AttributeListDeclaration.h"
#include <Pt/Xml/Api.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

class ContentParticle;

class PT_XML_API ContentModel : private NonCopyable
{
    enum ContentType
    {
        //TODO: Undeclared (only ATTLIST, missing ELEMENT)
        Invalid = 0,
        Expression = 1,
        Empty = 2,
        Any = 3
    };

    public:
        ContentModel();
        
        ~ContentModel();

        const AttributeListModel& attributeList() const;

        AttributeListModel& attributeList();
        
        bool isEmpty() const;

        void setEmpty();

        bool isAny() const;

        void setAny();

        bool isExpression() const;

        void setExpression(ContentParticle& start, unsigned n);

        const ContentParticle* content() const;

        std::size_t contentSize() const;

    private:
        // TODO: use std::vector<ContentParticle*> and own particles
        //       front() should be start
        //       DtdBuilder fills it
        //       reference to DocTypeContext for allocation
        ContentParticle* _start;
        std::size_t _size;
        ContentType _type;
        AttributeListModel _attrs;
};

} // namespace Xml

} // namespace Pt

#endif
