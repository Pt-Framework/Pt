/*
 * Copyright (C) 2008-2012 by Marc Boris Duerner
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
#ifndef Pt_Xml_XmlError_h
#define Pt_Xml_XmlError_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <stdexcept>

namespace Pt {

namespace Xml {

/** @brief Exception during XML processing.
*/
class PT_XML_API XmlError : public std::runtime_error
{
    public:
        /** @brief XML format errors.
        */
        explicit XmlError(const char* what);

        ~XmlError() throw()
        {}
};

/** @brief Indicates XML syntax errors.
*/
class PT_XML_API SyntaxError : public XmlError
{
    public:
        /** @brief Construct with message and line number.
        */
        SyntaxError(const char* what, unsigned line);

        ~SyntaxError() throw()
        {}

        unsigned line() const
        { return _line; }

    private:
        unsigned _line;
};


/** @brief Indicates XML syntax errors.
*/
class PT_XML_API NoSuchAttribute : public XmlError
{
    public:
        /** @brief Construct with attribute name.
        */
        NoSuchAttribute(const String& name);

        ~NoSuchAttribute() throw()
        {}

        const String& name() const
        { return _name; }

    private:
        String _name;
};

} // namespace Xml

} // namespace Pt

#endif
