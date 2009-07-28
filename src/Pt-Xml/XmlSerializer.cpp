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


bool XmlSerializationContext::beginSave(const std::string& name, const void* p)
{
    if( _idmap.find(p) == _idmap.end() )
    {
        unsigned id = _idmap.size();
        _idmap[p] = id;
        return true;
    }

    return false;
}


void XmlSerializationContext::finishSave()
{
}


void XmlSerializationContext::prepareId(const void* p)
{
    if(p == 0)
    {
        return;
    }

    if( _idmap.find(p) == _idmap.end() )
    {
        unsigned id = _idmap.size();
        _idmap[p] = id;
    }

    unsigned lid = _idmap[p];
    _linkmap[lid] = p;
}


bool XmlSerializationContext::hasId(const void* p)
{
    std::map<unsigned, const void*>::const_iterator it;

    for(it = _linkmap.begin(); it != _linkmap.end(); ++it)
    {
        if(it->second == p)
            return true;
    }

    return false;
}


std::string XmlSerializationContext::getId(const void* p)
{
    if(p == 0)
    {
        return "null";
    }

    if( _idmap.find(p) == _idmap.end() )
    {
        throw SerializationError("missing unlink information");
    }

    return convert<std::string>( _idmap[p] );
}


void XmlSerializationContext::reset()
{
    _linkmap.clear();
    _idmap.clear();
    
    _targets.clear();
    _pointers.clear();
}


void XmlSerializationContext::beginLoad(const std::string& name, const std::string& id,
                                        void* obj, const std::type_info& fixupInfo)
{
    if( id.empty() )
        return;

    //std::cerr << "beginLinkTarget: "  << obj << " " << fixupInfo.name() << " id: " << id << std::endl;
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    fi.fixup = 0;
    _targets[id] = fi;
}


void XmlSerializationContext::finishLoad()
{
}


void XmlSerializationContext::prepareFixup(const std::string& id, void* obj, FixupHandler fh)
{
    //std::cerr << "prepareLink: " << obj << " id " << id << std::endl;
    FixupInfo fi;
    fi.address = obj;
    fi.fixup = fh;
    _pointers.insert( std::pair<std::string, FixupInfo>(id, fi) );
}


void XmlSerializationContext::fixup()
{
    std::multimap<std::string, FixupInfo>::iterator it;
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        void* fixme = it->second.address;
        std::string id = it->first;

        if( id == "null" )
        {
            const std::type_info* targetType = &( typeid(void*) );
            it->second.fixup(fixme, 0, *targetType);
        }
        else if( _targets.find(id) != _targets.end() )
        {
            void* target = _targets[id].address;
            const std::type_info* targetType = _targets[id].type ;

            //std::cerr << "FIXING: " << fixme << " to " << target  << " by id " << id << std::endl;
            it->second.fixup(fixme, target, *targetType);
        }
        else
        {
            throw SerializationError("reference target not found");
        }
    }

    _targets.clear();
    _pointers.clear();
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
