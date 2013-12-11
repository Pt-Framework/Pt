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

#ifndef Pt_Xml_XmlResolver_h
#define Pt_Xml_XmlResolver_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/String.h>
#include <Pt/TextCodec.h>

namespace Pt {

namespace Xml {

class ByteorderMark;
class InputSource;
class XmlDeclaration;

/** @brief Resolves external entities and DTDs.

    The %XmlResolver is used, for example by the XmlReader, to resolve
    external entites by their public or system ID to an input source. All
    input sources must be released with the same resolver by which they were
    created. Encoding strings can also be resolved to to text codecs by the 
    standardised encoding names.
*/
class XmlResolver
{
    public:
        /** @brief Destructor.
        */
        virtual ~XmlResolver()
        {}

        /** @brief Returns an input source for the given IDs.
        */
        InputSource* resolveInput(const Pt::String& publicId, 
                                  const Pt::String& systemId)
        {
            return onResolveInput(publicId, systemId);
        }

        /** @brief Releases an input source resolved by this resolver.
        */
        void releaseInput(InputSource* is)
        {
            onReleaseInput(is);
        }

        /** @brief Returns a text codec for an encoding string.

            The returned codec will be treated like a stream facet by the
            caller, i.e. it will be deleted if its refcount is 0.
        */
        TextCodec<Char, char>* resolveEncoding(const ByteorderMark& bom, 
                                               const XmlDeclaration& decl)
        {
            return onResolveEncoding(bom, decl);
        }

    protected:
        /** @brief Default constructor.
        */
        XmlResolver()
        {}

        /** @brief Returns an input source for the given IDs.
        */
        virtual InputSource* onResolveInput(const Pt::String& publicId, 
                                            const Pt::String& systemId) = 0;

        /** @brief Releases an input source resolved by this resolver.
        */
        virtual void onReleaseInput(InputSource* is) = 0;

        /** @brief Returns a text codec for an encoding string.

            The returned codec will be treated like a stream facet by the
            caller, i.e. it will be deleted if its refcount is 0.
        */
        virtual TextCodec<Char, char>* onResolveEncoding(const ByteorderMark& bom, 
                                                         const XmlDeclaration& decl)
        { return 0; }
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_XmlResolver_h
