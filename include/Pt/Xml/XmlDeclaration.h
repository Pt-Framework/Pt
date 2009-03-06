/*
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
#ifndef PTV_Xml_StartDocument_h
#define PTV_Xml_StartDocument_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

class PT_XML_API StartDocument : public Node {
    public:
        StartDocument()
        : Node(Node::StartDocument)
        , _version( L"1.0" )
        , _encoding( L"UTF-8" )
        , _standalone(false)
        { }

        ~StartDocument()
        {}

        StartDocument* clone() const
        { return new StartDocument(*this); }

        const String& version() const
        {
            return _version;
        }

        void setVersion(const String& version)
        {
            _version = version;
        }

        const String& encoding() const
        {
            return _encoding;
        }

        void setEncoding(const String& encoding)
        {
            _encoding = encoding;
        }

        bool standalone() const
        {
            return _standalone;
        }

        void setStandalone(bool standalone)
        {
            _standalone = standalone;
        }
    private:
        String _version;
        String _encoding;
        bool _standalone;
};

}

}

#endif
