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
#ifndef Pt_Xml_DocType_h
#define Pt_Xml_DocType_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

class InputSource;
class InputStack;

class PT_XML_API DocType : public Node
                         , private Pt::NonCopyable
{
    public:
        DocType(InputStack& input);

        ~DocType();

        bool isDefined() const;

        void clear();

        const Pt::String& rootName() const;

        Pt::String& rootName();

        bool isExternal() const
        { return ! _publicId.empty() || ! _systemId.empty(); }

        bool isInternal() const
        { return _publicId.empty() && _systemId.empty(); }

        const Pt::String& publicId() const
        { return _publicId; }

        void setPublicId(const Pt::String& pubId)
        { _publicId = pubId; }

        const Pt::String& systemId() const
        { return _systemId; }

        void setSystemId(const Pt::String& sysId)
        { _systemId = sysId; }

        void setExternal(InputSource* is);

        //! @internal
        inline static Node::Type nodeId()
        { return Node::DocType; }

    private:
        Pt::String _rootName;
        Pt::String _publicId;
        Pt::String _systemId;
        InputStack* _input;
};


inline DocType* toDocType(Node* node)
{
    return nodeCast<DocType>(node);
}


inline const DocType* toDocType(const Node* node)
{
    return nodeCast<DocType>(node);
}


inline DocType& toDocType(Node& node)
{
    return nodeCast<DocType>(node);
}


inline const DocType& toDocType(const Node& node)
{
    return nodeCast<DocType>(node);
}


class EndDocType : public Node
                 , private Pt::NonCopyable
{
    public:
        EndDocType()
        : Node(Node::EndDocType)
        {}

        ~EndDocType()
        {}

        //! @internal
        inline static Node::Type nodeId()
        { return Node::EndDocType; }
};


inline EndDocType* toEndDocType(Node* node)
{
    return nodeCast<EndDocType>(node);
}


inline const EndDocType* toEndDocType(const Node* node)
{
    return nodeCast<EndDocType>(node);
}


inline EndDocType& toEndDocType(Node& node)
{
    return nodeCast<EndDocType>(node);
}


inline const EndDocType& toEndDocType(const Node& node)
{
    return nodeCast<EndDocType>(node);
}

} // namespace Xml

} // namespace Pt

#endif
