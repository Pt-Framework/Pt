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

#ifndef Pt_Xml_ProcessingInstruction_h
#define Pt_Xml_ProcessingInstruction_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

/** @brief A processing instruction of an XML document.
    
     Processing instruction are used to add instructions to an XML document
     to support specific XML processing software. The data of a processing
     instruction has no particular format and can contain plain text or 
     XML-like attribute/value-associations.
*/
class ProcessingInstruction : public Node 
{
    public:
        /** @brief Constructs an empty %ProcessingInstruction.
        */
        ProcessingInstruction()
        : Node(Node::ProcessingInstruction)
        { }

        /** @brief Clears all content.
        */
        void clear()
        { 
            _target.clear(); 
            _data.clear(); 
        }

        /** @brief Returns the target for the processing instruction.
        */
        const String& target() const
        { return _target; }

        /** @brief Returns the target for the processing instruction.
        */
        String& target()
        { return _target; }

        /** @brief Sets the target for the processing instruction.
        */
        void setTarget(const String& target)
        { _target = target; }

        /** @brief Returns the data for the processing instruction.
        */
        const String& data() const
        { return _data; }

        /** @brief Returns the data for the processing instruction.
        */
        String& data()
        { return _data; }

        /** @brief Sets the data for the processing instruction.
        */
        void setData(const String& data)
        { _data = data; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::ProcessingInstruction; }

    private:
        String _target;
        String _data;
};

/** @brief Casts a generic node to a %ProcessingInstruction node.
*/
inline ProcessingInstruction* toProcessingInstruction(Node* node)
{
    return nodeCast<ProcessingInstruction>(node);
}

/** @brief Casts a generic node to a %ProcessingInstruction node.
*/
inline const ProcessingInstruction* toProcessingInstruction(const Node* node)
{
    return nodeCast<ProcessingInstruction>(node);
}

/** @brief Casts a generic node to a %ProcessingInstruction node.
*/
inline ProcessingInstruction& toProcessingInstruction(Node& node)
{
    return nodeCast<ProcessingInstruction>(node);
}

/** @brief Casts a generic node to a %ProcessingInstruction node.
*/
inline const ProcessingInstruction& toProcessingInstruction(const Node& node)
{
    return nodeCast<ProcessingInstruction>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_ProcessingInstruction_h
