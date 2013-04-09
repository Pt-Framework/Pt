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
#include <Pt/Xml/QName.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

class ContentParticle;
class LeafParticle;
class SplitParticle;
class PcDataParticle;
class MatchParticle;

class PT_XML_API ElementModel : private NonCopyable
{
    enum ContentType
    {
        Undeclared = 0,
        Expression = 1,
        Empty = 2,
        Any = 3
    };

    public:
        ElementModel(const QName& name);
        
        ~ElementModel();

        void clear();

        const QName& qname() const;

        bool isUndeclared() const;
        
        bool isEmpty() const;

        void setEmpty();

        bool isAny() const;

        void setAny();

        bool isExpression() const;

        void setExpression(ContentParticle& start);

        const ContentParticle* content() const;

        std::size_t contentSize() const;

        const AttributeListModel& attributes() const;

        AttributeListModel& attributes();

    public:
        //! @internal use allocator
        LeafParticle& getLabel(const Pt::String& name);

        //! @internal use allocator
        SplitParticle& getSplit(ContentParticle& to);

        //! @internal use allocator
        PcDataParticle& getPcData();

        //! @internal use allocator
        MatchParticle& getMatch();

    private:
        QName _name;
        ContentParticle* _start;
        std::vector<ContentParticle*> _particles;
        ContentType _type;
        AttributeListModel _attrs;
};

} // namespace Xml

} // namespace Pt

#endif
