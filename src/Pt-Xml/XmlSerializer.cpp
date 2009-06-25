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

XmlSerializationContext::XmlSerializationContext()
{
}


XmlSerializationContext::~XmlSerializationContext()
{
}


std::string XmlSerializationContext::beginUnlinkTarget(const std::string& name, const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
    {
        unsigned id = _idmap.size();
        _idmap[p] = id;
    }

    return convert<std::string>( _idmap[p] );
}


void XmlSerializationContext::finishUnlinkTarget()
{
}


void XmlSerializationContext::prepareUnlink(const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
    {
        unsigned id = _idmap.size();
        _idmap[p] = id;
    }

    unsigned lid = _idmap[p];
    _linkmap[lid] = p;
}


bool XmlSerializationContext::isUnlinked(const std::string& id)
{
	if( id.empty() )
		return false;

    unsigned n = convert<unsigned>( id );
    return _linkmap.find(n) != _linkmap.end();
}


std::string XmlSerializationContext::getUnlinkId(const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
        throw SerializationError("missing unlink information");

    return convert<std::string>( _idmap[p] );
}


XmlSerializer::XmlSerializer()
: _context(0)
{
	_context = &_xmlcontext;
}


XmlSerializer::XmlSerializer(std::ostream& os)
: XmlFormatter(os)
, _context(0)
{
	_context = &_xmlcontext;
}


XmlSerializer::XmlSerializer(XmlWriter* writer)
: XmlFormatter(writer)
, _context(0)
{
	_context = &_xmlcontext;
}


XmlSerializer::~XmlSerializer()
{
    this->finish();
}


/*
void XmlSerializer::attach(std::ostream& os)
{
    _formatter.attach(os);
}
*/
/*
void XmlSerializer::attach(XmlWriter& writer)
{
    _formatter.attach(writer);
}
*/
/*
void XmlSerializer::detach()
{
    _formatter.detach();
}
*/

void XmlSerializer::finish()
{
    std::vector<ISerializer*>::iterator it;

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->format(*this);
    }

    for(it = _heap.begin(); it != _heap.end(); ++it)
    {
        delete *it;
    }

    _heap.clear();
    _stack.clear();
}

/*
void XmlSerializer::flush()
{
    _formatter.flush();
}
*/

} // namespace Xml

} // namespace Pt
