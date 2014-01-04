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

/** @brief A %Comment node represents a comment in an XML document.
  
    Comments are only reported if the XmlReader is configured to do so.
*/
class Comment : public Node 
{
    public:
        /** @brief Constructs an empty %Comment node.
        */
        explicit Comment()
        : Node(Node::Comment)
        , _content()
        { }

        /** @brief Returns true if is empty.
        */
        bool empty() const
        { return _content.empty(); }

        /** @brief Clears all content.
        */
        void clear()
        { _content.clear(); }

        /** @brief Returns the text of the comment.
        */
        String& content()
        { return _content; }

        /** @brief Returns the text of the comment.
        */
        const String& content() const
        { return _content; }

        /** @brief Sets the text of the comment.
        */
        void setContent(const String& text)
        { _content = text; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::Comment; }

    private:
        String _content;
};

/** @brief Casts a generic node to a Comment node.
*/
inline Comment* toComment(Node* node)
{
    return nodeCast<Comment>(node);
}

/** @brief Casts a generic node to a Comment node.
*/
inline const Comment* toComment(const Node* node)
{
    return nodeCast<Comment>(node);
}

/** @brief Casts a generic node to a Comment node.
*/
inline Comment& toComment(Node& node)
{
    return nodeCast<Comment>(node);
}

/** @brief Casts a generic node to a Comment node.
*/
inline const Comment& toComment(const Node& node)
{
    return nodeCast<Comment>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Comment_h
