/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Pt/Reflectable.h"


namespace Pt {

Reflectable::Reflectable(const std::string& name)
: _identiferName("IdentifierName", self(), name)
{
}


Reflectable::~Reflectable()
{
    for(PropertyMap::iterator it = _properties.begin(); it != _properties.end(); ++it) {
        delete it->second;
    }
    _properties.clear();

    MethodMap::iterator it;
    for( it = _methods.begin(); it != _methods.end(); ++it)
    {
        delete it->second;
    }

    _methods.clear();
}


Pt::Any Reflectable::property(const std::string& name)
{
    PropertyMap::iterator it = _properties.find(name);
    if(it == _properties.end())
        throw std::invalid_argument("No such property." + PT_SOURCEINFO);

    return it->second->value();
}


void Reflectable::setProperty(const std::string& name, const Pt::Any& value)
{
    PropertyMap::iterator it = _properties.find(name);
    if( it == _properties.end() ) {
        std::cerr << "Reflectable: Could not set '" << name << "' = " << value << std::endl;
        throw std::invalid_argument("No such property." + PT_SOURCEINFO);
        return;
    }

    it->second->setValue(value);
}


const ICallable& Reflectable::method(const std::string& name) const
{
    MethodMap::const_iterator it = _methods.find(name);
    if( it == _methods.end() )
        throw std::invalid_argument("No such method." + PT_SOURCEINFO);

    return *(it->second);
}


void Reflectable::call(const std::string& name, const Args& args)
{
    MethodMap::iterator it = _methods.find(name);
    if( it == _methods.end() )
        throw std::invalid_argument("No such method." + PT_SOURCEINFO);

    it->second->call(args);
}

} // namespace Pt
