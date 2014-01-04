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

#ifndef Pt_Xml_StartDocument_h
#define Pt_Xml_StartDocument_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>

namespace Pt {

namespace Xml {

/** @brief A Node representing the begin of the XML document.

    %StartDocument nodes are only reported when the XmlReader is configured to
    do so. They indicate that the XML declaration has been parsed.
*/
class StartDocument : public Node 
{
    public:
        /** @brief Creates an empty object.
        */
        StartDocument()
        : Node(Node::StartDocument)
        , _standalone(false)
        { }

        /** @brief Clears all content.
        */
        void clear()
        {
            _standalone = false;
        }

        /** @brief Returns true if the document is standalone.
        */
        bool isStandalone() const
        { return _standalone; }

        /** @brief Indicates that the document is standalone.
        */
        void setStandalone(bool alone)
        { _standalone = alone; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::StartDocument; }

    private:
        bool _standalone;
};

/** @brief Casts a generic node to a %StartDocument node.
*/
inline StartDocument* toStartDocument(Node* node)
{
    return nodeCast<StartDocument>(node);
}

/** @brief Casts a generic node to a %StartDocument node.
*/
inline const StartDocument* toStartDocument(const Node* node)
{
    return nodeCast<StartDocument>(node);
}

/** @brief Casts a generic node to a %StartDocument node.
*/
inline StartDocument& toStartDocument(Node& node)
{
    return nodeCast<StartDocument>(node);
}

/** @brief Casts a generic node to a %StartDocument node.
*/
inline const StartDocument& toStartDocument(const Node& node)
{
    return nodeCast<StartDocument>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_StartDocument_h
