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
#include "SerializationData.h"
#include "Pt/SerializationContext.h"
#include "Pt/SerializationError.h"
#include <Pt/SerializationInfo.h>

namespace Pt {


SerializationBinder::SerializationBinder()
{
}


SerializationBinder::~SerializationBinder()
{
}


bool SerializationBinder::beginUnlinkTarget(const std::string& name, const void* p)
{
    return false;
}


void SerializationBinder::finishUnlinkTarget()
{
}


void SerializationBinder::prepareUnlink(const void* p)
{
    throw SerializationError("missing unlink information");
}


bool SerializationBinder::isUnlinkTarget(const void* p)
{
    return false;
}


std::string SerializationBinder::getUnlinkId(const void* p)
{
    throw SerializationError("missing unlink information");
    return std::string();
}


void SerializationBinder::beginLinkTarget(const std::string& name, const std::string& id,
                                           void* obj, const std::type_info& fixupInfo)
{
}


void SerializationBinder::finishLinkTarget()
{
}


void SerializationBinder::prepareLink(const std::string& id, void* obj, FixupHandler)
{
}


void SerializationBinder::link()
{
}


SerializationContext::SerializationContext()
{
}


SerializationContext::~SerializationContext()
{
    std::vector<ValueNode*>::iterator it = _scalars.begin();
    for(; it != _scalars.end(); ++it)
    {
        //std::cerr << "destroy value" << std::endl;
        delete *it;
    }
    
    std::vector<SerializationInfo*>::iterator iter = _infos.begin();
    for(; iter != _infos.end(); ++iter)
    {
        //std::cerr << "destroy si" << std::endl;
        delete *iter;
    }
    
    //std::cerr << "cache destroyed" << std::endl;
}


SerializationInfo* SerializationContext::get()
{
    SerializationInfo* si = 0;

    if( _infos.empty() )
    {
        //std::cerr << "create si" << std::endl;
        si = new SerializationInfo(this);
        _infos.push_back(si);
    }
    else
    {
        si = _infos.back();
    }

    //std::cerr << "get si" << std::endl;
    _infos.pop_back();
    return si;
}


SerializationNode* SerializationContext::get(SerializationInfo::Category category)
{
    SerializationNode* node = 0;

    switch(category)
    {
        case SerializationInfo::Scalar:
        {
			if( _scalars.empty() )
			{
				node = new ValueNode();
				break;
		    }
				
            node = _scalars.back();
			_scalars.pop_back();
			break;
	    }

        case SerializationInfo::Reference:
            node = new ReferenceNode();
            break;

        case SerializationInfo::Sequence:
        case SerializationInfo::Struct:
            node = new ObjectNode(category);
            break;
            
        default:
            node = 0;
    }
    
    return node;
}


void SerializationContext::push(SerializationInfo* si)
{
    //std::cerr << "push si" << std::endl;

    si->clear();
    
    SerializationNode* node = si->releaseNode();
    
    if(node)
        this->push(node);
    
    _infos.push_back(si);
}


void SerializationContext::push(SerializationNode* node)
{
    if( node->category() == SerializationInfo::Scalar )
    {
        //std::cerr << "SerializationContext::push Value" << std::endl;
        ValueNode* scalar = static_cast<ValueNode*>(node);
        _scalars.push_back(scalar);
    }
    else if( node->category() == SerializationInfo::Struct || 
             node->category() == SerializationInfo::Sequence )
    {
        //std::cerr << "SerializationContext::push Struct" << std::endl;
        delete node;
    }
    else
    {
         //std::cerr << "SerializationContext::push unknown" << std::endl;
        delete node;
    }
}

} // namespace Pt
