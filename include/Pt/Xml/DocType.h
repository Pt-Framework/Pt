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
#include <Pt/Xml/QName.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

class DocTypeDefinition;

/** @brief A %DocType node represents the begin of a DTD.

    %DocType nodes are only reported when the XmlReader is configured to
    do so. %DocType nodes might appear twice, before the internal DTD subset
    is parsed and before the external DTD subset is parsed. An EndDocType
    node indicates that a DTD subset is complete.
*/
class PT_XML_API DocType : public Node
                         , private Pt::NonCopyable
{
    public:
        /** @brief Construct an empty DocType node.
        */
        explicit DocType(DocTypeDefinition& dtd);

        /** @brief Destructor.
        */
        ~DocType();

        /** @brief Clears all content.
        */
        void clear();

        /** @brief Returns true if the external DTD subset begins.
        */
        bool isExternal() const;

        /** @brief Returns true if the internal DTD subset begins.
        */
        bool isInternal() const;

        /** @brief Indicates the begin of internal or external DTD subsets.
        */
        void setInternal(bool hasInternal);

        /** @brief Returns the documents root element name.
        */
        const QName& rootName() const;

        /** @brief Returns the public ID of the external subset.
        */
        const Pt::String& publicId() const
        { return _publicId; }

        /** @brief Sets the public ID of the external subset.
        */
        void setPublicId(const Pt::String& pubId)
        { _publicId = pubId; }

        /** @brief Returns the system ID of the external subset.
        */
        const Pt::String& systemId() const
        { return _systemId; }

        /** @brief Sets the system ID of the external subset.
        */
        void setSystemId(const Pt::String& sysId)
        { _systemId = sysId; }

        //! @internal
        inline static Node::Type nodeId()
        { return Node::DocType; }

    private:
        DocTypeDefinition* _dtd;
        Pt::String _publicId;
        Pt::String _systemId;
        Pt::varint_t _internal;
};

/** @brief Casts a generic node to a DocType node.
*/
inline DocType* toDocType(Node* node)
{
    return nodeCast<DocType>(node);
}

/** @brief Casts a generic node to a DocType node.
*/
inline const DocType* toDocType(const Node* node)
{
    return nodeCast<DocType>(node);
}

/** @brief Casts a generic node to a DocType node.
*/
inline DocType& toDocType(Node& node)
{
    return nodeCast<DocType>(node);
}

/** @brief Casts a generic node to a DocType node.
*/
inline const DocType& toDocType(const Node& node)
{
    return nodeCast<DocType>(node);
}

/** @brief An %EndDocType node represents the end of a DTD.

    %EndDocType nodes are only reported when the XmlReader is configured to
    do so. An %EndDocType node indicates that a DTD subset is complete. 
    %EndDocType nodes might appear twice, after the internal DTD subset
    was parsed and after the external DTD subset was parsed. 
*/
class PT_XML_API EndDocType : public Node
                            , private Pt::NonCopyable
{
    public:
        /** @brief Construct an empty DocType node.
        */
        EndDocType();

        /** @brief Destructor.
        */
        ~EndDocType();

        /** @brief Clears all content.
        */
        void clear();

        /** @brief Returns true if the external DTD subset begins.
        */
        bool isExternal() const;

        /** @brief Returns true if the internal DTD subset begins.
        */
        bool isInternal() const;

        /** @brief Indicates the end of internal or external DTD subsets.
        */
        void setInternal(bool value);

        //! @internal
        inline static Node::Type nodeId()
        { return Node::EndDocType; }

    private:
        Pt::varint_t _internal;
};

/** @brief Casts a generic node to a EndDocType node.
*/
inline EndDocType* toEndDocType(Node* node)
{
    return nodeCast<EndDocType>(node);
}

/** @brief Casts a generic node to a EndDocType node.
*/
inline const EndDocType* toEndDocType(const Node* node)
{
    return nodeCast<EndDocType>(node);
}

/** @brief Casts a generic node to a EndDocType node.
*/
inline EndDocType& toEndDocType(Node& node)
{
    return nodeCast<EndDocType>(node);
}

/** @brief Casts a generic node to a EndDocType node.
*/
inline const EndDocType& toEndDocType(const Node& node)
{
    return nodeCast<EndDocType>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_DocType_h
