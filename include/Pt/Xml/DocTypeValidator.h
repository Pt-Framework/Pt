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

#ifndef Pt_Xml_DocTypeValidator_h
#define Pt_Xml_DocTypeValidator_h

#include <Pt/Xml/Api.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

class Node;
class DocTypeDefinition;

/** @brief Validates an XML document against a DTD.

    An XML document can be validated against its DTD by passing all XML
    nodes on to a %DocTypeValidator. Note, that it is also possible to 
    validate XML against a DTD from a different source.
*/
class PT_XML_API DocTypeValidator : private NonCopyable
{
    public:
        /** @brief Construct an empty DocType node.
        */
        explicit DocTypeValidator(DocTypeDefinition& dtd);

        /** @brief Destructor.
        */
        ~DocTypeValidator();

        /** @brief Clears the validator to start a new document.
        */
        void reset();

        /** @brief Returns true if the node is valid.
        */
        bool validate(Node& node);

    private:
        class DocTypeValidatorImpl* _impl;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_DocTypeValidator_h
