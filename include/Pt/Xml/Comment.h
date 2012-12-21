/*
 * Copyright (C) 2012 Marc Boris Duerner
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
#ifndef Pt_Xml_Comment_h
#define Pt_Xml_Comment_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

/** @brief A Comment element (Node) of an XML document.
  
    A Comment element stores the content of a comment. There is no interpretation of the
    comment's Text before it is stored.
*/
class PT_XML_API Comment : public Node 
{
    public:
        /** @brief Constructs with content string.
        */
        explicit Comment(const String& text)
        : Node(Node::Comment)
        , _text(text)
        { }

        /** @brief Returns the content of the comment.
        */
        String& text()
        { return _text; }

        /** @brief Returns the content of the comment.
        */
        const String& text() const
        { return _text; }

        /** @brief RSets the content of the comment.
        */
        void setText(const String& text)
        { _text = text; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::Comment; }

    private:
        String _text;
};


inline Comment* toComment(Node* node)
{
    return nodeCast<Comment>(node);
}


inline const Comment* toComment(const Node* node)
{
    return nodeCast<Comment>(node);
}


inline Comment& toComment(Node& node)
{
    return nodeCast<Comment>(node);
}


inline const Comment& toComment(const Node& node)
{
    return nodeCast<Comment>(node);
}

} // namespace Xml

} // namespace Pt

#endif
