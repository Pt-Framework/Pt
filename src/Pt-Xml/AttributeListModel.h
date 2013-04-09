/*
 * Copyright (C) 2013 by Marc Boris Duerner
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
#ifndef Pt_Xml_AttributeListDeclaration_h
#define Pt_Xml_AttributeListDeclaration_h

#include <Pt/Xml/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/String.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

class QName;
class AttributeModel;

class AttributeListModel : private NonCopyable
{
    public:
        // TODO: Iterator that is not pointer to pointer
        typedef AttributeModel** Iterator;
        typedef const AttributeModel* const* ConstIterator;
    
    public:
        AttributeListModel();

        ~AttributeListModel();

        void clear();

        // TODO: lowerBound() / insert() may give better performance 
        
        AttributeModel* findAttribute(const QName& name);

        void addAttribute(AttributeModel* decl);

        ConstIterator begin() const;

        ConstIterator end() const;

    private:
        std::vector<AttributeModel*> _decls;
};

} // namespace Xml

} // namespace Pt

#endif
