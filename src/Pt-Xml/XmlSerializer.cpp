/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#include "Pt/Xml/XmlSerializer.h"

namespace Pt {

namespace Xml {

XmlSerializer::XmlSerializer()
{
}


XmlSerializer::XmlSerializer(std::ostream& os)
: _formatter(os)
{
}


XmlSerializer::XmlSerializer(XmlWriter* writer)
: _formatter(writer)
{
}


XmlSerializer::~XmlSerializer()
{
    this->finish();
}


void XmlSerializer::attach(std::ostream& os)
{
    _formatter.attach(os);
}


void XmlSerializer::attach(XmlWriter& writer)
{
    _formatter.attach(writer);
}


void XmlSerializer::detach()
{
    _formatter.detach();
}


void XmlSerializer::finish()
{
    std::vector<ISerializer*>::iterator it;

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->format(*this, _formatter);
    }

    for(it = _heap.begin(); it != _heap.end(); ++it)
    {
        delete *it;
    }

    _heap.clear();
    _stack.clear();
}


void XmlSerializer::flush()
{
    _formatter.flush();
}


void XmlSerializer::beginUnlinkTarget(const std::string& name, const void* p)
{
}


void XmlSerializer::finishUnlinkTarget()
{
}


void XmlSerializer::prepareUnlink(const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
    {
        unsigned id = _idmap.size();
        _idmap[p] = id;
    }
}


bool XmlSerializer::isUnlinked(const void* p)
{
    return _idmap.find(p) != _idmap.end();
}


std::string XmlSerializer::getUnlinkId(const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
        throw SerializationError("missing unlink information");

    return convert<std::string>( _idmap[p] );
}

} // namespace Xml

} // namespace Pt
