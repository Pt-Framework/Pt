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

#ifndef Pt_Xml_Characters_h
#define Pt_Xml_Characters_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

/** @brief A Character node represents text in an XML document.
 
    Character nodes are reported for text between a start element and an
    end element, between two start elements or between two end elements.
    Not only normal text, but also CDATA sections are reported as %Characters,
    unless the XmlReader is configured to report CDATA sections separately.
*/
class Characters : public Node
{
    public:
        /** @brief Constructs an empty node.
        */
        Characters()
        : Node(Node::Characters)
        , _content()
        , _isSpace(true)
        , _cdata(false)
        , _isChunk(false)
        { }

        /** @brief Clears all content.
        */
        void clear()
        { 
            _content.clear(); 
            _isSpace = true;
            _cdata = false;
            _isChunk = false;
        }

        /** @brief Returns true if empty.
        */
        bool empty() const
        { return _content.empty(); }

        /** @brief Indicates that text was in a CDATA section.
        */
        void setCData(bool cdata)
        { _cdata = cdata; }

        /** @brief Indicates that text was in a CDATA section.
        */
        bool isCData() const
        { return _cdata; }

        /** @brief Indicates that text might be split up.
        */
        void setChunk(bool val)
        { _isChunk = val; }

        /** @brief Indicates that text might be split up.
        */
        bool isChunk() const
        { return _isChunk; }

        /** @brief Returns true if the text consists only of whitespace.
        */
        bool isSpace() const
        { return _isSpace; }
       
        /** @brief Appends a character to the text.
        */
        inline void append(Char ch)
        {
            if(_isSpace && ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
                _isSpace = false;

            _content += ch;
        }

        /** @brief Returns the text.
        */
        const String& content() const
        { return _content; }

        /** @brief Returns the text.
        */
        String& content()
        { return _content; }

        /** @brief Sets the content.
        */
        void setContent(const String& content)
        { _content = content; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::Characters; }

    private:
        String _content;
        bool _isSpace;
        bool _cdata;
        bool _isChunk;
};

/** @brief Casts a generic node to a Characters node.
*/
inline Characters* toCharacters(Node* node)
{
    return nodeCast<Characters>(node);
}

/** @brief Casts a generic node to a Characters node.
*/
inline const Characters* toCharacters(const Node* node)
{
    return nodeCast<Characters>(node);
}

/** @brief Casts a generic node to a Characters node.
*/
inline Characters& toCharacters(Node& node)
{
    return nodeCast<Characters>(node);
}

/** @brief Casts a generic node to a Characters node.
*/
inline const Characters& toCharacters(const Node& node)
{
    return nodeCast<Characters>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Characters_h
