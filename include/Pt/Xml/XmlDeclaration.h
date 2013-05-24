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
#ifndef Pt_Xml_XmlDeclaration_h
#define Pt_Xml_XmlDeclaration_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

namespace Xml {

/** @brief XML declaration.
*/
class XmlDeclaration
{
    public:
        XmlDeclaration()
        : _standalone(false)
        {}
       
        void clear()
        {
            _version.clear();
            _encoding.clear();
            _standalone = false;
        }
        
        const std::string& version() const
        { return _version; }

        std::string& version()
        { return _version; }

        const std::string& encoding() const
        { return _encoding; }

        std::string& encoding()
        { return _encoding;}

        bool isStandalone() const
        { return _standalone; }
        
        void setStandalone(bool value)
        { _standalone = value; }

    private:
        std::string _version;
        std::string _encoding;
        bool _standalone;
};

} // namespace Xml

} // namespace Pt

#endif
