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

/** @brief A Character element (Node) of an XML document, containing the body's Text of a tag.
 
    A Character element stores the data of a tag's body. The data is interpreted before it
    is set as content of a Character element. This means that entities were translated into
    their corresponding character sequence.
*/
class Characters : public Node
{
    public:
        /** @brief Constructs with content string.
        */
        Characters()
        : Node(Node::Characters)
        , _content()
        , _isSpace(true)
        , _cdata(false)
        , _isChunk(false)
        { }

        /** @brief Returns true if is empty.
        */
        bool empty() const
        { return _content.empty(); }

        void setCData(bool cdata)
        { _cdata = cdata; }

        bool isCData() const
        { return _cdata; }

        void setChunk(bool val)
        { _isChunk = val; }

        bool isChunk() const
        { return _isChunk; }

        void clear()
        { 
            _content.clear(); 
            _isSpace = true;
            _cdata = false;
            _isChunk = false;
        }

        bool isSpace() const
        { return _isSpace; }
       
        //inline void appendSpace(Char ch)
        //{
        //    _content += ch;
        //}

        inline void append(Char ch)
        {
            if(_isSpace && ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
                _isSpace = false;

            _content += ch;
        }

        /** @brief Returns the content as a string.
        */
        const String& content() const
        { return _content; }

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


inline Characters* toCharacters(Node* node)
{
    return nodeCast<Characters>(node);
}


inline const Characters* toCharacters(const Node* node)
{
    return nodeCast<Characters>(node);
}


inline Characters& toCharacters(Node& node)
{
    return nodeCast<Characters>(node);
}


inline const Characters& toCharacters(const Node& node)
{
    return nodeCast<Characters>(node);
}

} // namespace Xml

} // namespace Pt

#endif
