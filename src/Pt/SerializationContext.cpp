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

namespace Pt {

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


void SerializationContext::push(SerializationInfo* si)
{
    //std::cerr << "push si" << std::endl;
    _infos.push_back(si);
}


ValueNode* SerializationContext::getScalarData()
{
    ValueNode* node = 0;

    if( _scalars.empty() )
    {
        //std::cerr << "create value" << std::endl;
        node = new ValueNode();
        _scalars.push_back(node);
    }
    else
    {
        node = _scalars.back();
    }

    //std::cerr << "get value" << std::endl;
    _scalars.pop_back();
    return node;
}


SerializationInfo::Node* SerializationContext::getObjectData()
{
    //std::cerr << "get object" << std::endl;
    return new ObjectNode();
}


void SerializationContext::push(SerializationInfo::Node* node)
{
    if( node->category() == SerializationInfo::Value )
    {
        ValueNode* scalar = static_cast<ValueNode*>(node);
        _scalars.push_back(scalar);
    }
    else if( node->category() == SerializationInfo::Object || 
             node->category() == SerializationInfo::Array )
    {
        static_cast<ObjectNode*>(node)->release(*this);
        delete node;
    }
    else
    {
        delete node;
    }
}


std::string SerializationContext::beginUnlinkTarget(const std::string& name, const void* p)
{
    return std::string();
}


void SerializationContext::finishUnlinkTarget()
{
}


void SerializationContext::prepareUnlink(const void* p)
{
    throw SerializationError("missing unlink information");
}


bool SerializationContext::isUnlinked(const std::string& id)
{
    return false;
}


std::string SerializationContext::getUnlinkId(const void* p)
{
    throw SerializationError("missing unlink information");
    return std::string();
}


void SerializationContext::beginLinkTarget(const std::string& name, const std::string& id,
                                           void* obj, const std::type_info& fixupInfo)
{
}


void SerializationContext::finishLinkTarget()
{
}


void SerializationContext::prepareLink(const std::string& id, void* obj, const std::type_info& fixupInfo)
{
}


void SerializationContext::link()
{
}


bool SerializationContext::checkLink(const std::type_info& from, const std::type_info& to)
{
    return from == to;
}

} // namespace Pt
